# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-12 11:27 UTC
>
> Commit: [`374d7ae15469d165eab2b3c057d93375c3cf90ef`](https://github.com/dolthub/doltlite/commit/374d7ae15469d165eab2b3c057d93375c3cf90ef)
>
> Runner: ubuntu24 20261004.327.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/38232927144)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.17s | 12.13s | 1.2× | 1.3% | **PASS** |
| Writes | 2.08s | 3.34s | 1.6× | 1.1% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.95s | 12.41s | 1.1× | 1.4% | **PASS** |
| Writes | 3.52s | 4.17s | 1.2× | 1.7% | **PASS** |
| Autocommit writes | 857.27ms | 2.66s | 3.1× | 5.2% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.39s | 2.83s | 1.2× | 1.3% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.50s | 3.01s | 1.2× | 1.3% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.51s | 3.08s | 1.2× | 1.5% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.77s | 3.20s | 1.2× | 1.1% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 431.76ms | 721.80ms | 1.7× | 1.1% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 489.29ms | 766.91ms | 1.6× | 0.9% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 572.10ms | 931.83ms | 1.6× | 1.1% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 588.21ms | 923.57ms | 1.6× | 1.1% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.64s | 2.91s | 1.1× | 1.3% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.63s | 3.05s | 1.2× | 1.3% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.80s | 3.19s | 1.1× | 1.7% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.87s | 3.26s | 1.1× | 1.4% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 587.06ms | 799.82ms | 1.4× | 1.7% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 1.35s | 1.32s | 1.0× | 1.8% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 805.55ms | 1.04s | 1.3× | 1.8% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 769.77ms | 1.01s | 1.3× | 1.7% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.48s | 2.91s | 1.2× | 1.1% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.52s | 3.03s | 1.2× | 1.2% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.63s | 3.19s | 1.2× | 1.6% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.71s | 3.26s | 1.2× | 1.4% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 208.32ms | 666.95ms | 3.2× | 5.7% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 243.61ms | 702.69ms | 2.9× | 4.4% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 195.68ms | 627.84ms | 3.2× | 4.8% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 209.66ms | 662.31ms | 3.2× | 6.1% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 24.16ms | 32.40ms | 1.3× | 1.3% | PASS |
| mem_reads | `oltp_range_select` | 10.17ms | 12.62ms | 1.2× | 2.2% | PASS |
| mem_reads | `oltp_sum_range` | 8.90ms | 12.03ms | 1.4× | 1.3% | PASS |
| mem_reads | `oltp_order_range` | 2.54ms | 3.05ms | 1.2× | 1.4% | PASS |
| mem_reads | `oltp_distinct_range` | 3.62ms | 4.29ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_index_scan` | 3.81ms | 5.60ms | 1.5× | 1.4% | PASS |
| mem_reads | `select_random_points` | 9.62ms | 12.07ms | 1.3× | 1.6% | PASS |
| mem_reads | `select_random_ranges` | 4.49ms | 5.40ms | 1.2× | 1.5% | PASS |
| mem_reads | `covering_index_scan` | 7.55ms | 10.67ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 29.15ms | 33.99ms | 1.2× | 0.6% | PASS |
| mem_reads | `index_join` | 5.72ms | 8.56ms | 1.5× | 1.2% | PASS |
| mem_reads | `index_join_scan` | 3.10ms | 5.69ms | 1.8× | 1.7% | PASS |
| mem_reads | `types_table_scan` | 1.03s | 1.23s | 1.2× | 0.4% | PASS |
| mem_reads | `table_scan` | 1.16s | 1.33s | 1.1× | 0.6% | PASS |
| mem_reads | `oltp_read_only` | 99.04ms | 127.45ms | 1.3× | 0.7% | PASS |
| mem_writes | `oltp_bulk_insert` | 177.83ms | 283.74ms | 1.6× | 1.3% | PASS |
| mem_writes | `oltp_insert` | 15.09ms | 27.64ms | 1.8× | 0.9% | PASS |
| mem_writes | `oltp_update_index` | 48.78ms | 90.31ms | 1.9× | 1.4% | PASS |
| mem_writes | `oltp_update_non_index` | 33.69ms | 57.00ms | 1.7× | 1.1% | PASS |
| mem_writes | `oltp_delete_insert` | 44.28ms | 73.51ms | 1.7× | 1.0% | PASS |
| mem_writes | `oltp_write_only` | 21.43ms | 44.08ms | 2.1× | 1.1% | PASS |
| mem_writes | `types_delete_insert` | 24.15ms | 37.71ms | 1.6× | 1.3% | PASS |
| mem_writes | `oltp_read_write` | 66.52ms | 107.82ms | 1.6× | 1.1% | PASS |
| file_reads | `oltp_point_select` | 92.24ms | 50.54ms | 0.5× | 0.7% | PASS |
| file_reads | `oltp_range_select` | 17.46ms | 14.64ms | 0.8× | 1.7% | PASS |
| file_reads | `oltp_sum_range` | 16.82ms | 14.23ms | 0.8× | 1.4% | PASS |
| file_reads | `oltp_order_range` | 3.37ms | 3.33ms | 1.0× | 2.5% | PASS |
| file_reads | `oltp_distinct_range` | 4.41ms | 4.53ms | 1.0× | 1.4% | PASS |
| file_reads | `oltp_index_scan` | 10.86ms | 7.79ms | 0.7× | 1.3% | PASS |
| file_reads | `select_random_points` | 16.30ms | 14.03ms | 0.9× | 2.6% | PASS |
| file_reads | `select_random_ranges` | 11.50ms | 7.43ms | 0.6× | 1.1% | PASS |
| file_reads | `covering_index_scan` | 14.83ms | 12.86ms | 0.9× | 1.0% | PASS |
| file_reads | `groupby_scan` | 29.93ms | 34.36ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 9.66ms | 10.25ms | 1.1× | 1.4% | PASS |
| file_reads | `index_join_scan` | 4.06ms | 6.16ms | 1.5× | 1.6% | PASS |
| file_reads | `types_table_scan` | 1.03s | 1.24s | 1.2× | 0.5% | PASS |
| file_reads | `table_scan` | 1.17s | 1.33s | 1.1× | 0.9% | PASS |
| file_reads | `oltp_read_only` | 211.64ms | 158.53ms | 0.7× | 1.2% | PASS |
| file_writes | `oltp_bulk_insert` | 193.11ms | 296.21ms | 1.5× | 1.2% | PASS |
| file_writes | `oltp_insert` | 21.72ms | 30.96ms | 1.4× | 1.6% | PASS |
| file_writes | `oltp_update_index` | 77.29ms | 106.40ms | 1.4× | 2.2% | PASS |
| file_writes | `oltp_update_non_index` | 56.55ms | 67.50ms | 1.2× | 2.8% | PASS |
| file_writes | `oltp_delete_insert` | 66.86ms | 85.91ms | 1.3× | 1.6% | PASS |
| file_writes | `oltp_write_only` | 43.08ms | 52.64ms | 1.2× | 1.8% | PASS |
| file_writes | `types_delete_insert` | 38.46ms | 43.57ms | 1.1× | 1.8% | PASS |
| file_writes | `oltp_read_write` | 89.98ms | 116.61ms | 1.3× | 1.5% | PASS |
| ac_reads | `oltp_point_select` | 46.17ms | 50.58ms | 1.1× | 0.9% | PASS |
| ac_reads | `oltp_range_select` | 12.82ms | 14.56ms | 1.1× | 1.2% | PASS |
| ac_reads | `oltp_sum_range` | 12.08ms | 14.18ms | 1.2× | 0.8% | PASS |
| ac_reads | `oltp_order_range` | 2.96ms | 3.33ms | 1.1× | 1.1% | PASS |
| ac_reads | `oltp_distinct_range` | 4.01ms | 4.55ms | 1.1× | 1.1% | PASS |
| ac_reads | `oltp_index_scan` | 6.53ms | 7.81ms | 1.2× | 1.5% | PASS |
| ac_reads | `select_random_points` | 12.63ms | 14.23ms | 1.1× | 1.6% | PASS |
| ac_reads | `select_random_ranges` | 7.10ms | 7.52ms | 1.1× | 1.0% | PASS |
| ac_reads | `covering_index_scan` | 10.31ms | 12.90ms | 1.3× | 1.1% | PASS |
| ac_reads | `groupby_scan` | 29.84ms | 34.53ms | 1.2× | 0.6% | PASS |
| ac_reads | `index_join` | 7.25ms | 10.29ms | 1.4× | 1.2% | PASS |
| ac_reads | `index_join_scan` | 3.59ms | 6.14ms | 1.7× | 1.5% | PASS |
| ac_reads | `types_table_scan` | 1.03s | 1.25s | 1.2× | 0.5% | PASS |
| ac_reads | `table_scan` | 1.16s | 1.33s | 1.1× | 0.6% | PASS |
| ac_reads | `oltp_read_only` | 134.20ms | 155.69ms | 1.2× | 0.9% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 24.04ms | 69.98ms | 2.9× | 5.6% | PASS |
| ac_writes | `oltp_insert_ac` | 24.84ms | 83.23ms | 3.4× | 5.0% | PASS |
| ac_writes | `oltp_update_index_ac` | 28.56ms | 96.69ms | 3.4× | 5.5% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.98ms | 77.70ms | 3.1× | 7.6% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 25.70ms | 87.56ms | 3.4× | 5.8% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.93ms | 84.06ms | 3.2× | 7.4% | PASS |
| ac_writes | `types_delete_insert_ac` | 23.89ms | 77.65ms | 3.2× | 7.3% | PASS |
| ac_writes | `oltp_read_write_ac` | 30.39ms | 90.08ms | 3.0× | 3.9% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 30.89ms | 32.46ms | 1.1× | 1.6% | PASS |
| mem_reads | `oltp_range_select` | 14.05ms | 14.89ms | 1.1× | 1.5% | PASS |
| mem_reads | `oltp_sum_range` | 13.89ms | 13.65ms | 1.0× | 1.4% | PASS |
| mem_reads | `oltp_order_range` | 2.98ms | 3.32ms | 1.1× | 1.5% | PASS |
| mem_reads | `oltp_distinct_range` | 3.89ms | 4.33ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 3.63ms | 5.60ms | 1.5× | 1.5% | PASS |
| mem_reads | `select_random_points` | 20.60ms | 21.08ms | 1.0× | 1.1% | PASS |
| mem_reads | `select_random_ranges` | 5.70ms | 5.75ms | 1.0× | 1.3% | PASS |
| mem_reads | `covering_index_scan` | 6.50ms | 9.04ms | 1.4× | 1.8% | PASS |
| mem_reads | `groupby_scan` | 32.57ms | 34.96ms | 1.1× | 1.0% | PASS |
| mem_reads | `index_join` | 10.33ms | 9.21ms | 0.9× | 1.8% | PASS |
| mem_reads | `index_join_scan` | 3.25ms | 6.32ms | 1.9× | 1.0% | PASS |
| mem_reads | `types_table_scan` | 1.05s | 1.32s | 1.2× | 1.1% | PASS |
| mem_reads | `table_scan` | 1.18s | 1.40s | 1.2× | 0.5% | PASS |
| mem_reads | `oltp_read_only` | 120.17ms | 133.42ms | 1.1× | 1.1% | PASS |
| mem_writes | `oltp_bulk_insert` | 190.78ms | 267.12ms | 1.4× | 0.5% | PASS |
| mem_writes | `oltp_insert` | 15.29ms | 30.51ms | 2.0× | 1.3% | PASS |
| mem_writes | `oltp_update_index` | 57.41ms | 116.01ms | 2.0× | 1.2% | PASS |
| mem_writes | `oltp_update_non_index` | 40.99ms | 65.55ms | 1.6× | 0.9% | PASS |
| mem_writes | `oltp_delete_insert` | 46.82ms | 83.55ms | 1.8× | 1.0% | PASS |
| mem_writes | `oltp_write_only` | 23.84ms | 45.38ms | 1.9× | 0.7% | PASS |
| mem_writes | `types_delete_insert` | 33.62ms | 44.86ms | 1.3× | 1.1% | PASS |
| mem_writes | `oltp_read_write` | 80.53ms | 113.94ms | 1.4× | 0.8% | PASS |
| file_reads | `oltp_point_select` | 62.03ms | 40.97ms | 0.7× | 1.1% | PASS |
| file_reads | `oltp_range_select` | 17.59ms | 16.21ms | 0.9× | 0.9% | PASS |
| file_reads | `oltp_sum_range` | 17.68ms | 15.01ms | 0.8× | 1.3% | PASS |
| file_reads | `oltp_order_range` | 3.42ms | 3.56ms | 1.0× | 1.3% | PASS |
| file_reads | `oltp_distinct_range` | 4.40ms | 4.57ms | 1.0× | 1.5% | PASS |
| file_reads | `oltp_index_scan` | 6.91ms | 6.64ms | 1.0× | 1.7% | PASS |
| file_reads | `select_random_points` | 24.32ms | 21.95ms | 0.9× | 1.2% | PASS |
| file_reads | `select_random_ranges` | 9.10ms | 6.72ms | 0.7× | 1.5% | PASS |
| file_reads | `covering_index_scan` | 9.91ms | 10.22ms | 1.0× | 1.1% | PASS |
| file_reads | `groupby_scan` | 33.36ms | 35.27ms | 1.1× | 0.6% | PASS |
| file_reads | `index_join` | 12.44ms | 10.38ms | 0.8× | 1.9% | PASS |
| file_reads | `index_join_scan` | 3.67ms | 6.62ms | 1.8× | 1.7% | PASS |
| file_reads | `types_table_scan` | 1.04s | 1.30s | 1.3× | 0.9% | PASS |
| file_reads | `table_scan` | 1.22s | 1.43s | 1.2× | 1.9% | PASS |
| file_reads | `oltp_read_only` | 165.43ms | 145.36ms | 0.9× | 0.8% | PASS |
| file_writes | `oltp_bulk_insert` | 283.06ms | 354.14ms | 1.3× | 1.6% | PASS |
| file_writes | `oltp_insert` | 32.42ms | 59.30ms | 1.8× | 1.7% | PASS |
| file_writes | `oltp_update_index` | 187.21ms | 211.40ms | 1.1× | 1.5% | PASS |
| file_writes | `oltp_update_non_index` | 162.76ms | 138.40ms | 0.9× | 3.4% | PASS |
| file_writes | `oltp_delete_insert` | 198.69ms | 163.53ms | 0.8× | 1.6% | PASS |
| file_writes | `oltp_write_only` | 133.88ms | 111.34ms | 0.8× | 1.9% | PASS |
| file_writes | `types_delete_insert` | 158.08ms | 98.05ms | 0.6× | 4.5% | PASS |
| file_writes | `oltp_read_write` | 197.77ms | 182.35ms | 0.9× | 2.7% | PASS |
| ac_reads | `oltp_point_select` | 41.74ms | 41.62ms | 1.0× | 1.2% | PASS |
| ac_reads | `oltp_range_select` | 15.65ms | 16.31ms | 1.0× | 0.8% | PASS |
| ac_reads | `oltp_sum_range` | 15.23ms | 14.89ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_order_range` | 3.19ms | 3.54ms | 1.1× | 1.6% | PASS |
| ac_reads | `oltp_distinct_range` | 4.06ms | 4.52ms | 1.1× | 1.6% | PASS |
| ac_reads | `oltp_index_scan` | 4.78ms | 6.56ms | 1.4× | 1.2% | PASS |
| ac_reads | `select_random_points` | 22.29ms | 21.95ms | 1.0× | 0.9% | PASS |
| ac_reads | `select_random_ranges` | 6.98ms | 6.71ms | 1.0× | 2.0% | PASS |
| ac_reads | `covering_index_scan` | 7.69ms | 10.17ms | 1.3× | 1.5% | PASS |
| ac_reads | `groupby_scan` | 33.05ms | 35.30ms | 1.1× | 0.9% | PASS |
| ac_reads | `index_join` | 11.66ms | 10.51ms | 0.9× | 1.4% | PASS |
| ac_reads | `index_join_scan` | 3.54ms | 6.66ms | 1.9× | 1.8% | PASS |
| ac_reads | `types_table_scan` | 1.04s | 1.31s | 1.3× | 0.6% | PASS |
| ac_reads | `table_scan` | 1.18s | 1.40s | 1.2× | 0.3% | PASS |
| ac_reads | `oltp_read_only` | 131.35ms | 143.52ms | 1.1× | 0.9% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 29.04ms | 73.66ms | 2.5× | 3.9% | PASS |
| ac_writes | `oltp_insert_ac` | 31.03ms | 85.02ms | 2.7× | 4.1% | PASS |
| ac_writes | `oltp_update_index_ac` | 31.48ms | 98.90ms | 3.1× | 4.9% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 28.21ms | 81.89ms | 2.9× | 4.0% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 30.54ms | 93.89ms | 3.1× | 4.6% | PASS |
| ac_writes | `oltp_write_only_ac` | 29.95ms | 89.05ms | 3.0× | 5.0% | PASS |
| ac_writes | `types_delete_insert_ac` | 29.17ms | 84.77ms | 2.9× | 6.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 34.20ms | 95.53ms | 2.8× | 4.2% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.84ms | 40.69ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_range_select` | 15.57ms | 15.68ms | 1.0× | 2.1% | PASS |
| mem_reads | `oltp_sum_range` | 14.09ms | 15.29ms | 1.1× | 2.0% | PASS |
| mem_reads | `oltp_order_range` | 3.12ms | 3.47ms | 1.1× | 1.7% | PASS |
| mem_reads | `oltp_distinct_range` | 4.22ms | 4.64ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 3.86ms | 6.86ms | 1.8× | 1.5% | PASS |
| mem_reads | `select_random_points` | 20.48ms | 22.95ms | 1.1× | 1.6% | PASS |
| mem_reads | `select_random_ranges` | 6.30ms | 7.02ms | 1.1× | 2.4% | PASS |
| mem_reads | `covering_index_scan` | 7.63ms | 11.12ms | 1.5× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 33.23ms | 36.60ms | 1.1× | 0.6% | PASS |
| mem_reads | `index_join` | 9.84ms | 11.00ms | 1.1× | 1.6% | PASS |
| mem_reads | `index_join_scan` | 3.65ms | 6.63ms | 1.8× | 1.8% | PASS |
| mem_reads | `types_table_scan` | 1.04s | 1.33s | 1.3× | 0.5% | PASS |
| mem_reads | `table_scan` | 1.18s | 1.42s | 1.2× | 0.8% | PASS |
| mem_reads | `oltp_read_only` | 130.20ms | 148.08ms | 1.1× | 0.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 239.18ms | 342.31ms | 1.4× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 18.17ms | 38.14ms | 2.1× | 0.8% | PASS |
| mem_writes | `oltp_update_index` | 62.30ms | 127.78ms | 2.1× | 0.8% | PASS |
| mem_writes | `oltp_update_non_index` | 45.64ms | 78.08ms | 1.7× | 1.0% | PASS |
| mem_writes | `oltp_delete_insert` | 50.79ms | 97.14ms | 1.9× | 1.1% | PASS |
| mem_writes | `oltp_write_only` | 26.84ms | 55.87ms | 2.1× | 1.4% | PASS |
| mem_writes | `types_delete_insert` | 36.56ms | 54.89ms | 1.5× | 1.2% | PASS |
| mem_writes | `oltp_read_write` | 92.62ms | 137.62ms | 1.5× | 1.7% | PASS |
| file_reads | `oltp_point_select` | 103.61ms | 59.79ms | 0.6× | 1.1% | PASS |
| file_reads | `oltp_range_select` | 22.19ms | 17.70ms | 0.8× | 1.4% | PASS |
| file_reads | `oltp_sum_range` | 21.82ms | 17.52ms | 0.8× | 2.0% | PASS |
| file_reads | `oltp_order_range` | 4.28ms | 3.83ms | 0.9× | 2.6% | PASS |
| file_reads | `oltp_distinct_range` | 5.31ms | 5.00ms | 0.9× | 2.3% | PASS |
| file_reads | `oltp_index_scan` | 11.11ms | 8.95ms | 0.8× | 1.1% | PASS |
| file_reads | `select_random_points` | 29.31ms | 25.95ms | 0.9× | 1.8% | PASS |
| file_reads | `select_random_ranges` | 13.66ms | 9.15ms | 0.7× | 1.7% | PASS |
| file_reads | `covering_index_scan` | 15.04ms | 13.16ms | 0.9× | 1.0% | PASS |
| file_reads | `groupby_scan` | 34.36ms | 36.92ms | 1.1× | 0.9% | PASS |
| file_reads | `index_join` | 13.90ms | 12.24ms | 0.9× | 2.1% | PASS |
| file_reads | `index_join_scan` | 4.65ms | 6.99ms | 1.5× | 2.4% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.35s | 1.3× | 1.0% | PASS |
| file_reads | `table_scan` | 1.23s | 1.44s | 1.2× | 2.6% | PASS |
| file_reads | `oltp_read_only` | 242.91ms | 179.43ms | 0.7× | 1.1% | PASS |
| file_writes | `oltp_bulk_insert` | 260.75ms | 355.65ms | 1.4× | 0.9% | PASS |
| file_writes | `oltp_insert` | 25.08ms | 43.29ms | 1.7× | 1.4% | PASS |
| file_writes | `oltp_update_index` | 97.86ms | 149.22ms | 1.5× | 1.8% | PASS |
| file_writes | `oltp_update_non_index` | 90.06ms | 92.19ms | 1.0× | 9.8% | PASS |
| file_writes | `oltp_delete_insert` | 85.80ms | 113.24ms | 1.3× | 1.5% | PASS |
| file_writes | `oltp_write_only` | 57.01ms | 69.04ms | 1.2× | 2.0% | PASS |
| file_writes | `types_delete_insert` | 62.90ms | 66.44ms | 1.1× | 2.1% | PASS |
| file_writes | `oltp_read_write` | 126.11ms | 151.44ms | 1.2× | 1.7% | PASS |
| ac_reads | `oltp_point_select` | 58.47ms | 60.35ms | 1.0× | 1.6% | PASS |
| ac_reads | `oltp_range_select` | 18.62ms | 17.82ms | 1.0× | 1.9% | PASS |
| ac_reads | `oltp_sum_range` | 17.70ms | 17.53ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_order_range` | 3.88ms | 3.78ms | 1.0× | 2.3% | PASS |
| ac_reads | `oltp_distinct_range` | 4.87ms | 4.95ms | 1.0× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 6.58ms | 8.92ms | 1.4× | 1.7% | PASS |
| ac_reads | `select_random_points` | 24.74ms | 25.96ms | 1.0× | 2.3% | PASS |
| ac_reads | `select_random_ranges` | 9.18ms | 9.15ms | 1.0× | 1.7% | PASS |
| ac_reads | `covering_index_scan` | 10.42ms | 13.21ms | 1.3× | 1.1% | PASS |
| ac_reads | `groupby_scan` | 33.81ms | 36.90ms | 1.1× | 0.9% | PASS |
| ac_reads | `index_join` | 12.21ms | 12.29ms | 1.0× | 2.2% | PASS |
| ac_reads | `index_join_scan` | 4.30ms | 7.16ms | 1.7× | 2.2% | PASS |
| ac_reads | `types_table_scan` | 1.05s | 1.36s | 1.3× | 1.0% | PASS |
| ac_reads | `table_scan` | 1.20s | 1.43s | 1.2× | 1.6% | PASS |
| ac_reads | `oltp_read_only` | 170.86ms | 178.61ms | 1.0× | 1.4% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 22.65ms | 63.02ms | 2.8× | 5.5% | PASS |
| ac_writes | `oltp_insert_ac` | 23.82ms | 79.15ms | 3.3× | 5.0% | PASS |
| ac_writes | `oltp_update_index_ac` | 25.54ms | 91.45ms | 3.6× | 3.2% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 22.48ms | 72.52ms | 3.2× | 4.5% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 24.54ms | 80.47ms | 3.3× | 4.6% | PASS |
| ac_writes | `oltp_write_only_ac` | 24.00ms | 80.38ms | 3.3× | 3.2% | PASS |
| ac_writes | `types_delete_insert_ac` | 22.38ms | 73.05ms | 3.3× | 7.2% | PASS |
| ac_writes | `oltp_read_write_ac` | 30.27ms | 87.80ms | 2.9× | 5.7% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 33.92ms | 42.42ms | 1.3× | 1.0% | PASS |
| mem_reads | `oltp_range_select` | 19.82ms | 23.17ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_sum_range` | 17.91ms | 22.29ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_order_range` | 3.57ms | 4.39ms | 1.2× | 0.8% | PASS |
| mem_reads | `oltp_distinct_range` | 4.73ms | 5.61ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_index_scan` | 4.53ms | 6.51ms | 1.4× | 1.6% | PASS |
| mem_reads | `select_random_points` | 28.48ms | 33.95ms | 1.2× | 1.4% | PASS |
| mem_reads | `select_random_ranges` | 7.66ms | 9.47ms | 1.2× | 1.1% | PASS |
| mem_reads | `covering_index_scan` | 7.59ms | 10.94ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 36.03ms | 42.48ms | 1.2× | 0.9% | PASS |
| mem_reads | `index_join` | 7.83ms | 11.66ms | 1.5× | 1.5% | PASS |
| mem_reads | `index_join_scan` | 3.96ms | 6.45ms | 1.6× | 1.5% | PASS |
| mem_reads | `types_table_scan` | 1.08s | 1.34s | 1.2× | 1.6% | PASS |
| mem_reads | `table_scan` | 1.36s | 1.46s | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_read_only` | 158.52ms | 192.62ms | 1.2× | 1.1% | PASS |
| mem_writes | `oltp_bulk_insert` | 243.04ms | 333.70ms | 1.4× | 1.0% | PASS |
| mem_writes | `oltp_insert` | 18.71ms | 34.03ms | 1.8× | 1.0% | PASS |
| mem_writes | `oltp_update_index` | 66.28ms | 119.69ms | 1.8× | 1.2% | PASS |
| mem_writes | `oltp_update_non_index` | 51.09ms | 81.67ms | 1.6× | 1.2% | PASS |
| mem_writes | `oltp_delete_insert` | 48.80ms | 91.84ms | 1.9× | 1.2% | PASS |
| mem_writes | `oltp_write_only` | 26.48ms | 53.61ms | 2.0× | 1.1% | PASS |
| mem_writes | `types_delete_insert` | 32.84ms | 53.39ms | 1.6× | 1.0% | PASS |
| mem_writes | `oltp_read_write` | 100.98ms | 155.64ms | 1.5× | 1.6% | PASS |
| file_reads | `oltp_point_select` | 104.06ms | 61.40ms | 0.6× | 1.1% | PASS |
| file_reads | `oltp_range_select` | 27.38ms | 25.67ms | 0.9× | 1.4% | PASS |
| file_reads | `oltp_sum_range` | 26.35ms | 24.98ms | 0.9× | 1.7% | PASS |
| file_reads | `oltp_order_range` | 4.43ms | 4.68ms | 1.1× | 1.2% | PASS |
| file_reads | `oltp_distinct_range` | 5.63ms | 5.91ms | 1.1× | 1.2% | PASS |
| file_reads | `oltp_index_scan` | 11.79ms | 8.62ms | 0.7× | 1.7% | PASS |
| file_reads | `select_random_points` | 37.53ms | 37.55ms | 1.0× | 1.8% | PASS |
| file_reads | `select_random_ranges` | 15.34ms | 11.76ms | 0.8× | 1.3% | PASS |
| file_reads | `covering_index_scan` | 15.05ms | 13.10ms | 0.9× | 1.6% | PASS |
| file_reads | `groupby_scan` | 37.16ms | 43.13ms | 1.2× | 0.8% | PASS |
| file_reads | `index_join` | 12.08ms | 13.24ms | 1.1× | 1.8% | PASS |
| file_reads | `index_join_scan` | 4.91ms | 6.81ms | 1.4× | 2.3% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.35s | 1.3× | 0.8% | PASS |
| file_reads | `table_scan` | 1.26s | 1.43s | 1.1× | 3.9% | PASS |
| file_reads | `oltp_read_only` | 258.60ms | 219.59ms | 0.8× | 1.0% | PASS |
| file_writes | `oltp_bulk_insert` | 260.27ms | 344.10ms | 1.3× | 1.0% | PASS |
| file_writes | `oltp_insert` | 26.53ms | 38.32ms | 1.4× | 3.1% | PASS |
| file_writes | `oltp_update_index` | 98.73ms | 139.01ms | 1.4× | 1.8% | PASS |
| file_writes | `oltp_update_non_index` | 78.17ms | 94.26ms | 1.2× | 2.2% | PASS |
| file_writes | `oltp_delete_insert` | 76.18ms | 104.97ms | 1.4× | 1.7% | PASS |
| file_writes | `oltp_write_only` | 52.67ms | 65.39ms | 1.2× | 2.0% | PASS |
| file_writes | `types_delete_insert` | 50.43ms | 62.34ms | 1.2× | 1.5% | PASS |
| file_writes | `oltp_read_write` | 126.79ms | 166.52ms | 1.3× | 1.1% | PASS |
| ac_reads | `oltp_point_select` | 56.45ms | 61.10ms | 1.1× | 1.1% | PASS |
| ac_reads | `oltp_range_select` | 21.96ms | 25.64ms | 1.2× | 1.2% | PASS |
| ac_reads | `oltp_sum_range` | 20.53ms | 24.76ms | 1.2× | 1.1% | PASS |
| ac_reads | `oltp_order_range` | 3.90ms | 4.67ms | 1.2× | 1.6% | PASS |
| ac_reads | `oltp_distinct_range` | 5.11ms | 5.91ms | 1.2× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 7.58ms | 8.82ms | 1.2× | 1.9% | PASS |
| ac_reads | `select_random_points` | 32.64ms | 37.79ms | 1.2× | 1.4% | PASS |
| ac_reads | `select_random_ranges` | 10.15ms | 11.75ms | 1.2× | 1.2% | PASS |
| ac_reads | `covering_index_scan` | 10.54ms | 13.16ms | 1.2× | 2.1% | PASS |
| ac_reads | `groupby_scan` | 36.53ms | 43.22ms | 1.2× | 0.7% | PASS |
| ac_reads | `index_join` | 10.09ms | 13.71ms | 1.4× | 2.4% | PASS |
| ac_reads | `index_join_scan` | 4.63ms | 7.11ms | 1.5× | 2.2% | PASS |
| ac_reads | `types_table_scan` | 1.06s | 1.35s | 1.3× | 1.2% | PASS |
| ac_reads | `table_scan` | 1.24s | 1.43s | 1.2× | 2.2% | PASS |
| ac_reads | `oltp_read_only` | 191.91ms | 220.68ms | 1.1× | 1.3% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 24.64ms | 67.07ms | 2.7× | 7.2% | PASS |
| ac_writes | `oltp_insert_ac` | 26.53ms | 83.33ms | 3.1× | 6.5% | PASS |
| ac_writes | `oltp_update_index_ac` | 28.25ms | 95.63ms | 3.4× | 5.4% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 24.81ms | 77.70ms | 3.1× | 6.6% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 24.66ms | 83.84ms | 3.4× | 6.6% | PASS |
| ac_writes | `oltp_write_only_ac` | 25.34ms | 84.45ms | 3.3× | 5.7% | PASS |
| ac_writes | `types_delete_insert_ac` | 22.47ms | 76.43ms | 3.4× | 4.6% | PASS |
| ac_writes | `oltp_read_write_ac` | 32.97ms | 93.86ms | 2.8× | 5.1% | PASS |

</details>

</details>

## Version-control latency

Wall time: 4m 19s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 34.61ms | 38.00ms | 91.1% | 0.8% | PASS |
| `status_dirty_many_tables` | 38.44ms | 42.00ms | 91.5% | 1.7% | PASS |
| `diff_regular_working_one_table` | 30.02ms | 33.00ms | 91.0% | 1.2% | PASS |
| `diff_regular_working_many_tables` | 42.89ms | 50.00ms | 85.8% | 0.7% | PASS |
| `diff_stat_working_many_tables` | 42.78ms | 48.00ms | 89.1% | 0.7% | PASS |
| `diff_schema_working_many_tables` | 43.30ms | 48.00ms | 90.2% | 0.7% | PASS |
| `branch_list_many_branches` | 22.01ms | 25.00ms | 88.0% | 1.1% | PASS |
| `branch_create_delete` | 23.47ms | 27.00ms | 86.9% | 0.8% | PASS |
| `at_literal_deep_history` | 23.60ms | 28.00ms | 84.3% | 0.5% | PASS |
| `diff_literal_deep_history` | 23.70ms | 28.00ms | 84.6% | 0.6% | PASS |
| `history_literal_deep_history` | 24.85ms | 30.00ms | 82.8% | 0.6% | PASS |
| `checkout_branch_clean` | 32.59ms | 43.00ms | 75.8% | 1.0% | PASS |
| `merge_data_no_conflicts` | 27.68ms | 33.00ms | 83.9% | 2.1% | PASS |
| `merge_data_secondary_index` | 818.37ms | 944.00ms | 86.7% | 0.5% | PASS |
| `merge_schema_no_conflicts` | 20.91ms | 24.00ms | 87.1% | 0.8% | PASS |
| `merge_data_conflicts` | 29.79ms | 33.00ms | 90.3% | 0.6% | PASS |
| `merge_data_conflicts_with_resolve` | 30.54ms | 34.00ms | 89.8% | 0.4% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
