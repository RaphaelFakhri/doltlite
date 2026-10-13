#include <stdio.h>
#include <string.h>
#include "sqliteInt.h"
#include "prolly_hash.h"
#include "chunk_store.h"
#include "doltlite_commit.h"
#include "doltlite_remote.h"
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

static void mkhash(ProllyHash *h, u8 b){
  memset(h, 0, sizeof(*h));
  h->data[0] = b;
  h->data[1] = (u8)(b ^ 0x5a);
}

static int openStore(ChunkStore *cs, const char *path){
  char wal[512];
  remove(path);
  snprintf(wal, sizeof(wal), "%s-wal", path);
  remove(wal);
  memset(cs, 0, sizeof(*cs));
  return chunkStoreOpen(cs, sqlite3_vfs_find(0), path,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MAIN_DB);
}

/* chunkStoreAddBranch refuses a name it already holds, so a shadow entry can
** only arrive over the wire. Splice one into a serialized V7 blob: bump the
** branch count and append a second entry for zName at the end of the branch
** section. Layout is u8 version, u32 defLen + default, u32 count, then per
** branch u32 nameLen + name + commitHash + workingSetHash. */
static u8 *spliceDuplicateBranch(const u8 *pBlob, int nBlob, const char *zName,
                                 const ProllyHash *pCommit, int *pnOut){
  int nName = (int)strlen(zName);
  int nExtra = 4 + nName + PROLLY_HASH_SIZE*2;
  int defLen, nBranch, off, i;
  u8 *out;

  *pnOut = 0;
  if( nBlob < 5 || pBlob[0]!=7 ) return 0;
  defLen = (int)CS_READ_U32(pBlob + 1);
  off = 1 + 4 + defLen;
  if( off + 4 > nBlob ) return 0;
  nBranch = (int)CS_READ_U32(pBlob + off);
  off += 4;
  for(i=0; i<nBranch; i++){
    int nEntryName;
    if( off + 4 > nBlob ) return 0;
    nEntryName = (int)CS_READ_U32(pBlob + off);
    off += 4 + nEntryName + PROLLY_HASH_SIZE*2;
    if( off > nBlob ) return 0;
  }

  out = sqlite3_malloc(nBlob + nExtra);
  if( !out ) return 0;
  memcpy(out, pBlob, off);
  CS_WRITE_U32(out + 1 + 4 + defLen, (u32)(nBranch + 1));
  CS_WRITE_U32(out + off, (u32)nName);
  memcpy(out + off + 4, zName, nName);
  memcpy(out + off + 4 + nName, pCommit, PROLLY_HASH_SIZE);
  memset(out + off + 4 + nName + PROLLY_HASH_SIZE, 0, PROLLY_HASH_SIZE);
  memcpy(out + off + nExtra, pBlob + off, nBlob - off);
  *pnOut = nBlob + nExtra;
  return out;
}

/* Build a refs blob from a fresh store carrying main@a and foo@b, then hand
** back that same starting blob so scenarios can mutate copies of it. */
static u8 *seedRefs(ChunkStore *cs, const ProllyHash *a, const ProllyHash *b,
                    int *pn){
  u8 *blob = 0;
  if( chunkStoreSetDefaultBranch(cs, "main")!=SQLITE_OK ) return 0;
  if( chunkStoreAddBranch(cs, "main", a)!=SQLITE_OK ) return 0;
  if( chunkStoreAddBranch(cs, "foo", b)!=SQLITE_OK ) return 0;
  if( chunkStoreSerializeRefs(cs)!=SQLITE_OK ) return 0;
  if( chunkStoreCommit(cs)!=SQLITE_OK ) return 0;
  if( chunkStoreSerializeRefsToBlob(cs, &blob, pn)!=SQLITE_OK ) return 0;
  return blob;
}

static void test_file_pushed_sequences(void){
  const char *zPath = DOLTLITE_TEST_TMPDIR "/scoped_refs_file_sequences.db";
  sqlite3 *db = 0;
  DoltliteRemote *pRemote;
  ChunkStore next;
  u8 *pBlob = 0;
  int nBlob = 0;
  ProllyHash expected, head;
  int rc;
  remove(zPath);
  check("file sequences: open", sqlite3_open(zPath, &db)==SQLITE_OK);
  check("file sequences: seed", sqlite3_exec(db,
    "CREATE TABLE t(id INTEGER PRIMARY KEY AUTOINCREMENT,v);"
    "INSERT INTO t(v) VALUES('base'); SELECT dolt_commit('-Am','seed');",
    0, 0, 0)==SQLITE_OK);
  pRemote = doltliteFsRemoteOpen(sqlite3_vfs_find(0), zPath);
  check("file sequences: transport", pRemote!=0);
  memset(&next, 0, sizeof(next));
  if( pRemote ){
    check("file sequences: read refs", pRemote->xGetRefs(pRemote, &pBlob, &nBlob)==SQLITE_OK);
    prollyHashCompute(pBlob, nBlob, &expected);
    check("file sequences: load refs", chunkStoreLoadRefsFromBlob(&next, pBlob, nBlob)==SQLITE_OK);
    sqlite3_free(pBlob);
    pBlob = 0;
    check("file sequences: branch", chunkStoreFindBranch(&next, "main", &head)==SQLITE_OK);
    chunkStoreAddBranch(&next, "forged", &head);
    chunkStoreBumpSequence(&next, "t", LARGEST_INT64);
    chunkStoreBumpSequence(&next, "ghost", LARGEST_INT64);
    check("file sequences: serialize", chunkStoreSerializeRefsToBlob(&next, &pBlob, &nBlob)==SQLITE_OK);
    rc = pRemote->xSetRefsIf(pRemote, &expected, "forged", 0, pBlob, nBlob);
    check("file sequences: install", rc==SQLITE_OK);
    if( rc==SQLITE_OK ) check("file sequences: commit", pRemote->xCommit(pRemote)==SQLITE_OK);
    pRemote->xClose(pRemote);
  }
  check("file_pushed_sequences_do_not_exhaust_inserts", sqlite3_exec(db,
    "INSERT INTO t(v) VALUES('after');", 0, 0, 0)==SQLITE_OK);
  check("file sequences: actual counter", chunkStoreGetSequenceValue(doltliteGetChunkStore(db), "t")==2);
  check("file sequences: invented counter absent", chunkStoreGetSequenceValue(doltliteGetChunkStore(db), "ghost")==0);
  sqlite3_free(pBlob);
  chunkStoreClose(&next);
  sqlite3_close(db);
  remove(zPath);
}

int main(void){
  ChunkStore cs;
  ProllyHash Ha, Hb, Hc, Hd, found;
  u8 *curBlob = 0;
  int nCur = 0;
  int rc;
  const char *path = DOLTLITE_TEST_TMPDIR "/scoped_refs_push_test.db";

  sqlite3_initialize();
  test_file_pushed_sequences();
  printf("Scoped refs-push authorization tests\n");
  printf("====================================\n\n");

  mkhash(&Ha, 0xA1);
  mkhash(&Hb, 0xB2);
  mkhash(&Hc, 0xC3);
  mkhash(&Hd, 0xD4);

  check("open store", openStore(&cs, path)==SQLITE_OK);
  curBlob = seedRefs(&cs, &Ha, &Hb, &nCur);
  check("seed refs (main@a, foo@b)", curBlob!=0);
  if( !curBlob ){ printf("\n%d passed, %d failed\n", nPass, nFail); return 1; }

  {
    const char *azName[] = {"HEAD", "head", "WORKING", "STAGED", "a..b",
      "main~1", "x y", "x.lock", "/x", "x/", "a//b", "a/.b"};
    int k, force;
    for(k=0; k<(int)(sizeof(azName)/sizeof(azName[0])); k++){
      ChunkStore tmp;
      u8 *blob = 0;
      int n = 0;
      memset(&tmp, 0, sizeof(tmp));
      chunkStoreLoadRefsFromBlob(&tmp, curBlob, nCur);
      chunkStoreAddBranch(&tmp, azName[k], &Ha);
      chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
      for(force=0; force<=1; force++){
        check("reject_invalid_pushed_branch_name",
          doltliteValidateScopedRefsUpdate(&cs, blob, n, azName[k], force, 0)==SQLITE_CONSTRAINT);
      }
      sqlite3_free(blob);
      blob = 0;
      chunkStoreDeleteBranch(&tmp, azName[k]);
      chunkStoreAddTag(&tmp, azName[k], &Ha);
      chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
      for(force=0; force<=1; force++){
        char *zScope = sqlite3_mprintf("tag:%s", azName[k]);
        check("reject_invalid_pushed_tag_name",
          doltliteValidateScopedRefsUpdate(&cs, blob, n, zScope, force, 0)==SQLITE_CONSTRAINT);
        sqlite3_free(zScope);
      }
      sqlite3_free(blob);
      chunkStoreClose(&tmp);
    }
  }

  /* Attack: a push declaring branch foo carries a blob that also rewrites
  ** main. The scoped validator must reject it, with or without force. */
  {
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, curBlob, nCur);
    chunkStoreUpdateBranch(&tmp, "main", &Hc);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);

    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 0, 0);
    check("reject rewriting main while pushing foo", rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 1, 0);
    check("force does not authorize rewriting main", rc==SQLITE_CONSTRAINT);

    /* Pre-fix behavior: installing the blob wholesale (what the server used to
    ** do) silently hijacks main. This is the reported vulnerability. */
    {
      ChunkStore victim;
      check("open victim", openStore(&victim, DOLTLITE_TEST_TMPDIR "/scoped_refs_victim.db")==SQLITE_OK);
      chunkStoreAddBranch(&victim, "main", &Ha);
      chunkStoreAddBranch(&victim, "foo", &Hb);
      chunkStoreSerializeRefs(&victim);
      chunkStoreCommit(&victim);
      chunkStoreInstallRefsBlob(&victim, blob, n);
      chunkStoreFindBranch(&victim, "main", &found);
      check("pre-fix: raw install hijacks main (vuln demo)",
            memcmp(&found, &Hc, sizeof(found))==0);
      chunkStoreClose(&victim);
      remove(DOLTLITE_TEST_TMPDIR "/scoped_refs_victim.db");
    }
    sqlite3_free(blob);
  }

  /* Attack: delete another branch by omitting it from the blob. */
  {
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreSetDefaultBranch(&tmp, "foo");
    chunkStoreAddBranch(&tmp, "foo", &Hb);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 1, 0);
    check("reject deleting main via omission", rc==SQLITE_CONSTRAINT);
    sqlite3_free(blob);
  }

  /* Legitimate: move only foo. Non-fast-forward needs force (Hd has no commit
  ** chunk, so it is not an ancestor of Hb). */
  {
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, curBlob, nCur);
    chunkStoreUpdateBranch(&tmp, "foo", &Hd);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 0, 0);
    check("reject non-fast-forward foo without force", rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 1, 0);
    check("allow forced move of foo (scoped)", rc==SQLITE_OK);
    sqlite3_free(blob);
  }

  /* Legitimate: an unchanged blob is always in scope. */
  rc = doltliteValidateScopedRefsUpdate(&cs, curBlob, nCur, "foo", 0, 0);
  check("allow no-op refs update", rc==SQLITE_OK);

  /* Attack: delete the branch being pushed by omitting it. The fast-forward
  ** gate only fires when the declared branch appears on both sides, so an
  ** omission used to slip past it with no force. */
  {
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreSetDefaultBranch(&tmp, "main");
    chunkStoreAddBranch(&tmp, "main", &Ha);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 0, 0);
    check("reject deleting the declared branch by omission", rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 1, 0);
    check("force does not authorize deleting the declared branch",
          rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, ":foo", 0, 0);
    check("explicit delete authorizes only the named branch", rc==SQLITE_OK);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, ":main", 1, 0);
    check("explicit delete cannot omit another branch", rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&cs, curBlob, nCur, ":foo", 1, 0);
    check("delete scope cannot install a branch", rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, ":", 1, 0);
    check("delete scope requires a branch name", rc==SQLITE_MISUSE);
    sqlite3_free(blob);
  }

  /* Attack: a second entry for main. Every refs lookup resolves to the first
  ** matching slot, so the shadow entry is invisible to the scope check that
  ** compares main's commit, yet it rides along into the installed blob. */
  {
    u8 *blob = 0; int n = 0;
    blob = spliceDuplicateBranch(curBlob, nCur, "main", &Hc, &n);
    check("splice a shadow main entry", blob!=0);
    if( blob ){
      ChunkStore probe;
      ProllyHash seen;
      /* The shadow is real but invisible: the store still reports main@a. */
      memset(&probe, 0, sizeof(probe));
      chunkStoreLoadRefsFromBlob(&probe, blob, n);
      check("shadow entry is unreachable through the refs API",
            chunkStoreFindBranch(&probe, "main", &seen)==SQLITE_OK
            && memcmp(&seen, &Ha, sizeof(seen))==0);
      check("shadow entry still occupies a branch slot",
            refsTableBranchCount(&probe.refs)==3);
      chunkStoreClose(&probe);

      rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 0, 0);
      check("reject duplicate branch entry for main", rc==SQLITE_CONSTRAINT);
      rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 1, 0);
      check("force does not authorize a duplicate branch entry",
            rc==SQLITE_CONSTRAINT);
      sqlite3_free(blob);
    }
  }

  /* Attack: repoint the default branch. Every clone checks out whatever this
  ** names, so it steers readers without touching a single branch hash. */
  {
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, curBlob, nCur);
    chunkStoreSetDefaultBranch(&tmp, "foo");
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 0, 0);
    check("reject repointing the default branch", rc==SQLITE_CONSTRAINT);
    sqlite3_free(blob);
  }

  /* Attack: inject a remote URL that every later clone inherits. */
  {
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, curBlob, nCur);
    chunkStoreAddRemote(&tmp, "origin", "http://attacker.example/evil");
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 0, 0);
    check("reject injecting a remote", rc==SQLITE_CONSTRAINT);
    sqlite3_free(blob);
  }

  /* Attack: rewrite a tag's message while leaving its name and target alone. */
  {
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    ChunkStore tagged;
    u8 *tagBlob = 0; int nTag = 0;

    check("open tagged store",
          openStore(&tagged, DOLTLITE_TEST_TMPDIR "/scoped_refs_tagged.db")==SQLITE_OK);
    chunkStoreSetDefaultBranch(&tagged, "main");
    chunkStoreAddBranch(&tagged, "main", &Ha);
    chunkStoreAddBranch(&tagged, "foo", &Hb);
    chunkStoreAddTagFull(&tagged, "v1", &Ha, "t", "t@e", 1, "original");
    chunkStoreSerializeRefs(&tagged);
    chunkStoreCommit(&tagged);
    chunkStoreSerializeRefsToBlob(&tagged, &tagBlob, &nTag);

    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, tagBlob, nTag);
    chunkStoreDeleteTag(&tmp, "v1");
    chunkStoreAddTagFull(&tmp, "v1", &Ha, "t", "t@e", 1, "rewritten");
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);

    rc = doltliteValidateScopedRefsUpdate(&tagged, blob, n, "foo", 0, 0);
    check("reject rewriting a tag message", rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&tagged, blob, n, "tag:v1", 0, 0);
    check("allow rewriting only the declared tag", rc==SQLITE_OK);
    rc = doltliteValidateScopedRefsUpdate(&tagged, blob, n, "tag:v2", 1, 0);
    check("reject rewriting an undeclared tag", rc==SQLITE_CONSTRAINT);

    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, blob, n);
    sqlite3_free(blob);
    blob = 0;
    chunkStoreAddTagFull(&tmp, "v2", &Hb, "t", "t@e", 2, "other");
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&tagged, blob, n, "tag:v1", 1, 0);
    check("reject adding a second tag in the same update",
          rc==SQLITE_CONSTRAINT);

    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, tagBlob, nTag);
    chunkStoreDeleteTag(&tmp, "v1");
    chunkStoreAddTagFull(&tmp, "v1", &Ha, "t", "t@e", 1, "rewritten");
    chunkStoreUpdateBranch(&tmp, "foo", &Hc);
    sqlite3_free(blob);
    blob = 0;
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&tagged, blob, n, "tag:v1", 1, 0);
    check("tag update cannot rewrite a branch", rc==SQLITE_CONSTRAINT);

    /* And the same tag, untouched, stays in scope. */
    rc = doltliteValidateScopedRefsUpdate(&tagged, tagBlob, nTag, "foo", 0, 0);
    check("allow push that leaves the tag alone", rc==SQLITE_OK);

    sqlite3_free(blob);
    sqlite3_free(tagBlob);
    chunkStoreClose(&tagged);
    remove(DOLTLITE_TEST_TMPDIR "/scoped_refs_tagged.db");
  }

  /* Sequences are shared AUTOINCREMENT counters that a push legitimately
  ** bumps, so they may rise and gain entries but never regress or vanish. */
  {
    ChunkStore seqStore;
    u8 *seqBlob = 0; int nSeq = 0;

    check("open sequence store",
          openStore(&seqStore, DOLTLITE_TEST_TMPDIR "/scoped_refs_seq.db")==SQLITE_OK);
    chunkStoreSetDefaultBranch(&seqStore, "main");
    chunkStoreAddBranch(&seqStore, "main", &Ha);
    chunkStoreAddBranch(&seqStore, "foo", &Hb);
    chunkStoreBumpSequence(&seqStore, "t", 100);
    chunkStoreSerializeRefs(&seqStore);
    chunkStoreCommit(&seqStore);
    chunkStoreSerializeRefsToBlob(&seqStore, &seqBlob, &nSeq);

    {
      ChunkStore tmp; u8 *blob = 0; int n = 0;
      memset(&tmp, 0, sizeof(tmp));
      chunkStoreLoadRefsFromBlob(&tmp, seqBlob, nSeq);
      chunkStoreBumpSequence(&tmp, "t", 250);
      chunkStoreBumpSequence(&tmp, "other", 7);
      chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
      chunkStoreClose(&tmp);
      rc = doltliteValidateScopedRefsUpdate(&seqStore, blob, n, "foo", 0, 0);
      check("allow sequences to advance and appear", rc==SQLITE_OK);
      sqlite3_free(blob);
    }
    {
      ChunkStore tmp; u8 *blob = 0; int n = 0;
      memset(&tmp, 0, sizeof(tmp));
      chunkStoreSetDefaultBranch(&tmp, "main");
      chunkStoreAddBranch(&tmp, "main", &Ha);
      chunkStoreAddBranch(&tmp, "foo", &Hb);
      chunkStoreBumpSequence(&tmp, "t", 5);
      chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
      chunkStoreClose(&tmp);
      rc = doltliteValidateScopedRefsUpdate(&seqStore, blob, n, "foo", 0, 0);
      check("reject rewinding a sequence", rc==SQLITE_CONSTRAINT);
      sqlite3_free(blob);
    }
    {
      ChunkStore tmp; u8 *blob = 0; int n = 0;
      memset(&tmp, 0, sizeof(tmp));
      chunkStoreSetDefaultBranch(&tmp, "main");
      chunkStoreAddBranch(&tmp, "main", &Ha);
      chunkStoreAddBranch(&tmp, "foo", &Hb);
      chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
      chunkStoreClose(&tmp);
      rc = doltliteValidateScopedRefsUpdate(&seqStore, blob, n, "foo", 0, 0);
      check("reject dropping a sequence", rc==SQLITE_CONSTRAINT);
      sqlite3_free(blob);
    }

    sqlite3_free(seqBlob);
    chunkStoreClose(&seqStore);
    remove(DOLTLITE_TEST_TMPDIR "/scoped_refs_seq.db");
  }

  /* A fresh target legitimately adopts the pushed branch as its default, the
  ** way doltlitePush seeds an empty remote. */
  {
    ChunkStore fresh;
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    check("open fresh target",
          openStore(&fresh, DOLTLITE_TEST_TMPDIR "/scoped_refs_fresh.db")==SQLITE_OK);
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreSetDefaultBranch(&tmp, "foo");
    chunkStoreAddBranch(&tmp, "foo", &Hb);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&fresh, blob, n, "foo", 0, 0);
    check("allow fresh target to adopt the pushed branch as default",
          rc==SQLITE_OK);
    sqlite3_free(blob);
    chunkStoreClose(&fresh);
    remove(DOLTLITE_TEST_TMPDIR "/scoped_refs_fresh.db");
  }

  {
    ChunkStore working;
    ChunkStore tmp;
    DoltliteCommit commit;
    ProllyHash empty = {{0}};
    ProllyHash commitHash;
    ProllyHash wsHash;
    u8 *blob = 0; int n = 0;
    u8 *commitData = 0; int nCommit = 0;
    u8 wsData[WS_TOTAL_SIZE];
    check("open target with dirty working set",
          openStore(&working, DOLTLITE_TEST_TMPDIR "/scoped_refs_working.db")==SQLITE_OK);
    memset(&commit, 0, sizeof(commit));
    commit.catalogHash = Ha;
    commit.zName = "test";
    commit.zEmail = "test@example.com";
    commit.zMessage = "test";
    doltliteCommitSerialize(&commit, &commitData, &nCommit);
    chunkStorePut(&working, commitData, nCommit, &commitHash);
    sqlite3_free(commitData);
    memset(wsData, 0, sizeof(wsData));
    wsData[0] = WS_FORMAT_VERSION;
    memcpy(wsData + WS_WORKING_CAT_OFF, Ha.data, PROLLY_HASH_SIZE);
    memcpy(wsData + WS_WORKING_COMMIT_OFF, commitHash.data, PROLLY_HASH_SIZE);
    memcpy(wsData + WS_STAGED_OFF, Hb.data, PROLLY_HASH_SIZE);
    chunkStorePut(&working, wsData, sizeof(wsData), &wsHash);
    chunkStoreSetDefaultBranch(&working, "main");
    chunkStoreAddBranch(&working, "main", &Ha);
    chunkStoreAddBranch(&working, "foo", &commitHash);
    chunkStoreSetBranchWorkingSet(&working, "foo", &wsHash);
    chunkStoreSerializeRefs(&working);
    chunkStoreCommit(&working);

    memset(&tmp, 0, sizeof(tmp));
    chunkStoreSerializeRefsToBlob(&working, &blob, &n);
    chunkStoreLoadRefsFromBlob(&tmp, blob, n);
    sqlite3_free(blob);
    blob = 0;
    chunkStoreUpdateBranch(&tmp, "foo", &Hd);
    chunkStoreSetBranchWorkingSet(&tmp, "foo", &empty);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);

    rc = doltliteValidateScopedRefsUpdate(&working, blob, n, "foo", 0, 0);
    check("reject push over dirty target working set", rc==SQLITE_LOCKED);
    rc = doltliteValidateScopedRefsUpdate(&working, blob, n, "foo", 1, 0);
    check("force cannot overwrite dirty target working set", rc==SQLITE_LOCKED);
    sqlite3_free(blob);

    memset(wsData + WS_STAGED_OFF, 0, PROLLY_HASH_SIZE);
    chunkStorePut(&working, wsData, sizeof(wsData), &wsHash);
    chunkStoreSetBranchWorkingSet(&working, "foo", &wsHash);
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreSerializeRefsToBlob(&working, &blob, &n);
    rc = doltliteValidateScopedRefsUpdate(&working, blob, n, "foo", 0, 0);
    check("allow a push that leaves a clean working set unchanged",
          rc==SQLITE_OK);
    chunkStoreLoadRefsFromBlob(&tmp, blob, n);
    sqlite3_free(blob);
    blob = 0;
    chunkStoreUpdateBranch(&tmp, "foo", &Hd);
    chunkStoreSetBranchWorkingSet(&tmp, "foo", &empty);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&working, blob, n, "foo", 1, 0);
    check("allow push over clean target working set", rc==SQLITE_OK);
    sqlite3_free(blob);
    chunkStoreClose(&working);
    remove(DOLTLITE_TEST_TMPDIR "/scoped_refs_working.db");
  }

  /* Attack: an unforced push changes nothing but the declared branch's
  ** working set, pointing it at that branch's commit chunk. */
  {
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, curBlob, nCur);
    chunkStoreSetBranchWorkingSet(&tmp, "main", &Ha);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "main", 0, 0);
    check("reject working set retarget of main", rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "main", 1, 0);
    check("force does not authorize a working set retarget",
          rc==SQLITE_CONSTRAINT);
    sqlite3_free(blob);

    memset(&tmp, 0, sizeof(tmp));
    chunkStoreLoadRefsFromBlob(&tmp, curBlob, nCur);
    chunkStoreSetBranchWorkingSet(&tmp, "foo", &Hd);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&cs, blob, n, "foo", 0, 0);
    check("reject working set retarget of a non-default branch",
          rc==SQLITE_CONSTRAINT);
    sqlite3_free(blob);
  }

  {
    ChunkStore fresh;
    ChunkStore tmp;
    u8 *blob = 0; int n = 0;
    check("open fresh target for a working set",
          openStore(&fresh, DOLTLITE_TEST_TMPDIR "/scoped_refs_ws_fresh.db")==SQLITE_OK);
    memset(&tmp, 0, sizeof(tmp));
    chunkStoreSetDefaultBranch(&tmp, "foo");
    chunkStoreAddBranch(&tmp, "foo", &Hb);
    chunkStoreSetBranchWorkingSet(&tmp, "foo", &Hb);
    chunkStoreSerializeRefsToBlob(&tmp, &blob, &n);
    chunkStoreClose(&tmp);
    rc = doltliteValidateScopedRefsUpdate(&fresh, blob, n, "foo", 0, 0);
    check("reject creating a branch whose working set is not empty",
          rc==SQLITE_CONSTRAINT);
    rc = doltliteValidateScopedRefsUpdate(&fresh, blob, n, "foo", 1, 0);
    check("force does not authorize a new non-empty working set",
          rc==SQLITE_CONSTRAINT);
    sqlite3_free(blob);
    chunkStoreClose(&fresh);
    remove(DOLTLITE_TEST_TMPDIR "/scoped_refs_ws_fresh.db");
  }

  sqlite3_free(curBlob);
  chunkStoreClose(&cs);
  remove(path);

  printf("\n%d passed, %d failed\n", nPass, nFail);
  return nFail>0 ? 1 : 0;
}
