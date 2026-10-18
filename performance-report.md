# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-18 11:07 UTC
>
> Commit: [`b92803229f229125795dd6bee0a6f704e7f841f5`](https://github.com/dolthub/doltlite/commit/b92803229f229125795dd6bee0a6f704e7f841f5)
>
> Runner: ubuntu24 20261012.334.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/39115908093)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.19s | 11.89s | 1.2× | 1.4% | **PASS** |
| Writes | 2.20s | 3.52s | 1.6× | 1.6% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 11.50s | 12.21s | 1.1× | 1.6% | **PASS** |
| Writes | 3.06s | 3.88s | 1.3× | 1.8% | **PASS** |
| Autocommit writes | 812.29ms | 2.62s | 3.2× | 6.2% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.38s | 2.72s | 1.1× | 1.2% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.67s | 3.05s | 1.1× | 1.5% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.55s | 3.03s | 1.2× | 1.6% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.59s | 3.09s | 1.2× | 1.4% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 421.13ms | 704.45ms | 1.7× | 1.4% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 602.77ms | 955.57ms | 1.6× | 1.8% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 589.24ms | 937.05ms | 1.6× | 2.3% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 591.33ms | 925.80ms | 1.6× | 1.5% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.60s | 2.78s | 1.1× | 1.4% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 3.04s | 3.15s | 1.0× | 1.6% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.78s | 3.10s | 1.1× | 1.7% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 3.08s | 3.19s | 1.0× | 1.6% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 572.25ms | 773.57ms | 1.4× | 1.4% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 909.33ms | 1.05s | 1.2× | 3.1% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 811.94ms | 1.04s | 1.3× | 1.8% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 769.17ms | 1.01s | 1.3× | 1.8% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.46s | 2.78s | 1.1× | 1.2% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.69s | 3.10s | 1.2× | 1.5% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.63s | 3.09s | 1.2× | 1.6% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.99s | 3.21s | 1.1× | 1.6% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 190.18ms | 619.83ms | 3.3× | 6.1% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 214.94ms | 689.73ms | 3.2× | 6.7% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 207.22ms | 665.74ms | 3.2× | 6.1% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 199.95ms | 647.69ms | 3.2× | 5.4% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 22.15ms | 30.99ms | 1.4× | 1.4% | PASS |
| mem_reads | `oltp_range_select` | 9.42ms | 12.60ms | 1.3× | 1.7% | PASS |
| mem_reads | `oltp_sum_range` | 8.69ms | 11.96ms | 1.4× | 1.3% | PASS |
| mem_reads | `oltp_order_range` | 2.45ms | 2.96ms | 1.2× | 1.2% | PASS |
| mem_reads | `oltp_distinct_range` | 3.49ms | 4.04ms | 1.2× | 0.8% | PASS |
| mem_reads | `oltp_index_scan` | 3.60ms | 5.38ms | 1.5× | 1.2% | PASS |
| mem_reads | `select_random_points` | 9.02ms | 11.85ms | 1.3× | 5.3% | PASS |
| mem_reads | `select_random_ranges` | 4.28ms | 5.33ms | 1.2× | 2.6% | PASS |
| mem_reads | `covering_index_scan` | 7.51ms | 10.37ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 28.90ms | 33.15ms | 1.1× | 0.4% | PASS |
| mem_reads | `index_join` | 5.58ms | 8.43ms | 1.5× | 1.0% | PASS |
| mem_reads | `index_join_scan` | 2.90ms | 5.66ms | 2.0× | 2.3% | PASS |
| mem_reads | `types_table_scan` | 1.02s | 1.18s | 1.1× | 0.8% | PASS |
| mem_reads | `table_scan` | 1.15s | 1.28s | 1.1× | 0.7% | PASS |
| mem_reads | `oltp_read_only` | 99.68ms | 124.15ms | 1.2× | 1.0% | PASS |
| mem_writes | `oltp_bulk_insert` | 172.61ms | 273.29ms | 1.6× | 0.8% | PASS |
| mem_writes | `oltp_insert` | 14.84ms | 26.97ms | 1.8× | 1.0% | PASS |
| mem_writes | `oltp_update_index` | 48.79ms | 90.72ms | 1.9× | 1.3% | PASS |
| mem_writes | `oltp_update_non_index` | 33.17ms | 56.25ms | 1.7× | 1.3% | PASS |
| mem_writes | `oltp_delete_insert` | 44.13ms | 72.50ms | 1.6× | 1.5% | PASS |
| mem_writes | `oltp_write_only` | 21.55ms | 43.46ms | 2.0× | 1.6% | PASS |
| mem_writes | `types_delete_insert` | 23.24ms | 36.38ms | 1.6× | 1.6% | PASS |
| mem_writes | `oltp_read_write` | 62.80ms | 104.89ms | 1.7× | 1.4% | PASS |
| file_reads | `oltp_point_select` | 89.94ms | 49.40ms | 0.5× | 0.5% | PASS |
| file_reads | `oltp_range_select` | 16.69ms | 14.63ms | 0.9× | 2.7% | PASS |
| file_reads | `oltp_sum_range` | 15.80ms | 14.14ms | 0.9× | 1.3% | PASS |
| file_reads | `oltp_order_range` | 3.35ms | 3.35ms | 1.0× | 4.7% | PASS |
| file_reads | `oltp_distinct_range` | 4.44ms | 4.40ms | 1.0× | 2.6% | PASS |
| file_reads | `oltp_index_scan` | 10.65ms | 7.64ms | 0.7× | 1.5% | PASS |
| file_reads | `select_random_points` | 16.39ms | 14.05ms | 0.9× | 2.4% | PASS |
| file_reads | `select_random_ranges` | 11.40ms | 7.33ms | 0.6× | 1.4% | PASS |
| file_reads | `covering_index_scan` | 14.75ms | 12.74ms | 0.9× | 1.2% | PASS |
| file_reads | `groupby_scan` | 29.95ms | 33.60ms | 1.1× | 0.9% | PASS |
| file_reads | `index_join` | 9.50ms | 10.17ms | 1.1× | 1.4% | PASS |
| file_reads | `index_join_scan` | 4.06ms | 6.05ms | 1.5× | 2.6% | PASS |
| file_reads | `types_table_scan` | 1.03s | 1.18s | 1.1× | 0.8% | PASS |
| file_reads | `table_scan` | 1.15s | 1.27s | 1.1× | 0.6% | PASS |
| file_reads | `oltp_read_only` | 198.55ms | 151.38ms | 0.8× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 186.58ms | 282.35ms | 1.5× | 1.0% | PASS |
| file_writes | `oltp_insert` | 21.23ms | 30.25ms | 1.4× | 1.6% | PASS |
| file_writes | `oltp_update_index` | 73.19ms | 100.89ms | 1.4× | 1.6% | PASS |
| file_writes | `oltp_update_non_index` | 54.18ms | 65.78ms | 1.2× | 1.9% | PASS |
| file_writes | `oltp_delete_insert` | 65.97ms | 83.28ms | 1.3× | 1.3% | PASS |
| file_writes | `oltp_write_only` | 43.25ms | 52.86ms | 1.2× | 1.3% | PASS |
| file_writes | `types_delete_insert` | 38.57ms | 42.88ms | 1.1× | 1.6% | PASS |
| file_writes | `oltp_read_write` | 89.28ms | 115.28ms | 1.3× | 1.3% | PASS |
| ac_reads | `oltp_point_select` | 45.93ms | 49.74ms | 1.1× | 1.1% | PASS |
| ac_reads | `oltp_range_select` | 12.87ms | 14.63ms | 1.1× | 2.7% | PASS |
| ac_reads | `oltp_sum_range` | 11.73ms | 13.96ms | 1.2× | 0.9% | PASS |
| ac_reads | `oltp_order_range` | 2.96ms | 3.25ms | 1.1× | 2.1% | PASS |
| ac_reads | `oltp_distinct_range` | 3.97ms | 4.35ms | 1.1× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 6.13ms | 7.56ms | 1.2× | 1.7% | PASS |
| ac_reads | `select_random_points` | 11.54ms | 13.86ms | 1.2× | 1.7% | PASS |
| ac_reads | `select_random_ranges` | 6.66ms | 7.32ms | 1.1× | 1.2% | PASS |
| ac_reads | `covering_index_scan` | 9.94ms | 12.63ms | 1.3× | 1.2% | PASS |
| ac_reads | `groupby_scan` | 29.38ms | 33.56ms | 1.1× | 0.7% | PASS |
| ac_reads | `index_join` | 7.06ms | 10.09ms | 1.4× | 1.2% | PASS |
| ac_reads | `index_join_scan` | 3.53ms | 6.03ms | 1.7× | 1.7% | PASS |
| ac_reads | `types_table_scan` | 1.03s | 1.17s | 1.1× | 0.6% | PASS |
| ac_reads | `table_scan` | 1.15s | 1.27s | 1.1× | 0.5% | PASS |
| ac_reads | `oltp_read_only` | 131.13ms | 151.41ms | 1.2× | 0.5% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 21.14ms | 63.50ms | 3.0× | 7.1% | PASS |
| ac_writes | `oltp_insert_ac` | 24.64ms | 75.92ms | 3.1× | 6.6% | PASS |
| ac_writes | `oltp_update_index_ac` | 25.08ms | 89.40ms | 3.6× | 6.1% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.43ms | 70.45ms | 3.3× | 5.1% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 24.22ms | 81.25ms | 3.4× | 5.8% | PASS |
| ac_writes | `oltp_write_only_ac` | 24.08ms | 80.22ms | 3.3× | 6.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 20.75ms | 72.58ms | 3.5× | 6.0% | PASS |
| ac_writes | `oltp_read_write_ac` | 28.82ms | 86.50ms | 3.0× | 5.6% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 35.07ms | 41.37ms | 1.2× | 2.1% | PASS |
| mem_reads | `oltp_range_select` | 16.67ms | 16.44ms | 1.0× | 1.9% | PASS |
| mem_reads | `oltp_sum_range` | 15.71ms | 15.48ms | 1.0× | 1.5% | PASS |
| mem_reads | `oltp_order_range` | 3.30ms | 3.69ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_distinct_range` | 4.34ms | 4.82ms | 1.1× | 1.1% | PASS |
| mem_reads | `oltp_index_scan` | 3.93ms | 6.85ms | 1.7× | 1.3% | PASS |
| mem_reads | `select_random_points` | 22.80ms | 23.20ms | 1.0× | 2.5% | PASS |
| mem_reads | `select_random_ranges` | 6.75ms | 7.08ms | 1.0× | 1.7% | PASS |
| mem_reads | `covering_index_scan` | 7.64ms | 11.13ms | 1.5× | 1.1% | PASS |
| mem_reads | `groupby_scan` | 34.66ms | 36.66ms | 1.1× | 1.0% | PASS |
| mem_reads | `index_join` | 10.55ms | 10.68ms | 1.0× | 1.4% | PASS |
| mem_reads | `index_join_scan` | 3.73ms | 6.62ms | 1.8× | 2.0% | PASS |
| mem_reads | `types_table_scan` | 1.08s | 1.30s | 1.2× | 1.2% | PASS |
| mem_reads | `table_scan` | 1.28s | 1.42s | 1.1× | 3.4% | PASS |
| mem_reads | `oltp_read_only` | 147.20ms | 157.47ms | 1.1× | 1.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 237.32ms | 337.59ms | 1.4× | 0.7% | PASS |
| mem_writes | `oltp_insert` | 17.82ms | 37.95ms | 2.1× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 68.22ms | 136.72ms | 2.0× | 2.2% | PASS |
| mem_writes | `oltp_update_non_index` | 50.20ms | 83.59ms | 1.7× | 2.1% | PASS |
| mem_writes | `oltp_delete_insert` | 55.81ms | 101.42ms | 1.8× | 1.6% | PASS |
| mem_writes | `oltp_write_only` | 28.87ms | 57.33ms | 2.0× | 1.4% | PASS |
| mem_writes | `types_delete_insert` | 40.41ms | 56.84ms | 1.4× | 2.4% | PASS |
| mem_writes | `oltp_read_write` | 104.12ms | 144.14ms | 1.4× | 2.2% | PASS |
| file_reads | `oltp_point_select` | 108.99ms | 61.66ms | 0.6× | 1.3% | PASS |
| file_reads | `oltp_range_select` | 25.59ms | 18.79ms | 0.7× | 1.8% | PASS |
| file_reads | `oltp_sum_range` | 24.15ms | 17.91ms | 0.7× | 1.7% | PASS |
| file_reads | `oltp_order_range` | 4.38ms | 3.85ms | 0.9× | 1.8% | PASS |
| file_reads | `oltp_distinct_range` | 5.41ms | 4.99ms | 0.9× | 1.3% | PASS |
| file_reads | `oltp_index_scan` | 11.45ms | 9.02ms | 0.8× | 1.2% | PASS |
| file_reads | `select_random_points` | 32.24ms | 26.49ms | 0.8× | 1.8% | PASS |
| file_reads | `select_random_ranges` | 14.52ms | 9.23ms | 0.6× | 1.3% | PASS |
| file_reads | `covering_index_scan` | 15.30ms | 13.27ms | 0.9× | 1.6% | PASS |
| file_reads | `groupby_scan` | 35.72ms | 37.05ms | 1.0× | 0.7% | PASS |
| file_reads | `index_join` | 15.46ms | 12.08ms | 0.8× | 2.4% | PASS |
| file_reads | `index_join_scan` | 4.83ms | 7.03ms | 1.5× | 1.4% | PASS |
| file_reads | `types_table_scan` | 1.14s | 1.32s | 1.2× | 2.3% | PASS |
| file_reads | `table_scan` | 1.35s | 1.42s | 1.1× | 2.6% | PASS |
| file_reads | `oltp_read_only` | 254.58ms | 185.02ms | 0.7× | 0.9% | PASS |
| file_writes | `oltp_bulk_insert` | 261.31ms | 353.14ms | 1.4× | 1.2% | PASS |
| file_writes | `oltp_insert` | 25.34ms | 43.01ms | 1.7× | 1.8% | PASS |
| file_writes | `oltp_update_index` | 115.48ms | 151.20ms | 1.3× | 10.1% | PASS |
| file_writes | `oltp_update_non_index` | 106.78ms | 95.54ms | 0.9× | 9.9% | PASS |
| file_writes | `oltp_delete_insert` | 96.04ms | 115.48ms | 1.2× | 1.9% | PASS |
| file_writes | `oltp_write_only` | 82.33ms | 69.28ms | 0.8× | 14.9% | PASS |
| file_writes | `types_delete_insert` | 71.49ms | 67.23ms | 0.9× | 1.2% | PASS |
| file_writes | `oltp_read_write` | 150.56ms | 154.78ms | 1.0× | 4.3% | PASS |
| ac_reads | `oltp_point_select` | 59.27ms | 61.08ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 19.91ms | 18.55ms | 0.9× | 1.4% | PASS |
| ac_reads | `oltp_sum_range` | 18.48ms | 17.80ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 3.73ms | 3.84ms | 1.0× | 1.7% | PASS |
| ac_reads | `oltp_distinct_range` | 4.77ms | 4.98ms | 1.0× | 1.5% | PASS |
| ac_reads | `oltp_index_scan` | 6.48ms | 8.96ms | 1.4× | 1.8% | PASS |
| ac_reads | `select_random_points` | 25.79ms | 26.17ms | 1.0× | 2.7% | PASS |
| ac_reads | `select_random_ranges` | 9.23ms | 9.29ms | 1.0× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.36ms | 13.28ms | 1.3× | 2.0% | PASS |
| ac_reads | `groupby_scan` | 34.67ms | 36.84ms | 1.1× | 1.0% | PASS |
| ac_reads | `index_join` | 12.43ms | 11.92ms | 1.0× | 2.7% | PASS |
| ac_reads | `index_join_scan` | 4.28ms | 7.07ms | 1.7× | 2.9% | PASS |
| ac_reads | `types_table_scan` | 1.09s | 1.30s | 1.2× | 1.4% | PASS |
| ac_reads | `table_scan` | 1.21s | 1.40s | 1.2× | 1.0% | PASS |
| ac_reads | `oltp_read_only` | 178.01ms | 182.62ms | 1.0× | 1.8% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.80ms | 69.80ms | 2.9× | 5.4% | PASS |
| ac_writes | `oltp_insert_ac` | 26.84ms | 83.35ms | 3.1× | 6.4% | PASS |
| ac_writes | `oltp_update_index_ac` | 29.31ms | 101.89ms | 3.5× | 7.8% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.82ms | 79.44ms | 3.2× | 9.7% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.79ms | 90.77ms | 3.4× | 6.9% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.83ms | 89.16ms | 3.3× | 6.3% | PASS |
| ac_writes | `types_delete_insert_ac` | 24.47ms | 80.94ms | 3.3× | 9.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 32.08ms | 94.38ms | 2.9× | 4.6% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 35.79ms | 40.97ms | 1.1× | 2.0% | PASS |
| mem_reads | `oltp_range_select` | 16.76ms | 16.22ms | 1.0× | 2.8% | PASS |
| mem_reads | `oltp_sum_range` | 15.38ms | 15.29ms | 1.0× | 2.0% | PASS |
| mem_reads | `oltp_order_range` | 3.29ms | 3.54ms | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_distinct_range` | 4.37ms | 4.66ms | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_index_scan` | 4.05ms | 6.89ms | 1.7× | 1.6% | PASS |
| mem_reads | `select_random_points` | 22.78ms | 22.89ms | 1.0× | 2.7% | PASS |
| mem_reads | `select_random_ranges` | 6.75ms | 7.00ms | 1.0× | 1.5% | PASS |
| mem_reads | `covering_index_scan` | 7.67ms | 11.11ms | 1.4× | 1.4% | PASS |
| mem_reads | `groupby_scan` | 33.79ms | 36.44ms | 1.1× | 0.7% | PASS |
| mem_reads | `index_join` | 10.89ms | 11.07ms | 1.0× | 2.6% | PASS |
| mem_reads | `index_join_scan` | 3.86ms | 6.76ms | 1.8× | 2.4% | PASS |
| mem_reads | `types_table_scan` | 1.06s | 1.29s | 1.2× | 1.5% | PASS |
| mem_reads | `table_scan` | 1.19s | 1.40s | 1.2× | 1.6% | PASS |
| mem_reads | `oltp_read_only` | 135.05ms | 149.31ms | 1.1× | 0.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 238.99ms | 337.93ms | 1.4× | 1.0% | PASS |
| mem_writes | `oltp_insert` | 18.32ms | 37.61ms | 2.1× | 0.8% | PASS |
| mem_writes | `oltp_update_index` | 65.64ms | 131.70ms | 2.0× | 3.0% | PASS |
| mem_writes | `oltp_update_non_index` | 50.09ms | 80.40ms | 1.6× | 3.6% | PASS |
| mem_writes | `oltp_delete_insert` | 53.34ms | 98.27ms | 1.8× | 2.7% | PASS |
| mem_writes | `oltp_write_only` | 28.25ms | 56.39ms | 2.0× | 1.8% | PASS |
| mem_writes | `types_delete_insert` | 38.41ms | 54.89ms | 1.4× | 2.1% | PASS |
| mem_writes | `oltp_read_write` | 96.21ms | 139.85ms | 1.5× | 2.5% | PASS |
| file_reads | `oltp_point_select` | 105.89ms | 59.99ms | 0.6× | 1.7% | PASS |
| file_reads | `oltp_range_select` | 23.59ms | 18.28ms | 0.8× | 1.5% | PASS |
| file_reads | `oltp_sum_range` | 22.61ms | 17.57ms | 0.8× | 1.8% | PASS |
| file_reads | `oltp_order_range` | 4.33ms | 3.83ms | 0.9× | 2.4% | PASS |
| file_reads | `oltp_distinct_range` | 5.40ms | 4.90ms | 0.9× | 1.8% | PASS |
| file_reads | `oltp_index_scan` | 11.08ms | 8.89ms | 0.8× | 1.8% | PASS |
| file_reads | `select_random_points` | 30.49ms | 25.83ms | 0.8× | 3.0% | PASS |
| file_reads | `select_random_ranges` | 14.00ms | 9.04ms | 0.6× | 1.3% | PASS |
| file_reads | `covering_index_scan` | 15.00ms | 13.10ms | 0.9× | 1.3% | PASS |
| file_reads | `groupby_scan` | 34.71ms | 36.74ms | 1.1× | 1.1% | PASS |
| file_reads | `index_join` | 14.47ms | 12.17ms | 0.8× | 2.2% | PASS |
| file_reads | `index_join_scan` | 4.77ms | 6.97ms | 1.5× | 1.9% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.30s | 1.2× | 1.4% | PASS |
| file_reads | `table_scan` | 1.19s | 1.40s | 1.2× | 1.4% | PASS |
| file_reads | `oltp_read_only` | 248.93ms | 181.22ms | 0.7× | 1.2% | PASS |
| file_writes | `oltp_bulk_insert` | 261.26ms | 353.22ms | 1.4× | 1.2% | PASS |
| file_writes | `oltp_insert` | 25.44ms | 42.85ms | 1.7× | 1.4% | PASS |
| file_writes | `oltp_update_index` | 97.53ms | 147.97ms | 1.5× | 1.9% | PASS |
| file_writes | `oltp_update_non_index` | 91.81ms | 93.23ms | 1.0× | 9.5% | PASS |
| file_writes | `oltp_delete_insert` | 86.82ms | 113.15ms | 1.3× | 1.9% | PASS |
| file_writes | `oltp_write_only` | 58.12ms | 69.26ms | 1.2× | 2.2% | PASS |
| file_writes | `types_delete_insert` | 63.61ms | 65.86ms | 1.0× | 1.8% | PASS |
| file_writes | `oltp_read_write` | 127.34ms | 153.04ms | 1.2× | 1.3% | PASS |
| ac_reads | `oltp_point_select` | 58.75ms | 59.75ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_range_select` | 19.19ms | 18.19ms | 0.9× | 1.7% | PASS |
| ac_reads | `oltp_sum_range` | 18.12ms | 17.46ms | 1.0× | 2.3% | PASS |
| ac_reads | `oltp_order_range` | 3.87ms | 3.80ms | 1.0× | 2.0% | PASS |
| ac_reads | `oltp_distinct_range` | 4.91ms | 4.92ms | 1.0× | 2.3% | PASS |
| ac_reads | `oltp_index_scan` | 6.55ms | 8.86ms | 1.4× | 1.6% | PASS |
| ac_reads | `select_random_points` | 25.37ms | 25.69ms | 1.0× | 2.1% | PASS |
| ac_reads | `select_random_ranges` | 9.23ms | 9.12ms | 1.0× | 1.3% | PASS |
| ac_reads | `covering_index_scan` | 10.52ms | 13.08ms | 1.2× | 1.4% | PASS |
| ac_reads | `groupby_scan` | 34.23ms | 36.70ms | 1.1× | 0.8% | PASS |
| ac_reads | `index_join` | 12.37ms | 12.24ms | 1.0× | 3.0% | PASS |
| ac_reads | `index_join_scan` | 4.24ms | 6.96ms | 1.6× | 2.1% | PASS |
| ac_reads | `types_table_scan` | 1.05s | 1.29s | 1.2× | 1.1% | PASS |
| ac_reads | `table_scan` | 1.19s | 1.40s | 1.2× | 1.0% | PASS |
| ac_reads | `oltp_read_only` | 174.42ms | 179.52ms | 1.0× | 1.3% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.24ms | 66.98ms | 2.9× | 5.9% | PASS |
| ac_writes | `oltp_insert_ac` | 25.28ms | 82.17ms | 3.3× | 6.3% | PASS |
| ac_writes | `oltp_update_index_ac` | 26.78ms | 97.03ms | 3.6× | 4.1% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 22.79ms | 75.91ms | 3.3× | 4.7% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.73ms | 87.36ms | 3.4× | 3.4% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.15ms | 85.42ms | 3.3× | 7.1% | PASS |
| ac_writes | `types_delete_insert_ac` | 24.11ms | 77.41ms | 3.2× | 6.4% | PASS |
| ac_writes | `oltp_read_write_ac` | 33.13ms | 93.46ms | 2.8× | 6.6% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.91ms | 42.86ms | 1.3× | 1.4% | PASS |
| mem_reads | `oltp_range_select` | 20.00ms | 24.29ms | 1.2× | 2.0% | PASS |
| mem_reads | `oltp_sum_range` | 17.91ms | 22.46ms | 1.3× | 1.0% | PASS |
| mem_reads | `oltp_order_range` | 3.55ms | 4.27ms | 1.2× | 1.4% | PASS |
| mem_reads | `oltp_distinct_range` | 4.65ms | 5.42ms | 1.2× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 4.55ms | 6.68ms | 1.5× | 1.4% | PASS |
| mem_reads | `select_random_points` | 28.07ms | 33.07ms | 1.2× | 1.5% | PASS |
| mem_reads | `select_random_ranges` | 7.56ms | 9.37ms | 1.2× | 1.8% | PASS |
| mem_reads | `covering_index_scan` | 7.82ms | 11.30ms | 1.4× | 1.3% | PASS |
| mem_reads | `groupby_scan` | 35.95ms | 42.34ms | 1.2× | 0.7% | PASS |
| mem_reads | `index_join` | 7.95ms | 12.44ms | 1.6× | 2.2% | PASS |
| mem_reads | `index_join_scan` | 3.90ms | 6.73ms | 1.7× | 2.3% | PASS |
| mem_reads | `types_table_scan` | 1.05s | 1.28s | 1.2× | 1.3% | PASS |
| mem_reads | `table_scan` | 1.22s | 1.40s | 1.1× | 2.9% | PASS |
| mem_reads | `oltp_read_only` | 153.32ms | 186.83ms | 1.2× | 1.7% | PASS |
| mem_writes | `oltp_bulk_insert` | 242.98ms | 327.41ms | 1.3× | 1.1% | PASS |
| mem_writes | `oltp_insert` | 18.56ms | 33.62ms | 1.8× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 66.03ms | 120.83ms | 1.8× | 1.1% | PASS |
| mem_writes | `oltp_update_non_index` | 50.86ms | 82.07ms | 1.6× | 1.8% | PASS |
| mem_writes | `oltp_delete_insert` | 49.53ms | 93.55ms | 1.9× | 1.3% | PASS |
| mem_writes | `oltp_write_only` | 27.24ms | 55.45ms | 2.0× | 1.9% | PASS |
| mem_writes | `types_delete_insert` | 33.21ms | 54.64ms | 1.6× | 1.7% | PASS |
| mem_writes | `oltp_read_write` | 102.93ms | 158.24ms | 1.5× | 2.2% | PASS |
| file_reads | `oltp_point_select` | 103.89ms | 61.77ms | 0.6× | 0.9% | PASS |
| file_reads | `oltp_range_select` | 27.10ms | 26.82ms | 1.0× | 1.6% | PASS |
| file_reads | `oltp_sum_range` | 25.80ms | 25.12ms | 1.0× | 2.1% | PASS |
| file_reads | `oltp_order_range` | 4.67ms | 4.75ms | 1.0× | 1.6% | PASS |
| file_reads | `oltp_distinct_range` | 5.71ms | 5.85ms | 1.0× | 2.2% | PASS |
| file_reads | `oltp_index_scan` | 11.44ms | 8.70ms | 0.8× | 1.5% | PASS |
| file_reads | `select_random_points` | 35.44ms | 36.37ms | 1.0× | 1.5% | PASS |
| file_reads | `select_random_ranges` | 14.88ms | 11.75ms | 0.8× | 1.7% | PASS |
| file_reads | `covering_index_scan` | 15.20ms | 13.43ms | 0.9× | 1.2% | PASS |
| file_reads | `groupby_scan` | 36.97ms | 43.09ms | 1.2× | 1.2% | PASS |
| file_reads | `index_join` | 12.63ms | 14.07ms | 1.1× | 1.6% | PASS |
| file_reads | `index_join_scan` | 4.97ms | 7.08ms | 1.4× | 3.0% | PASS |
| file_reads | `types_table_scan` | 1.13s | 1.29s | 1.1× | 2.9% | PASS |
| file_reads | `table_scan` | 1.39s | 1.41s | 1.0× | 2.3% | PASS |
| file_reads | `oltp_read_only` | 266.55ms | 221.79ms | 0.8× | 0.7% | PASS |
| file_writes | `oltp_bulk_insert` | 261.34ms | 340.70ms | 1.3× | 0.9% | PASS |
| file_writes | `oltp_insert` | 26.39ms | 38.37ms | 1.5× | 1.3% | PASS |
| file_writes | `oltp_update_index` | 97.54ms | 138.64ms | 1.4× | 1.9% | PASS |
| file_writes | `oltp_update_non_index` | 76.65ms | 93.18ms | 1.2× | 1.5% | PASS |
| file_writes | `oltp_delete_insert` | 75.01ms | 103.60ms | 1.4× | 1.7% | PASS |
| file_writes | `oltp_write_only` | 48.95ms | 63.58ms | 1.3× | 2.3% | PASS |
| file_writes | `types_delete_insert` | 51.03ms | 63.13ms | 1.2× | 2.1% | PASS |
| file_writes | `oltp_read_write` | 132.27ms | 172.69ms | 1.3× | 1.8% | PASS |
| ac_reads | `oltp_point_select` | 57.59ms | 62.80ms | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_range_select` | 23.23ms | 27.21ms | 1.2× | 1.6% | PASS |
| ac_reads | `oltp_sum_range` | 21.71ms | 25.65ms | 1.2× | 1.2% | PASS |
| ac_reads | `oltp_order_range` | 4.28ms | 4.82ms | 1.1× | 2.5% | PASS |
| ac_reads | `oltp_distinct_range` | 5.26ms | 5.90ms | 1.1× | 1.8% | PASS |
| ac_reads | `oltp_index_scan` | 7.57ms | 9.22ms | 1.2× | 1.4% | PASS |
| ac_reads | `select_random_points` | 34.09ms | 39.61ms | 1.2× | 1.6% | PASS |
| ac_reads | `select_random_ranges` | 10.69ms | 12.14ms | 1.1× | 1.7% | PASS |
| ac_reads | `covering_index_scan` | 10.86ms | 13.68ms | 1.3× | 1.6% | PASS |
| ac_reads | `groupby_scan` | 36.69ms | 43.20ms | 1.2× | 0.8% | PASS |
| ac_reads | `index_join` | 9.84ms | 13.87ms | 1.4× | 1.7% | PASS |
| ac_reads | `index_join_scan` | 4.50ms | 7.34ms | 1.6× | 1.9% | PASS |
| ac_reads | `types_table_scan` | 1.18s | 1.31s | 1.1× | 1.1% | PASS |
| ac_reads | `table_scan` | 1.39s | 1.42s | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_read_only` | 196.46ms | 223.26ms | 1.1× | 0.9% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 21.82ms | 65.52ms | 3.0× | 5.5% | PASS |
| ac_writes | `oltp_insert_ac` | 24.68ms | 81.09ms | 3.3× | 6.3% | PASS |
| ac_writes | `oltp_update_index_ac` | 26.42ms | 93.83ms | 3.6× | 4.6% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.94ms | 73.57ms | 3.1× | 6.7% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 24.08ms | 82.52ms | 3.4× | 4.1% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.79ms | 84.71ms | 3.3× | 5.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.84ms | 76.36ms | 3.5× | 7.5% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.37ms | 90.09ms | 2.9× | 3.8% | PASS |

</details>

</details>

## Version-control latency

Wall time: 6m 4s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 28.51ms | 38.00ms | 75.0% | 1.7% | PASS |
| `status_dirty_many_tables` | 30.19ms | 42.00ms | 71.9% | 1.8% | PASS |
| `diff_regular_working_one_table` | 24.10ms | 33.00ms | 73.0% | 2.2% | PASS |
| `diff_regular_working_many_tables` | 34.48ms | 50.00ms | 69.0% | 1.7% | PASS |
| `diff_stat_working_many_tables` | 34.54ms | 48.00ms | 72.0% | 1.6% | PASS |
| `diff_schema_working_many_tables` | 34.54ms | 48.00ms | 72.0% | 2.2% | PASS |
| `branch_list_many_branches` | 17.19ms | 25.00ms | 68.8% | 1.8% | PASS |
| `branch_create_delete` | 18.05ms | 27.00ms | 66.9% | 2.5% | PASS |
| `at_literal_deep_history` | 17.39ms | 28.00ms | 62.1% | 2.2% | PASS |
| `diff_literal_deep_history` | 17.54ms | 28.00ms | 62.7% | 2.2% | PASS |
| `history_literal_deep_history` | 18.59ms | 30.00ms | 62.0% | 2.4% | PASS |
| `checkout_branch_clean` | 25.66ms | 43.00ms | 59.7% | 2.2% | PASS |
| `merge_data_no_conflicts` | 20.57ms | 33.00ms | 62.3% | 2.5% | PASS |
| `merge_data_secondary_index` | 730.33ms | 944.00ms | 77.4% | 1.4% | PASS |
| `merge_schema_no_conflicts` | 17.12ms | 24.00ms | 71.4% | 3.5% | PASS |
| `merge_data_conflicts` | 21.84ms | 33.00ms | 66.2% | 1.5% | PASS |
| `merge_data_conflicts_with_resolve` | 23.29ms | 34.00ms | 68.5% | 1.6% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
