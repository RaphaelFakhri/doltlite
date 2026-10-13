import http.client
import os
import struct
import subprocess
import sys
import urllib.parse


def main():
    engine, url, server_db = sys.argv[1:]
    engine = os.path.abspath(os.environ.get('DOLTLITE_SYSTEM', engine))
    remote = urllib.parse.urlsplit(url)
    failures = []

    def sql(statement, allow_error=False, db=None):
        result = subprocess.run([engine, '-bail', db or server_db, statement],
                                capture_output=True, text=True, timeout=60)
        output = (result.stdout + result.stderr).strip()
        if result.returncode and not allow_error:
            raise RuntimeError(output)
        return result.returncode, output

    def check(name, condition):
        print('  %s: %s' % ('PASS' if condition else 'FAIL', name), flush=True)
        if not condition:
            failures.append(name)

    def request(method, endpoint, body=b''):
        conn = http.client.HTTPConnection(remote.hostname, remote.port, timeout=60)
        try:
            conn.request(method, remote.path + '/' + endpoint, body)
            response = conn.getresponse()
            return response.status, response.read()
        finally:
            conn.close()

    def positions(blob):
        assert blob[0] == 7
        offset = 1

        def number():
            nonlocal offset
            value = struct.unpack_from('<I', blob, offset)[0]
            offset += 4
            return value

        def string():
            nonlocal offset
            size = number()
            value = blob[offset:offset + size]
            offset += size
            return value

        string()
        branches = offset
        count = number()
        for _ in range(count):
            string()
            offset += 40
        branches_end = offset
        tags = offset
        for _ in range(number()):
            string()
            offset += 20
            string()
            string()
            offset += 8
            string()
        tags_end = offset
        for _ in range(number()):
            string()
            string()
        for _ in range(number()):
            string()
            string()
            offset += 20
        sequences = offset
        for _ in range(number()):
            string()
            offset += 8
        assert offset == len(blob)
        return branches, branches_end, tags, tags_end, sequences

    def encoded(value):
        return struct.pack('<I', len(value)) + value

    def add_ref(blob, name, tag=False):
        branch_start, branch_end, tag_start, tag_end, _ = positions(blob)
        count_offset, end = (tag_start, tag_end) if tag else (branch_start, branch_end)
        first_name_size = struct.unpack_from('<I', blob, branch_start + 4)[0]
        commit_offset = branch_start + 8 + first_name_size
        commit = blob[commit_offset:commit_offset + 20]
        entry = encoded(name.encode()) + commit
        entry += (encoded(b'') + encoded(b'') + struct.pack('<q', 1) + encoded(b'')) if tag else bytes(20)
        candidate = bytearray(blob[:end] + entry + blob[end:])
        count = struct.unpack_from('<I', blob, count_offset)[0]
        struct.pack_into('<I', candidate, count_offset, count + 1)
        return bytes(candidate)

    def push(blob, candidate, scope, endpoint, force):
        prefix = struct.pack('<H', len(scope.encode())) + scope.encode() + bytes([force])
        if endpoint == 'refs-if':
            with open(server_db, 'rb') as database:
                data = database.read()
            assert struct.unpack_from('<I', data, 4)[0] == 12
            root = data.rfind(b'\x02' + data[:8])
            manifest = data[root + 1:root + 169] if root >= 0 else data[:168]
            prefix += manifest[104:124]
        return request('PUT', endpoint, prefix + candidate)

    sql("CREATE TABLE t(id INTEGER PRIMARY KEY AUTOINCREMENT,v TEXT); "
        "CREATE TABLE u(id INTEGER PRIMARY KEY,v TEXT); "
        "INSERT INTO t(v) VALUES('base'); INSERT INTO u VALUES(1,'base'); "
        "SELECT dolt_commit('-Am','seed');")
    invalid = ['HEAD', 'head', 'WORKING', 'STAGED', 'a..b', 'main~1',
               'x y', 'x.lock', '/x', 'x/', 'a//b', 'a/.b', 'a@{b',
               '0123456789012345678901234567890123456789']
    for endpoint in ('refs', 'refs-if'):
        for force in (0, 1):
            for tag in (False, True):
                for name in invalid:
                    status, blob = request('GET', 'refs')
                    assert status == 200
                    candidate = add_ref(blob, name, tag)
                    scope = ('tag:' if tag else '') + name
                    status, _ = push(blob, candidate, scope, endpoint, force)
                    check('invalid pushed ref name is rejected', 400 <= status < 500)
                    check('invalid name leaves refs unchanged', request('GET', 'refs') == (200, blob))

    for index, (endpoint, force) in enumerate((('refs', 0), ('refs', 1), ('refs-if', 0), ('refs-if', 1))):
        _, blob = request('GET', 'refs')
        name = 'counter_%d' % index
        candidate = add_ref(blob, name)
        *_, sequences = positions(candidate)
        payload = struct.pack('<I', 3)
        for table in (b't', b'u', b'ghost'):
            payload += encoded(table) + struct.pack('<q', 2**63 - 1)
        candidate = candidate[:sequences] + payload
        status, response = push(blob, candidate, name, endpoint, force)
        check('push ignores unproven sequence advances', status == 200)
        if status != 200:
            print(response.decode(errors='replace'), flush=True)
        result = sql("INSERT INTO t(v) VALUES('after'); INSERT INTO u(v) VALUES('after'); "
                     "SELECT max(id) FROM t; SELECT max(id) FROM u;", allow_error=True)
        check('forged counters do not exhaust server inserts', result == (0, '%d\n%d' % (index + 2, index + 2)))
    client = server_db + '.client.db'
    sql("SELECT dolt_clone('%s'); SELECT dolt_checkout('-b','history'); "
        "INSERT INTO t VALUES(500,'allocated'); DELETE FROM t WHERE id=500; "
        "INSERT INTO u VALUES(2000,'historical'); SELECT dolt_commit('-Am','allocate'); "
        "DELETE FROM u WHERE id=2000; SELECT dolt_commit('-am','delete'); "
        "SELECT dolt_push('origin','history');" % url, db=client)
    check('pushed data and history advance shared counters', sql(
        "INSERT INTO t(v) VALUES('legitimate'); INSERT INTO u(v) VALUES('legitimate'); "
        "SELECT id FROM t WHERE v='legitimate'; SELECT id FROM u WHERE v='legitimate';") == (0, '501\n2001'))
    check('server integrity after forged refs', sql('PRAGMA integrity_check;') == (0, 'ok'))
    return bool(failures)


if __name__ == '__main__':
    sys.exit(main())
