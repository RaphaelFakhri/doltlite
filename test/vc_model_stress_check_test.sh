#!/usr/bin/env bash
# Replays hand-written model logs through `vc_model_stress_test --check`.
#
#   bash test/vc_model_stress_check_test.sh path/to/vc_model_stress_test path/to/doltlite

set -u

BIN="${1:-./vc_model_stress_test}"
DOLTLITE="${2:-./doltlite}"
for f in "$BIN" "$DOLTLITE"; do
  if [ ! -x "$f" ]; then echo "ERROR: $f not built" >&2; exit 2; fi
done

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fails=0

check(){
  name=$1 want=$2 dir=$3
  out=$("$BIN" --check "$dir" 1 2>&1)
  got=$(echo "$out" | grep -c '^VIOLATION')
  if [ "$got" = "$want" ]; then
    echo "ok $name"
  else
    echo "FAIL $name: expected $want violations, got $got"
    echo "$out" | sed 's/^/  /'
    fails=$((fails+1))
  fi
}

newdir(){
  d="$TMP/$1"
  mkdir -p "$d"
  "$DOLTLITE" "$d/db.db" "CREATE TABLE w(id INTEGER PRIMARY KEY, v TEXT);
SELECT dolt_commit('-Am','base'); SELECT dolt_branch('b1'); SELECT dolt_branch('b2');" >/dev/null
  echo "$d"
}

# A row committed in m_9, amended into m_13 while a reset --hard HEAD~1
# waited for the lock; the reset then dropped m_13 (its logged victim).
d=$(newdir amend_then_reset)
printf '%s\n' \
  $'I\t100000012\tmain\t1000\t1100' \
  $'C\tm_9\tmain\t2000\t2100\t1' \
  $'X\t7\tmain\t3000\t9000\treset_hard_1\tm_13' \
  $'X\t4\tmain\t4000\t4500\tamend\tm_9\tm_13' \
  $'C\tm_13\tmain\t4000\t4500\t1' > "$d/log.0"
check amend_then_reset_excused 0 "$d"

# Same history, but the reset dropped an unrelated commit: the row is lost.
d=$(newdir reset_other_victim)
printf '%s\n' \
  $'I\t100000012\tmain\t1000\t1100' \
  $'C\tm_9\tmain\t2000\t2100\t1' \
  $'X\t7\tmain\t3000\t9000\treset_hard_1\tm_other' \
  $'X\t4\tmain\t4000\t4500\tamend\tm_9\tm_13' \
  $'C\tm_13\tmain\t4000\t4500\t1' > "$d/log.0"
check reset_other_victim_lost 1 "$d"

if [ "$fails" -ne 0 ]; then
  echo "vc_model_stress_check: $fails failed"
  exit 1
fi
echo "vc_model_stress_check: all passed"
