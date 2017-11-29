#pragma once

#include <limits>
#include <string>

namespace widget {

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

struct Name : public std::string {
	Name(std::string&& s) : std::string(std::move(s)) {}
};

} // namespace widget
