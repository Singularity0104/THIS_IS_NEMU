#include "macro.h"
#include "time.h"
#include "stdlib.h"
#include "stdint.h"
#include "common.h"
#define cache_line concat(cache_line_, level)
#define cache_set concat(cache_set_, level)
#define cache concat(cache_, level)
#define Cache concat(Cache_, level)
#define init concat(cache, _init)
#define find concat(cache, _find)
#define replace concat(cache, _replace)

typedef struct cache_line {
    char block[B_size];
    int tag;
    char valid_bit;
} cache_line;

typedef struct cache_set {
    cache_line line[E_size];

} cache_set;


typedef struct cache {
    cache_set set[S_size];
    // void (* init_cache)(struct cache *cur);
    // char *(* find_addr)(struct cache *cur, hwaddr_t addr);
    // char *(* replace_block)(struct cache *cur, hwaddr_t addr);
} cache;


void init(cache *cur);
char *find(cache *cur, hwaddr_t addr);
char *replace(cache *cur, hwaddr_t addr);

#undef cache_line
#undef cache_set
#undef cache
#undef Cache
#undef init
#undef find
#undef replace