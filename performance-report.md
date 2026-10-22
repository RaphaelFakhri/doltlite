# DoltLite Performance Report

> Nightly result: **FAIL**
>
> Generated: 2026-10-22 11:11 UTC
>
> Commit: [`4b07a2dd7fb560370926d987f2f2380cbcc907da`](https://github.com/dolthub/doltlite/commit/4b07a2dd7fb560370926d987f2f2380cbcc907da)
>
> Runner: ubuntu24 20261018.339.1
>
> [GitHub Actions run](https://github.com/dolthub/doltlite/actions/runs/39756988149)

This report compares optimized DoltLite against stock SQLite on the same GitHub-hosted runner. Baseline and candidate execution order alternates on each repetition. Reported timings are medians. Paired-ratio noise is the median absolute deviation of the paired DoltLite/SQLite ratios, expressed as a percentage.

## SQL workload summary

The primary view aggregates all key shapes and compares DoltLite with SQLite by storage mode and operation class.

### In-memory

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 9.09s | 10.98s | 1.2× | 1.3% | **FAIL** |
| Writes | 1.85s | 3.02s | 1.6× | 1.4% | **PASS** |

### File-backed

| Operation | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---:|---:|---:|---:|---|
| Reads | 9.92s | 11.24s | 1.1× | 1.4% | **PASS** |
| Writes | 3.70s | 4.07s | 1.1× | 1.9% | **PASS** |
| Autocommit writes | 901.36ms | 2.56s | 2.8× | 6.5% | **PASS** |

The absolute ceiling is 2.3× per ordinary workload and 1.9× for a section average. Durable autocommit writes use 5.5× and 5.0× ceilings respectively. A breach counts only when the median of the paired ratios exceeds the same ceiling.

<details>
<summary>Key-shape and individual-workload breakdown</summary>

The integer, text, blob, and composite primary-key runs verify that performance holds across key shapes.

| Storage | Operation | Key shape | Workloads | Samples/workload | SQLite median total | DoltLite median total | Ratio | Paired-ratio noise | Result |
|---|---|---|---:|---:|---:|---:|---:|---:|---|
| In-memory | Reads | int | 15 | 55 | 2.38s | 2.85s | 1.2× | 1.3% | **PASS** |
| In-memory | Reads | textpk | 15 | 55 | 2.42s | 2.93s | 1.2× | 0.9% | **PASS** |
| In-memory | Reads | blobpk | 15 | 55 | 1.72s | 1.95s | 1.1× | 3.2% | **FAIL** |
| In-memory | Reads | compositepk | 15 | 55 | 2.56s | 3.25s | 1.3× | 1.3% | **PASS** |
| In-memory | Writes | int | 8 | 55 | 435.44ms | 766.98ms | 1.8× | 1.8% | **PASS** |
| In-memory | Writes | textpk | 8 | 55 | 482.02ms | 775.05ms | 1.6× | 1.1% | **PASS** |
| In-memory | Writes | blobpk | 8 | 55 | 359.47ms | 570.23ms | 1.6× | 1.8% | **PASS** |
| In-memory | Writes | compositepk | 8 | 55 | 576.28ms | 910.60ms | 1.6× | 1.3% | **PASS** |
| File-backed | Reads | int | 15 | 55 | 2.67s | 2.93s | 1.1× | 1.6% | **PASS** |
| File-backed | Reads | textpk | 15 | 55 | 2.70s | 3.12s | 1.2× | 1.1% | **PASS** |
| File-backed | Reads | blobpk | 15 | 55 | 1.76s | 1.87s | 1.1× | 1.9% | **PASS** |
| File-backed | Reads | compositepk | 15 | 55 | 2.79s | 3.31s | 1.2× | 1.4% | **PASS** |
| File-backed | Writes | int | 8 | 55 | 603.27ms | 819.74ms | 1.4× | 1.6% | **PASS** |
| File-backed | Writes | textpk | 8 | 55 | 1.34s | 1.30s | 1.0× | 2.7% | **PASS** |
| File-backed | Writes | blobpk | 8 | 55 | 989.88ms | 965.93ms | 1.0× | 6.6% | **PASS** |
| File-backed | Writes | compositepk | 8 | 55 | 768.85ms | 992.18ms | 1.3× | 1.6% | **PASS** |
| File-backed | Autocommit reads | int | 15 | 55 | 2.53s | 2.93s | 1.2× | 1.7% | **PASS** |
| File-backed | Autocommit reads | textpk | 15 | 55 | 2.61s | 3.10s | 1.2× | 0.9% | **PASS** |
| File-backed | Autocommit reads | blobpk | 15 | 55 | 1.79s | 1.99s | 1.1× | 2.8% | **PASS** |
| File-backed | Autocommit reads | compositepk | 15 | 55 | 2.64s | 3.31s | 1.3× | 1.3% | **PASS** |
| File-backed | Autocommit writes | int | 8 | 55 | 188.90ms | 609.21ms | 3.2× | 7.0% | **PASS** |
| File-backed | Autocommit writes | textpk | 8 | 55 | 262.52ms | 748.64ms | 2.9× | 4.9% | **PASS** |
| File-backed | Autocommit writes | blobpk | 8 | 55 | 221.94ms | 508.85ms | 2.3× | 6.1% | **PASS** |
| File-backed | Autocommit writes | compositepk | 8 | 55 | 227.99ms | 693.32ms | 3.0× | 6.9% | **PASS** |

<details>
<summary>int workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 22.25ms | 31.26ms | 1.4× | 1.4% | PASS |
| mem_reads | `oltp_range_select` | 9.40ms | 12.88ms | 1.4× | 1.4% | PASS |
| mem_reads | `oltp_sum_range` | 8.79ms | 12.16ms | 1.4× | 1.1% | PASS |
| mem_reads | `oltp_order_range` | 2.46ms | 2.97ms | 1.2× | 1.5% | PASS |
| mem_reads | `oltp_distinct_range` | 3.50ms | 4.12ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_index_scan` | 3.62ms | 5.50ms | 1.5× | 2.0% | PASS |
| mem_reads | `select_random_points` | 8.94ms | 11.88ms | 1.3× | 2.7% | PASS |
| mem_reads | `select_random_ranges` | 4.28ms | 5.30ms | 1.2× | 1.5% | PASS |
| mem_reads | `covering_index_scan` | 7.51ms | 10.74ms | 1.4× | 0.8% | PASS |
| mem_reads | `groupby_scan` | 29.03ms | 33.89ms | 1.2× | 0.6% | PASS |
| mem_reads | `index_join` | 5.64ms | 8.58ms | 1.5× | 1.3% | PASS |
| mem_reads | `index_join_scan` | 3.01ms | 5.67ms | 1.9× | 1.6% | PASS |
| mem_reads | `types_table_scan` | 1.03s | 1.25s | 1.2× | 0.9% | PASS |
| mem_reads | `table_scan` | 1.15s | 1.34s | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_read_only` | 98.37ms | 125.60ms | 1.3× | 0.6% | PASS |
| mem_writes | `oltp_bulk_insert` | 173.68ms | 292.32ms | 1.7× | 1.5% | PASS |
| mem_writes | `oltp_insert` | 14.80ms | 26.25ms | 1.8× | 0.8% | PASS |
| mem_writes | `oltp_update_index` | 53.35ms | 104.24ms | 2.0× | 1.8% | PASS |
| mem_writes | `oltp_update_non_index` | 35.34ms | 64.73ms | 1.8× | 1.8% | PASS |
| mem_writes | `oltp_delete_insert` | 45.10ms | 79.04ms | 1.8× | 2.3% | PASS |
| mem_writes | `oltp_write_only` | 22.06ms | 49.33ms | 2.2× | 2.2% | PASS |
| mem_writes | `types_delete_insert` | 24.18ms | 37.17ms | 1.5× | 1.7% | PASS |
| mem_writes | `oltp_read_write` | 66.93ms | 113.89ms | 1.7× | 2.8% | PASS |
| file_reads | `oltp_point_select` | 88.13ms | 48.06ms | 0.5× | 1.6% | PASS |
| file_reads | `oltp_range_select` | 17.07ms | 14.67ms | 0.9× | 2.2% | PASS |
| file_reads | `oltp_sum_range` | 16.28ms | 14.07ms | 0.9× | 1.7% | PASS |
| file_reads | `oltp_order_range` | 3.39ms | 3.30ms | 1.0× | 2.7% | PASS |
| file_reads | `oltp_distinct_range` | 4.49ms | 4.44ms | 1.0× | 1.4% | PASS |
| file_reads | `oltp_index_scan` | 10.58ms | 7.52ms | 0.7× | 2.0% | PASS |
| file_reads | `select_random_points` | 16.77ms | 13.79ms | 0.8× | 2.7% | PASS |
| file_reads | `select_random_ranges` | 11.46ms | 7.19ms | 0.6× | 1.6% | PASS |
| file_reads | `covering_index_scan` | 14.57ms | 12.71ms | 0.9× | 1.3% | PASS |
| file_reads | `groupby_scan` | 30.32ms | 34.39ms | 1.1× | 0.7% | PASS |
| file_reads | `index_join` | 9.53ms | 10.27ms | 1.1× | 1.3% | PASS |
| file_reads | `index_join_scan` | 4.13ms | 6.17ms | 1.5× | 2.6% | PASS |
| file_reads | `types_table_scan` | 1.04s | 1.26s | 1.2× | 1.2% | PASS |
| file_reads | `table_scan` | 1.20s | 1.34s | 1.1× | 1.1% | PASS |
| file_reads | `oltp_read_only` | 198.32ms | 152.21ms | 0.8× | 1.3% | PASS |
| file_writes | `oltp_bulk_insert` | 187.43ms | 304.29ms | 1.6× | 0.9% | PASS |
| file_writes | `oltp_insert` | 23.87ms | 29.70ms | 1.2× | 1.5% | PASS |
| file_writes | `oltp_update_index` | 77.14ms | 108.28ms | 1.4× | 2.3% | PASS |
| file_writes | `oltp_update_non_index` | 60.05ms | 70.60ms | 1.2× | 1.6% | PASS |
| file_writes | `oltp_delete_insert` | 70.05ms | 89.57ms | 1.3× | 1.6% | PASS |
| file_writes | `oltp_write_only` | 48.79ms | 54.51ms | 1.1× | 1.9% | PASS |
| file_writes | `types_delete_insert` | 40.07ms | 43.45ms | 1.1× | 1.4% | PASS |
| file_writes | `oltp_read_write` | 95.86ms | 119.34ms | 1.2× | 2.5% | PASS |
| ac_reads | `oltp_point_select` | 44.64ms | 48.01ms | 1.1× | 1.9% | PASS |
| ac_reads | `oltp_range_select` | 12.76ms | 14.65ms | 1.1× | 1.7% | PASS |
| ac_reads | `oltp_sum_range` | 12.02ms | 14.06ms | 1.2× | 1.7% | PASS |
| ac_reads | `oltp_order_range` | 3.02ms | 3.29ms | 1.1× | 2.1% | PASS |
| ac_reads | `oltp_distinct_range` | 4.05ms | 4.43ms | 1.1× | 2.0% | PASS |
| ac_reads | `oltp_index_scan` | 6.39ms | 7.51ms | 1.2× | 1.7% | PASS |
| ac_reads | `select_random_points` | 12.64ms | 13.79ms | 1.1× | 2.0% | PASS |
| ac_reads | `select_random_ranges` | 6.99ms | 7.18ms | 1.0× | 1.5% | PASS |
| ac_reads | `covering_index_scan` | 10.23ms | 12.70ms | 1.2× | 1.4% | PASS |
| ac_reads | `groupby_scan` | 29.82ms | 34.41ms | 1.2× | 0.5% | PASS |
| ac_reads | `index_join` | 7.39ms | 10.31ms | 1.4× | 1.7% | PASS |
| ac_reads | `index_join_scan` | 3.65ms | 6.12ms | 1.7× | 1.5% | PASS |
| ac_reads | `types_table_scan` | 1.05s | 1.26s | 1.2× | 1.0% | PASS |
| ac_reads | `table_scan` | 1.20s | 1.34s | 1.1× | 1.4% | PASS |
| ac_reads | `oltp_read_only` | 135.33ms | 151.98ms | 1.1× | 1.3% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 20.50ms | 63.62ms | 3.1× | 6.8% | PASS |
| ac_writes | `oltp_insert_ac` | 23.98ms | 75.63ms | 3.2× | 6.6% | PASS |
| ac_writes | `oltp_update_index_ac` | 25.57ms | 87.65ms | 3.4× | 6.8% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 21.72ms | 68.67ms | 3.2× | 7.1% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 23.14ms | 79.40ms | 3.4× | 7.7% | PASS |
| ac_writes | `oltp_write_only_ac` | 23.39ms | 77.69ms | 3.3× | 8.1% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.06ms | 70.66ms | 3.4× | 7.7% | PASS |
| ac_writes | `oltp_read_write_ac` | 29.54ms | 85.89ms | 2.9× | 6.4% | PASS |

</details>

<details>
<summary>textpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 30.75ms | 32.38ms | 1.1× | 1.0% | PASS |
| mem_reads | `oltp_range_select` | 13.95ms | 14.95ms | 1.1× | 0.9% | PASS |
| mem_reads | `oltp_sum_range` | 13.79ms | 13.77ms | 1.0× | 0.9% | PASS |
| mem_reads | `oltp_order_range` | 2.93ms | 3.31ms | 1.1× | 1.4% | PASS |
| mem_reads | `oltp_distinct_range` | 3.83ms | 4.31ms | 1.1× | 1.1% | PASS |
| mem_reads | `oltp_index_scan` | 3.55ms | 5.55ms | 1.6× | 1.1% | PASS |
| mem_reads | `select_random_points` | 20.19ms | 20.76ms | 1.0× | 1.0% | PASS |
| mem_reads | `select_random_ranges` | 5.60ms | 5.66ms | 1.0× | 0.8% | PASS |
| mem_reads | `covering_index_scan` | 6.35ms | 9.20ms | 1.4× | 1.0% | PASS |
| mem_reads | `groupby_scan` | 32.05ms | 34.62ms | 1.1× | 0.6% | PASS |
| mem_reads | `index_join` | 10.28ms | 9.56ms | 0.9× | 0.9% | PASS |
| mem_reads | `index_join_scan` | 3.18ms | 6.34ms | 2.0× | 1.0% | PASS |
| mem_reads | `types_table_scan` | 1.00s | 1.27s | 1.3× | 0.3% | PASS |
| mem_reads | `table_scan` | 1.16s | 1.38s | 1.2× | 0.4% | PASS |
| mem_reads | `oltp_read_only` | 117.41ms | 131.52ms | 1.1× | 0.8% | PASS |
| mem_writes | `oltp_bulk_insert` | 187.37ms | 273.10ms | 1.5× | 0.5% | PASS |
| mem_writes | `oltp_insert` | 14.94ms | 29.52ms | 2.0× | 0.8% | PASS |
| mem_writes | `oltp_update_index` | 56.41ms | 118.63ms | 2.1× | 1.3% | PASS |
| mem_writes | `oltp_update_non_index` | 40.60ms | 64.34ms | 1.6× | 1.5% | PASS |
| mem_writes | `oltp_delete_insert` | 46.02ms | 86.00ms | 1.9× | 1.3% | PASS |
| mem_writes | `oltp_write_only` | 23.75ms | 43.97ms | 1.9× | 1.2% | PASS |
| mem_writes | `types_delete_insert` | 33.04ms | 47.17ms | 1.4× | 0.9% | PASS |
| mem_writes | `oltp_read_write` | 79.89ms | 112.32ms | 1.4× | 0.9% | PASS |
| file_reads | `oltp_point_select` | 58.51ms | 39.17ms | 0.7× | 0.7% | PASS |
| file_reads | `oltp_range_select` | 17.32ms | 16.20ms | 0.9× | 0.7% | PASS |
| file_reads | `oltp_sum_range` | 17.16ms | 15.08ms | 0.9× | 1.1% | PASS |
| file_reads | `oltp_order_range` | 3.40ms | 3.56ms | 1.0× | 1.4% | PASS |
| file_reads | `oltp_distinct_range` | 4.37ms | 4.59ms | 1.1× | 1.5% | PASS |
| file_reads | `oltp_index_scan` | 6.64ms | 6.55ms | 1.0× | 1.3% | PASS |
| file_reads | `select_random_points` | 25.30ms | 22.71ms | 0.9× | 0.8% | PASS |
| file_reads | `select_random_ranges` | 9.06ms | 6.67ms | 0.7× | 1.2% | PASS |
| file_reads | `covering_index_scan` | 9.72ms | 10.34ms | 1.1× | 0.5% | PASS |
| file_reads | `groupby_scan` | 33.83ms | 36.10ms | 1.1× | 0.5% | PASS |
| file_reads | `index_join` | 12.86ms | 10.77ms | 0.8× | 1.1% | PASS |
| file_reads | `index_join_scan` | 3.81ms | 6.98ms | 1.8× | 1.4% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.32s | 1.3× | 0.6% | PASS |
| file_reads | `table_scan` | 1.27s | 1.47s | 1.2× | 3.8% | PASS |
| file_reads | `oltp_read_only` | 172.11ms | 149.63ms | 0.9× | 1.0% | PASS |
| file_writes | `oltp_bulk_insert` | 280.22ms | 360.46ms | 1.3× | 3.7% | PASS |
| file_writes | `oltp_insert` | 30.43ms | 46.84ms | 1.5× | 3.1% | PASS |
| file_writes | `oltp_update_index` | 192.30ms | 220.93ms | 1.1× | 2.3% | PASS |
| file_writes | `oltp_update_non_index` | 171.62ms | 138.93ms | 0.8× | 4.5% | PASS |
| file_writes | `oltp_delete_insert` | 182.18ms | 170.13ms | 0.9× | 0.6% | PASS |
| file_writes | `oltp_write_only` | 141.17ms | 99.14ms | 0.7× | 0.5% | PASS |
| file_writes | `types_delete_insert` | 151.61ms | 89.68ms | 0.6× | 5.5% | PASS |
| file_writes | `oltp_read_write` | 192.00ms | 169.04ms | 0.9× | 1.9% | PASS |
| ac_reads | `oltp_point_select` | 41.52ms | 39.96ms | 1.0× | 0.9% | PASS |
| ac_reads | `oltp_range_select` | 15.82ms | 16.45ms | 1.0× | 0.6% | PASS |
| ac_reads | `oltp_sum_range` | 15.74ms | 15.30ms | 1.0× | 0.8% | PASS |
| ac_reads | `oltp_order_range` | 3.25ms | 3.59ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_distinct_range` | 4.19ms | 4.62ms | 1.1× | 0.8% | PASS |
| ac_reads | `oltp_index_scan` | 4.97ms | 6.63ms | 1.3× | 1.0% | PASS |
| ac_reads | `select_random_points` | 23.30ms | 22.56ms | 1.0× | 1.1% | PASS |
| ac_reads | `select_random_ranges` | 7.17ms | 6.67ms | 0.9× | 0.8% | PASS |
| ac_reads | `covering_index_scan` | 7.79ms | 10.38ms | 1.3× | 0.5% | PASS |
| ac_reads | `groupby_scan` | 33.64ms | 36.16ms | 1.1× | 0.5% | PASS |
| ac_reads | `index_join` | 12.04ms | 10.89ms | 0.9× | 1.3% | PASS |
| ac_reads | `index_join_scan` | 3.65ms | 6.91ms | 1.9× | 1.6% | PASS |
| ac_reads | `types_table_scan` | 1.06s | 1.33s | 1.3× | 0.9% | PASS |
| ac_reads | `table_scan` | 1.24s | 1.44s | 1.2× | 1.1% | PASS |
| ac_reads | `oltp_read_only` | 139.47ms | 147.79ms | 1.1× | 1.0% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 31.42ms | 78.43ms | 2.5× | 5.3% | PASS |
| ac_writes | `oltp_insert_ac` | 33.43ms | 91.82ms | 2.7× | 5.0% | PASS |
| ac_writes | `oltp_update_index_ac` | 33.39ms | 106.23ms | 3.2× | 4.7% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 29.87ms | 83.67ms | 2.8× | 4.6% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 32.72ms | 96.26ms | 2.9× | 3.6% | PASS |
| ac_writes | `oltp_write_only_ac` | 32.73ms | 96.39ms | 2.9× | 4.5% | PASS |
| ac_writes | `types_delete_insert_ac` | 31.72ms | 94.03ms | 3.0× | 6.7% | PASS |
| ac_writes | `oltp_read_write_ac` | 37.24ms | 101.81ms | 2.7× | 5.0% | PASS |

</details>

<details>
<summary>blobpk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 23.12ms | 23.70ms | 1.0× | 4.6% | PASS |
| mem_reads | `oltp_range_select` | 10.53ms | 9.84ms | 0.9× | 3.1% | PASS |
| mem_reads | `oltp_sum_range` | 9.72ms | 9.22ms | 0.9× | 4.2% | PASS |
| mem_reads | `oltp_order_range` | 2.11ms | 2.12ms | 1.0× | 2.5% | PASS |
| mem_reads | `oltp_distinct_range` | 2.70ms | 2.64ms | 1.0× | 1.3% | PASS |
| mem_reads | `oltp_index_scan` | 2.51ms | 4.46ms | 1.8× | 2.5% | PASS |
| mem_reads | `select_random_points` | 13.29ms | 13.96ms | 1.1× | 2.3% | PASS |
| mem_reads | `select_random_ranges` | 4.08ms | 4.04ms | 1.0× | 5.4% | PASS |
| mem_reads | `covering_index_scan` | 3.92ms | 6.11ms | 1.6× | 1.5% | PASS |
| mem_reads | `groupby_scan` | 19.16ms | 19.98ms | 1.0× | 3.3% | PASS |
| mem_reads | `index_join` | 7.36ms | 7.35ms | 1.0× | 3.9% | PASS |
| mem_reads | `index_join_scan` | 2.14ms | 5.19ms | 2.4× | 4.1% | FAIL |
| mem_reads | `types_table_scan` | 698.62ms | 821.54ms | 1.2× | 1.4% | PASS |
| mem_reads | `table_scan` | 842.33ms | 938.94ms | 1.1× | 3.2% | PASS |
| mem_reads | `oltp_read_only` | 79.04ms | 78.08ms | 1.0× | 4.5% | PASS |
| mem_writes | `oltp_bulk_insert` | 144.46ms | 204.03ms | 1.4× | 1.0% | PASS |
| mem_writes | `oltp_insert` | 11.25ms | 22.38ms | 2.0× | 1.3% | PASS |
| mem_writes | `oltp_update_index` | 41.55ms | 86.32ms | 2.1× | 1.7% | PASS |
| mem_writes | `oltp_update_non_index` | 33.09ms | 49.69ms | 1.5× | 1.7% | PASS |
| mem_writes | `oltp_delete_insert` | 34.26ms | 64.66ms | 1.9× | 3.1% | PASS |
| mem_writes | `oltp_write_only` | 18.00ms | 34.48ms | 1.9× | 1.9% | PASS |
| mem_writes | `types_delete_insert` | 24.00ms | 33.51ms | 1.4× | 2.7% | PASS |
| mem_writes | `oltp_read_write` | 52.88ms | 75.16ms | 1.4× | 2.7% | PASS |
| file_reads | `oltp_point_select` | 69.79ms | 35.70ms | 0.5× | 2.2% | PASS |
| file_reads | `oltp_range_select` | 14.66ms | 10.36ms | 0.7× | 2.5% | PASS |
| file_reads | `oltp_sum_range` | 14.26ms | 10.18ms | 0.7× | 2.0% | PASS |
| file_reads | `oltp_order_range` | 2.65ms | 2.23ms | 0.8× | 1.6% | PASS |
| file_reads | `oltp_distinct_range` | 3.19ms | 2.76ms | 0.9× | 1.1% | PASS |
| file_reads | `oltp_index_scan` | 7.68ms | 5.64ms | 0.7× | 1.9% | PASS |
| file_reads | `select_random_points` | 18.88ms | 14.55ms | 0.8× | 2.9% | PASS |
| file_reads | `select_random_ranges` | 9.29ms | 5.33ms | 0.6× | 1.5% | PASS |
| file_reads | `covering_index_scan` | 9.35ms | 7.42ms | 0.8× | 2.4% | PASS |
| file_reads | `groupby_scan` | 20.06ms | 19.97ms | 1.0× | 1.5% | PASS |
| file_reads | `index_join` | 10.62ms | 7.99ms | 0.8× | 2.9% | PASS |
| file_reads | `index_join_scan` | 3.25ms | 4.92ms | 1.5× | 2.5% | PASS |
| file_reads | `types_table_scan` | 676.51ms | 792.34ms | 1.2× | 1.5% | PASS |
| file_reads | `table_scan` | 757.75ms | 862.02ms | 1.1× | 1.7% | PASS |
| file_reads | `oltp_read_only` | 140.31ms | 92.62ms | 0.7× | 1.4% | PASS |
| file_writes | `oltp_bulk_insert` | 199.50ms | 249.89ms | 1.3× | 2.1% | PASS |
| file_writes | `oltp_insert` | 19.09ms | 41.08ms | 2.2× | 9.4% | PASS |
| file_writes | `oltp_update_index` | 141.63ms | 169.72ms | 1.2× | 3.8% | PASS |
| file_writes | `oltp_update_non_index` | 121.23ms | 100.16ms | 0.8× | 7.4% | PASS |
| file_writes | `oltp_delete_insert` | 143.60ms | 126.77ms | 0.9× | 2.2% | PASS |
| file_writes | `oltp_write_only` | 110.02ms | 86.10ms | 0.8× | 8.1% | PASS |
| file_writes | `types_delete_insert` | 104.73ms | 69.21ms | 0.7× | 7.7% | PASS |
| file_writes | `oltp_read_write` | 150.07ms | 123.02ms | 0.8× | 5.8% | PASS |
| ac_reads | `oltp_point_select` | 39.98ms | 36.97ms | 0.9× | 2.5% | PASS |
| ac_reads | `oltp_range_select` | 12.44ms | 10.64ms | 0.9× | 3.3% | PASS |
| ac_reads | `oltp_sum_range` | 11.85ms | 10.63ms | 0.9× | 5.5% | PASS |
| ac_reads | `oltp_order_range` | 2.35ms | 2.28ms | 1.0× | 4.6% | PASS |
| ac_reads | `oltp_distinct_range` | 2.79ms | 2.72ms | 1.0× | 2.3% | PASS |
| ac_reads | `oltp_index_scan` | 4.56ms | 5.72ms | 1.3× | 3.4% | PASS |
| ac_reads | `select_random_points` | 16.49ms | 14.97ms | 0.9× | 4.5% | PASS |
| ac_reads | `select_random_ranges` | 6.30ms | 5.32ms | 0.8× | 2.0% | PASS |
| ac_reads | `covering_index_scan` | 6.08ms | 7.38ms | 1.2× | 4.0% | PASS |
| ac_reads | `groupby_scan` | 19.53ms | 19.93ms | 1.0× | 1.7% | PASS |
| ac_reads | `index_join` | 8.44ms | 7.78ms | 0.9× | 3.0% | PASS |
| ac_reads | `index_join_scan` | 2.79ms | 4.89ms | 1.8× | 2.7% | PASS |
| ac_reads | `types_table_scan` | 705.31ms | 825.22ms | 1.2× | 1.6% | PASS |
| ac_reads | `table_scan` | 840.83ms | 937.37ms | 1.1× | 2.7% | PASS |
| ac_reads | `oltp_read_only` | 111.81ms | 101.97ms | 0.9× | 2.8% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 26.79ms | 52.56ms | 2.0× | 5.9% | PASS |
| ac_writes | `oltp_insert_ac` | 29.49ms | 64.29ms | 2.2× | 8.1% | PASS |
| ac_writes | `oltp_update_index_ac` | 29.92ms | 73.16ms | 2.4× | 4.5% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 25.12ms | 58.53ms | 2.3× | 6.3% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 29.04ms | 65.03ms | 2.2× | 3.4% | PASS |
| ac_writes | `oltp_write_only_ac` | 28.41ms | 64.70ms | 2.3× | 4.9% | PASS |
| ac_writes | `types_delete_insert_ac` | 21.98ms | 62.40ms | 2.8× | 7.8% | PASS |
| ac_writes | `oltp_read_write_ac` | 31.19ms | 68.17ms | 2.2× | 7.6% | PASS |

</details>

<details>
<summary>compositepk workload details</summary>

| Section | Workload | SQLite median | DoltLite median | Ratio | Paired-ratio noise | Result |
|---|---|---:|---:|---:|---:|---|
| mem_reads | `oltp_point_select` | 32.84ms | 42.43ms | 1.3× | 1.4% | PASS |
| mem_reads | `oltp_range_select` | 18.93ms | 23.83ms | 1.3× | 1.7% | PASS |
| mem_reads | `oltp_sum_range` | 17.39ms | 22.25ms | 1.3× | 1.0% | PASS |
| mem_reads | `oltp_order_range` | 3.51ms | 4.25ms | 1.2× | 1.0% | PASS |
| mem_reads | `oltp_distinct_range` | 4.57ms | 5.42ms | 1.2× | 0.9% | PASS |
| mem_reads | `oltp_index_scan` | 4.52ms | 6.64ms | 1.5× | 2.1% | PASS |
| mem_reads | `select_random_points` | 27.46ms | 32.53ms | 1.2× | 1.1% | PASS |
| mem_reads | `select_random_ranges` | 7.53ms | 9.28ms | 1.2× | 1.4% | PASS |
| mem_reads | `covering_index_scan` | 7.56ms | 10.95ms | 1.4× | 0.9% | PASS |
| mem_reads | `groupby_scan` | 35.71ms | 43.20ms | 1.2× | 0.6% | PASS |
| mem_reads | `index_join` | 7.83ms | 11.67ms | 1.5× | 2.0% | PASS |
| mem_reads | `index_join_scan` | 3.81ms | 6.57ms | 1.7× | 1.3% | PASS |
| mem_reads | `types_table_scan` | 1.04s | 1.37s | 1.3× | 1.0% | PASS |
| mem_reads | `table_scan` | 1.20s | 1.47s | 1.2× | 1.3% | PASS |
| mem_reads | `oltp_read_only` | 147.12ms | 185.82ms | 1.3× | 1.3% | PASS |
| mem_writes | `oltp_bulk_insert` | 238.61ms | 327.54ms | 1.4× | 1.0% | PASS |
| mem_writes | `oltp_insert` | 18.40ms | 32.62ms | 1.8× | 0.8% | PASS |
| mem_writes | `oltp_update_index` | 65.34ms | 118.52ms | 1.8× | 1.3% | PASS |
| mem_writes | `oltp_update_non_index` | 49.85ms | 81.74ms | 1.6× | 1.4% | PASS |
| mem_writes | `oltp_delete_insert` | 47.86ms | 89.29ms | 1.9× | 1.4% | PASS |
| mem_writes | `oltp_write_only` | 26.17ms | 52.52ms | 2.0× | 1.3% | PASS |
| mem_writes | `types_delete_insert` | 31.70ms | 52.74ms | 1.7× | 1.8% | PASS |
| mem_writes | `oltp_read_write` | 98.34ms | 155.63ms | 1.6× | 0.9% | PASS |
| file_reads | `oltp_point_select` | 101.36ms | 61.25ms | 0.6× | 1.3% | PASS |
| file_reads | `oltp_range_select` | 26.12ms | 26.28ms | 1.0× | 1.5% | PASS |
| file_reads | `oltp_sum_range` | 24.84ms | 24.88ms | 1.0× | 1.2% | PASS |
| file_reads | `oltp_order_range` | 4.39ms | 4.58ms | 1.0× | 1.5% | PASS |
| file_reads | `oltp_distinct_range` | 5.48ms | 5.77ms | 1.1× | 1.4% | PASS |
| file_reads | `oltp_index_scan` | 11.74ms | 8.59ms | 0.7× | 1.6% | PASS |
| file_reads | `select_random_points` | 36.79ms | 36.27ms | 1.0× | 1.6% | PASS |
| file_reads | `select_random_ranges` | 14.93ms | 11.63ms | 0.8× | 1.0% | PASS |
| file_reads | `covering_index_scan` | 14.93ms | 13.24ms | 0.9× | 1.1% | PASS |
| file_reads | `groupby_scan` | 36.67ms | 43.86ms | 1.2× | 0.7% | PASS |
| file_reads | `index_join` | 11.94ms | 13.19ms | 1.1× | 1.7% | PASS |
| file_reads | `index_join_scan` | 4.80ms | 6.87ms | 1.4× | 1.7% | PASS |
| file_reads | `types_table_scan` | 1.05s | 1.37s | 1.3× | 1.1% | PASS |
| file_reads | `table_scan` | 1.20s | 1.47s | 1.2× | 1.5% | PASS |
| file_reads | `oltp_read_only` | 250.87ms | 217.48ms | 0.9× | 0.9% | PASS |
| file_writes | `oltp_bulk_insert` | 254.12ms | 338.80ms | 1.3× | 1.1% | PASS |
| file_writes | `oltp_insert` | 27.96ms | 36.54ms | 1.3× | 1.6% | PASS |
| file_writes | `oltp_update_index` | 94.82ms | 131.75ms | 1.4× | 1.3% | PASS |
| file_writes | `oltp_update_non_index` | 79.15ms | 92.60ms | 1.2× | 1.8% | PASS |
| file_writes | `oltp_delete_insert` | 77.71ms | 101.66ms | 1.3× | 1.8% | PASS |
| file_writes | `oltp_write_only` | 55.11ms | 62.02ms | 1.1× | 1.3% | PASS |
| file_writes | `types_delete_insert` | 49.58ms | 60.92ms | 1.2× | 1.9% | PASS |
| file_writes | `oltp_read_write` | 130.39ms | 167.88ms | 1.3× | 1.5% | PASS |
| ac_reads | `oltp_point_select` | 55.18ms | 61.33ms | 1.1× | 1.3% | PASS |
| ac_reads | `oltp_range_select` | 21.50ms | 26.28ms | 1.2× | 1.9% | PASS |
| ac_reads | `oltp_sum_range` | 20.05ms | 24.92ms | 1.2× | 1.2% | PASS |
| ac_reads | `oltp_order_range` | 3.87ms | 4.57ms | 1.2× | 0.9% | PASS |
| ac_reads | `oltp_distinct_range` | 4.96ms | 5.78ms | 1.2× | 0.9% | PASS |
| ac_reads | `oltp_index_scan` | 7.06ms | 8.63ms | 1.2× | 2.1% | PASS |
| ac_reads | `select_random_points` | 30.74ms | 36.13ms | 1.2× | 1.3% | PASS |
| ac_reads | `select_random_ranges` | 10.09ms | 11.58ms | 1.1× | 1.4% | PASS |
| ac_reads | `covering_index_scan` | 10.21ms | 13.32ms | 1.3× | 1.2% | PASS |
| ac_reads | `groupby_scan` | 36.04ms | 43.84ms | 1.2× | 0.7% | PASS |
| ac_reads | `index_join` | 9.42ms | 13.22ms | 1.4× | 1.4% | PASS |
| ac_reads | `index_join_scan` | 4.25ms | 6.87ms | 1.6× | 1.7% | PASS |
| ac_reads | `types_table_scan` | 1.04s | 1.37s | 1.3× | 0.9% | PASS |
| ac_reads | `table_scan` | 1.20s | 1.47s | 1.2× | 1.1% | PASS |
| ac_reads | `oltp_read_only` | 181.94ms | 217.35ms | 1.2× | 1.3% | PASS |
| ac_writes | `oltp_bulk_insert_ac` | 26.38ms | 71.67ms | 2.7× | 7.1% | PASS |
| ac_writes | `oltp_insert_ac` | 28.19ms | 86.19ms | 3.1× | 7.0% | PASS |
| ac_writes | `oltp_update_index_ac` | 29.23ms | 97.28ms | 3.3× | 6.5% | PASS |
| ac_writes | `oltp_update_non_index_ac` | 26.71ms | 80.07ms | 3.0× | 6.3% | PASS |
| ac_writes | `oltp_delete_insert_ac` | 27.61ms | 89.29ms | 3.2× | 8.2% | PASS |
| ac_writes | `oltp_write_only_ac` | 28.87ms | 89.60ms | 3.1× | 6.7% | PASS |
| ac_writes | `types_delete_insert_ac` | 26.20ms | 83.15ms | 3.2× | 9.5% | PASS |
| ac_writes | `oltp_read_write_ac` | 34.80ms | 96.07ms | 2.8× | 6.5% | PASS |

</details>

</details>

## Version-control latency

Wall time: 4m 25s. Samples per benchmark: 101.

| Benchmark | Median | Ceiling | Ceiling used | MAD | Result |
|---|---:|---:|---:|---:|---|
| `status_clean_many_tables` | 34.30ms | 38.00ms | 90.3% | 0.5% | PASS |
| `status_dirty_many_tables` | 38.02ms | 42.00ms | 90.5% | 0.6% | PASS |
| `diff_regular_working_one_table` | 29.76ms | 33.00ms | 90.2% | 0.4% | PASS |
| `diff_regular_working_many_tables` | 42.90ms | 50.00ms | 85.8% | 0.5% | PASS |
| `diff_stat_working_many_tables` | 42.90ms | 48.00ms | 89.4% | 0.4% | PASS |
| `diff_schema_working_many_tables` | 43.51ms | 48.00ms | 90.6% | 0.6% | PASS |
| `branch_list_many_branches` | 23.79ms | 25.00ms | 95.2% | 1.6% | PASS |
| `branch_create_delete` | 23.48ms | 27.00ms | 87.0% | 0.6% | PASS |
| `at_literal_deep_history` | 23.82ms | 28.00ms | 85.1% | 0.5% | PASS |
| `diff_literal_deep_history` | 23.82ms | 28.00ms | 85.1% | 0.4% | PASS |
| `history_literal_deep_history` | 25.11ms | 30.00ms | 83.7% | 0.9% | PASS |
| `checkout_branch_clean` | 32.25ms | 43.00ms | 75.0% | 0.5% | PASS |
| `merge_data_no_conflicts` | 26.85ms | 33.00ms | 81.4% | 0.5% | PASS |
| `merge_data_secondary_index` | 879.20ms | 944.00ms | 93.1% | 0.3% | PASS |
| `merge_schema_no_conflicts` | 20.88ms | 24.00ms | 87.0% | 0.9% | PASS |
| `merge_data_conflicts` | 29.94ms | 33.00ms | 90.7% | 0.4% | PASS |
| `merge_data_conflicts_with_resolve` | 30.98ms | 34.00ms | 91.1% | 1.0% | PASS |

Version-control ceiling result: **PASS**.

## Reproducing

The workload definitions live in `test/sysbench_compare*.sh` and `test/vc_perf_ceiling.sh`. The nightly workflow retains the complete raw samples and generated reports as Actions artifacts for 30 days.
