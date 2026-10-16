# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-16 11:09 UTC
>
> Commit: [`35c2e050927c7f4b29093bf339e6becf2eb902de`](https://github.com/dolthub/doltlite/commit/35c2e050927c7f4b29093bf339e6becf2eb902de)
>
> Runner: ubuntu24 20261012.334.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/38881722540)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.07s | 11.26s | 1.1× | 1.5% | **PASS** |
| Writes | 1.98s | 3.08s | 1.6× | 1.3% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.86s | 11.50s | 1.1× | 1.2% | **PASS** |
| Writes | 3.80s | 4.14s | 1.1× | 1.9% | **PASS** |
| Autocommit writes | 837.52ms | 2.63s | 3.1× | 5.4% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.40s | 2.73s | 1.1× | 1.5% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.81s | 3.07s | 1.1× | 1.9% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.29s | 2.45s | 1.1× | 1.6% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.57s | 3.00s | 1.2× | 1.0% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 429.70ms | 708.62ms | 1.6× | 1.5% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 592.79ms | 947.61ms | 1.6× | 1.5% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 467.08ms | 683.48ms | 1.5× | 1.7% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 492.89ms | 743.37ms | 1.5× | 1.0% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.74s | 2.82s | 1.0× | 1.3% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.87s | 3.09s | 1.1× | 1.5% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.57s | 2.55s | 1.0× | 1.2% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.68s | 3.03s | 1.1× | 1.0% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 590.05ms | 785.32ms | 1.3× | 1.6% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 947.93ms | 1.06s | 1.1× | 3.4% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 1.20s | 1.16s | 1.0× | 13.0% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 1.06s | 1.13s | 1.1× | 1.1% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.53s | 2.82s | 1.1× | 1.6% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.75s | 3.09s | 1.1× | 1.9% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.25s | 2.47s | 1.1× | 1.0% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.58s | 3.03s | 1.2× | 0.8% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 198.46ms | 643.03ms | 3.2× | 4.7% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 211.32ms | 672.74ms | 3.2× | 6.2% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 171.43ms | 582.62ms | 3.4× | 26.5% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 256.31ms | 735.43ms | 2.9× | 4.0% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 23.53ms | 31.62ms | 1.3× | 2.1% | PASS |
| mem_reads | `oltp_range_select` | 10.24ms | 12.71ms | 1.2× | 2.6% | PASS |
| mem_reads | `oltp_sum_range` | 9.30ms | 12.03ms | 1.3× | 2.6% | PASS |
| mem_reads | `oltp_order_range` | 2.54ms | 2.98ms | 1.2× | 1.5% | PASS |
| mem_reads | `oltp_distinct_range` | 3.60ms | 4.08ms | 1.1× | 1.2% | PASS |
| mem_reads | `oltp_index_scan` | 3.79ms | 5.49ms | 1.4× | 2.1% | PASS |
| mem_reads | `select_random_points` | 9.72ms | 11.89ms | 1.2× | 4.2% | PASS |
| mem_reads | `select_random_ranges` | 4.50ms | 5.40ms | 1.2× | 2.2% | PASS |
| mem_reads | `covering_index_scan` | 7.58ms | 10.49ms | 1.4× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 29.32ms | 33.25ms | 1.1× | 0.8% | PASS |
| mem_reads | `index_join` | 5.73ms | 8.55ms | 1.5× | 1.4% | PASS |
| mem_reads | `index_join_scan` | 3.18ms | 5.60ms | 1.8× | 3.9% | PASS |
| mem_reads | `types_table_scan` | 1.03s | 1.18s | 1.1× | 0.8% | PASS |
| mem_reads | `table_scan` | 1.16s | 1.29s | 1.1× | 1.2% | PASS |
| mem_reads | `oltp_read_only` | 99.82ms | 124.91ms | 1.3× | 1.0% | PASS |
| mem_writes | `oltp_bulk_insert` | 177.73ms | 277.89ms | 1.6× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 15.05ms | 27.16ms | 1.8× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 48.86ms | 89.25ms | 1.8× | 1.1% | PASS |
| mem_writes | `oltp_update_non_index` | 33.54ms | 56.22ms | 1.7× | 1.6% | PASS |
| mem_writes | `oltp_delete_insert` | 44.28ms | 72.54ms | 1.6× | 1.6% | PASS |
| mem_writes | `oltp_write_only` | 21.27ms | 43.10ms | 2.0× | 1.5% | PASS |
| mem_writes | `types_delete_insert` | 24.31ms | 36.81ms | 1.5× | 1.9% | PASS |
| mem_writes | `oltp_read_write` | 64.66ms | 105.65ms | 1.6× | 1.7% | PASS |
| file_reads | `oltp_point_select` | 91.75ms | 50.09ms | 0.5× | 1.0% | PASS |
| file_reads | `oltp_range_select` | 16.71ms | 14.65ms | 0.9× | 2.0% | PASS |
| file_reads | `oltp_sum_range` | 16.12ms | 14.04ms | 0.9× | 1.4% | PASS |
| file_reads | `oltp_order_range` | 3.28ms | 3.23ms | 1.0× | 1.5% | PASS |
| file_reads | `oltp_distinct_range` | 4.32ms | 4.35ms | 1.0× | 1.5% | PASS |
| file_reads | `oltp_index_scan` | 10.90ms | 7.78ms | 0.7× | 1.2% | PASS |
| file_reads | `select_random_points` | 17.10ms | 13.96ms | 0.8× | 3.2% | PASS |
| file_reads | `select_random_ranges` | 11.58ms | 7.39ms | 0.6× | 1.0% | PASS |
| file_reads | `covering_index_scan` | 14.93ms | 12.74ms | 0.9× | 1.1% | PASS |
| file_reads | `groupby_scan` | 30.76ms | 33.97ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 9.74ms | 10.34ms | 1.1× | 1.9% | PASS |
| file_reads | `index_join_scan` | 4.24ms | 6.00ms | 1.4× | 3.2% | PASS |
| file_reads | `types_table_scan` | 1.12s | 1.20s | 1.1× | 1.0% | PASS |
| file_reads | `table_scan` | 1.18s | 1.29s | 1.1× | 1.1% | PASS |
| file_reads | `oltp_read_only` | 204.51ms | 153.33ms | 0.7× | 1.3% | PASS |
| file_writes | `oltp_bulk_insert` | 192.59ms | 288.10ms | 1.5× | 0.8% | PASS |
| file_writes | `oltp_insert` | 21.50ms | 30.50ms | 1.4× | 1.3% | PASS |
| file_writes | `oltp_update_index` | 74.64ms | 101.21ms | 1.4× | 1.0% | PASS |
| file_writes | `oltp_update_non_index` | 57.13ms | 66.57ms | 1.2× | 1.4% | PASS |
| file_writes | `oltp_delete_insert` | 69.04ms | 85.92ms | 1.2× | 1.8% | PASS |
| file_writes | `oltp_write_only` | 46.01ms | 54.37ms | 1.2× | 2.0% | PASS |
| file_writes | `types_delete_insert` | 39.21ms | 43.09ms | 1.1× | 2.2% | PASS |
| file_writes | `oltp_read_write` | 89.93ms | 115.56ms | 1.3× | 2.1% | PASS |
| ac_reads | `oltp_point_select` | 46.53ms | 50.27ms | 1.1× | 1.4% | PASS |
| ac_reads | `oltp_range_select` | 12.73ms | 14.67ms | 1.2× | 2.1% | PASS |
| ac_reads | `oltp_sum_range` | 11.98ms | 14.09ms | 1.2× | 2.3% | PASS |
| ac_reads | `oltp_order_range` | 2.90ms | 3.25ms | 1.1× | 1.6% | PASS |
| ac_reads | `oltp_distinct_range` | 3.94ms | 4.34ms | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_index_scan` | 6.49ms | 7.79ms | 1.2× | 2.9% | PASS |
| ac_reads | `select_random_points` | 13.72ms | 14.46ms | 1.1× | 2.8% | PASS |
| ac_reads | `select_random_ranges` | 7.04ms | 7.45ms | 1.1× | 1.6% | PASS |
| ac_reads | `covering_index_scan` | 10.08ms | 12.74ms | 1.3× | 1.4% | PASS |
| ac_reads | `groupby_scan` | 29.62ms | 33.66ms | 1.1× | 0.8% | PASS |
| ac_reads | `index_join` | 7.12ms | 10.24ms | 1.4× | 1.6% | PASS |
| ac_reads | `index_join_scan` | 3.72ms | 6.11ms | 1.6× | 2.5% | PASS |
| ac_reads | `types_table_scan` | 1.03s | 1.18s | 1.1× | 1.0% | PASS |
| ac_reads | `table_scan` | 1.21s | 1.30s | 1.1× | 2.5% | PASS |
| ac_reads | `oltp_read_only` | 137.11ms | 153.55ms | 1.1× | 1.3% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 22.89ms | 67.47ms | 2.9× | 6.2% | PASS |
| ac_writes | `oltp_insert_ac` | 25.16ms | 79.60ms | 3.2× | 5.1% | PASS |
| ac_writes | `oltp_update_index_ac` | 25.91ms | 89.73ms | 3.5× | 4.2% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 22.82ms | 73.19ms | 3.2× | 5.4% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.11ms | 88.52ms | 3.4× | 5.4% | PASS |
| ac_writes | `oltp_write_only_ac` | 24.74ms | 84.21ms | 3.4× | 3.8% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.45ms | 72.30ms | 3.4× | 3.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 29.37ms | 88.01ms | 3.0× | 3.6% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 37.32ms | 42.01ms | 1.1× | 2.6% | PASS |
| mem_reads | `oltp_range_select` | 17.68ms | 16.03ms | 0.9× | 2.3% | PASS |
| mem_reads | `oltp_sum_range` | 16.05ms | 15.41ms | 1.0× | 1.4% | PASS |
| mem_reads | `oltp_order_range` | 3.33ms | 3.61ms | 1.1× | 1.5% | PASS |
| mem_reads | `oltp_distinct_range` | 4.45ms | 4.74ms | 1.1× | 1.1% | PASS |
| mem_reads | `oltp_index_scan` | 3.94ms | 6.80ms | 1.7× | 1.3% | PASS |
| mem_reads | `select_random_points` | 22.74ms | 22.97ms | 1.0× | 2.2% | PASS |
| mem_reads | `select_random_ranges` | 6.77ms | 6.99ms | 1.0× | 1.6% | PASS |
| mem_reads | `covering_index_scan` | 7.72ms | 11.14ms | 1.4× | 1.1% | PASS |
| mem_reads | `groupby_scan` | 35.31ms | 36.94ms | 1.0× | 0.7% | PASS |
| mem_reads | `index_join` | 10.77ms | 10.56ms | 1.0× | 2.1% | PASS |
| mem_reads | `index_join_scan` | 3.88ms | 6.58ms | 1.7× | 2.4% | PASS |
| mem_reads | `types_table_scan` | 1.20s | 1.33s | 1.1× | 2.2% | PASS |
| mem_reads | `table_scan` | 1.30s | 1.41s | 1.1× | 4.1% | PASS |
| mem_reads | `oltp_read_only` | 139.01ms | 152.94ms | 1.1× | 1.9% | PASS |
| mem_writes | `oltp_bulk_insert` | 236.25ms | 338.15ms | 1.4× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 17.79ms | 37.84ms | 2.1× | 1.1% | PASS |
| mem_writes | `oltp_update_index` | 68.77ms | 137.26ms | 2.0× | 1.8% | PASS |
| mem_writes | `oltp_update_non_index` | 51.71ms | 83.20ms | 1.6× | 2.1% | PASS |
| mem_writes | `oltp_delete_insert` | 55.02ms | 100.09ms | 1.8× | 1.0% | PASS |
| mem_writes | `oltp_write_only` | 27.79ms | 56.27ms | 2.0× | 1.6% | PASS |
| mem_writes | `types_delete_insert` | 39.10ms | 55.34ms | 1.4× | 1.7% | PASS |
| mem_writes | `oltp_read_write` | 96.35ms | 139.44ms | 1.4× | 1.5% | PASS |
| file_reads | `oltp_point_select` | 105.79ms | 60.14ms | 0.6× | 1.0% | PASS |
| file_reads | `oltp_range_select` | 24.73ms | 17.91ms | 0.7× | 1.9% | PASS |
| file_reads | `oltp_sum_range` | 23.61ms | 17.57ms | 0.7× | 1.5% | PASS |
| file_reads | `oltp_order_range` | 4.30ms | 3.77ms | 0.9× | 1.7% | PASS |
| file_reads | `oltp_distinct_range` | 5.30ms | 4.87ms | 0.9× | 1.2% | PASS |
| file_reads | `oltp_index_scan` | 11.35ms | 8.82ms | 0.8× | 1.4% | PASS |
| file_reads | `select_random_points` | 31.30ms | 25.47ms | 0.8× | 2.0% | PASS |
| file_reads | `select_random_ranges` | 14.27ms | 9.04ms | 0.6× | 1.0% | PASS |
| file_reads | `covering_index_scan` | 15.32ms | 13.01ms | 0.8× | 1.4% | PASS |
| file_reads | `groupby_scan` | 35.67ms | 36.84ms | 1.0× | 0.8% | PASS |
| file_reads | `index_join` | 15.12ms | 11.83ms | 0.8× | 2.0% | PASS |
| file_reads | `index_join_scan` | 4.76ms | 6.83ms | 1.4× | 1.5% | PASS |
| file_reads | `types_table_scan` | 1.10s | 1.31s | 1.2× | 2.0% | PASS |
| file_reads | `table_scan` | 1.23s | 1.39s | 1.1× | 2.0% | PASS |
| file_reads | `oltp_read_only` | 244.68ms | 178.76ms | 0.7× | 1.7% | PASS |
| file_writes | `oltp_bulk_insert` | 262.59ms | 354.94ms | 1.4× | 1.0% | PASS |
| file_writes | `oltp_insert` | 26.41ms | 43.95ms | 1.7× | 1.8% | PASS |
| file_writes | `oltp_update_index` | 137.21ms | 155.09ms | 1.1× | 5.5% | PASS |
| file_writes | `oltp_update_non_index` | 103.32ms | 97.25ms | 0.9× | 12.7% | PASS |
| file_writes | `oltp_delete_insert` | 98.22ms | 117.37ms | 1.2× | 1.5% | PASS |
| file_writes | `oltp_write_only` | 93.20ms | 70.71ms | 0.8× | 9.4% | PASS |
| file_writes | `types_delete_insert` | 72.75ms | 67.53ms | 0.9× | 1.7% | PASS |
| file_writes | `oltp_read_write` | 154.24ms | 152.36ms | 1.0× | 5.0% | PASS |
| ac_reads | `oltp_point_select` | 58.47ms | 60.21ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_range_select` | 19.44ms | 17.97ms | 0.9× | 1.9% | PASS |
| ac_reads | `oltp_sum_range` | 18.30ms | 17.64ms | 1.0× | 2.2% | PASS |
| ac_reads | `oltp_order_range` | 3.71ms | 3.73ms | 1.0× | 2.6% | PASS |
| ac_reads | `oltp_distinct_range` | 4.77ms | 4.86ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 6.52ms | 8.85ms | 1.4× | 2.2% | PASS |
| ac_reads | `select_random_points` | 24.90ms | 25.57ms | 1.0× | 2.4% | PASS |
| ac_reads | `select_random_ranges` | 9.37ms | 8.98ms | 1.0× | 1.4% | PASS |
| ac_reads | `covering_index_scan` | 10.33ms | 13.02ms | 1.3× | 1.5% | PASS |
| ac_reads | `groupby_scan` | 34.97ms | 36.82ms | 1.1× | 0.7% | PASS |
| ac_reads | `index_join` | 12.09ms | 11.77ms | 1.0× | 3.3% | PASS |
| ac_reads | `index_join_scan` | 4.20ms | 6.79ms | 1.6× | 2.1% | PASS |
| ac_reads | `types_table_scan` | 1.08s | 1.29s | 1.2× | 1.3% | PASS |
| ac_reads | `table_scan` | 1.28s | 1.40s | 1.1× | 3.2% | PASS |
| ac_reads | `oltp_read_only` | 177.62ms | 180.34ms | 1.0× | 1.3% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 24.54ms | 67.19ms | 2.7× | 3.8% | PASS |
| ac_writes | `oltp_insert_ac` | 26.04ms | 80.55ms | 3.1× | 5.0% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.14ms | 95.68ms | 3.5× | 6.7% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.21ms | 77.07ms | 3.3× | 5.6% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.61ms | 89.56ms | 3.4× | 6.3% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.80ms | 86.44ms | 3.3× | 6.1% | PASS |
| ac_writes | `types_delete_insert_ac` | 24.15ms | 80.01ms | 3.3× | 7.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 33.84ms | 96.25ms | 2.8× | 6.9% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 28.56ms | 29.90ms | 1.0× | 2.0% | PASS |
| mem_reads | `oltp_range_select` | 14.12ms | 12.06ms | 0.9× | 3.3% | PASS |
| mem_reads | `oltp_sum_range` | 12.09ms | 11.47ms | 0.9× | 1.5% | PASS |
| mem_reads | `oltp_order_range` | 2.82ms | 2.78ms | 1.0× | 1.0% | PASS |
| mem_reads | `oltp_distinct_range` | 3.63ms | 3.60ms | 1.0× | 0.9% | PASS |
| mem_reads | `oltp_index_scan` | 3.18ms | 5.19ms | 1.6× | 1.2% | PASS |
| mem_reads | `select_random_points` | 17.04ms | 17.10ms | 1.0× | 2.1% | PASS |
| mem_reads | `select_random_ranges` | 5.15ms | 5.09ms | 1.0× | 1.7% | PASS |
| mem_reads | `covering_index_scan` | 5.87ms | 7.96ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 29.20ms | 29.56ms | 1.0× | 0.8% | PASS |
| mem_reads | `index_join` | 8.58ms | 8.60ms | 1.0× | 1.6% | PASS |
| mem_reads | `index_join_scan` | 3.08ms | 5.46ms | 1.8× | 1.9% | PASS |
| mem_reads | `types_table_scan` | 910.96ms | 1.04s | 1.1× | 1.5% | PASS |
| mem_reads | `table_scan` | 1.13s | 1.16s | 1.0× | 2.8% | PASS |
| mem_reads | `oltp_read_only` | 113.03ms | 110.93ms | 1.0× | 1.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 184.53ms | 240.05ms | 1.3× | 0.6% | PASS |
| mem_writes | `oltp_insert` | 14.35ms | 28.04ms | 2.0× | 0.7% | PASS |
| mem_writes | `oltp_update_index` | 52.01ms | 97.72ms | 1.9× | 1.9% | PASS |
| mem_writes | `oltp_update_non_index` | 39.39ms | 58.61ms | 1.5× | 2.0% | PASS |
| mem_writes | `oltp_delete_insert` | 43.14ms | 73.89ms | 1.7× | 1.6% | PASS |
| mem_writes | `oltp_write_only` | 23.38ms | 42.88ms | 1.8× | 1.8% | PASS |
| mem_writes | `types_delete_insert` | 31.06ms | 40.14ms | 1.3× | 1.0% | PASS |
| mem_writes | `oltp_read_write` | 79.22ms | 102.14ms | 1.3× | 3.2% | PASS |
| file_reads | `oltp_point_select` | 95.11ms | 46.90ms | 0.5× | 1.4% | PASS |
| file_reads | `oltp_range_select` | 21.17ms | 13.77ms | 0.7× | 1.5% | PASS |
| file_reads | `oltp_sum_range` | 19.86ms | 13.46ms | 0.7× | 1.2% | PASS |
| file_reads | `oltp_order_range` | 3.58ms | 2.99ms | 0.8× | 0.9% | PASS |
| file_reads | `oltp_distinct_range` | 4.37ms | 3.78ms | 0.9× | 0.7% | PASS |
| file_reads | `oltp_index_scan` | 10.10ms | 7.03ms | 0.7× | 0.9% | PASS |
| file_reads | `select_random_points` | 26.07ms | 19.66ms | 0.8× | 2.6% | PASS |
| file_reads | `select_random_ranges` | 12.31ms | 6.96ms | 0.6× | 1.1% | PASS |
| file_reads | `covering_index_scan` | 13.01ms | 9.87ms | 0.8× | 1.2% | PASS |
| file_reads | `groupby_scan` | 30.56ms | 30.11ms | 1.0× | 0.7% | PASS |
| file_reads | `index_join` | 13.53ms | 9.83ms | 0.7× | 4.1% | PASS |
| file_reads | `index_join_scan` | 3.97ms | 5.91ms | 1.5× | 2.1% | PASS |
| file_reads | `types_table_scan` | 982.31ms | 1.09s | 1.1× | 3.8% | PASS |
| file_reads | `table_scan` | 1.13s | 1.16s | 1.0× | 2.8% | PASS |
| file_reads | `oltp_read_only` | 204.32ms | 133.93ms | 0.7× | 0.6% | PASS |
| file_writes | `oltp_bulk_insert` | 251.63ms | 310.35ms | 1.2× | 2.6% | PASS |
| file_writes | `oltp_insert` | 32.11ms | 51.60ms | 1.6× | 14.3% | PASS |
| file_writes | `oltp_update_index` | 160.91ms | 176.08ms | 1.1× | 3.8% | PASS |
| file_writes | `oltp_update_non_index` | 140.74ms | 117.75ms | 0.8× | 12.7% | PASS |
| file_writes | `oltp_delete_insert` | 179.04ms | 152.07ms | 0.8× | 18.0% | PASS |
| file_writes | `oltp_write_only` | 129.94ms | 97.21ms | 0.7× | 13.3% | PASS |
| file_writes | `types_delete_insert` | 127.86ms | 90.36ms | 0.7× | 15.9% | PASS |
| file_writes | `oltp_read_write` | 177.11ms | 162.61ms | 0.9× | 11.9% | PASS |
| ac_reads | `oltp_point_select` | 48.81ms | 46.04ms | 0.9× | 0.7% | PASS |
| ac_reads | `oltp_range_select` | 16.33ms | 13.70ms | 0.8× | 1.2% | PASS |
| ac_reads | `oltp_sum_range` | 14.86ms | 13.21ms | 0.9× | 0.8% | PASS |
| ac_reads | `oltp_order_range` | 3.18ms | 2.99ms | 0.9× | 1.0% | PASS |
| ac_reads | `oltp_distinct_range` | 3.94ms | 3.77ms | 1.0× | 0.7% | PASS |
| ac_reads | `oltp_index_scan` | 5.73ms | 6.92ms | 1.2× | 0.8% | PASS |
| ac_reads | `select_random_points` | 20.17ms | 18.98ms | 0.9× | 1.9% | PASS |
| ac_reads | `select_random_ranges` | 7.78ms | 6.94ms | 0.9× | 1.6% | PASS |
| ac_reads | `covering_index_scan` | 8.55ms | 9.76ms | 1.1× | 1.0% | PASS |
| ac_reads | `groupby_scan` | 29.78ms | 29.91ms | 1.0× | 0.8% | PASS |
| ac_reads | `index_join` | 10.57ms | 9.49ms | 0.9× | 1.2% | PASS |
| ac_reads | `index_join_scan` | 3.50ms | 5.61ms | 1.6× | 1.1% | PASS |
| ac_reads | `types_table_scan` | 881.38ms | 1.03s | 1.2× | 1.5% | PASS |
| ac_reads | `table_scan` | 1.06s | 1.13s | 1.1× | 2.9% | PASS |
| ac_reads | `oltp_read_only` | 137.47ms | 132.92ms | 1.0× | 0.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 17.86ms | 52.73ms | 3.0× | 5.2% | PASS |
| ac_writes | `oltp_insert_ac` | 22.43ms | 71.42ms | 3.2× | 32.3% | PASS |
| ac_writes | `oltp_update_index_ac` | 21.40ms | 79.13ms | 3.7× | 19.3% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 18.09ms | 66.79ms | 3.7× | 26.4% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 21.45ms | 71.78ms | 3.3× | 15.5% | PASS |
| ac_writes | `oltp_write_only_ac` | 23.46ms | 95.13ms | 4.1× | 41.1% | PASS |
| ac_writes | `types_delete_insert_ac` | 19.24ms | 68.70ms | 3.6× | 26.6% | PASS |
| ac_writes | `oltp_read_write_ac` | 27.50ms | 76.95ms | 2.8× | 27.8% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 28.76ms | 33.52ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_range_select` | 17.48ms | 21.57ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_sum_range` | 16.34ms | 19.69ms | 1.2× | 0.7% | PASS |
| mem_reads | `oltp_order_range` | 3.38ms | 3.95ms | 1.2× | 0.6% | PASS |
| mem_reads | `oltp_distinct_range` | 4.31ms | 4.96ms | 1.2× | 0.7% | PASS |
| mem_reads | `oltp_index_scan` | 4.12ms | 5.53ms | 1.3× | 1.9% | PASS |
| mem_reads | `select_random_points` | 26.58ms | 30.80ms | 1.2× | 1.0% | PASS |
| mem_reads | `select_random_ranges` | 6.52ms | 7.88ms | 1.2× | 0.7% | PASS |
| mem_reads | `covering_index_scan` | 6.53ms | 9.06ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 34.62ms | 39.56ms | 1.1× | 0.4% | PASS |
| mem_reads | `index_join` | 7.13ms | 10.58ms | 1.5× | 1.5% | PASS |
| mem_reads | `index_join_scan` | 3.48ms | 5.95ms | 1.7× | 1.3% | PASS |
| mem_reads | `types_table_scan` | 1.05s | 1.27s | 1.2× | 1.2% | PASS |
| mem_reads | `table_scan` | 1.23s | 1.38s | 1.1× | 1.9% | PASS |
| mem_reads | `oltp_read_only` | 133.75ms | 160.81ms | 1.2× | 0.9% | PASS |
| mem_writes | `oltp_bulk_insert` | 201.81ms | 256.45ms | 1.3× | 0.5% | PASS |
| mem_writes | `oltp_insert` | 16.28ms | 27.31ms | 1.7× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 57.67ms | 103.86ms | 1.8× | 1.2% | PASS |
| mem_writes | `oltp_update_non_index` | 42.41ms | 66.69ms | 1.6× | 1.1% | PASS |
| mem_writes | `oltp_delete_insert` | 42.49ms | 76.76ms | 1.8× | 1.0% | PASS |
| mem_writes | `oltp_write_only` | 22.72ms | 44.58ms | 2.0× | 1.4% | PASS |
| mem_writes | `types_delete_insert` | 26.73ms | 42.50ms | 1.6× | 1.2% | PASS |
| mem_writes | `oltp_read_write` | 82.78ms | 125.22ms | 1.5× | 0.9% | PASS |
| file_reads | `oltp_point_select` | 59.42ms | 41.92ms | 0.7× | 1.0% | PASS |
| file_reads | `oltp_range_select` | 20.71ms | 22.61ms | 1.1× | 1.0% | PASS |
| file_reads | `oltp_sum_range` | 19.66ms | 20.81ms | 1.1× | 1.1% | PASS |
| file_reads | `oltp_order_range` | 3.77ms | 4.09ms | 1.1× | 1.0% | PASS |
| file_reads | `oltp_distinct_range` | 4.72ms | 5.14ms | 1.1× | 0.9% | PASS |
| file_reads | `oltp_index_scan` | 7.45ms | 6.65ms | 0.9× | 1.2% | PASS |
| file_reads | `select_random_points` | 29.73ms | 31.40ms | 1.1× | 0.8% | PASS |
| file_reads | `select_random_ranges` | 9.83ms | 8.85ms | 0.9× | 0.9% | PASS |
| file_reads | `covering_index_scan` | 9.94ms | 10.28ms | 1.0× | 0.7% | PASS |
| file_reads | `groupby_scan` | 34.86ms | 39.67ms | 1.1× | 0.6% | PASS |
| file_reads | `index_join` | 9.11ms | 11.44ms | 1.3× | 1.1% | PASS |
| file_reads | `index_join_scan` | 3.91ms | 6.17ms | 1.6× | 1.3% | PASS |
| file_reads | `types_table_scan` | 1.04s | 1.27s | 1.2× | 0.6% | PASS |
| file_reads | `table_scan` | 1.25s | 1.38s | 1.1× | 2.1% | PASS |
| file_reads | `oltp_read_only` | 179.30ms | 172.92ms | 1.0× | 0.7% | PASS |
| file_writes | `oltp_bulk_insert` | 248.18ms | 316.18ms | 1.3× | 2.5% | PASS |
| file_writes | `oltp_insert` | 30.22ms | 40.90ms | 1.4× | 0.9% | PASS |
| file_writes | `oltp_update_index` | 165.36ms | 175.18ms | 1.1× | 1.2% | PASS |
| file_writes | `oltp_update_non_index` | 138.20ms | 121.65ms | 0.9× | 1.0% | PASS |
| file_writes | `oltp_delete_insert` | 138.74ms | 135.69ms | 1.0× | 0.8% | PASS |
| file_writes | `oltp_write_only` | 91.24ms | 91.56ms | 1.0× | 0.9% | PASS |
| file_writes | `types_delete_insert` | 91.05ms | 79.01ms | 0.9× | 1.8% | PASS |
| file_writes | `oltp_read_write` | 160.76ms | 173.78ms | 1.1× | 1.8% | PASS |
| ac_reads | `oltp_point_select` | 37.91ms | 41.85ms | 1.1× | 0.7% | PASS |
| ac_reads | `oltp_range_select` | 18.45ms | 22.68ms | 1.2× | 0.9% | PASS |
| ac_reads | `oltp_sum_range` | 17.34ms | 20.80ms | 1.2× | 0.7% | PASS |
| ac_reads | `oltp_order_range` | 3.58ms | 4.11ms | 1.1× | 1.0% | PASS |
| ac_reads | `oltp_distinct_range` | 4.49ms | 5.11ms | 1.1× | 0.8% | PASS |
| ac_reads | `oltp_index_scan` | 5.39ms | 6.66ms | 1.2× | 1.2% | PASS |
| ac_reads | `select_random_points` | 27.84ms | 31.61ms | 1.1× | 0.8% | PASS |
| ac_reads | `select_random_ranges` | 7.73ms | 8.90ms | 1.2× | 0.6% | PASS |
| ac_reads | `covering_index_scan` | 7.76ms | 10.29ms | 1.3× | 0.7% | PASS |
| ac_reads | `groupby_scan` | 34.81ms | 39.74ms | 1.1× | 0.3% | PASS |
| ac_reads | `index_join` | 8.11ms | 11.47ms | 1.4× | 1.0% | PASS |
| ac_reads | `index_join_scan` | 3.79ms | 6.18ms | 1.6× | 1.1% | PASS |
| ac_reads | `types_table_scan` | 1.04s | 1.27s | 1.2× | 0.6% | PASS |
| ac_reads | `table_scan` | 1.22s | 1.38s | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_read_only` | 146.21ms | 171.77ms | 1.2× | 0.7% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 30.38ms | 77.92ms | 2.6× | 3.9% | PASS |
| ac_writes | `oltp_insert_ac` | 32.28ms | 91.13ms | 2.8× | 3.6% | PASS |
| ac_writes | `oltp_update_index_ac` | 33.41ms | 104.56ms | 3.1× | 5.0% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 30.09ms | 84.86ms | 2.8× | 3.9% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 31.64ms | 93.82ms | 3.0× | 3.7% | PASS |
| ac_writes | `oltp_write_only_ac` | 31.97ms | 95.57ms | 3.0× | 4.1% | PASS |
| ac_writes | `types_delete_insert_ac` | 29.61ms | 85.72ms | 2.9× | 5.7% | PASS |
| ac_writes | `oltp_read_write_ac` | 36.94ms | 101.85ms | 2.8× | 4.2% | PASS |

</details>

</details>

## Version-control latency

Wall time: 6m 46s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 31.20ms | 38.00ms | 82.1% | 1.4% | PASS |
| `status_dirty_many_tables` | 33.65ms | 42.00ms | 80.1% | 0.8% | PASS |
| `diff_regular_working_one_table` | 26.24ms | 33.00ms | 79.5% | 2.5% | PASS |
| `diff_regular_working_many_tables` | 33.86ms | 50.00ms | 67.7% | 0.5% | PASS |
| `diff_stat_working_many_tables` | 33.96ms | 48.00ms | 70.8% | 0.8% | PASS |
| `diff_schema_working_many_tables` | 34.13ms | 48.00ms | 71.1% | 0.3% | PASS |
| `branch_list_many_branches` | 17.63ms | 25.00ms | 70.5% | 0.6% | PASS |
| `branch_create_delete` | 19.01ms | 27.00ms | 70.4% | 1.1% | PASS |
| `at_literal_deep_history` | 19.05ms | 28.00ms | 68.0% | 0.6% | PASS |
| `diff_literal_deep_history` | 19.08ms | 28.00ms | 68.1% | 0.4% | PASS |
| `history_literal_deep_history` | 20.08ms | 30.00ms | 66.9% | 0.5% | PASS |
| `checkout_branch_clean` | 25.97ms | 43.00ms | 60.4% | 0.9% | PASS |
| `merge_data_no_conflicts` | 21.52ms | 33.00ms | 65.2% | 1.1% | PASS |
| `merge_data_secondary_index` | 694.17ms | 944.00ms | 73.5% | 3.0% | PASS |
| `merge_schema_no_conflicts` | 17.16ms | 24.00ms | 71.5% | 1.5% | PASS |
| `merge_data_conflicts` | 23.64ms | 33.00ms | 71.6% | 0.8% | PASS |
| `merge_data_conflicts_with_resolve` | 24.05ms | 34.00ms | 70.7% | 0.4% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
