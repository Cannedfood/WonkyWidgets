#include "../include/wwidget/Attributes.hpp"

#include <algorithm>
extern "C" {
	#include <memory.h>
}

namespace wwidget {

PreferredSize::PreferredSize() :
	min(0), pref(20), max(std::numeric_limits<float>::infinity())
{}
PreferredSize::PreferredSize(Size const& pref) :
	min(0), pref(pref), max(std::numeric_limits<float>::infinity())
{}
PreferredSize::PreferredSize(Size const& min, Size const& pref) :
	min(min), pref(pref), max(std::numeric_limits<float>::infinity())
{}
PreferredSize::PreferredSize(Size const& min, Size const& pref, Size const& max) :
	min(min), pref(pref), max(max)
{}
PreferredSize::PreferredSize(float prefx, float prefy) :
	PreferredSize(Size(prefx, prefy))
{}
PreferredSize::PreferredSize(
	float minx, float prefx, float maxx,
	float miny, float prefy, float maxy) :
	min(minx, miny),
	pref(prefx, prefy),
	max(maxx, maxy)
{}

PreferredSize PreferredSize::Zero() {
	return {{}, {}, {}};
}

PreferredSize PreferredSize::MinMaxAccumulator() {
	return PreferredSize(Size(0.f));
}

void PreferredSize::include(PreferredSize const& other, float xoff, float yoff) {
	min.x  = std::max(min.x,  other.min.x  + xoff);
	min.y  = std::max(min.y,  other.min.y  + yoff);
	max.x  = std::min(max.x,  other.max.x  + xoff);
	max.y  = std::min(max.y,  other.max.y  + yoff);
	pref.x = std::max(pref.x, other.pref.x + xoff);
	pref.y = std::max(pref.y, other.pref.y + yoff);
}

void PreferredSize::sanitize() {
	if(min.x > max.x) max.x = min.x;
	if(min.y > max.y) max.y = min.y;
	pref.x = std::clamp(pref.x, min.x, max.x);
	pref.y = std::clamp(pref.y, min.y, max.y);
}

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
TinyString::TinyString(const char* s, size_t len) :
	TinyString()
{
	reset(s, len);
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
	char* data = new char[len + 1];
	memcpy(data, s, len);
	data[len] = '\0';

	if(mData != empty_string) delete[] mData;
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
