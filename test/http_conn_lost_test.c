#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include "sqlite3.h"
#include "lib/test_tmpdir.h"

static int nPass = 0;
static int nFail = 0;

static void check(const char *zName, int condition){
  if( condition ){
    nPass++;
  }else{
    nFail++;
    fprintf(stderr, "FAIL: %s\n", zName);
  }
}

static int execSql(sqlite3 *db, const char *zSql){
  char *zErr = 0;
  int rc = sqlite3_exec(db, zSql, 0, 0, &zErr);
  sqlite3_free(zErr);
  return rc;
}

/* Read one request, then hang up. mode 0 closes cleanly (EOF). mode 1
** aborts the socket (read error). */
struct DropListener {
  int listenFd;
  int mode;
  /* Set by the stopper. accept does not wake when another thread closes
  ** the listening socket, so the server polls this instead of blocking. */
  volatile int stop;
  pthread_t thread;
};

static void drainAndClose(int fd, int mode){
  char buf[8192];
  char *zMark;
  char *zLen;
  int nHave = 0;
  int headerEnd = -1;
  int contentLength = 0;
  int n;
  int bodyNeed;
  struct timeval tv;
  struct linger lin;

  tv.tv_sec = 2;
  tv.tv_usec = 0;
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  while( nHave<(int)sizeof(buf)-1 ){
    n = (int)recv(fd, buf+nHave, sizeof(buf)-1-nHave, 0);
    if( n<=0 ) break;
    nHave += n;
    buf[nHave] = 0;
    if( headerEnd<0 ){
      zMark = strstr(buf, "\r\n\r\n");
      if( !zMark ) continue;
      headerEnd = (int)(zMark - buf) + 4;
      contentLength = 0;
      zLen = strstr(buf, "Content-Length:");
      if( zLen && zLen < buf+headerEnd ){
        contentLength = atoi(zLen + 15);
      }
    }
    bodyNeed = headerEnd;
    if( contentLength>0 ) bodyNeed += contentLength;
    if( headerEnd>=0 && nHave>=bodyNeed ) break;
  }
  if( mode ){
    lin.l_onoff = 1;
    lin.l_linger = 0;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));
  }
  close(fd);
}

static void *serveDrops(void *pArg){
  struct DropListener *p = (struct DropListener *)pArg;
  while( !p->stop ){
    fd_set rfds;
    struct timeval tv;
    int fd;
    int rc;
    FD_ZERO(&rfds);
    FD_SET(p->listenFd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 200000;
    rc = select(p->listenFd + 1, &rfds, 0, 0, &tv);
    if( p->stop ) break;
    if( rc<0 ){
      if( errno==EINTR ) continue;
      break;
    }
    if( rc==0 ) continue;
    fd = accept(p->listenFd, 0, 0);
    if( fd<0 ) continue;
    drainAndClose(fd, p->mode);
  }
  return 0;
}

static int startDrop(struct DropListener *p, int mode, int *pPort){
  struct sockaddr_in addr;
  socklen_t nAddr = sizeof(addr);
  int one = 1;
  int flags;
  int fd;

  *pPort = 0;
  p->listenFd = -1;
  p->mode = mode;
  p->stop = 0;
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if( fd<0 ) return 0;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(0);
  flags = fcntl(fd, F_GETFL, 0);
  if( bind(fd, (struct sockaddr *)&addr, sizeof(addr))!=0
   || listen(fd, 16)!=0
   || flags<0
   || fcntl(fd, F_SETFL, flags | O_NONBLOCK)!=0
   || getsockname(fd, (struct sockaddr *)&addr, &nAddr)!=0 ){
    close(fd);
    return 0;
  }
  p->listenFd = fd;
  *pPort = (int)ntohs(addr.sin_port);
  if( pthread_create(&p->thread, 0, serveDrops, p)!=0 ){
    close(fd);
    p->listenFd = -1;
    return 0;
  }
  return 1;
}

static void stopDrop(struct DropListener *p){
  if( p->listenFd>=0 ){
    p->stop = 1;
    pthread_join(p->thread, 0);
    close(p->listenFd);
    p->listenFd = -1;
  }
}

static void test_push_drop(int mode, const char *zName){
  struct DropListener srv;
  sqlite3 *db = 0;
  char zPath[256];
  char zSql[512];
  char zCheck[80];
  char *zErr = 0;
  int port = 0;
  int rc;

  snprintf(zPath, sizeof(zPath),
           DOLTLITE_TEST_TMPDIR "/http_lost_%s_%d.db", zName, (int)getpid());
  remove(zPath);
  check(zName, startDrop(&srv, mode, &port));
  if( port==0 ) return;

  rc = sqlite3_open(zPath, &db);
  snprintf(zCheck, sizeof(zCheck), "%s: open", zName);
  check(zCheck, rc==SQLITE_OK);
  if( rc!=SQLITE_OK ){
    stopDrop(&srv);
    return;
  }
  sqlite3_extended_result_codes(db, 1);
  snprintf(zCheck, sizeof(zCheck), "%s: seed", zName);
  check(zCheck, execSql(db,
    "CREATE TABLE t(id INTEGER PRIMARY KEY);"
    "INSERT INTO t VALUES(1);"
    "SELECT dolt_commit('-Am','base');")==SQLITE_OK);
  snprintf(zSql, sizeof(zSql),
           "SELECT dolt_remote('add','origin','http://127.0.0.1:%d/repo.db');"
           "SELECT dolt_push('origin','main');", port);
  rc = sqlite3_exec(db, zSql, 0, 0, &zErr);
  if( rc!=SQLITE_ERROR || !zErr
   || strstr(zErr, "connection to remote lost")==0 ){
    fprintf(stderr, "  %s rc=%d err=%s\n", zName, rc, zErr ? zErr : "(null)");
  }
  snprintf(zCheck, sizeof(zCheck), "%s: code", zName);
  check(zCheck, rc==SQLITE_ERROR);
  snprintf(zCheck, sizeof(zCheck), "%s: extended code", zName);
  check(zCheck, sqlite3_extended_errcode(db)==SQLITE_ERROR);
  snprintf(zCheck, sizeof(zCheck), "%s: message", zName);
  check(zCheck, zErr && strstr(zErr, "connection to remote lost")!=0);
  snprintf(zCheck, sizeof(zCheck), "%s: locking text absent", zName);
  check(zCheck, zErr && strstr(zErr, "locking protocol")==0);
  snprintf(zCheck, sizeof(zCheck), "%s: disk text absent", zName);
  check(zCheck, zErr && strstr(zErr, "disk I/O")==0);
  sqlite3_free(zErr);
  sqlite3_close(db);
  stopDrop(&srv);
  remove(zPath);
}

static void test_connect_failure(void){
  sqlite3 *db = 0;
  char zPath[256];
  char *zErr = 0;
  int rc;

  snprintf(zPath, sizeof(zPath),
           DOLTLITE_TEST_TMPDIR "/http_lost_connect_%d.db", (int)getpid());
  remove(zPath);
  rc = sqlite3_open(zPath, &db);
  check("connect: open", rc==SQLITE_OK);
  if( rc!=SQLITE_OK ) return;
  sqlite3_extended_result_codes(db, 1);
  check("connect: seed", execSql(db,
    "CREATE TABLE t(id INTEGER PRIMARY KEY);"
    "INSERT INTO t VALUES(1);"
    "SELECT dolt_commit('-Am','base');")==SQLITE_OK);
  rc = sqlite3_exec(db,
    "SELECT dolt_remote('add','origin','http://127.0.0.1:1/repo.db');"
    "SELECT dolt_push('origin','main');", 0, 0, &zErr);
  if( rc!=SQLITE_ERROR || !zErr
   || strstr(zErr, "could not connect to remote")==0 ){
    fprintf(stderr, "  connect rc=%d err=%s\n", rc, zErr ? zErr : "(null)");
  }
  check("connect: code is SQLITE_ERROR", rc==SQLITE_ERROR);
  check("connect: names the connect failure",
        zErr && strstr(zErr, "could not connect to remote")!=0);
  sqlite3_free(zErr);
  sqlite3_close(db);
  remove(zPath);
}

int main(void){
  sqlite3_initialize();
  setenv("DOLTLITE_HTTP_TIMEOUT_MS", "3000", 1);
  printf("http connection-lost error test\n");
  printf("================================\n\n");

  test_push_drop(0, "eof");
  test_push_drop(1, "reset");
  test_connect_failure();

  printf("\n%d passed, %d failed\n", nPass, nFail);
  return nFail>0 ? 1 : 0;
}
