#include <iostream>
using namespace std;

#include "testharness.h"
#include "hash.h"

class HashTest {};

TEST(HashTest, NormalTest)
{
	const char *data = "hashtest";
	uint32_t h = ::hash::Hash(data, strlen(data), 0);
	cout << h << endl;
}

int main()
{
    return ::test::RunAllTests();
}
