#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
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

static int touchFile(const char *zPath){
  FILE *f = fopen(zPath, "w");
  if( !f ) return 0;
  if( fputc('x', f)==EOF ){
    fclose(f);
    return 0;
  }
  return fclose(f)==0;
}

static int waitReady(const char *zReady){
  int i;
  for(i=0; i<250; i++){
    if( access(zReady, F_OK)==0 ) return 1;
    usleep(20000);
  }
  return 0;
}

typedef struct FetchJob FetchJob;
struct FetchJob {
  sqlite3 *db;
  int rc;
  char *zErr;
};

static void *fetchThreadMain(void *pArg){
  FetchJob *p = (FetchJob *)pArg;
  p->zErr = 0;
  p->rc = sqlite3_exec(p->db, "SELECT dolt_fetch('origin','main');",
                       0, 0, &p->zErr);
  return 0;
}

/* bAbort: stop the download after the local insert and expect tracking to
** stay put. Otherwise the fetch finishes and the tracking ref moves. */
static void test_fetch_download(int bAbort){
  char zRemote[512], zLocal[512], zPause[512];
  /* A full pause path plus ".ready" or ".fail" is 518 bytes. */
  char zReady[520], zFail[520];
  char zSql[640], zMsg[128];
  sqlite3 *dbRemote = 0, *dbLocal = 0, *dbWrite = 0, *dbCheck = 0;
  FetchJob job;
  pthread_t thread;
  int n = -1;
  int rc;
  int started;
  const char *zTag = bAbort ? "abort" : "live";

  snprintf(zRemote, sizeof(zRemote),
           DOLTLITE_TEST_TMPDIR "/fetch_dl_%s_remote_%d.db", zTag, (int)getpid());
  snprintf(zLocal, sizeof(zLocal),
           DOLTLITE_TEST_TMPDIR "/fetch_dl_%s_local_%d.db", zTag, (int)getpid());
  snprintf(zPause, sizeof(zPause),
           DOLTLITE_TEST_TMPDIR "/fetch_dl_%s_pause_%d", zTag, (int)getpid());
  snprintf(zReady, sizeof(zReady), "%s.ready", zPause);
  snprintf(zFail, sizeof(zFail), "%s.fail", zPause);
  rmDb(zRemote);
  rmDb(zLocal);
  remove(zPause);
  remove(zReady);
  remove(zFail);
  unsetenv("DOLTLITE_PAUSE_DURING_LOCAL_PUT");
  unsetenv("DOLTLITE_FETCH_FLUSH_BYTES");

  check(bAbort ? "abort: open remote" : "live: open remote",
        openDb(zRemote, &dbRemote)==SQLITE_OK);
  check(bAbort ? "abort: seed" : "live: seed",
        execSql(dbRemote,
          "CREATE TABLE t(id INTEGER PRIMARY KEY, v TEXT);"
          "INSERT INTO t VALUES(1,'base');"
          "SELECT dolt_commit('-Am','base');")==SQLITE_OK);

  check(bAbort ? "abort: open local" : "live: open local",
        openDb(zLocal, &dbLocal)==SQLITE_OK);
  sqlite3_snprintf(sizeof(zSql), zSql,
                   "SELECT dolt_clone('file://%s');"
                   "CREATE TABLE app(x INTEGER);"
                   "INSERT INTO app VALUES(0);"
                   "SELECT dolt_commit('-Am','app');", zRemote);
  check(bAbort ? "abort: clone" : "live: clone", execSql(dbLocal, zSql)==SQLITE_OK);
  check(bAbort ? "abort: remote moves" : "live: remote moves",
        execSql(dbRemote,
          "INSERT INTO t VALUES(2,'more');"
          "SELECT dolt_commit('-Am','more');")==SQLITE_OK);

  check(bAbort ? "abort: pause file" : "live: pause file", touchFile(zPause));
  setenv("DOLTLITE_PAUSE_DURING_LOCAL_PUT", zPause, 1);
  if( !bAbort ) setenv("DOLTLITE_FETCH_FLUSH_BYTES", "1", 1);

  memset(&job, 0, sizeof(job));
  job.db = dbLocal;
  job.rc = SQLITE_ERROR;
  started = pthread_create(&thread, 0, fetchThreadMain, &job)==0;
  check(bAbort ? "abort: fetch thread" : "live: fetch thread", started);

  if( started ){
    check(bAbort ? "abort: download paused" : "live: download paused",
          waitReady(zReady));
    check(bAbort ? "abort: open writer" : "live: open writer",
          openDb(zLocal, &dbWrite)==SQLITE_OK);
    rc = execSql(dbWrite, "INSERT INTO app VALUES(1);");
    check(bAbort ? "fetch_abort_write_during_download"
                 : "fetch_download_allows_local_write",
          rc==SQLITE_OK);
    if( bAbort ){
      check("abort: fail marker", touchFile(zFail));
    }
    remove(zPause);
    pthread_join(thread, 0);
    if( bAbort ){
      check("fetch_abort_leaves_tracking", job.rc!=SQLITE_OK);
    }else{
      check("live: fetch finished", job.rc==SQLITE_OK);
      if( job.rc!=SQLITE_OK ){
        fprintf(stderr, "  fetch rc=%d err=%s\n", job.rc,
                job.zErr ? job.zErr : "(null)");
      }
    }
    sqlite3_free(job.zErr);
  }

  unsetenv("DOLTLITE_PAUSE_DURING_LOCAL_PUT");
  unsetenv("DOLTLITE_FETCH_FLUSH_BYTES");
  remove(zPause);
  remove(zReady);
  remove(zFail);

  check(bAbort ? "abort: reopen" : "live: reopen",
        openDb(zLocal, &dbCheck)==SQLITE_OK);
  n = -1;
  rc = queryInt(dbCheck, "SELECT count(*) FROM app WHERE x=1", &n);
  check(bAbort ? "abort: inserted row kept" : "live: inserted row kept",
        rc==SQLITE_OK && n==1);
  rc = queryText(dbCheck,
      "SELECT message FROM dolt_log('origin/main') LIMIT 1",
      zMsg, sizeof(zMsg));
  if( bAbort ){
    check("abort: tracking still base",
          rc==SQLITE_OK && strcmp(zMsg, "base")==0);
  }else{
    check("live: tracking installed",
          rc==SQLITE_OK && strcmp(zMsg, "more")==0);
  }
  if( rc!=SQLITE_OK || strcmp(zMsg, bAbort ? "base" : "more")!=0 ){
    fprintf(stderr, "  origin/main tip is '%s'\n", zMsg);
  }
  rc = queryText(dbCheck,
      "SELECT message FROM dolt_log('main') LIMIT 1",
      zMsg, sizeof(zMsg));
  check(bAbort ? "abort: local branch stays" : "live: local branch stays",
        rc==SQLITE_OK && strcmp(zMsg, "app")==0);

  if( dbCheck ) sqlite3_close(dbCheck);
  if( dbWrite ) sqlite3_close(dbWrite);
  if( dbLocal ) sqlite3_close(dbLocal);
  if( dbRemote ) sqlite3_close(dbRemote);
  rmDb(zRemote);
  rmDb(zLocal);
}

int main(void){
  sqlite3_initialize();
  test_fetch_download(0);
  test_fetch_download(1);
  printf("fetch_download_lock_test: %d passed, %d failed\n", nPass, nFail);
  return nFail ? 1 : 0;
}
