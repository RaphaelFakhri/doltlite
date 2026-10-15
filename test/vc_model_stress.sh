#!/usr/bin/env bash
# Runs vc_model_stress_test over consecutive seeds until the time budget is
# spent. Seed s picks its own shape (processes, connections, busy timeout),
# so a failing seed reruns with the command printed below.
#
#   bash test/vc_model_stress.sh [path/to/vc_model_stress_test]
#
# VCMODEL_SECONDS  wall-clock budget (default 600)
# VCMODEL_SEED     first seed (default derived from the clock; printed)
# VCMODEL_PAR      seeds run at once (default: CPU count)
# VCMODEL_OUT      where failing runs are kept (default ./vc-model-stress)

set -u

BIN="${1:-./vc_model_stress_test}"
case "$BIN" in /*) ;; *) BIN="$PWD/$BIN" ;; esac
SECONDS_BUDGET="${VCMODEL_SECONDS-600}"
FIRST="${VCMODEL_SEED:-$(( $(date +%s) % 1000000 * 10 ))}"
PAR="${VCMODEL_PAR-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}"
OUT="${VCMODEL_OUT:-$PWD/vc-model-stress}"

case "$SECONDS_BUDGET" in
  ''|*[!0-9]*|0) echo "ERROR: VCMODEL_SECONDS must be a positive integer" >&2; exit 2 ;;
esac
case "$PAR" in
  ''|*[!0-9]*|0) echo "ERROR: VCMODEL_PAR must be a positive integer" >&2; exit 2 ;;
esac
if [ ! -x "$BIN" ]; then
  echo "ERROR: $BIN not built (make vc_model_stress_test)" >&2
  exit 2
fi
mkdir -p "$OUT"
DEADLINE=$(( $(date +%s) + SECONDS_BUDGET ))
export BIN OUT DEADLINE

run_seed(){
  s=$1
  [ "$(date +%s)" -lt "$DEADLINE" ] || exit 0
  np=$(( 2 + s % 3 ))
  nc=$(( 1 + (s / 3) % 4 ))
  bm=$(( s % 5 == 0 ? 2000 : 300 ))
  d="$OUT/s$s"
  rm -rf "$d"
  if VCMODEL_BUSY_MAX=$bm VCMODEL_WEDGE_SEC=${VCMODEL_WEDGE_SEC:-600} \
     "$BIN" "$s" "$d" "$np" "$nc" 3 120 > "$d.out" 2>&1; then
    rm -rf "$d" "$d.out"
    echo "ok $s"
  else
    echo "FAIL $s VCMODEL_BUSY_MAX=$bm $BIN $s DIR $np $nc 3 120"
  fi
}
export -f run_seed

echo "vc_model_stress: first seed $FIRST, ${SECONDS_BUDGET}s, $PAR at a time"
s=$FIRST
while [ "$(date +%s)" -lt "$DEADLINE" ]; do
  echo "$s"
  s=$((s + 1))
done | xargs -P "$PAR" -n 1 bash -c 'run_seed "$0"' > "$OUT/summary.txt"

ok=$(grep -c '^ok ' "$OUT/summary.txt")
if ! grep -Eq '^(ok|FAIL) ' "$OUT/summary.txt"; then
  echo "vc_model_stress: no seed ran" >&2
  exit 1
fi
if grep -q '^FAIL ' "$OUT/summary.txt"; then
  echo "vc_model_stress: $ok seeds clean, failures:"
  grep '^FAIL ' "$OUT/summary.txt" | while read -r _ seed cmd; do
    echo "--- seed $seed  (rerun: $cmd)"
    grep -m 5 VIOLATION "$OUT/s$seed.out" | cut -c1-240
  done
  exit 1
fi
echo "vc_model_stress: $ok seeds clean"
