# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-24 11:10 UTC
>
> Commit: [`54cbfac15c3a62aa742097560fe0361f9723e957`](https://github.com/dolthub/doltlite/commit/54cbfac15c3a62aa742097560fe0361f9723e957)
>
> Runner: ubuntu24 20261023.334.3
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/40082587585)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.25s | 12.00s | 1.2× | 1.6% | **PASS** |
| Writes | 2.12s | 3.31s | 1.6× | 1.4% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.88s | 12.15s | 1.1× | 1.3% | **PASS** |
| Writes | 3.57s | 4.08s | 1.1× | 1.9% | **PASS** |
| Autocommit writes | 792.71ms | 2.57s | 3.2× | 7.4% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.52s | 2.94s | 1.2× | 1.5% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.42s | 2.57s | 1.1× | 1.9% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.75s | 3.26s | 1.2× | 2.0% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.56s | 3.23s | 1.3× | 1.2% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 435.74ms | 728.37ms | 1.7× | 1.4% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 471.17ms | 686.96ms | 1.5× | 2.5% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 630.22ms | 979.90ms | 1.6× | 1.8% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 586.37ms | 914.34ms | 1.6× | 1.2% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.75s | 2.99s | 1.1× | 1.6% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.42s | 2.57s | 1.1× | 1.0% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.93s | 3.30s | 1.1× | 1.6% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.78s | 3.29s | 1.2× | 1.0% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 601.86ms | 806.60ms | 1.3× | 1.8% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 1.37s | 1.21s | 0.9× | 12.5% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 840.47ms | 1.06s | 1.3× | 2.0% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 759.11ms | 999.48ms | 1.3× | 1.6% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.62s | 3.00s | 1.1× | 1.7% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.22s | 2.53s | 1.1× | 0.7% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.90s | 3.36s | 1.2× | 1.7% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.63s | 3.29s | 1.3× | 1.3% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 202.14ms | 667.53ms | 3.3× | 7.6% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 164.85ms | 539.29ms | 3.3× | 11.8% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 221.24ms | 704.90ms | 3.2× | 7.3% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 204.48ms | 659.08ms | 3.2× | 5.8% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 23.94ms | 32.50ms | 1.4× | 1.6% | PASS |
| mem_reads | `oltp_range_select` | 10.47ms | 12.72ms | 1.2× | 2.3% | PASS |
| mem_reads | `oltp_sum_range` | 9.50ms | 12.33ms | 1.3× | 1.5% | PASS |
| mem_reads | `oltp_order_range` | 2.60ms | 3.02ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_distinct_range` | 3.67ms | 4.12ms | 1.1× | 1.2% | PASS |
| mem_reads | `oltp_index_scan` | 3.86ms | 5.87ms | 1.5× | 1.1% | PASS |
| mem_reads | `select_random_points` | 9.91ms | 12.60ms | 1.3× | 2.6% | PASS |
| mem_reads | `select_random_ranges` | 4.62ms | 5.49ms | 1.2× | 1.8% | PASS |
| mem_reads | `covering_index_scan` | 7.59ms | 11.02ms | 1.5× | 1.0% | PASS |
| mem_reads | `groupby_scan` | 29.92ms | 34.06ms | 1.1× | 0.7% | PASS |
| mem_reads | `index_join` | 5.83ms | 9.40ms | 1.6× | 1.6% | PASS |
| mem_reads | `index_join_scan` | 3.28ms | 5.85ms | 1.8× | 2.6% | PASS |
| mem_reads | `types_table_scan` | 1.08s | 1.29s | 1.2× | 1.2% | PASS |
| mem_reads | `table_scan` | 1.23s | 1.37s | 1.1× | 1.9% | PASS |
| mem_reads | `oltp_read_only` | 103.09ms | 126.59ms | 1.2× | 1.3% | PASS |
| mem_writes | `oltp_bulk_insert` | 178.65ms | 289.83ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_insert` | 15.07ms | 27.01ms | 1.8× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 49.71ms | 90.23ms | 1.8× | 1.3% | PASS |
| mem_writes | `oltp_update_non_index` | 34.11ms | 57.83ms | 1.7× | 1.2% | PASS |
| mem_writes | `oltp_delete_insert` | 44.92ms | 73.06ms | 1.6× | 1.5% | PASS |
| mem_writes | `oltp_write_only` | 22.22ms | 44.14ms | 2.0× | 1.8% | PASS |
| mem_writes | `types_delete_insert` | 24.64ms | 38.36ms | 1.6× | 2.5% | PASS |
| mem_writes | `oltp_read_write` | 66.41ms | 107.92ms | 1.6× | 1.4% | PASS |
| file_reads | `oltp_point_select` | 92.64ms | 51.15ms | 0.6× | 0.8% | PASS |
| file_reads | `oltp_range_select` | 17.33ms | 14.73ms | 0.9× | 1.9% | PASS |
| file_reads | `oltp_sum_range` | 17.11ms | 14.65ms | 0.9× | 2.5% | PASS |
| file_reads | `oltp_order_range` | 3.43ms | 3.31ms | 1.0× | 1.6% | PASS |
| file_reads | `oltp_distinct_range` | 4.55ms | 4.49ms | 1.0× | 1.3% | PASS |
| file_reads | `oltp_index_scan` | 11.35ms | 8.12ms | 0.7× | 1.7% | PASS |
| file_reads | `select_random_points` | 18.08ms | 15.21ms | 0.8× | 1.9% | PASS |
| file_reads | `select_random_ranges` | 12.27ms | 7.71ms | 0.6× | 1.3% | PASS |
| file_reads | `covering_index_scan` | 15.61ms | 13.60ms | 0.9× | 1.6% | PASS |
| file_reads | `groupby_scan` | 30.73ms | 34.53ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 9.93ms | 10.70ms | 1.1× | 1.9% | PASS |
| file_reads | `index_join_scan` | 4.19ms | 6.15ms | 1.5× | 2.5% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.28s | 1.2× | 0.9% | PASS |
| file_reads | `table_scan` | 1.26s | 1.37s | 1.1× | 1.8% | PASS |
| file_reads | `oltp_read_only` | 208.54ms | 156.05ms | 0.7× | 1.2% | PASS |
| file_writes | `oltp_bulk_insert` | 191.89ms | 297.43ms | 1.6× | 1.5% | PASS |
| file_writes | `oltp_insert` | 21.43ms | 30.14ms | 1.4× | 1.5% | PASS |
| file_writes | `oltp_update_index` | 77.12ms | 103.23ms | 1.3× | 2.3% | PASS |
| file_writes | `oltp_update_non_index` | 59.15ms | 69.51ms | 1.2× | 1.8% | PASS |
| file_writes | `oltp_delete_insert` | 67.66ms | 85.23ms | 1.3× | 2.0% | PASS |
| file_writes | `oltp_write_only` | 44.78ms | 53.24ms | 1.2× | 2.7% | PASS |
| file_writes | `types_delete_insert` | 41.12ms | 45.01ms | 1.1× | 1.8% | PASS |
| file_writes | `oltp_read_write` | 98.72ms | 122.81ms | 1.2× | 1.7% | PASS |
| ac_reads | `oltp_point_select` | 48.15ms | 52.32ms | 1.1× | 1.6% | PASS |
| ac_reads | `oltp_range_select` | 13.88ms | 15.03ms | 1.1× | 2.9% | PASS |
| ac_reads | `oltp_sum_range` | 12.94ms | 15.03ms | 1.2× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 3.27ms | 3.53ms | 1.1× | 3.0% | PASS |
| ac_reads | `oltp_distinct_range` | 4.37ms | 4.67ms | 1.1× | 1.8% | PASS |
| ac_reads | `oltp_index_scan` | 6.93ms | 8.42ms | 1.2× | 1.7% | PASS |
| ac_reads | `select_random_points` | 13.99ms | 16.06ms | 1.1× | 3.1% | PASS |
| ac_reads | `select_random_ranges` | 7.50ms | 7.76ms | 1.0× | 1.6% | PASS |
| ac_reads | `covering_index_scan` | 10.79ms | 13.72ms | 1.3× | 1.8% | PASS |
| ac_reads | `groupby_scan` | 30.31ms | 34.64ms | 1.1× | 0.6% | PASS |
| ac_reads | `index_join` | 7.67ms | 10.98ms | 1.4× | 2.3% | PASS |
| ac_reads | `index_join_scan` | 3.79ms | 6.27ms | 1.7× | 3.2% | PASS |
| ac_reads | `types_table_scan` | 1.06s | 1.28s | 1.2× | 0.7% | PASS |
| ac_reads | `table_scan` | 1.25s | 1.37s | 1.1× | 1.4% | PASS |
| ac_reads | `oltp_read_only` | 141.76ms | 157.16ms | 1.1× | 1.5% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 22.62ms | 70.04ms | 3.1× | 8.7% | PASS |
| ac_writes | `oltp_insert_ac` | 25.30ms | 81.31ms | 3.2× | 8.4% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.88ms | 93.56ms | 3.4× | 8.0% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.78ms | 74.39ms | 3.1× | 7.7% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 24.80ms | 85.90ms | 3.5× | 7.4% | PASS |
| ac_writes | `oltp_write_only_ac` | 24.45ms | 84.31ms | 3.4× | 4.9% | PASS |
| ac_writes | `types_delete_insert_ac` | 22.20ms | 77.42ms | 3.5× | 7.1% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.12ms | 100.59ms | 3.2× | 6.3% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.05ms | 30.85ms | 0.9× | 3.3% | PASS |
| mem_reads | `oltp_range_select` | 16.45ms | 12.81ms | 0.8× | 1.8% | PASS |
| mem_reads | `oltp_sum_range` | 13.59ms | 12.28ms | 0.9× | 3.6% | PASS |
| mem_reads | `oltp_order_range` | 2.93ms | 2.80ms | 1.0× | 1.9% | PASS |
| mem_reads | `oltp_distinct_range` | 3.70ms | 3.64ms | 1.0× | 1.6% | PASS |
| mem_reads | `oltp_index_scan` | 3.25ms | 5.17ms | 1.6× | 1.1% | PASS |
| mem_reads | `select_random_points` | 18.67ms | 17.76ms | 1.0× | 5.0% | PASS |
| mem_reads | `select_random_ranges` | 5.41ms | 5.20ms | 1.0× | 1.9% | PASS |
| mem_reads | `covering_index_scan` | 5.90ms | 8.20ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 29.36ms | 29.58ms | 1.0× | 0.9% | PASS |
| mem_reads | `index_join` | 9.07ms | 8.36ms | 0.9× | 2.7% | PASS |
| mem_reads | `index_join_scan` | 3.16ms | 5.56ms | 1.8× | 2.2% | PASS |
| mem_reads | `types_table_scan` | 936.80ms | 1.11s | 1.2× | 4.6% | PASS |
| mem_reads | `table_scan` | 1.22s | 1.21s | 1.0× | 3.3% | PASS |
| mem_reads | `oltp_read_only` | 114.23ms | 110.85ms | 1.0× | 1.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 182.70ms | 246.20ms | 1.3× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 14.08ms | 27.33ms | 1.9× | 1.4% | PASS |
| mem_writes | `oltp_update_index` | 55.26ms | 99.04ms | 1.8× | 2.6% | PASS |
| mem_writes | `oltp_update_non_index` | 40.38ms | 59.71ms | 1.5× | 2.6% | PASS |
| mem_writes | `oltp_delete_insert` | 44.89ms | 73.01ms | 1.6× | 1.1% | PASS |
| mem_writes | `oltp_write_only` | 23.10ms | 40.59ms | 1.8× | 2.5% | PASS |
| mem_writes | `types_delete_insert` | 31.58ms | 39.81ms | 1.3× | 2.7% | PASS |
| mem_writes | `oltp_read_write` | 79.19ms | 101.27ms | 1.3× | 2.9% | PASS |
| file_reads | `oltp_point_select` | 95.08ms | 46.81ms | 0.5× | 1.3% | PASS |
| file_reads | `oltp_range_select` | 21.49ms | 14.15ms | 0.7× | 1.6% | PASS |
| file_reads | `oltp_sum_range` | 19.46ms | 13.79ms | 0.7× | 1.0% | PASS |
| file_reads | `oltp_order_range` | 3.63ms | 3.00ms | 0.8× | 0.8% | PASS |
| file_reads | `oltp_distinct_range` | 4.43ms | 3.82ms | 0.9× | 0.6% | PASS |
| file_reads | `oltp_index_scan` | 10.03ms | 6.94ms | 0.7× | 0.8% | PASS |
| file_reads | `select_random_points` | 26.90ms | 20.21ms | 0.8× | 2.5% | PASS |
| file_reads | `select_random_ranges` | 12.47ms | 7.03ms | 0.6× | 0.9% | PASS |
| file_reads | `covering_index_scan` | 12.94ms | 10.04ms | 0.8× | 0.9% | PASS |
| file_reads | `groupby_scan` | 31.21ms | 30.18ms | 1.0× | 0.9% | PASS |
| file_reads | `index_join` | 13.16ms | 9.42ms | 0.7× | 2.3% | PASS |
| file_reads | `index_join_scan` | 3.96ms | 5.76ms | 1.5× | 1.6% | PASS |
| file_reads | `types_table_scan` | 910.76ms | 1.08s | 1.2× | 1.3% | PASS |
| file_reads | `table_scan` | 1.05s | 1.18s | 1.1× | 2.4% | PASS |
| file_reads | `oltp_read_only` | 204.07ms | 134.78ms | 0.7× | 0.6% | PASS |
| file_writes | `oltp_bulk_insert` | 287.15ms | 348.15ms | 1.2× | 10.8% | PASS |
| file_writes | `oltp_insert` | 33.89ms | 54.74ms | 1.6× | 23.0% | PASS |
| file_writes | `oltp_update_index` | 226.10ms | 193.38ms | 0.9× | 30.1% | PASS |
| file_writes | `oltp_update_non_index` | 157.14ms | 123.80ms | 0.8× | 10.6% | PASS |
| file_writes | `oltp_delete_insert` | 193.93ms | 143.57ms | 0.7× | 12.4% | PASS |
| file_writes | `oltp_write_only` | 127.12ms | 99.27ms | 0.8× | 12.6% | PASS |
| file_writes | `types_delete_insert` | 155.59ms | 93.10ms | 0.6× | 10.2% | PASS |
| file_writes | `oltp_read_write` | 191.10ms | 157.83ms | 0.8× | 14.0% | PASS |
| ac_reads | `oltp_point_select` | 49.43ms | 46.46ms | 0.9× | 0.7% | PASS |
| ac_reads | `oltp_range_select` | 16.67ms | 14.07ms | 0.8× | 0.9% | PASS |
| ac_reads | `oltp_sum_range` | 15.00ms | 13.87ms | 0.9× | 1.1% | PASS |
| ac_reads | `oltp_order_range` | 3.26ms | 3.00ms | 0.9× | 1.0% | PASS |
| ac_reads | `oltp_distinct_range` | 3.95ms | 3.83ms | 1.0× | 0.7% | PASS |
| ac_reads | `oltp_index_scan` | 5.70ms | 6.96ms | 1.2× | 0.9% | PASS |
| ac_reads | `select_random_points` | 20.41ms | 19.27ms | 0.9× | 1.0% | PASS |
| ac_reads | `select_random_ranges` | 7.90ms | 6.98ms | 0.9× | 0.6% | PASS |
| ac_reads | `covering_index_scan` | 8.54ms | 10.03ms | 1.2× | 0.7% | PASS |
| ac_reads | `groupby_scan` | 29.54ms | 29.53ms | 1.0× | 0.8% | PASS |
| ac_reads | `index_join` | 10.71ms | 9.27ms | 0.9× | 0.9% | PASS |
| ac_reads | `index_join_scan` | 3.54ms | 5.66ms | 1.6× | 0.7% | PASS |
| ac_reads | `types_table_scan` | 893.60ms | 1.07s | 1.2× | 0.4% | PASS |
| ac_reads | `table_scan` | 1.01s | 1.16s | 1.1× | 0.4% | PASS |
| ac_reads | `oltp_read_only` | 139.38ms | 134.28ms | 1.0× | 0.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 20.11ms | 60.33ms | 3.0× | 15.2% | PASS |
| ac_writes | `oltp_insert_ac` | 20.57ms | 61.27ms | 3.0× | 10.0% | PASS |
| ac_writes | `oltp_update_index_ac` | 21.69ms | 78.78ms | 3.6× | 17.8% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 17.64ms | 61.11ms | 3.5× | 11.1% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 19.74ms | 67.36ms | 3.4× | 7.0% | PASS |
| ac_writes | `oltp_write_only_ac` | 20.10ms | 68.02ms | 3.4× | 7.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 18.53ms | 64.46ms | 3.5× | 12.6% | PASS |
| ac_writes | `oltp_read_write_ac` | 26.47ms | 77.96ms | 2.9× | 22.7% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 39.21ms | 42.57ms | 1.1× | 2.6% | PASS |
| mem_reads | `oltp_range_select` | 17.89ms | 16.47ms | 0.9× | 2.2% | PASS |
| mem_reads | `oltp_sum_range` | 16.44ms | 16.26ms | 1.0× | 3.0% | PASS |
| mem_reads | `oltp_order_range` | 3.38ms | 3.50ms | 1.0× | 1.6% | PASS |
| mem_reads | `oltp_distinct_range` | 4.46ms | 4.70ms | 1.1× | 1.2% | PASS |
| mem_reads | `oltp_index_scan` | 4.21ms | 7.33ms | 1.7× | 2.4% | PASS |
| mem_reads | `select_random_points` | 24.33ms | 24.26ms | 1.0× | 4.4% | PASS |
| mem_reads | `select_random_ranges` | 6.82ms | 7.16ms | 1.1× | 1.7% | PASS |
| mem_reads | `covering_index_scan` | 7.69ms | 11.41ms | 1.5× | 1.4% | PASS |
| mem_reads | `groupby_scan` | 34.02ms | 35.92ms | 1.1× | 0.9% | PASS |
| mem_reads | `index_join` | 10.68ms | 11.24ms | 1.1× | 2.8% | PASS |
| mem_reads | `index_join_scan` | 3.96ms | 6.77ms | 1.7× | 4.2% | PASS |
| mem_reads | `types_table_scan` | 1.16s | 1.43s | 1.2× | 1.1% | PASS |
| mem_reads | `table_scan` | 1.28s | 1.49s | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_read_only` | 142.89ms | 152.17ms | 1.1× | 2.0% | PASS |
| mem_writes | `oltp_bulk_insert` | 240.29ms | 346.61ms | 1.4× | 1.2% | PASS |
| mem_writes | `oltp_insert` | 18.55ms | 38.06ms | 2.1× | 1.3% | PASS |
| mem_writes | `oltp_update_index` | 70.72ms | 137.05ms | 1.9× | 2.4% | PASS |
| mem_writes | `oltp_update_non_index` | 52.50ms | 83.17ms | 1.6× | 2.6% | PASS |
| mem_writes | `oltp_delete_insert` | 57.33ms | 102.76ms | 1.8× | 2.3% | PASS |
| mem_writes | `oltp_write_only` | 30.66ms | 58.54ms | 1.9× | 2.2% | PASS |
| mem_writes | `types_delete_insert` | 42.25ms | 58.54ms | 1.4× | 1.4% | PASS |
| mem_writes | `oltp_read_write` | 117.93ms | 155.17ms | 1.3× | 1.4% | PASS |
| file_reads | `oltp_point_select` | 115.10ms | 65.31ms | 0.6× | 1.6% | PASS |
| file_reads | `oltp_range_select` | 26.65ms | 19.58ms | 0.7× | 1.6% | PASS |
| file_reads | `oltp_sum_range` | 25.48ms | 19.23ms | 0.8× | 1.3% | PASS |
| file_reads | `oltp_order_range` | 4.58ms | 3.82ms | 0.8× | 1.9% | PASS |
| file_reads | `oltp_distinct_range` | 5.51ms | 4.96ms | 0.9× | 1.6% | PASS |
| file_reads | `oltp_index_scan` | 11.76ms | 9.30ms | 0.8× | 1.6% | PASS |
| file_reads | `select_random_points` | 33.48ms | 27.32ms | 0.8× | 3.7% | PASS |
| file_reads | `select_random_ranges` | 14.37ms | 9.22ms | 0.6× | 1.8% | PASS |
| file_reads | `covering_index_scan` | 15.32ms | 13.43ms | 0.9× | 1.4% | PASS |
| file_reads | `groupby_scan` | 35.04ms | 36.32ms | 1.0× | 1.3% | PASS |
| file_reads | `index_join` | 16.31ms | 12.99ms | 0.8× | 2.3% | PASS |
| file_reads | `index_join_scan` | 5.13ms | 7.29ms | 1.4× | 3.3% | PASS |
| file_reads | `types_table_scan` | 1.10s | 1.41s | 1.3× | 2.0% | PASS |
| file_reads | `table_scan` | 1.27s | 1.48s | 1.2× | 1.5% | PASS |
| file_reads | `oltp_read_only` | 253.45ms | 183.53ms | 0.7× | 1.3% | PASS |
| file_writes | `oltp_bulk_insert` | 263.40ms | 363.33ms | 1.4× | 1.2% | PASS |
| file_writes | `oltp_insert` | 25.95ms | 43.17ms | 1.7× | 2.4% | PASS |
| file_writes | `oltp_update_index` | 97.28ms | 146.57ms | 1.5× | 1.8% | PASS |
| file_writes | `oltp_update_non_index` | 94.85ms | 93.89ms | 1.0× | 8.0% | PASS |
| file_writes | `oltp_delete_insert` | 90.52ms | 115.50ms | 1.3× | 2.8% | PASS |
| file_writes | `oltp_write_only` | 61.97ms | 72.04ms | 1.2× | 2.1% | PASS |
| file_writes | `types_delete_insert` | 68.48ms | 68.29ms | 1.0× | 2.0% | PASS |
| file_writes | `oltp_read_write` | 138.03ms | 157.26ms | 1.1× | 1.7% | PASS |
| ac_reads | `oltp_point_select` | 61.73ms | 61.59ms | 1.0× | 1.7% | PASS |
| ac_reads | `oltp_range_select` | 20.55ms | 18.39ms | 0.9× | 2.4% | PASS |
| ac_reads | `oltp_sum_range` | 18.89ms | 18.11ms | 1.0× | 1.7% | PASS |
| ac_reads | `oltp_order_range` | 3.91ms | 3.74ms | 1.0× | 1.6% | PASS |
| ac_reads | `oltp_distinct_range` | 4.96ms | 4.96ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 7.03ms | 9.35ms | 1.3× | 1.5% | PASS |
| ac_reads | `select_random_points` | 29.90ms | 28.95ms | 1.0× | 3.3% | PASS |
| ac_reads | `select_random_ranges` | 9.86ms | 9.66ms | 1.0× | 1.7% | PASS |
| ac_reads | `covering_index_scan` | 10.99ms | 13.70ms | 1.2× | 1.8% | PASS |
| ac_reads | `groupby_scan` | 34.87ms | 36.65ms | 1.1× | 0.7% | PASS |
| ac_reads | `index_join` | 13.57ms | 12.77ms | 0.9× | 3.0% | PASS |
| ac_reads | `index_join_scan` | 4.40ms | 7.04ms | 1.6× | 3.0% | PASS |
| ac_reads | `types_table_scan` | 1.18s | 1.45s | 1.2× | 0.7% | PASS |
| ac_reads | `table_scan` | 1.32s | 1.50s | 1.1× | 1.6% | PASS |
| ac_reads | `oltp_read_only` | 184.82ms | 183.94ms | 1.0× | 1.9% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 25.84ms | 71.12ms | 2.8× | 7.5% | PASS |
| ac_writes | `oltp_insert_ac` | 26.71ms | 88.62ms | 3.3× | 6.7% | PASS |
| ac_writes | `oltp_update_index_ac` | 28.91ms | 101.42ms | 3.5× | 8.4% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.96ms | 79.29ms | 3.3× | 6.9% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 27.35ms | 91.67ms | 3.4× | 7.2% | PASS |
| ac_writes | `oltp_write_only_ac` | 27.05ms | 88.75ms | 3.3× | 7.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 26.08ms | 85.44ms | 3.3× | 9.1% | PASS |
| ac_writes | `oltp_read_write_ac` | 35.33ms | 98.59ms | 2.8× | 5.7% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.33ms | 42.55ms | 1.3× | 1.7% | PASS |
| mem_reads | `oltp_range_select` | 19.04ms | 23.72ms | 1.2× | 1.2% | PASS |
| mem_reads | `oltp_sum_range` | 17.60ms | 22.59ms | 1.3× | 0.8% | PASS |
| mem_reads | `oltp_order_range` | 3.50ms | 4.11ms | 1.2× | 1.6% | PASS |
| mem_reads | `oltp_distinct_range` | 4.61ms | 5.29ms | 1.1× | 0.7% | PASS |
| mem_reads | `oltp_index_scan` | 4.43ms | 6.46ms | 1.5× | 1.9% | PASS |
| mem_reads | `select_random_points` | 27.70ms | 33.02ms | 1.2× | 1.4% | PASS |
| mem_reads | `select_random_ranges` | 7.56ms | 9.38ms | 1.2× | 1.5% | PASS |
| mem_reads | `covering_index_scan` | 7.59ms | 10.98ms | 1.4× | 1.1% | PASS |
| mem_reads | `groupby_scan` | 35.47ms | 41.83ms | 1.2× | 0.7% | PASS |
| mem_reads | `index_join` | 7.82ms | 11.62ms | 1.5× | 1.5% | PASS |
| mem_reads | `index_join_scan` | 3.93ms | 6.43ms | 1.6× | 1.4% | PASS |
| mem_reads | `types_table_scan` | 1.04s | 1.37s | 1.3× | 0.5% | PASS |
| mem_reads | `table_scan` | 1.19s | 1.45s | 1.2× | 0.9% | PASS |
| mem_reads | `oltp_read_only` | 149.16ms | 183.55ms | 1.2× | 0.7% | PASS |
| mem_writes | `oltp_bulk_insert` | 244.02ms | 331.05ms | 1.4× | 1.5% | PASS |
| mem_writes | `oltp_insert` | 18.62ms | 32.98ms | 1.8× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 65.62ms | 117.15ms | 1.8× | 1.1% | PASS |
| mem_writes | `oltp_update_non_index` | 50.31ms | 81.06ms | 1.6× | 1.3% | PASS |
| mem_writes | `oltp_delete_insert` | 48.55ms | 90.06ms | 1.9× | 0.8% | PASS |
| mem_writes | `oltp_write_only` | 26.37ms | 52.37ms | 2.0× | 0.8% | PASS |
| mem_writes | `types_delete_insert` | 32.51ms | 53.41ms | 1.6× | 1.3% | PASS |
| mem_writes | `oltp_read_write` | 100.35ms | 156.24ms | 1.6× | 1.2% | PASS |
| file_reads | `oltp_point_select` | 102.78ms | 61.77ms | 0.6× | 1.0% | PASS |
| file_reads | `oltp_range_select` | 26.41ms | 26.24ms | 1.0× | 0.9% | PASS |
| file_reads | `oltp_sum_range` | 25.09ms | 25.22ms | 1.0× | 1.1% | PASS |
| file_reads | `oltp_order_range` | 4.34ms | 4.46ms | 1.0× | 1.1% | PASS |
| file_reads | `oltp_distinct_range` | 5.50ms | 5.64ms | 1.0× | 1.3% | PASS |
| file_reads | `oltp_index_scan` | 11.60ms | 8.73ms | 0.8× | 1.4% | PASS |
| file_reads | `select_random_points` | 36.68ms | 36.78ms | 1.0× | 1.1% | PASS |
| file_reads | `select_random_ranges` | 14.98ms | 11.73ms | 0.8× | 1.0% | PASS |
| file_reads | `covering_index_scan` | 14.84ms | 13.36ms | 0.9× | 1.0% | PASS |
| file_reads | `groupby_scan` | 36.46ms | 42.58ms | 1.2× | 0.7% | PASS |
| file_reads | `index_join` | 11.79ms | 13.46ms | 1.1× | 1.6% | PASS |
| file_reads | `index_join_scan` | 4.87ms | 6.88ms | 1.4× | 1.2% | PASS |
| file_reads | `types_table_scan` | 1.04s | 1.37s | 1.3× | 0.5% | PASS |
| file_reads | `table_scan` | 1.19s | 1.45s | 1.2× | 0.8% | PASS |
| file_reads | `oltp_read_only` | 253.27ms | 215.47ms | 0.9× | 1.0% | PASS |
| file_writes | `oltp_bulk_insert` | 261.59ms | 344.54ms | 1.3× | 0.9% | PASS |
| file_writes | `oltp_insert` | 25.59ms | 36.98ms | 1.4× | 1.8% | PASS |
| file_writes | `oltp_update_index` | 94.39ms | 131.93ms | 1.4× | 1.2% | PASS |
| file_writes | `oltp_update_non_index` | 76.30ms | 91.70ms | 1.2× | 1.7% | PASS |
| file_writes | `oltp_delete_insert` | 75.76ms | 102.97ms | 1.4× | 1.6% | PASS |
| file_writes | `oltp_write_only` | 50.37ms | 62.77ms | 1.2× | 1.8% | PASS |
| file_writes | `types_delete_insert` | 49.76ms | 61.65ms | 1.2× | 1.6% | PASS |
| file_writes | `oltp_read_write` | 125.36ms | 166.94ms | 1.3× | 1.4% | PASS |
| ac_reads | `oltp_point_select` | 56.03ms | 61.87ms | 1.1× | 1.1% | PASS |
| ac_reads | `oltp_range_select` | 21.57ms | 26.27ms | 1.2× | 1.4% | PASS |
| ac_reads | `oltp_sum_range` | 20.13ms | 25.23ms | 1.3× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 3.81ms | 4.45ms | 1.2× | 1.5% | PASS |
| ac_reads | `oltp_distinct_range` | 4.94ms | 5.63ms | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_index_scan` | 6.88ms | 8.81ms | 1.3× | 1.9% | PASS |
| ac_reads | `select_random_points` | 31.16ms | 36.93ms | 1.2× | 1.7% | PASS |
| ac_reads | `select_random_ranges` | 9.97ms | 11.76ms | 1.2× | 1.3% | PASS |
| ac_reads | `covering_index_scan` | 10.09ms | 13.44ms | 1.3× | 0.9% | PASS |
| ac_reads | `groupby_scan` | 36.09ms | 42.64ms | 1.2× | 0.6% | PASS |
| ac_reads | `index_join` | 9.29ms | 13.36ms | 1.4× | 1.4% | PASS |
| ac_reads | `index_join_scan` | 4.32ms | 6.92ms | 1.6× | 1.5% | PASS |
| ac_reads | `types_table_scan` | 1.04s | 1.37s | 1.3× | 0.6% | PASS |
| ac_reads | `table_scan` | 1.19s | 1.45s | 1.2× | 0.7% | PASS |
| ac_reads | `oltp_read_only` | 183.76ms | 215.27ms | 1.2× | 1.2% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.10ms | 67.68ms | 2.9× | 5.9% | PASS |
| ac_writes | `oltp_insert_ac` | 25.49ms | 83.02ms | 3.3× | 5.8% | PASS |
| ac_writes | `oltp_update_index_ac` | 28.35ms | 91.97ms | 3.2× | 5.1% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.03ms | 77.17ms | 3.2× | 6.7% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 24.03ms | 83.97ms | 3.5× | 5.7% | PASS |
| ac_writes | `oltp_write_only_ac` | 24.99ms | 83.38ms | 3.3× | 4.2% | PASS |
| ac_writes | `types_delete_insert_ac` | 22.33ms | 77.95ms | 3.5× | 8.6% | PASS |
| ac_writes | `oltp_read_write_ac` | 32.16ms | 93.93ms | 2.9× | 5.1% | PASS |

</details>

</details>

## Version-control latency

Wall time: 6m 29s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 29.42ms | 38.00ms | 77.4% | 2.6% | PASS |
| `status_dirty_many_tables` | 32.03ms | 42.00ms | 76.3% | 2.5% | PASS |
| `diff_regular_working_one_table` | 25.05ms | 33.00ms | 75.9% | 3.6% | PASS |
| `diff_regular_working_many_tables` | 36.63ms | 50.00ms | 73.3% | 2.7% | PASS |
| `diff_stat_working_many_tables` | 36.58ms | 48.00ms | 76.2% | 2.4% | PASS |
| `diff_schema_working_many_tables` | 37.27ms | 48.00ms | 77.6% | 2.4% | PASS |
| `branch_list_many_branches` | 19.78ms | 25.00ms | 79.1% | 3.7% | PASS |
| `branch_create_delete` | 21.32ms | 27.00ms | 79.0% | 3.4% | PASS |
| `at_literal_deep_history` | 21.42ms | 28.00ms | 76.5% | 3.7% | PASS |
| `diff_literal_deep_history` | 20.94ms | 28.00ms | 74.8% | 2.6% | PASS |
| `history_literal_deep_history` | 22.29ms | 30.00ms | 74.3% | 4.8% | PASS |
| `checkout_branch_clean` | 28.85ms | 43.00ms | 67.1% | 3.5% | PASS |
| `merge_data_no_conflicts` | 22.94ms | 33.00ms | 69.5% | 1.7% | PASS |
| `merge_data_secondary_index` | 761.71ms | 944.00ms | 80.7% | 1.2% | PASS |
| `merge_schema_no_conflicts` | 19.28ms | 24.00ms | 80.3% | 3.3% | PASS |
| `merge_data_conflicts` | 26.23ms | 33.00ms | 79.5% | 2.6% | PASS |
| `merge_data_conflicts_with_resolve` | 27.06ms | 34.00ms | 79.6% | 2.8% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
