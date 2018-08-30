#include "Test.hpp"

#include <cstdio>

static
const char* _test_hint = "";

void test_hint(const char* msg) {
	_test_hint = msg;
}

bool _test_result(bool success, const char* content, const char* file, int line) {
	if(!success)
		printf("!> Failed: %s:%i: (%s) %s\n", file, line, content, _test_hint);
	// else
	// 	printf("|  Success: %s:%i: (%s) %s\n", file, line, content, _test_hint);
	_test_hint = "";
	return success;
}

void testParsing();
void testWidgetTreeOps();
void printSizes();

int main(int argc, char const** argv) {
	printSizes();
	testWidgetTreeOps();
	// testParsing();
	return 0;
}

#include <wwidget/Widget.hpp>

void printSizes() {
	#define PRINT_SIZE(TYPE) printf("sizeof(" #TYPE ") = %zu\n", sizeof(TYPE))

	PRINT_SIZE(wwidget::Widget);
	PRINT_SIZE(wwidget::Alignment);
	PRINT_SIZE(wwidget::PreferredSize);
}
