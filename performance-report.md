# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-23 11:01 UTC
>
> Commit: [`b2056488e5c24eadedd7eda760448aae90549d24`](https://github.com/dolthub/doltlite/commit/b2056488e5c24eadedd7eda760448aae90549d24)
>
> Runner: ubuntu24 20261012.334.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/39933269148)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 8.14s | 9.29s | 1.1× | 1.2% | **PASS** |
| Writes | 1.61s | 2.41s | 1.5× | 1.1% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 8.82s | 9.38s | 1.1× | 1.1% | **PASS** |
| Writes | 4.28s | 4.01s | 0.9× | 7.2% | **PASS** |
| Autocommit writes | 729.59ms | 2.34s | 3.2× | 11.3% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.06s | 2.28s | 1.1× | 1.7% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.15s | 2.47s | 1.1× | 0.8% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 1.84s | 2.04s | 1.1× | 2.8% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.09s | 2.50s | 1.2× | 0.8% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 336.36ms | 516.90ms | 1.5× | 1.4% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 460.21ms | 682.37ms | 1.5× | 1.1% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 366.85ms | 557.69ms | 1.5× | 2.8% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 449.00ms | 651.31ms | 1.5× | 0.7% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.20s | 2.30s | 1.0× | 1.4% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.39s | 2.53s | 1.1× | 0.9% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 1.91s | 1.99s | 1.0× | 2.5% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.34s | 2.56s | 1.1× | 0.8% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 934.89ms | 866.31ms | 0.9× | 14.3% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 1.27s | 1.15s | 0.9× | 2.8% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 984.77ms | 954.43ms | 1.0× | 3.5% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 1.09s | 1.04s | 1.0× | 10.3% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.08s | 2.31s | 1.1× | 1.5% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.23s | 2.52s | 1.1× | 0.6% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 1.79s | 1.99s | 1.1× | 2.2% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.17s | 2.55s | 1.2× | 0.7% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 188.55ms | 649.38ms | 3.4× | 33.6% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 161.46ms | 530.05ms | 3.3× | 9.5% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 205.63ms | 530.68ms | 2.6× | 5.9% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 173.95ms | 625.70ms | 3.6× | 43.7% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 19.43ms | 23.18ms | 1.2× | 2.5% | PASS |
| mem_reads | `oltp_range_select` | 9.14ms | 9.86ms | 1.1× | 2.0% | PASS |
| mem_reads | `oltp_sum_range` | 7.98ms | 9.54ms | 1.2× | 2.0% | PASS |
| mem_reads | `oltp_order_range` | 2.22ms | 2.48ms | 1.1× | 1.1% | PASS |
| mem_reads | `oltp_distinct_range` | 3.04ms | 3.33ms | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_index_scan` | 3.14ms | 4.44ms | 1.4× | 1.7% | PASS |
| mem_reads | `select_random_points` | 8.65ms | 9.61ms | 1.1× | 2.3% | PASS |
| mem_reads | `select_random_ranges` | 3.77ms | 3.96ms | 1.0× | 1.3% | PASS |
| mem_reads | `covering_index_scan` | 5.84ms | 8.05ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 25.62ms | 27.48ms | 1.1× | 0.5% | PASS |
| mem_reads | `index_join` | 4.50ms | 7.14ms | 1.6× | 1.5% | PASS |
| mem_reads | `index_join_scan` | 2.65ms | 4.81ms | 1.8× | 1.9% | PASS |
| mem_reads | `types_table_scan` | 871.14ms | 985.62ms | 1.1× | 1.3% | PASS |
| mem_reads | `table_scan` | 1.01s | 1.08s | 1.1× | 1.9% | PASS |
| mem_reads | `oltp_read_only` | 80.52ms | 94.76ms | 1.2× | 1.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 136.01ms | 208.90ms | 1.5× | 1.1% | PASS |
| mem_writes | `oltp_insert` | 11.93ms | 19.20ms | 1.6× | 0.8% | PASS |
| mem_writes | `oltp_update_index` | 38.66ms | 64.25ms | 1.7× | 1.0% | PASS |
| mem_writes | `oltp_update_non_index` | 27.35ms | 41.00ms | 1.5× | 1.3% | PASS |
| mem_writes | `oltp_delete_insert` | 34.83ms | 51.22ms | 1.5× | 2.1% | PASS |
| mem_writes | `oltp_write_only` | 17.71ms | 31.13ms | 1.8× | 1.7% | PASS |
| mem_writes | `types_delete_insert` | 19.37ms | 26.34ms | 1.4× | 1.5% | PASS |
| mem_writes | `oltp_read_write` | 50.51ms | 74.86ms | 1.5× | 1.4% | PASS |
| file_reads | `oltp_point_select` | 83.29ms | 40.00ms | 0.5× | 1.4% | PASS |
| file_reads | `oltp_range_select` | 15.36ms | 11.40ms | 0.7× | 1.9% | PASS |
| file_reads | `oltp_sum_range` | 14.42ms | 11.16ms | 0.8× | 1.4% | PASS |
| file_reads | `oltp_order_range` | 2.91ms | 2.66ms | 0.9× | 1.9% | PASS |
| file_reads | `oltp_distinct_range` | 3.75ms | 3.51ms | 0.9× | 1.3% | PASS |
| file_reads | `oltp_index_scan` | 9.87ms | 6.12ms | 0.6× | 1.6% | PASS |
| file_reads | `select_random_points` | 15.21ms | 11.40ms | 0.7× | 1.5% | PASS |
| file_reads | `select_random_ranges` | 10.28ms | 5.72ms | 0.6× | 1.0% | PASS |
| file_reads | `covering_index_scan` | 12.80ms | 9.75ms | 0.8× | 1.0% | PASS |
| file_reads | `groupby_scan` | 26.08ms | 27.56ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 8.24ms | 8.01ms | 1.0× | 1.6% | PASS |
| file_reads | `index_join_scan` | 3.41ms | 4.88ms | 1.4× | 1.8% | PASS |
| file_reads | `types_table_scan` | 849.75ms | 972.05ms | 1.1× | 0.6% | PASS |
| file_reads | `table_scan` | 965.43ms | 1.07s | 1.1× | 0.6% | PASS |
| file_reads | `oltp_read_only` | 174.97ms | 118.87ms | 0.7× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 192.25ms | 262.45ms | 1.4× | 14.3% | PASS |
| file_writes | `oltp_insert` | 29.75ms | 31.71ms | 1.1× | 15.5% | PASS |
| file_writes | `oltp_update_index` | 147.05ms | 125.27ms | 0.9× | 14.3% | PASS |
| file_writes | `oltp_update_non_index` | 112.68ms | 86.56ms | 0.8× | 13.3% | PASS |
| file_writes | `oltp_delete_insert` | 118.92ms | 101.56ms | 0.9× | 12.7% | PASS |
| file_writes | `oltp_write_only` | 120.72ms | 75.84ms | 0.6× | 37.2% | PASS |
| file_writes | `types_delete_insert` | 76.66ms | 60.53ms | 0.8× | 20.9% | PASS |
| file_writes | `oltp_read_write` | 136.87ms | 122.38ms | 0.9× | 12.6% | PASS |
| ac_reads | `oltp_point_select` | 40.74ms | 40.58ms | 1.0× | 2.1% | PASS |
| ac_reads | `oltp_range_select` | 11.22ms | 11.45ms | 1.0× | 1.9% | PASS |
| ac_reads | `oltp_sum_range` | 10.18ms | 11.12ms | 1.1× | 1.0% | PASS |
| ac_reads | `oltp_order_range` | 2.55ms | 2.65ms | 1.0× | 1.0% | PASS |
| ac_reads | `oltp_distinct_range` | 3.37ms | 3.55ms | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_index_scan` | 5.84ms | 6.35ms | 1.1× | 1.8% | PASS |
| ac_reads | `select_random_points` | 13.41ms | 12.96ms | 1.0× | 7.6% | PASS |
| ac_reads | `select_random_ranges` | 6.28ms | 5.84ms | 0.9× | 2.4% | PASS |
| ac_reads | `covering_index_scan` | 8.51ms | 9.83ms | 1.2× | 0.9% | PASS |
| ac_reads | `groupby_scan` | 25.52ms | 27.53ms | 1.1× | 0.6% | PASS |
| ac_reads | `index_join` | 6.08ms | 7.99ms | 1.3× | 1.6% | PASS |
| ac_reads | `index_join_scan` | 3.00ms | 4.87ms | 1.6× | 1.5% | PASS |
| ac_reads | `types_table_scan` | 847.32ms | 970.23ms | 1.1× | 0.4% | PASS |
| ac_reads | `table_scan` | 981.07ms | 1.07s | 1.1× | 1.6% | PASS |
| ac_reads | `oltp_read_only` | 110.86ms | 118.82ms | 1.1× | 0.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 18.18ms | 71.51ms | 3.9× | 22.1% | PASS |
| ac_writes | `oltp_insert_ac` | 20.62ms | 68.03ms | 3.3× | 11.8% | PASS |
| ac_writes | `oltp_update_index_ac` | 24.35ms | 77.91ms | 3.2× | 20.4% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.75ms | 75.02ms | 3.4× | 32.3% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.18ms | 86.24ms | 3.4× | 49.4% | PASS |
| ac_writes | `oltp_write_only_ac` | 24.29ms | 88.02ms | 3.6× | 35.0% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.65ms | 88.75ms | 4.1× | 35.7% | PASS |
| ac_writes | `oltp_read_write_ac` | 32.51ms | 93.90ms | 2.9× | 38.2% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 27.41ms | 29.61ms | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_range_select` | 13.57ms | 12.27ms | 0.9× | 1.0% | PASS |
| mem_reads | `oltp_sum_range` | 12.13ms | 11.97ms | 1.0× | 0.9% | PASS |
| mem_reads | `oltp_order_range` | 2.80ms | 2.80ms | 1.0× | 0.8% | PASS |
| mem_reads | `oltp_distinct_range` | 3.63ms | 3.65ms | 1.0× | 0.8% | PASS |
| mem_reads | `oltp_index_scan` | 3.15ms | 5.20ms | 1.6× | 0.7% | PASS |
| mem_reads | `select_random_points` | 17.37ms | 17.50ms | 1.0× | 1.3% | PASS |
| mem_reads | `select_random_ranges` | 5.28ms | 5.11ms | 1.0× | 0.9% | PASS |
| mem_reads | `covering_index_scan` | 5.87ms | 8.24ms | 1.4× | 0.7% | PASS |
| mem_reads | `groupby_scan` | 29.54ms | 29.32ms | 1.0× | 0.4% | PASS |
| mem_reads | `index_join` | 8.73ms | 8.29ms | 0.9× | 0.9% | PASS |
| mem_reads | `index_join_scan` | 3.05ms | 5.48ms | 1.8× | 0.8% | PASS |
| mem_reads | `types_table_scan` | 887.48ms | 1.06s | 1.2× | 0.4% | PASS |
| mem_reads | `table_scan` | 1.02s | 1.15s | 1.1× | 0.6% | PASS |
| mem_reads | `oltp_read_only` | 108.33ms | 110.36ms | 1.0× | 0.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 182.63ms | 246.41ms | 1.3× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 13.89ms | 26.63ms | 1.9× | 0.6% | PASS |
| mem_writes | `oltp_update_index` | 51.79ms | 96.96ms | 1.9× | 1.2% | PASS |
| mem_writes | `oltp_update_non_index` | 38.81ms | 59.68ms | 1.5× | 1.2% | PASS |
| mem_writes | `oltp_delete_insert` | 44.04ms | 72.28ms | 1.6× | 1.1% | PASS |
| mem_writes | `oltp_write_only` | 22.43ms | 40.51ms | 1.8× | 0.9% | PASS |
| mem_writes | `types_delete_insert` | 31.11ms | 40.11ms | 1.3× | 1.3% | PASS |
| mem_writes | `oltp_read_write` | 75.51ms | 99.79ms | 1.3× | 1.4% | PASS |
| file_reads | `oltp_point_select` | 93.62ms | 46.80ms | 0.5× | 0.8% | PASS |
| file_reads | `oltp_range_select` | 20.82ms | 14.07ms | 0.7× | 1.5% | PASS |
| file_reads | `oltp_sum_range` | 19.37ms | 13.82ms | 0.7× | 1.0% | PASS |
| file_reads | `oltp_order_range` | 3.60ms | 3.02ms | 0.8× | 1.1% | PASS |
| file_reads | `oltp_distinct_range` | 4.40ms | 3.89ms | 0.9× | 0.9% | PASS |
| file_reads | `oltp_index_scan` | 9.98ms | 7.01ms | 0.7× | 0.5% | PASS |
| file_reads | `select_random_points` | 24.96ms | 19.42ms | 0.8× | 1.2% | PASS |
| file_reads | `select_random_ranges` | 12.38ms | 7.04ms | 0.6× | 1.0% | PASS |
| file_reads | `covering_index_scan` | 12.90ms | 10.07ms | 0.8× | 0.7% | PASS |
| file_reads | `groupby_scan` | 30.51ms | 29.59ms | 1.0× | 0.6% | PASS |
| file_reads | `index_join` | 12.88ms | 9.30ms | 0.7× | 0.8% | PASS |
| file_reads | `index_join_scan` | 3.92ms | 5.72ms | 1.5× | 0.9% | PASS |
| file_reads | `types_table_scan` | 896.68ms | 1.06s | 1.2× | 0.7% | PASS |
| file_reads | `table_scan` | 1.03s | 1.15s | 1.1× | 0.7% | PASS |
| file_reads | `oltp_read_only` | 208.64ms | 136.31ms | 0.7× | 1.3% | PASS |
| file_writes | `oltp_bulk_insert` | 268.07ms | 322.11ms | 1.2× | 7.3% | PASS |
| file_writes | `oltp_insert` | 31.84ms | 49.77ms | 1.6× | 3.9% | PASS |
| file_writes | `oltp_update_index` | 175.50ms | 182.63ms | 1.0× | 2.7% | PASS |
| file_writes | `oltp_update_non_index` | 153.08ms | 121.80ms | 0.8× | 2.9% | PASS |
| file_writes | `oltp_delete_insert` | 187.88ms | 142.15ms | 0.8× | 1.6% | PASS |
| file_writes | `oltp_write_only` | 125.14ms | 95.77ms | 0.8× | 2.2% | PASS |
| file_writes | `types_delete_insert` | 149.65ms | 84.27ms | 0.6× | 4.1% | PASS |
| file_writes | `oltp_read_write` | 178.24ms | 155.44ms | 0.9× | 2.1% | PASS |
| ac_reads | `oltp_point_select` | 49.66ms | 46.91ms | 0.9× | 0.6% | PASS |
| ac_reads | `oltp_range_select` | 16.79ms | 14.05ms | 0.8× | 0.6% | PASS |
| ac_reads | `oltp_sum_range` | 15.22ms | 13.86ms | 0.9× | 0.6% | PASS |
| ac_reads | `oltp_order_range` | 3.25ms | 3.02ms | 0.9× | 0.9% | PASS |
| ac_reads | `oltp_distinct_range` | 4.00ms | 3.87ms | 1.0× | 0.8% | PASS |
| ac_reads | `oltp_index_scan` | 5.77ms | 7.04ms | 1.2× | 0.9% | PASS |
| ac_reads | `select_random_points` | 20.67ms | 19.43ms | 0.9× | 0.6% | PASS |
| ac_reads | `select_random_ranges` | 7.99ms | 7.04ms | 0.9× | 0.9% | PASS |
| ac_reads | `covering_index_scan` | 8.57ms | 10.05ms | 1.2× | 0.5% | PASS |
| ac_reads | `groupby_scan` | 29.85ms | 29.54ms | 1.0× | 0.4% | PASS |
| ac_reads | `index_join` | 10.84ms | 9.28ms | 0.9× | 0.9% | PASS |
| ac_reads | `index_join_scan` | 3.55ms | 5.72ms | 1.6× | 0.7% | PASS |
| ac_reads | `types_table_scan` | 892.95ms | 1.06s | 1.2× | 0.5% | PASS |
| ac_reads | `table_scan` | 1.02s | 1.15s | 1.1× | 0.5% | PASS |
| ac_reads | `oltp_read_only` | 140.29ms | 135.46ms | 1.0× | 0.6% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 17.99ms | 53.04ms | 2.9× | 8.3% | PASS |
| ac_writes | `oltp_insert_ac` | 20.31ms | 62.62ms | 3.1× | 9.6% | PASS |
| ac_writes | `oltp_update_index_ac` | 21.19ms | 79.80ms | 3.8× | 9.7% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 17.86ms | 63.18ms | 3.5× | 10.5% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 20.44ms | 69.24ms | 3.4× | 9.3% | PASS |
| ac_writes | `oltp_write_only_ac` | 20.02ms | 66.56ms | 3.3× | 7.9% | PASS |
| ac_writes | `types_delete_insert_ac` | 18.41ms | 62.02ms | 3.4× | 10.9% | PASS |
| ac_writes | `oltp_read_write_ac` | 25.24ms | 73.60ms | 2.9× | 7.9% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 23.98ms | 24.71ms | 1.0× | 3.4% | PASS |
| mem_reads | `oltp_range_select` | 10.46ms | 9.89ms | 0.9× | 3.1% | PASS |
| mem_reads | `oltp_sum_range` | 9.84ms | 9.59ms | 1.0× | 3.9% | PASS |
| mem_reads | `oltp_order_range` | 2.26ms | 2.23ms | 1.0× | 2.3% | PASS |
| mem_reads | `oltp_distinct_range` | 2.75ms | 2.75ms | 1.0× | 2.1% | PASS |
| mem_reads | `oltp_index_scan` | 2.68ms | 4.74ms | 1.8× | 2.2% | PASS |
| mem_reads | `select_random_points` | 16.78ms | 15.62ms | 0.9× | 3.7% | PASS |
| mem_reads | `select_random_ranges` | 4.61ms | 4.30ms | 0.9× | 2.5% | PASS |
| mem_reads | `covering_index_scan` | 4.23ms | 6.73ms | 1.6× | 1.9% | PASS |
| mem_reads | `groupby_scan` | 21.76ms | 21.97ms | 1.0× | 1.5% | PASS |
| mem_reads | `index_join` | 8.36ms | 8.47ms | 1.0× | 2.5% | PASS |
| mem_reads | `index_join_scan` | 2.93ms | 5.87ms | 2.0× | 4.4% | PASS |
| mem_reads | `types_table_scan` | 723.94ms | 858.13ms | 1.2× | 3.5% | PASS |
| mem_reads | `table_scan` | 922.11ms | 984.49ms | 1.1× | 2.8% | PASS |
| mem_reads | `oltp_read_only` | 87.46ms | 84.62ms | 1.0× | 5.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 143.17ms | 191.47ms | 1.3× | 1.0% | PASS |
| mem_writes | `oltp_insert` | 11.14ms | 22.38ms | 2.0× | 1.4% | PASS |
| mem_writes | `oltp_update_index` | 44.27ms | 84.66ms | 1.9× | 3.3% | PASS |
| mem_writes | `oltp_update_non_index` | 33.07ms | 50.20ms | 1.5× | 2.9% | PASS |
| mem_writes | `oltp_delete_insert` | 36.01ms | 63.26ms | 1.8× | 2.7% | PASS |
| mem_writes | `oltp_write_only` | 18.74ms | 35.31ms | 1.9× | 1.7% | PASS |
| mem_writes | `types_delete_insert` | 25.12ms | 32.99ms | 1.3× | 3.5% | PASS |
| mem_writes | `oltp_read_write` | 55.33ms | 77.42ms | 1.4× | 3.9% | PASS |
| file_reads | `oltp_point_select` | 77.48ms | 38.32ms | 0.5× | 2.5% | PASS |
| file_reads | `oltp_range_select` | 16.16ms | 10.96ms | 0.7× | 2.6% | PASS |
| file_reads | `oltp_sum_range` | 15.31ms | 10.69ms | 0.7× | 2.9% | PASS |
| file_reads | `oltp_order_range` | 2.89ms | 2.37ms | 0.8× | 1.7% | PASS |
| file_reads | `oltp_distinct_range` | 3.43ms | 2.90ms | 0.8× | 1.4% | PASS |
| file_reads | `oltp_index_scan` | 8.53ms | 6.01ms | 0.7× | 1.6% | PASS |
| file_reads | `select_random_points` | 21.43ms | 15.90ms | 0.7× | 3.9% | PASS |
| file_reads | `select_random_ranges` | 9.99ms | 5.63ms | 0.6× | 1.6% | PASS |
| file_reads | `covering_index_scan` | 10.00ms | 7.74ms | 0.8× | 2.5% | PASS |
| file_reads | `groupby_scan` | 21.25ms | 21.15ms | 1.0× | 1.3% | PASS |
| file_reads | `index_join` | 11.45ms | 8.10ms | 0.7× | 5.1% | PASS |
| file_reads | `index_join_scan` | 3.42ms | 5.11ms | 1.5× | 4.0% | PASS |
| file_reads | `types_table_scan` | 714.22ms | 834.23ms | 1.2× | 2.6% | PASS |
| file_reads | `table_scan` | 828.46ms | 916.16ms | 1.1× | 2.8% | PASS |
| file_reads | `oltp_read_only` | 161.09ms | 103.91ms | 0.6× | 2.4% | PASS |
| file_writes | `oltp_bulk_insert` | 204.84ms | 253.87ms | 1.2× | 2.8% | PASS |
| file_writes | `oltp_insert` | 19.52ms | 44.33ms | 2.3× | 12.5% | PASS |
| file_writes | `oltp_update_index` | 141.54ms | 157.46ms | 1.1× | 3.5% | PASS |
| file_writes | `oltp_update_non_index` | 120.93ms | 101.50ms | 0.8× | 3.6% | PASS |
| file_writes | `oltp_delete_insert` | 143.87ms | 117.33ms | 0.8× | 4.4% | PASS |
| file_writes | `oltp_write_only` | 100.45ms | 80.12ms | 0.8× | 3.3% | PASS |
| file_writes | `types_delete_insert` | 105.79ms | 70.04ms | 0.7× | 3.5% | PASS |
| file_writes | `oltp_read_write` | 147.84ms | 129.78ms | 0.9× | 4.5% | PASS |
| ac_reads | `oltp_point_select` | 44.01ms | 39.79ms | 0.9× | 2.9% | PASS |
| ac_reads | `oltp_range_select` | 13.68ms | 11.74ms | 0.9× | 3.2% | PASS |
| ac_reads | `oltp_sum_range` | 13.03ms | 11.41ms | 0.9× | 3.1% | PASS |
| ac_reads | `oltp_order_range` | 2.58ms | 2.38ms | 0.9× | 1.6% | PASS |
| ac_reads | `oltp_distinct_range` | 3.12ms | 2.94ms | 0.9× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 4.95ms | 6.01ms | 1.2× | 2.0% | PASS |
| ac_reads | `select_random_points` | 19.66ms | 17.24ms | 0.9× | 7.2% | PASS |
| ac_reads | `select_random_ranges` | 6.66ms | 5.69ms | 0.9× | 2.1% | PASS |
| ac_reads | `covering_index_scan` | 6.50ms | 7.99ms | 1.2× | 2.2% | PASS |
| ac_reads | `groupby_scan` | 21.27ms | 21.39ms | 1.0× | 1.7% | PASS |
| ac_reads | `index_join` | 10.13ms | 8.32ms | 0.8× | 5.7% | PASS |
| ac_reads | `index_join_scan` | 3.13ms | 5.15ms | 1.6× | 3.5% | PASS |
| ac_reads | `types_table_scan` | 706.81ms | 833.79ms | 1.2× | 1.7% | PASS |
| ac_reads | `table_scan` | 813.52ms | 908.85ms | 1.1× | 1.9% | PASS |
| ac_reads | `oltp_read_only` | 117.02ms | 106.29ms | 0.9× | 4.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 24.37ms | 53.55ms | 2.2× | 4.4% | PASS |
| ac_writes | `oltp_insert_ac` | 27.77ms | 67.56ms | 2.4× | 7.4% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.60ms | 74.10ms | 2.7× | 7.2% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.08ms | 61.51ms | 2.9× | 6.0% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.84ms | 67.36ms | 2.5× | 5.9% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.06ms | 67.97ms | 2.6× | 4.6% | PASS |
| ac_writes | `types_delete_insert_ac` | 22.29ms | 63.64ms | 2.9× | 6.5% | PASS |
| ac_writes | `oltp_read_write_ac` | 29.63ms | 75.00ms | 2.5× | 5.7% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 25.59ms | 30.08ms | 1.2× | 1.7% | PASS |
| mem_reads | `oltp_range_select` | 14.84ms | 17.42ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_sum_range` | 13.55ms | 16.80ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_order_range` | 2.88ms | 3.21ms | 1.1× | 0.8% | PASS |
| mem_reads | `oltp_distinct_range` | 3.71ms | 4.10ms | 1.1× | 0.7% | PASS |
| mem_reads | `oltp_index_scan` | 3.55ms | 4.76ms | 1.3× | 1.6% | PASS |
| mem_reads | `select_random_points` | 20.92ms | 23.98ms | 1.1× | 0.7% | PASS |
| mem_reads | `select_random_ranges` | 5.68ms | 6.65ms | 1.2× | 0.9% | PASS |
| mem_reads | `covering_index_scan` | 5.83ms | 7.80ms | 1.3× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 29.77ms | 33.10ms | 1.1× | 0.6% | PASS |
| mem_reads | `index_join` | 6.16ms | 8.43ms | 1.4× | 1.4% | PASS |
| mem_reads | `index_join_scan` | 3.10ms | 5.08ms | 1.6× | 0.8% | PASS |
| mem_reads | `types_table_scan` | 859.92ms | 1.05s | 1.2× | 0.6% | PASS |
| mem_reads | `table_scan` | 983.21ms | 1.15s | 1.2× | 0.4% | PASS |
| mem_reads | `oltp_read_only` | 112.57ms | 134.13ms | 1.2× | 0.5% | PASS |
| mem_writes | `oltp_bulk_insert` | 187.39ms | 239.65ms | 1.3× | 0.7% | PASS |
| mem_writes | `oltp_insert` | 14.60ms | 23.71ms | 1.6× | 0.7% | PASS |
| mem_writes | `oltp_update_index` | 51.22ms | 84.06ms | 1.6× | 0.7% | PASS |
| mem_writes | `oltp_update_non_index` | 39.00ms | 57.94ms | 1.5× | 0.7% | PASS |
| mem_writes | `oltp_delete_insert` | 37.80ms | 63.69ms | 1.7× | 1.0% | PASS |
| mem_writes | `oltp_write_only` | 21.04ms | 37.33ms | 1.8× | 0.8% | PASS |
| mem_writes | `types_delete_insert` | 24.71ms | 37.07ms | 1.5× | 0.6% | PASS |
| mem_writes | `oltp_read_write` | 73.23ms | 107.87ms | 1.5× | 0.6% | PASS |
| file_reads | `oltp_point_select` | 90.83ms | 47.12ms | 0.5× | 0.6% | PASS |
| file_reads | `oltp_range_select` | 22.30ms | 19.31ms | 0.9× | 0.6% | PASS |
| file_reads | `oltp_sum_range` | 20.55ms | 18.81ms | 0.9× | 0.9% | PASS |
| file_reads | `oltp_order_range` | 3.71ms | 3.49ms | 0.9× | 1.0% | PASS |
| file_reads | `oltp_distinct_range` | 4.50ms | 4.38ms | 1.0× | 0.9% | PASS |
| file_reads | `oltp_index_scan` | 10.50ms | 6.90ms | 0.7× | 0.6% | PASS |
| file_reads | `select_random_points` | 28.39ms | 26.18ms | 0.9× | 0.6% | PASS |
| file_reads | `select_random_ranges` | 12.64ms | 8.69ms | 0.7× | 0.8% | PASS |
| file_reads | `covering_index_scan` | 12.85ms | 10.05ms | 0.8× | 0.9% | PASS |
| file_reads | `groupby_scan` | 30.55ms | 33.46ms | 1.1× | 0.6% | PASS |
| file_reads | `index_join` | 9.91ms | 10.09ms | 1.0× | 1.1% | PASS |
| file_reads | `index_join_scan` | 3.95ms | 5.47ms | 1.4× | 1.0% | PASS |
| file_reads | `types_table_scan` | 865.79ms | 1.05s | 1.2× | 0.8% | PASS |
| file_reads | `table_scan` | 1.01s | 1.16s | 1.1× | 1.9% | PASS |
| file_reads | `oltp_read_only` | 208.31ms | 159.73ms | 0.8× | 0.6% | PASS |
| file_writes | `oltp_bulk_insert` | 240.31ms | 294.09ms | 1.2× | 2.6% | PASS |
| file_writes | `oltp_insert` | 33.33ms | 39.53ms | 1.2× | 17.9% | PASS |
| file_writes | `oltp_update_index` | 160.17ms | 151.90ms | 0.9× | 7.0% | PASS |
| file_writes | `oltp_update_non_index` | 134.90ms | 107.59ms | 0.8× | 8.6% | PASS |
| file_writes | `oltp_delete_insert` | 162.93ms | 137.03ms | 0.8× | 24.8% | PASS |
| file_writes | `oltp_write_only` | 108.47ms | 83.42ms | 0.8× | 23.6% | PASS |
| file_writes | `types_delete_insert` | 86.35ms | 72.27ms | 0.8× | 7.4% | PASS |
| file_writes | `oltp_read_write` | 167.19ms | 153.46ms | 0.9× | 12.0% | PASS |
| ac_reads | `oltp_point_select` | 47.16ms | 46.95ms | 1.0× | 0.7% | PASS |
| ac_reads | `oltp_range_select` | 17.57ms | 19.25ms | 1.1× | 0.5% | PASS |
| ac_reads | `oltp_sum_range` | 16.03ms | 18.77ms | 1.2× | 0.7% | PASS |
| ac_reads | `oltp_order_range` | 3.27ms | 3.46ms | 1.1× | 0.6% | PASS |
| ac_reads | `oltp_distinct_range` | 4.06ms | 4.37ms | 1.1× | 0.8% | PASS |
| ac_reads | `oltp_index_scan` | 6.13ms | 6.84ms | 1.1× | 1.0% | PASS |
| ac_reads | `select_random_points` | 23.88ms | 26.05ms | 1.1× | 0.5% | PASS |
| ac_reads | `select_random_ranges` | 8.13ms | 8.62ms | 1.1× | 0.7% | PASS |
| ac_reads | `covering_index_scan` | 8.52ms | 10.04ms | 1.2× | 0.7% | PASS |
| ac_reads | `groupby_scan` | 30.01ms | 33.42ms | 1.1× | 0.6% | PASS |
| ac_reads | `index_join` | 7.69ms | 10.02ms | 1.3× | 0.7% | PASS |
| ac_reads | `index_join_scan` | 3.56ms | 5.44ms | 1.5× | 1.0% | PASS |
| ac_reads | `types_table_scan` | 860.59ms | 1.05s | 1.2× | 0.5% | PASS |
| ac_reads | `table_scan` | 987.32ms | 1.15s | 1.2× | 0.5% | PASS |
| ac_reads | `oltp_read_only` | 144.80ms | 159.42ms | 1.1× | 0.4% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 19.09ms | 63.26ms | 3.3× | 58.8% | PASS |
| ac_writes | `oltp_insert_ac` | 22.95ms | 82.28ms | 3.6× | 55.1% | PASS |
| ac_writes | `oltp_update_index_ac` | 23.82ms | 104.51ms | 4.4× | 47.0% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.55ms | 70.47ms | 3.3× | 33.2% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 21.32ms | 90.47ms | 4.2× | 52.5% | PASS |
| ac_writes | `oltp_write_only_ac` | 21.04ms | 72.09ms | 3.4× | 40.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 17.27ms | 66.46ms | 3.8× | 34.3% | PASS |
| ac_writes | `oltp_read_write_ac` | 26.91ms | 76.16ms | 2.8× | 17.1% | PASS |

</details>

</details>

## Version-control latency

Wall time: 4m 16s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 33.70ms | 38.00ms | 88.7% | 0.4% | PASS |
| `status_dirty_many_tables` | 37.24ms | 42.00ms | 88.7% | 0.3% | PASS |
| `diff_regular_working_one_table` | 29.08ms | 33.00ms | 88.1% | 0.4% | PASS |
| `diff_regular_working_many_tables` | 42.40ms | 50.00ms | 84.8% | 0.4% | PASS |
| `diff_stat_working_many_tables` | 42.40ms | 48.00ms | 88.3% | 0.4% | PASS |
| `diff_schema_working_many_tables` | 42.80ms | 48.00ms | 89.2% | 0.3% | PASS |
| `branch_list_many_branches` | 21.49ms | 25.00ms | 86.0% | 0.4% | PASS |
| `branch_create_delete` | 22.95ms | 27.00ms | 85.0% | 0.6% | PASS |
| `at_literal_deep_history` | 23.17ms | 28.00ms | 82.8% | 0.4% | PASS |
| `diff_literal_deep_history` | 23.25ms | 28.00ms | 83.0% | 0.4% | PASS |
| `history_literal_deep_history` | 24.42ms | 30.00ms | 81.4% | 0.4% | PASS |
| `checkout_branch_clean` | 32.17ms | 43.00ms | 74.8% | 0.4% | PASS |
| `merge_data_no_conflicts` | 26.93ms | 33.00ms | 81.6% | 1.0% | PASS |
| `merge_data_secondary_index` | 807.00ms | 944.00ms | 85.5% | 0.4% | PASS |
| `merge_schema_no_conflicts` | 20.24ms | 24.00ms | 84.3% | 0.6% | PASS |
| `merge_data_conflicts` | 29.38ms | 33.00ms | 89.0% | 0.4% | PASS |
| `merge_data_conflicts_with_resolve` | 30.12ms | 34.00ms | 88.6% | 0.4% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
