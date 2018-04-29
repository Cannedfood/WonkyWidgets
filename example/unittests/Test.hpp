#pragma once

void test_hint(const char* msg);
bool _test_result(bool success, const char* content, const char* file, int line);

#define expect(a) _test_result((a), #a, __FILE__, __LINE__)
#define expect_eq(a, b) _test_result((a) == (b), #a " == " #b, __FILE__, __LINE__)
#define expect_similar(a, b) _test_result((a) - (b) < 1e-7, #a " ≈ " #b, __FILE__, __LINE__)

#define expect_exception(Exception, Lambda) \
	try { \
		Lambda(); \
		_test_result(false, "Didn't catch expected exception " #Exception, __FILE__, __LINE__); \
	} \
	catch(Exception&) { \
		_test_result(true, "Caught expected exception " #Exception, __FILE__, __LINE__); \
	} \
	catch(std::exception& e) { \
		_test_result(false, "Caught and unexpected exception", __FILE__, __LINE__); \
	}

#define expect_noexcept(Lambda) \
	try { \
		Lambda(); \
		_test_result(true, "No exceptions occurred", __FILE__, __LINE__); \
	} \
	catch(std::exception& e) { \
		_test_result(false, "Caught unexpected exception", __FILE__, __LINE__); \
	}
