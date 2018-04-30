#include "../../include/wwidget/widget/WrappedList.hpp"

#include <algorithm>

namespace wwidget {

WrappedList::WrappedList() :
	List()
{
	align(AlignFill);
}
WrappedList::WrappedList(Widget* addTo) :
	WrappedList()
{
	addTo->add(this);
}

PreferredSize WrappedList::onCalcPreferredSize() {
	PreferredSize result;
	result.minx  = result.miny  = 0;
	result.maxx  = result.maxy  = 0;
	result.prefx = result.prefy = 0;
	eachChild([&](Widget* w) {
		auto& size = w->preferredSize();
		result.minx   = std::max(result.minx, size.minx);
		result.miny   = std::max(result.miny, size.miny);
		result.maxx  += size.maxy;
		result.maxy  += size.maxy;

		if(flow() & BitFlowHorizontal) {
			result.prefx += size.prefy;
			result.prefy  = std::max(result.prefx, size.prefx);
		}
		else {
			result.prefy += size.prefy;
			result.prefx  = std::max(result.prefx, size.prefx);
		}
	});
	result.sanitize();
	return result;
}
void WrappedList::onLayout() {
	// TODO: padding
	// TODO: layout directions other than down
	float pos = 0;
	float line_pos = 0;
	float line_height = 0;

	/*
	for(Widget* line = children(); line; line = line->nextSibling()) {
		// Widget* lineEnd;
		float lineWidth = 0;
		for(Widget* child = line; child; child = line->nextSibling()) {

		}
	}
	*/

	eachChild([&](Widget* child) {
		auto& prefSize = child->preferredSize();
		child->size(std::min(width(), prefSize.prefx), prefSize.prefy);

		if(width() <= child->width() + line_pos) {
			pos += line_height;
			line_pos = 0;
			line_height = 0;
		}

		child->offset(line_pos, pos);

		line_pos    += child->width();
		line_height  = std::max(line_height, child->height());
	});
}

} // namespace wwidget
