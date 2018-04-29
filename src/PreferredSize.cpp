#include "../include/wwidget/PreferredSize.hpp"

namespace wwidget {

PreferredSize::PreferredSize() :
	minx(0), maxx(std::numeric_limits<float>::infinity()), prefx(20),
	miny(0), maxy(std::numeric_limits<float>::infinity()), prefy(20)
{}

PreferredSize PreferredSize::Zero() {
	PreferredSize info;
	info.minx = info.maxx = info.prefx = 0;
	info.miny = info.maxy = info.prefy = 0;
	return info;
}

PreferredSize PreferredSize::MinMaxAccumulator() {
	PreferredSize info;
	info.minx   = info.miny = 0;
	info.maxx   = info.maxy = std::numeric_limits<float>::infinity();
	info.prefx  = info.prefy = 0;
	return info;
}

void PreferredSize::include(PreferredSize const& other, float xoff, float yoff) {
	minx  = std::max(minx,  other.minx  + xoff);
	miny  = std::max(miny,  other.miny  + yoff);
	maxx  = std::min(maxx,  other.maxx  + xoff);
	maxy  = std::min(maxy,  other.maxy  + yoff);
	prefx = std::max(prefx, other.prefx + xoff);
	prefy = std::max(prefy, other.prefy + yoff);
}

void PreferredSize::sanitize() {
	if(minx > maxx) maxx = minx;
	if(miny > maxy) maxy = miny;
	prefx = std::clamp(prefx, minx, maxx);
	prefy = std::clamp(prefy, miny, maxy);
}

} // namespace wwidget
