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
	float x, y;
	float width, height;

	Area(float w, float h) :
		x(std::numeric_limits<float>::quiet_NaN()),
		y(std::numeric_limits<float>::quiet_NaN()),
		width(w), height(h)
	{}

	Area(float x, float y, float w, float h) :
		x(x), y(y), width(w), height(h)
	{}
};

struct Name : public std::string {
	Name(std::string&& s) : std::string(std::move(s)) {}
};

} // namespace widget
