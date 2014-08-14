#include "bloom_filter.h"
#include "hash.h"

BloomFilter::BloomFilter(int bits_per_key, int max_key_num)
{
    bits_per_key_ = bits_per_key;
    max_key_num_  = max_key_num;
    
    // Compute bloom filter size (in both bits and bytes)
    bits_ = max_key_num_ * bits_per_key_;
    if (bits_ < 64) bits_ = 64;
    bytes_ = (bits_ + 7) / 8;
    bits_ = bytes_ * 8;

    array_ = (char*) new char[bytes_];
    if (array_ == NULL) return;
    memset(array_, 0, bytes_);
}

BloomFilter::~BloomFilter()
{
    if (array_ != NULL) delete array_;
}

static uint32_t BloomHash(const string &key)
{
    return hash::Hash(key.data(), key.size(), 0xbc9f1d34);
}

int BloomFilter::AddKey(const string &key)
{
    // Use double-hashing to generate a sequence of hash values.
	// See analysis in [Kirsch,Mitzenmacher 2006].
    uint32_t h = BloomHash(key);
    const uint32_t delta = (h>>17) | (h<<15);
    for (size_t i = 0; i < bits_per_key_; i ++) {
        const uint32_t bitpos = h % bits_;
        array_[bitpos/8] |= (1<<(bitpos%8));
        h += delta;
    }
    return 0;
}

bool BloomFilter::Match(const string &key)
{
    uint32_t h = BloomHash(key);
    const uint32_t delta = (h>>17) | (h<<15);
    for (size_t i = 0; i < bits_per_key_; i ++) {
        const uint32_t bitpos = h % bits_;
        if ((array_[bitpos/8] & (1<<(bitpos%8))) == 0) return false;
        h += delta;
    }
    return true;
}

void BloomFilter::Dump()
{
    fprintf(stderr, "bpk=%zu mkn=%zu\n", bits_per_key_, max_key_num_);
    fprintf(stderr, "bytes=%zu bits=%zu\n", bytes_, bits_);
    for (size_t i = 0; i < bytes_; i ++) {
        fprintf(stderr, "%zu:%x\t", i+1, array_[i]);
        if ((i+1)%5 == 0) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}
