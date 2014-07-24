#include <iostream>
#include "cache.h"

using namespace std;

typedef int (*COMPARE)(const void*left_key, const int left_key_sz, const void*right_key, const int right_key_sz);
typedef void (*DELETER)(const void* key, const int key_sz, const void *value, const int value_sz);
typedef uint32_t (*HASHFUNC)(const void* key, const int key_sz);

int StrCmp(const void*left_key, const int left_key_sz, const void*right_key, const int right_key_sz)
{
    int cmplen = left_key_sz<right_key_sz?left_key_sz:right_key_sz;
    int cmp = memcmp(left_key, right_key, cmplen);
    if (cmp != 0) return cmp;
    return left_key_sz - right_key_sz;
}

void Deleter(const void* key, const int key_sz, const void *value, const int value_sz)
{
    printf("Deleter key=%s val=%s\n", (char*)key, (char *)value);
}

inline uint32_t DecodeFixed32(const char* ptr) {
    // Load the raw bytes
    uint32_t result;
    memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
    return result;
}
#define FALLTHROUGH_INTENDED do { } while (0)
uint32_t StrHash(const void* key, const int n)
{
    uint32_t seed=0;
    char *data = (char*)key;
    // Similar to murmur hash
    const uint32_t m = 0xc6a4a793;
    const uint32_t r = 24;
    const char* limit = data + n;
    uint32_t h = seed ^ (n * m);

    // Pick up four bytes at a time
    while (data + 4 <= limit) {
        uint32_t w = DecodeFixed32(data);
        data += 4;
        h += w;
        h *= m;
        h ^= (h >> 16);
    }

    // Pick up remaining bytes
    switch (limit - data) {
    case 3:
        h += data[2] << 16;
        FALLTHROUGH_INTENDED;
    case 2:
        h += data[1] << 8;
        FALLTHROUGH_INTENDED;
    case 1:
        h += data[0];
        h *= m;
        h ^= (h >> r);
        break;
    }
    return h;
}

Cache *cache = CreateTimeExpiredCache(10, StrCmp, StrHash, 2);

static char keys[][20] = {"a1", "a2", "a3", "b1", "b2", "b3"};
static char values[][20] = {"a1", "a2", "a3", "b1", "b2", "b3"};
int numbers = 6;

int main()
{
    for (int i = 0; i < 20; i ++) {
        int idx = i % numbers;
        printf("Insert1 key=%s value=%s\n", keys[idx], values[idx]);
        cache->Insert(keys[idx], strlen(keys[idx])+1, values[idx], strlen(values[idx])+1, Deleter);
        printf("Insert2 key=%s value=%s\n", keys[idx], values[idx]);
        //if (i == 2) sleep(5);
    }

    //sleep(5);
    
    for (int i = numbers-1; i >=0; i --) {
        char *value = NULL;
        int vsz = 0;
        value = (char*)cache->Lookup(keys[i], strlen(keys[i])+1, vsz);
        if (value == NULL) printf("key=%s value=NULL\n", keys[i]);
        else printf("key=%s value=%s %d\n", keys[i], value, vsz);
    }
    return 0;
}
