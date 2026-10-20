# DoltLite Performance Report

> Nightly result: **FAIL**
>
> Generated: 2026-10-20 11:16 UTC
>
> Commit: [`96f91330d2277169dadff95bf2aa15db912e4c4d`](https://github.com/dolthub/doltlite/commit/96f91330d2277169dadff95bf2aa15db912e4c4d)
>
> Runner: ubuntu24 20261018.339.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/39402072901)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.29s | 11.52s | 1.1× | 1.7% | **PASS** |
| Writes | 2.00s | 3.25s | 1.6× | 1.4% | **FAIL** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.65s | 11.32s | 1.1× | 1.6% | **PASS** |
| Writes | 4.03s | 4.29s | 1.1× | 3.9% | **PASS** |
| Autocommit writes | 1.18s | 3.81s | 3.2× | 20.2% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.40s | 2.78s | 1.2× | 1.1% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.20s | 2.45s | 1.1× | 1.7% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.48s | 3.07s | 1.2× | 1.5% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 3.21s | 3.21s | 1.0× | 6.8% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 424.44ms | 761.48ms | 1.8× | 1.2% | **FAIL** |
| In-memory | Writes | textpk | 8 | 55 | 472.43ms | 722.50ms | 1.5× | 1.8% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 563.50ms | 953.75ms | 1.7× | 1.1% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 539.12ms | 810.83ms | 1.5× | 5.3% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.62s | 2.83s | 1.1× | 1.3% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.63s | 2.55s | 1.0× | 1.2% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.70s | 3.13s | 1.2× | 1.6% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.70s | 2.81s | 1.0× | 7.9% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 599.55ms | 819.52ms | 1.4× | 1.4% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 1.59s | 1.35s | 0.8× | 21.1% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 806.47ms | 1.07s | 1.3× | 2.0% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 1.04s | 1.06s | 1.0× | 6.7% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.48s | 2.83s | 1.1× | 1.2% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.72s | 2.65s | 1.0× | 1.1% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.57s | 3.13s | 1.2× | 2.3% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 3.08s | 3.09s | 1.0× | 1.3% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 318.80ms | 877.58ms | 2.8× | 8.8% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 420.77ms | 1.46s | 3.5× | 71.2% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 206.46ms | 683.37ms | 3.3× | 7.4% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 233.50ms | 793.04ms | 3.4× | 38.9% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 22.66ms | 31.33ms | 1.4× | 1.1% | PASS |
| mem_reads | `oltp_range_select` | 9.85ms | 12.39ms | 1.3× | 1.6% | PASS |
| mem_reads | `oltp_sum_range` | 9.04ms | 12.31ms | 1.4× | 1.2% | PASS |
| mem_reads | `oltp_order_range` | 2.51ms | 2.95ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_distinct_range` | 3.58ms | 4.06ms | 1.1× | 1.1% | PASS |
| mem_reads | `oltp_index_scan` | 3.75ms | 5.51ms | 1.5× | 1.6% | PASS |
| mem_reads | `select_random_points` | 9.35ms | 11.88ms | 1.3× | 1.6% | PASS |
| mem_reads | `select_random_ranges` | 4.43ms | 5.35ms | 1.2× | 1.3% | PASS |
| mem_reads | `covering_index_scan` | 7.51ms | 10.69ms | 1.4× | 1.0% | PASS |
| mem_reads | `groupby_scan` | 29.32ms | 33.06ms | 1.1× | 0.7% | PASS |
| mem_reads | `index_join` | 5.68ms | 8.70ms | 1.5× | 1.2% | PASS |
| mem_reads | `index_join_scan` | 3.12ms | 5.67ms | 1.8× | 1.3% | PASS |
| mem_reads | `types_table_scan` | 1.03s | 1.20s | 1.2× | 0.6% | PASS |
| mem_reads | `table_scan` | 1.16s | 1.31s | 1.1× | 0.6% | PASS |
| mem_reads | `oltp_read_only` | 99.94ms | 124.41ms | 1.2× | 0.7% | PASS |
| mem_writes | `oltp_bulk_insert` | 173.52ms | 291.49ms | 1.7× | 1.2% | PASS |
| mem_writes | `oltp_insert` | 14.82ms | 27.15ms | 1.8× | 1.1% | PASS |
| mem_writes | `oltp_update_index` | 48.89ms | 99.50ms | 2.0× | 1.4% | PASS |
| mem_writes | `oltp_update_non_index` | 33.41ms | 63.23ms | 1.9× | 0.8% | PASS |
| mem_writes | `oltp_delete_insert` | 43.74ms | 79.72ms | 1.8× | 0.8% | PASS |
| mem_writes | `oltp_write_only` | 21.50ms | 49.75ms | 2.3× | 1.2% | FAIL |
| mem_writes | `types_delete_insert` | 23.77ms | 37.27ms | 1.6× | 1.1% | PASS |
| mem_writes | `oltp_read_write` | 64.81ms | 113.38ms | 1.7× | 1.4% | PASS |
| file_reads | `oltp_point_select` | 86.74ms | 47.88ms | 0.6× | 0.9% | PASS |
| file_reads | `oltp_range_select` | 16.73ms | 14.19ms | 0.8× | 2.2% | PASS |
| file_reads | `oltp_sum_range` | 15.96ms | 14.22ms | 0.9× | 1.9% | PASS |
| file_reads | `oltp_order_range` | 3.37ms | 3.28ms | 1.0× | 2.4% | PASS |
| file_reads | `oltp_distinct_range` | 4.41ms | 4.34ms | 1.0× | 1.3% | PASS |
| file_reads | `oltp_index_scan` | 10.53ms | 7.47ms | 0.7× | 1.1% | PASS |
| file_reads | `select_random_points` | 16.52ms | 13.72ms | 0.8× | 1.8% | PASS |
| file_reads | `select_random_ranges` | 11.16ms | 7.22ms | 0.6× | 1.1% | PASS |
| file_reads | `covering_index_scan` | 14.48ms | 12.59ms | 0.9× | 1.3% | PASS |
| file_reads | `groupby_scan` | 30.29ms | 33.35ms | 1.1× | 0.6% | PASS |
| file_reads | `index_join` | 9.48ms | 10.18ms | 1.1× | 1.4% | PASS |
| file_reads | `index_join_scan` | 4.02ms | 6.02ms | 1.5× | 1.3% | PASS |
| file_reads | `types_table_scan` | 1.03s | 1.20s | 1.2× | 0.6% | PASS |
| file_reads | `table_scan` | 1.17s | 1.31s | 1.1× | 0.9% | PASS |
| file_reads | `oltp_read_only` | 193.62ms | 149.81ms | 0.8× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 189.02ms | 300.60ms | 1.6× | 0.9% | PASS |
| file_writes | `oltp_insert` | 24.23ms | 30.58ms | 1.3× | 1.4% | PASS |
| file_writes | `oltp_update_index` | 75.89ms | 109.02ms | 1.4× | 1.7% | PASS |
| file_writes | `oltp_update_non_index` | 59.25ms | 69.86ms | 1.2× | 1.4% | PASS |
| file_writes | `oltp_delete_insert` | 69.30ms | 90.95ms | 1.3× | 1.2% | PASS |
| file_writes | `oltp_write_only` | 48.15ms | 55.98ms | 1.2× | 1.4% | PASS |
| file_writes | `types_delete_insert` | 40.09ms | 43.37ms | 1.1× | 1.9% | PASS |
| file_writes | `oltp_read_write` | 93.62ms | 119.16ms | 1.3× | 1.7% | PASS |
| ac_reads | `oltp_point_select` | 44.02ms | 47.79ms | 1.1× | 0.7% | PASS |
| ac_reads | `oltp_range_select` | 12.56ms | 14.25ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_sum_range` | 11.58ms | 14.20ms | 1.2× | 1.1% | PASS |
| ac_reads | `oltp_order_range` | 2.89ms | 3.22ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_distinct_range` | 3.91ms | 4.33ms | 1.1× | 0.9% | PASS |
| ac_reads | `oltp_index_scan` | 6.17ms | 7.50ms | 1.2× | 1.4% | PASS |
| ac_reads | `select_random_points` | 12.16ms | 13.72ms | 1.1× | 1.2% | PASS |
| ac_reads | `select_random_ranges` | 6.80ms | 7.20ms | 1.1× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.04ms | 12.67ms | 1.3× | 1.3% | PASS |
| ac_reads | `groupby_scan` | 29.74ms | 33.37ms | 1.1× | 0.8% | PASS |
| ac_reads | `index_join` | 7.20ms | 10.14ms | 1.4× | 1.8% | PASS |
| ac_reads | `index_join_scan` | 3.55ms | 6.01ms | 1.7× | 1.2% | PASS |
| ac_reads | `types_table_scan` | 1.03s | 1.20s | 1.2× | 0.5% | PASS |
| ac_reads | `table_scan` | 1.17s | 1.30s | 1.1× | 0.6% | PASS |
| ac_reads | `oltp_read_only` | 131.42ms | 149.96ms | 1.1× | 0.9% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 40.21ms | 98.73ms | 2.5× | 8.0% | PASS |
| ac_writes | `oltp_insert_ac` | 39.67ms | 111.47ms | 2.8× | 8.8% | PASS |
| ac_writes | `oltp_update_index_ac` | 42.67ms | 120.27ms | 2.8× | 9.3% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 36.23ms | 102.83ms | 2.8× | 8.7% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 38.29ms | 107.98ms | 2.8× | 9.3% | PASS |
| ac_writes | `oltp_write_only_ac` | 38.85ms | 111.22ms | 2.9× | 8.0% | PASS |
| ac_writes | `types_delete_insert_ac` | 38.03ms | 106.53ms | 2.8× | 12.9% | PASS |
| ac_writes | `oltp_read_write_ac` | 44.85ms | 118.54ms | 2.6× | 7.7% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 28.99ms | 29.78ms | 1.0× | 2.0% | PASS |
| mem_reads | `oltp_range_select` | 15.42ms | 12.66ms | 0.8× | 1.8% | PASS |
| mem_reads | `oltp_sum_range` | 13.48ms | 12.04ms | 0.9× | 2.2% | PASS |
| mem_reads | `oltp_order_range` | 2.97ms | 2.84ms | 1.0× | 1.7% | PASS |
| mem_reads | `oltp_distinct_range` | 3.83ms | 3.64ms | 0.9× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 3.22ms | 5.34ms | 1.7× | 1.4% | PASS |
| mem_reads | `select_random_points` | 20.18ms | 17.70ms | 0.9× | 3.3% | PASS |
| mem_reads | `select_random_ranges` | 5.75ms | 5.25ms | 0.9× | 1.7% | PASS |
| mem_reads | `covering_index_scan` | 5.87ms | 8.15ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 30.08ms | 29.75ms | 1.0× | 0.8% | PASS |
| mem_reads | `index_join` | 9.52ms | 8.78ms | 0.9× | 2.8% | PASS |
| mem_reads | `index_join_scan` | 3.35ms | 5.76ms | 1.7× | 2.4% | PASS |
| mem_reads | `types_table_scan` | 913.88ms | 1.06s | 1.2× | 1.5% | PASS |
| mem_reads | `table_scan` | 1.03s | 1.14s | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_read_only` | 114.11ms | 110.03ms | 1.0× | 2.5% | PASS |
| mem_writes | `oltp_bulk_insert` | 183.68ms | 257.80ms | 1.4× | 0.8% | PASS |
| mem_writes | `oltp_insert` | 14.02ms | 27.90ms | 2.0× | 0.8% | PASS |
| mem_writes | `oltp_update_index` | 54.76ms | 108.94ms | 2.0× | 2.1% | PASS |
| mem_writes | `oltp_update_non_index` | 40.85ms | 59.77ms | 1.5× | 1.9% | PASS |
| mem_writes | `oltp_delete_insert` | 45.03ms | 80.43ms | 1.8× | 1.7% | PASS |
| mem_writes | `oltp_write_only` | 23.13ms | 41.96ms | 1.8× | 1.3% | PASS |
| mem_writes | `types_delete_insert` | 32.45ms | 43.87ms | 1.4× | 3.7% | PASS |
| mem_writes | `oltp_read_write` | 78.51ms | 101.82ms | 1.3× | 3.0% | PASS |
| file_reads | `oltp_point_select` | 90.67ms | 46.27ms | 0.5× | 1.2% | PASS |
| file_reads | `oltp_range_select` | 23.25ms | 14.54ms | 0.6× | 2.4% | PASS |
| file_reads | `oltp_sum_range` | 20.23ms | 13.83ms | 0.7× | 1.5% | PASS |
| file_reads | `oltp_order_range` | 3.85ms | 3.02ms | 0.8× | 1.2% | PASS |
| file_reads | `oltp_distinct_range` | 4.65ms | 3.80ms | 0.8× | 0.9% | PASS |
| file_reads | `oltp_index_scan` | 9.62ms | 7.04ms | 0.7× | 1.2% | PASS |
| file_reads | `select_random_points` | 25.44ms | 19.38ms | 0.8× | 2.0% | PASS |
| file_reads | `select_random_ranges` | 12.03ms | 6.95ms | 0.6× | 0.9% | PASS |
| file_reads | `covering_index_scan` | 12.49ms | 9.84ms | 0.8× | 0.9% | PASS |
| file_reads | `groupby_scan` | 31.08ms | 30.11ms | 1.0× | 1.1% | PASS |
| file_reads | `index_join` | 12.73ms | 9.54ms | 0.7× | 1.0% | PASS |
| file_reads | `index_join_scan` | 4.14ms | 5.97ms | 1.4× | 2.2% | PASS |
| file_reads | `types_table_scan` | 909.48ms | 1.05s | 1.2× | 1.3% | PASS |
| file_reads | `table_scan` | 1.26s | 1.20s | 1.0× | 2.8% | PASS |
| file_reads | `oltp_read_only` | 213.85ms | 136.88ms | 0.6× | 2.0% | PASS |
| file_writes | `oltp_bulk_insert` | 306.19ms | 368.37ms | 1.2× | 19.0% | PASS |
| file_writes | `oltp_insert` | 49.91ms | 59.34ms | 1.2× | 36.9% | PASS |
| file_writes | `oltp_update_index` | 207.17ms | 207.68ms | 1.0× | 19.6% | PASS |
| file_writes | `oltp_update_non_index` | 170.05ms | 132.71ms | 0.8× | 19.9% | PASS |
| file_writes | `oltp_delete_insert` | 288.84ms | 182.55ms | 0.6× | 37.9% | PASS |
| file_writes | `oltp_write_only` | 184.25ms | 121.83ms | 0.7× | 24.2% | PASS |
| file_writes | `types_delete_insert` | 180.38ms | 103.78ms | 0.6× | 22.3% | PASS |
| file_writes | `oltp_read_write` | 198.90ms | 168.79ms | 0.8× | 8.4% | PASS |
| ac_reads | `oltp_point_select` | 49.10ms | 45.79ms | 0.9× | 0.8% | PASS |
| ac_reads | `oltp_range_select` | 19.13ms | 14.52ms | 0.8× | 2.4% | PASS |
| ac_reads | `oltp_sum_range` | 16.32ms | 13.77ms | 0.8× | 1.9% | PASS |
| ac_reads | `oltp_order_range` | 3.48ms | 3.01ms | 0.9× | 1.0% | PASS |
| ac_reads | `oltp_distinct_range` | 4.21ms | 3.78ms | 0.9× | 1.0% | PASS |
| ac_reads | `oltp_index_scan` | 5.72ms | 7.08ms | 1.2× | 1.0% | PASS |
| ac_reads | `select_random_points` | 21.25ms | 19.55ms | 0.9× | 1.6% | PASS |
| ac_reads | `select_random_ranges` | 8.11ms | 6.98ms | 0.9× | 1.1% | PASS |
| ac_reads | `covering_index_scan` | 8.51ms | 9.82ms | 1.2× | 0.8% | PASS |
| ac_reads | `groupby_scan` | 30.35ms | 30.01ms | 1.0× | 0.6% | PASS |
| ac_reads | `index_join` | 11.23ms | 9.68ms | 0.9× | 1.1% | PASS |
| ac_reads | `index_join_scan` | 3.77ms | 6.00ms | 1.6× | 1.4% | PASS |
| ac_reads | `types_table_scan` | 1.07s | 1.11s | 1.0× | 3.7% | PASS |
| ac_reads | `table_scan` | 1.31s | 1.22s | 0.9× | 1.5% | PASS |
| ac_reads | `oltp_read_only` | 161.74ms | 140.19ms | 0.9× | 1.8% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 44.96ms | 198.28ms | 4.4× | 76.2% | PASS |
| ac_writes | `oltp_insert_ac` | 44.82ms | 171.61ms | 3.8× | 71.3% | PASS |
| ac_writes | `oltp_update_index_ac` | 76.74ms | 185.12ms | 2.4× | 63.7% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.01ms | 112.41ms | 4.7× | 67.5% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 49.02ms | 145.11ms | 3.0× | 66.0% | PASS |
| ac_writes | `oltp_write_only_ac` | 75.69ms | 240.01ms | 3.2× | 74.7% | PASS |
| ac_writes | `types_delete_insert_ac` | 45.78ms | 188.52ms | 4.1× | 71.1% | PASS |
| ac_writes | `oltp_read_write_ac` | 59.76ms | 214.17ms | 3.6× | 71.6% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 32.40ms | 40.56ms | 1.3× | 1.5% | PASS |
| mem_reads | `oltp_range_select` | 14.71ms | 16.30ms | 1.1× | 2.5% | PASS |
| mem_reads | `oltp_sum_range` | 13.78ms | 15.61ms | 1.1× | 1.4% | PASS |
| mem_reads | `oltp_order_range` | 3.03ms | 3.58ms | 1.2× | 1.8% | PASS |
| mem_reads | `oltp_distinct_range` | 4.09ms | 4.69ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 3.74ms | 6.80ms | 1.8× | 1.9% | PASS |
| mem_reads | `select_random_points` | 19.72ms | 22.45ms | 1.1× | 2.0% | PASS |
| mem_reads | `select_random_ranges` | 5.95ms | 6.96ms | 1.2× | 2.0% | PASS |
| mem_reads | `covering_index_scan` | 7.58ms | 11.17ms | 1.5× | 0.7% | PASS |
| mem_reads | `groupby_scan` | 32.84ms | 36.88ms | 1.1× | 0.8% | PASS |
| mem_reads | `index_join` | 9.61ms | 10.94ms | 1.1× | 1.8% | PASS |
| mem_reads | `index_join_scan` | 3.53ms | 6.49ms | 1.8× | 2.2% | PASS |
| mem_reads | `types_table_scan` | 1.04s | 1.33s | 1.3× | 0.8% | PASS |
| mem_reads | `table_scan` | 1.16s | 1.41s | 1.2× | 0.9% | PASS |
| mem_reads | `oltp_read_only` | 128.59ms | 150.27ms | 1.2× | 0.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 234.42ms | 348.89ms | 1.5× | 0.6% | PASS |
| mem_writes | `oltp_insert` | 17.95ms | 37.68ms | 2.1× | 0.6% | PASS |
| mem_writes | `oltp_update_index` | 61.66ms | 136.03ms | 2.2× | 1.6% | PASS |
| mem_writes | `oltp_update_non_index` | 45.45ms | 78.33ms | 1.7× | 1.0% | PASS |
| mem_writes | `oltp_delete_insert` | 50.58ms | 102.45ms | 2.0× | 1.0% | PASS |
| mem_writes | `oltp_write_only` | 26.41ms | 55.95ms | 2.1× | 1.2% | PASS |
| mem_writes | `types_delete_insert` | 36.79ms | 57.45ms | 1.6× | 1.3% | PASS |
| mem_writes | `oltp_read_write` | 90.23ms | 136.98ms | 1.5× | 1.1% | PASS |
| file_reads | `oltp_point_select` | 96.87ms | 57.57ms | 0.6× | 1.2% | PASS |
| file_reads | `oltp_range_select` | 21.55ms | 18.19ms | 0.8× | 1.8% | PASS |
| file_reads | `oltp_sum_range` | 20.88ms | 17.50ms | 0.8× | 2.0% | PASS |
| file_reads | `oltp_order_range` | 4.06ms | 3.82ms | 0.9× | 2.0% | PASS |
| file_reads | `oltp_distinct_range` | 5.15ms | 4.96ms | 1.0× | 1.6% | PASS |
| file_reads | `oltp_index_scan` | 10.31ms | 8.58ms | 0.8× | 1.0% | PASS |
| file_reads | `select_random_points` | 28.14ms | 24.97ms | 0.9× | 2.6% | PASS |
| file_reads | `select_random_ranges` | 12.83ms | 8.80ms | 0.7× | 1.6% | PASS |
| file_reads | `covering_index_scan` | 14.32ms | 12.92ms | 0.9× | 1.1% | PASS |
| file_reads | `groupby_scan` | 34.22ms | 37.35ms | 1.1× | 1.0% | PASS |
| file_reads | `index_join` | 13.41ms | 12.08ms | 0.9× | 2.1% | PASS |
| file_reads | `index_join_scan` | 4.66ms | 6.94ms | 1.5× | 2.0% | PASS |
| file_reads | `types_table_scan` | 1.04s | 1.33s | 1.3× | 0.7% | PASS |
| file_reads | `table_scan` | 1.16s | 1.42s | 1.2× | 0.9% | PASS |
| file_reads | `oltp_read_only` | 227.24ms | 176.47ms | 0.8× | 0.9% | PASS |
| file_writes | `oltp_bulk_insert` | 255.52ms | 365.37ms | 1.4× | 1.2% | PASS |
| file_writes | `oltp_insert` | 25.30ms | 43.28ms | 1.7× | 1.6% | PASS |
| file_writes | `oltp_update_index` | 95.11ms | 156.37ms | 1.6× | 1.7% | PASS |
| file_writes | `oltp_update_non_index` | 95.54ms | 92.27ms | 1.0× | 10.0% | PASS |
| file_writes | `oltp_delete_insert` | 86.82ms | 120.68ms | 1.4× | 2.8% | PASS |
| file_writes | `oltp_write_only` | 57.01ms | 70.14ms | 1.2× | 1.9% | PASS |
| file_writes | `types_delete_insert` | 63.05ms | 66.24ms | 1.1× | 2.0% | PASS |
| file_writes | `oltp_read_write` | 128.12ms | 151.53ms | 1.2× | 3.2% | PASS |
| ac_reads | `oltp_point_select` | 55.05ms | 57.93ms | 1.1× | 1.6% | PASS |
| ac_reads | `oltp_range_select` | 18.31ms | 18.12ms | 1.0× | 3.5% | PASS |
| ac_reads | `oltp_sum_range` | 17.44ms | 17.69ms | 1.0× | 4.4% | PASS |
| ac_reads | `oltp_order_range` | 3.66ms | 3.84ms | 1.1× | 2.6% | PASS |
| ac_reads | `oltp_distinct_range` | 4.73ms | 4.98ms | 1.1× | 2.5% | PASS |
| ac_reads | `oltp_index_scan` | 6.09ms | 8.71ms | 1.4× | 1.8% | PASS |
| ac_reads | `select_random_points` | 23.68ms | 25.14ms | 1.1× | 2.9% | PASS |
| ac_reads | `select_random_ranges` | 8.57ms | 8.81ms | 1.0× | 2.4% | PASS |
| ac_reads | `covering_index_scan` | 9.95ms | 12.95ms | 1.3× | 1.3% | PASS |
| ac_reads | `groupby_scan` | 33.69ms | 37.41ms | 1.1× | 0.9% | PASS |
| ac_reads | `index_join` | 11.53ms | 12.11ms | 1.1× | 2.3% | PASS |
| ac_reads | `index_join_scan` | 4.09ms | 6.90ms | 1.7× | 3.2% | PASS |
| ac_reads | `types_table_scan` | 1.04s | 1.33s | 1.3× | 0.8% | PASS |
| ac_reads | `table_scan` | 1.16s | 1.41s | 1.2× | 0.6% | PASS |
| ac_reads | `oltp_read_only` | 165.77ms | 176.82ms | 1.1× | 0.8% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.07ms | 67.54ms | 2.9× | 6.1% | PASS |
| ac_writes | `oltp_insert_ac` | 25.87ms | 86.14ms | 3.3× | 6.8% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.16ms | 99.69ms | 3.7× | 7.7% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 22.59ms | 77.22ms | 3.4× | 6.5% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.82ms | 88.73ms | 3.4× | 8.9% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.51ms | 86.81ms | 3.4× | 8.6% | PASS |
| ac_writes | `types_delete_insert_ac` | 25.10ms | 81.77ms | 3.3× | 7.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.34ms | 95.46ms | 3.0× | 7.2% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 30.73ms | 36.73ms | 1.2× | 6.5% | PASS |
| mem_reads | `oltp_range_select` | 22.16ms | 27.40ms | 1.2× | 17.3% | PASS |
| mem_reads | `oltp_sum_range` | 18.66ms | 22.86ms | 1.2× | 12.0% | PASS |
| mem_reads | `oltp_order_range` | 3.70ms | 4.50ms | 1.2× | 12.5% | PASS |
| mem_reads | `oltp_distinct_range` | 4.81ms | 5.24ms | 1.1× | 10.2% | PASS |
| mem_reads | `oltp_index_scan` | 4.12ms | 5.81ms | 1.4× | 5.9% | PASS |
| mem_reads | `select_random_points` | 25.17ms | 28.56ms | 1.1× | 4.6% | PASS |
| mem_reads | `select_random_ranges` | 6.75ms | 8.41ms | 1.2× | 5.2% | PASS |
| mem_reads | `covering_index_scan` | 6.53ms | 9.51ms | 1.5× | 10.6% | PASS |
| mem_reads | `groupby_scan` | 36.19ms | 43.16ms | 1.2× | 6.1% | PASS |
| mem_reads | `index_join` | 7.32ms | 11.74ms | 1.6× | 6.8% | PASS |
| mem_reads | `index_join_scan` | 4.16ms | 7.65ms | 1.8× | 7.1% | PASS |
| mem_reads | `types_table_scan` | 1.50s | 1.46s | 1.0× | 7.7% | PASS |
| mem_reads | `table_scan` | 1.41s | 1.38s | 1.0× | 5.9% | PASS |
| mem_reads | `oltp_read_only` | 131.51ms | 163.80ms | 1.2× | 3.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 206.99ms | 272.27ms | 1.3× | 3.2% | PASS |
| mem_writes | `oltp_insert` | 16.18ms | 29.11ms | 1.8× | 5.6% | PASS |
| mem_writes | `oltp_update_index` | 60.97ms | 107.97ms | 1.8× | 4.3% | PASS |
| mem_writes | `oltp_update_non_index` | 46.28ms | 68.72ms | 1.5× | 4.4% | PASS |
| mem_writes | `oltp_delete_insert` | 46.25ms | 80.47ms | 1.7× | 5.8% | PASS |
| mem_writes | `oltp_write_only` | 27.14ms | 51.33ms | 1.9× | 6.8% | PASS |
| mem_writes | `types_delete_insert` | 31.89ms | 46.56ms | 1.5× | 5.1% | PASS |
| mem_writes | `oltp_read_write` | 103.42ms | 154.40ms | 1.5× | 6.9% | PASS |
| file_reads | `oltp_point_select` | 106.04ms | 57.09ms | 0.5× | 8.6% | PASS |
| file_reads | `oltp_range_select` | 29.90ms | 29.71ms | 1.0× | 12.4% | PASS |
| file_reads | `oltp_sum_range` | 26.02ms | 25.83ms | 1.0× | 9.9% | PASS |
| file_reads | `oltp_order_range` | 4.71ms | 4.29ms | 0.9× | 11.7% | PASS |
| file_reads | `oltp_distinct_range` | 5.67ms | 5.47ms | 1.0× | 12.7% | PASS |
| file_reads | `oltp_index_scan` | 11.70ms | 8.20ms | 0.7× | 6.3% | PASS |
| file_reads | `select_random_points` | 33.54ms | 31.42ms | 0.9× | 5.1% | PASS |
| file_reads | `select_random_ranges` | 14.49ms | 10.50ms | 0.7× | 7.1% | PASS |
| file_reads | `covering_index_scan` | 14.52ms | 12.60ms | 0.9× | 7.9% | PASS |
| file_reads | `groupby_scan` | 37.85ms | 42.04ms | 1.1× | 5.3% | PASS |
| file_reads | `index_join` | 11.43ms | 12.18ms | 1.1× | 8.0% | PASS |
| file_reads | `index_join_scan` | 4.62ms | 7.35ms | 1.6× | 9.3% | PASS |
| file_reads | `types_table_scan` | 1.04s | 1.19s | 1.1× | 2.9% | PASS |
| file_reads | `table_scan` | 1.15s | 1.22s | 1.1× | 4.7% | PASS |
| file_reads | `oltp_read_only` | 211.18ms | 157.69ms | 0.7× | 1.2% | PASS |
| file_writes | `oltp_bulk_insert` | 246.89ms | 290.60ms | 1.2× | 3.8% | PASS |
| file_writes | `oltp_insert` | 28.41ms | 36.65ms | 1.3× | 13.6% | PASS |
| file_writes | `oltp_update_index` | 155.21ms | 151.61ms | 1.0× | 4.1% | PASS |
| file_writes | `oltp_update_non_index` | 140.33ms | 108.68ms | 0.8× | 6.6% | PASS |
| file_writes | `oltp_delete_insert` | 139.52ms | 135.73ms | 1.0× | 12.2% | PASS |
| file_writes | `oltp_write_only` | 90.54ms | 88.62ms | 1.0× | 6.8% | PASS |
| file_writes | `types_delete_insert` | 82.30ms | 80.22ms | 1.0× | 12.4% | PASS |
| file_writes | `oltp_read_write` | 159.81ms | 165.85ms | 1.0× | 6.6% | PASS |
| ac_reads | `oltp_point_select` | 48.82ms | 49.28ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 18.55ms | 19.23ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_sum_range` | 16.60ms | 18.28ms | 1.1× | 1.0% | PASS |
| ac_reads | `oltp_order_range` | 3.33ms | 3.41ms | 1.0× | 1.1% | PASS |
| ac_reads | `oltp_distinct_range` | 4.09ms | 4.22ms | 1.0× | 1.1% | PASS |
| ac_reads | `oltp_index_scan` | 6.19ms | 6.96ms | 1.1× | 1.0% | PASS |
| ac_reads | `select_random_points` | 24.74ms | 26.84ms | 1.1× | 1.2% | PASS |
| ac_reads | `select_random_ranges` | 8.31ms | 8.78ms | 1.1× | 0.7% | PASS |
| ac_reads | `covering_index_scan` | 9.72ms | 12.16ms | 1.2× | 9.1% | PASS |
| ac_reads | `groupby_scan` | 36.19ms | 43.44ms | 1.2× | 7.1% | PASS |
| ac_reads | `index_join` | 9.05ms | 12.49ms | 1.4× | 7.3% | PASS |
| ac_reads | `index_join_scan` | 4.39ms | 7.19ms | 1.6× | 8.8% | PASS |
| ac_reads | `types_table_scan` | 1.07s | 1.22s | 1.1× | 2.7% | PASS |
| ac_reads | `table_scan` | 1.64s | 1.45s | 0.9× | 8.2% | PASS |
| ac_reads | `oltp_read_only` | 179.25ms | 208.17ms | 1.2× | 3.6% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 21.13ms | 70.21ms | 3.3× | 34.4% | PASS |
| ac_writes | `oltp_insert_ac` | 29.02ms | 94.07ms | 3.2× | 46.2% | PASS |
| ac_writes | `oltp_update_index_ac` | 31.21ms | 103.75ms | 3.3× | 27.5% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.26ms | 82.44ms | 3.5× | 36.0% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 30.32ms | 95.80ms | 3.2× | 46.7% | PASS |
| ac_writes | `oltp_write_only_ac` | 30.50ms | 119.98ms | 3.9× | 38.2% | PASS |
| ac_writes | `types_delete_insert_ac` | 30.56ms | 97.23ms | 3.2× | 39.7% | PASS |
| ac_writes | `oltp_read_write_ac` | 37.49ms | 129.54ms | 3.5× | 43.3% | PASS |

</details>

</details>

## Version-control latency

Wall time: 4m 33s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 35.31ms | 38.00ms | 92.9% | 1.1% | PASS |
| `status_dirty_many_tables` | 39.04ms | 42.00ms | 92.9% | 1.0% | PASS |
| `diff_regular_working_one_table` | 30.83ms | 33.00ms | 93.4% | 1.0% | PASS |
| `diff_regular_working_many_tables` | 44.66ms | 50.00ms | 89.3% | 1.1% | PASS |
| `diff_stat_working_many_tables` | 44.19ms | 48.00ms | 92.1% | 0.9% | PASS |
| `diff_schema_working_many_tables` | 45.16ms | 48.00ms | 94.1% | 0.8% | PASS |
| `branch_list_many_branches` | 22.93ms | 25.00ms | 91.7% | 1.3% | PASS |
| `branch_create_delete` | 24.07ms | 27.00ms | 89.1% | 0.8% | PASS |
| `at_literal_deep_history` | 24.51ms | 28.00ms | 87.5% | 1.4% | PASS |
| `diff_literal_deep_history` | 24.55ms | 28.00ms | 87.7% | 1.2% | PASS |
| `history_literal_deep_history` | 25.79ms | 30.00ms | 86.0% | 0.8% | PASS |
| `checkout_branch_clean` | 33.68ms | 43.00ms | 78.3% | 0.9% | PASS |
| `merge_data_no_conflicts` | 27.62ms | 33.00ms | 83.7% | 1.2% | PASS |
| `merge_data_secondary_index` | 916.91ms | 944.00ms | 97.1% | 0.8% | PASS |
| `merge_schema_no_conflicts` | 22.06ms | 24.00ms | 91.9% | 2.2% | PASS |
| `merge_data_conflicts` | 31.16ms | 33.00ms | 94.4% | 1.7% | PASS |
| `merge_data_conflicts_with_resolve` | 31.77ms | 34.00ms | 93.4% | 1.2% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
