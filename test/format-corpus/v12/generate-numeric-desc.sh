#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "$0")" && pwd)"
binary="${1:?pass a v0.50.14 doltlite binary}"
output="${2:?pass a new output database path}"
if [[ -e "$output" ]]; then
  echo "Output already exists: $output" >&2
  exit 1
fi
"$binary" -bail "$output" < "$script_dir/numeric-desc.sql" >/dev/null
