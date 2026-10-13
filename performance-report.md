# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-13 11:15 UTC
>
> Commit: [`220ce51e12ccf89ae28c34ef71e5a553e903a03d`](https://github.com/dolthub/doltlite/commit/220ce51e12ccf89ae28c34ef71e5a553e903a03d)
>
> Runner: ubuntu24 20261004.327.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/38378301931)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 9.65s | 11.20s | 1.2× | 1.5% | **PASS** |
| Writes | 2.04s | 3.26s | 1.6× | 1.5% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.77s | 11.56s | 1.1× | 1.3% | **PASS** |
| Writes | 3.26s | 3.84s | 1.2× | 2.2% | **PASS** |
| Autocommit writes | 758.28ms | 2.41s | 3.2× | 5.5% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 1.64s | 1.75s | 1.1× | 1.7% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.57s | 3.08s | 1.2× | 1.3% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.77s | 3.21s | 1.2× | 1.8% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.67s | 3.16s | 1.2× | 1.4% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 271.97ms | 421.07ms | 1.5× | 1.9% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 572.96ms | 959.76ms | 1.7× | 1.0% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 605.40ms | 944.22ms | 1.6× | 1.6% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 588.68ms | 937.60ms | 1.6× | 1.4% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 1.81s | 1.80s | 1.0× | 1.9% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.88s | 3.17s | 1.1× | 1.6% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 3.26s | 3.38s | 1.0× | 1.1% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.82s | 3.21s | 1.1× | 1.1% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 733.69ms | 707.30ms | 1.0× | 5.0% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 941.33ms | 1.09s | 1.2× | 5.9% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 830.04ms | 1.03s | 1.2× | 2.1% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 759.40ms | 1.01s | 1.3× | 1.4% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 1.71s | 1.81s | 1.1× | 1.9% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.68s | 3.16s | 1.2× | 1.5% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 3.03s | 3.36s | 1.1× | 1.0% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.70s | 3.22s | 1.2× | 1.3% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 182.82ms | 535.40ms | 2.9× | 4.6% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 215.80ms | 676.65ms | 3.1× | 7.6% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 153.39ms | 549.12ms | 3.6× | 4.5% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 206.27ms | 652.33ms | 3.2× | 5.9% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 15.93ms | 18.43ms | 1.2× | 2.3% | PASS |
| mem_reads | `oltp_range_select` | 6.51ms | 7.26ms | 1.1× | 2.0% | PASS |
| mem_reads | `oltp_sum_range` | 6.17ms | 7.36ms | 1.2× | 1.7% | PASS |
| mem_reads | `oltp_order_range` | 1.74ms | 1.86ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_distinct_range` | 2.24ms | 2.40ms | 1.1× | 1.5% | PASS |
| mem_reads | `oltp_index_scan` | 2.54ms | 3.73ms | 1.5× | 2.4% | PASS |
| mem_reads | `select_random_points` | 8.32ms | 8.93ms | 1.1× | 3.9% | PASS |
| mem_reads | `select_random_ranges` | 2.92ms | 3.14ms | 1.1× | 1.6% | PASS |
| mem_reads | `covering_index_scan` | 4.10ms | 5.44ms | 1.3× | 2.6% | PASS |
| mem_reads | `groupby_scan` | 17.47ms | 19.38ms | 1.1× | 1.4% | PASS |
| mem_reads | `index_join` | 3.56ms | 5.22ms | 1.5× | 2.6% | PASS |
| mem_reads | `index_join_scan` | 2.34ms | 4.19ms | 1.8× | 4.4% | PASS |
| mem_reads | `types_table_scan` | 705.60ms | 757.45ms | 1.1× | 1.3% | PASS |
| mem_reads | `table_scan` | 802.10ms | 839.50ms | 1.0× | 1.2% | PASS |
| mem_reads | `oltp_read_only` | 58.78ms | 67.02ms | 1.1× | 1.4% | PASS |
| mem_writes | `oltp_bulk_insert` | 108.70ms | 160.92ms | 1.5× | 0.8% | PASS |
| mem_writes | `oltp_insert` | 9.46ms | 16.44ms | 1.7× | 2.0% | PASS |
| mem_writes | `oltp_update_index` | 33.35ms | 57.69ms | 1.7× | 1.4% | PASS |
| mem_writes | `oltp_update_non_index` | 23.78ms | 35.44ms | 1.5× | 1.7% | PASS |
| mem_writes | `oltp_delete_insert` | 28.46ms | 43.43ms | 1.5× | 2.7% | PASS |
| mem_writes | `oltp_write_only` | 14.00ms | 26.50ms | 1.9× | 2.2% | PASS |
| mem_writes | `types_delete_insert` | 15.99ms | 21.78ms | 1.4× | 3.5% | PASS |
| mem_writes | `oltp_read_write` | 38.22ms | 58.87ms | 1.5× | 1.8% | PASS |
| file_reads | `oltp_point_select` | 69.72ms | 33.13ms | 0.5× | 1.0% | PASS |
| file_reads | `oltp_range_select` | 12.25ms | 8.78ms | 0.7× | 2.6% | PASS |
| file_reads | `oltp_sum_range` | 11.63ms | 8.73ms | 0.8× | 2.0% | PASS |
| file_reads | `oltp_order_range` | 2.30ms | 1.99ms | 0.9× | 2.4% | PASS |
| file_reads | `oltp_distinct_range` | 2.80ms | 2.59ms | 0.9× | 1.9% | PASS |
| file_reads | `oltp_index_scan` | 8.39ms | 5.28ms | 0.6× | 2.8% | PASS |
| file_reads | `select_random_points` | 13.92ms | 10.49ms | 0.8× | 2.4% | PASS |
| file_reads | `select_random_ranges` | 8.53ms | 4.67ms | 0.5× | 1.1% | PASS |
| file_reads | `covering_index_scan` | 10.29ms | 7.65ms | 0.7× | 1.5% | PASS |
| file_reads | `groupby_scan` | 18.29ms | 19.50ms | 1.1× | 1.2% | PASS |
| file_reads | `index_join` | 6.88ms | 7.08ms | 1.0× | 4.6% | PASS |
| file_reads | `index_join_scan` | 3.03ms | 4.71ms | 1.6× | 4.1% | PASS |
| file_reads | `types_table_scan` | 708.44ms | 756.86ms | 1.1× | 0.8% | PASS |
| file_reads | `table_scan` | 799.22ms | 843.19ms | 1.1× | 1.5% | PASS |
| file_reads | `oltp_read_only` | 138.18ms | 88.65ms | 0.6× | 1.9% | PASS |
| file_writes | `oltp_bulk_insert` | 150.15ms | 205.46ms | 1.4× | 4.1% | PASS |
| file_writes | `oltp_insert` | 19.90ms | 27.32ms | 1.4× | 7.5% | PASS |
| file_writes | `oltp_update_index` | 121.53ms | 109.01ms | 0.9× | 7.1% | PASS |
| file_writes | `oltp_update_non_index` | 101.17ms | 78.39ms | 0.8× | 1.0% | PASS |
| file_writes | `oltp_delete_insert` | 107.45ms | 90.11ms | 0.8× | 4.2% | PASS |
| file_writes | `oltp_write_only` | 72.12ms | 60.19ms | 0.8× | 4.1% | PASS |
| file_writes | `types_delete_insert` | 63.23ms | 46.20ms | 0.7× | 8.7% | PASS |
| file_writes | `oltp_read_write` | 98.14ms | 90.61ms | 0.9× | 5.7% | PASS |
| ac_reads | `oltp_point_select` | 33.34ms | 33.10ms | 1.0× | 1.6% | PASS |
| ac_reads | `oltp_range_select` | 8.54ms | 8.78ms | 1.0× | 3.4% | PASS |
| ac_reads | `oltp_sum_range` | 8.03ms | 8.79ms | 1.1× | 2.0% | PASS |
| ac_reads | `oltp_order_range` | 1.95ms | 2.01ms | 1.0× | 2.8% | PASS |
| ac_reads | `oltp_distinct_range` | 2.51ms | 2.58ms | 1.0× | 2.1% | PASS |
| ac_reads | `oltp_index_scan` | 4.90ms | 5.39ms | 1.1× | 1.4% | PASS |
| ac_reads | `select_random_points` | 10.31ms | 10.53ms | 1.0× | 2.5% | PASS |
| ac_reads | `select_random_ranges` | 4.91ms | 4.70ms | 1.0× | 0.8% | PASS |
| ac_reads | `covering_index_scan` | 6.61ms | 7.73ms | 1.2× | 1.9% | PASS |
| ac_reads | `groupby_scan` | 17.84ms | 19.45ms | 1.1× | 1.1% | PASS |
| ac_reads | `index_join` | 5.06ms | 7.13ms | 1.4× | 4.1% | PASS |
| ac_reads | `index_join_scan` | 2.63ms | 4.69ms | 1.8× | 4.0% | PASS |
| ac_reads | `types_table_scan` | 710.97ms | 760.28ms | 1.1× | 1.1% | PASS |
| ac_reads | `table_scan` | 804.89ms | 843.39ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_read_only` | 84.29ms | 87.28ms | 1.0× | 1.0% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 20.11ms | 54.27ms | 2.7× | 4.0% | PASS |
| ac_writes | `oltp_insert_ac` | 22.95ms | 65.89ms | 2.9× | 4.4% | PASS |
| ac_writes | `oltp_update_index_ac` | 24.65ms | 76.16ms | 3.1× | 4.8% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.21ms | 61.76ms | 2.9× | 6.0% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 23.15ms | 70.36ms | 3.0× | 3.5% | PASS |
| ac_writes | `oltp_write_only_ac` | 23.54ms | 70.74ms | 3.0× | 3.3% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.25ms | 63.56ms | 3.0× | 7.2% | PASS |
| ac_writes | `oltp_read_write_ac` | 25.95ms | 72.68ms | 2.8× | 4.9% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.50ms | 41.95ms | 1.3× | 1.5% | PASS |
| mem_reads | `oltp_range_select` | 16.11ms | 16.88ms | 1.0× | 1.3% | PASS |
| mem_reads | `oltp_sum_range` | 15.24ms | 15.76ms | 1.0× | 1.5% | PASS |
| mem_reads | `oltp_order_range` | 3.36ms | 3.62ms | 1.1× | 1.5% | PASS |
| mem_reads | `oltp_distinct_range` | 4.37ms | 4.83ms | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_index_scan` | 3.87ms | 6.92ms | 1.8× | 1.3% | PASS |
| mem_reads | `select_random_points` | 20.77ms | 23.67ms | 1.1× | 1.7% | PASS |
| mem_reads | `select_random_ranges` | 6.56ms | 7.28ms | 1.1× | 1.8% | PASS |
| mem_reads | `covering_index_scan` | 7.65ms | 11.16ms | 1.5× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 33.94ms | 37.89ms | 1.1× | 0.7% | PASS |
| mem_reads | `index_join` | 9.93ms | 10.80ms | 1.1× | 1.4% | PASS |
| mem_reads | `index_join_scan` | 3.62ms | 6.65ms | 1.8× | 1.7% | PASS |
| mem_reads | `types_table_scan` | 1.08s | 1.32s | 1.2× | 1.3% | PASS |
| mem_reads | `table_scan` | 1.20s | 1.41s | 1.2× | 0.7% | PASS |
| mem_reads | `oltp_read_only` | 135.78ms | 155.01ms | 1.1× | 1.1% | PASS |
| mem_writes | `oltp_bulk_insert` | 235.95ms | 343.38ms | 1.5× | 2.0% | PASS |
| mem_writes | `oltp_insert` | 17.63ms | 38.24ms | 2.2× | 1.5% | PASS |
| mem_writes | `oltp_update_index` | 63.13ms | 137.02ms | 2.2× | 1.2% | PASS |
| mem_writes | `oltp_update_non_index` | 45.91ms | 83.49ms | 1.8× | 0.8% | PASS |
| mem_writes | `oltp_delete_insert` | 52.21ms | 100.96ms | 1.9× | 1.1% | PASS |
| mem_writes | `oltp_write_only` | 26.88ms | 56.95ms | 2.1× | 1.0% | PASS |
| mem_writes | `types_delete_insert` | 36.80ms | 56.34ms | 1.5× | 0.9% | PASS |
| mem_writes | `oltp_read_write` | 94.46ms | 143.38ms | 1.5× | 1.0% | PASS |
| file_reads | `oltp_point_select` | 104.47ms | 62.50ms | 0.6× | 0.7% | PASS |
| file_reads | `oltp_range_select` | 23.72ms | 19.14ms | 0.8× | 1.8% | PASS |
| file_reads | `oltp_sum_range` | 22.87ms | 18.11ms | 0.8× | 1.8% | PASS |
| file_reads | `oltp_order_range` | 4.11ms | 3.88ms | 0.9× | 2.0% | PASS |
| file_reads | `oltp_distinct_range` | 5.23ms | 5.10ms | 1.0× | 1.3% | PASS |
| file_reads | `oltp_index_scan` | 11.25ms | 9.15ms | 0.8× | 1.5% | PASS |
| file_reads | `select_random_points` | 30.67ms | 26.92ms | 0.9× | 2.3% | PASS |
| file_reads | `select_random_ranges` | 14.43ms | 9.58ms | 0.7× | 1.6% | PASS |
| file_reads | `covering_index_scan` | 15.34ms | 13.40ms | 0.9× | 1.2% | PASS |
| file_reads | `groupby_scan` | 34.86ms | 38.36ms | 1.1× | 0.8% | PASS |
| file_reads | `index_join` | 14.28ms | 12.14ms | 0.9× | 2.6% | PASS |
| file_reads | `index_join_scan` | 4.67ms | 6.97ms | 1.5× | 2.0% | PASS |
| file_reads | `types_table_scan` | 1.06s | 1.32s | 1.2× | 0.8% | PASS |
| file_reads | `table_scan` | 1.29s | 1.43s | 1.1× | 1.7% | PASS |
| file_reads | `oltp_read_only` | 243.83ms | 186.43ms | 0.8× | 1.0% | PASS |
| file_writes | `oltp_bulk_insert` | 261.69ms | 365.20ms | 1.4× | 2.2% | PASS |
| file_writes | `oltp_insert` | 25.57ms | 44.23ms | 1.7× | 1.9% | PASS |
| file_writes | `oltp_update_index` | 120.44ms | 157.22ms | 1.3× | 11.7% | PASS |
| file_writes | `oltp_update_non_index` | 106.62ms | 98.10ms | 0.9× | 9.6% | PASS |
| file_writes | `oltp_delete_insert` | 96.16ms | 117.70ms | 1.2× | 1.8% | PASS |
| file_writes | `oltp_write_only` | 84.85ms | 71.07ms | 0.8× | 12.5% | PASS |
| file_writes | `types_delete_insert` | 73.94ms | 70.31ms | 1.0× | 2.4% | PASS |
| file_writes | `oltp_read_write` | 172.07ms | 166.44ms | 1.0× | 9.4% | PASS |
| ac_reads | `oltp_point_select` | 62.24ms | 65.12ms | 1.0× | 1.5% | PASS |
| ac_reads | `oltp_range_select` | 20.24ms | 19.32ms | 1.0× | 2.1% | PASS |
| ac_reads | `oltp_sum_range` | 19.14ms | 18.67ms | 1.0× | 1.9% | PASS |
| ac_reads | `oltp_order_range` | 3.82ms | 3.90ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_distinct_range` | 4.82ms | 5.13ms | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_index_scan` | 6.61ms | 9.18ms | 1.4× | 1.8% | PASS |
| ac_reads | `select_random_points` | 24.81ms | 26.95ms | 1.1× | 2.3% | PASS |
| ac_reads | `select_random_ranges` | 9.26ms | 9.53ms | 1.0× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.36ms | 13.39ms | 1.3× | 1.2% | PASS |
| ac_reads | `groupby_scan` | 34.45ms | 38.37ms | 1.1× | 0.7% | PASS |
| ac_reads | `index_join` | 12.32ms | 12.15ms | 1.0× | 2.1% | PASS |
| ac_reads | `index_join_scan` | 4.17ms | 7.02ms | 1.7× | 2.0% | PASS |
| ac_reads | `types_table_scan` | 1.07s | 1.32s | 1.2× | 1.0% | PASS |
| ac_reads | `table_scan` | 1.22s | 1.42s | 1.2× | 1.4% | PASS |
| ac_reads | `oltp_read_only` | 184.16ms | 190.69ms | 1.0× | 1.8% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 24.64ms | 68.90ms | 2.8× | 8.0% | PASS |
| ac_writes | `oltp_insert_ac` | 27.56ms | 80.83ms | 2.9× | 7.3% | PASS |
| ac_writes | `oltp_update_index_ac` | 29.50ms | 99.78ms | 3.4× | 6.7% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.33ms | 78.67ms | 3.2× | 7.8% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.87ms | 89.92ms | 3.5× | 5.5% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.49ms | 87.17ms | 3.3× | 8.6% | PASS |
| ac_writes | `types_delete_insert_ac` | 23.99ms | 79.44ms | 3.3× | 7.9% | PASS |
| ac_writes | `oltp_read_write_ac` | 33.42ms | 91.95ms | 2.8× | 4.9% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 34.97ms | 37.24ms | 1.1× | 2.5% | PASS |
| mem_reads | `oltp_range_select` | 17.15ms | 15.31ms | 0.9× | 2.3% | PASS |
| mem_reads | `oltp_sum_range` | 15.06ms | 14.79ms | 1.0× | 1.1% | PASS |
| mem_reads | `oltp_order_range` | 3.48ms | 3.53ms | 1.0× | 1.5% | PASS |
| mem_reads | `oltp_distinct_range` | 4.53ms | 4.72ms | 1.0× | 0.7% | PASS |
| mem_reads | `oltp_index_scan` | 3.95ms | 6.50ms | 1.6× | 0.9% | PASS |
| mem_reads | `select_random_points` | 21.58ms | 21.90ms | 1.0× | 2.9% | PASS |
| mem_reads | `select_random_ranges` | 6.57ms | 6.46ms | 1.0× | 1.5% | PASS |
| mem_reads | `covering_index_scan` | 7.57ms | 10.44ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 36.90ms | 37.55ms | 1.0× | 0.6% | PASS |
| mem_reads | `index_join` | 10.68ms | 10.88ms | 1.0× | 2.1% | PASS |
| mem_reads | `index_join_scan` | 3.82ms | 6.79ms | 1.8× | 1.8% | PASS |
| mem_reads | `types_table_scan` | 1.15s | 1.39s | 1.2× | 2.2% | PASS |
| mem_reads | `table_scan` | 1.31s | 1.50s | 1.1× | 1.9% | PASS |
| mem_reads | `oltp_read_only` | 140.75ms | 143.25ms | 1.0× | 2.0% | PASS |
| mem_writes | `oltp_bulk_insert` | 237.64ms | 334.46ms | 1.4× | 2.4% | PASS |
| mem_writes | `oltp_insert` | 18.53ms | 38.70ms | 2.1× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 67.15ms | 132.92ms | 2.0× | 1.5% | PASS |
| mem_writes | `oltp_update_non_index` | 49.97ms | 80.71ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_delete_insert` | 54.54ms | 101.27ms | 1.9× | 1.7% | PASS |
| mem_writes | `oltp_write_only` | 29.34ms | 59.88ms | 2.0× | 1.9% | PASS |
| mem_writes | `types_delete_insert` | 40.19ms | 56.08ms | 1.4× | 1.8% | PASS |
| mem_writes | `oltp_read_write` | 108.05ms | 140.19ms | 1.3× | 1.4% | PASS |
| file_reads | `oltp_point_select` | 121.88ms | 59.91ms | 0.5× | 1.1% | PASS |
| file_reads | `oltp_range_select` | 27.31ms | 17.98ms | 0.7× | 1.2% | PASS |
| file_reads | `oltp_sum_range` | 25.58ms | 17.55ms | 0.7× | 1.2% | PASS |
| file_reads | `oltp_order_range` | 4.46ms | 3.78ms | 0.8× | 0.8% | PASS |
| file_reads | `oltp_distinct_range` | 5.48ms | 4.98ms | 0.9× | 0.8% | PASS |
| file_reads | `oltp_index_scan` | 12.85ms | 8.92ms | 0.7× | 1.0% | PASS |
| file_reads | `select_random_points` | 33.62ms | 25.65ms | 0.8× | 2.0% | PASS |
| file_reads | `select_random_ranges` | 15.61ms | 8.85ms | 0.6× | 1.2% | PASS |
| file_reads | `covering_index_scan` | 16.62ms | 12.69ms | 0.8× | 0.7% | PASS |
| file_reads | `groupby_scan` | 38.43ms | 38.38ms | 1.0× | 0.6% | PASS |
| file_reads | `index_join` | 16.47ms | 12.47ms | 0.8× | 1.4% | PASS |
| file_reads | `index_join_scan` | 4.94ms | 7.31ms | 1.5× | 1.9% | PASS |
| file_reads | `types_table_scan` | 1.25s | 1.44s | 1.2× | 1.3% | PASS |
| file_reads | `table_scan` | 1.41s | 1.54s | 1.1× | 0.5% | PASS |
| file_reads | `oltp_read_only` | 274.20ms | 180.81ms | 0.7× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 261.67ms | 345.48ms | 1.3× | 2.1% | PASS |
| file_writes | `oltp_insert` | 25.14ms | 43.40ms | 1.7× | 1.4% | PASS |
| file_writes | `oltp_update_index` | 100.08ms | 150.03ms | 1.5× | 2.0% | PASS |
| file_writes | `oltp_update_non_index` | 97.10ms | 94.10ms | 1.0× | 11.2% | PASS |
| file_writes | `oltp_delete_insert` | 88.22ms | 114.44ms | 1.3× | 2.0% | PASS |
| file_writes | `oltp_write_only` | 58.63ms | 71.89ms | 1.2× | 2.2% | PASS |
| file_writes | `types_delete_insert` | 65.10ms | 64.78ms | 1.0× | 2.1% | PASS |
| file_writes | `oltp_read_write` | 134.11ms | 149.12ms | 1.1× | 2.3% | PASS |
| ac_reads | `oltp_point_select` | 64.34ms | 60.01ms | 0.9× | 1.5% | PASS |
| ac_reads | `oltp_range_select` | 21.11ms | 17.80ms | 0.8× | 1.5% | PASS |
| ac_reads | `oltp_sum_range` | 19.11ms | 17.29ms | 0.9× | 0.9% | PASS |
| ac_reads | `oltp_order_range` | 3.94ms | 3.77ms | 1.0× | 0.9% | PASS |
| ac_reads | `oltp_distinct_range` | 4.89ms | 4.98ms | 1.0× | 0.8% | PASS |
| ac_reads | `oltp_index_scan` | 7.13ms | 8.79ms | 1.2× | 1.2% | PASS |
| ac_reads | `select_random_points` | 26.36ms | 25.20ms | 1.0× | 1.9% | PASS |
| ac_reads | `select_random_ranges` | 9.77ms | 8.78ms | 0.9× | 1.0% | PASS |
| ac_reads | `covering_index_scan` | 10.83ms | 12.64ms | 1.2× | 0.8% | PASS |
| ac_reads | `groupby_scan` | 37.34ms | 38.15ms | 1.0× | 0.5% | PASS |
| ac_reads | `index_join` | 13.04ms | 12.08ms | 0.9× | 1.6% | PASS |
| ac_reads | `index_join_scan` | 4.36ms | 7.04ms | 1.6× | 1.0% | PASS |
| ac_reads | `types_table_scan` | 1.22s | 1.43s | 1.2× | 1.8% | PASS |
| ac_reads | `table_scan` | 1.40s | 1.53s | 1.1× | 0.5% | PASS |
| ac_reads | `oltp_read_only` | 187.78ms | 177.79ms | 0.9× | 1.1% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 16.90ms | 51.64ms | 3.1× | 3.7% | PASS |
| ac_writes | `oltp_insert_ac` | 18.92ms | 68.36ms | 3.6× | 3.7% | PASS |
| ac_writes | `oltp_update_index_ac` | 20.27ms | 83.27ms | 4.1× | 4.3% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 16.68ms | 63.48ms | 3.8× | 6.7% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 19.01ms | 71.56ms | 3.8× | 4.0% | PASS |
| ac_writes | `oltp_write_only_ac` | 18.93ms | 70.50ms | 3.7× | 4.7% | PASS |
| ac_writes | `types_delete_insert_ac` | 17.05ms | 62.02ms | 3.6× | 5.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 25.63ms | 78.28ms | 3.1× | 4.7% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.84ms | 43.73ms | 1.3× | 1.6% | PASS |
| mem_reads | `oltp_range_select` | 19.74ms | 24.55ms | 1.2× | 2.4% | PASS |
| mem_reads | `oltp_sum_range` | 17.77ms | 23.09ms | 1.3× | 1.7% | PASS |
| mem_reads | `oltp_order_range` | 3.55ms | 4.31ms | 1.2× | 1.4% | PASS |
| mem_reads | `oltp_distinct_range` | 4.65ms | 5.55ms | 1.2× | 0.9% | PASS |
| mem_reads | `oltp_index_scan` | 4.51ms | 6.58ms | 1.5× | 1.1% | PASS |
| mem_reads | `select_random_points` | 28.27ms | 34.50ms | 1.2× | 1.9% | PASS |
| mem_reads | `select_random_ranges` | 7.64ms | 9.49ms | 1.2× | 1.4% | PASS |
| mem_reads | `covering_index_scan` | 7.62ms | 10.96ms | 1.4× | 1.3% | PASS |
| mem_reads | `groupby_scan` | 35.70ms | 44.04ms | 1.2× | 1.0% | PASS |
| mem_reads | `index_join` | 7.89ms | 12.06ms | 1.5× | 2.0% | PASS |
| mem_reads | `index_join_scan` | 3.97ms | 6.66ms | 1.7× | 1.6% | PASS |
| mem_reads | `types_table_scan` | 1.05s | 1.32s | 1.3× | 0.9% | PASS |
| mem_reads | `table_scan` | 1.29s | 1.43s | 1.1× | 3.4% | PASS |
| mem_reads | `oltp_read_only` | 154.59ms | 192.12ms | 1.2× | 1.3% | PASS |
| mem_writes | `oltp_bulk_insert` | 243.06ms | 337.84ms | 1.4× | 1.8% | PASS |
| mem_writes | `oltp_insert` | 18.61ms | 34.36ms | 1.8× | 1.1% | PASS |
| mem_writes | `oltp_update_index` | 66.45ms | 121.32ms | 1.8× | 1.5% | PASS |
| mem_writes | `oltp_update_non_index` | 51.05ms | 83.10ms | 1.6× | 1.5% | PASS |
| mem_writes | `oltp_delete_insert` | 49.10ms | 92.58ms | 1.9× | 1.5% | PASS |
| mem_writes | `oltp_write_only` | 26.90ms | 54.14ms | 2.0× | 1.2% | PASS |
| mem_writes | `types_delete_insert` | 32.96ms | 54.52ms | 1.7× | 1.3% | PASS |
| mem_writes | `oltp_read_write` | 100.54ms | 159.73ms | 1.6× | 1.0% | PASS |
| file_reads | `oltp_point_select` | 103.74ms | 62.23ms | 0.6× | 1.0% | PASS |
| file_reads | `oltp_range_select` | 27.15ms | 26.36ms | 1.0× | 1.6% | PASS |
| file_reads | `oltp_sum_range` | 25.77ms | 25.23ms | 1.0× | 1.5% | PASS |
| file_reads | `oltp_order_range` | 4.47ms | 4.62ms | 1.0× | 1.1% | PASS |
| file_reads | `oltp_distinct_range` | 5.59ms | 5.88ms | 1.1× | 1.3% | PASS |
| file_reads | `oltp_index_scan` | 11.98ms | 8.83ms | 0.7× | 1.1% | PASS |
| file_reads | `select_random_points` | 37.90ms | 37.98ms | 1.0× | 1.8% | PASS |
| file_reads | `select_random_ranges` | 15.30ms | 11.64ms | 0.8× | 1.1% | PASS |
| file_reads | `covering_index_scan` | 15.35ms | 13.14ms | 0.9× | 0.9% | PASS |
| file_reads | `groupby_scan` | 36.73ms | 44.22ms | 1.2× | 0.8% | PASS |
| file_reads | `index_join` | 12.27ms | 13.45ms | 1.1× | 1.6% | PASS |
| file_reads | `index_join_scan` | 4.95ms | 7.03ms | 1.4× | 1.9% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.31s | 1.2× | 0.8% | PASS |
| file_reads | `table_scan` | 1.21s | 1.42s | 1.2× | 1.2% | PASS |
| file_reads | `oltp_read_only` | 258.53ms | 219.55ms | 0.8× | 0.9% | PASS |
| file_writes | `oltp_bulk_insert` | 259.37ms | 347.33ms | 1.3× | 1.4% | PASS |
| file_writes | `oltp_insert` | 25.85ms | 38.13ms | 1.5× | 2.4% | PASS |
| file_writes | `oltp_update_index` | 94.53ms | 134.63ms | 1.4× | 1.3% | PASS |
| file_writes | `oltp_update_non_index` | 76.22ms | 92.51ms | 1.2× | 1.5% | PASS |
| file_writes | `oltp_delete_insert` | 75.73ms | 104.59ms | 1.4× | 1.2% | PASS |
| file_writes | `oltp_write_only` | 50.79ms | 63.83ms | 1.3× | 1.7% | PASS |
| file_writes | `types_delete_insert` | 49.33ms | 62.53ms | 1.3× | 1.6% | PASS |
| file_writes | `oltp_read_write` | 127.58ms | 169.76ms | 1.3× | 1.2% | PASS |
| ac_reads | `oltp_point_select` | 56.36ms | 62.01ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 21.97ms | 26.32ms | 1.2× | 1.5% | PASS |
| ac_reads | `oltp_sum_range` | 20.23ms | 25.27ms | 1.2× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 3.92ms | 4.61ms | 1.2× | 1.7% | PASS |
| ac_reads | `oltp_distinct_range` | 5.01ms | 5.87ms | 1.2× | 1.3% | PASS |
| ac_reads | `oltp_index_scan` | 7.16ms | 8.78ms | 1.2× | 1.1% | PASS |
| ac_reads | `select_random_points` | 31.11ms | 38.05ms | 1.2× | 1.4% | PASS |
| ac_reads | `select_random_ranges` | 10.23ms | 11.61ms | 1.1× | 1.1% | PASS |
| ac_reads | `covering_index_scan` | 10.40ms | 13.25ms | 1.3× | 1.6% | PASS |
| ac_reads | `groupby_scan` | 35.89ms | 44.20ms | 1.2× | 0.5% | PASS |
| ac_reads | `index_join` | 9.59ms | 13.48ms | 1.4× | 1.9% | PASS |
| ac_reads | `index_join_scan` | 4.42ms | 7.08ms | 1.6× | 2.0% | PASS |
| ac_reads | `types_table_scan` | 1.06s | 1.32s | 1.2× | 1.0% | PASS |
| ac_reads | `table_scan` | 1.23s | 1.42s | 1.1× | 1.0% | PASS |
| ac_reads | `oltp_read_only` | 186.12ms | 220.94ms | 1.2× | 1.0% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 22.74ms | 66.59ms | 2.9× | 5.1% | PASS |
| ac_writes | `oltp_insert_ac` | 26.24ms | 82.18ms | 3.1× | 5.9% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.62ms | 93.37ms | 3.4× | 5.5% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.34ms | 76.02ms | 3.3× | 6.3% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.26ms | 83.33ms | 3.3× | 5.9% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.08ms | 83.73ms | 3.2× | 6.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 23.78ms | 75.28ms | 3.2× | 6.6% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.20ms | 91.82ms | 2.9× | 4.6% | PASS |

</details>

</details>

## Version-control latency

Wall time: 7m 0s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 33.10ms | 38.00ms | 87.1% | 0.7% | PASS |
| `status_dirty_many_tables` | 36.06ms | 42.00ms | 85.9% | 0.5% | PASS |
| `diff_regular_working_one_table` | 27.69ms | 33.00ms | 83.9% | 0.8% | PASS |
| `diff_regular_working_many_tables` | 39.72ms | 50.00ms | 79.4% | 0.4% | PASS |
| `diff_stat_working_many_tables` | 39.69ms | 48.00ms | 82.7% | 0.5% | PASS |
| `diff_schema_working_many_tables` | 40.17ms | 48.00ms | 83.7% | 0.6% | PASS |
| `branch_list_many_branches` | 20.07ms | 25.00ms | 80.3% | 0.8% | PASS |
| `branch_create_delete` | 22.13ms | 27.00ms | 82.0% | 0.9% | PASS |
| `at_literal_deep_history` | 21.56ms | 28.00ms | 77.0% | 1.0% | PASS |
| `diff_literal_deep_history` | 21.56ms | 28.00ms | 77.0% | 1.0% | PASS |
| `history_literal_deep_history` | 22.53ms | 30.00ms | 75.1% | 0.8% | PASS |
| `checkout_branch_clean` | 31.39ms | 43.00ms | 73.0% | 0.9% | PASS |
| `merge_data_no_conflicts` | 24.29ms | 33.00ms | 73.6% | 0.6% | PASS |
| `merge_data_secondary_index` | 825.85ms | 944.00ms | 87.5% | 0.4% | PASS |
| `merge_schema_no_conflicts` | 20.50ms | 24.00ms | 85.4% | 1.3% | PASS |
| `merge_data_conflicts` | 27.29ms | 33.00ms | 82.7% | 0.7% | PASS |
| `merge_data_conflicts_with_resolve` | 28.22ms | 34.00ms | 83.0% | 0.6% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
