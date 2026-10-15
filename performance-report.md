# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-15 11:10 UTC
>
> Commit: [`a89951e4b92fc12aa76b03fb457d8ba01aaa3d3d`](https://github.com/dolthub/doltlite/commit/a89951e4b92fc12aa76b03fb457d8ba01aaa3d3d)
>
> Runner: ubuntu24 20261012.334.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/38714355343)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.38s | 12.72s | 1.2× | 1.4% | **PASS** |
| Writes | 2.21s | 3.53s | 1.6× | 1.4% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 11.53s | 13.06s | 1.1× | 1.6% | **PASS** |
| Writes | 3.13s | 3.91s | 1.3× | 2.0% | **PASS** |
| Autocommit writes | 817.42ms | 2.63s | 3.2× | 6.6% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.43s | 2.97s | 1.2× | 1.3% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.58s | 3.21s | 1.2× | 1.7% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.62s | 3.24s | 1.2× | 1.6% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.75s | 3.29s | 1.2× | 1.4% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 433.56ms | 712.40ms | 1.6× | 1.4% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 590.11ms | 943.42ms | 1.6× | 1.3% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 600.37ms | 951.76ms | 1.6× | 1.9% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 590.93ms | 917.56ms | 1.6× | 1.4% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.65s | 3.03s | 1.1× | 1.4% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.91s | 3.30s | 1.1× | 1.7% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.96s | 3.36s | 1.1× | 1.9% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 3.01s | 3.37s | 1.1× | 1.9% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 588.95ms | 786.03ms | 1.3× | 1.8% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 922.65ms | 1.04s | 1.1× | 3.0% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 825.36ms | 1.05s | 1.3× | 2.0% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 789.30ms | 1.03s | 1.3× | 2.0% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.51s | 3.03s | 1.2× | 1.6% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.80s | 3.32s | 1.2× | 2.2% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.68s | 3.31s | 1.2× | 1.7% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.76s | 3.35s | 1.2× | 1.3% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 198.29ms | 656.03ms | 3.3× | 7.4% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 214.18ms | 670.80ms | 3.1× | 6.6% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 207.69ms | 655.27ms | 3.2× | 5.6% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 197.27ms | 643.10ms | 3.3× | 6.7% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 23.70ms | 31.51ms | 1.3× | 1.7% | PASS |
| mem_reads | `oltp_range_select` | 10.25ms | 12.94ms | 1.3× | 2.3% | PASS |
| mem_reads | `oltp_sum_range` | 9.39ms | 12.50ms | 1.3× | 2.0% | PASS |
| mem_reads | `oltp_order_range` | 2.56ms | 3.00ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_distinct_range` | 3.62ms | 4.17ms | 1.2× | 0.9% | PASS |
| mem_reads | `oltp_index_scan` | 3.82ms | 5.56ms | 1.5× | 1.3% | PASS |
| mem_reads | `select_random_points` | 9.71ms | 11.91ms | 1.2× | 2.5% | PASS |
| mem_reads | `select_random_ranges` | 4.56ms | 5.40ms | 1.2× | 1.1% | PASS |
| mem_reads | `covering_index_scan` | 7.58ms | 10.53ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 29.36ms | 32.95ms | 1.1× | 0.8% | PASS |
| mem_reads | `index_join` | 5.70ms | 8.85ms | 1.6× | 1.7% | PASS |
| mem_reads | `index_join_scan` | 3.23ms | 5.58ms | 1.7× | 2.1% | PASS |
| mem_reads | `types_table_scan` | 1.03s | 1.29s | 1.3× | 0.4% | PASS |
| mem_reads | `table_scan` | 1.19s | 1.41s | 1.2× | 1.4% | PASS |
| mem_reads | `oltp_read_only` | 101.99ms | 126.66ms | 1.2× | 0.9% | PASS |
| mem_writes | `oltp_bulk_insert` | 177.58ms | 277.40ms | 1.6× | 0.8% | PASS |
| mem_writes | `oltp_insert` | 15.05ms | 27.20ms | 1.8× | 1.2% | PASS |
| mem_writes | `oltp_update_index` | 49.43ms | 89.83ms | 1.8× | 1.6% | PASS |
| mem_writes | `oltp_update_non_index` | 34.33ms | 56.52ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_delete_insert` | 44.69ms | 72.35ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_write_only` | 21.95ms | 43.81ms | 2.0× | 1.2% | PASS |
| mem_writes | `types_delete_insert` | 24.67ms | 37.60ms | 1.5× | 1.5% | PASS |
| mem_writes | `oltp_read_write` | 65.87ms | 107.69ms | 1.6× | 1.6% | PASS |
| file_reads | `oltp_point_select` | 93.18ms | 50.51ms | 0.5× | 1.2% | PASS |
| file_reads | `oltp_range_select` | 17.41ms | 15.16ms | 0.9× | 1.6% | PASS |
| file_reads | `oltp_sum_range` | 16.74ms | 14.44ms | 0.9× | 1.9% | PASS |
| file_reads | `oltp_order_range` | 3.38ms | 3.29ms | 1.0× | 1.9% | PASS |
| file_reads | `oltp_distinct_range` | 4.47ms | 4.42ms | 1.0× | 1.4% | PASS |
| file_reads | `oltp_index_scan` | 11.03ms | 7.81ms | 0.7× | 1.4% | PASS |
| file_reads | `select_random_points` | 17.22ms | 14.17ms | 0.8× | 2.4% | PASS |
| file_reads | `select_random_ranges` | 11.79ms | 7.49ms | 0.6× | 1.5% | PASS |
| file_reads | `covering_index_scan` | 15.01ms | 12.83ms | 0.9× | 1.0% | PASS |
| file_reads | `groupby_scan` | 30.23ms | 33.35ms | 1.1× | 0.8% | PASS |
| file_reads | `index_join` | 9.68ms | 10.31ms | 1.1× | 1.2% | PASS |
| file_reads | `index_join_scan` | 4.16ms | 5.97ms | 1.4× | 2.0% | PASS |
| file_reads | `types_table_scan` | 1.03s | 1.29s | 1.2× | 0.5% | PASS |
| file_reads | `table_scan` | 1.17s | 1.40s | 1.2× | 0.9% | PASS |
| file_reads | `oltp_read_only` | 204.47ms | 154.97ms | 0.8× | 1.1% | PASS |
| file_writes | `oltp_bulk_insert` | 193.05ms | 289.42ms | 1.5× | 1.3% | PASS |
| file_writes | `oltp_insert` | 21.58ms | 30.39ms | 1.4× | 1.7% | PASS |
| file_writes | `oltp_update_index` | 76.35ms | 102.05ms | 1.3× | 1.7% | PASS |
| file_writes | `oltp_update_non_index` | 57.42ms | 66.87ms | 1.2× | 2.9% | PASS |
| file_writes | `oltp_delete_insert` | 67.04ms | 84.09ms | 1.3× | 2.1% | PASS |
| file_writes | `oltp_write_only` | 44.05ms | 52.52ms | 1.2× | 2.5% | PASS |
| file_writes | `types_delete_insert` | 39.61ms | 43.28ms | 1.1× | 1.8% | PASS |
| file_writes | `oltp_read_write` | 89.86ms | 117.40ms | 1.3× | 1.9% | PASS |
| ac_reads | `oltp_point_select` | 46.89ms | 50.36ms | 1.1× | 1.0% | PASS |
| ac_reads | `oltp_range_select` | 12.85ms | 15.15ms | 1.2× | 1.6% | PASS |
| ac_reads | `oltp_sum_range` | 12.11ms | 14.40ms | 1.2× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 2.96ms | 3.31ms | 1.1× | 2.1% | PASS |
| ac_reads | `oltp_distinct_range` | 4.00ms | 4.42ms | 1.1× | 1.9% | PASS |
| ac_reads | `oltp_index_scan` | 6.35ms | 7.79ms | 1.2× | 1.7% | PASS |
| ac_reads | `select_random_points` | 12.66ms | 14.09ms | 1.1× | 2.1% | PASS |
| ac_reads | `select_random_ranges` | 7.05ms | 7.47ms | 1.1× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.26ms | 12.83ms | 1.3× | 1.7% | PASS |
| ac_reads | `groupby_scan` | 29.75ms | 33.36ms | 1.1× | 0.5% | PASS |
| ac_reads | `index_join` | 7.30ms | 10.34ms | 1.4× | 2.0% | PASS |
| ac_reads | `index_join_scan` | 3.64ms | 5.98ms | 1.6× | 2.0% | PASS |
| ac_reads | `types_table_scan` | 1.04s | 1.29s | 1.2× | 0.5% | PASS |
| ac_reads | `table_scan` | 1.18s | 1.41s | 1.2× | 0.9% | PASS |
| ac_reads | `oltp_read_only` | 136.49ms | 155.69ms | 1.1× | 1.1% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 21.75ms | 68.61ms | 3.2× | 7.3% | PASS |
| ac_writes | `oltp_insert_ac` | 24.37ms | 81.67ms | 3.4× | 8.8% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.57ms | 92.60ms | 3.4× | 8.9% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.87ms | 74.24ms | 3.4× | 5.2% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 24.94ms | 83.92ms | 3.4× | 7.3% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.34ms | 85.46ms | 3.4× | 7.6% | PASS |
| ac_writes | `types_delete_insert_ac` | 23.05ms | 77.79ms | 3.4× | 8.2% | PASS |
| ac_writes | `oltp_read_write_ac` | 29.42ms | 91.75ms | 3.1× | 5.8% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 36.40ms | 41.05ms | 1.1× | 2.6% | PASS |
| mem_reads | `oltp_range_select` | 17.64ms | 17.17ms | 1.0× | 1.5% | PASS |
| mem_reads | `oltp_sum_range` | 16.00ms | 16.68ms | 1.0× | 1.9% | PASS |
| mem_reads | `oltp_order_range` | 3.22ms | 3.66ms | 1.1× | 1.8% | PASS |
| mem_reads | `oltp_distinct_range` | 4.36ms | 4.73ms | 1.1× | 1.1% | PASS |
| mem_reads | `oltp_index_scan` | 3.88ms | 6.81ms | 1.8× | 1.5% | PASS |
| mem_reads | `select_random_points` | 21.35ms | 22.99ms | 1.1× | 2.1% | PASS |
| mem_reads | `select_random_ranges` | 6.36ms | 7.06ms | 1.1× | 1.9% | PASS |
| mem_reads | `covering_index_scan` | 7.65ms | 11.07ms | 1.4× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 34.62ms | 36.76ms | 1.1× | 0.7% | PASS |
| mem_reads | `index_join` | 10.39ms | 10.82ms | 1.0× | 1.8% | PASS |
| mem_reads | `index_join_scan` | 3.76ms | 6.65ms | 1.8× | 3.1% | PASS |
| mem_reads | `types_table_scan` | 1.06s | 1.38s | 1.3× | 0.9% | PASS |
| mem_reads | `table_scan` | 1.21s | 1.49s | 1.2× | 1.7% | PASS |
| mem_reads | `oltp_read_only` | 139.06ms | 153.94ms | 1.1× | 1.4% | PASS |
| mem_writes | `oltp_bulk_insert` | 237.01ms | 334.34ms | 1.4× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 17.80ms | 37.28ms | 2.1× | 0.6% | PASS |
| mem_writes | `oltp_update_index` | 65.81ms | 134.03ms | 2.0× | 1.6% | PASS |
| mem_writes | `oltp_update_non_index` | 47.78ms | 80.95ms | 1.7× | 1.1% | PASS |
| mem_writes | `oltp_delete_insert` | 55.18ms | 100.68ms | 1.8× | 1.5% | PASS |
| mem_writes | `oltp_write_only` | 28.95ms | 57.54ms | 2.0× | 1.0% | PASS |
| mem_writes | `types_delete_insert` | 40.19ms | 56.62ms | 1.4× | 1.4% | PASS |
| mem_writes | `oltp_read_write` | 97.39ms | 141.97ms | 1.5× | 1.9% | PASS |
| file_reads | `oltp_point_select` | 105.69ms | 60.14ms | 0.6× | 0.8% | PASS |
| file_reads | `oltp_range_select` | 24.46ms | 19.13ms | 0.8× | 2.8% | PASS |
| file_reads | `oltp_sum_range` | 24.05ms | 18.92ms | 0.8× | 1.7% | PASS |
| file_reads | `oltp_order_range` | 4.34ms | 3.81ms | 0.9× | 1.6% | PASS |
| file_reads | `oltp_distinct_range` | 5.40ms | 4.91ms | 0.9× | 1.8% | PASS |
| file_reads | `oltp_index_scan` | 11.37ms | 8.90ms | 0.8× | 1.7% | PASS |
| file_reads | `select_random_points` | 31.39ms | 25.98ms | 0.8× | 2.9% | PASS |
| file_reads | `select_random_ranges` | 14.12ms | 9.21ms | 0.7× | 2.1% | PASS |
| file_reads | `covering_index_scan` | 15.13ms | 13.09ms | 0.9× | 1.5% | PASS |
| file_reads | `groupby_scan` | 35.62ms | 36.95ms | 1.0× | 0.9% | PASS |
| file_reads | `index_join` | 14.85ms | 12.10ms | 0.8× | 2.3% | PASS |
| file_reads | `index_join_scan` | 4.84ms | 7.17ms | 1.5× | 2.6% | PASS |
| file_reads | `types_table_scan` | 1.07s | 1.39s | 1.3× | 1.2% | PASS |
| file_reads | `table_scan` | 1.29s | 1.51s | 1.2× | 4.1% | PASS |
| file_reads | `oltp_read_only` | 256.35ms | 183.73ms | 0.7× | 1.2% | PASS |
| file_writes | `oltp_bulk_insert` | 262.76ms | 350.21ms | 1.3× | 1.3% | PASS |
| file_writes | `oltp_insert` | 25.53ms | 42.49ms | 1.7× | 2.0% | PASS |
| file_writes | `oltp_update_index` | 126.55ms | 150.40ms | 1.2× | 12.2% | PASS |
| file_writes | `oltp_update_non_index` | 103.38ms | 94.92ms | 0.9× | 8.5% | PASS |
| file_writes | `oltp_delete_insert` | 93.60ms | 113.25ms | 1.2× | 1.8% | PASS |
| file_writes | `oltp_write_only` | 88.40ms | 68.63ms | 0.8× | 11.8% | PASS |
| file_writes | `types_delete_insert` | 71.87ms | 67.53ms | 0.9× | 1.9% | PASS |
| file_writes | `oltp_read_write` | 150.56ms | 155.79ms | 1.0× | 3.9% | PASS |
| ac_reads | `oltp_point_select` | 60.22ms | 60.39ms | 1.0× | 1.6% | PASS |
| ac_reads | `oltp_range_select` | 19.95ms | 19.19ms | 1.0× | 2.2% | PASS |
| ac_reads | `oltp_sum_range` | 18.49ms | 18.85ms | 1.0× | 2.1% | PASS |
| ac_reads | `oltp_order_range` | 3.90ms | 3.82ms | 1.0× | 2.4% | PASS |
| ac_reads | `oltp_distinct_range` | 5.00ms | 4.90ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_index_scan` | 6.88ms | 9.02ms | 1.3× | 1.4% | PASS |
| ac_reads | `select_random_points` | 28.08ms | 26.79ms | 1.0× | 2.7% | PASS |
| ac_reads | `select_random_ranges` | 9.79ms | 9.33ms | 1.0× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.82ms | 13.34ms | 1.2× | 2.2% | PASS |
| ac_reads | `groupby_scan` | 35.97ms | 37.59ms | 1.0× | 1.0% | PASS |
| ac_reads | `index_join` | 14.06ms | 13.00ms | 0.9× | 3.0% | PASS |
| ac_reads | `index_join_scan` | 4.55ms | 7.48ms | 1.6× | 2.7% | PASS |
| ac_reads | `types_table_scan` | 1.09s | 1.40s | 1.3× | 2.6% | PASS |
| ac_reads | `table_scan` | 1.31s | 1.51s | 1.2× | 5.0% | PASS |
| ac_reads | `oltp_read_only` | 176.03ms | 181.26ms | 1.0× | 1.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 24.74ms | 68.94ms | 2.8× | 8.4% | PASS |
| ac_writes | `oltp_insert_ac` | 27.84ms | 81.85ms | 2.9× | 6.4% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.47ms | 96.82ms | 3.5× | 6.8% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.07ms | 77.73ms | 3.2× | 7.4% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.54ms | 88.38ms | 3.3× | 6.3% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.33ms | 85.77ms | 3.3× | 5.5% | PASS |
| ac_writes | `types_delete_insert_ac` | 23.64ms | 76.42ms | 3.2× | 7.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 33.55ms | 94.89ms | 2.8× | 6.1% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 34.74ms | 41.28ms | 1.2× | 2.2% | PASS |
| mem_reads | `oltp_range_select` | 15.99ms | 17.10ms | 1.1× | 2.7% | PASS |
| mem_reads | `oltp_sum_range` | 14.54ms | 16.41ms | 1.1× | 2.1% | PASS |
| mem_reads | `oltp_order_range` | 3.17ms | 3.50ms | 1.1× | 1.1% | PASS |
| mem_reads | `oltp_distinct_range` | 4.27ms | 4.58ms | 1.1× | 1.2% | PASS |
| mem_reads | `oltp_index_scan` | 3.97ms | 6.95ms | 1.8× | 1.2% | PASS |
| mem_reads | `select_random_points` | 21.71ms | 22.66ms | 1.0× | 3.5% | PASS |
| mem_reads | `select_random_ranges` | 6.35ms | 7.04ms | 1.1× | 3.1% | PASS |
| mem_reads | `covering_index_scan` | 7.67ms | 11.16ms | 1.5× | 1.0% | PASS |
| mem_reads | `groupby_scan` | 33.56ms | 36.52ms | 1.1× | 0.8% | PASS |
| mem_reads | `index_join` | 10.16ms | 11.10ms | 1.1× | 2.5% | PASS |
| mem_reads | `index_join_scan` | 3.73ms | 6.68ms | 1.8× | 2.6% | PASS |
| mem_reads | `types_table_scan` | 1.17s | 1.43s | 1.2× | 1.4% | PASS |
| mem_reads | `table_scan` | 1.16s | 1.48s | 1.3× | 0.6% | PASS |
| mem_reads | `oltp_read_only` | 132.15ms | 151.22ms | 1.1× | 1.6% | PASS |
| mem_writes | `oltp_bulk_insert` | 239.58ms | 339.49ms | 1.4× | 1.0% | PASS |
| mem_writes | `oltp_insert` | 18.25ms | 37.68ms | 2.1× | 0.8% | PASS |
| mem_writes | `oltp_update_index` | 62.88ms | 128.78ms | 2.0× | 1.2% | PASS |
| mem_writes | `oltp_update_non_index` | 47.51ms | 80.28ms | 1.7× | 2.1% | PASS |
| mem_writes | `oltp_delete_insert` | 55.28ms | 100.93ms | 1.8× | 2.1% | PASS |
| mem_writes | `oltp_write_only` | 29.31ms | 58.22ms | 2.0× | 2.6% | PASS |
| mem_writes | `types_delete_insert` | 40.18ms | 57.04ms | 1.4× | 2.0% | PASS |
| mem_writes | `oltp_read_write` | 107.38ms | 149.34ms | 1.4× | 1.8% | PASS |
| file_reads | `oltp_point_select` | 107.54ms | 61.68ms | 0.6× | 1.7% | PASS |
| file_reads | `oltp_range_select` | 23.60ms | 19.23ms | 0.8× | 1.9% | PASS |
| file_reads | `oltp_sum_range` | 22.42ms | 18.68ms | 0.8× | 2.3% | PASS |
| file_reads | `oltp_order_range` | 4.19ms | 3.75ms | 0.9× | 2.1% | PASS |
| file_reads | `oltp_distinct_range` | 5.22ms | 4.84ms | 0.9× | 1.9% | PASS |
| file_reads | `oltp_index_scan` | 11.40ms | 9.08ms | 0.8× | 1.4% | PASS |
| file_reads | `select_random_points` | 30.36ms | 25.99ms | 0.9× | 2.0% | PASS |
| file_reads | `select_random_ranges` | 14.13ms | 9.18ms | 0.6× | 1.6% | PASS |
| file_reads | `covering_index_scan` | 15.48ms | 13.28ms | 0.9× | 1.6% | PASS |
| file_reads | `groupby_scan` | 35.07ms | 37.05ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 15.24ms | 12.49ms | 0.8× | 3.5% | PASS |
| file_reads | `index_join_scan` | 4.89ms | 7.22ms | 1.5× | 3.3% | PASS |
| file_reads | `types_table_scan` | 1.13s | 1.42s | 1.3× | 2.6% | PASS |
| file_reads | `table_scan` | 1.28s | 1.52s | 1.2× | 1.4% | PASS |
| file_reads | `oltp_read_only` | 255.53ms | 188.51ms | 0.7× | 1.3% | PASS |
| file_writes | `oltp_bulk_insert` | 261.85ms | 353.29ms | 1.3× | 1.3% | PASS |
| file_writes | `oltp_insert` | 25.85ms | 43.53ms | 1.7× | 2.0% | PASS |
| file_writes | `oltp_update_index` | 99.26ms | 151.62ms | 1.5× | 2.0% | PASS |
| file_writes | `oltp_update_non_index` | 93.92ms | 95.61ms | 1.0× | 10.6% | PASS |
| file_writes | `oltp_delete_insert` | 88.53ms | 115.09ms | 1.3× | 1.6% | PASS |
| file_writes | `oltp_write_only` | 59.32ms | 70.87ms | 1.2× | 1.9% | PASS |
| file_writes | `types_delete_insert` | 64.23ms | 66.35ms | 1.0× | 2.1% | PASS |
| file_writes | `oltp_read_write` | 132.40ms | 157.06ms | 1.2× | 2.3% | PASS |
| ac_reads | `oltp_point_select` | 58.85ms | 61.14ms | 1.0× | 1.8% | PASS |
| ac_reads | `oltp_range_select` | 19.00ms | 19.15ms | 1.0× | 3.1% | PASS |
| ac_reads | `oltp_sum_range` | 17.66ms | 18.76ms | 1.1× | 1.7% | PASS |
| ac_reads | `oltp_order_range` | 3.82ms | 3.78ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_distinct_range` | 4.81ms | 4.87ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 6.70ms | 9.04ms | 1.4× | 1.3% | PASS |
| ac_reads | `select_random_points` | 25.07ms | 25.64ms | 1.0× | 3.0% | PASS |
| ac_reads | `select_random_ranges` | 8.77ms | 9.18ms | 1.0× | 2.4% | PASS |
| ac_reads | `covering_index_scan` | 10.22ms | 13.25ms | 1.3× | 1.1% | PASS |
| ac_reads | `groupby_scan` | 33.66ms | 36.77ms | 1.1× | 0.6% | PASS |
| ac_reads | `index_join` | 11.62ms | 12.24ms | 1.1× | 2.4% | PASS |
| ac_reads | `index_join_scan` | 4.14ms | 6.89ms | 1.7× | 2.1% | PASS |
| ac_reads | `types_table_scan` | 1.07s | 1.40s | 1.3× | 1.2% | PASS |
| ac_reads | `table_scan` | 1.23s | 1.51s | 1.2× | 1.9% | PASS |
| ac_reads | `oltp_read_only` | 172.32ms | 181.98ms | 1.1× | 1.6% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.68ms | 67.09ms | 2.8× | 7.2% | PASS |
| ac_writes | `oltp_insert_ac` | 25.92ms | 82.59ms | 3.2× | 7.4% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.98ms | 96.96ms | 3.5× | 5.5% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 22.36ms | 75.77ms | 3.4× | 4.2% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.65ms | 83.67ms | 3.3× | 3.4% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.77ms | 83.54ms | 3.1× | 6.1% | PASS |
| ac_writes | `types_delete_insert_ac` | 23.65ms | 75.18ms | 3.2× | 5.6% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.68ms | 90.48ms | 2.9× | 5.2% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 32.64ms | 43.47ms | 1.3× | 1.6% | PASS |
| mem_reads | `oltp_range_select` | 18.54ms | 24.01ms | 1.3× | 1.4% | PASS |
| mem_reads | `oltp_sum_range` | 17.28ms | 22.73ms | 1.3× | 1.1% | PASS |
| mem_reads | `oltp_order_range` | 3.51ms | 4.18ms | 1.2× | 1.6% | PASS |
| mem_reads | `oltp_distinct_range` | 4.68ms | 5.26ms | 1.1× | 0.8% | PASS |
| mem_reads | `oltp_index_scan` | 4.53ms | 6.71ms | 1.5× | 2.3% | PASS |
| mem_reads | `select_random_points` | 28.34ms | 33.85ms | 1.2× | 1.8% | PASS |
| mem_reads | `select_random_ranges` | 7.91ms | 9.66ms | 1.2× | 1.0% | PASS |
| mem_reads | `covering_index_scan` | 7.67ms | 11.10ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 35.92ms | 41.18ms | 1.1× | 0.8% | PASS |
| mem_reads | `index_join` | 7.84ms | 11.50ms | 1.5× | 1.4% | PASS |
| mem_reads | `index_join_scan` | 3.52ms | 6.37ms | 1.8× | 1.5% | PASS |
| mem_reads | `types_table_scan` | 1.05s | 1.37s | 1.3× | 1.0% | PASS |
| mem_reads | `table_scan` | 1.37s | 1.51s | 1.1× | 2.0% | PASS |
| mem_reads | `oltp_read_only` | 161.35ms | 188.70ms | 1.2× | 1.1% | PASS |
| mem_writes | `oltp_bulk_insert` | 243.27ms | 325.90ms | 1.3× | 0.8% | PASS |
| mem_writes | `oltp_insert` | 18.62ms | 33.25ms | 1.8× | 1.5% | PASS |
| mem_writes | `oltp_update_index` | 65.71ms | 117.81ms | 1.8× | 1.4% | PASS |
| mem_writes | `oltp_update_non_index` | 50.12ms | 81.15ms | 1.6× | 1.5% | PASS |
| mem_writes | `oltp_delete_insert` | 49.16ms | 92.04ms | 1.9× | 1.0% | PASS |
| mem_writes | `oltp_write_only` | 27.37ms | 54.95ms | 2.0× | 1.6% | PASS |
| mem_writes | `types_delete_insert` | 33.07ms | 54.03ms | 1.6× | 1.3% | PASS |
| mem_writes | `oltp_read_write` | 103.62ms | 158.43ms | 1.5× | 1.7% | PASS |
| file_reads | `oltp_point_select` | 103.92ms | 62.97ms | 0.6× | 1.2% | PASS |
| file_reads | `oltp_range_select` | 27.46ms | 26.46ms | 1.0× | 1.6% | PASS |
| file_reads | `oltp_sum_range` | 25.60ms | 25.30ms | 1.0× | 1.2% | PASS |
| file_reads | `oltp_order_range` | 4.49ms | 4.52ms | 1.0× | 2.1% | PASS |
| file_reads | `oltp_distinct_range` | 5.60ms | 5.59ms | 1.0× | 1.9% | PASS |
| file_reads | `oltp_index_scan` | 11.67ms | 8.77ms | 0.8× | 2.0% | PASS |
| file_reads | `select_random_points` | 37.71ms | 37.17ms | 1.0× | 1.9% | PASS |
| file_reads | `select_random_ranges` | 15.05ms | 11.79ms | 0.8× | 1.9% | PASS |
| file_reads | `covering_index_scan` | 14.88ms | 13.12ms | 0.9× | 1.1% | PASS |
| file_reads | `groupby_scan` | 35.88ms | 41.60ms | 1.2× | 0.7% | PASS |
| file_reads | `index_join` | 11.85ms | 13.37ms | 1.1× | 1.4% | PASS |
| file_reads | `index_join_scan` | 5.08ms | 7.00ms | 1.4× | 2.3% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.37s | 1.3× | 1.3% | PASS |
| file_reads | `table_scan` | 1.40s | 1.52s | 1.1× | 3.9% | PASS |
| file_reads | `oltp_read_only` | 271.29ms | 220.64ms | 0.8× | 3.1% | PASS |
| file_writes | `oltp_bulk_insert` | 264.19ms | 341.64ms | 1.3× | 1.4% | PASS |
| file_writes | `oltp_insert` | 27.60ms | 38.49ms | 1.4× | 2.9% | PASS |
| file_writes | `oltp_update_index` | 102.50ms | 142.91ms | 1.4× | 1.9% | PASS |
| file_writes | `oltp_update_non_index` | 82.01ms | 96.69ms | 1.2× | 2.0% | PASS |
| file_writes | `oltp_delete_insert` | 81.35ms | 109.53ms | 1.3× | 2.6% | PASS |
| file_writes | `oltp_write_only` | 55.76ms | 67.53ms | 1.2× | 2.0% | PASS |
| file_writes | `types_delete_insert` | 52.45ms | 63.73ms | 1.2× | 1.9% | PASS |
| file_writes | `oltp_read_write` | 123.43ms | 165.78ms | 1.3× | 1.3% | PASS |
| ac_reads | `oltp_point_select` | 55.26ms | 62.15ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 21.29ms | 26.20ms | 1.2× | 2.6% | PASS |
| ac_reads | `oltp_sum_range` | 20.29ms | 25.15ms | 1.2× | 1.6% | PASS |
| ac_reads | `oltp_order_range` | 3.90ms | 4.49ms | 1.2× | 1.6% | PASS |
| ac_reads | `oltp_distinct_range` | 5.06ms | 5.60ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_index_scan` | 7.18ms | 8.83ms | 1.2× | 1.4% | PASS |
| ac_reads | `select_random_points` | 32.04ms | 37.39ms | 1.2× | 1.9% | PASS |
| ac_reads | `select_random_ranges` | 10.34ms | 11.87ms | 1.1× | 0.8% | PASS |
| ac_reads | `covering_index_scan` | 10.50ms | 13.25ms | 1.3× | 1.1% | PASS |
| ac_reads | `groupby_scan` | 36.43ms | 41.82ms | 1.1× | 0.8% | PASS |
| ac_reads | `index_join` | 9.26ms | 13.29ms | 1.4× | 1.2% | PASS |
| ac_reads | `index_join_scan` | 4.62ms | 7.10ms | 1.5× | 3.3% | PASS |
| ac_reads | `types_table_scan` | 1.05s | 1.37s | 1.3× | 1.1% | PASS |
| ac_reads | `table_scan` | 1.30s | 1.50s | 1.1× | 3.0% | PASS |
| ac_reads | `oltp_read_only` | 189.84ms | 218.50ms | 1.2× | 1.3% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 22.66ms | 63.97ms | 2.8× | 6.3% | PASS |
| ac_writes | `oltp_insert_ac` | 24.49ms | 81.95ms | 3.3× | 7.1% | PASS |
| ac_writes | `oltp_update_index_ac` | 25.11ms | 89.93ms | 3.6× | 4.2% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 22.33ms | 73.06ms | 3.3× | 7.3% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.04ms | 84.45ms | 3.4× | 9.3% | PASS |
| ac_writes | `oltp_write_only_ac` | 24.89ms | 84.62ms | 3.4× | 5.0% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.67ms | 75.72ms | 3.5× | 7.0% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.08ms | 89.40ms | 2.9× | 6.3% | PASS |

</details>

</details>

## Version-control latency

Wall time: 4m 16s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 34.00ms | 38.00ms | 89.5% | 0.4% | PASS |
| `status_dirty_many_tables` | 37.48ms | 42.00ms | 89.2% | 0.4% | PASS |
| `diff_regular_working_one_table` | 29.20ms | 33.00ms | 88.5% | 0.4% | PASS |
| `diff_regular_working_many_tables` | 42.69ms | 50.00ms | 85.4% | 0.4% | PASS |
| `diff_stat_working_many_tables` | 42.76ms | 48.00ms | 89.1% | 0.4% | PASS |
| `diff_schema_working_many_tables` | 43.11ms | 48.00ms | 89.8% | 0.2% | PASS |
| `branch_list_many_branches` | 21.56ms | 25.00ms | 86.2% | 0.4% | PASS |
| `branch_create_delete` | 23.14ms | 27.00ms | 85.7% | 0.7% | PASS |
| `at_literal_deep_history` | 23.37ms | 28.00ms | 83.5% | 0.4% | PASS |
| `diff_literal_deep_history` | 23.34ms | 28.00ms | 83.3% | 0.4% | PASS |
| `history_literal_deep_history` | 24.56ms | 30.00ms | 81.9% | 0.7% | PASS |
| `checkout_branch_clean` | 31.90ms | 43.00ms | 74.2% | 0.5% | PASS |
| `merge_data_no_conflicts` | 26.61ms | 33.00ms | 80.6% | 0.6% | PASS |
| `merge_data_secondary_index` | 809.83ms | 944.00ms | 85.8% | 0.4% | PASS |
| `merge_schema_no_conflicts` | 20.78ms | 24.00ms | 86.6% | 1.0% | PASS |
| `merge_data_conflicts` | 29.44ms | 33.00ms | 89.2% | 0.4% | PASS |
| `merge_data_conflicts_with_resolve` | 30.34ms | 34.00ms | 89.2% | 0.6% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
