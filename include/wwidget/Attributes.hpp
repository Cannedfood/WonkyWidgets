#pragma once

#include <cstring>
#include <cstddef>
#include <cinttypes>

#include <string>
#include <functional>

#pragma pack(push, 1) // Make everything as small as possible

namespace wwidget {

// =============================================================
// == Padding, Alignment, Flow =============================================
// =============================================================

/// Used in the Flow enum
enum FlowBits {
	BitFlowInvert     = 1,
	BitFlowHorizontal = 2,
	BitFlowInvertLine = 4
};

/// Specifies the direction of lists and sliders
enum Flow : unsigned char {
	FlowDown  = 0,
	FlowUp    = BitFlowInvert,
	FlowRight = BitFlowHorizontal,
	FlowLeft  = BitFlowHorizontal | BitFlowInvert,

	/// Mostly for WrappedList
	FlowUpLeft    = FlowUp | BitFlowInvertLine,
	FlowUpRight   = FlowUp,
	FlowDownLeft  = FlowDown | BitFlowInvertLine,
	FlowDownRight = FlowDown,
	FlowRightUp   = FlowRight | BitFlowInvertLine,
	FlowRightDown = FlowRight,
	FlowLeftUp    = FlowLeft | BitFlowInvertLine,
	FlowLeftDown  = FlowLeft,
};

/// How the widget will resize relative to the parent
enum HalfAlignment : unsigned char {
	AlignNone,   //!< Do not resize automatically
	AlignCenter, //!< Use preferred size (usually: center in parent)
	AlignMax,    //!< Use preferred size (usually: align to bottom/right)
	AlignMin,    //!< Use preferred size (usually: align to top/left)
	AlignFill,   //!< Take up as much space as possible
	AlignDefault = AlignMin
};

struct Alignment {
	HalfAlignment x, y;
	// implicit
	constexpr inline Alignment(HalfAlignment a = AlignDefault) : x(a), y(a) {}
	constexpr inline Alignment(HalfAlignment x, HalfAlignment y) : x(x), y(y) {}
};

struct Padding {
	float left, top, right, bottom;
	constexpr inline
	Padding(float left, float top, float right, float bottom) :
		left(left), top(top), right(right), bottom(bottom) {}
	constexpr inline
	Padding(float pad_x, float pad_y) :
		Padding(pad_x, pad_y, pad_x, pad_y) {}
	constexpr inline
	Padding(float all_around) :
		Padding(all_around, all_around) {}
	constexpr inline
	float vertical() const noexcept { return top + bottom; }
	constexpr inline
	float horizontal() const noexcept { return left + right; }
};

// =============================================================
// == Position and size =============================================
// =============================================================

struct Point {
	union {
		struct { float x, y; };
		float xy[2];
	};
	constexpr inline Point() : x(0), y(0) {}
	constexpr inline Point(float f) : x(f), y(f) {}
	constexpr inline Point(float x, float y) : x(x), y(y) {}
};

struct Offset : Point { using Point::Point; };
struct Size : Point { using Point::Point; };

// =============================================================
// == Color =============================================
// =============================================================

struct Color {
	union {
		struct {
			float r, g, b, a;
		};
		float rgba[4];
	};
	constexpr inline
	Color(float r, float g, float b, float a = 1) noexcept :
		r(r), g(g), b(b), a(a)
	{}
	constexpr inline
	Color(float f = 0) noexcept :
		Color(f, f, f, 1)
	{}
	constexpr inline
	Color(uint32_t color) noexcept :
		r((color >> 16) / 255.f),
		g((color >> 8)  / 255.f),
		b((color >> 0)  / 255.f),
		a((color >> 24) / 255.f)
	{}
	constexpr inline
	operator uint32_t() const noexcept {
		return (uint32_t(0xFF * r) << 16) |
		       (uint32_t(0xFF * g) << 8)  |
		       (uint32_t(0xFF * b) << 0)  |
					 (uint32_t(0xFF * a) << 24);
	}
	constexpr static Color white() noexcept { return {1, 1, 1, 1}; }
	constexpr static Color black() noexcept { return {0, 0, 0, 1}; }
	constexpr static Color gray(float f = .5f) noexcept { return {f, f, f, 1}; }
	constexpr static Color eyecancer1() noexcept { return {1, 0, 1, 1}; }
	constexpr static Color eyecancer2() noexcept { return {0, 1, 0, 1}; }
};

constexpr inline
Color rgba(uint8_t r, uint8_t g, uint8_t b, float a) noexcept {
	return Color(r / 255.f, g / 255.f, b / 255.f, a);
}
constexpr inline
Color rgb(uint8_t r, uint8_t g, uint8_t b) noexcept {
	return Color(r / 255.f, g / 255.f, b / 255.f, 1);
}

struct ForegroundColor : Color { using Color::Color; };
struct BackgroundColor : Color { using Color::Color; };

// =============================================================
// == TinyTypes =============================================
// =============================================================

class TinyString {
	const char* mData;
public:
	TinyString() noexcept;
	TinyString(const char* s);
	TinyString(const char* s, size_t len);
	~TinyString() noexcept;

	TinyString(TinyString&&) noexcept;
	TinyString(TinyString const&) noexcept;
	TinyString& operator=(TinyString&&) noexcept;
	TinyString& operator=(TinyString const&) noexcept;

	void reset(const char* s, size_t len);
	void reset(const char* s);
	void clear();

	bool        empty() const noexcept { return !*mData;}
	size_t      length() const noexcept { return strlen(mData); }
	const char* c_str() const noexcept { return mData; }
	const char* data() const noexcept { return mData; }
	operator const char*() const noexcept { return mData; }

	bool operator<(const char* s) const noexcept { return strcmp(mData, s) < 0; }
	bool operator<=(const char* s) const noexcept { return strcmp(mData, s) <= 0; }
	bool operator>(const char* s) const noexcept { return strcmp(mData, s) > 0; }
	bool operator>=(const char* s) const noexcept { return strcmp(mData, s) >= 0; }
	bool operator==(const char* s) const noexcept { return strcmp(mData, s) == 0; }
};

// =============================================================
// == Other stuff =============================================
// =============================================================

struct Rect {
	Offset min, max;

	constexpr inline
	Rect(float x, float y, float w, float h) :
		min(x, y), max(x + w, y + h)
	{}

	constexpr inline
	Rect(float w, float h) :
		Rect(0, 0, w, h)
	{}

	constexpr inline
	Rect(Offset pos, Size s) :
		min(pos), max(s.x, s.y)
	{}

	constexpr inline
	Rect() :
		min(0, 0), max(0, 0)
	{}

	constexpr static inline
	Rect absolute(float x0, float y0, float x1, float y1) {
		Rect result;
		result.min.x = x0;
		result.min.y = y0;
		result.max.x = x1;
		result.max.y = y1;
		return result;
	}
};

} // namespace wwidget

#pragma pack(pop)

namespace std {

template<>
struct hash<::wwidget::TinyString> {
	size_t operator()(::wwidget::TinyString const& in) const noexcept {
		// Hashing algorithm: FNV-1a

		size_t hash;
		size_t FNV_prime;

		if constexpr(sizeof(size_t) == 4) {
			hash      = 2166136261U;
			FNV_prime = 16777619U;
		}
		else if constexpr(sizeof(size_t) == 8) {
			hash      = 14695981039346656037UL;
			FNV_prime = 1099511628211UL;
		}
		else {
			static_assert("No implementation for this size of size_t");
		}

		const char* s = in.data();
		while(*s) {
			hash ^= *s;
			hash *= FNV_prime;
			s++;
		}

		return hash;
	}
};

}
