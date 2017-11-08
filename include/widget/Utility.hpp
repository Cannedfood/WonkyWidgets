#pragma once

#include <limits>
#include <string>

namespace widget {

enum Unit { DP, PX, MM, CM };

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

	explicit
	Area(std::string const& value);
	operator std::string();

	bool operator==(const Area& other) const noexcept {
		return
			x == other.x && y == other.y && width == other.width && height == other.height &&
			ux == other.ux && uy == other.uy && uwidth == other.uwidth && uheight == other.uheight;
	}

	bool contains(float pixel_x, float pixel_y) const noexcept {
		return pixel_x >= x && pixel_y >= y && pixel_x <= (x + width) && pixel_y <= (y + height);
	}
};

struct LayoutInfo {
	float minx = 0, maxx = std::numeric_limits<float>::infinity(), prefx = 20;
	float miny = 0, maxy = std::numeric_limits<float>::infinity(), prefy = 20;
	float weight = 1;

	constexpr static
	LayoutInfo Zero() {
		LayoutInfo info;
		info.minx = info.maxx = info.prefx = 0;
		info.miny = info.maxy = info.prefy = 0;
		info.weight = 0;
		return info;
	}

	constexpr static
	LayoutInfo MinMaxAccumulator() {
		LayoutInfo info;
		info.minx   = info.miny = 0;
		info.maxx   = info.maxy = std::numeric_limits<float>::infinity();
		info.prefx  = info.prefy = 0;
		info.weight = 0;
		return info;
	}

	void include(LayoutInfo const& other, float xoff, float yoff) {
		minx  = std::max(minx,  other.minx  + xoff);
		miny  = std::max(miny,  other.miny  + yoff);
		maxx  = std::min(maxx,  other.maxx  + xoff);
		maxy  = std::min(maxy,  other.maxy  + yoff);
		prefx = std::max(prefx, other.prefx + xoff);
		prefy = std::max(prefy, other.prefy + yoff);
	}

	void sanitize() {
		if(minx > maxx) maxx = minx;
		if(miny > maxy) maxy = miny;
		prefx = std::min(std::max(prefx, minx), maxx);
		prefy = std::min(std::max(prefy, miny), maxy);
	}
};

using Size = Area;

struct Name : public std::string {
	Name(std::string&& s) : std::string(std::move(s)) {}
};

} // namespace widget
