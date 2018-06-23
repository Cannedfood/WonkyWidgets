#include "../include/wwidget/Attributes.hpp"

#include <algorithm>
extern "C" {
	#include <memory.h>
}
#include <cctype>

namespace wwidget {

std::string to_string(float f) {
	std::string result = std::to_string(f);
	while(result.size() > 1 && result.back() == '0') result.pop_back();
	if(result.back() == '.') result.pop_back();
	return result;
}

size_t parse_float_vector(std::string_view s, std::string_view* end, size_t max_count, float* to) {

	char* p_end;

	size_t i = 0;

	while(!s.empty() && i < max_count) {
		while(!s.empty() && std::isspace(s.front())) s.remove_prefix(1);
		to[i] = strtof(s.data(), &p_end);
		s.remove_prefix(p_end - s.data());
		++i;
	}

	if(end) *end = s;

	return i;
}

template<>
Flow from_string(std::string_view s, std::string_view* end) {
	switch(s[0]) {
		case 'u': return FlowUp;
		case 'd': return FlowDown;
		case 'l': return FlowLeft;
		case 'r': return FlowRight;
	}
	// TODO: inverted line versions
	throw std::runtime_error("Unknown layout");
}
std::string to_string(Flow const& f) {
	// TODO: fancy names for up left n stuff
	switch(f) {
		case FlowUpLeft: return "ul";
		case FlowUp: return "up";
		case FlowDownLeft: return "dl";
		case FlowDown: return "down";
		case FlowRightUp: return "ru";
		case FlowRight: return "right";
		case FlowLeftUp: return "lu";
		case FlowLeft: return "left";
	}
}

template<>
HalfAlignment from_string<HalfAlignment>(std::string_view s, std::string_view* end) {
	switch(s[0]) {
		case 'f': return AlignFill;
		case 'c': return AlignCenter;
		case 'm': switch(s[1]) {
			case 'i': case 'n': return AlignMin;
			case 'x': case 'a': return AlignMax;
			default: return AlignDefault;
		}
		case 'n': case '\0': return AlignNone;
		default: return AlignDefault;
	}
	// TODO: use end
}

template<>
Alignment from_string<Alignment>(std::string_view s, std::string_view* end) {
	Alignment result;
	switch(s[0]) {
		default: return from_string<HalfAlignment>(s, end);
		case 'b': result.y = AlignMax; break;
		case 't': result.y = AlignMin; break;
		case 'f': result.y = AlignFill; break;
		case 'c': result.y = AlignCenter; break;
	}
	switch(s[1]) {
		default: return from_string<HalfAlignment>(s, end);
		case 'l': result.x = AlignMin; break;
		case 'r': result.x = AlignMax; break;
		case 'f': result.x = AlignFill; break;
		case 'c': result.x = AlignCenter; break;
	}
	return result;
}
std::string to_string(Alignment const& a) {
	if(a.x == a.y) {
		switch(a.x) {
			case AlignNone:   return "none";
			case AlignMin:    return "min";
			case AlignMax:    return "max";
			case AlignCenter: return "center";
			case AlignFill:   return "fill";
		}
	}
	// TODO: two directional alignment
	return "";
}

template<>
Padding from_string<Padding>(std::string_view s, std::string_view* end) {
	float  pad[4];
	size_t count = parse_float_vector(s, end, std::size(pad), pad);
	switch(count) {
		case 1: return Padding(pad[0]);
		case 2: return Padding(pad[0], pad[1]);
		case 4: return Padding(pad[0], pad[1], pad[2], pad[3]);
		default: throw std::runtime_error("Padding only allows the formats '<all_around>' '<x> <y>' and '<left> <top> <right> <bottom>'");
	}
}
std::string to_string(Padding const& p) {
	if(p.left == p.right && p.top == p.bottom) {
		if(p.left == p.top)
			return to_string(p.top);
		else
			return to_string(p.left) + " " + to_string(p.top);
	}
	else {
		return to_string(p.left) + " " + to_string(p.top) + " " + to_string(p.right) + " " + to_string(p.bottom);
	}
}

template<>
Point from_string<Point>(std::string_view s, std::string_view* end) {
	float p[2];
	size_t count = parse_float_vector(s, end, std::size(p), p);
	switch(count) {
		case 0: return Point();
		case 1: return Point(p[0]);
		case 2: return Point(p[0], p[1]);
		default: throw std::runtime_error("Point only allows the formats '', '<xy>' and '<x> <y>'");
	}
}
std::string to_string(Point const& p) {
	return to_string(p.x) + " " + to_string(p.y);
}

template<>
Color from_string<Color>(std::string_view s, std::string_view* end) {
	// TODO: Hexadecimal n stuff
	float c[4];
	size_t count = parse_float_vector(s, end, std::size(c), c);
	switch(count) {
		case 0: return Color();
		case 1: return Color(c[0]);
		case 3: return Color(c[0], c[1], c[2]);
		case 4: return Color(c[0], c[1], c[2], c[3]);
		default: throw std::runtime_error("Color only allows the formats '', '<gray>', '<r> <g> <b>' and '<r> <g> <b> <a>'");
	}
}
std::string to_string(Color const& c) {
	std::string result = to_string(c.r) + " " + to_string(c.g) + " " + to_string(c.b);
	if(c.a != 1)
		result = to_string(c.a) + " " + result;
	return result;
}

template<>
Rect from_string<Rect>(std::string_view s, std::string_view* end) {
	float f[4];
	size_t count = parse_float_vector(s, end, std::size(f), f);
	switch(count) {
		case 1: return Rect(f[0]);
		case 2: return Rect(f[0], f[1]);
		case 3: return Rect({f[0], f[1]}, Size(f[2]));
		case 4: return Rect(f[0], f[1], f[2], f[3]);
		default: throw std::runtime_error("Padding only allows the formats '<size>', '<width> <height>', '<x> <y> <size>' and '<x> <y> <width> <height>'");
	}
}
std::string to_string(Rect const& rect) {
	return
		to_string(rect.min.x) + " " + to_string(rect.min.y) + " " +
		to_string(rect.width()) + " " + to_string(rect.height());
}


PreferredSize::PreferredSize() :
	min(0), pref(20), max(Size::infinite())
{}
PreferredSize::PreferredSize(Size const& pref) :
	min(0), pref(pref), max(Size::infinite())
{}
PreferredSize::PreferredSize(Size const& min, Size const& pref) :
	min(min), pref(pref), max(Size::infinite())
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
