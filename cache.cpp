#include "Cache.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

class NoCache : public Cache
{
public:
    NoCache() {}
    ~NoCache() {}
    int GetUsage() {return 0;}
    void Insert(const void* key, const int key_sz, const void *value, const int value_sz, DELETER deleter) {}
    void* Lookup(const void* key, const int key_sz, int &value_sz) {return NULL;}
};

// TimeExpiredCache的内部数据结构
struct TEHandler
{
    struct TEHandler *prev;
    struct TEHandler *next;
    struct TEHandler *next_hash;
    uint32_t intime;
    uint32_t hash;
    DELETER deleter;
    int key_sz;
    int value_sz;
    void *key;
    void *value;

    TEHandler() {}
    TEHandler(const void *k, const int ksz)
    {
        key = (void *)k; key_sz = ksz;
        value = NULL;
        deleter = NULL;
        prev = next = NULL;
        value_sz = 0;
    }
    TEHandler(void *k, int ksz, void *v, int vsz, DELETER del)
    {
        key = k; key_sz = ksz;
        value = v; value_sz = vsz;
        deleter = del;
        prev = next = NULL;
    }
};
//简单的TEHandler的table.
class HandleTable
{
public:
    HandleTable() : compare_(NULL), length_(0), elems_(0), list_(NULL) { Resize(); }
    HandleTable(COMPARE compare) : compare_(compare), length_(0), elems_(0), list_(NULL) { Resize(); }
    ~HandleTable() { delete[] list_; }

    void SetCompare(COMPARE compare) {compare_ = compare;}

	TEHandler* Lookup(const void* key, const int key_sz, uint32_t hash) {
		return *FindPointer(key, key_sz, hash);
	}

	TEHandler* Insert(TEHandler* h) {
		TEHandler** ptr = FindPointer(h->key, h->key_sz, h->hash);
		TEHandler* old = *ptr;
		h->next_hash = (old == NULL ? NULL : old->next_hash);
		*ptr = h;
		if (old == NULL) {
			++elems_;
			if (elems_ > length_) {
				// Since each cache entry is fairly large, we aim for a small
				// average linked list length (<= 1).
				Resize();
			}
		}
		return old;
	}

	TEHandler* Remove(const void* key, const int key_sz, uint32_t hash) {
		TEHandler** ptr = FindPointer(key, key_sz, hash);
		TEHandler* result = *ptr;
		if (result != NULL) {
			*ptr = result->next_hash;
			--elems_;
		}
		return result;
	}

private:
    COMPARE compare_;
	// The table consists of an array of buckets where each bucket is
	// a linked list of cache entries that hash into the bucket.
	uint32_t length_;
	uint32_t elems_;
	TEHandler** list_;

	// Return a pointer to slot that points to a cache entry that
	// matches key/hash.  If there is no such cache entry, return a
	// pointer to the trailing slot in the corresponding linked list.
	TEHandler** FindPointer(const void* key, const int key_sz, uint32_t hash) {
		TEHandler** ptr = &list_[hash & (length_ - 1)];
		while (*ptr != NULL &&
				((*ptr)->hash != hash || compare_(key, key_sz, (*ptr)->key, (*ptr)->key_sz))!=0) {
			ptr = &(*ptr)->next_hash;
		}
		return ptr;
	}

	void Resize() {
		uint32_t new_length = 4;
		while (new_length < elems_) {
			new_length *= 2;
		}
		TEHandler** new_list = new TEHandler*[new_length];
		memset(new_list, 0, sizeof(new_list[0]) * new_length);
		uint32_t count = 0;
		for (uint32_t i = 0; i < length_; i++) {
			TEHandler* h = list_[i];
			while (h != NULL) {
				TEHandler* next = h->next_hash;
				uint32_t hash = h->hash;
				TEHandler** ptr = &new_list[hash & (new_length - 1)];
				h->next_hash = *ptr;
				*ptr = h;
				h = next;
				count++;
			}
		}
		assert(elems_ == count);
		delete[] list_;
		list_ = new_list;
		length_ = new_length;
	}
};

class TimeExpiredCache : public Cache
{
public:
    TimeExpiredCache(size_t capacity, COMPARE compare, HASHFUNC hashfunc, uint32_t expire)
        :capacity_(capacity),compare_(compare),hashfunc_(hashfunc), expire_(expire) 
    {
        lru_.next = &lru_;
        lru_.prev = &lru_;
        table_.SetCompare(compare_);
    }
    ~TimeExpiredCache() {}

    int GetUsage() {return 0;}

    void Insert(const void* key, const int key_sz, const void *value, const int value_sz, DELETER deleter)
    {
        char *buf = (char *)malloc(sizeof(TEHandler) + key_sz + value_sz + 1);
        if (buf == NULL) return ;
        TEHandler *n = reinterpret_cast<TEHandler*>(buf);
        if (n == NULL) return ;

        n->prev = n->next = NULL;
        n->intime = time(NULL);
        n->hash   = hashfunc_==NULL?0:hashfunc_(key, key_sz);
        n->deleter = deleter;
        n->key_sz = key_sz;
        n->value_sz = value_sz;
        n->key = buf + sizeof(TEHandler);
        n->value = buf + sizeof(TEHandler) + key_sz;
        memcpy(n->key, key, key_sz);
        memcpy(n->value, value, value_sz);
        
        DLinkAppend(&lru_, n);
        usage_ ++;

        TEHandler* old = table_.Insert(n);
        if (old != NULL) {
            DLinkRemove(old);
            FreeHandle(old);
        }

        //uint32_t now = time(NULL);
        while(usage_ > capacity_ && lru_.next != &lru_) {
            TEHandler *old = lru_.next;
            //if ((usage_ < capacity_) && (expire_ > 0 && (now - old->intime) <= expire_)) break;
            DLinkRemove(old);
            table_.Remove(old->key, old->key_sz, old->hash);
            FreeHandle(old);
        }
    }

    void* Lookup(const void* key, const int key_sz, int &value_sz)
    {
        uint32_t hash = 0;
        if (hashfunc_ != NULL) hash = hashfunc_(key, key_sz);
        TEHandler* old = table_.Lookup(key, key_sz, hash);
        if (old != NULL) {
            if (expire_ > 0 && (time(NULL) - old->intime) > expire_) {
                DLinkRemove(old);
                table_.Remove(old->key, old->key_sz, old->hash);
                FreeHandle(old);
                return NULL;
            }
            value_sz = old->value_sz;
            return old->value;
        }
        return NULL;
    }

private:
    void DLinkRemove(TEHandler *n) {
        n->next->prev = n->prev;
        n->prev->next = n->next;
    }
    void DLinkAppend(TEHandler *head, TEHandler *n) {
        n->next = head;
        n->prev = head->prev;
        n->prev->next = n;
        n->next->prev = n;
    }
    void FreeHandle(TEHandler *n) {
        if (n->deleter != NULL) {
            n->deleter(n->key, n->key_sz, n->value, n->value_sz);
        }
        usage_ --;
        free((char*)n);
    }
private:
    size_t  capacity_;
    COMPARE compare_;
    HASHFUNC hashfunc_;
    uint32_t expire_;

    uint32_t usage_;
    HandleTable table_;
    TEHandler lru_;
};


Cache* CreateNoCache()
{
    return new NoCache();
}

//定期淘汰策略. 即进入cache的时间超过expire时, 就自动删除.
Cache* CreateTimeExpiredCache(size_t capacity, COMPARE compare, HASHFUNC hashfunc, uint32_t expire)
{
    return new TimeExpiredCache(capacity, compare, hashfunc, expire);
}


