#include <stdio.h>
#include <string.h>
#include <unistd.h>
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

static int execSql(sqlite3 *db, const char *zSql){
  char *zErr = 0;
  int rc = sqlite3_exec(db, zSql, 0, 0, &zErr);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL rc=%d %s\n  %s\n", rc, zErr ? zErr : "", zSql);
  }
  sqlite3_free(zErr);
  return rc;
}

static int execErr(sqlite3 *db, const char *zSql, char **pzErr){
  return sqlite3_exec(db, zSql, 0, 0, pzErr);
}

static int openDb(const char *zPath, sqlite3 **ppDb){
  int rc = sqlite3_open(zPath, ppDb);
  if( rc==SQLITE_OK ) sqlite3_busy_timeout(*ppDb, 0);
  return rc;
}

static int openRo(const char *zPath, sqlite3 **ppDb){
  char zUri[512];
  int rc;
  sqlite3_snprintf(sizeof(zUri), zUri, "file:%s?mode=ro", zPath);
  rc = sqlite3_open_v2(zUri, ppDb, SQLITE_OPEN_READONLY|SQLITE_OPEN_URI, 0);
  if( rc==SQLITE_OK ) sqlite3_busy_timeout(*ppDb, 0);
  return rc;
}

static int queryText(sqlite3 *db, const char *zSql, char *zOut, int nOut){
  sqlite3_stmt *st = 0;
  int rc;
  zOut[0] = 0;
  rc = sqlite3_prepare_v2(db, zSql, -1, &st, 0);
  if( rc!=SQLITE_OK ){
    sqlite3_snprintf(nOut, zOut, "prepare:%s", sqlite3_errmsg(db));
    return rc;
  }
  rc = sqlite3_step(st);
  if( rc==SQLITE_ROW ){
    const unsigned char *z = sqlite3_column_text(st, 0);
    sqlite3_snprintf(nOut, zOut, "%s", z ? (const char *)z : "");
    rc = SQLITE_OK;
  }else if( rc==SQLITE_DONE ){
    sqlite3_snprintf(nOut, zOut, "(no row)");
    rc = SQLITE_DONE;
  }else{
    sqlite3_snprintf(nOut, zOut, "step:%s", sqlite3_errmsg(db));
  }
  sqlite3_finalize(st);
  return rc;
}

static int queryInt(sqlite3 *db, const char *zSql, int *pOut){
  sqlite3_stmt *st = 0;
  int rc = sqlite3_prepare_v2(db, zSql, -1, &st, 0);
  if( rc!=SQLITE_OK ) return rc;
  rc = sqlite3_step(st);
  if( rc==SQLITE_ROW ){
    *pOut = sqlite3_column_int(st, 0);
    rc = SQLITE_OK;
  }
  sqlite3_finalize(st);
  return rc;
}

static void rmDb(const char *zPath){
  char zBuf[512];
  const char *zSlash = strrchr(zPath, '/');
  remove(zPath);
  sqlite3_snprintf(sizeof(zBuf), zBuf, "%s-lock", zPath);
  remove(zBuf);
  sqlite3_snprintf(sizeof(zBuf), zBuf, "%s-wal", zPath);
  remove(zBuf);
  if( zSlash ){
    sqlite3_snprintf(sizeof(zBuf), zBuf, "%.*s.%s-lock",
                     (int)(zSlash - zPath + 1), zPath, zSlash + 1);
  }else{
    sqlite3_snprintf(sizeof(zBuf), zBuf, ".%s-lock", zPath);
  }
  remove(zBuf);
}

static int readLog(const char *zPath, const char *zRev, char *zOut, int nOut){
  sqlite3 *db = 0;
  char zSql[160];
  int rc = openDb(zPath, &db);
  if( rc!=SQLITE_OK ){
    sqlite3_snprintf(nOut, zOut, "open");
    return rc;
  }
  sqlite3_snprintf(sizeof(zSql), zSql,
                   "SELECT message FROM dolt_log('%s') LIMIT 1", zRev);
  rc = queryText(db, zSql, zOut, nOut);
  sqlite3_close(db);
  return rc;
}

static int branchCount(const char *zPath, const char *zName, int *pOut){
  sqlite3 *db = 0;
  char zSql[160];
  int rc = openDb(zPath, &db);
  if( rc!=SQLITE_OK ) return rc;
  sqlite3_snprintf(sizeof(zSql), zSql,
      "SELECT count(*) FROM dolt_branches WHERE name='%s'", zName);
  rc = queryInt(db, zSql, pOut);
  sqlite3_close(db);
  return rc;
}

/* A local write lock or a read-only connection is refused before the remote
** ref moves. Deleting a remote branch this clone never fetched succeeds. */
static void test_push_tracking_preconditions(void){
  char zRemote[256], zLocal[256];
  char zSql[640], zMsg[128];
  sqlite3 *dbRemote = 0, *dbLocal = 0, *dbPeer = 0;
  sqlite3 *dbRo = 0, *dbQo = 0;
  char *zErr = 0;
  int n = -1;
  int rc;

  snprintf(zRemote, sizeof(zRemote),
           DOLTLITE_TEST_TMPDIR "/push_pre_remote_%d.db", (int)getpid());
  snprintf(zLocal, sizeof(zLocal),
           DOLTLITE_TEST_TMPDIR "/push_pre_local_%d.db", (int)getpid());
  rmDb(zRemote);
  rmDb(zLocal);

  check("open remote", openDb(zRemote, &dbRemote)==SQLITE_OK);
  check("seed", execSql(dbRemote,
    "CREATE TABLE t(id INTEGER PRIMARY KEY, v TEXT);"
    "INSERT INTO t VALUES(1,'a');"
    "SELECT dolt_commit('-Am','c1');")==SQLITE_OK);
  check("open local", openDb(zLocal, &dbLocal)==SQLITE_OK);
  sqlite3_snprintf(sizeof(zSql), zSql,
                   "SELECT dolt_clone('file://%s');", zRemote);
  check("clone", execSql(dbLocal, zSql)==SQLITE_OK);
  check("remote other", execSql(dbRemote,
      "SELECT dolt_branch('other');")==SQLITE_OK);
  check("local c2", execSql(dbLocal,
    "INSERT INTO t VALUES(2,'b');"
    "SELECT dolt_commit('-Am','c2');")==SQLITE_OK);

  check("open peer", openDb(zLocal, &dbPeer)==SQLITE_OK);
  check("peer holds write", execSql(dbPeer,
    "BEGIN IMMEDIATE; INSERT INTO t VALUES(99,'peer');")==SQLITE_OK);
  rc = execErr(dbLocal, "SELECT dolt_push('origin','main');", &zErr);
  check("push_locked_busy",
        rc==SQLITE_BUSY && zErr && strcmp(zErr, "database is locked")==0);
  if( rc!=SQLITE_BUSY || !zErr || strcmp(zErr, "database is locked")!=0 ){
    fprintf(stderr, "  locked push rc=%d err=%s\n", rc, zErr ? zErr : "(null)");
  }
  sqlite3_free(zErr);
  zErr = 0;
  rc = readLog(zRemote, "main", zMsg, sizeof(zMsg));
  check("push_locked_leaves_remote", rc==SQLITE_OK && strcmp(zMsg, "c1")==0);
  rc = queryText(dbLocal,
      "SELECT message FROM dolt_log('origin/main') LIMIT 1",
      zMsg, sizeof(zMsg));
  check("tracking stays c1", rc==SQLITE_OK && strcmp(zMsg, "c1")==0);

  check("peer rolls back", execSql(dbPeer, "ROLLBACK;")==SQLITE_OK);
  n = -1;
  rc = queryInt(dbLocal, "SELECT dolt_push('origin','main');", &n);
  check("push_after_unlock_advances", rc==SQLITE_OK && n==0);
  rc = readLog(zRemote, "main", zMsg, sizeof(zMsg));
  check("remote advances to c2", rc==SQLITE_OK && strcmp(zMsg, "c2")==0);
  rc = queryText(dbLocal,
      "SELECT message FROM dolt_log('origin/main') LIMIT 1",
      zMsg, sizeof(zMsg));
  check("tracking advances to c2", rc==SQLITE_OK && strcmp(zMsg, "c2")==0);

  check("open mode=ro", openRo(zLocal, &dbRo)==SQLITE_OK);
  rc = execErr(dbRo, "SELECT dolt_push('origin','main:ro_created');", &zErr);
  check("push_readonly_leaves_remote",
        rc==SQLITE_READONLY && zErr
        && strcmp(zErr, "attempt to write a readonly database")==0);
  if( rc!=SQLITE_READONLY || !zErr
   || strcmp(zErr, "attempt to write a readonly database")!=0 ){
    fprintf(stderr, "  ro push rc=%d err=%s\n", rc, zErr ? zErr : "(null)");
  }
  sqlite3_free(zErr);
  zErr = 0;
  n = -1;
  rc = branchCount(zRemote, "ro_created", &n);
  check("ro_created absent", rc==SQLITE_OK && n==0);

  check("open query_only", openDb(zLocal, &dbQo)==SQLITE_OK);
  check("set query_only", execSql(dbQo, "PRAGMA query_only=1;")==SQLITE_OK);
  rc = execErr(dbQo, "SELECT dolt_push('origin','main:qo_created');", &zErr);
  check("push_query_only_leaves_remote",
        rc==SQLITE_READONLY && zErr
        && strcmp(zErr, "attempt to write a readonly database")==0);
  if( rc!=SQLITE_READONLY || !zErr
   || strcmp(zErr, "attempt to write a readonly database")!=0 ){
    fprintf(stderr, "  qo push rc=%d err=%s\n", rc, zErr ? zErr : "(null)");
  }
  sqlite3_free(zErr);
  n = -1;
  rc = branchCount(zRemote, "qo_created", &n);
  check("qo_created absent", rc==SQLITE_OK && n==0);

  n = -1;
  rc = branchCount(zRemote, "other", &n);
  check("remote has other", rc==SQLITE_OK && n==1);
  n = -1;
  rc = queryInt(dbLocal, "SELECT dolt_push('origin',':other');", &n);
  check("push_delete_untracked_ok", rc==SQLITE_OK && n==0);
  n = -1;
  rc = branchCount(zRemote, "other", &n);
  check("untracked delete removed other", rc==SQLITE_OK && n==0);

  sqlite3_close(dbQo);
  sqlite3_close(dbRo);
  sqlite3_close(dbPeer);
  sqlite3_close(dbLocal);
  sqlite3_close(dbRemote);
  rmDb(zRemote);
  rmDb(zLocal);
}

int main(void){
  sqlite3_initialize();
  test_push_tracking_preconditions();
  printf("push_tracking_precondition_test: %d passed, %d failed\n",
         nPass, nFail);
  return nFail ? 1 : 0;
}
