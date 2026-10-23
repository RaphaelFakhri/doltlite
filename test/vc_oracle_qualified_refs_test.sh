#!/bin/bash
set -u
set -o pipefail

DOLTLITE="${1:-./doltlite}"
DOLT="${2:-dolt}"
TMPROOT=$(mktemp -d)
trap 'rm -rf "$TMPROOT"' EXIT
pass=0; fail=0
FAILED_NAMES=""
source "$(dirname "$0")/lib/vc_oracle_common.sh"

mkdir -p "$TMPROOT/dl" "$TMPROOT/dt" "$TMPROOT/dt-remote"
seed="CREATE TABLE t(id INTEGER PRIMARY KEY,v INT);
INSERT INTO t VALUES(1,10);
SELECT dolt_commit('-Am','base');
SELECT dolt_tag('base-ref');
UPDATE t SET v=20;
SELECT dolt_commit('-Am','tag-tip');
SELECT dolt_tag('b1');
SELECT dolt_tag('v1');
SELECT dolt_tag('tag-tip-ref');
SELECT dolt_tag('origin/main');
UPDATE t SET v=30;
SELECT dolt_commit('-Am','branch-tip');
SELECT dolt_branch('b1');
SELECT dolt_branch('branch-tip-ref');
SELECT dolt_remote('add','origin','@REMOTE@');
SELECT dolt_push('origin','main');
UPDATE t SET v=40;
SELECT dolt_commit('-Am','head-tip');
SELECT dolt_branch('heads/b1');
SELECT dolt_tag('tags/b1');
SELECT dolt_tag('heads/v1');"

if ! vc_oracle_run_doltlite_script "$TMPROOT/dl/db" "$TMPROOT/dl.seed.out" \
    "$TMPROOT/dl.seed.err" "${seed//@REMOTE@/file://$TMPROOT/dl-remote.db}"; then
  cat "$TMPROOT/dl.seed.err"
  exit 1
fi
dolt_seed=$(vc_oracle_translate_for_dolt "${seed//@REMOTE@/file://$TMPROOT/dt-remote}")
if ! vc_oracle_run_dolt_script "$TMPROOT/dt" "$TMPROOT/dt.seed.out" \
    "$TMPROOT/dt.seed.err" "$dolt_seed" -r csv; then
  cat "$TMPROOT/dt.seed.err"
  exit 1
fi

dl_rc=0; dt_rc=0
vc_oracle_run_doltlite_script "$TMPROOT/dl/db" "$TMPROOT/dl.ambiguous.out" \
  "$TMPROOT/dl.ambiguous.err" "SELECT dolt_hashof('origin/main');" \
  --expect-error || dl_rc=$?
(cd "$TMPROOT/dt" && "$DOLT" sql -q "SELECT dolt_hashof('origin/main');" \
  >"$TMPROOT/dt.ambiguous.out" 2>"$TMPROOT/dt.ambiguous.err") || dt_rc=$?
if vc_oracle_is_clean_error "$dl_rc" && vc_oracle_is_clean_error "$dt_rc" \
   && grep -q 'invalid ref spec' "$TMPROOT/dl.ambiguous.err" \
   && grep -qi ambiguous "$TMPROOT/dt.ambiguous.err"; then
  pass=$((pass+1))
else
  fail=$((fail+1))
  FAILED_NAMES="$FAILED_NAMES hashof_ambiguous_tracking_tag"
  echo 'FAIL: hashof_ambiguous_tracking_tag'
  cat "$TMPROOT/dl.ambiguous.err" "$TMPROOT/dt.ambiguous.err"
fi

oracle_ref() {
  local ref="$1" expected="$2" name="$3"
  local query="SELECT CONCAT('R|log|',message) FROM dolt_log('$ref')
WHERE message IN ('base','tag-tip','branch-tip','head-tip');
SELECT CONCAT('R|diff|',COUNT(*)) FROM dolt_diff_summary('$ref','$expected');"
  case "$ref" in
    b1|heads/b1|tags/b1|heads/b1^|tags/b1^1|origin/main|heads/v1)
      if [ "$name" != tag-origin/main ]; then
        query+="SELECT CONCAT('R|hash|',message) FROM dolt_log
WHERE commit_hash=dolt_hashof('$ref');"
      fi ;;
  esac
  local dl_query="SELECT CONCAT('R|rows|',v) FROM dolt_at_t('$ref'); $query"
  local dt_query="SELECT CONCAT('R|rows|',v) FROM t AS OF '$ref'; $query"
  local dl_rc=0 dt_rc=0 dl_out dt_out
  vc_oracle_run_doltlite_script "$TMPROOT/dl/db" "$TMPROOT/dl.out" \
    "$TMPROOT/dl.err" "$dl_query" || dl_rc=$?
  (
    cd "$TMPROOT/dt" || exit 1
    "$DOLT" sql -r csv -q "$dt_query" >"$TMPROOT/dt.out" 2>"$TMPROOT/dt.err"
  ) || dt_rc=$?
  if [ "$dl_rc" -ne 0 ] || [ "$dt_rc" -ne 0 ]; then
    fail=$((fail+1))
    FAILED_NAMES="$FAILED_NAMES $name"
    echo "FAIL: $name (doltlite=$dl_rc, dolt=$dt_rc)"
    cat "$TMPROOT/dl.err" "$TMPROOT/dt.err"
    return
  fi
  dl_out=$(tr -d '\r"' < "$TMPROOT/dl.out" | grep '^R|' | sort)
  dt_out=$(tr -d '\r"' < "$TMPROOT/dt.out" | grep '^R|' | sort)
  vc_oracle_assert_match "$name" "$dl_out" "$dt_out"
}

echo "=== Version Control Oracle Tests: qualified refs ==="
for phase in tag branch; do
  if [ "$phase" = branch ]; then
    vc_oracle_run_doltlite_script "$TMPROOT/dl/db" "$TMPROOT/dl.branch.out" \
      "$TMPROOT/dl.branch.err" "SELECT dolt_branch('origin/main');" || exit 1
    (cd "$TMPROOT/dt" && "$DOLT" sql -q "CALL dolt_branch('origin/main');" \
      >"$TMPROOT/dt.branch.out" 2>"$TMPROOT/dt.branch.err") || exit 1
  fi
  while read -r ref expected; do
    oracle_ref "$ref" "$expected" "$phase-$ref"
  done <<'REFS'
b1 branch-tip-ref
refs/heads/b1 branch-tip-ref
heads/b1 branch-tip-ref
refs/tags/b1 tag-tip-ref
tags/b1 tag-tip-ref
refs/tags/v1 tag-tip-ref
remotes/origin/main branch-tip-ref
refs/remotes/origin/main branch-tip-ref
refs/heads/heads/b1 main
refs/tags/tags/b1 main
heads/v1 main
refs/heads/b1~1 tag-tip-ref
heads/b1^ tag-tip-ref
refs/tags/b1~1 base-ref
tags/b1^1 base-ref
refs/remotes/origin/main~1 tag-tip-ref
remotes/origin/main^1~1 base-ref
REFS
  expected=tag-tip-ref
  [ "$phase" = tag ] || expected=main
  oracle_ref origin/main "$expected" "$phase-origin/main"
done

vc_oracle_finish
