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

/* A file:// read must see committed rows while a write transaction is open
** on the remote. A push still needs that remote's graph lock. */
static void test_file_fetch_during_remote_write(void){
  char zRemote[256], zLocal[256], zClone[256];
  char zSql[640], zMsg[128];
  sqlite3 *dbRemote = 0, *dbWriter = 0, *dbRead = 0;
  sqlite3 *dbLocal = 0, *dbClone = 0;
  char *zErr = 0;
  int n = -1;
  int rc;

  snprintf(zRemote, sizeof(zRemote),
           DOLTLITE_TEST_TMPDIR "/file_read_remote_%d.db", (int)getpid());
  snprintf(zLocal, sizeof(zLocal),
           DOLTLITE_TEST_TMPDIR "/file_read_local_%d.db", (int)getpid());
  snprintf(zClone, sizeof(zClone),
           DOLTLITE_TEST_TMPDIR "/file_read_clone_%d.db", (int)getpid());
  rmDb(zRemote);
  rmDb(zLocal);
  rmDb(zClone);

  check("open remote", openDb(zRemote, &dbRemote)==SQLITE_OK);
  check("seed", execSql(dbRemote,
    "CREATE TABLE t(id INTEGER PRIMARY KEY, v TEXT);"
    "INSERT INTO t VALUES(1,'a');"
    "SELECT dolt_commit('-Am','c1');")==SQLITE_OK);

  check("open local", openDb(zLocal, &dbLocal)==SQLITE_OK);
  sqlite3_snprintf(sizeof(zSql), zSql,
                   "SELECT dolt_clone('file://%s');", zRemote);
  check("clone", execSql(dbLocal, zSql)==SQLITE_OK);

  check("open writer", openDb(zRemote, &dbWriter)==SQLITE_OK);
  check("writer begins", execSql(dbWriter,
    "BEGIN; INSERT INTO t VALUES(50,'uncommitted');")==SQLITE_OK);
  rc = queryInt(dbWriter, "SELECT count(*) FROM t", &n);
  check("writer sees its row", rc==SQLITE_OK && n==2);

  check("open plain reader", openDb(zRemote, &dbRead)==SQLITE_OK);
  n = -1;
  rc = queryInt(dbRead, "SELECT count(*) FROM t", &n);
  check("plain read sees committed row", rc==SQLITE_OK && n==1);
  sqlite3_close(dbRead);

  n = -1;
  rc = queryInt(dbLocal, "SELECT dolt_fetch('origin');", &n);
  check("file_fetch_during_remote_write", rc==SQLITE_OK && n==0);
  n = -1;
  rc = queryInt(dbLocal, "SELECT dolt_pull('origin');", &n);
  check("file pull during remote write", rc==SQLITE_OK && n==0);

  check("open second clone", openDb(zClone, &dbClone)==SQLITE_OK);
  sqlite3_snprintf(sizeof(zSql), zSql,
                   "SELECT dolt_clone('file://%s');", zRemote);
  n = -1;
  rc = queryInt(dbClone, zSql, &n);
  check("file clone during remote write", rc==SQLITE_OK && n==0);
  n = -1;
  rc = queryInt(dbClone, "SELECT count(*) FROM t", &n);
  check("clone has only the committed row", rc==SQLITE_OK && n==1);
  sqlite3_close(dbClone);

  check("local commit", execSql(dbLocal,
    "INSERT INTO t VALUES(2,'local');"
    "SELECT dolt_commit('-Am','local');")==SQLITE_OK);
  rc = execErr(dbLocal, "SELECT dolt_push('origin','main');", &zErr);
  check("push still reports the lock",
        rc==SQLITE_BUSY && zErr && strstr(zErr, "locked")!=0);
  if( rc!=SQLITE_BUSY ){
    fprintf(stderr, "  push rc=%d err=%s\n", rc, zErr ? zErr : "(null)");
  }
  sqlite3_free(zErr);

  check("writer commits", execSql(dbWriter,
    "COMMIT; SELECT dolt_commit('-Am','c2');")==SQLITE_OK);
  n = -1;
  rc = queryInt(dbLocal, "SELECT dolt_pull('origin');", &n);
  check("pull after remote commit", rc==SQLITE_OK && n==0);
  n = -1;
  rc = queryInt(dbLocal, "SELECT count(*) FROM t", &n);
  check("pull brought the committed row", rc==SQLITE_OK && n==3);
  rc = queryText(dbLocal,
      "SELECT message FROM dolt_log('origin/main') LIMIT 1",
      zMsg, sizeof(zMsg));
  check("tracking advanced to c2", rc==SQLITE_OK && strcmp(zMsg, "c2")==0);

  sqlite3_close(dbWriter);
  sqlite3_close(dbLocal);
  sqlite3_close(dbRemote);
  rmDb(zRemote);
  rmDb(zLocal);
  rmDb(zClone);
}

int main(void){
  sqlite3_initialize();
  test_file_fetch_during_remote_write();
  printf("file_remote_read_lock_test: %d passed, %d failed\n", nPass, nFail);
  return nFail ? 1 : 0;
}
