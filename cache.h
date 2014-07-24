#ifndef CACHE_H_
#define CACHE_H_

#include <stdlib.h>
#include <stdint.h>

typedef int (*COMPARE)(const void*left_key, const int left_key_sz, const void*right_key, const int right_key_sz);
typedef void (*DELETER)(const void* key, const int key_sz, const void *value, const int value_sz);
typedef uint32_t (*HASHFUNC)(const void* key, const int key_sz);

class Cache;

//不缓存
extern Cache* CreateNoCache();

//定期淘汰策略. 即进入cache的时间超过expire时, 就自动删除.
extern Cache* CreateTimeExpiredCache(size_t capacity, COMPARE compare, HASHFUNC hashfunc, uint32_t expire);

class Cache
{
public:
    Cache() {};
    virtual ~Cache() {};
    
    virtual int GetUsage() = 0;
    //cache应该分配自己的内存来存储key和value
    virtual void Insert(const void* key, const int key_sz, const void *value, const int value_sz, DELETER deleter) = 0;
    virtual void* Lookup(const void* key, const int key_sz, int &value_sz) = 0;

};


#endif

