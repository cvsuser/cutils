#include <iostream>
using namespace std;

#include "testharness.h"
#include "bloom_filter.h"

class BloomTest
{
public:
    BloomTest()
    {
        bf_ = new BloomFilter(8, 10);
        ASSERT_TRUE(bf_!=NULL);
    }

    void Add(const string &key)
    {
        bf_->AddKey(key);
    }
    
    bool Match(const string &key)
    {
        return bf_->Match(key);
    }
    void Dump() {
        bf_->Dump();
    }
private:
    BloomFilter *bf_;
};


TEST(BloomTest, Empty)
{
    ASSERT_TRUE(!Match("hello"));
    ASSERT_TRUE(!Match("world"));
}

TEST(BloomTest, Small)
{
    Dump();
    Add("hello");
    Dump();
    Add("world");
    Dump();
    ASSERT_TRUE(Match("hello"));
    ASSERT_TRUE(Match("world"));
    ASSERT_TRUE(!Match("beijing"));
}

int main(int argc, char **argv)
{
	return ::test::RunAllTests();
}
