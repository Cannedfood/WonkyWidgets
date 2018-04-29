#include "../include/wwidget/Attributes.hpp"

#include <algorithm>
extern "C" {
	#include <memory.h>
}

namespace wwidget {

// =============================================================
// == TinyString =============================================
// =============================================================

static
const char* empty_string = "";

TinyString::TinyString() noexcept :
	mData(empty_string)
{}
TinyString::TinyString(const char* s) :
	TinyString()
{
	reset(s);
}
TinyString::~TinyString() noexcept {
	clear();
}

TinyString::TinyString(TinyString&& s) noexcept :
	mData(s.mData)
{
	s.mData = empty_string;
}
TinyString::TinyString(TinyString const& s) noexcept {
	reset(s.data(), s.length());
}
TinyString& TinyString::operator=(TinyString&& s) noexcept {
	clear();
	std::swap(this->mData, s.mData);
	return *this;
}
TinyString& TinyString::operator=(TinyString const& s) noexcept {
	reset(s.data(), s.length());
	return *this;
}

void TinyString::reset(const char* s, size_t len) {
	if(mData != empty_string) delete[] mData;
	char* data = new char[len + 1];
	memcpy(data, s, len);
	data[len] = '\0';
	mData = data;
}
void TinyString::reset(const char* s) {
	reset(s, strlen(s));
}

void TinyString::clear() {
	if(mData != empty_string) {
		delete[] mData;
		mData = empty_string;
	}
}

} // namespace wwidget
