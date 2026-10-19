<!-- Draft: DoltLite 0.51.1. Changes from v0.51.0 through master 78f0785fab. -->

Improved version-control concurrency and recovery, schema merging, diff reporting, and remote synchronization.

## Reliability and correctness

- Prevented lost concurrent writes and stale branch state during checkout, reset, cherry-pick, and revert, including deferred transactions and savepoints.
- Fixed checkout and hard-reset recovery after concurrent branch changes and garbage collection.
- Fixed storage allocation on reopened connections to prevent new tables from overwriting existing tables.
- Corrected merges involving dropped or reordered columns and conflicting generated-column definitions.
- Made schema-conflict failures during rebase, cherry-pick, and revert leave the branch's schema, data, and staging unchanged.
- Fixed hard resets when untracked tables share names with tracked tables; preserved AUTOINCREMENT counters when `sqlite_sequence` is ignored.
- Aligned `dolt_diff_stat` and `dolt_diff_summary` with Dolt for schema-only and primary-key changes.

## Performance and testing

- Increased chunk-transfer batches to reduce request overhead during eager clone, fetch, pull, and push.
- Expanded version-control stress testing with replayable seeds, broader concurrency scenarios, and better failure diagnostics; extended schema-merge and Dolt oracle coverage.
