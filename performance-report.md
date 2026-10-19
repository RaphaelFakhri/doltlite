# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-19 11:18 UTC
>
> Commit: [`df5c8a3e227b32ae5703ee9042b50e49a2a155dd`](https://github.com/dolthub/doltlite/commit/df5c8a3e227b32ae5703ee9042b50e49a2a155dd)
>
> Runner: ubuntu24 20261012.334.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/39232303303)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.97s | 12.50s | 1.1× | 1.6% | **PASS** |
| Writes | 2.24s | 3.59s | 1.6× | 1.5% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 11.87s | 12.75s | 1.1× | 1.5% | **PASS** |
| Writes | 3.17s | 3.98s | 1.3× | 2.1% | **PASS** |
| Autocommit writes | 846.61ms | 2.78s | 3.3× | 8.5% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.55s | 2.87s | 1.1× | 1.6% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.94s | 3.20s | 1.1× | 1.5% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.82s | 3.25s | 1.2× | 1.9% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.66s | 3.19s | 1.2× | 1.4% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 443.57ms | 727.31ms | 1.6× | 2.3% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 592.30ms | 954.90ms | 1.6× | 1.6% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 614.20ms | 976.80ms | 1.6× | 1.8% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 591.32ms | 927.41ms | 1.6× | 1.1% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.79s | 2.94s | 1.1× | 1.8% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 3.23s | 3.26s | 1.0× | 1.2% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 3.04s | 3.30s | 1.1× | 1.6% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.81s | 3.24s | 1.2× | 1.2% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 617.44ms | 810.07ms | 1.3× | 2.5% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 936.70ms | 1.07s | 1.1× | 4.0% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 855.59ms | 1.09s | 1.3× | 2.1% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 764.28ms | 1.01s | 1.3× | 1.8% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.63s | 2.94s | 1.1× | 1.6% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 3.07s | 3.27s | 1.1× | 1.5% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.89s | 3.30s | 1.1× | 1.8% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.67s | 3.25s | 1.2× | 1.5% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 222.61ms | 721.68ms | 3.2× | 8.7% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 209.98ms | 691.08ms | 3.3× | 10.8% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 215.38ms | 710.85ms | 3.3× | 8.6% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 198.64ms | 651.81ms | 3.3× | 6.9% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 24.51ms | 31.84ms | 1.3× | 1.6% | PASS |
| mem_reads | `oltp_range_select` | 10.72ms | 12.79ms | 1.2× | 2.2% | PASS |
| mem_reads | `oltp_sum_range` | 9.85ms | 12.16ms | 1.2× | 2.0% | PASS |
| mem_reads | `oltp_order_range` | 2.63ms | 3.01ms | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_distinct_range` | 3.71ms | 4.14ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 4.08ms | 5.98ms | 1.5× | 2.4% | PASS |
| mem_reads | `select_random_points` | 11.94ms | 12.31ms | 1.0× | 2.6% | PASS |
| mem_reads | `select_random_ranges` | 4.90ms | 5.52ms | 1.1× | 2.2% | PASS |
| mem_reads | `covering_index_scan` | 7.75ms | 12.30ms | 1.6× | 1.8% | PASS |
| mem_reads | `groupby_scan` | 29.96ms | 33.55ms | 1.1× | 0.6% | PASS |
| mem_reads | `index_join` | 5.81ms | 9.09ms | 1.6× | 1.5% | PASS |
| mem_reads | `index_join_scan` | 3.37ms | 5.71ms | 1.7× | 2.3% | PASS |
| mem_reads | `types_table_scan` | 1.09s | 1.25s | 1.1× | 1.2% | PASS |
| mem_reads | `table_scan` | 1.23s | 1.35s | 1.1× | 1.5% | PASS |
| mem_reads | `oltp_read_only` | 105.04ms | 126.59ms | 1.2× | 1.6% | PASS |
| mem_writes | `oltp_bulk_insert` | 177.44ms | 280.40ms | 1.6× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 15.08ms | 27.59ms | 1.8× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 50.95ms | 92.40ms | 1.8× | 1.5% | PASS |
| mem_writes | `oltp_update_non_index` | 35.62ms | 58.26ms | 1.6× | 2.7% | PASS |
| mem_writes | `oltp_delete_insert` | 47.64ms | 77.11ms | 1.6× | 2.3% | PASS |
| mem_writes | `oltp_write_only` | 23.19ms | 45.64ms | 2.0× | 2.6% | PASS |
| mem_writes | `types_delete_insert` | 25.04ms | 37.46ms | 1.5× | 2.3% | PASS |
| mem_writes | `oltp_read_write` | 68.62ms | 108.45ms | 1.6× | 2.3% | PASS |
| file_reads | `oltp_point_select` | 93.60ms | 50.87ms | 0.5× | 0.8% | PASS |
| file_reads | `oltp_range_select` | 18.09ms | 14.93ms | 0.8× | 2.2% | PASS |
| file_reads | `oltp_sum_range` | 17.30ms | 14.37ms | 0.8× | 1.8% | PASS |
| file_reads | `oltp_order_range` | 3.52ms | 3.32ms | 0.9× | 2.1% | PASS |
| file_reads | `oltp_distinct_range` | 4.58ms | 4.41ms | 1.0× | 1.7% | PASS |
| file_reads | `oltp_index_scan` | 11.30ms | 7.92ms | 0.7× | 1.6% | PASS |
| file_reads | `select_random_points` | 18.39ms | 14.12ms | 0.8× | 2.3% | PASS |
| file_reads | `select_random_ranges` | 11.97ms | 7.50ms | 0.6× | 1.4% | PASS |
| file_reads | `covering_index_scan` | 15.29ms | 14.31ms | 0.9× | 1.3% | PASS |
| file_reads | `groupby_scan` | 30.80ms | 33.88ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 10.01ms | 10.40ms | 1.0× | 2.2% | PASS |
| file_reads | `index_join_scan` | 4.32ms | 6.10ms | 1.4× | 2.0% | PASS |
| file_reads | `types_table_scan` | 1.08s | 1.25s | 1.2× | 1.9% | PASS |
| file_reads | `table_scan` | 1.26s | 1.35s | 1.1× | 1.9% | PASS |
| file_reads | `oltp_read_only` | 209.96ms | 156.76ms | 0.7× | 1.0% | PASS |
| file_writes | `oltp_bulk_insert` | 193.33ms | 292.22ms | 1.5× | 1.5% | PASS |
| file_writes | `oltp_insert` | 22.96ms | 31.31ms | 1.4× | 2.9% | PASS |
| file_writes | `oltp_update_index` | 82.49ms | 110.29ms | 1.3× | 2.2% | PASS |
| file_writes | `oltp_update_non_index` | 61.11ms | 69.40ms | 1.1× | 2.0% | PASS |
| file_writes | `oltp_delete_insert` | 70.61ms | 87.20ms | 1.2× | 2.6% | PASS |
| file_writes | `oltp_write_only` | 47.53ms | 54.86ms | 1.2× | 2.7% | PASS |
| file_writes | `types_delete_insert` | 41.76ms | 44.95ms | 1.1× | 2.7% | PASS |
| file_writes | `oltp_read_write` | 97.66ms | 119.83ms | 1.2× | 2.4% | PASS |
| ac_reads | `oltp_point_select` | 47.82ms | 51.17ms | 1.1× | 1.4% | PASS |
| ac_reads | `oltp_range_select` | 13.31ms | 14.82ms | 1.1× | 2.2% | PASS |
| ac_reads | `oltp_sum_range` | 12.04ms | 14.24ms | 1.2× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 2.98ms | 3.31ms | 1.1× | 2.0% | PASS |
| ac_reads | `oltp_distinct_range` | 4.07ms | 4.44ms | 1.1× | 2.2% | PASS |
| ac_reads | `oltp_index_scan` | 6.61ms | 7.94ms | 1.2× | 1.6% | PASS |
| ac_reads | `select_random_points` | 13.75ms | 14.21ms | 1.0× | 2.1% | PASS |
| ac_reads | `select_random_ranges` | 7.16ms | 7.52ms | 1.1× | 0.9% | PASS |
| ac_reads | `covering_index_scan` | 10.25ms | 14.03ms | 1.4× | 2.1% | PASS |
| ac_reads | `groupby_scan` | 29.91ms | 33.83ms | 1.1× | 0.8% | PASS |
| ac_reads | `index_join` | 7.08ms | 10.32ms | 1.5× | 1.5% | PASS |
| ac_reads | `index_join_scan` | 3.68ms | 6.11ms | 1.7× | 2.1% | PASS |
| ac_reads | `types_table_scan` | 1.04s | 1.24s | 1.2× | 2.2% | PASS |
| ac_reads | `table_scan` | 1.28s | 1.36s | 1.1× | 0.7% | PASS |
| ac_reads | `oltp_read_only` | 144.56ms | 157.51ms | 1.1× | 1.3% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.51ms | 74.31ms | 3.2× | 7.1% | PASS |
| ac_writes | `oltp_insert_ac` | 27.77ms | 89.06ms | 3.2× | 10.8% | PASS |
| ac_writes | `oltp_update_index_ac` | 30.63ms | 104.32ms | 3.4× | 6.8% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.37ms | 84.44ms | 3.5× | 9.9% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 28.36ms | 93.57ms | 3.3× | 8.2% | PASS |
| ac_writes | `oltp_write_only_ac` | 28.95ms | 93.93ms | 3.2× | 9.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 25.41ms | 83.55ms | 3.3× | 9.1% | PASS |
| ac_writes | `oltp_read_write_ac` | 33.60ms | 98.51ms | 2.9× | 8.2% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 35.53ms | 41.43ms | 1.2× | 1.9% | PASS |
| mem_reads | `oltp_range_select` | 16.95ms | 16.28ms | 1.0× | 1.5% | PASS |
| mem_reads | `oltp_sum_range` | 16.35ms | 16.82ms | 1.0× | 1.5% | PASS |
| mem_reads | `oltp_order_range` | 3.38ms | 3.68ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_distinct_range` | 4.42ms | 4.81ms | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_index_scan` | 3.97ms | 6.91ms | 1.7× | 1.5% | PASS |
| mem_reads | `select_random_points` | 22.61ms | 23.30ms | 1.0× | 3.1% | PASS |
| mem_reads | `select_random_ranges` | 6.88ms | 7.08ms | 1.0× | 2.0% | PASS |
| mem_reads | `covering_index_scan` | 7.61ms | 12.19ms | 1.6× | 1.0% | PASS |
| mem_reads | `groupby_scan` | 34.85ms | 37.32ms | 1.1× | 0.8% | PASS |
| mem_reads | `index_join` | 10.66ms | 10.81ms | 1.0× | 2.5% | PASS |
| mem_reads | `index_join_scan` | 3.81ms | 6.76ms | 1.8× | 1.6% | PASS |
| mem_reads | `types_table_scan` | 1.25s | 1.39s | 1.1× | 1.6% | PASS |
| mem_reads | `table_scan` | 1.37s | 1.46s | 1.1× | 1.2% | PASS |
| mem_reads | `oltp_read_only` | 149.56ms | 157.97ms | 1.1× | 1.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 231.75ms | 332.98ms | 1.4× | 1.0% | PASS |
| mem_writes | `oltp_insert` | 17.57ms | 37.98ms | 2.2× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 66.39ms | 138.48ms | 2.1× | 1.9% | PASS |
| mem_writes | `oltp_update_non_index` | 49.41ms | 83.05ms | 1.7× | 2.5% | PASS |
| mem_writes | `oltp_delete_insert` | 55.32ms | 102.64ms | 1.9× | 1.7% | PASS |
| mem_writes | `oltp_write_only` | 28.23ms | 57.75ms | 2.0× | 1.5% | PASS |
| mem_writes | `types_delete_insert` | 39.23ms | 57.07ms | 1.5× | 2.6% | PASS |
| mem_writes | `oltp_read_write` | 104.40ms | 144.94ms | 1.4× | 1.2% | PASS |
| file_reads | `oltp_point_select` | 107.16ms | 60.50ms | 0.6× | 0.8% | PASS |
| file_reads | `oltp_range_select` | 25.29ms | 18.58ms | 0.7× | 2.2% | PASS |
| file_reads | `oltp_sum_range` | 24.24ms | 19.10ms | 0.8× | 1.5% | PASS |
| file_reads | `oltp_order_range` | 4.33ms | 3.79ms | 0.9× | 1.2% | PASS |
| file_reads | `oltp_distinct_range` | 5.33ms | 4.92ms | 0.9× | 1.1% | PASS |
| file_reads | `oltp_index_scan` | 11.30ms | 8.87ms | 0.8× | 1.2% | PASS |
| file_reads | `select_random_points` | 31.80ms | 26.34ms | 0.8× | 2.0% | PASS |
| file_reads | `select_random_ranges` | 14.26ms | 9.16ms | 0.6× | 1.3% | PASS |
| file_reads | `covering_index_scan` | 15.26ms | 14.13ms | 0.9× | 0.9% | PASS |
| file_reads | `groupby_scan` | 35.45ms | 37.51ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 15.15ms | 12.05ms | 0.8× | 1.7% | PASS |
| file_reads | `index_join_scan` | 4.79ms | 7.13ms | 1.5× | 2.7% | PASS |
| file_reads | `types_table_scan` | 1.26s | 1.39s | 1.1× | 1.0% | PASS |
| file_reads | `table_scan` | 1.41s | 1.47s | 1.0× | 1.5% | PASS |
| file_reads | `oltp_read_only` | 257.75ms | 185.98ms | 0.7× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 256.80ms | 349.87ms | 1.4× | 0.9% | PASS |
| file_writes | `oltp_insert` | 26.98ms | 43.49ms | 1.6× | 1.1% | PASS |
| file_writes | `oltp_update_index` | 124.18ms | 161.91ms | 1.3× | 9.4% | PASS |
| file_writes | `oltp_update_non_index` | 100.83ms | 98.32ms | 1.0× | 8.4% | PASS |
| file_writes | `oltp_delete_insert` | 100.78ms | 119.80ms | 1.2× | 1.7% | PASS |
| file_writes | `oltp_write_only` | 93.88ms | 71.56ms | 0.8× | 11.3% | PASS |
| file_writes | `types_delete_insert` | 75.11ms | 68.89ms | 0.9× | 2.1% | PASS |
| file_writes | `oltp_read_write` | 158.14ms | 159.77ms | 1.0× | 5.8% | PASS |
| ac_reads | `oltp_point_select` | 60.80ms | 61.01ms | 1.0× | 1.2% | PASS |
| ac_reads | `oltp_range_select` | 20.49ms | 18.57ms | 0.9× | 2.1% | PASS |
| ac_reads | `oltp_sum_range` | 19.38ms | 19.08ms | 1.0× | 1.5% | PASS |
| ac_reads | `oltp_order_range` | 3.86ms | 3.80ms | 1.0× | 1.6% | PASS |
| ac_reads | `oltp_distinct_range` | 4.85ms | 4.93ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 6.66ms | 8.95ms | 1.3× | 1.5% | PASS |
| ac_reads | `select_random_points` | 26.61ms | 26.67ms | 1.0× | 3.1% | PASS |
| ac_reads | `select_random_ranges` | 9.48ms | 9.17ms | 1.0× | 1.3% | PASS |
| ac_reads | `covering_index_scan` | 10.49ms | 14.25ms | 1.4× | 1.2% | PASS |
| ac_reads | `groupby_scan` | 35.12ms | 37.68ms | 1.1× | 0.6% | PASS |
| ac_reads | `index_join` | 12.80ms | 12.26ms | 1.0× | 2.4% | PASS |
| ac_reads | `index_join_scan` | 4.31ms | 7.21ms | 1.7× | 1.9% | PASS |
| ac_reads | `types_table_scan` | 1.23s | 1.38s | 1.1× | 1.5% | PASS |
| ac_reads | `table_scan` | 1.43s | 1.48s | 1.0× | 0.6% | PASS |
| ac_reads | `oltp_read_only` | 189.13ms | 187.87ms | 1.0× | 0.9% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.89ms | 71.32ms | 3.0× | 11.5% | PASS |
| ac_writes | `oltp_insert_ac` | 27.06ms | 83.18ms | 3.1× | 11.2% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.93ms | 101.52ms | 3.6× | 10.8% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.45ms | 80.69ms | 3.3× | 10.8% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.77ms | 94.98ms | 3.5× | 11.6% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.30ms | 86.27ms | 3.4× | 9.1% | PASS |
| ac_writes | `types_delete_insert_ac` | 22.70ms | 79.35ms | 3.5× | 10.3% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.88ms | 93.76ms | 2.9× | 8.5% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 39.34ms | 43.79ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_range_select` | 17.66ms | 16.73ms | 0.9× | 2.5% | PASS |
| mem_reads | `oltp_sum_range` | 16.09ms | 17.46ms | 1.1× | 1.8% | PASS |
| mem_reads | `oltp_order_range` | 3.31ms | 3.56ms | 1.1× | 2.1% | PASS |
| mem_reads | `oltp_distinct_range` | 4.41ms | 4.71ms | 1.1× | 1.9% | PASS |
| mem_reads | `oltp_index_scan` | 4.18ms | 7.05ms | 1.7× | 4.2% | PASS |
| mem_reads | `select_random_points` | 25.23ms | 24.43ms | 1.0× | 7.3% | PASS |
| mem_reads | `select_random_ranges` | 6.88ms | 7.33ms | 1.1× | 2.6% | PASS |
| mem_reads | `covering_index_scan` | 8.00ms | 12.55ms | 1.6× | 1.8% | PASS |
| mem_reads | `groupby_scan` | 34.45ms | 37.66ms | 1.1× | 0.6% | PASS |
| mem_reads | `index_join` | 11.62ms | 11.92ms | 1.0× | 4.7% | PASS |
| mem_reads | `index_join_scan` | 3.77ms | 6.94ms | 1.8× | 5.2% | PASS |
| mem_reads | `types_table_scan` | 1.18s | 1.41s | 1.2× | 0.6% | PASS |
| mem_reads | `table_scan` | 1.32s | 1.49s | 1.1× | 0.5% | PASS |
| mem_reads | `oltp_read_only` | 148.43ms | 159.20ms | 1.1× | 1.3% | PASS |
| mem_writes | `oltp_bulk_insert` | 239.44ms | 341.70ms | 1.4× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 18.78ms | 39.38ms | 2.1× | 1.2% | PASS |
| mem_writes | `oltp_update_index` | 71.00ms | 141.64ms | 2.0× | 1.6% | PASS |
| mem_writes | `oltp_update_non_index` | 51.78ms | 83.89ms | 1.6× | 2.4% | PASS |
| mem_writes | `oltp_delete_insert` | 55.89ms | 103.65ms | 1.9× | 2.0% | PASS |
| mem_writes | `oltp_write_only` | 29.36ms | 58.81ms | 2.0× | 2.0% | PASS |
| mem_writes | `types_delete_insert` | 39.70ms | 57.71ms | 1.5× | 1.7% | PASS |
| mem_writes | `oltp_read_write` | 108.26ms | 150.02ms | 1.4× | 2.0% | PASS |
| file_reads | `oltp_point_select` | 111.28ms | 62.80ms | 0.6× | 1.4% | PASS |
| file_reads | `oltp_range_select` | 24.85ms | 18.55ms | 0.7× | 1.6% | PASS |
| file_reads | `oltp_sum_range` | 23.57ms | 19.26ms | 0.8× | 2.0% | PASS |
| file_reads | `oltp_order_range` | 4.37ms | 3.82ms | 0.9× | 1.6% | PASS |
| file_reads | `oltp_distinct_range` | 5.48ms | 4.96ms | 0.9× | 1.9% | PASS |
| file_reads | `oltp_index_scan` | 11.61ms | 9.25ms | 0.8× | 1.8% | PASS |
| file_reads | `select_random_points` | 33.16ms | 27.43ms | 0.8× | 1.9% | PASS |
| file_reads | `select_random_ranges` | 14.41ms | 9.42ms | 0.7× | 2.1% | PASS |
| file_reads | `covering_index_scan` | 15.67ms | 14.62ms | 0.9× | 1.3% | PASS |
| file_reads | `groupby_scan` | 35.39ms | 37.95ms | 1.1× | 1.0% | PASS |
| file_reads | `index_join` | 15.40ms | 12.68ms | 0.8× | 2.4% | PASS |
| file_reads | `index_join_scan` | 4.82ms | 7.15ms | 1.5× | 2.8% | PASS |
| file_reads | `types_table_scan` | 1.16s | 1.40s | 1.2× | 0.6% | PASS |
| file_reads | `table_scan` | 1.32s | 1.48s | 1.1× | 1.3% | PASS |
| file_reads | `oltp_read_only` | 260.11ms | 189.57ms | 0.7× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 263.90ms | 358.77ms | 1.4× | 1.2% | PASS |
| file_writes | `oltp_insert` | 26.70ms | 44.59ms | 1.7× | 2.3% | PASS |
| file_writes | `oltp_update_index` | 106.50ms | 160.20ms | 1.5× | 2.7% | PASS |
| file_writes | `oltp_update_non_index` | 90.05ms | 98.23ms | 1.1× | 4.8% | PASS |
| file_writes | `oltp_delete_insert` | 93.58ms | 119.69ms | 1.3× | 2.3% | PASS |
| file_writes | `oltp_write_only` | 62.91ms | 72.47ms | 1.2× | 1.7% | PASS |
| file_writes | `types_delete_insert` | 68.27ms | 68.53ms | 1.0× | 2.0% | PASS |
| file_writes | `oltp_read_write` | 143.67ms | 163.67ms | 1.1× | 1.9% | PASS |
| ac_reads | `oltp_point_select` | 61.97ms | 62.23ms | 1.0× | 1.5% | PASS |
| ac_reads | `oltp_range_select` | 19.90ms | 18.62ms | 0.9× | 2.4% | PASS |
| ac_reads | `oltp_sum_range` | 18.35ms | 19.32ms | 1.1× | 2.3% | PASS |
| ac_reads | `oltp_order_range` | 3.86ms | 3.81ms | 1.0× | 2.3% | PASS |
| ac_reads | `oltp_distinct_range` | 4.89ms | 4.97ms | 1.0× | 1.5% | PASS |
| ac_reads | `oltp_index_scan` | 6.78ms | 9.14ms | 1.3× | 1.8% | PASS |
| ac_reads | `select_random_points` | 26.91ms | 27.46ms | 1.0× | 3.0% | PASS |
| ac_reads | `select_random_ranges` | 9.48ms | 9.42ms | 1.0× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.71ms | 14.62ms | 1.4× | 1.9% | PASS |
| ac_reads | `groupby_scan` | 34.67ms | 37.89ms | 1.1× | 0.8% | PASS |
| ac_reads | `index_join` | 12.68ms | 12.67ms | 1.0× | 3.4% | PASS |
| ac_reads | `index_join_scan` | 4.29ms | 7.25ms | 1.7× | 3.6% | PASS |
| ac_reads | `types_table_scan` | 1.17s | 1.40s | 1.2× | 0.8% | PASS |
| ac_reads | `table_scan` | 1.32s | 1.48s | 1.1× | 0.7% | PASS |
| ac_reads | `oltp_read_only` | 189.38ms | 189.55ms | 1.0× | 0.9% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.43ms | 70.47ms | 3.0× | 8.4% | PASS |
| ac_writes | `oltp_insert_ac` | 27.00ms | 94.27ms | 3.5× | 7.1% | PASS |
| ac_writes | `oltp_update_index_ac` | 29.51ms | 103.22ms | 3.5× | 8.7% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.77ms | 80.28ms | 3.4× | 10.6% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.62ms | 89.72ms | 3.4× | 6.8% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.51ms | 91.96ms | 3.5× | 10.0% | PASS |
| ac_writes | `types_delete_insert_ac` | 24.37ms | 83.86ms | 3.4× | 9.6% | PASS |
| ac_writes | `oltp_read_write_ac` | 34.17ms | 97.07ms | 2.8× | 8.2% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.73ms | 42.50ms | 1.3× | 1.5% | PASS |
| mem_reads | `oltp_range_select` | 19.51ms | 23.36ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_sum_range` | 17.81ms | 23.40ms | 1.3× | 0.9% | PASS |
| mem_reads | `oltp_order_range` | 3.52ms | 4.21ms | 1.2× | 0.8% | PASS |
| mem_reads | `oltp_distinct_range` | 4.64ms | 5.32ms | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_index_scan` | 4.54ms | 6.61ms | 1.5× | 2.0% | PASS |
| mem_reads | `select_random_points` | 28.45ms | 33.54ms | 1.2× | 1.5% | PASS |
| mem_reads | `select_random_ranges` | 7.71ms | 9.63ms | 1.2× | 1.6% | PASS |
| mem_reads | `covering_index_scan` | 7.61ms | 11.98ms | 1.6× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 35.81ms | 41.91ms | 1.2× | 0.6% | PASS |
| mem_reads | `index_join` | 7.88ms | 11.97ms | 1.5× | 2.1% | PASS |
| mem_reads | `index_join_scan` | 3.97ms | 6.48ms | 1.6× | 1.2% | PASS |
| mem_reads | `types_table_scan` | 1.06s | 1.34s | 1.3× | 1.4% | PASS |
| mem_reads | `table_scan` | 1.27s | 1.44s | 1.1× | 2.9% | PASS |
| mem_reads | `oltp_read_only` | 151.35ms | 186.38ms | 1.2× | 1.4% | PASS |
| mem_writes | `oltp_bulk_insert` | 242.85ms | 328.69ms | 1.4× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 18.64ms | 34.26ms | 1.8× | 1.1% | PASS |
| mem_writes | `oltp_update_index` | 67.56ms | 123.88ms | 1.8× | 1.2% | PASS |
| mem_writes | `oltp_update_non_index` | 51.14ms | 82.23ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_delete_insert` | 49.73ms | 92.86ms | 1.9× | 0.9% | PASS |
| mem_writes | `oltp_write_only` | 27.08ms | 54.30ms | 2.0× | 0.8% | PASS |
| mem_writes | `types_delete_insert` | 33.14ms | 53.71ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_read_write` | 101.18ms | 157.48ms | 1.6× | 1.6% | PASS |
| file_reads | `oltp_point_select` | 105.18ms | 62.57ms | 0.6× | 1.2% | PASS |
| file_reads | `oltp_range_select` | 27.86ms | 26.29ms | 0.9× | 1.2% | PASS |
| file_reads | `oltp_sum_range` | 25.47ms | 26.01ms | 1.0× | 1.0% | PASS |
| file_reads | `oltp_order_range` | 4.37ms | 4.56ms | 1.0× | 1.8% | PASS |
| file_reads | `oltp_distinct_range` | 5.51ms | 5.69ms | 1.0× | 1.5% | PASS |
| file_reads | `oltp_index_scan` | 11.73ms | 8.90ms | 0.8× | 1.4% | PASS |
| file_reads | `select_random_points` | 37.29ms | 37.30ms | 1.0× | 1.8% | PASS |
| file_reads | `select_random_ranges` | 15.26ms | 11.98ms | 0.8× | 1.8% | PASS |
| file_reads | `covering_index_scan` | 15.06ms | 14.29ms | 0.9× | 1.2% | PASS |
| file_reads | `groupby_scan` | 36.76ms | 42.69ms | 1.2× | 0.7% | PASS |
| file_reads | `index_join` | 12.09ms | 13.57ms | 1.1× | 1.7% | PASS |
| file_reads | `index_join_scan` | 5.04ms | 6.99ms | 1.4× | 2.4% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.33s | 1.3× | 0.9% | PASS |
| file_reads | `table_scan` | 1.20s | 1.43s | 1.2× | 0.9% | PASS |
| file_reads | `oltp_read_only` | 253.53ms | 218.08ms | 0.9× | 0.6% | PASS |
| file_writes | `oltp_bulk_insert` | 260.03ms | 341.71ms | 1.3× | 1.0% | PASS |
| file_writes | `oltp_insert` | 25.70ms | 37.91ms | 1.5× | 1.6% | PASS |
| file_writes | `oltp_update_index` | 96.10ms | 137.49ms | 1.4× | 1.4% | PASS |
| file_writes | `oltp_update_non_index` | 77.17ms | 93.73ms | 1.2× | 1.9% | PASS |
| file_writes | `oltp_delete_insert` | 76.40ms | 104.78ms | 1.4× | 1.9% | PASS |
| file_writes | `oltp_write_only` | 50.24ms | 63.86ms | 1.3× | 2.0% | PASS |
| file_writes | `types_delete_insert` | 49.58ms | 61.71ms | 1.2× | 1.6% | PASS |
| file_writes | `oltp_read_write` | 129.06ms | 168.02ms | 1.3× | 2.2% | PASS |
| ac_reads | `oltp_point_select` | 57.05ms | 62.91ms | 1.1× | 1.7% | PASS |
| ac_reads | `oltp_range_select` | 22.59ms | 26.16ms | 1.2× | 1.4% | PASS |
| ac_reads | `oltp_sum_range` | 20.69ms | 26.14ms | 1.3× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 3.91ms | 4.59ms | 1.2× | 1.3% | PASS |
| ac_reads | `oltp_distinct_range` | 5.07ms | 5.69ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_index_scan` | 7.20ms | 9.05ms | 1.3× | 1.9% | PASS |
| ac_reads | `select_random_points` | 32.24ms | 37.76ms | 1.2× | 1.8% | PASS |
| ac_reads | `select_random_ranges` | 10.34ms | 12.14ms | 1.2× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.16ms | 14.32ms | 1.4× | 1.4% | PASS |
| ac_reads | `groupby_scan` | 35.89ms | 42.63ms | 1.2× | 0.6% | PASS |
| ac_reads | `index_join` | 9.56ms | 13.72ms | 1.4× | 2.7% | PASS |
| ac_reads | `index_join_scan` | 4.47ms | 7.06ms | 1.6× | 3.0% | PASS |
| ac_reads | `types_table_scan` | 1.06s | 1.33s | 1.3× | 1.0% | PASS |
| ac_reads | `table_scan` | 1.21s | 1.43s | 1.2× | 1.9% | PASS |
| ac_reads | `oltp_read_only` | 187.18ms | 219.51ms | 1.2× | 1.8% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 21.94ms | 67.54ms | 3.1× | 7.1% | PASS |
| ac_writes | `oltp_insert_ac` | 23.76ms | 80.28ms | 3.4× | 4.7% | PASS |
| ac_writes | `oltp_update_index_ac` | 26.16ms | 92.16ms | 3.5× | 5.9% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.01ms | 75.44ms | 3.1× | 7.2% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 23.94ms | 83.55ms | 3.5× | 4.2% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.04ms | 84.92ms | 3.3× | 7.5% | PASS |
| ac_writes | `types_delete_insert_ac` | 22.15ms | 76.53ms | 3.5× | 7.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 30.64ms | 91.38ms | 3.0× | 6.7% | PASS |

</details>

</details>

## Version-control latency

Wall time: 4m 15s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 33.61ms | 38.00ms | 88.4% | 0.3% | PASS |
| `status_dirty_many_tables` | 37.13ms | 42.00ms | 88.4% | 0.4% | PASS |
| `diff_regular_working_one_table` | 28.87ms | 33.00ms | 87.5% | 0.3% | PASS |
| `diff_regular_working_many_tables` | 42.20ms | 50.00ms | 84.4% | 0.3% | PASS |
| `diff_stat_working_many_tables` | 42.08ms | 48.00ms | 87.7% | 0.3% | PASS |
| `diff_schema_working_many_tables` | 42.51ms | 48.00ms | 88.6% | 0.4% | PASS |
| `branch_list_many_branches` | 21.38ms | 25.00ms | 85.5% | 0.3% | PASS |
| `branch_create_delete` | 22.82ms | 27.00ms | 84.5% | 0.3% | PASS |
| `at_literal_deep_history` | 23.10ms | 28.00ms | 82.5% | 0.3% | PASS |
| `diff_literal_deep_history` | 23.14ms | 28.00ms | 82.7% | 0.3% | PASS |
| `history_literal_deep_history` | 24.22ms | 30.00ms | 80.7% | 0.2% | PASS |
| `checkout_branch_clean` | 31.54ms | 43.00ms | 73.3% | 0.3% | PASS |
| `merge_data_no_conflicts` | 26.28ms | 33.00ms | 79.6% | 0.4% | PASS |
| `merge_data_secondary_index` | 802.19ms | 944.00ms | 85.0% | 0.3% | PASS |
| `merge_schema_no_conflicts` | 20.40ms | 24.00ms | 85.0% | 0.5% | PASS |
| `merge_data_conflicts` | 29.14ms | 33.00ms | 88.3% | 0.3% | PASS |
| `merge_data_conflicts_with_resolve` | 29.93ms | 34.00ms | 88.0% | 0.3% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
