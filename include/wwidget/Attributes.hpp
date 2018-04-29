#pragma once

namespace wwidget {

// =============================================================
// == Padding, Alignment, Flow =============================================
// =============================================================

/// Used in the Flow enum
enum FlowBits {
	BitFlowInvert     = 1,
	BitFlowHorizontal = 2,
};

/// Specifies the direction of lists and sliders
enum Flow {
	FlowDown  = 0,
	FlowUp    = BitFlowInvert,
	FlowRight = BitFlowHorizontal,
	FlowLeft  = BitFlowHorizontal | BitFlowInvert
};

/// How the widget will resize relative to the parent
enum HalfAlignment {
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
