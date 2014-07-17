#include <iostream>
using namespace std;

#include "testharness.h"

class EmptyTest {};
TEST(EmptyTest, Int) {
	ASSERT_EQ(0, 0);
	ASSERT_LT(0, 1);
	ASSERT_LE(0, 1);
	ASSERT_GT(10, 8);
}

TEST(EmptyTest, Bool) {
	ASSERT_TRUE(true);
}

int main(int argc, char **argv)
{
	return ::test::RunAllTests();
}
