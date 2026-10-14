#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqliteInt.h"
#include "prolly_hash.h"
#include "prolly_xxhash.h"
#include "prolly_chunker.h"
#include "prolly_node.h"
#include "chunk_store.h"
#include "lib/test_tmpdir.h"

static int nPass = 0;
static int nFail = 0;

static void check(const char *name, int cond){
  if( cond ){
    nPass++;
  }else{
    nFail++;
    fprintf(stderr, "FAIL: %s\n", name);
  }
}

static void encodeIntKey(i64 v, u8 key[8]){
  int j;
  for(j=7; j>=0; j--){
    key[j] = (u8)(v & 0xff);
    v >>= 8;
  }
}

static int parseHexHash(const char *z, ProllyHash *pOut){
  int i;
  if( !z || (int)strlen(z)!=PROLLY_HASH_SIZE*2 ) return 0;
  for(i=0; i<PROLLY_HASH_SIZE; i++){
    unsigned int b = 0;
    if( sscanf(z + 2*i, "%2x", &b)!=1 ) return 0;
    pOut->data[i] = (u8)b;
  }
  return 1;
}

/* XXH32(seed=0), BE int keys 0..31. */
static const struct { int key; u32 h; } aXxh[] = {
  {0, 0xdeb39513u},
  {1, 0xf414a945u},
  {2, 0xd53c8ecbu},
  {3, 0xea0bee70u},
  {4, 0xf8cdd998u},
  {5, 0x17d5f46au},
  {6, 0xb2edcb43u},
  {7, 0x87a9925eu},
  {8, 0xa3301e1du},
  {9, 0x4c537cb4u},
  {10, 0x8b4174ddu},
  {11, 0x5ece03f1u},
  {12, 0xbb28819cu},
  {13, 0x52f7a644u},
  {14, 0x867ce427u},
  {15, 0xfd624c39u},
  {16, 0x15d91f09u},
  {17, 0x9efb9836u},
  {18, 0xd2b74cf3u},
  {19, 0x7d456185u},
  {20, 0xeaabb43du},
  {21, 0xa3f424a0u},
  {22, 0xff4b9b11u},
  {23, 0x0817d47au},
  {24, 0x10d28df6u},
  {25, 0x08ab1330u},
  {26, 0x14c039bdu},
  {27, 0x04d7fa0eu},
  {28, 0xe76e2dbdu},
  {29, 0x74e636d9u},
  {30, 0x3119a83eu},
  {31, 0xf5b8a0e0u},
};

/* Weibull grid, float bit-for-bit. */
static const struct {
  u32 size;
  u32 thisSize;
  u32 hash;
  int want;
} aWeibull[] = {
  {512u, 8u, 0x00000000u, 1},
  {512u, 8u, 0x00000001u, 1},
  {512u, 8u, 0x00000100u, 1},
  {512u, 8u, 0x7fffffffu, 0},
  {512u, 8u, 0x80000000u, 0},
  {512u, 8u, 0xffffffffu, 0},
  {512u, 8u, 0x12345678u, 0},
  {512u, 8u, 0xabcdef01u, 0},
  {512u, 8u, 0x55aa55aau, 0},
  {512u, 8u, 0x0f0f0f0fu, 0},
  {512u, 16u, 0x00000000u, 1},
  {512u, 16u, 0x00000001u, 1},
  {512u, 16u, 0x00000100u, 1},
  {512u, 16u, 0x7fffffffu, 0},
  {512u, 16u, 0x80000000u, 0},
  {512u, 16u, 0xffffffffu, 0},
  {512u, 16u, 0x12345678u, 0},
  {512u, 16u, 0xabcdef01u, 0},
  {512u, 16u, 0x55aa55aau, 0},
  {512u, 16u, 0x0f0f0f0fu, 0},
  {512u, 24u, 0x00000000u, 1},
  {512u, 24u, 0x00000001u, 1},
  {512u, 24u, 0x00000100u, 1},
  {512u, 24u, 0x7fffffffu, 0},
  {512u, 24u, 0x80000000u, 0},
  {512u, 24u, 0xffffffffu, 0},
  {512u, 24u, 0x12345678u, 0},
  {512u, 24u, 0xabcdef01u, 0},
  {512u, 24u, 0x55aa55aau, 0},
  {512u, 24u, 0x0f0f0f0fu, 0},
  {512u, 32u, 0x00000000u, 1},
  {512u, 32u, 0x00000001u, 1},
  {512u, 32u, 0x00000100u, 1},
  {512u, 32u, 0x7fffffffu, 0},
  {512u, 32u, 0x80000000u, 0},
  {512u, 32u, 0xffffffffu, 0},
  {512u, 32u, 0x12345678u, 0},
  {512u, 32u, 0xabcdef01u, 0},
  {512u, 32u, 0x55aa55aau, 0},
  {512u, 32u, 0x0f0f0f0fu, 0},
  {512u, 64u, 0x00000000u, 1},
  {512u, 64u, 0x00000001u, 1},
  {512u, 64u, 0x00000100u, 1},
  {512u, 64u, 0x7fffffffu, 0},
  {512u, 64u, 0x80000000u, 0},
  {512u, 64u, 0xffffffffu, 0},
  {512u, 64u, 0x12345678u, 0},
  {512u, 64u, 0xabcdef01u, 0},
  {512u, 64u, 0x55aa55aau, 0},
  {512u, 64u, 0x0f0f0f0fu, 0},
  {512u, 100u, 0x00000000u, 1},
  {512u, 100u, 0x00000001u, 1},
  {512u, 100u, 0x00000100u, 1},
  {512u, 100u, 0x7fffffffu, 0},
  {512u, 100u, 0x80000000u, 0},
  {512u, 100u, 0xffffffffu, 0},
  {512u, 100u, 0x12345678u, 0},
  {512u, 100u, 0xabcdef01u, 0},
  {512u, 100u, 0x55aa55aau, 0},
  {512u, 100u, 0x0f0f0f0fu, 0},
  {512u, 128u, 0x00000000u, 1},
  {512u, 128u, 0x00000001u, 1},
  {512u, 128u, 0x00000100u, 1},
  {512u, 128u, 0x7fffffffu, 0},
  {512u, 128u, 0x80000000u, 0},
  {512u, 128u, 0xffffffffu, 0},
  {512u, 128u, 0x12345678u, 0},
  {512u, 128u, 0xabcdef01u, 0},
  {512u, 128u, 0x55aa55aau, 0},
  {512u, 128u, 0x0f0f0f0fu, 0},
  {512u, 256u, 0x00000000u, 1},
  {512u, 256u, 0x00000001u, 1},
  {512u, 256u, 0x00000100u, 1},
  {512u, 256u, 0x7fffffffu, 0},
  {512u, 256u, 0x80000000u, 0},
  {512u, 256u, 0xffffffffu, 0},
  {512u, 256u, 0x12345678u, 0},
  {512u, 256u, 0xabcdef01u, 0},
  {512u, 256u, 0x55aa55aau, 0},
  {512u, 256u, 0x0f0f0f0fu, 0},
  {512u, 400u, 0x00000000u, 1},
  {512u, 400u, 0x00000001u, 1},
  {512u, 400u, 0x00000100u, 1},
  {512u, 400u, 0x7fffffffu, 0},
  {512u, 400u, 0x80000000u, 0},
  {512u, 400u, 0xffffffffu, 0},
  {512u, 400u, 0x12345678u, 0},
  {512u, 400u, 0xabcdef01u, 0},
  {512u, 400u, 0x55aa55aau, 0},
  {512u, 400u, 0x0f0f0f0fu, 0},
  {512u, 512u, 0x00000000u, 1},
  {512u, 512u, 0x00000001u, 1},
  {512u, 512u, 0x00000100u, 1},
  {512u, 512u, 0x7fffffffu, 0},
  {512u, 512u, 0x80000000u, 0},
  {512u, 512u, 0xffffffffu, 0},
  {512u, 512u, 0x12345678u, 0},
  {512u, 512u, 0xabcdef01u, 0},
  {512u, 512u, 0x55aa55aau, 0},
  {512u, 512u, 0x0f0f0f0fu, 0},
  {513u, 8u, 0x00000000u, 1},
  {513u, 8u, 0x00000001u, 1},
  {513u, 8u, 0x00000100u, 1},
  {513u, 8u, 0x7fffffffu, 0},
  {513u, 8u, 0x80000000u, 0},
  {513u, 8u, 0xffffffffu, 0},
  {513u, 8u, 0x12345678u, 0},
  {513u, 8u, 0xabcdef01u, 0},
  {513u, 8u, 0x55aa55aau, 0},
  {513u, 8u, 0x0f0f0f0fu, 0},
  {513u, 16u, 0x00000000u, 1},
  {513u, 16u, 0x00000001u, 1},
  {513u, 16u, 0x00000100u, 1},
  {513u, 16u, 0x7fffffffu, 0},
  {513u, 16u, 0x80000000u, 0},
  {513u, 16u, 0xffffffffu, 0},
  {513u, 16u, 0x12345678u, 0},
  {513u, 16u, 0xabcdef01u, 0},
  {513u, 16u, 0x55aa55aau, 0},
  {513u, 16u, 0x0f0f0f0fu, 0},
  {513u, 24u, 0x00000000u, 1},
  {513u, 24u, 0x00000001u, 1},
  {513u, 24u, 0x00000100u, 1},
  {513u, 24u, 0x7fffffffu, 0},
  {513u, 24u, 0x80000000u, 0},
  {513u, 24u, 0xffffffffu, 0},
  {513u, 24u, 0x12345678u, 0},
  {513u, 24u, 0xabcdef01u, 0},
  {513u, 24u, 0x55aa55aau, 0},
  {513u, 24u, 0x0f0f0f0fu, 0},
  {513u, 32u, 0x00000000u, 1},
  {513u, 32u, 0x00000001u, 1},
  {513u, 32u, 0x00000100u, 1},
  {513u, 32u, 0x7fffffffu, 0},
  {513u, 32u, 0x80000000u, 0},
  {513u, 32u, 0xffffffffu, 0},
  {513u, 32u, 0x12345678u, 0},
  {513u, 32u, 0xabcdef01u, 0},
  {513u, 32u, 0x55aa55aau, 0},
  {513u, 32u, 0x0f0f0f0fu, 0},
  {513u, 64u, 0x00000000u, 1},
  {513u, 64u, 0x00000001u, 1},
  {513u, 64u, 0x00000100u, 1},
  {513u, 64u, 0x7fffffffu, 0},
  {513u, 64u, 0x80000000u, 0},
  {513u, 64u, 0xffffffffu, 0},
  {513u, 64u, 0x12345678u, 0},
  {513u, 64u, 0xabcdef01u, 0},
  {513u, 64u, 0x55aa55aau, 0},
  {513u, 64u, 0x0f0f0f0fu, 0},
  {513u, 100u, 0x00000000u, 1},
  {513u, 100u, 0x00000001u, 1},
  {513u, 100u, 0x00000100u, 1},
  {513u, 100u, 0x7fffffffu, 0},
  {513u, 100u, 0x80000000u, 0},
  {513u, 100u, 0xffffffffu, 0},
  {513u, 100u, 0x12345678u, 0},
  {513u, 100u, 0xabcdef01u, 0},
  {513u, 100u, 0x55aa55aau, 0},
  {513u, 100u, 0x0f0f0f0fu, 0},
  {513u, 128u, 0x00000000u, 1},
  {513u, 128u, 0x00000001u, 1},
  {513u, 128u, 0x00000100u, 1},
  {513u, 128u, 0x7fffffffu, 0},
  {513u, 128u, 0x80000000u, 0},
  {513u, 128u, 0xffffffffu, 0},
  {513u, 128u, 0x12345678u, 0},
  {513u, 128u, 0xabcdef01u, 0},
  {513u, 128u, 0x55aa55aau, 0},
  {513u, 128u, 0x0f0f0f0fu, 0},
  {513u, 256u, 0x00000000u, 1},
  {513u, 256u, 0x00000001u, 1},
  {513u, 256u, 0x00000100u, 1},
  {513u, 256u, 0x7fffffffu, 0},
  {513u, 256u, 0x80000000u, 0},
  {513u, 256u, 0xffffffffu, 0},
  {513u, 256u, 0x12345678u, 0},
  {513u, 256u, 0xabcdef01u, 0},
  {513u, 256u, 0x55aa55aau, 0},
  {513u, 256u, 0x0f0f0f0fu, 0},
  {513u, 400u, 0x00000000u, 1},
  {513u, 400u, 0x00000001u, 1},
  {513u, 400u, 0x00000100u, 1},
  {513u, 400u, 0x7fffffffu, 0},
  {513u, 400u, 0x80000000u, 0},
  {513u, 400u, 0xffffffffu, 0},
  {513u, 400u, 0x12345678u, 0},
  {513u, 400u, 0xabcdef01u, 0},
  {513u, 400u, 0x55aa55aau, 0},
  {513u, 400u, 0x0f0f0f0fu, 0},
  {513u, 512u, 0x00000000u, 1},
  {513u, 512u, 0x00000001u, 1},
  {513u, 512u, 0x00000100u, 1},
  {513u, 512u, 0x7fffffffu, 0},
  {513u, 512u, 0x80000000u, 0},
  {513u, 512u, 0xffffffffu, 0},
  {513u, 512u, 0x12345678u, 0},
  {513u, 512u, 0xabcdef01u, 0},
  {513u, 512u, 0x55aa55aau, 0},
  {513u, 512u, 0x0f0f0f0fu, 0},
  {600u, 8u, 0x00000000u, 1},
  {600u, 8u, 0x00000001u, 1},
  {600u, 8u, 0x00000100u, 1},
  {600u, 8u, 0x7fffffffu, 0},
  {600u, 8u, 0x80000000u, 0},
  {600u, 8u, 0xffffffffu, 0},
  {600u, 8u, 0x12345678u, 0},
  {600u, 8u, 0xabcdef01u, 0},
  {600u, 8u, 0x55aa55aau, 0},
  {600u, 8u, 0x0f0f0f0fu, 0},
  {600u, 16u, 0x00000000u, 1},
  {600u, 16u, 0x00000001u, 1},
  {600u, 16u, 0x00000100u, 1},
  {600u, 16u, 0x7fffffffu, 0},
  {600u, 16u, 0x80000000u, 0},
  {600u, 16u, 0xffffffffu, 0},
  {600u, 16u, 0x12345678u, 0},
  {600u, 16u, 0xabcdef01u, 0},
  {600u, 16u, 0x55aa55aau, 0},
  {600u, 16u, 0x0f0f0f0fu, 0},
  {600u, 24u, 0x00000000u, 1},
  {600u, 24u, 0x00000001u, 1},
  {600u, 24u, 0x00000100u, 1},
  {600u, 24u, 0x7fffffffu, 0},
  {600u, 24u, 0x80000000u, 0},
  {600u, 24u, 0xffffffffu, 0},
  {600u, 24u, 0x12345678u, 0},
  {600u, 24u, 0xabcdef01u, 0},
  {600u, 24u, 0x55aa55aau, 0},
  {600u, 24u, 0x0f0f0f0fu, 0},
  {600u, 32u, 0x00000000u, 1},
  {600u, 32u, 0x00000001u, 1},
  {600u, 32u, 0x00000100u, 1},
  {600u, 32u, 0x7fffffffu, 0},
  {600u, 32u, 0x80000000u, 0},
  {600u, 32u, 0xffffffffu, 0},
  {600u, 32u, 0x12345678u, 0},
  {600u, 32u, 0xabcdef01u, 0},
  {600u, 32u, 0x55aa55aau, 0},
  {600u, 32u, 0x0f0f0f0fu, 0},
  {600u, 64u, 0x00000000u, 1},
  {600u, 64u, 0x00000001u, 1},
  {600u, 64u, 0x00000100u, 1},
  {600u, 64u, 0x7fffffffu, 0},
  {600u, 64u, 0x80000000u, 0},
  {600u, 64u, 0xffffffffu, 0},
  {600u, 64u, 0x12345678u, 0},
  {600u, 64u, 0xabcdef01u, 0},
  {600u, 64u, 0x55aa55aau, 0},
  {600u, 64u, 0x0f0f0f0fu, 0},
  {600u, 100u, 0x00000000u, 1},
  {600u, 100u, 0x00000001u, 1},
  {600u, 100u, 0x00000100u, 1},
  {600u, 100u, 0x7fffffffu, 0},
  {600u, 100u, 0x80000000u, 0},
  {600u, 100u, 0xffffffffu, 0},
  {600u, 100u, 0x12345678u, 0},
  {600u, 100u, 0xabcdef01u, 0},
  {600u, 100u, 0x55aa55aau, 0},
  {600u, 100u, 0x0f0f0f0fu, 0},
  {600u, 128u, 0x00000000u, 1},
  {600u, 128u, 0x00000001u, 1},
  {600u, 128u, 0x00000100u, 1},
  {600u, 128u, 0x7fffffffu, 0},
  {600u, 128u, 0x80000000u, 0},
  {600u, 128u, 0xffffffffu, 0},
  {600u, 128u, 0x12345678u, 0},
  {600u, 128u, 0xabcdef01u, 0},
  {600u, 128u, 0x55aa55aau, 0},
  {600u, 128u, 0x0f0f0f0fu, 0},
  {600u, 256u, 0x00000000u, 1},
  {600u, 256u, 0x00000001u, 1},
  {600u, 256u, 0x00000100u, 1},
  {600u, 256u, 0x7fffffffu, 0},
  {600u, 256u, 0x80000000u, 0},
  {600u, 256u, 0xffffffffu, 0},
  {600u, 256u, 0x12345678u, 0},
  {600u, 256u, 0xabcdef01u, 0},
  {600u, 256u, 0x55aa55aau, 0},
  {600u, 256u, 0x0f0f0f0fu, 0},
  {600u, 400u, 0x00000000u, 1},
  {600u, 400u, 0x00000001u, 1},
  {600u, 400u, 0x00000100u, 1},
  {600u, 400u, 0x7fffffffu, 0},
  {600u, 400u, 0x80000000u, 0},
  {600u, 400u, 0xffffffffu, 0},
  {600u, 400u, 0x12345678u, 0},
  {600u, 400u, 0xabcdef01u, 0},
  {600u, 400u, 0x55aa55aau, 0},
  {600u, 400u, 0x0f0f0f0fu, 0},
  {600u, 512u, 0x00000000u, 1},
  {600u, 512u, 0x00000001u, 1},
  {600u, 512u, 0x00000100u, 1},
  {600u, 512u, 0x7fffffffu, 0},
  {600u, 512u, 0x80000000u, 0},
  {600u, 512u, 0xffffffffu, 0},
  {600u, 512u, 0x12345678u, 0},
  {600u, 512u, 0xabcdef01u, 0},
  {600u, 512u, 0x55aa55aau, 0},
  {600u, 512u, 0x0f0f0f0fu, 0},
  {1024u, 8u, 0x00000000u, 1},
  {1024u, 8u, 0x00000001u, 1},
  {1024u, 8u, 0x00000100u, 1},
  {1024u, 8u, 0x7fffffffu, 0},
  {1024u, 8u, 0x80000000u, 0},
  {1024u, 8u, 0xffffffffu, 0},
  {1024u, 8u, 0x12345678u, 0},
  {1024u, 8u, 0xabcdef01u, 0},
  {1024u, 8u, 0x55aa55aau, 0},
  {1024u, 8u, 0x0f0f0f0fu, 0},
  {1024u, 16u, 0x00000000u, 1},
  {1024u, 16u, 0x00000001u, 1},
  {1024u, 16u, 0x00000100u, 1},
  {1024u, 16u, 0x7fffffffu, 0},
  {1024u, 16u, 0x80000000u, 0},
  {1024u, 16u, 0xffffffffu, 0},
  {1024u, 16u, 0x12345678u, 0},
  {1024u, 16u, 0xabcdef01u, 0},
  {1024u, 16u, 0x55aa55aau, 0},
  {1024u, 16u, 0x0f0f0f0fu, 0},
  {1024u, 24u, 0x00000000u, 1},
  {1024u, 24u, 0x00000001u, 1},
  {1024u, 24u, 0x00000100u, 1},
  {1024u, 24u, 0x7fffffffu, 0},
  {1024u, 24u, 0x80000000u, 0},
  {1024u, 24u, 0xffffffffu, 0},
  {1024u, 24u, 0x12345678u, 0},
  {1024u, 24u, 0xabcdef01u, 0},
  {1024u, 24u, 0x55aa55aau, 0},
  {1024u, 24u, 0x0f0f0f0fu, 0},
  {1024u, 32u, 0x00000000u, 1},
  {1024u, 32u, 0x00000001u, 1},
  {1024u, 32u, 0x00000100u, 1},
  {1024u, 32u, 0x7fffffffu, 0},
  {1024u, 32u, 0x80000000u, 0},
  {1024u, 32u, 0xffffffffu, 0},
  {1024u, 32u, 0x12345678u, 0},
  {1024u, 32u, 0xabcdef01u, 0},
  {1024u, 32u, 0x55aa55aau, 0},
  {1024u, 32u, 0x0f0f0f0fu, 0},
  {1024u, 64u, 0x00000000u, 1},
  {1024u, 64u, 0x00000001u, 1},
  {1024u, 64u, 0x00000100u, 1},
  {1024u, 64u, 0x7fffffffu, 0},
  {1024u, 64u, 0x80000000u, 0},
  {1024u, 64u, 0xffffffffu, 0},
  {1024u, 64u, 0x12345678u, 0},
  {1024u, 64u, 0xabcdef01u, 0},
  {1024u, 64u, 0x55aa55aau, 0},
  {1024u, 64u, 0x0f0f0f0fu, 0},
  {1024u, 100u, 0x00000000u, 1},
  {1024u, 100u, 0x00000001u, 1},
  {1024u, 100u, 0x00000100u, 1},
  {1024u, 100u, 0x7fffffffu, 0},
  {1024u, 100u, 0x80000000u, 0},
  {1024u, 100u, 0xffffffffu, 0},
  {1024u, 100u, 0x12345678u, 0},
  {1024u, 100u, 0xabcdef01u, 0},
  {1024u, 100u, 0x55aa55aau, 0},
  {1024u, 100u, 0x0f0f0f0fu, 0},
  {1024u, 128u, 0x00000000u, 1},
  {1024u, 128u, 0x00000001u, 1},
  {1024u, 128u, 0x00000100u, 1},
  {1024u, 128u, 0x7fffffffu, 0},
  {1024u, 128u, 0x80000000u, 0},
  {1024u, 128u, 0xffffffffu, 0},
  {1024u, 128u, 0x12345678u, 0},
  {1024u, 128u, 0xabcdef01u, 0},
  {1024u, 128u, 0x55aa55aau, 0},
  {1024u, 128u, 0x0f0f0f0fu, 0},
  {1024u, 256u, 0x00000000u, 1},
  {1024u, 256u, 0x00000001u, 1},
  {1024u, 256u, 0x00000100u, 1},
  {1024u, 256u, 0x7fffffffu, 0},
  {1024u, 256u, 0x80000000u, 0},
  {1024u, 256u, 0xffffffffu, 0},
  {1024u, 256u, 0x12345678u, 0},
  {1024u, 256u, 0xabcdef01u, 0},
  {1024u, 256u, 0x55aa55aau, 0},
  {1024u, 256u, 0x0f0f0f0fu, 0},
  {1024u, 400u, 0x00000000u, 1},
  {1024u, 400u, 0x00000001u, 1},
  {1024u, 400u, 0x00000100u, 1},
  {1024u, 400u, 0x7fffffffu, 0},
  {1024u, 400u, 0x80000000u, 0},
  {1024u, 400u, 0xffffffffu, 0},
  {1024u, 400u, 0x12345678u, 0},
  {1024u, 400u, 0xabcdef01u, 0},
  {1024u, 400u, 0x55aa55aau, 0},
  {1024u, 400u, 0x0f0f0f0fu, 0},
  {1024u, 512u, 0x00000000u, 1},
  {1024u, 512u, 0x00000001u, 1},
  {1024u, 512u, 0x00000100u, 1},
  {1024u, 512u, 0x7fffffffu, 0},
  {1024u, 512u, 0x80000000u, 0},
  {1024u, 512u, 0xffffffffu, 0},
  {1024u, 512u, 0x12345678u, 0},
  {1024u, 512u, 0xabcdef01u, 0},
  {1024u, 512u, 0x55aa55aau, 0},
  {1024u, 512u, 0x0f0f0f0fu, 0},
  {2048u, 8u, 0x00000000u, 1},
  {2048u, 8u, 0x00000001u, 1},
  {2048u, 8u, 0x00000100u, 1},
  {2048u, 8u, 0x7fffffffu, 0},
  {2048u, 8u, 0x80000000u, 0},
  {2048u, 8u, 0xffffffffu, 0},
  {2048u, 8u, 0x12345678u, 0},
  {2048u, 8u, 0xabcdef01u, 0},
  {2048u, 8u, 0x55aa55aau, 0},
  {2048u, 8u, 0x0f0f0f0fu, 0},
  {2048u, 16u, 0x00000000u, 1},
  {2048u, 16u, 0x00000001u, 1},
  {2048u, 16u, 0x00000100u, 1},
  {2048u, 16u, 0x7fffffffu, 0},
  {2048u, 16u, 0x80000000u, 0},
  {2048u, 16u, 0xffffffffu, 0},
  {2048u, 16u, 0x12345678u, 0},
  {2048u, 16u, 0xabcdef01u, 0},
  {2048u, 16u, 0x55aa55aau, 0},
  {2048u, 16u, 0x0f0f0f0fu, 0},
  {2048u, 24u, 0x00000000u, 1},
  {2048u, 24u, 0x00000001u, 1},
  {2048u, 24u, 0x00000100u, 1},
  {2048u, 24u, 0x7fffffffu, 0},
  {2048u, 24u, 0x80000000u, 0},
  {2048u, 24u, 0xffffffffu, 0},
  {2048u, 24u, 0x12345678u, 0},
  {2048u, 24u, 0xabcdef01u, 0},
  {2048u, 24u, 0x55aa55aau, 0},
  {2048u, 24u, 0x0f0f0f0fu, 0},
  {2048u, 32u, 0x00000000u, 1},
  {2048u, 32u, 0x00000001u, 1},
  {2048u, 32u, 0x00000100u, 1},
  {2048u, 32u, 0x7fffffffu, 0},
  {2048u, 32u, 0x80000000u, 0},
  {2048u, 32u, 0xffffffffu, 0},
  {2048u, 32u, 0x12345678u, 0},
  {2048u, 32u, 0xabcdef01u, 0},
  {2048u, 32u, 0x55aa55aau, 0},
  {2048u, 32u, 0x0f0f0f0fu, 0},
  {2048u, 64u, 0x00000000u, 1},
  {2048u, 64u, 0x00000001u, 1},
  {2048u, 64u, 0x00000100u, 1},
  {2048u, 64u, 0x7fffffffu, 0},
  {2048u, 64u, 0x80000000u, 0},
  {2048u, 64u, 0xffffffffu, 0},
  {2048u, 64u, 0x12345678u, 0},
  {2048u, 64u, 0xabcdef01u, 0},
  {2048u, 64u, 0x55aa55aau, 0},
  {2048u, 64u, 0x0f0f0f0fu, 0},
  {2048u, 100u, 0x00000000u, 1},
  {2048u, 100u, 0x00000001u, 1},
  {2048u, 100u, 0x00000100u, 1},
  {2048u, 100u, 0x7fffffffu, 0},
  {2048u, 100u, 0x80000000u, 0},
  {2048u, 100u, 0xffffffffu, 0},
  {2048u, 100u, 0x12345678u, 0},
  {2048u, 100u, 0xabcdef01u, 0},
  {2048u, 100u, 0x55aa55aau, 0},
  {2048u, 100u, 0x0f0f0f0fu, 0},
  {2048u, 128u, 0x00000000u, 1},
  {2048u, 128u, 0x00000001u, 1},
  {2048u, 128u, 0x00000100u, 1},
  {2048u, 128u, 0x7fffffffu, 0},
  {2048u, 128u, 0x80000000u, 0},
  {2048u, 128u, 0xffffffffu, 0},
  {2048u, 128u, 0x12345678u, 0},
  {2048u, 128u, 0xabcdef01u, 0},
  {2048u, 128u, 0x55aa55aau, 0},
  {2048u, 128u, 0x0f0f0f0fu, 0},
  {2048u, 256u, 0x00000000u, 1},
  {2048u, 256u, 0x00000001u, 1},
  {2048u, 256u, 0x00000100u, 1},
  {2048u, 256u, 0x7fffffffu, 0},
  {2048u, 256u, 0x80000000u, 0},
  {2048u, 256u, 0xffffffffu, 0},
  {2048u, 256u, 0x12345678u, 0},
  {2048u, 256u, 0xabcdef01u, 0},
  {2048u, 256u, 0x55aa55aau, 0},
  {2048u, 256u, 0x0f0f0f0fu, 0},
  {2048u, 400u, 0x00000000u, 1},
  {2048u, 400u, 0x00000001u, 1},
  {2048u, 400u, 0x00000100u, 1},
  {2048u, 400u, 0x7fffffffu, 0},
  {2048u, 400u, 0x80000000u, 0},
  {2048u, 400u, 0xffffffffu, 0},
  {2048u, 400u, 0x12345678u, 0},
  {2048u, 400u, 0xabcdef01u, 0},
  {2048u, 400u, 0x55aa55aau, 0},
  {2048u, 400u, 0x0f0f0f0fu, 0},
  {2048u, 512u, 0x00000000u, 1},
  {2048u, 512u, 0x00000001u, 1},
  {2048u, 512u, 0x00000100u, 1},
  {2048u, 512u, 0x7fffffffu, 0},
  {2048u, 512u, 0x80000000u, 0},
  {2048u, 512u, 0xffffffffu, 0},
  {2048u, 512u, 0x12345678u, 0},
  {2048u, 512u, 0xabcdef01u, 0},
  {2048u, 512u, 0x55aa55aau, 0},
  {2048u, 512u, 0x0f0f0f0fu, 0},
  {4096u, 8u, 0x00000000u, 1},
  {4096u, 8u, 0x00000001u, 1},
  {4096u, 8u, 0x00000100u, 1},
  {4096u, 8u, 0x7fffffffu, 0},
  {4096u, 8u, 0x80000000u, 0},
  {4096u, 8u, 0xffffffffu, 0},
  {4096u, 8u, 0x12345678u, 0},
  {4096u, 8u, 0xabcdef01u, 0},
  {4096u, 8u, 0x55aa55aau, 0},
  {4096u, 8u, 0x0f0f0f0fu, 0},
  {4096u, 16u, 0x00000000u, 1},
  {4096u, 16u, 0x00000001u, 1},
  {4096u, 16u, 0x00000100u, 1},
  {4096u, 16u, 0x7fffffffu, 0},
  {4096u, 16u, 0x80000000u, 0},
  {4096u, 16u, 0xffffffffu, 0},
  {4096u, 16u, 0x12345678u, 0},
  {4096u, 16u, 0xabcdef01u, 0},
  {4096u, 16u, 0x55aa55aau, 0},
  {4096u, 16u, 0x0f0f0f0fu, 0},
  {4096u, 24u, 0x00000000u, 1},
  {4096u, 24u, 0x00000001u, 1},
  {4096u, 24u, 0x00000100u, 1},
  {4096u, 24u, 0x7fffffffu, 0},
  {4096u, 24u, 0x80000000u, 0},
  {4096u, 24u, 0xffffffffu, 0},
  {4096u, 24u, 0x12345678u, 0},
  {4096u, 24u, 0xabcdef01u, 0},
  {4096u, 24u, 0x55aa55aau, 0},
  {4096u, 24u, 0x0f0f0f0fu, 0},
  {4096u, 32u, 0x00000000u, 1},
  {4096u, 32u, 0x00000001u, 1},
  {4096u, 32u, 0x00000100u, 1},
  {4096u, 32u, 0x7fffffffu, 0},
  {4096u, 32u, 0x80000000u, 0},
  {4096u, 32u, 0xffffffffu, 0},
  {4096u, 32u, 0x12345678u, 0},
  {4096u, 32u, 0xabcdef01u, 0},
  {4096u, 32u, 0x55aa55aau, 0},
  {4096u, 32u, 0x0f0f0f0fu, 0},
  {4096u, 64u, 0x00000000u, 1},
  {4096u, 64u, 0x00000001u, 1},
  {4096u, 64u, 0x00000100u, 1},
  {4096u, 64u, 0x7fffffffu, 0},
  {4096u, 64u, 0x80000000u, 0},
  {4096u, 64u, 0xffffffffu, 0},
  {4096u, 64u, 0x12345678u, 0},
  {4096u, 64u, 0xabcdef01u, 0},
  {4096u, 64u, 0x55aa55aau, 0},
  {4096u, 64u, 0x0f0f0f0fu, 1},
  {4096u, 100u, 0x00000000u, 1},
  {4096u, 100u, 0x00000001u, 1},
  {4096u, 100u, 0x00000100u, 1},
  {4096u, 100u, 0x7fffffffu, 0},
  {4096u, 100u, 0x80000000u, 0},
  {4096u, 100u, 0xffffffffu, 0},
  {4096u, 100u, 0x12345678u, 1},
  {4096u, 100u, 0xabcdef01u, 0},
  {4096u, 100u, 0x55aa55aau, 0},
  {4096u, 100u, 0x0f0f0f0fu, 1},
  {4096u, 128u, 0x00000000u, 1},
  {4096u, 128u, 0x00000001u, 1},
  {4096u, 128u, 0x00000100u, 1},
  {4096u, 128u, 0x7fffffffu, 0},
  {4096u, 128u, 0x80000000u, 0},
  {4096u, 128u, 0xffffffffu, 0},
  {4096u, 128u, 0x12345678u, 1},
  {4096u, 128u, 0xabcdef01u, 0},
  {4096u, 128u, 0x55aa55aau, 0},
  {4096u, 128u, 0x0f0f0f0fu, 1},
  {4096u, 256u, 0x00000000u, 1},
  {4096u, 256u, 0x00000001u, 1},
  {4096u, 256u, 0x00000100u, 1},
  {4096u, 256u, 0x7fffffffu, 0},
  {4096u, 256u, 0x80000000u, 0},
  {4096u, 256u, 0xffffffffu, 0},
  {4096u, 256u, 0x12345678u, 1},
  {4096u, 256u, 0xabcdef01u, 0},
  {4096u, 256u, 0x55aa55aau, 0},
  {4096u, 256u, 0x0f0f0f0fu, 1},
  {4096u, 400u, 0x00000000u, 1},
  {4096u, 400u, 0x00000001u, 1},
  {4096u, 400u, 0x00000100u, 1},
  {4096u, 400u, 0x7fffffffu, 0},
  {4096u, 400u, 0x80000000u, 0},
  {4096u, 400u, 0xffffffffu, 0},
  {4096u, 400u, 0x12345678u, 1},
  {4096u, 400u, 0xabcdef01u, 0},
  {4096u, 400u, 0x55aa55aau, 0},
  {4096u, 400u, 0x0f0f0f0fu, 1},
  {4096u, 512u, 0x00000000u, 1},
  {4096u, 512u, 0x00000001u, 1},
  {4096u, 512u, 0x00000100u, 1},
  {4096u, 512u, 0x7fffffffu, 0},
  {4096u, 512u, 0x80000000u, 0},
  {4096u, 512u, 0xffffffffu, 0},
  {4096u, 512u, 0x12345678u, 1},
  {4096u, 512u, 0xabcdef01u, 0},
  {4096u, 512u, 0x55aa55aau, 1},
  {4096u, 512u, 0x0f0f0f0fu, 1},
  {8192u, 8u, 0x00000000u, 1},
  {8192u, 8u, 0x00000001u, 1},
  {8192u, 8u, 0x00000100u, 1},
  {8192u, 8u, 0x7fffffffu, 0},
  {8192u, 8u, 0x80000000u, 0},
  {8192u, 8u, 0xffffffffu, 0},
  {8192u, 8u, 0x12345678u, 0},
  {8192u, 8u, 0xabcdef01u, 0},
  {8192u, 8u, 0x55aa55aau, 0},
  {8192u, 8u, 0x0f0f0f0fu, 1},
  {8192u, 16u, 0x00000000u, 1},
  {8192u, 16u, 0x00000001u, 1},
  {8192u, 16u, 0x00000100u, 1},
  {8192u, 16u, 0x7fffffffu, 0},
  {8192u, 16u, 0x80000000u, 0},
  {8192u, 16u, 0xffffffffu, 0},
  {8192u, 16u, 0x12345678u, 1},
  {8192u, 16u, 0xabcdef01u, 0},
  {8192u, 16u, 0x55aa55aau, 0},
  {8192u, 16u, 0x0f0f0f0fu, 1},
  {8192u, 24u, 0x00000000u, 1},
  {8192u, 24u, 0x00000001u, 1},
  {8192u, 24u, 0x00000100u, 1},
  {8192u, 24u, 0x7fffffffu, 0},
  {8192u, 24u, 0x80000000u, 0},
  {8192u, 24u, 0xffffffffu, 0},
  {8192u, 24u, 0x12345678u, 1},
  {8192u, 24u, 0xabcdef01u, 0},
  {8192u, 24u, 0x55aa55aau, 0},
  {8192u, 24u, 0x0f0f0f0fu, 1},
  {8192u, 32u, 0x00000000u, 1},
  {8192u, 32u, 0x00000001u, 1},
  {8192u, 32u, 0x00000100u, 1},
  {8192u, 32u, 0x7fffffffu, 0},
  {8192u, 32u, 0x80000000u, 0},
  {8192u, 32u, 0xffffffffu, 0},
  {8192u, 32u, 0x12345678u, 1},
  {8192u, 32u, 0xabcdef01u, 0},
  {8192u, 32u, 0x55aa55aau, 0},
  {8192u, 32u, 0x0f0f0f0fu, 1},
  {8192u, 64u, 0x00000000u, 1},
  {8192u, 64u, 0x00000001u, 1},
  {8192u, 64u, 0x00000100u, 1},
  {8192u, 64u, 0x7fffffffu, 0},
  {8192u, 64u, 0x80000000u, 0},
  {8192u, 64u, 0xffffffffu, 0},
  {8192u, 64u, 0x12345678u, 1},
  {8192u, 64u, 0xabcdef01u, 0},
  {8192u, 64u, 0x55aa55aau, 1},
  {8192u, 64u, 0x0f0f0f0fu, 1},
  {8192u, 100u, 0x00000000u, 1},
  {8192u, 100u, 0x00000001u, 1},
  {8192u, 100u, 0x00000100u, 1},
  {8192u, 100u, 0x7fffffffu, 1},
  {8192u, 100u, 0x80000000u, 1},
  {8192u, 100u, 0xffffffffu, 0},
  {8192u, 100u, 0x12345678u, 1},
  {8192u, 100u, 0xabcdef01u, 0},
  {8192u, 100u, 0x55aa55aau, 1},
  {8192u, 100u, 0x0f0f0f0fu, 1},
  {8192u, 128u, 0x00000000u, 1},
  {8192u, 128u, 0x00000001u, 1},
  {8192u, 128u, 0x00000100u, 1},
  {8192u, 128u, 0x7fffffffu, 1},
  {8192u, 128u, 0x80000000u, 1},
  {8192u, 128u, 0xffffffffu, 0},
  {8192u, 128u, 0x12345678u, 1},
  {8192u, 128u, 0xabcdef01u, 0},
  {8192u, 128u, 0x55aa55aau, 1},
  {8192u, 128u, 0x0f0f0f0fu, 1},
  {8192u, 256u, 0x00000000u, 1},
  {8192u, 256u, 0x00000001u, 1},
  {8192u, 256u, 0x00000100u, 1},
  {8192u, 256u, 0x7fffffffu, 1},
  {8192u, 256u, 0x80000000u, 1},
  {8192u, 256u, 0xffffffffu, 0},
  {8192u, 256u, 0x12345678u, 1},
  {8192u, 256u, 0xabcdef01u, 1},
  {8192u, 256u, 0x55aa55aau, 1},
  {8192u, 256u, 0x0f0f0f0fu, 1},
  {8192u, 400u, 0x00000000u, 1},
  {8192u, 400u, 0x00000001u, 1},
  {8192u, 400u, 0x00000100u, 1},
  {8192u, 400u, 0x7fffffffu, 1},
  {8192u, 400u, 0x80000000u, 1},
  {8192u, 400u, 0xffffffffu, 0},
  {8192u, 400u, 0x12345678u, 1},
  {8192u, 400u, 0xabcdef01u, 1},
  {8192u, 400u, 0x55aa55aau, 1},
  {8192u, 400u, 0x0f0f0f0fu, 1},
  {8192u, 512u, 0x00000000u, 1},
  {8192u, 512u, 0x00000001u, 1},
  {8192u, 512u, 0x00000100u, 1},
  {8192u, 512u, 0x7fffffffu, 1},
  {8192u, 512u, 0x80000000u, 1},
  {8192u, 512u, 0xffffffffu, 0},
  {8192u, 512u, 0x12345678u, 1},
  {8192u, 512u, 0xabcdef01u, 1},
  {8192u, 512u, 0x55aa55aau, 1},
  {8192u, 512u, 0x0f0f0f0fu, 1},
  {12000u, 8u, 0x00000000u, 1},
  {12000u, 8u, 0x00000001u, 1},
  {12000u, 8u, 0x00000100u, 1},
  {12000u, 8u, 0x7fffffffu, 1},
  {12000u, 8u, 0x80000000u, 1},
  {12000u, 8u, 0xffffffffu, 1},
  {12000u, 8u, 0x12345678u, 1},
  {12000u, 8u, 0xabcdef01u, 1},
  {12000u, 8u, 0x55aa55aau, 1},
  {12000u, 8u, 0x0f0f0f0fu, 1},
  {12000u, 16u, 0x00000000u, 1},
  {12000u, 16u, 0x00000001u, 1},
  {12000u, 16u, 0x00000100u, 1},
  {12000u, 16u, 0x7fffffffu, 1},
  {12000u, 16u, 0x80000000u, 1},
  {12000u, 16u, 0xffffffffu, 1},
  {12000u, 16u, 0x12345678u, 1},
  {12000u, 16u, 0xabcdef01u, 1},
  {12000u, 16u, 0x55aa55aau, 1},
  {12000u, 16u, 0x0f0f0f0fu, 1},
  {12000u, 24u, 0x00000000u, 1},
  {12000u, 24u, 0x00000001u, 1},
  {12000u, 24u, 0x00000100u, 1},
  {12000u, 24u, 0x7fffffffu, 1},
  {12000u, 24u, 0x80000000u, 1},
  {12000u, 24u, 0xffffffffu, 1},
  {12000u, 24u, 0x12345678u, 1},
  {12000u, 24u, 0xabcdef01u, 1},
  {12000u, 24u, 0x55aa55aau, 1},
  {12000u, 24u, 0x0f0f0f0fu, 1},
  {12000u, 32u, 0x00000000u, 1},
  {12000u, 32u, 0x00000001u, 1},
  {12000u, 32u, 0x00000100u, 1},
  {12000u, 32u, 0x7fffffffu, 1},
  {12000u, 32u, 0x80000000u, 1},
  {12000u, 32u, 0xffffffffu, 1},
  {12000u, 32u, 0x12345678u, 1},
  {12000u, 32u, 0xabcdef01u, 1},
  {12000u, 32u, 0x55aa55aau, 1},
  {12000u, 32u, 0x0f0f0f0fu, 1},
  {12000u, 64u, 0x00000000u, 1},
  {12000u, 64u, 0x00000001u, 1},
  {12000u, 64u, 0x00000100u, 1},
  {12000u, 64u, 0x7fffffffu, 1},
  {12000u, 64u, 0x80000000u, 1},
  {12000u, 64u, 0xffffffffu, 1},
  {12000u, 64u, 0x12345678u, 1},
  {12000u, 64u, 0xabcdef01u, 1},
  {12000u, 64u, 0x55aa55aau, 1},
  {12000u, 64u, 0x0f0f0f0fu, 1},
  {12000u, 100u, 0x00000000u, 1},
  {12000u, 100u, 0x00000001u, 1},
  {12000u, 100u, 0x00000100u, 1},
  {12000u, 100u, 0x7fffffffu, 1},
  {12000u, 100u, 0x80000000u, 1},
  {12000u, 100u, 0xffffffffu, 1},
  {12000u, 100u, 0x12345678u, 1},
  {12000u, 100u, 0xabcdef01u, 1},
  {12000u, 100u, 0x55aa55aau, 1},
  {12000u, 100u, 0x0f0f0f0fu, 1},
  {12000u, 128u, 0x00000000u, 1},
  {12000u, 128u, 0x00000001u, 1},
  {12000u, 128u, 0x00000100u, 1},
  {12000u, 128u, 0x7fffffffu, 1},
  {12000u, 128u, 0x80000000u, 1},
  {12000u, 128u, 0xffffffffu, 1},
  {12000u, 128u, 0x12345678u, 1},
  {12000u, 128u, 0xabcdef01u, 1},
  {12000u, 128u, 0x55aa55aau, 1},
  {12000u, 128u, 0x0f0f0f0fu, 1},
  {12000u, 256u, 0x00000000u, 1},
  {12000u, 256u, 0x00000001u, 1},
  {12000u, 256u, 0x00000100u, 1},
  {12000u, 256u, 0x7fffffffu, 1},
  {12000u, 256u, 0x80000000u, 1},
  {12000u, 256u, 0xffffffffu, 1},
  {12000u, 256u, 0x12345678u, 1},
  {12000u, 256u, 0xabcdef01u, 1},
  {12000u, 256u, 0x55aa55aau, 1},
  {12000u, 256u, 0x0f0f0f0fu, 1},
  {12000u, 400u, 0x00000000u, 1},
  {12000u, 400u, 0x00000001u, 1},
  {12000u, 400u, 0x00000100u, 1},
  {12000u, 400u, 0x7fffffffu, 1},
  {12000u, 400u, 0x80000000u, 1},
  {12000u, 400u, 0xffffffffu, 1},
  {12000u, 400u, 0x12345678u, 1},
  {12000u, 400u, 0xabcdef01u, 1},
  {12000u, 400u, 0x55aa55aau, 1},
  {12000u, 400u, 0x0f0f0f0fu, 1},
  {12000u, 512u, 0x00000000u, 1},
  {12000u, 512u, 0x00000001u, 1},
  {12000u, 512u, 0x00000100u, 1},
  {12000u, 512u, 0x7fffffffu, 1},
  {12000u, 512u, 0x80000000u, 1},
  {12000u, 512u, 0xffffffffu, 1},
  {12000u, 512u, 0x12345678u, 1},
  {12000u, 512u, 0xabcdef01u, 1},
  {12000u, 512u, 0x55aa55aau, 1},
  {12000u, 512u, 0x0f0f0f0fu, 1},
  {16383u, 8u, 0x00000000u, 1},
  {16383u, 8u, 0x00000001u, 1},
  {16383u, 8u, 0x00000100u, 1},
  {16383u, 8u, 0x7fffffffu, 1},
  {16383u, 8u, 0x80000000u, 1},
  {16383u, 8u, 0xffffffffu, 1},
  {16383u, 8u, 0x12345678u, 1},
  {16383u, 8u, 0xabcdef01u, 1},
  {16383u, 8u, 0x55aa55aau, 1},
  {16383u, 8u, 0x0f0f0f0fu, 1},
  {16383u, 16u, 0x00000000u, 1},
  {16383u, 16u, 0x00000001u, 1},
  {16383u, 16u, 0x00000100u, 1},
  {16383u, 16u, 0x7fffffffu, 1},
  {16383u, 16u, 0x80000000u, 1},
  {16383u, 16u, 0xffffffffu, 1},
  {16383u, 16u, 0x12345678u, 1},
  {16383u, 16u, 0xabcdef01u, 1},
  {16383u, 16u, 0x55aa55aau, 1},
  {16383u, 16u, 0x0f0f0f0fu, 1},
  {16383u, 24u, 0x00000000u, 1},
  {16383u, 24u, 0x00000001u, 1},
  {16383u, 24u, 0x00000100u, 1},
  {16383u, 24u, 0x7fffffffu, 1},
  {16383u, 24u, 0x80000000u, 1},
  {16383u, 24u, 0xffffffffu, 1},
  {16383u, 24u, 0x12345678u, 1},
  {16383u, 24u, 0xabcdef01u, 1},
  {16383u, 24u, 0x55aa55aau, 1},
  {16383u, 24u, 0x0f0f0f0fu, 1},
  {16383u, 32u, 0x00000000u, 1},
  {16383u, 32u, 0x00000001u, 1},
  {16383u, 32u, 0x00000100u, 1},
  {16383u, 32u, 0x7fffffffu, 1},
  {16383u, 32u, 0x80000000u, 1},
  {16383u, 32u, 0xffffffffu, 1},
  {16383u, 32u, 0x12345678u, 1},
  {16383u, 32u, 0xabcdef01u, 1},
  {16383u, 32u, 0x55aa55aau, 1},
  {16383u, 32u, 0x0f0f0f0fu, 1},
  {16383u, 64u, 0x00000000u, 1},
  {16383u, 64u, 0x00000001u, 1},
  {16383u, 64u, 0x00000100u, 1},
  {16383u, 64u, 0x7fffffffu, 1},
  {16383u, 64u, 0x80000000u, 1},
  {16383u, 64u, 0xffffffffu, 1},
  {16383u, 64u, 0x12345678u, 1},
  {16383u, 64u, 0xabcdef01u, 1},
  {16383u, 64u, 0x55aa55aau, 1},
  {16383u, 64u, 0x0f0f0f0fu, 1},
  {16383u, 100u, 0x00000000u, 1},
  {16383u, 100u, 0x00000001u, 1},
  {16383u, 100u, 0x00000100u, 1},
  {16383u, 100u, 0x7fffffffu, 1},
  {16383u, 100u, 0x80000000u, 1},
  {16383u, 100u, 0xffffffffu, 1},
  {16383u, 100u, 0x12345678u, 1},
  {16383u, 100u, 0xabcdef01u, 1},
  {16383u, 100u, 0x55aa55aau, 1},
  {16383u, 100u, 0x0f0f0f0fu, 1},
  {16383u, 128u, 0x00000000u, 1},
  {16383u, 128u, 0x00000001u, 1},
  {16383u, 128u, 0x00000100u, 1},
  {16383u, 128u, 0x7fffffffu, 1},
  {16383u, 128u, 0x80000000u, 1},
  {16383u, 128u, 0xffffffffu, 1},
  {16383u, 128u, 0x12345678u, 1},
  {16383u, 128u, 0xabcdef01u, 1},
  {16383u, 128u, 0x55aa55aau, 1},
  {16383u, 128u, 0x0f0f0f0fu, 1},
  {16383u, 256u, 0x00000000u, 1},
  {16383u, 256u, 0x00000001u, 1},
  {16383u, 256u, 0x00000100u, 1},
  {16383u, 256u, 0x7fffffffu, 1},
  {16383u, 256u, 0x80000000u, 1},
  {16383u, 256u, 0xffffffffu, 1},
  {16383u, 256u, 0x12345678u, 1},
  {16383u, 256u, 0xabcdef01u, 1},
  {16383u, 256u, 0x55aa55aau, 1},
  {16383u, 256u, 0x0f0f0f0fu, 1},
  {16383u, 400u, 0x00000000u, 1},
  {16383u, 400u, 0x00000001u, 1},
  {16383u, 400u, 0x00000100u, 1},
  {16383u, 400u, 0x7fffffffu, 1},
  {16383u, 400u, 0x80000000u, 1},
  {16383u, 400u, 0xffffffffu, 1},
  {16383u, 400u, 0x12345678u, 1},
  {16383u, 400u, 0xabcdef01u, 1},
  {16383u, 400u, 0x55aa55aau, 1},
  {16383u, 400u, 0x0f0f0f0fu, 1},
  {16383u, 512u, 0x00000000u, 1},
  {16383u, 512u, 0x00000001u, 1},
  {16383u, 512u, 0x00000100u, 1},
  {16383u, 512u, 0x7fffffffu, 1},
  {16383u, 512u, 0x80000000u, 1},
  {16383u, 512u, 0xffffffffu, 1},
  {16383u, 512u, 0x12345678u, 1},
  {16383u, 512u, 0xabcdef01u, 1},
  {16383u, 512u, 0x55aa55aau, 1},
  {16383u, 512u, 0x0f0f0f0fu, 1},
  {16384u, 8u, 0x00000000u, 1},
  {16384u, 8u, 0x00000001u, 1},
  {16384u, 8u, 0x00000100u, 1},
  {16384u, 8u, 0x7fffffffu, 1},
  {16384u, 8u, 0x80000000u, 1},
  {16384u, 8u, 0xffffffffu, 1},
  {16384u, 8u, 0x12345678u, 1},
  {16384u, 8u, 0xabcdef01u, 1},
  {16384u, 8u, 0x55aa55aau, 1},
  {16384u, 8u, 0x0f0f0f0fu, 1},
  {16384u, 16u, 0x00000000u, 1},
  {16384u, 16u, 0x00000001u, 1},
  {16384u, 16u, 0x00000100u, 1},
  {16384u, 16u, 0x7fffffffu, 1},
  {16384u, 16u, 0x80000000u, 1},
  {16384u, 16u, 0xffffffffu, 1},
  {16384u, 16u, 0x12345678u, 1},
  {16384u, 16u, 0xabcdef01u, 1},
  {16384u, 16u, 0x55aa55aau, 1},
  {16384u, 16u, 0x0f0f0f0fu, 1},
  {16384u, 24u, 0x00000000u, 1},
  {16384u, 24u, 0x00000001u, 1},
  {16384u, 24u, 0x00000100u, 1},
  {16384u, 24u, 0x7fffffffu, 1},
  {16384u, 24u, 0x80000000u, 1},
  {16384u, 24u, 0xffffffffu, 1},
  {16384u, 24u, 0x12345678u, 1},
  {16384u, 24u, 0xabcdef01u, 1},
  {16384u, 24u, 0x55aa55aau, 1},
  {16384u, 24u, 0x0f0f0f0fu, 1},
  {16384u, 32u, 0x00000000u, 1},
  {16384u, 32u, 0x00000001u, 1},
  {16384u, 32u, 0x00000100u, 1},
  {16384u, 32u, 0x7fffffffu, 1},
  {16384u, 32u, 0x80000000u, 1},
  {16384u, 32u, 0xffffffffu, 1},
  {16384u, 32u, 0x12345678u, 1},
  {16384u, 32u, 0xabcdef01u, 1},
  {16384u, 32u, 0x55aa55aau, 1},
  {16384u, 32u, 0x0f0f0f0fu, 1},
  {16384u, 64u, 0x00000000u, 1},
  {16384u, 64u, 0x00000001u, 1},
  {16384u, 64u, 0x00000100u, 1},
  {16384u, 64u, 0x7fffffffu, 1},
  {16384u, 64u, 0x80000000u, 1},
  {16384u, 64u, 0xffffffffu, 1},
  {16384u, 64u, 0x12345678u, 1},
  {16384u, 64u, 0xabcdef01u, 1},
  {16384u, 64u, 0x55aa55aau, 1},
  {16384u, 64u, 0x0f0f0f0fu, 1},
  {16384u, 100u, 0x00000000u, 1},
  {16384u, 100u, 0x00000001u, 1},
  {16384u, 100u, 0x00000100u, 1},
  {16384u, 100u, 0x7fffffffu, 1},
  {16384u, 100u, 0x80000000u, 1},
  {16384u, 100u, 0xffffffffu, 1},
  {16384u, 100u, 0x12345678u, 1},
  {16384u, 100u, 0xabcdef01u, 1},
  {16384u, 100u, 0x55aa55aau, 1},
  {16384u, 100u, 0x0f0f0f0fu, 1},
  {16384u, 128u, 0x00000000u, 1},
  {16384u, 128u, 0x00000001u, 1},
  {16384u, 128u, 0x00000100u, 1},
  {16384u, 128u, 0x7fffffffu, 1},
  {16384u, 128u, 0x80000000u, 1},
  {16384u, 128u, 0xffffffffu, 1},
  {16384u, 128u, 0x12345678u, 1},
  {16384u, 128u, 0xabcdef01u, 1},
  {16384u, 128u, 0x55aa55aau, 1},
  {16384u, 128u, 0x0f0f0f0fu, 1},
  {16384u, 256u, 0x00000000u, 1},
  {16384u, 256u, 0x00000001u, 1},
  {16384u, 256u, 0x00000100u, 1},
  {16384u, 256u, 0x7fffffffu, 1},
  {16384u, 256u, 0x80000000u, 1},
  {16384u, 256u, 0xffffffffu, 1},
  {16384u, 256u, 0x12345678u, 1},
  {16384u, 256u, 0xabcdef01u, 1},
  {16384u, 256u, 0x55aa55aau, 1},
  {16384u, 256u, 0x0f0f0f0fu, 1},
  {16384u, 400u, 0x00000000u, 1},
  {16384u, 400u, 0x00000001u, 1},
  {16384u, 400u, 0x00000100u, 1},
  {16384u, 400u, 0x7fffffffu, 1},
  {16384u, 400u, 0x80000000u, 1},
  {16384u, 400u, 0xffffffffu, 1},
  {16384u, 400u, 0x12345678u, 1},
  {16384u, 400u, 0xabcdef01u, 1},
  {16384u, 400u, 0x55aa55aau, 1},
  {16384u, 400u, 0x0f0f0f0fu, 1},
  {16384u, 512u, 0x00000000u, 1},
  {16384u, 512u, 0x00000001u, 1},
  {16384u, 512u, 0x00000100u, 1},
  {16384u, 512u, 0x7fffffffu, 1},
  {16384u, 512u, 0x80000000u, 1},
  {16384u, 512u, 0xffffffffu, 1},
  {16384u, 512u, 0x12345678u, 1},
  {16384u, 512u, 0xabcdef01u, 1},
  {16384u, 512u, 0x55aa55aau, 1},
  {16384u, 512u, 0x0f0f0f0fu, 1},
};

/* Chunker roots, N intkey rows. */
static const struct { int n; const char *zRoot; } aRoots[] = {
  {0, "0000000000000000000000000000000000000000"},
  {1, "98f292686c25a82ec24688a20fc9b9a7b63483f8"},
  {2, "799d4a707e8e69102252ce978f539f8484f29b2b"},
  {10, "82690a7d1a73a1d699ab8b6c253eccf95a4fa1b2"},
  {50, "9caeb31eaa26dcf7db5dda7644a7e64aa9b9e934"},
  {100, "c63aa536ce523cbd01f0019df5765d27e3fd4482"},
  {200, "270633c83111cc1734581ebf2e8015fc740fe8b4"},
  {500, "ec910c2cb4ed3e437c64742e8d77dad0ee9ebcb1"},
  {1000, "1473220361cee2c16f077ec4a7976ba5c50f719a"},
  {2000, "2322be9bc59995147ae8c69e62155cf7dca65f2e"},
  {5000, "8710988fa386978595f3c9faa9d8324866957acf"},
};

static void test_xxh32_golden(void){
  int i;
  char zName[64];
  for(i=0; i<(int)(sizeof(aXxh)/sizeof(aXxh[0])); i++){
    u8 key[8];
    u32 got;
    encodeIntKey(aXxh[i].key, key);
    got = prollyXXH32(key, 8, 0);
    sqlite3_snprintf(sizeof(zName), zName, "xxh32 key=%d", aXxh[i].key);
    check(zName, got==aXxh[i].h);
  }
}

static void test_weibull_golden(void){
  int i;
  char zName[96];
  for(i=0; i<(int)(sizeof(aWeibull)/sizeof(aWeibull[0])); i++){
    int got = prollyWeibullCheck(
      aWeibull[i].size, aWeibull[i].thisSize, aWeibull[i].hash
    );
    sqlite3_snprintf(sizeof(zName), zName, "weibull %u/%u/0x%x",
                     aWeibull[i].size, aWeibull[i].thisSize, aWeibull[i].hash);
    check(zName, got==aWeibull[i].want);
  }
}

static void test_chunker_root_golden(void){
  ChunkStore cs;
  int i, rc;
  char zName[64];

  memset(&cs, 0, sizeof(cs));
  rc = chunkStoreOpen(&cs, sqlite3_vfs_find(0), ":memory:",
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                      | SQLITE_OPEN_MAIN_DB);
  check("chunker: open memory store", rc==SQLITE_OK);
  if( rc!=SQLITE_OK ) return;

  for(i=0; i<(int)(sizeof(aRoots)/sizeof(aRoots[0])); i++){
    ProllyChunker ch;
    ProllyHash root, want;
    int k, n = aRoots[i].n;
    int ok;

    if( !parseHexHash(aRoots[i].zRoot, &want) ){
      check("chunker: parse golden root", 0);
      continue;
    }
    prollyChunkerInit(&ch, &cs, PROLLY_NODE_INTKEY | PROLLY_NODE_SUBTREE_COUNTS);
    ok = 1;
    for(k=0; k<n; k++){
      u8 key[8];
      u8 val[4] = {1,2,3,4};
      encodeIntKey(k, key);
      if( prollyChunkerAdd(&ch, key, 8, val, 4)!=SQLITE_OK ){
        ok = 0;
        break;
      }
    }
    if( ok && prollyChunkerFinish(&ch)!=SQLITE_OK ) ok = 0;
    prollyChunkerGetRoot(&ch, &root);
    sqlite3_snprintf(sizeof(zName), zName, "chunker root n=%d", n);
    check(zName, ok && memcmp(root.data, want.data, PROLLY_HASH_SIZE)==0);
    prollyChunkerFree(&ch);
    chunkStoreRollback(&cs);
  }
  chunkStoreClose(&cs);
}

static void test_chunker_repeatable(void){
  ChunkStore cs;
  ProllyHash a, b;
  ProllyChunker ch;
  int k, rc;

  memset(&cs, 0, sizeof(cs));
  rc = chunkStoreOpen(&cs, sqlite3_vfs_find(0), ":memory:",
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                      | SQLITE_OPEN_MAIN_DB);
  check("repeatable: open", rc==SQLITE_OK);
  if( rc!=SQLITE_OK ) return;

  prollyChunkerInit(&ch, &cs, PROLLY_NODE_INTKEY | PROLLY_NODE_SUBTREE_COUNTS);
  for(k=0; k<777; k++){
    u8 key[8];
    u8 val[8];
    encodeIntKey(k, key);
    encodeIntKey(k*3+1, val);
    if( prollyChunkerAdd(&ch, key, 8, val, 8)!=SQLITE_OK ){
      check("repeatable: add1", 0);
      prollyChunkerFree(&ch);
      chunkStoreClose(&cs);
      return;
    }
  }
  check("repeatable: finish1", prollyChunkerFinish(&ch)==SQLITE_OK);
  prollyChunkerGetRoot(&ch, &a);
  prollyChunkerFree(&ch);
  chunkStoreRollback(&cs);

  prollyChunkerInit(&ch, &cs, PROLLY_NODE_INTKEY | PROLLY_NODE_SUBTREE_COUNTS);
  for(k=0; k<777; k++){
    u8 key[8];
    u8 val[8];
    encodeIntKey(k, key);
    encodeIntKey(k*3+1, val);
    if( prollyChunkerAdd(&ch, key, 8, val, 8)!=SQLITE_OK ){
      check("repeatable: add2", 0);
      prollyChunkerFree(&ch);
      chunkStoreClose(&cs);
      return;
    }
  }
  check("repeatable: finish2", prollyChunkerFinish(&ch)==SQLITE_OK);
  prollyChunkerGetRoot(&ch, &b);
  prollyChunkerFree(&ch);
  chunkStoreClose(&cs);

  check("repeatable: same root twice",
        memcmp(a.data, b.data, PROLLY_HASH_SIZE)==0);
}

static int storeBuiltNode(ChunkStore *cs, ProllyNodeBuilder *b, ProllyHash *hash){
  u8 *pData = 0;
  int nData = 0;
  int rc = prollyNodeBuilderFinish(b, &pData, &nData);
  if( rc==SQLITE_OK ) rc = chunkStorePut(cs, pData, nData, hash);
  sqlite3_free(pData);
  prollyNodeBuilderFree(b);
  return rc;
}

static void test_canonical_root_chain(void){
  int useCache, internal;
  for(useCache=0; useCache<2; useCache++){
    for(internal=0; internal<2; internal++){
      ChunkStore cs;
      ProllyCache cache;
      ProllyChunker ch;
      ProllyCursor cur;
      ProllyNodeBuilder b;
      ProllyHash leaves[2], want, wrapped, root;
      u8 key[8];
      u8 flags = PROLLY_NODE_INTKEY | PROLLY_NODE_SUBTREE_COUNTS;
      int rc, i, level, res = 1, nRows = 0;

      memset(&cs, 0, sizeof(cs));
      rc = chunkStoreOpen(&cs, sqlite3_vfs_find(0), ":memory:",
                         SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                         | SQLITE_OPEN_MAIN_DB);
      check("root chain: open", rc==SQLITE_OK);
      if( rc!=SQLITE_OK ) continue;
      rc = prollyCacheInit(&cache, 1024*1024);
      check("root chain: cache init", rc==SQLITE_OK);
      if( rc!=SQLITE_OK ){
        chunkStoreClose(&cs);
        continue;
      }
      for(i=0; i<2 && rc==SQLITE_OK; i++){
        prollyNodeBuilderInit(&b, 0, flags);
        encodeIntKey(i+1, key);
        rc = prollyNodeBuilderAdd(&b, key, 8, key, 8);
        if( rc==SQLITE_OK ) rc = storeBuiltNode(&cs, &b, &leaves[i]);
        else prollyNodeBuilderFree(&b);
      }
      if( rc==SQLITE_OK ){
        want = leaves[0];
        if( internal ){
          prollyNodeBuilderInit(&b, 1, flags);
          for(i=0; i<2 && rc==SQLITE_OK; i++){
            encodeIntKey(i+1, key);
            rc = prollyNodeBuilderAddWithCount(&b, key, 8,
                                              leaves[i].data, PROLLY_HASH_SIZE, 1);
          }
          if( rc==SQLITE_OK ) rc = storeBuiltNode(&cs, &b, &want);
          else prollyNodeBuilderFree(&b);
        }
        wrapped = want;
        encodeIntKey(internal+1, key);
        for(level=internal+1; level<=internal+3 && rc==SQLITE_OK; level++){
          prollyNodeBuilderInit(&b, (u8)level, flags);
          rc = prollyNodeBuilderAddWithCount(&b, key, 8, wrapped.data,
                                            PROLLY_HASH_SIZE, internal+1);
          if( rc==SQLITE_OK ) rc = storeBuiltNode(&cs, &b, &wrapped);
          else prollyNodeBuilderFree(&b);
        }
      }
      check("root chain: build legacy wrappers", rc==SQLITE_OK);
      if( rc==SQLITE_OK ){
        prollyCursorInit(&cur, &cs, &cache, &wrapped, flags);
        rc = prollyCursorFirst(&cur, &res);
        while( rc==SQLITE_OK && prollyCursorIsValid(&cur) ){
          nRows++;
          rc = prollyCursorNext(&cur);
        }
        prollyCursorClose(&cur);
        check("root chain: legacy root remains readable",
              rc==SQLITE_OK && res==0 && nRows==internal+1);
        prollyChunkerInitWithCache(&ch, &cs, useCache ? &cache : 0, flags);
        rc = prollyChunkerAddAtLevelWithCount(&ch, internal+4, key, 8,
                                             wrapped.data, PROLLY_HASH_SIZE,
                                             internal+1);
        if( rc==SQLITE_OK ) rc = prollyChunkerFinish(&ch);
        prollyChunkerGetRoot(&ch, &root);
        check("root chain: stop at leaf or branching node",
              rc==SQLITE_OK && memcmp(root.data, want.data, PROLLY_HASH_SIZE)==0);
        prollyChunkerFree(&ch);
      }
      prollyCacheFree(&cache);
      chunkStoreClose(&cs);
    }
  }
}

static void test_sparse_canonical_root(void){
  int useCache;
  for(useCache=0; useCache<2; useCache++){
    ChunkStore cs;
    ProllyCache cache;
    ProllyChunker ch;
    ProllyHash want, root;
    ProllyNode node;
    u8 key[8], *pData = 0;
    char zPath[256];
    sqlite3_vfs *pVfs = sqlite3_vfs_find(0);
    int rc, nData = 0, nDataPhys = 0;
    sqlite3_int64 oldLimit;

    memset(&cs, 0, sizeof(cs));
    sqlite3_snprintf(sizeof(zPath), zPath,
                     DOLTLITE_TEST_TMPDIR "/chunker-sparse-%d.db", useCache);
    pVfs->xDelete(pVfs, zPath, 0);
    rc = chunkStoreOpen(&cs, pVfs, zPath,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                       | SQLITE_OPEN_MAIN_DB);
    check("sparse root: open", rc==SQLITE_OK);
    if( rc!=SQLITE_OK ) continue;
    rc = prollyCacheInit(&cache, 1024*1024);
    check("sparse root: cache init", rc==SQLITE_OK);
    if( rc!=SQLITE_OK ){
      chunkStoreClose(&cs);
      continue;
    }
    prollyChunkerInitWithCache(&ch, &cs, useCache ? &cache : 0,
                              PROLLY_NODE_INTKEY | PROLLY_NODE_SUBTREE_COUNTS);
    encodeIntKey(1, key);
    rc = prollyChunkerAdd(&ch, key, 8, (const u8*)"x", 1);
    if( rc==SQLITE_OK ){
      encodeIntKey(2, key);
      rc = prollyChunkerAddZeroTail(&ch, key, 8, 0, 0, 64*1024*1024);
    }
    check("sparse root: add", rc==SQLITE_OK);
    check("sparse root: last row flushed leaf",
          rc==SQLITE_OK && ch.nLevels==2
          && ch.aLevel[0].builder.nItems==0
          && ch.aLevel[1].builder.nItems==1);
    if( rc==SQLITE_OK && ch.nLevels==2
        && ch.aLevel[1].builder.nItems==1 ){
      memcpy(want.data, ch.aLevel[1].builder.pValBuf, PROLLY_HASH_SIZE);
      oldLimit = sqlite3_hard_heap_limit64(sqlite3_memory_used() + 1024*1024);
      rc = prollyChunkerFinish(&ch);
      sqlite3_hard_heap_limit64(oldLimit);
      check("sparse root: finish within memory limit", rc==SQLITE_OK);
      if( rc==SQLITE_OK ){
        prollyChunkerGetRoot(&ch, &root);
        check("sparse root: collapsed to leaf",
              memcmp(root.data, want.data, PROLLY_HASH_SIZE)==0);
        rc = chunkStoreGetSparse(&cs, &root, &pData, &nData, &nDataPhys);
        if( rc==SQLITE_OK ){
          rc = prollyNodeParseSparse(&node, pData, nData, nDataPhys);
        }
        check("sparse root: leaf remains sparse",
              rc==SQLITE_OK && node.level==0 && node.nItems==2
              && nData>64*1024*1024 && nDataPhys<1024);
        sqlite3_free(pData);
      }
    }
    prollyChunkerFree(&ch);
    prollyCacheFree(&cache);
    chunkStoreClose(&cs);
    pVfs->xDelete(pVfs, zPath, 0);
  }
}

int main(void){
  sqlite3_initialize();
  printf("Prolly chunker boundary golden tests\n");
  printf("====================================\n\n");

  test_xxh32_golden();
  test_weibull_golden();
  test_chunker_root_golden();
  test_chunker_repeatable();
  test_canonical_root_chain();
  test_sparse_canonical_root();

  printf("\n%d passed, %d failed\n", nPass, nFail);
  return nFail>0 ? 1 : 0;
}
