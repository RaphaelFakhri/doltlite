#!/usr/bin/env bash
# Runs vc_model_stress_test over consecutive seeds until the time budget is
# spent. Seed s picks its own shape (processes, connections, busy timeout),
# so a failing seed reruns with the command printed below.
#
#   bash test/vc_model_stress.sh [path/to/vc_model_stress_test]
#
# VCMODEL_SECONDS  wall-clock budget (default 600)
# VCMODEL_SEED     first seed (default derived from the clock; printed)
# VCMODEL_SEEDS    whitespace-separated seeds, run once each within the budget
#                 (duplicates replay a seed under another process schedule)
# VCMODEL_CLUSTERED  0=rowid alias, 1=clustered PK (default varies by seed)
# VCMODEL_PAR      seeds run at once (default: CPU count)
# VCMODEL_OUT      where failing runs are kept (default ./vc-model-stress)

set -euo pipefail
set -f

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
check_seed(){
  case "$1" in
    ''|*[!0-9]*) echo "ERROR: seeds must be integers from 0 to 2147483647" >&2; exit 2 ;;
  esac
  if [ "${#1}" -gt 10 ] || [ "$((10#$1))" -gt 2147483647 ]; then
    echo "ERROR: seeds must be integers from 0 to 2147483647" >&2
    exit 2
  fi
}
check_seed "$FIRST"
if [ "${VCMODEL_SEEDS+x}" ]; then
  set -- $VCMODEL_SEEDS
  if [ "$#" -eq 0 ]; then
    echo "ERROR: VCMODEL_SEEDS must contain at least one seed" >&2
    exit 2
  fi
  for s do check_seed "$s"; done
fi
case "${VCMODEL_CLUSTERED-0}" in
  0|1) ;;
  *) echo "ERROR: VCMODEL_CLUSTERED must be 0 or 1" >&2; exit 2 ;;
esac
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
{
  printf 'revision=%s\nbinary=%s\n' "$(git rev-parse HEAD 2>/dev/null || echo unknown)" "$BIN"
  printf 'seconds=%s\nparallel=%s\nfirst=%s\n' "$SECONDS_BUDGET" "$PAR" "$FIRST"
  while IFS= read -r name; do
    case "$name" in VCMODEL_*) printf 'export %s=%q\n' "$name" "${!name}" ;; esac
  done < <(compgen -e | sort)
} > "$OUT/run-info.txt"
DEADLINE=$(( $(date +%s) + SECONDS_BUDGET ))
export BIN OUT DEADLINE

run_seed(){
  s=$((10#$1))
  attempt=$2
  [ "$(date +%s)" -lt "$DEADLINE" ] || exit 0
  np=$(( 2 + s % 3 ))
  nc=$(( 1 + (s / 3) % 4 ))
  bm=$(( s % 5 == 0 ? 2000 : 300 ))
  clustered=${VCMODEL_CLUSTERED:-$(( (s / 12) % 2 ))}
  d=$(mktemp -d "$OUT/s$s-r$attempt.XXXXXX")
  {
    echo '#!/usr/bin/env bash'
    echo 'for name in ${!VCMODEL_@}; do unset "$name"; done'
    while IFS= read -r name; do
      case "$name" in VCMODEL_*) printf 'export %s=%q\n' "$name" "${!name}" ;; esac
    done < <(compgen -e | sort)
    printf 'default_dir=%q\n' "$d-replay"
    printf 'VCMODEL_BUSY_MAX=%q VCMODEL_WEDGE_SEC=%q VCMODEL_CLUSTERED=%q ' \
      "$bm" "${VCMODEL_WEDGE_SEC:-600}" "$clustered"
    printf 'exec %q %q "${1:-$default_dir}" %q %q 3 120\n' "$BIN" "$s" "$np" "$nc"
  } > "$d.rerun.sh"
  if VCMODEL_BUSY_MAX=$bm VCMODEL_WEDGE_SEC=${VCMODEL_WEDGE_SEC:-600} \
     VCMODEL_CLUSTERED=$clustered \
     "$BIN" "$s" "$d" "$np" "$nc" 3 120 > "$d.out" 2>&1; then
    rm -rf "$d" "$d.out" "$d.rerun.sh"
    echo "ok $s attempt=$attempt processes=$np connections=$nc busy=$bm clustered=$clustered"
  else
    echo "FAIL $s $d.rerun.sh"
  fi
}
export -f run_seed

echo "vc_model_stress: seeds ${VCMODEL_SEEDS:-from $FIRST}, ${SECONDS_BUDGET}s, $PAR at a time"
{
  attempt=0
  if [ "${VCMODEL_SEEDS+x}" ]; then
    for s in $VCMODEL_SEEDS; do
      attempt=$((attempt + 1))
      echo "$s $attempt"
    done
  else
    s=$((10#$FIRST))
    while [ "$(date +%s)" -lt "$DEADLINE" ] && [ "$s" -le 2147483647 ]; do
      attempt=$((attempt + 1))
      echo "$s $attempt"
      s=$((s + 1))
    done
  fi
} | xargs -P "$PAR" -n 2 bash -eu -c 'run_seed "$0" "$1"' > "$OUT/summary.txt"

ok=$(grep -c '^ok ' "$OUT/summary.txt" || true)
if ! grep -Eq '^(ok|FAIL) ' "$OUT/summary.txt"; then
  echo "vc_model_stress: no seed ran" >&2
  exit 1
fi
if grep -q '^FAIL ' "$OUT/summary.txt"; then
  echo "vc_model_stress: $ok seeds clean, failures:"
  grep '^FAIL ' "$OUT/summary.txt" | while read -r _ seed replay; do
    printf -- '--- seed %s  (rerun: bash %q)\n' "$seed" "$replay"
    grep -m 5 VIOLATION "${replay%.rerun.sh}.out" | cut -c1-240 || true
  done
  exit 1
fi
if [ "${VCMODEL_SEEDS+x}" ] && [ "$(wc -l < "$OUT/summary.txt")" -ne "$#" ]; then
  echo "ERROR: time budget expired before all requested seeds ran" >&2
  exit 1
fi
echo "vc_model_stress: $ok seeds clean"
