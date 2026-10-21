# DoltLite Performance Report

> Nightly result: **PASS**
>
> Generated: 2026-10-21 11:16 UTC
>
> Commit: [`0bd33659f20a8f97fbc4fb97e8a8c275710fa0ea`](https://github.com/dolthub/doltlite/commit/0bd33659f20a8f97fbc4fb97e8a8c275710fa0ea)
>
> Runner: ubuntu24 20261012.334.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/39578485820)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 10.24s | 12.13s | 1.2× | 1.3% | **PASS** |
| Writes | 2.13s | 3.45s | 1.6× | 1.2% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 11.11s | 12.35s | 1.1× | 1.3% | **PASS** |
| Writes | 3.41s | 4.05s | 1.2× | 1.9% | **PASS** |
| Autocommit writes | 763.91ms | 2.46s | 3.2× | 7.0% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.40s | 2.94s | 1.2× | 1.3% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 3.02s | 3.38s | 1.1× | 1.7% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 2.73s | 3.30s | 1.2× | 1.8% | **PASS** |
| In-memory | Reads | compositepk | 15 | 55 | 2.09s | 2.51s | 1.2× | 0.7% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 430.17ms | 768.68ms | 1.8× | 1.1% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 644.04ms | 1.05s | 1.6× | 2.2% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 602.05ms | 988.96ms | 1.6× | 1.5% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 451.68ms | 650.06ms | 1.4× | 0.9% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.61s | 2.99s | 1.1× | 1.5% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 3.24s | 3.43s | 1.1× | 1.8% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 2.96s | 3.36s | 1.1× | 1.3% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.30s | 2.56s | 1.1× | 1.0% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 584.27ms | 829.36ms | 1.4× | 1.3% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 941.89ms | 1.11s | 1.2× | 4.5% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 855.93ms | 1.09s | 1.3× | 1.5% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 1.03s | 1.02s | 1.0× | 7.1% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.47s | 2.99s | 1.2× | 1.4% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.73s | 3.32s | 1.2× | 2.0% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 2.82s | 3.37s | 1.2× | 1.7% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.17s | 2.56s | 1.2× | 0.6% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 178.40ms | 589.56ms | 3.3× | 6.5% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 211.66ms | 678.86ms | 3.2× | 8.0% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 214.23ms | 665.93ms | 3.1× | 6.4% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 159.62ms | 530.53ms | 3.3× | 11.7% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 23.17ms | 31.51ms | 1.4× | 1.7% | PASS |
| mem_reads | `oltp_range_select` | 10.08ms | 13.16ms | 1.3× | 2.6% | PASS |
| mem_reads | `oltp_sum_range` | 9.12ms | 12.42ms | 1.4× | 2.0% | PASS |
| mem_reads | `oltp_order_range` | 2.50ms | 3.01ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_distinct_range` | 3.55ms | 4.18ms | 1.2× | 1.1% | PASS |
| mem_reads | `oltp_index_scan` | 3.70ms | 5.48ms | 1.5× | 1.6% | PASS |
| mem_reads | `select_random_points` | 9.32ms | 11.81ms | 1.3× | 2.6% | PASS |
| mem_reads | `select_random_ranges` | 4.52ms | 5.36ms | 1.2× | 1.3% | PASS |
| mem_reads | `covering_index_scan` | 7.55ms | 10.75ms | 1.4× | 1.0% | PASS |
| mem_reads | `groupby_scan` | 29.23ms | 34.15ms | 1.2× | 0.8% | PASS |
| mem_reads | `index_join` | 5.68ms | 8.63ms | 1.5× | 1.2% | PASS |
| mem_reads | `index_join_scan` | 3.08ms | 5.67ms | 1.8× | 2.0% | PASS |
| mem_reads | `types_table_scan` | 1.02s | 1.28s | 1.3× | 0.6% | PASS |
| mem_reads | `table_scan` | 1.16s | 1.38s | 1.2× | 1.3% | PASS |
| mem_reads | `oltp_read_only` | 101.40ms | 127.74ms | 1.3× | 1.5% | PASS |
| mem_writes | `oltp_bulk_insert` | 178.90ms | 300.88ms | 1.7× | 1.2% | PASS |
| mem_writes | `oltp_insert` | 15.02ms | 27.57ms | 1.8× | 1.1% | PASS |
| mem_writes | `oltp_update_index` | 48.84ms | 98.17ms | 2.0× | 1.4% | PASS |
| mem_writes | `oltp_update_non_index` | 33.37ms | 63.38ms | 1.9× | 0.8% | PASS |
| mem_writes | `oltp_delete_insert` | 44.03ms | 78.97ms | 1.8× | 1.3% | PASS |
| mem_writes | `oltp_write_only` | 21.58ms | 49.61ms | 2.3× | 1.0% | PASS |
| mem_writes | `types_delete_insert` | 23.83ms | 37.07ms | 1.6× | 1.1% | PASS |
| mem_writes | `oltp_read_write` | 64.59ms | 113.03ms | 1.7× | 1.0% | PASS |
| file_reads | `oltp_point_select` | 87.49ms | 48.05ms | 0.5× | 0.9% | PASS |
| file_reads | `oltp_range_select` | 16.78ms | 14.86ms | 0.9× | 1.9% | PASS |
| file_reads | `oltp_sum_range` | 16.16ms | 14.27ms | 0.9× | 2.1% | PASS |
| file_reads | `oltp_order_range` | 3.36ms | 3.29ms | 1.0× | 2.1% | PASS |
| file_reads | `oltp_distinct_range` | 4.39ms | 4.44ms | 1.0× | 1.6% | PASS |
| file_reads | `oltp_index_scan` | 10.50ms | 7.46ms | 0.7× | 1.8% | PASS |
| file_reads | `select_random_points` | 16.59ms | 13.69ms | 0.8× | 1.9% | PASS |
| file_reads | `select_random_ranges` | 11.38ms | 7.18ms | 0.6× | 1.3% | PASS |
| file_reads | `covering_index_scan` | 14.59ms | 12.74ms | 0.9× | 1.5% | PASS |
| file_reads | `groupby_scan` | 30.23ms | 34.48ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 9.49ms | 10.09ms | 1.1× | 1.5% | PASS |
| file_reads | `index_join_scan` | 4.04ms | 5.99ms | 1.5× | 1.5% | PASS |
| file_reads | `types_table_scan` | 1.03s | 1.28s | 1.2× | 0.5% | PASS |
| file_reads | `table_scan` | 1.16s | 1.38s | 1.2× | 0.7% | PASS |
| file_reads | `oltp_read_only` | 196.33ms | 153.19ms | 0.8× | 1.0% | PASS |
| file_writes | `oltp_bulk_insert` | 192.64ms | 312.06ms | 1.6× | 1.3% | PASS |
| file_writes | `oltp_insert` | 21.65ms | 30.81ms | 1.4× | 1.3% | PASS |
| file_writes | `oltp_update_index` | 74.93ms | 107.31ms | 1.4× | 1.2% | PASS |
| file_writes | `oltp_update_non_index` | 56.54ms | 70.33ms | 1.2× | 1.8% | PASS |
| file_writes | `oltp_delete_insert` | 66.95ms | 90.36ms | 1.3× | 1.4% | PASS |
| file_writes | `oltp_write_only` | 43.84ms | 55.40ms | 1.3× | 1.6% | PASS |
| file_writes | `types_delete_insert` | 38.90ms | 43.72ms | 1.1× | 1.3% | PASS |
| file_writes | `oltp_read_write` | 88.83ms | 119.38ms | 1.3× | 0.9% | PASS |
| ac_reads | `oltp_point_select` | 45.26ms | 48.48ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 12.73ms | 14.92ms | 1.2× | 2.1% | PASS |
| ac_reads | `oltp_sum_range` | 11.74ms | 14.21ms | 1.2× | 1.4% | PASS |
| ac_reads | `oltp_order_range` | 2.92ms | 3.29ms | 1.1× | 1.8% | PASS |
| ac_reads | `oltp_distinct_range` | 3.91ms | 4.44ms | 1.1× | 1.4% | PASS |
| ac_reads | `oltp_index_scan` | 6.12ms | 7.47ms | 1.2× | 1.4% | PASS |
| ac_reads | `select_random_points` | 12.01ms | 13.66ms | 1.1× | 1.3% | PASS |
| ac_reads | `select_random_ranges` | 6.81ms | 7.15ms | 1.1× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.08ms | 12.70ms | 1.3× | 1.7% | PASS |
| ac_reads | `groupby_scan` | 29.68ms | 34.50ms | 1.2× | 0.5% | PASS |
| ac_reads | `index_join` | 7.10ms | 10.06ms | 1.4× | 1.2% | PASS |
| ac_reads | `index_join_scan` | 3.51ms | 6.02ms | 1.7× | 1.5% | PASS |
| ac_reads | `types_table_scan` | 1.03s | 1.28s | 1.2× | 0.6% | PASS |
| ac_reads | `table_scan` | 1.16s | 1.38s | 1.2× | 0.5% | PASS |
| ac_reads | `oltp_read_only` | 132.85ms | 153.07ms | 1.2× | 1.1% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 20.48ms | 60.72ms | 3.0× | 4.6% | PASS |
| ac_writes | `oltp_insert_ac` | 21.71ms | 74.55ms | 3.4× | 6.5% | PASS |
| ac_writes | `oltp_update_index_ac` | 24.30ms | 84.51ms | 3.5× | 6.8% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.16ms | 67.32ms | 3.2× | 7.0% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 22.77ms | 76.18ms | 3.3× | 5.2% | PASS |
| ac_writes | `oltp_write_only_ac` | 22.27ms | 75.19ms | 3.4× | 7.0% | PASS |
| ac_writes | `types_delete_insert_ac` | 19.20ms | 68.52ms | 3.6× | 6.5% | PASS |
| ac_writes | `oltp_read_write_ac` | 26.51ms | 82.57ms | 3.1× | 5.1% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 38.82ms | 42.67ms | 1.1× | 2.7% | PASS |
| mem_reads | `oltp_range_select` | 18.18ms | 16.70ms | 0.9× | 2.3% | PASS |
| mem_reads | `oltp_sum_range` | 16.62ms | 16.38ms | 1.0× | 2.2% | PASS |
| mem_reads | `oltp_order_range` | 3.46ms | 3.83ms | 1.1× | 1.4% | PASS |
| mem_reads | `oltp_distinct_range` | 4.48ms | 5.03ms | 1.1× | 1.5% | PASS |
| mem_reads | `oltp_index_scan` | 4.22ms | 7.11ms | 1.7× | 1.9% | PASS |
| mem_reads | `select_random_points` | 26.27ms | 25.85ms | 1.0× | 4.4% | PASS |
| mem_reads | `select_random_ranges` | 7.16ms | 7.28ms | 1.0× | 1.7% | PASS |
| mem_reads | `covering_index_scan` | 7.92ms | 11.54ms | 1.5× | 1.3% | PASS |
| mem_reads | `groupby_scan` | 35.19ms | 38.73ms | 1.1× | 0.8% | PASS |
| mem_reads | `index_join` | 11.37ms | 11.66ms | 1.0× | 3.6% | PASS |
| mem_reads | `index_join_scan` | 3.93ms | 7.24ms | 1.8× | 3.3% | PASS |
| mem_reads | `types_table_scan` | 1.26s | 1.47s | 1.2× | 0.8% | PASS |
| mem_reads | `table_scan` | 1.43s | 1.55s | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_read_only` | 154.77ms | 163.86ms | 1.1× | 1.4% | PASS |
| mem_writes | `oltp_bulk_insert` | 239.42ms | 362.61ms | 1.5× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 18.84ms | 39.98ms | 2.1× | 2.0% | PASS |
| mem_writes | `oltp_update_index` | 78.72ms | 160.84ms | 2.0× | 2.7% | PASS |
| mem_writes | `oltp_update_non_index` | 55.28ms | 88.59ms | 1.6× | 3.0% | PASS |
| mem_writes | `oltp_delete_insert` | 62.64ms | 116.81ms | 1.9× | 1.5% | PASS |
| mem_writes | `oltp_write_only` | 32.76ms | 61.73ms | 1.9× | 2.1% | PASS |
| mem_writes | `types_delete_insert` | 43.11ms | 64.44ms | 1.5× | 2.3% | PASS |
| mem_writes | `oltp_read_write` | 113.28ms | 152.16ms | 1.3× | 2.6% | PASS |
| file_reads | `oltp_point_select` | 104.59ms | 59.64ms | 0.6× | 1.5% | PASS |
| file_reads | `oltp_range_select` | 25.20ms | 18.50ms | 0.7× | 2.4% | PASS |
| file_reads | `oltp_sum_range` | 24.29ms | 18.41ms | 0.8× | 2.0% | PASS |
| file_reads | `oltp_order_range` | 4.49ms | 4.00ms | 0.9× | 1.8% | PASS |
| file_reads | `oltp_distinct_range` | 5.51ms | 5.17ms | 0.9× | 2.1% | PASS |
| file_reads | `oltp_index_scan` | 11.18ms | 8.85ms | 0.8× | 1.6% | PASS |
| file_reads | `select_random_points` | 33.23ms | 27.12ms | 0.8× | 2.1% | PASS |
| file_reads | `select_random_ranges` | 14.36ms | 9.23ms | 0.6× | 1.4% | PASS |
| file_reads | `covering_index_scan` | 15.03ms | 13.16ms | 0.9× | 1.9% | PASS |
| file_reads | `groupby_scan` | 35.56ms | 38.43ms | 1.1× | 1.0% | PASS |
| file_reads | `index_join` | 15.86ms | 12.55ms | 0.8× | 3.9% | PASS |
| file_reads | `index_join_scan` | 5.39ms | 8.05ms | 1.5× | 5.0% | PASS |
| file_reads | `types_table_scan` | 1.28s | 1.49s | 1.2× | 0.7% | PASS |
| file_reads | `table_scan` | 1.41s | 1.54s | 1.1× | 1.4% | PASS |
| file_reads | `oltp_read_only` | 254.22ms | 185.91ms | 0.7× | 1.2% | PASS |
| file_writes | `oltp_bulk_insert` | 264.62ms | 378.33ms | 1.4× | 1.4% | PASS |
| file_writes | `oltp_insert` | 26.20ms | 43.58ms | 1.7× | 2.8% | PASS |
| file_writes | `oltp_update_index` | 139.11ms | 164.26ms | 1.2× | 6.2% | PASS |
| file_writes | `oltp_update_non_index` | 103.56ms | 98.59ms | 1.0× | 12.7% | PASS |
| file_writes | `oltp_delete_insert` | 100.54ms | 126.76ms | 1.3× | 2.6% | PASS |
| file_writes | `oltp_write_only` | 88.97ms | 70.18ms | 0.8× | 11.1% | PASS |
| file_writes | `types_delete_insert` | 72.18ms | 70.07ms | 1.0× | 2.1% | PASS |
| file_writes | `oltp_read_write` | 146.69ms | 155.66ms | 1.1× | 8.1% | PASS |
| ac_reads | `oltp_point_select` | 57.31ms | 59.02ms | 1.0× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 19.36ms | 18.25ms | 0.9× | 1.5% | PASS |
| ac_reads | `oltp_sum_range` | 18.35ms | 18.04ms | 1.0× | 2.0% | PASS |
| ac_reads | `oltp_order_range` | 3.76ms | 3.95ms | 1.1× | 2.4% | PASS |
| ac_reads | `oltp_distinct_range` | 4.79ms | 5.09ms | 1.1× | 2.1% | PASS |
| ac_reads | `oltp_index_scan` | 6.53ms | 8.81ms | 1.3× | 1.6% | PASS |
| ac_reads | `select_random_points` | 26.27ms | 26.55ms | 1.0× | 2.8% | PASS |
| ac_reads | `select_random_ranges` | 9.08ms | 9.11ms | 1.0× | 1.9% | PASS |
| ac_reads | `covering_index_scan` | 10.17ms | 13.13ms | 1.3× | 1.3% | PASS |
| ac_reads | `groupby_scan` | 34.70ms | 38.34ms | 1.1× | 0.7% | PASS |
| ac_reads | `index_join` | 12.60ms | 12.45ms | 1.0× | 3.6% | PASS |
| ac_reads | `index_join_scan` | 4.21ms | 7.25ms | 1.7× | 2.9% | PASS |
| ac_reads | `types_table_scan` | 1.10s | 1.42s | 1.3× | 2.4% | PASS |
| ac_reads | `table_scan` | 1.24s | 1.50s | 1.2× | 2.2% | PASS |
| ac_reads | `oltp_read_only` | 181.95ms | 184.60ms | 1.0× | 1.9% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 23.54ms | 66.40ms | 2.8× | 6.9% | PASS |
| ac_writes | `oltp_insert_ac` | 26.66ms | 80.35ms | 3.0× | 8.9% | PASS |
| ac_writes | `oltp_update_index_ac` | 28.61ms | 100.24ms | 3.5× | 8.9% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 22.75ms | 76.18ms | 3.3× | 7.2% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 26.06ms | 90.24ms | 3.5× | 5.6% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.56ms | 89.27ms | 3.4× | 11.9% | PASS |
| ac_writes | `types_delete_insert_ac` | 24.15ms | 81.50ms | 3.4× | 9.1% | PASS |
| ac_writes | `oltp_read_write_ac` | 33.33ms | 94.68ms | 2.8× | 6.7% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 36.36ms | 41.50ms | 1.1× | 2.7% | PASS |
| mem_reads | `oltp_range_select` | 16.36ms | 16.05ms | 1.0× | 2.6% | PASS |
| mem_reads | `oltp_sum_range` | 14.88ms | 15.75ms | 1.1× | 1.8% | PASS |
| mem_reads | `oltp_order_range` | 3.24ms | 3.61ms | 1.1× | 1.4% | PASS |
| mem_reads | `oltp_distinct_range` | 4.30ms | 4.75ms | 1.1× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 4.00ms | 6.88ms | 1.7× | 2.1% | PASS |
| mem_reads | `select_random_points` | 22.42ms | 23.31ms | 1.0× | 3.6% | PASS |
| mem_reads | `select_random_ranges` | 6.68ms | 7.01ms | 1.0× | 2.2% | PASS |
| mem_reads | `covering_index_scan` | 7.65ms | 11.28ms | 1.5× | 0.6% | PASS |
| mem_reads | `groupby_scan` | 33.84ms | 37.85ms | 1.1× | 0.6% | PASS |
| mem_reads | `index_join` | 10.49ms | 11.20ms | 1.1× | 2.9% | PASS |
| mem_reads | `index_join_scan` | 3.82ms | 6.70ms | 1.8× | 3.1% | PASS |
| mem_reads | `types_table_scan` | 1.13s | 1.44s | 1.3× | 1.1% | PASS |
| mem_reads | `table_scan` | 1.29s | 1.52s | 1.2× | 0.6% | PASS |
| mem_reads | `oltp_read_only` | 141.32ms | 154.61ms | 1.1× | 1.3% | PASS |
| mem_writes | `oltp_bulk_insert` | 241.12ms | 359.80ms | 1.5× | 1.1% | PASS |
| mem_writes | `oltp_insert` | 18.52ms | 38.27ms | 2.1× | 1.1% | PASS |
| mem_writes | `oltp_update_index` | 67.34ms | 141.99ms | 2.1× | 1.5% | PASS |
| mem_writes | `oltp_update_non_index` | 50.56ms | 81.17ms | 1.6× | 2.5% | PASS |
| mem_writes | `oltp_delete_insert` | 55.00ms | 106.24ms | 1.9× | 1.4% | PASS |
| mem_writes | `oltp_write_only` | 28.75ms | 56.79ms | 2.0× | 1.5% | PASS |
| mem_writes | `types_delete_insert` | 39.15ms | 59.89ms | 1.5× | 2.7% | PASS |
| mem_writes | `oltp_read_write` | 101.61ms | 144.82ms | 1.4× | 1.9% | PASS |
| file_reads | `oltp_point_select` | 103.07ms | 58.67ms | 0.6× | 1.3% | PASS |
| file_reads | `oltp_range_select` | 23.95ms | 17.89ms | 0.7× | 2.5% | PASS |
| file_reads | `oltp_sum_range` | 22.66ms | 17.73ms | 0.8× | 2.2% | PASS |
| file_reads | `oltp_order_range` | 4.26ms | 3.85ms | 0.9× | 1.3% | PASS |
| file_reads | `oltp_distinct_range` | 5.30ms | 5.00ms | 0.9× | 0.9% | PASS |
| file_reads | `oltp_index_scan` | 10.95ms | 8.64ms | 0.8× | 1.5% | PASS |
| file_reads | `select_random_points` | 32.27ms | 26.14ms | 0.8× | 3.2% | PASS |
| file_reads | `select_random_ranges` | 13.77ms | 8.87ms | 0.6× | 1.3% | PASS |
| file_reads | `covering_index_scan` | 14.87ms | 12.97ms | 0.9× | 0.9% | PASS |
| file_reads | `groupby_scan` | 34.92ms | 38.20ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 15.12ms | 12.55ms | 0.8× | 3.3% | PASS |
| file_reads | `index_join_scan` | 4.78ms | 7.08ms | 1.5× | 2.4% | PASS |
| file_reads | `types_table_scan` | 1.13s | 1.44s | 1.3× | 1.1% | PASS |
| file_reads | `table_scan` | 1.30s | 1.53s | 1.2× | 0.8% | PASS |
| file_reads | `oltp_read_only` | 249.48ms | 183.55ms | 0.7× | 1.2% | PASS |
| file_writes | `oltp_bulk_insert` | 264.15ms | 374.81ms | 1.4× | 1.3% | PASS |
| file_writes | `oltp_insert` | 28.33ms | 43.74ms | 1.5× | 2.0% | PASS |
| file_writes | `oltp_update_index` | 105.93ms | 160.09ms | 1.5× | 1.6% | PASS |
| file_writes | `oltp_update_non_index` | 90.25ms | 94.70ms | 1.0× | 3.2% | PASS |
| file_writes | `oltp_delete_insert` | 93.04ms | 121.99ms | 1.3× | 1.0% | PASS |
| file_writes | `oltp_write_only` | 64.91ms | 69.47ms | 1.1× | 1.5% | PASS |
| file_writes | `types_delete_insert` | 67.73ms | 66.88ms | 1.0× | 1.8% | PASS |
| file_writes | `oltp_read_write` | 141.59ms | 157.58ms | 1.1× | 1.4% | PASS |
| ac_reads | `oltp_point_select` | 58.92ms | 59.31ms | 1.0× | 1.8% | PASS |
| ac_reads | `oltp_range_select` | 19.52ms | 17.98ms | 0.9× | 1.5% | PASS |
| ac_reads | `oltp_sum_range` | 17.98ms | 17.79ms | 1.0× | 1.9% | PASS |
| ac_reads | `oltp_order_range` | 3.79ms | 3.85ms | 1.0× | 1.7% | PASS |
| ac_reads | `oltp_distinct_range` | 4.90ms | 5.03ms | 1.0× | 1.1% | PASS |
| ac_reads | `oltp_index_scan` | 6.59ms | 8.74ms | 1.3× | 1.8% | PASS |
| ac_reads | `select_random_points` | 25.38ms | 26.14ms | 1.0× | 2.3% | PASS |
| ac_reads | `select_random_ranges` | 9.22ms | 8.87ms | 1.0× | 1.8% | PASS |
| ac_reads | `covering_index_scan` | 10.40ms | 13.03ms | 1.3× | 1.2% | PASS |
| ac_reads | `groupby_scan` | 34.61ms | 38.25ms | 1.1× | 0.7% | PASS |
| ac_reads | `index_join` | 12.87ms | 12.61ms | 1.0× | 4.1% | PASS |
| ac_reads | `index_join_scan` | 4.28ms | 7.12ms | 1.7× | 1.8% | PASS |
| ac_reads | `types_table_scan` | 1.13s | 1.44s | 1.3× | 1.4% | PASS |
| ac_reads | `table_scan` | 1.30s | 1.53s | 1.2× | 1.1% | PASS |
| ac_reads | `oltp_read_only` | 180.47ms | 183.12ms | 1.0× | 1.2% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 24.05ms | 65.68ms | 2.7× | 7.5% | PASS |
| ac_writes | `oltp_insert_ac` | 27.13ms | 85.60ms | 3.2× | 6.8% | PASS |
| ac_writes | `oltp_update_index_ac` | 27.91ms | 96.76ms | 3.5× | 6.3% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 23.27ms | 74.01ms | 3.2× | 6.0% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 27.22ms | 87.43ms | 3.2× | 6.4% | PASS |
| ac_writes | `oltp_write_only_ac` | 26.62ms | 84.91ms | 3.2× | 7.0% | PASS |
| ac_writes | `types_delete_insert_ac` | 24.52ms | 79.11ms | 3.2× | 6.2% | PASS |
| ac_writes | `oltp_read_write_ac` | 33.52ms | 92.42ms | 2.8× | 5.3% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 25.39ms | 30.08ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_range_select` | 14.92ms | 17.92ms | 1.2× | 0.9% | PASS |
| mem_reads | `oltp_sum_range` | 13.45ms | 17.16ms | 1.3× | 0.6% | PASS |
| mem_reads | `oltp_order_range` | 2.88ms | 3.25ms | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_distinct_range` | 3.70ms | 4.15ms | 1.1× | 0.6% | PASS |
| mem_reads | `oltp_index_scan` | 3.55ms | 4.72ms | 1.3× | 1.2% | PASS |
| mem_reads | `select_random_points` | 20.94ms | 24.33ms | 1.2× | 1.0% | PASS |
| mem_reads | `select_random_ranges` | 5.72ms | 6.72ms | 1.2× | 0.6% | PASS |
| mem_reads | `covering_index_scan` | 5.84ms | 7.85ms | 1.3× | 0.7% | PASS |
| mem_reads | `groupby_scan` | 29.68ms | 34.49ms | 1.2× | 0.5% | PASS |
| mem_reads | `index_join` | 6.09ms | 8.51ms | 1.4× | 0.9% | PASS |
| mem_reads | `index_join_scan` | 3.12ms | 5.27ms | 1.7× | 1.5% | PASS |
| mem_reads | `types_table_scan` | 859.36ms | 1.06s | 1.2× | 0.6% | PASS |
| mem_reads | `table_scan` | 983.18ms | 1.15s | 1.2× | 0.5% | PASS |
| mem_reads | `oltp_read_only` | 113.09ms | 136.64ms | 1.2× | 0.4% | PASS |
| mem_writes | `oltp_bulk_insert` | 188.29ms | 239.17ms | 1.3× | 0.9% | PASS |
| mem_writes | `oltp_insert` | 14.66ms | 23.70ms | 1.6× | 0.7% | PASS |
| mem_writes | `oltp_update_index` | 51.45ms | 84.01ms | 1.6× | 1.0% | PASS |
| mem_writes | `oltp_update_non_index` | 39.27ms | 57.47ms | 1.5× | 0.6% | PASS |
| mem_writes | `oltp_delete_insert` | 37.92ms | 63.30ms | 1.7× | 0.9% | PASS |
| mem_writes | `oltp_write_only` | 21.06ms | 37.06ms | 1.8× | 0.7% | PASS |
| mem_writes | `types_delete_insert` | 24.94ms | 36.67ms | 1.5× | 0.9% | PASS |
| mem_writes | `oltp_read_write` | 74.09ms | 108.68ms | 1.5× | 1.1% | PASS |
| file_reads | `oltp_point_select` | 91.25ms | 47.39ms | 0.5× | 0.7% | PASS |
| file_reads | `oltp_range_select` | 22.04ms | 19.91ms | 0.9× | 1.2% | PASS |
| file_reads | `oltp_sum_range` | 20.27ms | 19.18ms | 0.9× | 1.0% | PASS |
| file_reads | `oltp_order_range` | 3.65ms | 3.53ms | 1.0× | 1.9% | PASS |
| file_reads | `oltp_distinct_range` | 4.43ms | 4.43ms | 1.0× | 1.6% | PASS |
| file_reads | `oltp_index_scan` | 10.43ms | 6.84ms | 0.7× | 1.0% | PASS |
| file_reads | `select_random_points` | 28.23ms | 26.37ms | 0.9× | 1.1% | PASS |
| file_reads | `select_random_ranges` | 12.48ms | 8.73ms | 0.7× | 1.1% | PASS |
| file_reads | `covering_index_scan` | 12.80ms | 9.94ms | 0.8× | 1.0% | PASS |
| file_reads | `groupby_scan` | 30.43ms | 34.89ms | 1.1× | 0.5% | PASS |
| file_reads | `index_join` | 9.77ms | 10.18ms | 1.0× | 0.9% | PASS |
| file_reads | `index_join_scan` | 3.85ms | 5.54ms | 1.4× | 1.1% | PASS |
| file_reads | `types_table_scan` | 860.06ms | 1.05s | 1.2× | 0.6% | PASS |
| file_reads | `table_scan` | 984.22ms | 1.15s | 1.2× | 0.4% | PASS |
| file_reads | `oltp_read_only` | 207.61ms | 161.33ms | 0.8× | 0.6% | PASS |
| file_writes | `oltp_bulk_insert` | 244.39ms | 297.74ms | 1.2× | 7.8% | PASS |
| file_writes | `oltp_insert` | 32.30ms | 39.13ms | 1.2× | 11.4% | PASS |
| file_writes | `oltp_update_index` | 159.93ms | 151.62ms | 0.9× | 7.2% | PASS |
| file_writes | `oltp_update_non_index` | 134.93ms | 107.93ms | 0.8× | 7.1% | PASS |
| file_writes | `oltp_delete_insert` | 130.76ms | 118.04ms | 0.9× | 2.8% | PASS |
| file_writes | `oltp_write_only` | 96.49ms | 81.93ms | 0.8× | 4.4% | PASS |
| file_writes | `types_delete_insert` | 86.38ms | 76.38ms | 0.9× | 8.4% | PASS |
| file_writes | `oltp_read_write` | 147.00ms | 152.03ms | 1.0× | 1.7% | PASS |
| ac_reads | `oltp_point_select` | 47.13ms | 47.38ms | 1.0× | 0.9% | PASS |
| ac_reads | `oltp_range_select` | 17.71ms | 19.89ms | 1.1× | 0.6% | PASS |
| ac_reads | `oltp_sum_range` | 16.04ms | 19.17ms | 1.2× | 0.6% | PASS |
| ac_reads | `oltp_order_range` | 3.29ms | 3.52ms | 1.1× | 0.9% | PASS |
| ac_reads | `oltp_distinct_range` | 4.08ms | 4.42ms | 1.1× | 0.7% | PASS |
| ac_reads | `oltp_index_scan` | 6.18ms | 6.84ms | 1.1× | 0.7% | PASS |
| ac_reads | `select_random_points` | 23.91ms | 26.38ms | 1.1× | 0.6% | PASS |
| ac_reads | `select_random_ranges` | 8.21ms | 8.72ms | 1.1× | 0.7% | PASS |
| ac_reads | `covering_index_scan` | 8.54ms | 9.98ms | 1.2× | 0.6% | PASS |
| ac_reads | `groupby_scan` | 30.08ms | 34.88ms | 1.2× | 0.5% | PASS |
| ac_reads | `index_join` | 7.73ms | 10.21ms | 1.3× | 0.6% | PASS |
| ac_reads | `index_join_scan` | 3.53ms | 5.54ms | 1.6× | 1.1% | PASS |
| ac_reads | `types_table_scan` | 861.20ms | 1.05s | 1.2× | 0.5% | PASS |
| ac_reads | `table_scan` | 987.57ms | 1.15s | 1.2× | 0.5% | PASS |
| ac_reads | `oltp_read_only` | 144.94ms | 161.40ms | 1.1× | 0.5% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 17.32ms | 49.95ms | 2.9× | 7.7% | PASS |
| ac_writes | `oltp_insert_ac` | 19.50ms | 63.83ms | 3.3× | 12.6% | PASS |
| ac_writes | `oltp_update_index_ac` | 21.06ms | 77.48ms | 3.7× | 12.0% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 18.37ms | 60.09ms | 3.3× | 11.0% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 20.85ms | 70.11ms | 3.4× | 13.8% | PASS |
| ac_writes | `oltp_write_only_ac` | 19.81ms | 70.99ms | 3.6× | 24.6% | PASS |
| ac_writes | `types_delete_insert_ac` | 16.98ms | 63.31ms | 3.7× | 11.5% | PASS |
| ac_writes | `oltp_read_write_ac` | 25.73ms | 74.77ms | 2.9× | 10.5% | PASS |

</details>

</details>

## Version-control latency

Wall time: 4m 21s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 35.75ms | 38.00ms | 94.1% | 1.1% | PASS |
| `status_dirty_many_tables` | 39.27ms | 42.00ms | 93.5% | 0.7% | PASS |
| `diff_regular_working_one_table` | 30.38ms | 33.00ms | 92.1% | 0.8% | PASS |
| `diff_regular_working_many_tables` | 44.16ms | 50.00ms | 88.3% | 0.8% | PASS |
| `diff_stat_working_many_tables` | 43.24ms | 48.00ms | 90.1% | 0.6% | PASS |
| `diff_schema_working_many_tables` | 43.62ms | 48.00ms | 90.9% | 0.6% | PASS |
| `branch_list_many_branches` | 22.27ms | 25.00ms | 89.1% | 1.5% | PASS |
| `branch_create_delete` | 23.70ms | 27.00ms | 87.8% | 1.1% | PASS |
| `at_literal_deep_history` | 23.92ms | 28.00ms | 85.4% | 1.2% | PASS |
| `diff_literal_deep_history` | 23.68ms | 28.00ms | 84.6% | 1.0% | PASS |
| `history_literal_deep_history` | 25.22ms | 30.00ms | 84.1% | 0.8% | PASS |
| `checkout_branch_clean` | 33.15ms | 43.00ms | 77.1% | 0.8% | PASS |
| `merge_data_no_conflicts` | 27.42ms | 33.00ms | 83.1% | 1.0% | PASS |
| `merge_data_secondary_index` | 822.54ms | 944.00ms | 87.1% | 0.7% | PASS |
| `merge_schema_no_conflicts` | 21.03ms | 24.00ms | 87.6% | 1.3% | PASS |
| `merge_data_conflicts` | 29.80ms | 33.00ms | 90.3% | 0.8% | PASS |
| `merge_data_conflicts_with_resolve` | 30.69ms | 34.00ms | 90.3% | 0.9% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
