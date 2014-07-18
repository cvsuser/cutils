#include <iostream>
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


int main(int argc, char **argv)
{
	return ::test::RunAllTests();
}
