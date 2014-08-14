#ifndef BLOOM_FILTER_H_
#define BLOOM_FILTER_H_

#include <string>
using namespace std;

class BloomFilter
{
public:
    explicit BloomFilter(int bits_per_key, int max_key_num);
    ~BloomFilter();

    int AddKey(const string &key);
    bool Match(const string &key);
   
    void Dump();
private:
    size_t bits_per_key_;
    size_t max_key_num_;

    size_t bytes_, bits_;
    char*  array_;
};

#endif
