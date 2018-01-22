#pragma once

#include <limits>
#include <algorithm>

namespace wwidget {

struct PreferredSize {
	float minx, maxx, prefx;
	float miny, maxy, prefy;

	PreferredSize();

	static PreferredSize Zero();
	static PreferredSize MinMaxAccumulator();

	void include(PreferredSize const& other, float xoff, float yoff);
	void sanitize();
};

} // namespace wwidget
