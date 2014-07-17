#include "testharness.h"

#include <string>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

namespace test {

namespace {
struct Test {
	const char *base;
	const char *name;
	void (*func)();
};
std::vector<Test> *tests;
}

bool RegisterTest(const char* base, const char* name, void (*func)()) {
	if (tests == NULL) {
		tests = new std::vector<Test>;
	}
	Test t;
	t.base = base;
	t.name = name;
	t.func = func;
	tests->push_back(t);
	return true;
}

int RunAllTests() {
	const char* matcher = getenv("UNIT_TESTS");

	int num = 0;
	if (tests != NULL) {
		for (size_t i = 0; i < tests->size(); i++) {
			const Test& t = (*tests)[i];
			if (matcher != NULL) {
				std::string name = t.base;
				name.push_back('.');
				name.append(t.name);
				if (strstr(name.c_str(), matcher) == NULL) {
					continue;
				}
			}
			fprintf(stderr, "==== Test %s.%s\n", t.base, t.name);
			(*t.func)();
			++ num;
		}
	}
	fprintf(stderr, "==== PASSED %d tests\n", num);
	return 0;
}


int CreateDir(const std::string& name) {
	int ret = mkdir(name.c_str(), 0755);
	return ret;
}

int GetTestDirectory(std::string* result) {
	const char* env = getenv("TEST_TMPDIR");
	if (env && env[0] != '\0') {
		*result = env;
	} else {
		char buf[100];
		snprintf(buf, sizeof(buf), "/tmp/test-%d", int(geteuid()));
		*result = buf;
	}
	// Directory may already exist
	CreateDir(*result);
	return 0;
}
std::string TmpDir() {
	std::string dir;
	GetTestDirectory(&dir);
	return dir;
}

}
