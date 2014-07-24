#include <iostream>
#include <set>
using namespace std;

#include "testharness.h"
#include "skiplist.h"

class SkipListTest {};

TEST(SkipListTest, NormalTest) {
	SkipList list;
	for (int i = 1; i < 10; i ++) list.Insert(i);
	list.Debug();

	for (int i = 1; i < 10; i ++) {
		ASSERT_TRUE(list.Contains(i));
	}
	for (int i = 1; i < 10; i += 2) list.Erase(i);
	list.Debug();

	for (int i = 2; i < 10; i += 2) {
		ASSERT_TRUE(!list.Contains(i-1));
		ASSERT_TRUE(list.Contains(i));
	}
}

TEST(SkipListTest, IteratorTest) {
	const int N = 2000;
	const int R = 5000;
	std::set<int> keys;
	SkipList list;
	for (int i = 0; i < N; i ++) {
		int k = rand()%R;
		if (keys.insert(k).second) {
			list.Insert(k);
		}
	}
	for (int i = 0; i < R; i++) {
		if (list.Contains(i)) ASSERT_EQ((int)keys.count(i), 1);
		else ASSERT_EQ((int)keys.count(i), 0);
	}
	
	{//Simple Iterator tests
		SkipList::Iterator it(&list);
		ASSERT_TRUE(!it.Valid());
		
		it.SeekToFirst();
		ASSERT_TRUE(it.Valid());
		ASSERT_EQ(*(keys.begin()), it.key());
	}
	

	{//Forwar Iteration test
		SkipList::Iterator it(&list);
		std::set<int>::iterator model_it = keys.begin();
		
		for(it.SeekToFirst(); it.Valid() && model_it!=keys.end(); it.Next(), model_it++) {
			ASSERT_EQ(*model_it, it.key());
		}
		ASSERT_TRUE(!it.Valid());
		ASSERT_TRUE(model_it==keys.end());
	}
}
int main(int argc, char **argv)
{
	return ::test::RunAllTests();
}
