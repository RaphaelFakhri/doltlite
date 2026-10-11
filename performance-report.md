# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-11 11:10 UTC
>
> Commit: [`8bab4fb519e219c4209161d1a96f1a622ccd2dbd`](https://github.com/dolthub/doltlite/commit/8bab4fb519e219c4209161d1a96f1a622ccd2dbd)
>
> Runner: ubuntu24 20261004.327.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/38129049126)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 9.46s | 10.64s | 1.1× | 2.0% | **PASS** |
| Writes | 1.91s | 2.95s | 1.5× | 1.7% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.46s | 10.93s | 1.0× | 1.7% | **PASS** |
| Writes | 3.97s | 4.04s | 1.0× | 5.5% | **PASS** |
| Autocommit writes | 902.64ms | 3.07s | 3.4× | 7.6% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.48s | 2.90s | 1.2× | 1.6% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.67s | 3.21s | 1.2× | 2.3% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.47s | 2.64s | 1.1× | 1.9% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 1.83s | 1.90s | 1.0× | 2.4% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 441.88ms | 729.75ms | 1.7× | 1.6% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 614.18ms | 976.86ms | 1.6× | 1.7% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 489.02ms | 710.79ms | 1.5× | 2.2% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 366.76ms | 534.25ms | 1.5× | 1.8% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.72s | 2.98s | 1.1× | 1.4% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.94s | 3.28s | 1.1× | 1.8% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.71s | 2.70s | 1.0× | 1.4% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.10s | 1.98s | 0.9× | 2.4% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 593.13ms | 796.70ms | 1.3× | 2.1% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 940.24ms | 1.07s | 1.1× | 3.6% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 1.54s | 1.29s | 0.8× | 30.1% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 893.21ms | 875.05ms | 1.0× | 6.0% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.56s | 2.98s | 1.2× | 1.6% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.71s | 3.25s | 1.2× | 2.0% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.23s | 2.56s | 1.1× | 1.3% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 1.91s | 2.01s | 1.1× | 2.6% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 202.55ms | 649.88ms | 3.2× | 7.7% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 213.27ms | 666.62ms | 3.1× | 7.2% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 299.10ms | 1.21s | 4.0× | 69.6% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 187.72ms | 546.56ms | 2.9× | 5.3% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 23.68ms | 31.56ms | 1.3× | 2.1% | PASS |
| mem_reads | `oltp_range_select` | 10.31ms | 12.30ms | 1.2× | 2.2% | PASS |
| mem_reads | `oltp_sum_range` | 9.31ms | 11.93ms | 1.3× | 1.6% | PASS |
| mem_reads | `oltp_order_range` | 2.57ms | 3.02ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_distinct_range` | 3.63ms | 4.21ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_index_scan` | 3.88ms | 5.69ms | 1.5× | 1.8% | PASS |
| mem_reads | `select_random_points` | 10.15ms | 11.93ms | 1.2× | 3.7% | PASS |
| mem_reads | `select_random_ranges` | 4.63ms | 5.39ms | 1.2× | 1.4% | PASS |
| mem_reads | `covering_index_scan` | 7.62ms | 10.90ms | 1.4× | 1.1% | PASS |
| mem_reads | `groupby_scan` | 29.52ms | 33.55ms | 1.1× | 0.9% | PASS |
| mem_reads | `index_join` | 5.75ms | 8.82ms | 1.5× | 3.2% | PASS |
| mem_reads | `index_join_scan` | 3.20ms | 5.59ms | 1.7× | 1.8% | PASS |
| mem_reads | `types_table_scan` | 1.04s | 1.26s | 1.2× | 1.3% | PASS |
| mem_reads | `table_scan` | 1.22s | 1.37s | 1.1× | 2.5% | PASS |
| mem_reads | `oltp_read_only` | 103.63ms | 127.02ms | 1.2× | 1.5% | PASS |
| mem_writes | `oltp_bulk_insert` | 176.92ms | 282.68ms | 1.6× | 1.1% | PASS |
| mem_writes | `oltp_insert` | 15.05ms | 27.64ms | 1.8× | 1.2% | PASS |
| mem_writes | `oltp_update_index` | 49.66ms | 90.50ms | 1.8× | 1.5% | PASS |
| mem_writes | `oltp_update_non_index` | 34.83ms | 57.54ms | 1.7× | 1.7% | PASS |
| mem_writes | `oltp_delete_insert` | 47.48ms | 77.06ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_write_only` | 22.80ms | 45.67ms | 2.0× | 1.6% | PASS |
| mem_writes | `types_delete_insert` | 24.88ms | 38.45ms | 1.5× | 1.8% | PASS |
| mem_writes | `oltp_read_write` | 70.26ms | 110.20ms | 1.6× | 2.1% | PASS |
| file_reads | `oltp_point_select` | 92.84ms | 50.31ms | 0.5× | 0.8% | PASS |
| file_reads | `oltp_range_select` | 17.65ms | 14.36ms | 0.8× | 2.5% | PASS |
| file_reads | `oltp_sum_range` | 17.05ms | 14.13ms | 0.8× | 1.7% | PASS |
| file_reads | `oltp_order_range` | 3.41ms | 3.30ms | 1.0× | 1.5% | PASS |
| file_reads | `oltp_distinct_range` | 4.50ms | 4.49ms | 1.0× | 1.2% | PASS |
| file_reads | `oltp_index_scan` | 11.06ms | 7.78ms | 0.7× | 1.4% | PASS |
| file_reads | `select_random_points` | 17.36ms | 13.91ms | 0.8× | 2.2% | PASS |
| file_reads | `select_random_ranges` | 11.81ms | 7.41ms | 0.6× | 0.9% | PASS |
| file_reads | `covering_index_scan` | 15.26ms | 13.02ms | 0.9× | 1.0% | PASS |
| file_reads | `groupby_scan` | 30.68ms | 34.12ms | 1.1× | 0.8% | PASS |
| file_reads | `index_join` | 9.84ms | 10.35ms | 1.1× | 1.5% | PASS |
| file_reads | `index_join_scan` | 4.18ms | 5.97ms | 1.4× | 1.9% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.27s | 1.2× | 1.2% | PASS |
| file_reads | `table_scan` | 1.23s | 1.37s | 1.1× | 2.0% | PASS |
| file_reads | `oltp_read_only` | 209.70ms | 156.87ms | 0.7× | 1.1% | PASS |
| file_writes | `oltp_bulk_insert` | 192.01ms | 293.60ms | 1.5× | 1.6% | PASS |
| file_writes | `oltp_insert` | 22.00ms | 31.13ms | 1.4× | 2.2% | PASS |
| file_writes | `oltp_update_index` | 77.83ms | 104.81ms | 1.3× | 2.3% | PASS |
| file_writes | `oltp_update_non_index` | 58.46ms | 67.40ms | 1.2× | 2.0% | PASS |
| file_writes | `oltp_delete_insert` | 68.16ms | 85.83ms | 1.3× | 2.2% | PASS |
| file_writes | `oltp_write_only` | 44.62ms | 53.50ms | 1.2× | 2.4% | PASS |
| file_writes | `types_delete_insert` | 39.63ms | 43.93ms | 1.1× | 2.1% | PASS |
| file_writes | `oltp_read_write` | 90.42ms | 116.50ms | 1.3× | 1.8% | PASS |
| ac_reads | `oltp_point_select` | 47.03ms | 50.17ms | 1.1× | 1.5% | PASS |
| ac_reads | `oltp_range_select` | 13.02ms | 14.33ms | 1.1× | 1.9% | PASS |
| ac_reads | `oltp_sum_range` | 12.23ms | 14.10ms | 1.2× | 1.6% | PASS |
| ac_reads | `oltp_order_range` | 2.98ms | 3.29ms | 1.1× | 1.7% | PASS |
| ac_reads | `oltp_distinct_range` | 4.00ms | 4.50ms | 1.1× | 1.0% | PASS |
| ac_reads | `oltp_index_scan` | 6.64ms | 7.86ms | 1.2× | 1.4% | PASS |
| ac_reads | `select_random_points` | 13.14ms | 13.94ms | 1.1× | 3.1% | PASS |
| ac_reads | `select_random_ranges` | 7.13ms | 7.43ms | 1.0× | 0.9% | PASS |
| ac_reads | `covering_index_scan` | 10.37ms | 12.86ms | 1.2× | 1.2% | PASS |
| ac_reads | `groupby_scan` | 29.86ms | 33.88ms | 1.1× | 0.6% | PASS |
| ac_reads | `index_join` | 7.53ms | 10.35ms | 1.4× | 1.6% | PASS |
| ac_reads | `index_join_scan` | 3.68ms | 6.01ms | 1.6× | 2.0% | PASS |
| ac_reads | `types_table_scan` | 1.05s | 1.27s | 1.2× | 1.4% | PASS |
| ac_reads | `table_scan` | 1.22s | 1.37s | 1.1× | 2.3% | PASS |
| ac_reads | `oltp_read_only` | 138.22ms | 155.62ms | 1.1× | 1.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 21.34ms | 65.31ms | 3.1× | 6.5% | PASS |
| ac_writes | `oltp_insert_ac` | 25.07ms | 81.45ms | 3.2× | 7.5% | PASS |
| ac_writes | `oltp_update_index_ac` | 28.49ms | 95.70ms | 3.4× | 9.5% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.94ms | 74.73ms | 3.1× | 7.8% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.57ms | 85.70ms | 3.4× | 8.5% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.10ms | 84.92ms | 3.3× | 6.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 22.89ms | 74.23ms | 3.2× | 10.6% | PASS |
| ac_writes | `oltp_read_write_ac` | 29.15ms | 87.83ms | 3.0× | 5.6% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 36.60ms | 40.38ms | 1.1× | 2.3% | PASS |
| mem_reads | `oltp_range_select` | 17.36ms | 16.35ms | 0.9× | 2.7% | PASS |
| mem_reads | `oltp_sum_range` | 16.27ms | 15.37ms | 0.9× | 2.5% | PASS |
| mem_reads | `oltp_order_range` | 3.38ms | 3.53ms | 1.0× | 1.3% | PASS |
| mem_reads | `oltp_distinct_range` | 4.46ms | 4.72ms | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_index_scan` | 4.03ms | 6.80ms | 1.7× | 1.6% | PASS |
| mem_reads | `select_random_points` | 24.69ms | 23.71ms | 1.0× | 3.9% | PASS |
| mem_reads | `select_random_ranges` | 7.01ms | 7.02ms | 1.0× | 1.0% | PASS |
| mem_reads | `covering_index_scan` | 7.70ms | 11.03ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 34.90ms | 36.95ms | 1.1× | 0.7% | PASS |
| mem_reads | `index_join` | 11.06ms | 10.68ms | 1.0× | 2.5% | PASS |
| mem_reads | `index_join_scan` | 3.82ms | 6.61ms | 1.7× | 2.1% | PASS |
| mem_reads | `types_table_scan` | 1.11s | 1.40s | 1.3× | 3.7% | PASS |
| mem_reads | `table_scan` | 1.25s | 1.47s | 1.2× | 2.8% | PASS |
| mem_reads | `oltp_read_only` | 149.48ms | 153.69ms | 1.0× | 2.4% | PASS |
| mem_writes | `oltp_bulk_insert` | 237.08ms | 344.19ms | 1.5× | 0.8% | PASS |
| mem_writes | `oltp_insert` | 17.94ms | 38.97ms | 2.2× | 1.1% | PASS |
| mem_writes | `oltp_update_index` | 70.60ms | 139.93ms | 2.0× | 1.7% | PASS |
| mem_writes | `oltp_update_non_index` | 51.99ms | 84.37ms | 1.6× | 1.8% | PASS |
| mem_writes | `oltp_delete_insert` | 58.83ms | 106.21ms | 1.8× | 1.7% | PASS |
| mem_writes | `oltp_write_only` | 31.20ms | 60.17ms | 1.9× | 1.7% | PASS |
| mem_writes | `types_delete_insert` | 42.59ms | 58.60ms | 1.4× | 1.3% | PASS |
| mem_writes | `oltp_read_write` | 103.94ms | 144.43ms | 1.4× | 2.2% | PASS |
| file_reads | `oltp_point_select` | 108.43ms | 60.52ms | 0.6× | 1.3% | PASS |
| file_reads | `oltp_range_select` | 24.88ms | 18.46ms | 0.7× | 2.4% | PASS |
| file_reads | `oltp_sum_range` | 24.55ms | 17.81ms | 0.7× | 1.7% | PASS |
| file_reads | `oltp_order_range` | 4.36ms | 3.82ms | 0.9× | 2.0% | PASS |
| file_reads | `oltp_distinct_range` | 5.49ms | 5.00ms | 0.9× | 1.8% | PASS |
| file_reads | `oltp_index_scan` | 11.23ms | 8.97ms | 0.8× | 1.3% | PASS |
| file_reads | `select_random_points` | 33.22ms | 26.87ms | 0.8× | 2.1% | PASS |
| file_reads | `select_random_ranges` | 14.57ms | 9.29ms | 0.6× | 1.8% | PASS |
| file_reads | `covering_index_scan` | 15.14ms | 13.47ms | 0.9× | 1.1% | PASS |
| file_reads | `groupby_scan` | 35.64ms | 37.25ms | 1.0× | 0.9% | PASS |
| file_reads | `index_join` | 15.54ms | 12.01ms | 0.8× | 2.1% | PASS |
| file_reads | `index_join_scan` | 4.89ms | 7.05ms | 1.4× | 2.6% | PASS |
| file_reads | `types_table_scan` | 1.07s | 1.38s | 1.3× | 0.9% | PASS |
| file_reads | `table_scan` | 1.31s | 1.49s | 1.1× | 3.3% | PASS |
| file_reads | `oltp_read_only` | 253.48ms | 181.65ms | 0.7× | 1.3% | PASS |
| file_writes | `oltp_bulk_insert` | 264.30ms | 360.44ms | 1.4× | 1.1% | PASS |
| file_writes | `oltp_insert` | 25.67ms | 44.05ms | 1.7× | 2.2% | PASS |
| file_writes | `oltp_update_index` | 132.68ms | 155.13ms | 1.2× | 9.3% | PASS |
| file_writes | `oltp_update_non_index` | 100.70ms | 98.90ms | 1.0× | 4.9% | PASS |
| file_writes | `oltp_delete_insert` | 99.78ms | 119.93ms | 1.2× | 1.5% | PASS |
| file_writes | `oltp_write_only` | 89.99ms | 71.33ms | 0.8× | 9.5% | PASS |
| file_writes | `types_delete_insert` | 73.93ms | 68.51ms | 0.9× | 2.3% | PASS |
| file_writes | `oltp_read_write` | 153.19ms | 155.75ms | 1.0× | 7.0% | PASS |
| ac_reads | `oltp_point_select` | 59.20ms | 59.77ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 19.81ms | 18.49ms | 0.9× | 2.8% | PASS |
| ac_reads | `oltp_sum_range` | 19.12ms | 17.70ms | 0.9× | 2.8% | PASS |
| ac_reads | `oltp_order_range` | 3.88ms | 3.83ms | 1.0× | 3.4% | PASS |
| ac_reads | `oltp_distinct_range` | 4.94ms | 5.01ms | 1.0× | 2.0% | PASS |
| ac_reads | `oltp_index_scan` | 6.58ms | 8.95ms | 1.4× | 2.6% | PASS |
| ac_reads | `select_random_points` | 27.31ms | 26.70ms | 1.0× | 2.0% | PASS |
| ac_reads | `select_random_ranges` | 9.58ms | 9.23ms | 1.0× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.40ms | 13.39ms | 1.3× | 1.7% | PASS |
| ac_reads | `groupby_scan` | 34.78ms | 37.07ms | 1.1× | 0.7% | PASS |
| ac_reads | `index_join` | 12.81ms | 11.97ms | 0.9× | 3.0% | PASS |
| ac_reads | `index_join_scan` | 4.36ms | 7.04ms | 1.6× | 2.2% | PASS |
| ac_reads | `types_table_scan` | 1.10s | 1.39s | 1.3× | 2.9% | PASS |
| ac_reads | `table_scan` | 1.22s | 1.46s | 1.2× | 2.0% | PASS |
| ac_reads | `oltp_read_only` | 174.50ms | 179.30ms | 1.0× | 1.6% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.98ms | 67.87ms | 2.8× | 6.8% | PASS |
| ac_writes | `oltp_insert_ac` | 28.29ms | 81.68ms | 2.9× | 7.9% | PASS |
| ac_writes | `oltp_update_index_ac` | 28.13ms | 96.99ms | 3.4× | 6.6% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.05ms | 76.77ms | 3.3× | 7.6% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.08ms | 87.73ms | 3.4× | 8.5% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.36ms | 83.69ms | 3.3× | 5.6% | PASS |
| ac_writes | `types_delete_insert_ac` | 23.81ms | 76.33ms | 3.2× | 8.3% | PASS |
| ac_writes | `oltp_read_write_ac` | 34.57ms | 95.55ms | 2.8× | 6.5% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 32.52ms | 31.92ms | 1.0× | 2.0% | PASS |
| mem_reads | `oltp_range_select` | 16.48ms | 12.93ms | 0.8× | 1.9% | PASS |
| mem_reads | `oltp_sum_range` | 13.42ms | 12.10ms | 0.9× | 2.3% | PASS |
| mem_reads | `oltp_order_range` | 2.99ms | 2.84ms | 1.0× | 1.2% | PASS |
| mem_reads | `oltp_distinct_range` | 3.79ms | 3.76ms | 1.0× | 1.0% | PASS |
| mem_reads | `oltp_index_scan` | 3.33ms | 5.39ms | 1.6× | 1.5% | PASS |
| mem_reads | `select_random_points` | 18.84ms | 17.97ms | 1.0× | 3.8% | PASS |
| mem_reads | `select_random_ranges` | 5.60ms | 5.20ms | 0.9× | 2.8% | PASS |
| mem_reads | `covering_index_scan` | 5.91ms | 8.29ms | 1.4× | 1.0% | PASS |
| mem_reads | `groupby_scan` | 29.70ms | 30.03ms | 1.0× | 1.1% | PASS |
| mem_reads | `index_join` | 9.26ms | 8.97ms | 1.0× | 3.0% | PASS |
| mem_reads | `index_join_scan` | 3.30ms | 6.03ms | 1.8× | 4.1% | PASS |
| mem_reads | `types_table_scan` | 995.29ms | 1.14s | 1.1× | 2.9% | PASS |
| mem_reads | `table_scan` | 1.20s | 1.24s | 1.0× | 0.9% | PASS |
| mem_reads | `oltp_read_only` | 133.34ms | 121.71ms | 0.9× | 1.3% | PASS |
| mem_writes | `oltp_bulk_insert` | 184.01ms | 246.81ms | 1.3× | 1.0% | PASS |
| mem_writes | `oltp_insert` | 14.44ms | 28.98ms | 2.0× | 1.0% | PASS |
| mem_writes | `oltp_update_index` | 57.20ms | 105.64ms | 1.8× | 2.2% | PASS |
| mem_writes | `oltp_update_non_index` | 43.19ms | 61.20ms | 1.4× | 2.5% | PASS |
| mem_writes | `oltp_delete_insert` | 46.13ms | 77.16ms | 1.7× | 1.6% | PASS |
| mem_writes | `oltp_write_only` | 24.26ms | 43.59ms | 1.8× | 2.9% | PASS |
| mem_writes | `types_delete_insert` | 33.15ms | 41.76ms | 1.3× | 2.2% | PASS |
| mem_writes | `oltp_read_write` | 86.66ms | 105.64ms | 1.2× | 2.2% | PASS |
| file_reads | `oltp_point_select` | 98.37ms | 48.28ms | 0.5× | 1.6% | PASS |
| file_reads | `oltp_range_select` | 22.59ms | 14.27ms | 0.6× | 1.6% | PASS |
| file_reads | `oltp_sum_range` | 21.07ms | 14.19ms | 0.7× | 1.4% | PASS |
| file_reads | `oltp_order_range` | 3.75ms | 3.03ms | 0.8× | 0.9% | PASS |
| file_reads | `oltp_distinct_range` | 4.53ms | 3.96ms | 0.9× | 1.1% | PASS |
| file_reads | `oltp_index_scan` | 10.26ms | 7.24ms | 0.7× | 1.5% | PASS |
| file_reads | `select_random_points` | 27.11ms | 20.48ms | 0.8× | 2.9% | PASS |
| file_reads | `select_random_ranges` | 12.50ms | 7.11ms | 0.6× | 1.1% | PASS |
| file_reads | `covering_index_scan` | 12.99ms | 10.15ms | 0.8× | 1.0% | PASS |
| file_reads | `groupby_scan` | 30.52ms | 30.39ms | 1.0× | 0.6% | PASS |
| file_reads | `index_join` | 13.52ms | 9.87ms | 0.7× | 1.7% | PASS |
| file_reads | `index_join_scan` | 4.06ms | 6.04ms | 1.5× | 2.2% | PASS |
| file_reads | `types_table_scan` | 1.01s | 1.15s | 1.1× | 2.0% | PASS |
| file_reads | `table_scan` | 1.20s | 1.23s | 1.0× | 1.1% | PASS |
| file_reads | `oltp_read_only` | 225.27ms | 143.98ms | 0.6× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 266.77ms | 335.50ms | 1.3× | 10.7% | PASS |
| file_writes | `oltp_insert` | 49.08ms | 60.00ms | 1.2× | 42.9% | PASS |
| file_writes | `oltp_update_index` | 225.40ms | 197.85ms | 0.9× | 29.8% | PASS |
| file_writes | `oltp_update_non_index` | 210.77ms | 137.43ms | 0.7× | 30.3% | PASS |
| file_writes | `oltp_delete_insert` | 231.23ms | 156.58ms | 0.7× | 21.0% | PASS |
| file_writes | `oltp_write_only` | 169.68ms | 119.26ms | 0.7× | 30.8% | PASS |
| file_writes | `types_delete_insert` | 141.13ms | 105.67ms | 0.7× | 28.3% | PASS |
| file_writes | `oltp_read_write` | 248.57ms | 177.35ms | 0.7× | 32.0% | PASS |
| ac_reads | `oltp_point_select` | 49.44ms | 46.48ms | 0.9× | 1.1% | PASS |
| ac_reads | `oltp_range_select` | 17.19ms | 13.89ms | 0.8× | 2.3% | PASS |
| ac_reads | `oltp_sum_range` | 15.63ms | 13.64ms | 0.9× | 2.2% | PASS |
| ac_reads | `oltp_order_range` | 3.32ms | 3.00ms | 0.9× | 1.2% | PASS |
| ac_reads | `oltp_distinct_range` | 4.07ms | 3.92ms | 1.0× | 0.9% | PASS |
| ac_reads | `oltp_index_scan` | 5.79ms | 7.07ms | 1.2× | 1.2% | PASS |
| ac_reads | `select_random_points` | 20.60ms | 19.35ms | 0.9× | 2.2% | PASS |
| ac_reads | `select_random_ranges` | 7.95ms | 6.96ms | 0.9× | 1.3% | PASS |
| ac_reads | `covering_index_scan` | 8.61ms | 10.12ms | 1.2× | 1.2% | PASS |
| ac_reads | `groupby_scan` | 29.89ms | 30.27ms | 1.0× | 0.6% | PASS |
| ac_reads | `index_join` | 11.04ms | 9.67ms | 0.9× | 1.6% | PASS |
| ac_reads | `index_join_scan` | 3.64ms | 5.86ms | 1.6× | 3.1% | PASS |
| ac_reads | `types_table_scan` | 905.77ms | 1.09s | 1.2× | 1.8% | PASS |
| ac_reads | `table_scan` | 1.00s | 1.16s | 1.2× | 1.4% | PASS |
| ac_reads | `oltp_read_only` | 141.16ms | 135.41ms | 1.0× | 1.1% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 34.46ms | 139.53ms | 4.0× | 67.9% | PASS |
| ac_writes | `oltp_insert_ac` | 37.56ms | 229.51ms | 6.1× (paired 3.7×) | 70.8% | PASS |
| ac_writes | `oltp_update_index_ac` | 41.45ms | 195.91ms | 4.7× | 77.3% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.28ms | 72.67ms | 3.4× | 25.6% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 28.70ms | 127.56ms | 4.4× | 83.2% | PASS |
| ac_writes | `oltp_write_only_ac` | 31.40ms | 126.22ms | 4.0× | 63.8% | PASS |
| ac_writes | `types_delete_insert_ac` | 48.60ms | 150.38ms | 3.1× | 68.5% | PASS |
| ac_writes | `oltp_read_write_ac` | 55.65ms | 163.54ms | 2.9× | 72.2% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 22.07ms | 24.05ms | 1.1× | 2.8% | PASS |
| mem_reads | `oltp_range_select` | 11.88ms | 12.06ms | 1.0× | 2.6% | PASS |
| mem_reads | `oltp_sum_range` | 10.86ms | 11.92ms | 1.1× | 1.7% | PASS |
| mem_reads | `oltp_order_range` | 2.26ms | 2.29ms | 1.0× | 2.4% | PASS |
| mem_reads | `oltp_distinct_range` | 2.75ms | 2.74ms | 1.0× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 2.98ms | 4.16ms | 1.4× | 2.0% | PASS |
| mem_reads | `select_random_points` | 18.47ms | 20.09ms | 1.1× | 1.5% | PASS |
| mem_reads | `select_random_ranges` | 4.71ms | 5.24ms | 1.1× | 1.3% | PASS |
| mem_reads | `covering_index_scan` | 4.09ms | 6.03ms | 1.5× | 2.0% | PASS |
| mem_reads | `groupby_scan` | 21.39ms | 22.91ms | 1.1× | 1.8% | PASS |
| mem_reads | `index_join` | 4.94ms | 7.73ms | 1.6× | 5.2% | PASS |
| mem_reads | `index_join_scan` | 2.67ms | 4.30ms | 1.6× | 3.2% | PASS |
| mem_reads | `types_table_scan` | 704.61ms | 778.55ms | 1.1× | 2.4% | PASS |
| mem_reads | `table_scan` | 933.31ms | 902.16ms | 1.0× | 5.3% | PASS |
| mem_reads | `oltp_read_only` | 87.83ms | 92.53ms | 1.1× | 3.5% | PASS |
| mem_writes | `oltp_bulk_insert` | 142.26ms | 179.19ms | 1.3× | 1.2% | PASS |
| mem_writes | `oltp_insert` | 11.04ms | 19.44ms | 1.8× | 1.6% | PASS |
| mem_writes | `oltp_update_index` | 43.57ms | 75.06ms | 1.7× | 1.9% | PASS |
| mem_writes | `oltp_update_non_index` | 33.39ms | 48.84ms | 1.5× | 1.8% | PASS |
| mem_writes | `oltp_delete_insert` | 33.29ms | 58.30ms | 1.8× | 1.9% | PASS |
| mem_writes | `oltp_write_only` | 17.91ms | 33.97ms | 1.9× | 1.2% | PASS |
| mem_writes | `types_delete_insert` | 22.21ms | 32.88ms | 1.5× | 1.9% | PASS |
| mem_writes | `oltp_read_write` | 63.08ms | 86.56ms | 1.4× | 2.3% | PASS |
| file_reads | `oltp_point_select` | 76.43ms | 40.33ms | 0.5× | 1.9% | PASS |
| file_reads | `oltp_range_select` | 16.95ms | 13.44ms | 0.8× | 2.3% | PASS |
| file_reads | `oltp_sum_range` | 16.43ms | 13.59ms | 0.8× | 2.7% | PASS |
| file_reads | `oltp_order_range` | 2.89ms | 2.48ms | 0.9× | 2.4% | PASS |
| file_reads | `oltp_distinct_range` | 3.43ms | 3.03ms | 0.9× | 1.5% | PASS |
| file_reads | `oltp_index_scan` | 8.71ms | 5.80ms | 0.7× | 2.1% | PASS |
| file_reads | `select_random_points` | 22.79ms | 21.57ms | 0.9× | 3.2% | PASS |
| file_reads | `select_random_ranges` | 10.04ms | 6.72ms | 0.7× | 1.8% | PASS |
| file_reads | `covering_index_scan` | 9.80ms | 7.48ms | 0.8× | 2.5% | PASS |
| file_reads | `groupby_scan` | 20.73ms | 21.77ms | 1.1× | 1.9% | PASS |
| file_reads | `index_join` | 7.88ms | 8.16ms | 1.0× | 2.7% | PASS |
| file_reads | `index_join_scan` | 3.42ms | 4.87ms | 1.4× | 6.5% | PASS |
| file_reads | `types_table_scan` | 743.85ms | 806.06ms | 1.1× | 4.6% | PASS |
| file_reads | `table_scan` | 989.31ms | 911.66ms | 0.9× | 3.1% | PASS |
| file_reads | `oltp_read_only` | 170.28ms | 116.34ms | 0.7× | 2.0% | PASS |
| file_writes | `oltp_bulk_insert` | 199.99ms | 236.77ms | 1.2× | 2.2% | PASS |
| file_writes | `oltp_insert` | 19.57ms | 29.81ms | 1.5× | 9.7% | PASS |
| file_writes | `oltp_update_index` | 141.36ms | 133.46ms | 0.9× | 5.2% | PASS |
| file_writes | `oltp_update_non_index` | 120.81ms | 94.58ms | 0.8× | 4.0% | PASS |
| file_writes | `oltp_delete_insert` | 113.63ms | 99.61ms | 0.9× | 5.7% | PASS |
| file_writes | `oltp_write_only` | 87.72ms | 79.00ms | 0.9× | 6.3% | PASS |
| file_writes | `types_delete_insert` | 78.29ms | 69.51ms | 0.9× | 9.2% | PASS |
| file_writes | `oltp_read_write` | 131.84ms | 132.30ms | 1.0× | 6.5% | PASS |
| ac_reads | `oltp_point_select` | 40.55ms | 40.84ms | 1.0× | 1.9% | PASS |
| ac_reads | `oltp_range_select` | 13.79ms | 13.46ms | 1.0× | 3.1% | PASS |
| ac_reads | `oltp_sum_range` | 12.20ms | 13.28ms | 1.1× | 2.6% | PASS |
| ac_reads | `oltp_order_range` | 2.53ms | 2.45ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_distinct_range` | 3.12ms | 2.97ms | 1.0× | 1.6% | PASS |
| ac_reads | `oltp_index_scan` | 5.37ms | 6.01ms | 1.1× | 2.0% | PASS |
| ac_reads | `select_random_points` | 21.51ms | 24.30ms | 1.1× | 4.3% | PASS |
| ac_reads | `select_random_ranges` | 6.86ms | 7.04ms | 1.0× | 1.8% | PASS |
| ac_reads | `covering_index_scan` | 6.70ms | 7.89ms | 1.2× | 2.5% | PASS |
| ac_reads | `groupby_scan` | 21.77ms | 23.11ms | 1.1× | 2.8% | PASS |
| ac_reads | `index_join` | 6.54ms | 9.01ms | 1.4× | 5.0% | PASS |
| ac_reads | `index_join_scan` | 3.17ms | 5.07ms | 1.6× | 5.9% | PASS |
| ac_reads | `types_table_scan` | 719.57ms | 812.01ms | 1.1× | 1.4% | PASS |
| ac_reads | `table_scan` | 918.68ms | 920.78ms | 1.0× | 7.9% | PASS |
| ac_reads | `oltp_read_only` | 125.06ms | 120.64ms | 1.0× | 4.2% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 21.37ms | 59.30ms | 2.8× | 3.7% | PASS |
| ac_writes | `oltp_insert_ac` | 24.42ms | 71.48ms | 2.9× | 4.9% | PASS |
| ac_writes | `oltp_update_index_ac` | 26.14ms | 76.13ms | 2.9× | 6.0% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.04ms | 63.91ms | 3.0× | 5.6% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 23.09ms | 70.42ms | 3.1× | 4.5% | PASS |
| ac_writes | `oltp_write_only_ac` | 23.16ms | 70.45ms | 3.0× | 3.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.97ms | 59.79ms | 2.7× | 7.4% | PASS |
| ac_writes | `oltp_read_write_ac` | 26.54ms | 75.08ms | 2.8× | 6.4% | PASS |

</details>

</details>

## Version-control latency

Wall time: 5m 41s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 25.30ms | 38.00ms | 66.6% | 1.8% | PASS |
| `status_dirty_many_tables` | 27.66ms | 42.00ms | 65.8% | 2.0% | PASS |
| `diff_regular_working_one_table` | 21.34ms | 33.00ms | 64.7% | 2.0% | PASS |
| `diff_regular_working_many_tables` | 29.45ms | 50.00ms | 58.9% | 2.6% | PASS |
| `diff_stat_working_many_tables` | 28.00ms | 48.00ms | 58.3% | 1.1% | PASS |
| `diff_schema_working_many_tables` | 28.40ms | 48.00ms | 59.2% | 1.2% | PASS |
| `branch_list_many_branches` | 15.99ms | 25.00ms | 64.0% | 2.8% | PASS |
| `branch_create_delete` | 17.82ms | 27.00ms | 66.0% | 3.6% | PASS |
| `at_literal_deep_history` | 17.35ms | 28.00ms | 62.0% | 1.4% | PASS |
| `diff_literal_deep_history` | 17.46ms | 28.00ms | 62.3% | 1.4% | PASS |
| `history_literal_deep_history` | 18.53ms | 30.00ms | 61.8% | 1.8% | PASS |
| `checkout_branch_clean` | 23.32ms | 43.00ms | 54.2% | 1.0% | PASS |
| `merge_data_no_conflicts` | 20.37ms | 33.00ms | 61.7% | 2.1% | PASS |
| `merge_data_secondary_index` | 641.89ms | 944.00ms | 68.0% | 3.0% | PASS |
| `merge_schema_no_conflicts` | 15.87ms | 24.00ms | 66.1% | 2.7% | PASS |
| `merge_data_conflicts` | 22.11ms | 33.00ms | 67.0% | 2.2% | PASS |
| `merge_data_conflicts_with_resolve` | 23.20ms | 34.00ms | 68.2% | 2.1% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
