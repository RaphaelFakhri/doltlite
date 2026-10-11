#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "sqlite3.h"
#include "doltlite_internal.h"
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

static void checkEq(const char *name, const char *got, const char *want){
  if( got && want && strcmp(got, want)==0 ){
    nPass++;
  }else{
    nFail++;
    fprintf(stderr, "FAIL: %s (got '%s', want '%s')\n",
            name, got ? got : "(null)", want ? want : "(null)");
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

static int openDb(const char *zPath, sqlite3 **ppDb){
  int rc = sqlite3_open(zPath, ppDb);
  if( rc==SQLITE_OK ) sqlite3_busy_timeout(*ppDb, 5000);
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

/* The slow fetch has already read C1. Commit C2 and let a second
** connection install it before the slow fetch writes its tracking ref. */
typedef struct FetchRace {
  sqlite3 *dbRemote;
  sqlite3 *dbPeer;
  int remoteRc;
  int fetchRc;
  int fired;
} FetchRace;

static void raceInstallHook(void *pArg){
  FetchRace *p = (FetchRace *)pArg;
  p->fired = 1;
  p->remoteRc = execSql(p->dbRemote,
      "INSERT INTO t VALUES(3,'c2');"
      "SELECT dolt_commit('-Am','C2');");
  if( p->remoteRc==SQLITE_OK ){
    p->fetchRc = execSql(p->dbPeer, "SELECT dolt_fetch('origin','main');");
  }
}

static void test_stale_fetch_keeps_newer_tracking(void){
  char zRemote[256], zLocal[256], zUrl[512], zMsg[128];
  sqlite3 *dbRemote = 0, *dbLocal = 0, *dbPeer = 0, *dbDisk = 0;
  FetchRace race;
  int fetchRc = -1;
  int rc;

  snprintf(zRemote, sizeof(zRemote),
           DOLTLITE_TEST_TMPDIR "/fetch_order_remote_%d.db", (int)getpid());
  snprintf(zLocal, sizeof(zLocal),
           DOLTLITE_TEST_TMPDIR "/fetch_order_local_%d.db", (int)getpid());
  rmDb(zRemote);
  rmDb(zLocal);

  check("open remote", openDb(zRemote, &dbRemote)==SQLITE_OK);
  check("seed base",
        execSql(dbRemote,
          "CREATE TABLE t(id INTEGER PRIMARY KEY, v TEXT);"
          "INSERT INTO t VALUES(1,'base');"
          "SELECT dolt_commit('-Am','base');")==SQLITE_OK);

  check("open local", openDb(zLocal, &dbLocal)==SQLITE_OK);
  sqlite3_snprintf(sizeof(zUrl), zUrl,
                   "SELECT dolt_clone('file://%s');", zRemote);
  check("clone", execSql(dbLocal, zUrl)==SQLITE_OK);
  check("remote C1",
        execSql(dbRemote,
          "INSERT INTO t VALUES(2,'c1');"
          "SELECT dolt_commit('-Am','C1');")==SQLITE_OK);
  check("open peer", openDb(zLocal, &dbPeer)==SQLITE_OK);

  memset(&race, 0, sizeof(race));
  race.dbRemote = dbRemote;
  race.dbPeer = dbPeer;
  race.remoteRc = SQLITE_ERROR;
  race.fetchRc = SQLITE_ERROR;
  doltliteTestSetBeforeRefInstallHook(raceInstallHook, &race);
  rc = queryInt(dbLocal, "SELECT dolt_fetch('origin','main');", &fetchRc);
  doltliteTestSetBeforeRefInstallHook(0, 0);

  check("slow fetch ran the hook", race.fired==1);
  check("hook commit C2", race.remoteRc==SQLITE_OK);
  check("fast fetch returns 0", race.fetchRc==SQLITE_OK);
  check("slow fetch returns 0", rc==SQLITE_OK && fetchRc==0);

  rc = queryText(dbLocal,
      "SELECT message FROM dolt_log('origin/main') LIMIT 1",
      zMsg, sizeof(zMsg));
  check("stale_fetch_keeps_newer_tracking",
        rc==SQLITE_OK && strcmp(zMsg, "C2")==0);
  if( rc!=SQLITE_OK || strcmp(zMsg, "C2")!=0 ){
    fprintf(stderr, "  origin/main tip is '%s'\n", zMsg);
  }
  rc = queryText(dbRemote,
      "SELECT message FROM dolt_log('main') LIMIT 1",
      zMsg, sizeof(zMsg));
  checkEq("remote tip stays C2", rc==SQLITE_OK ? zMsg : "", "C2");

  check("open disk", openDb(zLocal, &dbDisk)==SQLITE_OK);
  rc = queryText(dbDisk,
      "SELECT message FROM dolt_log('origin/main') LIMIT 1",
      zMsg, sizeof(zMsg));
  checkEq("reopened tracking is C2", rc==SQLITE_OK ? zMsg : "", "C2");
  sqlite3_close(dbDisk);

  check("remote C3",
        execSql(dbRemote,
          "INSERT INTO t VALUES(4,'c3');"
          "SELECT dolt_commit('-Am','C3');")==SQLITE_OK);
  fetchRc = -1;
  rc = queryInt(dbLocal, "SELECT dolt_fetch('origin','main');", &fetchRc);
  check("later fetch returns 0", rc==SQLITE_OK && fetchRc==0);
  rc = queryText(dbLocal,
      "SELECT message FROM dolt_log('origin/main') LIMIT 1",
      zMsg, sizeof(zMsg));
  checkEq("later fetch advances tracking", rc==SQLITE_OK ? zMsg : "", "C3");

  sqlite3_close(dbPeer);
  sqlite3_close(dbLocal);
  sqlite3_close(dbRemote);
  rmDb(zRemote);
  rmDb(zLocal);
}

int main(void){
  sqlite3_initialize();
  test_stale_fetch_keeps_newer_tracking();
  printf("fetch_tracking_order_test: %d passed, %d failed\n", nPass, nFail);
  return nFail ? 1 : 0;
}
