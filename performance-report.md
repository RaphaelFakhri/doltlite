# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-14 11:12 UTC
>
> Commit: [`59fcaf6245444eb3221d4768f3c086aea6f960c5`](https://github.com/dolthub/doltlite/commit/59fcaf6245444eb3221d4768f3c086aea6f960c5)
>
> Runner: ubuntu24 20261004.327.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/38543710454)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 9.32s | 10.70s | 1.1× | 1.3% | **PASS** |
| Writes | 1.96s | 3.00s | 1.5× | 1.2% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.34s | 11.05s | 1.1× | 1.3% | **PASS** |
| Writes | 4.40s | 4.36s | 1.0× | 5.4% | **PASS** |
| Autocommit writes | 987.03ms | 3.36s | 3.4× | 17.3% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.42s | 2.76s | 1.1× | 1.1% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.14s | 2.39s | 1.1× | 1.1% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.11s | 2.40s | 1.1× | 1.3% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.64s | 3.15s | 1.2× | 2.3% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 444.26ms | 721.03ms | 1.6× | 1.2% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 458.45ms | 675.08ms | 1.5× | 0.9% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 465.97ms | 688.44ms | 1.5× | 1.5% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 591.50ms | 914.07ms | 1.5× | 2.0% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.65s | 2.83s | 1.1× | 1.8% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.38s | 2.44s | 1.0× | 0.8% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.42s | 2.56s | 1.1× | 1.0% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.88s | 3.21s | 1.1× | 2.0% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 597.63ms | 790.20ms | 1.3× | 1.6% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 1.44s | 1.25s | 0.9× | 18.8% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 1.60s | 1.33s | 0.8× | 31.5% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 769.34ms | 995.85ms | 1.3× | 1.9% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.49s | 2.83s | 1.1× | 1.4% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.24s | 2.45s | 1.1× | 0.9% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.26s | 2.48s | 1.1× | 1.4% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.82s | 3.23s | 1.1× | 2.2% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 203.68ms | 653.90ms | 3.2× | 7.6% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 229.07ms | 822.33ms | 3.6× | 42.2% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 355.48ms | 1.26s | 3.5× | 63.3% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 198.79ms | 632.59ms | 3.2× | 5.7% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 23.42ms | 31.50ms | 1.3× | 1.6% | PASS |
| mem_reads | `oltp_range_select` | 10.10ms | 12.44ms | 1.2× | 1.7% | PASS |
| mem_reads | `oltp_sum_range` | 9.46ms | 11.97ms | 1.3× | 1.2% | PASS |
| mem_reads | `oltp_order_range` | 2.59ms | 2.97ms | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_distinct_range` | 3.64ms | 4.08ms | 1.1× | 0.8% | PASS |
| mem_reads | `oltp_index_scan` | 3.87ms | 5.68ms | 1.5× | 1.6% | PASS |
| mem_reads | `select_random_points` | 9.38ms | 11.66ms | 1.2× | 1.3% | PASS |
| mem_reads | `select_random_ranges` | 4.49ms | 5.36ms | 1.2× | 2.0% | PASS |
| mem_reads | `covering_index_scan` | 7.54ms | 10.35ms | 1.4× | 1.1% | PASS |
| mem_reads | `groupby_scan` | 29.10ms | 32.61ms | 1.1× | 0.6% | PASS |
| mem_reads | `index_join` | 5.67ms | 8.60ms | 1.5× | 1.0% | PASS |
| mem_reads | `index_join_scan` | 3.14ms | 5.77ms | 1.8× | 1.5% | PASS |
| mem_reads | `types_table_scan` | 1.03s | 1.19s | 1.2× | 1.0% | PASS |
| mem_reads | `table_scan` | 1.18s | 1.31s | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_read_only` | 101.64ms | 124.51ms | 1.2× | 0.9% | PASS |
| mem_writes | `oltp_bulk_insert` | 178.88ms | 275.14ms | 1.5× | 1.2% | PASS |
| mem_writes | `oltp_insert` | 15.12ms | 27.37ms | 1.8× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 50.71ms | 92.83ms | 1.8× | 1.6% | PASS |
| mem_writes | `oltp_update_non_index` | 35.03ms | 57.95ms | 1.7× | 1.6% | PASS |
| mem_writes | `oltp_delete_insert` | 44.99ms | 73.51ms | 1.6× | 0.8% | PASS |
| mem_writes | `oltp_write_only` | 22.07ms | 44.37ms | 2.0× | 1.2% | PASS |
| mem_writes | `types_delete_insert` | 24.64ms | 37.96ms | 1.5× | 1.1% | PASS |
| mem_writes | `oltp_read_write` | 72.81ms | 111.89ms | 1.5× | 1.2% | PASS |
| file_reads | `oltp_point_select` | 94.44ms | 51.28ms | 0.5× | 0.9% | PASS |
| file_reads | `oltp_range_select` | 17.42ms | 14.53ms | 0.8× | 1.8% | PASS |
| file_reads | `oltp_sum_range` | 16.60ms | 14.02ms | 0.8× | 1.5% | PASS |
| file_reads | `oltp_order_range` | 3.40ms | 3.24ms | 1.0× | 1.8% | PASS |
| file_reads | `oltp_distinct_range` | 4.48ms | 4.36ms | 1.0× | 1.8% | PASS |
| file_reads | `oltp_index_scan` | 11.17ms | 7.77ms | 0.7× | 2.1% | PASS |
| file_reads | `select_random_points` | 17.08ms | 13.96ms | 0.8× | 2.3% | PASS |
| file_reads | `select_random_ranges` | 11.78ms | 7.47ms | 0.6× | 1.9% | PASS |
| file_reads | `covering_index_scan` | 15.02ms | 12.72ms | 0.8× | 1.4% | PASS |
| file_reads | `groupby_scan` | 30.20ms | 32.98ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 9.84ms | 10.50ms | 1.1× | 1.8% | PASS |
| file_reads | `index_join_scan` | 4.16ms | 6.21ms | 1.5× | 2.1% | PASS |
| file_reads | `types_table_scan` | 1.04s | 1.20s | 1.2× | 0.8% | PASS |
| file_reads | `table_scan` | 1.18s | 1.31s | 1.1× | 1.3% | PASS |
| file_reads | `oltp_read_only` | 200.63ms | 151.81ms | 0.8× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 192.12ms | 287.29ms | 1.5× | 1.0% | PASS |
| file_writes | `oltp_insert` | 21.77ms | 30.45ms | 1.4× | 1.6% | PASS |
| file_writes | `oltp_update_index` | 75.35ms | 101.99ms | 1.4× | 1.5% | PASS |
| file_writes | `oltp_update_non_index` | 56.51ms | 66.29ms | 1.2× | 1.5% | PASS |
| file_writes | `oltp_delete_insert` | 67.12ms | 84.41ms | 1.3× | 2.1% | PASS |
| file_writes | `oltp_write_only` | 45.55ms | 53.71ms | 1.2× | 2.2% | PASS |
| file_writes | `types_delete_insert` | 40.28ms | 44.06ms | 1.1× | 1.6% | PASS |
| file_writes | `oltp_read_write` | 98.93ms | 121.99ms | 1.2× | 1.6% | PASS |
| ac_reads | `oltp_point_select` | 48.29ms | 51.89ms | 1.1× | 1.1% | PASS |
| ac_reads | `oltp_range_select` | 13.83ms | 14.92ms | 1.1× | 1.5% | PASS |
| ac_reads | `oltp_sum_range` | 12.70ms | 14.44ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 3.00ms | 3.26ms | 1.1× | 1.9% | PASS |
| ac_reads | `oltp_distinct_range` | 3.93ms | 4.34ms | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_index_scan` | 6.37ms | 7.78ms | 1.2× | 1.4% | PASS |
| ac_reads | `select_random_points` | 12.31ms | 13.95ms | 1.1× | 1.8% | PASS |
| ac_reads | `select_random_ranges` | 7.00ms | 7.48ms | 1.1× | 1.7% | PASS |
| ac_reads | `covering_index_scan` | 10.09ms | 12.78ms | 1.3× | 1.5% | PASS |
| ac_reads | `groupby_scan` | 29.41ms | 33.01ms | 1.1× | 0.7% | PASS |
| ac_reads | `index_join` | 7.42ms | 10.55ms | 1.4× | 1.3% | PASS |
| ac_reads | `index_join_scan` | 3.64ms | 6.22ms | 1.7× | 1.7% | PASS |
| ac_reads | `types_table_scan` | 1.03s | 1.19s | 1.2× | 1.1% | PASS |
| ac_reads | `table_scan` | 1.16s | 1.30s | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_read_only` | 134.08ms | 153.19ms | 1.1× | 1.4% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 22.79ms | 68.17ms | 3.0× | 9.4% | PASS |
| ac_writes | `oltp_insert_ac` | 25.28ms | 84.14ms | 3.3× | 8.0% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.50ms | 92.35ms | 3.4× | 5.9% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.13ms | 75.82ms | 3.1× | 7.5% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.27ms | 84.62ms | 3.3× | 7.6% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.39ms | 83.44ms | 3.3× | 6.7% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.80ms | 76.23ms | 3.5× | 6.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.52ms | 89.13ms | 2.8× | 7.8% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 27.47ms | 29.23ms | 1.1× | 1.1% | PASS |
| mem_reads | `oltp_range_select` | 14.55ms | 12.14ms | 0.8× | 1.5% | PASS |
| mem_reads | `oltp_sum_range` | 12.46ms | 11.60ms | 0.9× | 1.4% | PASS |
| mem_reads | `oltp_order_range` | 2.82ms | 2.94ms | 1.0× | 1.3% | PASS |
| mem_reads | `oltp_distinct_range` | 3.66ms | 3.78ms | 1.0× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 3.12ms | 5.16ms | 1.7× | 1.0% | PASS |
| mem_reads | `select_random_points` | 17.40ms | 17.35ms | 1.0× | 1.5% | PASS |
| mem_reads | `select_random_ranges` | 5.30ms | 5.11ms | 1.0× | 1.1% | PASS |
| mem_reads | `covering_index_scan` | 5.86ms | 7.89ms | 1.3× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 28.96ms | 29.23ms | 1.0× | 0.7% | PASS |
| mem_reads | `index_join` | 8.75ms | 8.30ms | 0.9× | 0.9% | PASS |
| mem_reads | `index_join_scan` | 3.08ms | 5.45ms | 1.8× | 1.4% | PASS |
| mem_reads | `types_table_scan` | 886.97ms | 1.02s | 1.2× | 0.7% | PASS |
| mem_reads | `table_scan` | 1.01s | 1.12s | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_read_only` | 108.58ms | 111.11ms | 1.0× | 0.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 182.52ms | 238.62ms | 1.3× | 0.8% | PASS |
| mem_writes | `oltp_insert` | 13.90ms | 27.16ms | 2.0× | 0.6% | PASS |
| mem_writes | `oltp_update_index` | 51.65ms | 98.00ms | 1.9× | 0.7% | PASS |
| mem_writes | `oltp_update_non_index` | 38.41ms | 58.55ms | 1.5× | 1.1% | PASS |
| mem_writes | `oltp_delete_insert` | 43.24ms | 72.75ms | 1.7× | 1.0% | PASS |
| mem_writes | `oltp_write_only` | 22.55ms | 41.24ms | 1.8× | 0.7% | PASS |
| mem_writes | `types_delete_insert` | 30.66ms | 39.18ms | 1.3× | 1.0% | PASS |
| mem_writes | `oltp_read_write` | 75.52ms | 99.59ms | 1.3× | 1.3% | PASS |
| file_reads | `oltp_point_select` | 93.72ms | 46.44ms | 0.5× | 0.7% | PASS |
| file_reads | `oltp_range_select` | 22.39ms | 13.92ms | 0.6× | 1.0% | PASS |
| file_reads | `oltp_sum_range` | 19.86ms | 13.41ms | 0.7× | 0.9% | PASS |
| file_reads | `oltp_order_range` | 3.63ms | 3.14ms | 0.9× | 1.4% | PASS |
| file_reads | `oltp_distinct_range` | 4.47ms | 4.01ms | 0.9× | 0.7% | PASS |
| file_reads | `oltp_index_scan` | 10.02ms | 6.96ms | 0.7× | 1.0% | PASS |
| file_reads | `select_random_points` | 25.09ms | 19.23ms | 0.8× | 1.0% | PASS |
| file_reads | `select_random_ranges` | 12.33ms | 6.94ms | 0.6× | 1.2% | PASS |
| file_reads | `covering_index_scan` | 12.87ms | 9.68ms | 0.8× | 0.8% | PASS |
| file_reads | `groupby_scan` | 29.82ms | 29.41ms | 1.0× | 0.7% | PASS |
| file_reads | `index_join` | 12.96ms | 9.24ms | 0.7× | 0.8% | PASS |
| file_reads | `index_join_scan` | 4.00ms | 5.70ms | 1.4× | 1.2% | PASS |
| file_reads | `types_table_scan` | 896.45ms | 1.02s | 1.1× | 0.6% | PASS |
| file_reads | `table_scan` | 1.03s | 1.12s | 1.1× | 0.6% | PASS |
| file_reads | `oltp_read_only` | 205.25ms | 136.64ms | 0.7× | 0.7% | PASS |
| file_writes | `oltp_bulk_insert` | 301.91ms | 328.13ms | 1.1× | 11.4% | PASS |
| file_writes | `oltp_insert` | 37.23ms | 51.17ms | 1.4× | 25.0% | PASS |
| file_writes | `oltp_update_index` | 223.60ms | 216.15ms | 1.0× | 23.4% | PASS |
| file_writes | `oltp_update_non_index` | 161.92ms | 139.27ms | 0.9× | 17.0% | PASS |
| file_writes | `oltp_delete_insert` | 207.83ms | 155.28ms | 0.7× | 13.6% | PASS |
| file_writes | `oltp_write_only` | 155.45ms | 105.59ms | 0.7× | 25.6% | PASS |
| file_writes | `types_delete_insert` | 157.20ms | 93.18ms | 0.6× | 20.7% | PASS |
| file_writes | `oltp_read_write` | 195.80ms | 157.72ms | 0.8× | 8.6% | PASS |
| ac_reads | `oltp_point_select` | 49.57ms | 46.41ms | 0.9× | 0.7% | PASS |
| ac_reads | `oltp_range_select` | 18.03ms | 13.95ms | 0.8× | 1.1% | PASS |
| ac_reads | `oltp_sum_range` | 15.54ms | 13.45ms | 0.9× | 1.0% | PASS |
| ac_reads | `oltp_order_range` | 3.27ms | 3.14ms | 1.0× | 1.0% | PASS |
| ac_reads | `oltp_distinct_range` | 4.03ms | 4.02ms | 1.0× | 0.9% | PASS |
| ac_reads | `oltp_index_scan` | 5.74ms | 6.94ms | 1.2× | 0.9% | PASS |
| ac_reads | `select_random_points` | 20.69ms | 19.33ms | 0.9× | 1.0% | PASS |
| ac_reads | `select_random_ranges` | 8.06ms | 6.93ms | 0.9× | 0.6% | PASS |
| ac_reads | `covering_index_scan` | 8.55ms | 9.68ms | 1.1× | 0.6% | PASS |
| ac_reads | `groupby_scan` | 29.50ms | 29.47ms | 1.0× | 0.5% | PASS |
| ac_reads | `index_join` | 11.00ms | 9.28ms | 0.8× | 1.2% | PASS |
| ac_reads | `index_join_scan` | 3.61ms | 5.72ms | 1.6× | 1.2% | PASS |
| ac_reads | `types_table_scan` | 892.14ms | 1.02s | 1.1× | 0.5% | PASS |
| ac_reads | `table_scan` | 1.03s | 1.12s | 1.1× | 0.8% | PASS |
| ac_reads | `oltp_read_only` | 141.45ms | 136.71ms | 1.0× | 0.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 32.13ms | 84.62ms | 2.6× | 51.4% | PASS |
| ac_writes | `oltp_insert_ac` | 30.60ms | 118.73ms | 3.9× | 49.5% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.88ms | 95.79ms | 3.4× | 37.0% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 22.16ms | 90.38ms | 4.1× | 41.3% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.77ms | 96.43ms | 3.7× | 33.2% | PASS |
| ac_writes | `oltp_write_only_ac` | 29.98ms | 107.23ms | 3.6× | 43.2% | PASS |
| ac_writes | `types_delete_insert_ac` | 24.34ms | 110.14ms | 4.5× | 63.9% | PASS |
| ac_writes | `oltp_read_write_ac` | 36.22ms | 119.00ms | 3.3× | 37.9% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 28.51ms | 30.02ms | 1.1× | 2.8% | PASS |
| mem_reads | `oltp_range_select` | 14.38ms | 12.18ms | 0.8× | 3.1% | PASS |
| mem_reads | `oltp_sum_range` | 13.04ms | 11.77ms | 0.9× | 1.9% | PASS |
| mem_reads | `oltp_order_range` | 2.86ms | 2.77ms | 1.0× | 1.2% | PASS |
| mem_reads | `oltp_distinct_range` | 3.65ms | 3.57ms | 1.0× | 1.2% | PASS |
| mem_reads | `oltp_index_scan` | 3.21ms | 5.33ms | 1.7× | 1.3% | PASS |
| mem_reads | `select_random_points` | 16.70ms | 17.17ms | 1.0× | 1.8% | PASS |
| mem_reads | `select_random_ranges` | 5.24ms | 5.19ms | 1.0× | 1.6% | PASS |
| mem_reads | `covering_index_scan` | 5.88ms | 8.05ms | 1.4× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 29.08ms | 29.18ms | 1.0× | 0.9% | PASS |
| mem_reads | `index_join` | 8.78ms | 8.90ms | 1.0× | 1.4% | PASS |
| mem_reads | `index_join_scan` | 3.09ms | 5.68ms | 1.8× | 1.1% | PASS |
| mem_reads | `types_table_scan` | 881.23ms | 1.03s | 1.2× | 1.0% | PASS |
| mem_reads | `table_scan` | 989.18ms | 1.11s | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_read_only` | 109.58ms | 109.14ms | 1.0× | 2.0% | PASS |
| mem_writes | `oltp_bulk_insert` | 184.21ms | 239.98ms | 1.3× | 0.7% | PASS |
| mem_writes | `oltp_insert` | 14.35ms | 28.09ms | 2.0× | 0.7% | PASS |
| mem_writes | `oltp_update_index` | 53.37ms | 100.78ms | 1.9× | 1.3% | PASS |
| mem_writes | `oltp_update_non_index` | 41.35ms | 60.46ms | 1.5× | 2.2% | PASS |
| mem_writes | `oltp_delete_insert` | 45.50ms | 76.56ms | 1.7× | 1.6% | PASS |
| mem_writes | `oltp_write_only` | 23.25ms | 43.09ms | 1.9× | 1.9% | PASS |
| mem_writes | `types_delete_insert` | 30.61ms | 39.95ms | 1.3× | 1.8% | PASS |
| mem_writes | `oltp_read_write` | 73.33ms | 99.53ms | 1.4× | 1.4% | PASS |
| file_reads | `oltp_point_select` | 93.04ms | 46.56ms | 0.5× | 0.9% | PASS |
| file_reads | `oltp_range_select` | 20.76ms | 13.79ms | 0.7× | 1.4% | PASS |
| file_reads | `oltp_sum_range` | 19.45ms | 13.29ms | 0.7× | 1.9% | PASS |
| file_reads | `oltp_order_range` | 3.61ms | 2.95ms | 0.8× | 0.9% | PASS |
| file_reads | `oltp_distinct_range` | 4.39ms | 3.76ms | 0.9× | 0.7% | PASS |
| file_reads | `oltp_index_scan` | 10.23ms | 7.22ms | 0.7× | 0.9% | PASS |
| file_reads | `select_random_points` | 27.49ms | 20.25ms | 0.7× | 2.0% | PASS |
| file_reads | `select_random_ranges` | 12.24ms | 7.02ms | 0.6× | 1.0% | PASS |
| file_reads | `covering_index_scan` | 12.97ms | 9.82ms | 0.8× | 0.9% | PASS |
| file_reads | `groupby_scan` | 30.27ms | 29.74ms | 1.0× | 0.9% | PASS |
| file_reads | `index_join` | 14.14ms | 10.53ms | 0.7× | 3.8% | PASS |
| file_reads | `index_join_scan` | 4.34ms | 6.76ms | 1.6× | 5.1% | PASS |
| file_reads | `types_table_scan` | 979.95ms | 1.14s | 1.2× | 5.6% | PASS |
| file_reads | `table_scan` | 991.88ms | 1.12s | 1.1× | 1.2% | PASS |
| file_reads | `oltp_read_only` | 200.15ms | 132.57ms | 0.7× | 0.6% | PASS |
| file_writes | `oltp_bulk_insert` | 301.81ms | 342.02ms | 1.1× | 16.8% | PASS |
| file_writes | `oltp_insert` | 57.53ms | 53.76ms | 0.9× | 47.6% | PASS |
| file_writes | `oltp_update_index` | 210.91ms | 198.07ms | 0.9× | 24.7% | PASS |
| file_writes | `oltp_update_non_index` | 217.35ms | 146.45ms | 0.7× | 37.7% | PASS |
| file_writes | `oltp_delete_insert` | 232.89ms | 189.36ms | 0.8× | 43.0% | PASS |
| file_writes | `oltp_write_only` | 204.49ms | 117.53ms | 0.6× | 40.9% | PASS |
| file_writes | `types_delete_insert` | 129.13ms | 102.41ms | 0.8× | 25.3% | PASS |
| file_writes | `oltp_read_write` | 241.39ms | 182.70ms | 0.8× | 17.6% | PASS |
| ac_reads | `oltp_point_select` | 49.62ms | 47.01ms | 0.9× | 1.4% | PASS |
| ac_reads | `oltp_range_select` | 16.81ms | 13.96ms | 0.8× | 1.5% | PASS |
| ac_reads | `oltp_sum_range` | 15.43ms | 13.46ms | 0.9× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 3.23ms | 2.96ms | 0.9× | 0.8% | PASS |
| ac_reads | `oltp_distinct_range` | 4.00ms | 3.79ms | 0.9× | 0.9% | PASS |
| ac_reads | `oltp_index_scan` | 5.80ms | 7.15ms | 1.2× | 1.3% | PASS |
| ac_reads | `select_random_points` | 20.72ms | 19.24ms | 0.9× | 1.6% | PASS |
| ac_reads | `select_random_ranges` | 7.82ms | 7.01ms | 0.9× | 1.6% | PASS |
| ac_reads | `covering_index_scan` | 8.57ms | 9.82ms | 1.1× | 0.8% | PASS |
| ac_reads | `groupby_scan` | 29.51ms | 29.40ms | 1.0× | 0.6% | PASS |
| ac_reads | `index_join` | 10.84ms | 9.77ms | 0.9× | 1.8% | PASS |
| ac_reads | `index_join_scan` | 3.58ms | 5.96ms | 1.7× | 2.7% | PASS |
| ac_reads | `types_table_scan` | 894.69ms | 1.04s | 1.2× | 1.3% | PASS |
| ac_reads | `table_scan` | 1.04s | 1.14s | 1.1× | 3.0% | PASS |
| ac_reads | `oltp_read_only` | 145.89ms | 135.94ms | 0.9× | 1.8% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 19.96ms | 62.00ms | 3.1× | 25.2% | PASS |
| ac_writes | `oltp_insert_ac` | 36.10ms | 136.34ms | 3.8× | 55.9% | PASS |
| ac_writes | `oltp_update_index_ac` | 65.41ms | 182.78ms | 2.8× | 69.6% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 36.05ms | 125.30ms | 3.5× | 65.2% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 49.63ms | 139.08ms | 2.8× | 48.0% | PASS |
| ac_writes | `oltp_write_only_ac` | 32.37ms | 276.95ms | 8.6× (paired 4.1×) | 73.9% | PASS |
| ac_writes | `types_delete_insert_ac` | 66.29ms | 156.28ms | 2.4× | 78.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 49.66ms | 177.17ms | 3.6× | 61.4% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.91ms | 42.64ms | 1.3× | 2.3% | PASS |
| mem_reads | `oltp_range_select` | 19.65ms | 22.87ms | 1.2× | 2.5% | PASS |
| mem_reads | `oltp_sum_range` | 18.13ms | 22.27ms | 1.2× | 2.3% | PASS |
| mem_reads | `oltp_order_range` | 3.55ms | 4.09ms | 1.2× | 1.7% | PASS |
| mem_reads | `oltp_distinct_range` | 4.69ms | 5.20ms | 1.1× | 2.6% | PASS |
| mem_reads | `oltp_index_scan` | 4.59ms | 6.56ms | 1.4× | 2.8% | PASS |
| mem_reads | `select_random_points` | 28.55ms | 33.90ms | 1.2× | 2.8% | PASS |
| mem_reads | `select_random_ranges` | 7.74ms | 9.31ms | 1.2× | 2.2% | PASS |
| mem_reads | `covering_index_scan` | 7.65ms | 10.79ms | 1.4× | 1.4% | PASS |
| mem_reads | `groupby_scan` | 35.93ms | 40.84ms | 1.1× | 1.2% | PASS |
| mem_reads | `index_join` | 7.88ms | 11.76ms | 1.5× | 2.1% | PASS |
| mem_reads | `index_join_scan` | 4.04ms | 6.58ms | 1.6× | 3.0% | PASS |
| mem_reads | `types_table_scan` | 1.06s | 1.27s | 1.2× | 1.0% | PASS |
| mem_reads | `table_scan` | 1.25s | 1.49s | 1.2× | 2.3% | PASS |
| mem_reads | `oltp_read_only` | 152.39ms | 180.82ms | 1.2× | 2.0% | PASS |
| mem_writes | `oltp_bulk_insert` | 242.69ms | 322.96ms | 1.3× | 0.8% | PASS |
| mem_writes | `oltp_insert` | 18.76ms | 33.32ms | 1.8× | 1.1% | PASS |
| mem_writes | `oltp_update_index` | 67.07ms | 121.17ms | 1.8× | 1.6% | PASS |
| mem_writes | `oltp_update_non_index` | 51.63ms | 81.96ms | 1.6× | 2.3% | PASS |
| mem_writes | `oltp_delete_insert` | 49.16ms | 91.66ms | 1.9× | 2.4% | PASS |
| mem_writes | `oltp_write_only` | 27.06ms | 53.88ms | 2.0× | 2.3% | PASS |
| mem_writes | `types_delete_insert` | 33.11ms | 53.43ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_read_write` | 102.01ms | 155.69ms | 1.5× | 2.6% | PASS |
| file_reads | `oltp_point_select` | 104.32ms | 61.17ms | 0.6× | 1.4% | PASS |
| file_reads | `oltp_range_select` | 27.36ms | 25.09ms | 0.9× | 1.8% | PASS |
| file_reads | `oltp_sum_range` | 26.30ms | 24.56ms | 0.9× | 2.0% | PASS |
| file_reads | `oltp_order_range` | 4.51ms | 4.44ms | 1.0× | 3.2% | PASS |
| file_reads | `oltp_distinct_range` | 5.62ms | 5.48ms | 1.0× | 2.1% | PASS |
| file_reads | `oltp_index_scan` | 12.05ms | 8.60ms | 0.7× | 1.5% | PASS |
| file_reads | `select_random_points` | 37.92ms | 37.49ms | 1.0× | 2.2% | PASS |
| file_reads | `select_random_ranges` | 15.37ms | 11.48ms | 0.7× | 1.6% | PASS |
| file_reads | `covering_index_scan` | 15.41ms | 12.97ms | 0.8× | 2.8% | PASS |
| file_reads | `groupby_scan` | 36.84ms | 41.31ms | 1.1× | 1.2% | PASS |
| file_reads | `index_join` | 12.29ms | 13.25ms | 1.1× | 2.4% | PASS |
| file_reads | `index_join_scan` | 4.98ms | 6.96ms | 1.4× | 3.4% | PASS |
| file_reads | `types_table_scan` | 1.06s | 1.27s | 1.2× | 1.2% | PASS |
| file_reads | `table_scan` | 1.26s | 1.48s | 1.2× | 2.6% | PASS |
| file_reads | `oltp_read_only` | 261.27ms | 211.64ms | 0.8× | 1.6% | PASS |
| file_writes | `oltp_bulk_insert` | 258.96ms | 334.03ms | 1.3× | 0.8% | PASS |
| file_writes | `oltp_insert` | 26.18ms | 37.24ms | 1.4× | 1.3% | PASS |
| file_writes | `oltp_update_index` | 97.21ms | 135.76ms | 1.4× | 2.2% | PASS |
| file_writes | `oltp_update_non_index` | 78.28ms | 92.23ms | 1.2× | 2.1% | PASS |
| file_writes | `oltp_delete_insert` | 76.93ms | 103.75ms | 1.3× | 1.7% | PASS |
| file_writes | `oltp_write_only` | 51.07ms | 63.89ms | 1.3× | 2.1% | PASS |
| file_writes | `types_delete_insert` | 50.23ms | 61.71ms | 1.2× | 1.8% | PASS |
| file_writes | `oltp_read_write` | 130.46ms | 167.24ms | 1.3× | 2.3% | PASS |
| ac_reads | `oltp_point_select` | 56.65ms | 61.31ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 22.46ms | 25.36ms | 1.1× | 2.2% | PASS |
| ac_reads | `oltp_sum_range` | 21.34ms | 24.70ms | 1.2× | 2.1% | PASS |
| ac_reads | `oltp_order_range` | 4.02ms | 4.44ms | 1.1× | 3.4% | PASS |
| ac_reads | `oltp_distinct_range` | 5.11ms | 5.52ms | 1.1× | 3.7% | PASS |
| ac_reads | `oltp_index_scan` | 7.45ms | 8.69ms | 1.2× | 3.0% | PASS |
| ac_reads | `select_random_points` | 32.35ms | 38.13ms | 1.2× | 1.6% | PASS |
| ac_reads | `select_random_ranges` | 10.40ms | 11.58ms | 1.1× | 1.7% | PASS |
| ac_reads | `covering_index_scan` | 10.82ms | 13.15ms | 1.2× | 3.6% | PASS |
| ac_reads | `groupby_scan` | 36.47ms | 41.27ms | 1.1× | 1.1% | PASS |
| ac_reads | `index_join` | 9.73ms | 13.37ms | 1.4× | 3.6% | PASS |
| ac_reads | `index_join_scan` | 4.51ms | 7.05ms | 1.6× | 4.3% | PASS |
| ac_reads | `types_table_scan` | 1.11s | 1.28s | 1.2× | 1.5% | PASS |
| ac_reads | `table_scan` | 1.30s | 1.49s | 1.1× | 4.1% | PASS |
| ac_reads | `oltp_read_only` | 189.32ms | 210.46ms | 1.1× | 1.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 22.50ms | 63.31ms | 2.8× | 5.4% | PASS |
| ac_writes | `oltp_insert_ac` | 24.11ms | 78.17ms | 3.2× | 6.3% | PASS |
| ac_writes | `oltp_update_index_ac` | 26.45ms | 90.38ms | 3.4× | 5.1% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.30ms | 73.44ms | 3.2× | 5.2% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 24.11ms | 81.11ms | 3.4× | 8.6% | PASS |
| ac_writes | `oltp_write_only_ac` | 24.18ms | 81.08ms | 3.4× | 5.5% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.48ms | 74.10ms | 3.4× | 5.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 32.67ms | 91.00ms | 2.8× | 6.7% | PASS |

</details>

</details>

## Version-control latency

Wall time: 7m 46s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 27.30ms | 38.00ms | 71.8% | 0.6% | PASS |
| `status_dirty_many_tables` | 29.59ms | 42.00ms | 70.4% | 0.3% | PASS |
| `diff_regular_working_one_table` | 23.11ms | 33.00ms | 70.0% | 0.3% | PASS |
| `diff_regular_working_many_tables` | 33.80ms | 50.00ms | 67.6% | 0.7% | PASS |
| `diff_stat_working_many_tables` | 33.72ms | 48.00ms | 70.2% | 0.5% | PASS |
| `diff_schema_working_many_tables` | 34.08ms | 48.00ms | 71.0% | 0.4% | PASS |
| `branch_list_many_branches` | 17.87ms | 25.00ms | 71.5% | 1.0% | PASS |
| `branch_create_delete` | 20.80ms | 27.00ms | 77.0% | 8.2% | PASS |
| `at_literal_deep_history` | 19.03ms | 28.00ms | 68.0% | 0.6% | PASS |
| `diff_literal_deep_history` | 18.98ms | 28.00ms | 67.8% | 0.3% | PASS |
| `history_literal_deep_history` | 19.95ms | 30.00ms | 66.5% | 0.5% | PASS |
| `checkout_branch_clean` | 26.00ms | 43.00ms | 60.5% | 1.2% | PASS |
| `merge_data_no_conflicts` | 21.74ms | 33.00ms | 65.9% | 1.8% | PASS |
| `merge_data_secondary_index` | 726.72ms | 944.00ms | 77.0% | 4.8% | PASS |
| `merge_schema_no_conflicts` | 19.56ms | 24.00ms | 81.5% | 13.6% | PASS |
| `merge_data_conflicts` | 23.70ms | 33.00ms | 71.8% | 0.8% | PASS |
| `merge_data_conflicts_with_resolve` | 24.17ms | 34.00ms | 71.1% | 0.4% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
