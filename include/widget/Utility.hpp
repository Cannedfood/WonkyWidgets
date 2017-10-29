#pragma once

#include <limits>
#include <string>

namespace widget {

enum Unit { DP, PX, MM, CM };

struct Padding {
	float left, up, right, down;
	Unit  uleft, uup, uright, udown;

	constexpr inline
	Padding(
		float left, Unit uleft,
		float up, Unit uup,
		float right, Unit uright,
		float down, Unit udown) :
		left(left), up(up), right(right), down(down),
		uleft(uleft), uup(uup), uright(uright), udown(udown)
	{}

	constexpr inline
	Padding(float left, float up, float right, float down, Unit u = PX) :
		Padding(left, u, up, u, right, u, down, u)
	{}

	constexpr inline
	Padding(float x, Unit ux, float y, Unit uy) :
		Padding(x, ux, y, uy, x, ux, y, uy)
	{}

	constexpr inline
	Padding(float x, float y, Unit u = PX) :
		Padding(x, u, y, u, x, u, y, u)
	{}

	constexpr inline
	Padding(float f = 0, Unit u = PX) :
		Padding(f, u, f, u)
	{}
};

struct Area {
	float x, y, width, height;
	Unit  ux, uy, uwidth, uheight;

	constexpr inline
	Area(
		float x, Unit ux,
		float y, Unit uy,
		float width, Unit uwidth,
		float height, Unit uheight) :
		x(x), y(y), width(width), height(height),
		ux(ux), uy(uy), uwidth(uwidth), uheight(uheight)
	{}

	constexpr inline
	Area(float x, float y, float width, float height, Unit u = PX) :
		Area(x, u, y, u, width, u, height, u)
	{}

	constexpr inline
	Area(float width, Unit uwidth, float height, Unit uheight) :
		Area(0, PX, 0, PX, width, uwidth, height, uheight)
	{}

	constexpr inline
	Area(float width, float height, Unit u = PX) :
		Area(0, PX, 0, PX, width, u, height, u)
	{}

	constexpr inline
	Area(float f = 0, Unit u = PX) :
		Area(f, f, u)
	{}

	bool contains(float pixel_x, float pixel_y) {
		return pixel_x >= x && pixel_y >= y && pixel_x <= x + width && pixel_y <= y + width;
	}
};

using Size = Area;

struct Name : public std::string {
	Name(std::string&& s) : std::string(std::move(s)) {}
};

} // namespace widget
