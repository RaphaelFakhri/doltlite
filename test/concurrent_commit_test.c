#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "lib/test_tmpdir.h"

static int nPass = 0;
static int nFail = 0;

static void check(const char *name, int condition){
  if( condition ){
    nPass++;
  }else{
    nFail++;
    fprintf(stderr, "FAIL: %s\n", name);
  }
}

static char result_buf[4096];
static const char *queryScalarText(sqlite3 *db, const char *sql){
  sqlite3_stmt *stmt = 0;
  int rc;
  result_buf[0] = 0;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if( rc!=SQLITE_OK ){
    snprintf(result_buf, sizeof(result_buf), "ERROR: %s", sqlite3_errmsg(db));
    return result_buf;
  }
  rc = sqlite3_step(stmt);
  if( rc==SQLITE_ROW ){
    const char *val = (const char*)sqlite3_column_text(stmt, 0);
    if( val ) snprintf(result_buf, sizeof(result_buf), "%s", val);
  }else if( rc==SQLITE_ERROR ){
    snprintf(result_buf, sizeof(result_buf), "ERROR: %s", sqlite3_errmsg(db));
  }
  sqlite3_finalize(stmt);
  return result_buf;
}

static int execSql(sqlite3 *db, const char *sql){
  char *err = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &err);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "  SQL error: %s (rc=%d)\n  SQL: %s\n", err ? err : "?", rc, sql);
    sqlite3_free(err);
  }
  return rc;
}

static void test_aaron_scenario(void){
  sqlite3 *db1 = 0, *db2 = 0;
  const char *dbpath = DOLTLITE_TEST_TMPDIR "/test_concurrent_commit.db";
  int rc;
  const char *res;

  printf("--- Test 1: Aaron's concurrent commit scenario ---\n");

  remove(dbpath);

  rc = sqlite3_open(dbpath, &db1);
  check("open_db1", rc==SQLITE_OK);

  execSql(db1, "CREATE TABLE vals (id INT, val INT)");
  res = queryScalarText(db1, "SELECT dolt_commit('-A', '-m', 'Initial commit')");
  check("initial_commit_ok", strlen(res)==40);

  rc = sqlite3_open(dbpath, &db2);
  check("open_db2", rc==SQLITE_OK);
  res = queryScalarText(db2, "SELECT count(*) FROM vals");
  check("db2_sees_table", strcmp(res, "0")==0);

  execSql(db1, "INSERT INTO vals VALUES (1, 1)");
  res = queryScalarText(db1, "SELECT dolt_commit('-A', '-m', 'add one')");
  check("commit_add_one", strlen(res)==40);

  /* db2 opened before db1's commit and has not observed it, but it is not
  ** pinned to that view: its next statement adopts the branch head, so this
  ** commit stacks on "add one" rather than racing it. Dolt 2.2.2 does the
  ** same -- verified against a sql-server with two live sessions, where the
  ** second session's commit is accepted and both commits and both rows
  ** survive. The pinned session that genuinely cannot see the peer is
  ** covered by test_pinned_session_commit_rejected below. */
  execSql(db2, "INSERT INTO vals VALUES (2, 2)");
  res = queryScalarText(db2, "SELECT dolt_commit('-A', '-m', 'add two')");
  check("commit_add_two_accepted", strlen(res)==40);

  sqlite3_close(db1);
  rc = sqlite3_open(dbpath, &db1);
  check("reopen_db1", rc==SQLITE_OK);

  res = queryScalarText(db1, "SELECT count(*) FROM dolt_log");
  /* Both commits plus the "Initialize data repository" seed commit. */
  check("log_has_both_commits", strcmp(res, "4")==0);

  res = queryScalarText(db1, "SELECT message FROM dolt_log LIMIT 1");
  check("latest_commit_is_add_two", strcmp(res, "add two")==0);

  res = queryScalarText(db1, "SELECT val FROM vals WHERE id=1");
  check("add_one_data_intact", strcmp(res, "1")==0);

  res = queryScalarText(db1, "SELECT val FROM vals WHERE id=2");
  check("add_two_data_intact", strcmp(res, "2")==0);

  sqlite3_close(db1);
  sqlite3_close(db2);
  remove(dbpath);
}

/* A session that pins its snapshot in a transaction cannot observe the peer
** commit, so its own commit is refused rather than built on a head it never
** saw. This is the protection the scenario above used to get for free from
** a stale head. */
static void test_pinned_session_commit_rejected(void){
  sqlite3 *db1 = 0, *db2 = 0;
  const char *dbpath = DOLTLITE_TEST_TMPDIR "/test_pinned_commit.db";
  int rc;
  const char *res;

  printf("--- Test 1b: pinned session's commit is rejected ---\n");

  remove(dbpath);
  rc = sqlite3_open(dbpath, &db1);
  check("pin_open_db1", rc==SQLITE_OK);
  execSql(db1, "CREATE TABLE vals (id INT, val INT)");
  res = queryScalarText(db1, "SELECT dolt_commit('-A', '-m', 'Initial commit')");
  check("pin_initial_commit", strlen(res)==40);

  rc = sqlite3_open(dbpath, &db2);
  check("pin_open_db2", rc==SQLITE_OK);
  execSql(db2, "BEGIN");
  queryScalarText(db2, "SELECT count(*) FROM vals");

  execSql(db1, "INSERT INTO vals VALUES (1, 1)");
  res = queryScalarText(db1, "SELECT dolt_commit('-A', '-m', 'add one')");
  check("pin_peer_commit", strlen(res)==40);

  rc = execSql(db2, "INSERT INTO vals VALUES (2, 2)");
  check("pin_write_rejected", (rc&0xff)==SQLITE_BUSY);
  rc = execSql(db2, "SELECT dolt_commit('-A', '-m', 'add two')");
  check("pin_commit_rejected", (rc&0xff)==SQLITE_BUSY);
  check("pin_transaction_preserved", !sqlite3_get_autocommit(db2));
  execSql(db2, "ROLLBACK");

  sqlite3_close(db1);
  sqlite3_close(db2);
  rc = sqlite3_open(dbpath, &db1);
  check("pin_reopen", rc==SQLITE_OK);
  res = queryScalarText(db1, "SELECT message FROM dolt_log LIMIT 1");
  check("pin_peer_commit_is_head", strcmp(res, "add one")==0);
  res = queryScalarText(db1, "SELECT val FROM vals WHERE id=1");
  check("pin_peer_data_intact", strcmp(res, "1")==0);
  sqlite3_close(db1);
  remove(dbpath);
}

static void test_single_connection(void){
  sqlite3 *db = 0;
  const char *dbpath = DOLTLITE_TEST_TMPDIR "/test_single_commit.db";
  int rc;
  const char *res;

  printf("--- Test 2: Single connection commits work normally ---\n");

  remove(dbpath);
  rc = sqlite3_open(dbpath, &db);
  check("single_open", rc==SQLITE_OK);

  execSql(db, "CREATE TABLE t1 (a INT, b TEXT)");
  res = queryScalarText(db, "SELECT dolt_commit('-A', '-m', 'first')");
  check("single_commit_1", strlen(res)==40);

  execSql(db, "INSERT INTO t1 VALUES (1, 'hello')");
  res = queryScalarText(db, "SELECT dolt_commit('-A', '-m', 'second')");
  check("single_commit_2", strlen(res)==40);

  execSql(db, "INSERT INTO t1 VALUES (2, 'world')");
  res = queryScalarText(db, "SELECT dolt_commit('-A', '-m', 'third')");
  check("single_commit_3", strlen(res)==40);

  res = queryScalarText(db, "SELECT count(*) FROM dolt_log");
  /* Three commits plus the seed commit. */
  check("single_log_count", strcmp(res, "4")==0);

  res = queryScalarText(db, "SELECT count(*) FROM t1");
  check("single_data_count", strcmp(res, "2")==0);

  sqlite3_close(db);
  remove(dbpath);
}

static void test_sequential_multi_connection(void){
  sqlite3 *db1 = 0, *db2 = 0;
  const char *dbpath = DOLTLITE_TEST_TMPDIR "/test_sequential_commit.db";
  int rc;
  const char *res;

  printf("--- Test 3: Sequential multi-connection commits (no overlap) ---\n");

  remove(dbpath);

  rc = sqlite3_open(dbpath, &db1);
  check("seq_open_db1", rc==SQLITE_OK);
  rc = sqlite3_open(dbpath, &db2);
  check("seq_open_db2", rc==SQLITE_OK);

  execSql(db1, "CREATE TABLE seq (id INT, name TEXT)");
  res = queryScalarText(db1, "SELECT dolt_commit('-A', '-m', 'create seq')");
  check("seq_first_commit", strlen(res)==40);

  sqlite3_close(db2);
  rc = sqlite3_open(dbpath, &db2);
  check("seq_reopen_db2", rc==SQLITE_OK);

  execSql(db2, "INSERT INTO seq VALUES (1, 'from_db2')");
  res = queryScalarText(db2, "SELECT dolt_commit('-A', '-m', 'db2 insert')");
  check("seq_db2_commit", strlen(res)==40);

  sqlite3_close(db1);
  rc = sqlite3_open(dbpath, &db1);
  check("seq_reopen_db1", rc==SQLITE_OK);

  execSql(db1, "INSERT INTO seq VALUES (2, 'from_db1')");
  res = queryScalarText(db1, "SELECT dolt_commit('-A', '-m', 'db1 insert')");
  check("seq_db1_commit", strlen(res)==40);

  res = queryScalarText(db1, "SELECT count(*) FROM seq");
  check("seq_both_rows", strcmp(res, "2")==0);

  res = queryScalarText(db1, "SELECT count(*) FROM dolt_log");
  /* Three commits plus the seed commit. */
  check("seq_log_count", strcmp(res, "4")==0);

  sqlite3_close(db1);
  sqlite3_close(db2);
  remove(dbpath);
}

static void test_checkout_then_peer_write(void){
  const char *azCreate[] = {
    "CREATE TABLE w(id INTEGER PRIMARY KEY,v TEXT)",
    "CREATE TABLE w(id INT PRIMARY KEY,v TEXT)",
    "CREATE TABLE w(id INT PRIMARY KEY,v TEXT) WITHOUT ROWID"
  };
  int layout, sameBranch, busy;
  for(layout=0; layout<3; layout++){
    for(sameBranch=0; sameBranch<2; sameBranch++){
      for(busy=0; busy<2; busy++){
        char path[512], branchPath[544], otherPath[544];
        sqlite3 *db = 0, *peer = 0, *other = 0;
        int rc;
        char *err = 0;
        snprintf(path, sizeof(path), DOLTLITE_TEST_TMPDIR
            "/checkout_peer_%d_%d_%d.db", layout, sameBranch, busy);
        snprintf(branchPath, sizeof(branchPath), "%s@b1", path);
        snprintf(otherPath, sizeof(otherPath), "%s@other", path);
        remove(path);
        printf("--- Checkout peer: layout=%d same_branch=%d busy=%d ---\n",
               layout, sameBranch, busy);
        check("checkout_peer_open_setup", sqlite3_open(path, &db)==SQLITE_OK);
        check("checkout_peer_create", execSql(db, azCreate[layout])==SQLITE_OK);
        check("checkout_peer_setup", execSql(db,
            "INSERT INTO w VALUES(1,'base'),(2,'tracked');"
            "CREATE INDEX ix_w_v ON w(v);"
            "SELECT dolt_commit('-Am','base');"
            "SELECT dolt_branch('b1');"
            "SELECT dolt_checkout('-b','other');"
            "INSERT INTO w VALUES(3,'other');"
            "SELECT dolt_commit('-am','other');")==SQLITE_OK);
        sqlite3_close(db);
        db = 0;
        check("checkout_peer_open_victim",
              sqlite3_open(branchPath, &db)==SQLITE_OK);
        check("checkout_peer_open_writer",
              sqlite3_open(branchPath, &peer)==SQLITE_OK);
        check("checkout_peer_open_other",
              sqlite3_open(otherPath, &other)==SQLITE_OK);
        sqlite3_busy_timeout(db, 0);
        sqlite3_busy_timeout(peer, 0);
        sqlite3_busy_timeout(other, 0);
        check("checkout_peer_load_view", strcmp(queryScalarText(db,
              "SELECT count(*) FROM w"), "2")==0);
        if( sameBranch ){
          if( busy ){
            check("checkout_peer_hold_merge_lock",
                  execSql(other, "BEGIN IMMEDIATE")==SQLITE_OK);
            rc = sqlite3_exec(db, "SELECT dolt_merge('other')", 0, 0, &err);
            check("checkout_peer_merge_busy", (rc&0xff)==SQLITE_BUSY);
            sqlite3_free(err);
            err = 0;
            check("checkout_peer_release_merge_lock",
                  execSql(other, "ROLLBACK")==SQLITE_OK);
          }
        }else{
          check("checkout_peer_discard", execSql(db,
              "DELETE FROM w WHERE id=2;"
              "SELECT dolt_checkout('w');")==SQLITE_OK);
          check("checkout_peer_soft_reset",
                execSql(peer, "SELECT dolt_reset('--soft')")==SQLITE_OK);
        }
        check("checkout_peer_acknowledged", execSql(peer,
              "INSERT INTO w VALUES(99,'peer')")==SQLITE_OK);
        if( sameBranch ){
          check("checkout_peer_advance", execSql(peer,
              "CREATE INDEX ix_peer ON w(id,v);"
              "SELECT dolt_commit('-Am','peer');")==SQLITE_OK);
          check("checkout_peer_same_branch",
                execSql(db, "SELECT dolt_checkout('b1')")==SQLITE_OK);
        }else{
          if( busy ){
            check("checkout_peer_hold_begin_lock",
                  execSql(other, "BEGIN IMMEDIATE")==SQLITE_OK);
            rc = sqlite3_exec(db, "BEGIN IMMEDIATE", 0, 0, &err);
            check("checkout_peer_begin_busy", (rc&0xff)==SQLITE_BUSY);
            sqlite3_free(err);
            err = 0;
            check("checkout_peer_release_begin_lock",
                  execSql(other, "COMMIT")==SQLITE_OK);
          }
          check("checkout_peer_begin",
                execSql(db, "BEGIN IMMEDIATE")==SQLITE_OK);
        }
        check("checkout_peer_publish", execSql(db, sameBranch
              ? "SELECT dolt_commit('-Am','after','--allow-empty')"
              : "SELECT dolt_commit('-am','after')")==SQLITE_OK);
        check("checkout_peer_committed_row", strcmp(queryScalarText(db,
              "SELECT count(*) FROM dolt_at_w('HEAD') WHERE id=99"), "1")==0);
        check("checkout_peer_parent", strcmp(queryScalarText(db,
              "SELECT message FROM dolt_log LIMIT 1 OFFSET 1"),
              sameBranch ? "peer" : "base")==0);
        check("checkout_peer_committed_index", strcmp(queryScalarText(db,
              "SELECT count(*) FROM sqlite_schema WHERE type='index' "
              "AND name IN ('ix_w_v','ix_peer')"), sameBranch ? "2" : "1")==0);
        sqlite3_close(other);
        sqlite3_close(peer);
        sqlite3_close(db);
        db = 0;
        check("checkout_peer_reopen", sqlite3_open(branchPath, &db)==SQLITE_OK);
        check("checkout_peer_durable_rows", strcmp(queryScalarText(db,
              "SELECT group_concat(id) FROM (SELECT id FROM w ORDER BY id)"),
              "1,2,99")==0);
        check("checkout_peer_durable_indexes", strcmp(queryScalarText(db,
              "SELECT count(*) FROM sqlite_schema WHERE type='index' "
              "AND name IN ('ix_w_v','ix_peer')"), sameBranch ? "2" : "1")==0);
        check("checkout_peer_integrity", strcmp(queryScalarText(db,
              "PRAGMA integrity_check"), "ok")==0);
        sqlite3_close(db);
        remove(path);
      }
    }
  }
}

typedef struct ApplyPeerProbe ApplyPeerProbe;
struct ApplyPeerProbe {
  sqlite3 *peer;
  int fired;
  int rc;
};

static int applyPeerTrace(unsigned type, void *pArg, void *pStmt, void *pSql){
  ApplyPeerProbe *p = (ApplyPeerProbe*)pArg;
  const char *sql = sqlite3_sql((sqlite3_stmt*)pStmt);
  (void)pSql;
  if( type==SQLITE_TRACE_STMT && !p->fired && sql
   && strstr(sql, "SELECT 1 FROM main.sqlite_master WHERE")
   && strstr(sql, "REFERENCES") ){
    p->fired = 1;
    p->rc = sqlite3_exec(p->peer, "BEGIN IMMEDIATE", 0, 0, 0);
  }
  return 0;
}

static void test_apply_keeps_write_lock(void){
  const char *azCreate[] = {
    "CREATE TABLE w(id INTEGER PRIMARY KEY,v TEXT)",
    "CREATE TABLE w(id INT PRIMARY KEY,v TEXT)",
    "CREATE TABLE w(id INT PRIMARY KEY,v TEXT) WITHOUT ROWID"
  };
  int layout, revert, otherBranch;
  for(layout=0; layout<3; layout++){
    for(revert=0; revert<2; revert++){
      for(otherBranch=0; otherBranch<2; otherBranch++){
        char path[512], branchPath[544], peerPath[544];
        sqlite3 *db = 0, *peer = 0;
        ApplyPeerProbe probe;
        int rc;
        snprintf(path, sizeof(path), DOLTLITE_TEST_TMPDIR
            "/apply_peer_%d_%d_%d.db", layout, revert, otherBranch);
        snprintf(branchPath, sizeof(branchPath), "%s@%s", path,
                 revert ? "feature" : "main");
        snprintf(peerPath, sizeof(peerPath), "%s@%s", path,
                 otherBranch ? "other" : (revert ? "feature" : "main"));
        remove(path);
        printf("--- Apply peer: layout=%d revert=%d other_branch=%d ---\n",
               layout, revert, otherBranch);
        check("apply_peer_setup_open", sqlite3_open(path, &db)==SQLITE_OK);
        check("apply_peer_create", execSql(db, azCreate[layout])==SQLITE_OK);
        check("apply_peer_setup", execSql(db,
            "INSERT INTO w VALUES(1,'base');"
            "CREATE INDEX ix_w_v ON w(v);"
            "SELECT dolt_commit('-Am','base');"
            "SELECT dolt_checkout('-b','feature');"
            "DROP INDEX ix_w_v; INSERT INTO w VALUES(2,'feature');"
            "SELECT dolt_commit('-Am','feature');"
            "SELECT dolt_branch('other');")==SQLITE_OK);
        sqlite3_close(db);
        db = 0;
        check("apply_peer_open", sqlite3_open(branchPath, &db)==SQLITE_OK);
        check("apply_peer_writer_open", sqlite3_open(peerPath, &peer)==SQLITE_OK);
        sqlite3_busy_timeout(db, 0);
        sqlite3_busy_timeout(peer, 0);
        memset(&probe, 0, sizeof(probe));
        probe.peer = peer;
        probe.rc = SQLITE_ERROR;
        sqlite3_trace_v2(db, SQLITE_TRACE_STMT, applyPeerTrace, &probe);
        rc = sqlite3_exec(db, revert ? "SELECT dolt_revert('HEAD')"
            : "SELECT dolt_cherry_pick('feature')", 0, 0, 0);
        sqlite3_trace_v2(db, 0, 0, 0);
        check("apply_peer_attempted", probe.fired);
        check("apply_peer_write_blocked", (probe.rc&0xff)==SQLITE_BUSY);
        if( !sqlite3_get_autocommit(peer) ){
          check("apply_peer_release", execSql(peer, "ROLLBACK")==SQLITE_OK);
        }
        check("apply_peer_success", rc==SQLITE_OK);
        check("apply_peer_autocommit", sqlite3_get_autocommit(db));
        check("apply_peer_txn_ended", sqlite3_txn_state(db, "main")==SQLITE_TXN_NONE);
        check("apply_peer_head_rows", strcmp(queryScalarText(db,
            "SELECT group_concat(id) FROM "
            "(SELECT id FROM dolt_at_w('HEAD') ORDER BY id)"),
            revert ? "1" : "1,2")==0);
        sqlite3_close(peer);
        sqlite3_close(db);
        db = 0;
        check("apply_peer_reopen", sqlite3_open(branchPath, &db)==SQLITE_OK);
        check("apply_peer_durable_rows", strcmp(queryScalarText(db,
            "SELECT group_concat(id) FROM (SELECT id FROM w ORDER BY id)"),
            revert ? "1" : "1,2")==0);
        check("apply_peer_durable_index", strcmp(queryScalarText(db,
            "SELECT count(*) FROM sqlite_schema WHERE name='ix_w_v'"),
            revert ? "1" : "0")==0);
        check("apply_peer_durable_integrity", strcmp(queryScalarText(db,
            "PRAGMA integrity_check"), "ok")==0);
        sqlite3_close(db);
        remove(path);
      }
    }
  }
}

int main(){
  printf("=== Concurrent Commit Test ===\n\n");

  test_aaron_scenario();
  test_pinned_session_commit_rejected();
  test_single_connection();
  test_sequential_multi_connection();
  test_checkout_then_peer_write();
  test_apply_keeps_write_lock();

  printf("\nResults: %d passed, %d failed out of %d tests\n",
    nPass, nFail, nPass+nFail);
  return nFail > 0 ? 1 : 0;
}
