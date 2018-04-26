#include "Demangle.hpp"

#ifdef __GNUC__
extern "C" {
	#include <cxxabi.h>
	#include <assert.h>
}
std::string demangle(const char* name) {
	int status;
	const char* realname = abi::__cxa_demangle(name, 0, 0, &status);
	assert(status == 0);
	std::string result = realname;
	::free((void*)realname);
	return result;
}
#else
std::string demangle(const char* name) {
	return name;
}
#endif
