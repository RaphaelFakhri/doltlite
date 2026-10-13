import http.client
import http.server
import os
import subprocess
import sys
import tempfile
import threading
import urllib.parse


def main():
    engine, url, server_db, scratch = sys.argv[1:5]
    engine = os.path.abspath(os.environ.get("DOLTLITE_SYSTEM", engine))
    fixture = os.path.abspath(sys.argv[5]) if len(sys.argv) > 5 else None
    upstream = urllib.parse.urlsplit(url)
    state = {"reads": 0, "delete": None, "deleted": False,
             "fault": None, "faulted": False, "errors": []}
    failures = []

    def sql(db, statement, allow_error=False, client=False):
        command = [engine, "-bail", db, statement]
        if fixture and client:
            command = [fixture, driver, db, statement]
        result = subprocess.run(command, text=True, capture_output=True, timeout=60)
        output = (result.stdout + result.stderr).strip()
        if result.returncode and not allow_error:
            raise RuntimeError(output)
        return result.returncode, output

    def check(name, condition):
        print("  %s: %s" % ("PASS" if condition else "FAIL", name), flush=True)
        if not condition:
            failures.append(name)

    class Proxy(http.server.BaseHTTPRequestHandler):
        def log_message(self, *args):
            pass

        def forward(self):
            conn = http.client.HTTPConnection(upstream.hostname, upstream.port, timeout=60)
            try:
                refs = self.command == "GET" and self.path.endswith("/refs")
                if refs:
                    state["reads"] += 1
                if state["fault"] == "refs" and refs and state["reads"] == 2:
                    state["faulted"] = True
                    self.send_error(404)
                    return
                if state["fault"] == "chunks" and (
                    self.path.endswith("/get-chunks") or "/chunk/" in self.path
                ):
                    state["faulted"] = True
                    self.send_error(404)
                    return
                body = self.rfile.read(int(self.headers.get("Content-Length", 0)))
                conn.request(self.command, self.path, body)
                response = conn.getresponse()
                data = response.read()
                if refs and state["reads"] == 1 and state["delete"]:
                    sql(state["delete"], "SELECT dolt_branch('-D','zzz');")
                    state["deleted"] = True
                if state["fault"] == "corrupt" and refs and state["reads"] == 2:
                    state["faulted"] = True
                    data = b"invalid refs"
                self.send_response(response.status)
                self.send_header("Content-Length", str(len(data)))
                self.end_headers()
                self.wfile.write(data)
            except Exception as error:
                state["errors"].append(str(error))
                self.send_error(500)
            finally:
                conn.close()

        do_GET = forward
        do_POST = forward

    proxy = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Proxy)
    thread = threading.Thread(target=proxy.serve_forever, daemon=True)
    thread.start()
    try:
        with tempfile.TemporaryDirectory(dir=scratch) as tmp:
            driver = os.path.join(tmp, "sql.tcl")
            with open(driver, "w") as out:
                out.write("""sqlite3 db [lindex $argv 0]
if {[catch {
  db eval [lindex $argv 1] row {
    set values {}
    foreach column $row(*) {lappend values $row($column)}
    puts [join $values |]
  }
} message]} {
  puts stderr $message
  db close
  exit 1
}
db close
""")
            cases = [
                ("fetch-new", "SELECT dolt_fetch('origin');", False, None),
                ("fetch-existing", "SELECT dolt_fetch('origin');", True, None),
                ("fetch-prune", "SELECT dolt_fetch('--prune','origin');", True, None),
                ("fetch-glob", "SELECT dolt_fetch('origin','refs/heads/*:refs/remotes/origin/mapped/*');", False, None),
                ("pull", "SELECT dolt_pull();", True, None),
                ("pull-origin", "SELECT dolt_pull('origin');", True, None),
                ("fetch-exact", "SELECT dolt_fetch('origin','zzz');", True, None),
                ("fetch-mapped", "SELECT dolt_fetch('origin','zzz:refs/remotes/origin/mapped');", True, None),
                ("pull-required", "SELECT dolt_pull();", True, None),
                ("missing-refs", "SELECT dolt_fetch('origin');", False, "refs"),
                ("missing-chunks", "SELECT dolt_fetch('origin');", False, "chunks"),
                ("corrupt-refs", "SELECT dolt_fetch('origin');", False, "corrupt"),
            ]
            for name, statement, cloned, fault in cases:
                basename = os.path.splitext(os.path.basename(server_db))[0] + "_" + name + ".db"
                remote = os.path.join(os.path.dirname(server_db), basename)
                client = os.path.join(tmp, name + ".db")
                remote_url = "http://127.0.0.1:%d/%s" % (proxy.server_port, basename)
                state.update(reads=0, delete=None, deleted=False,
                             fault=None, faulted=False, errors=[])
                sql(remote, "CREATE TABLE t(id INTEGER PRIMARY KEY); INSERT INTO t VALUES(1); "
                    "SELECT dolt_commit('-Am','seed'); SELECT dolt_branch('zzz'); "
                    "SELECT dolt_branch('feature');")
                if cloned:
                    sql(client, "SELECT dolt_clone('%s');" % remote_url, client=True)
                else:
                    sql(client, "SELECT dolt_remote('add','origin','%s');" % remote_url, client=True)
                if name == "pull-required":
                    sql(client, "SELECT dolt_checkout('zzz');", client=True)
                    client += "/zzz"
                before = sql(client, "SELECT dolt_hashof('HEAD');", client=True)[1]
                deleted_head = sql(remote, "SELECT dolt_hashof('zzz');")[1]
                sql(remote, "INSERT INTO t VALUES(2); SELECT dolt_commit('-am','main update'); "
                    "SELECT dolt_checkout('feature'); INSERT INTO t VALUES(3); "
                    "SELECT dolt_commit('-am','feature update'); SELECT dolt_checkout('main');")
                main_head = sql(remote, "SELECT dolt_hashof('main');")[1]
                feature_head = sql(remote, "SELECT dolt_hashof('feature');")[1]
                state.update(reads=0, delete=None if fault else remote, fault=fault)
                result = sql(client, statement, allow_error=True, client=True)
                state.update(delete=None, fault=None)
                check(name + " proxy completed", not state["errors"] and
                      (state["faulted"] if fault else state["deleted"]))
                if fault or name in ("fetch-exact", "fetch-mapped", "pull-required"):
                    check(name + " reports failure", result[0] != 0)
                    check(name + " keeps local HEAD", sql(client,
                        "SELECT dolt_hashof('HEAD');", client=True)[1] == before)
                    continue
                check(name + " succeeds after remote deletion", result == (0, "0"))
                if result != (0, "0"):
                    print(result, flush=True)
                prefix = "remotes/origin/mapped/" if name == "fetch-glob" else "remotes/origin/"
                for branch, head in (("main", main_head), ("feature", feature_head)):
                    check(name + " installs " + branch, sql(client,
                        "SELECT hash FROM dolt_remote_branches WHERE name='%s%s';" % (prefix, branch),
                        client=True)[1] == head)
                keep = cloned and name != "fetch-prune"
                check(name + " preserves or prunes deleted tracking", sql(client,
                    "SELECT count(*) FROM dolt_remote_branches WHERE name='%szzz';" % prefix,
                    client=True)[1] == str(int(keep)))
                if keep:
                    check(name + " keeps deleted tracking tip", sql(client,
                        "SELECT hash FROM dolt_remote_branches WHERE name='%szzz';" % prefix,
                        client=True)[1] == deleted_head)
                if name.startswith("pull"):
                    check(name + " merges main", sql(client,
                        "SELECT group_concat(id) FROM (SELECT id FROM t ORDER BY id);",
                        client=True)[1] == "1,2")
                check(name + " integrity", sql(client, "PRAGMA integrity_check;", client=True)[1] == "ok")
    finally:
        proxy.shutdown()
        proxy.server_close()
        thread.join()
    print("Fetch deletion races: %d failures" % len(failures), flush=True)
    return bool(failures)


if __name__ == "__main__":
    sys.exit(main())
