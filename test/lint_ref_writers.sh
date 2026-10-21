#!/usr/bin/env bash
# Every ref write goes through doltliteMutateRefs or an allowlisted
# publisher; a new direct writer skips the lock, refresh, and expected-tip
# checks those paths hold.
set -u
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

if [ "${1:-}" = "--selftest" ]; then
  tmp=$(mktemp -d)
  trap 'rm -rf "$tmp"' EXIT
  mkdir -p "$tmp/src"
  cat > "$tmp/src/doltlite_ok.c" <<'C'
static int helper(ChunkStore *cs){
  return chunkStoreAddBranch(cs, "b", 0);
}
static int mutateOk(sqlite3 *db, ChunkStore *cs, void *pArg){
  return helper(cs);
}
int okEntry(sqlite3 *db){
  /* chunkStoreUpdateBranch(cs) in a comment is not a call. */
  const char *z = "chunkStoreDeleteBranch(cs)";
  return doltliteMutateRefs(db,
                            mutateOk, 0);
}
int listed(ChunkStore *cs){
  return chunkStoreSerializeRefs(cs);
}
C
  cat > "$tmp/src/chunk_store_x.c" <<'C'
int chunkStoreUpdateBranch(ChunkStore *cs){ return chunkStoreSerializeRefs(cs); }
C
  printf 'src/doltlite_ok.c:listed\n' > "$tmp/allow.txt"
  fail=0
  if ! bash "$0" "$tmp" "$tmp/allow.txt"; then
    echo "selftest: clean tree was rejected"
    fail=1
  fi
  cat > "$tmp/src/doltlite_bad.c" <<'C'
static int sharedHelper(ChunkStore *cs){
  return chunkStoreUpdateBranch(cs, "main", 0);
}
static int mutateBad(sqlite3 *db, ChunkStore *cs, void *pArg){
  return sharedHelper(cs);
}
int badEntry(sqlite3 *db, ChunkStore *cs){
  doltliteMutateRefs(db, mutateBad, 0);
  return sharedHelper(cs);
}
C
  out=$(bash "$0" "$tmp" "$tmp/allow.txt" 2>&1)
  if [ $? -eq 0 ] || ! printf '%s\n' "$out" | grep -q 'sharedHelper'; then
    echo "selftest: helper reachable outside a callback was accepted"
    fail=1
  fi
  rm -f "$tmp/src/doltlite_bad.c"
  printf 'src/doltlite_ok.c:listed\nsrc/doltlite_ok.c:gone\n' > "$tmp/allow.txt"
  out=$(bash "$0" "$tmp" "$tmp/allow.txt" 2>&1)
  if [ $? -eq 0 ] || ! printf '%s\n' "$out" | grep -q 'gone'; then
    echo "selftest: stale allowlist entry was accepted"
    fail=1
  fi
  if [ "$fail" -ne 0 ]; then
    exit 1
  fi
  echo "lint_ref_writers: selftest passed"
  exit 0
fi

root="${1:-$REPO_ROOT}"
allow="${2:-$REPO_ROOT/test/lint_ref_writers_allow.txt}"
python3 - "$root" "$allow" <<'PY'
import os, re, sys, glob

PRIMS = ("chunkStoreUpdateBranch", "chunkStoreAddBranch", "chunkStoreDeleteBranch",
         "chunkStoreSetDefaultBranch", "chunkStoreAddTagFull", "chunkStoreDeleteTag",
         "chunkStoreSetBranchWorkingSet", "chunkStoreSerializeRefs",
         "chunkStoreInstallRefsBlob")
RUNNERS = ("doltliteMutateRefs", "doltliteMutateRefsExpected")
TOKEN_RE = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"'
                      r"|'(?:\\.|[^'\\\n])*'", re.S)
DEF_RE = re.compile(r"^(static\s+)?[A-Za-z_][^;=()]*?\b([A-Za-z_]\w*)\s*\(")
CALL_RE = re.compile(r"\b([A-Za-z_]\w*)\s*\(")

def blank(text):
    return TOKEN_RE.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), text)

def parse(path):
    text = blank(open(path, errors="replace").read())
    funcs = {}
    depth = 0
    pending = None
    cur = None
    for ln, line in enumerate(text.split("\n"), 1):
        if depth == 0:
            m = DEF_RE.match(line)
            if m and m.group(2) not in ("if", "for", "while", "switch", "return"):
                pending = (m.group(2), bool(m.group(1)), ln)
        for ch in line:
            if ch == "{":
                if depth == 0 and pending:
                    cur = {"name": pending[0], "static": pending[1],
                           "line": pending[2], "body": []}
                    funcs.setdefault(pending[0], cur)
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0:
                    cur = None
                    pending = None
        if cur is not None and depth > 0:
            cur["body"].append((ln, line))
    return funcs

def runner_args(body_text):
    out = set()
    for m in re.finditer(r"\b(?:%s)\s*\(" % "|".join(RUNNERS), body_text):
        i = m.end()
        nest = 1
        j = i
        while j < len(body_text) and nest:
            if body_text[j] == "(":
                nest += 1
            elif body_text[j] == ")":
                nest -= 1
            j += 1
        out.update(re.findall(r"\b[A-Za-z_]\w*\b", body_text[i:j - 1]))
    return out

def scan(root, allowed):
    files = [p for p in sorted(glob.glob(os.path.join(root, "src", "*.c")))
             if not os.path.basename(p).startswith("chunk_")]
    defs = {}
    for path in files:
        rel = os.path.relpath(path, root)
        for name, f in parse(path).items():
            f["file"] = rel
            f["text"] = "\n".join(l for _, l in f["body"])
            f["calls"] = set(CALL_RE.findall(f["text"]))
            defs[(rel, name)] = f

    ok = {k for k in defs if "%s:%s" % k in allowed}
    for f in defs.values():
        for arg in runner_args(f["text"]):
            ok.update(k for k in defs if k[1] == arg)
    changed = True
    while changed:
        changed = False
        for key, f in defs.items():
            if key in ok or not f["static"]:
                continue
            callers = [k for k, g in defs.items()
                       if k[0] == key[0] and k != key and key[1] in g["calls"]]
            if callers and all(c in ok for c in callers):
                ok.add(key)
                changed = True

    hits = []
    used = set()
    prim_re = re.compile(r"\b(%s)\s*\(" % "|".join(PRIMS))
    for key, f in sorted(defs.items()):
        for ln, line in f["body"]:
            for m in prim_re.finditer(line):
                if key in ok:
                    used.add("%s:%s" % key)
                else:
                    hits.append("%s:%d: %s() calls %s" % (key[0], ln, key[1], m.group(1)))
    stale = sorted(a for a in allowed if a not in used)
    return hits, stale

root, allow_path = sys.argv[1], sys.argv[2]
allowed = set()
for line in open(allow_path):
    line = line.strip()
    if line and not line.startswith("#"):
        allowed.add(line)
hits, stale = scan(root, allowed)
if hits or stale:
    print("lint_ref_writers: ref writes outside doltliteMutateRefs")
    for hit in hits:
        print("  " + hit)
    for entry in stale:
        print("  stale allowlist entry: " + entry)
    print("  route the write through a doltliteMutateRefs callback or "
          "doltliteCompareAndAdvanceBranch, or list a new publisher in "
          "test/lint_ref_writers_allow.txt")
    sys.exit(1)
print("lint_ref_writers: ok")
PY
