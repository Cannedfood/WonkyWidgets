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
		result.minx   = std::max(result.minx, size.minx + w->padX());
		result.miny   = std::max(result.miny, size.miny + w->padY());
		result.maxx  += size.maxy;
		result.maxy  += size.maxy;

		if(flow() & BitFlowHorizontal) {
			result.prefx += size.prefx + w->padX();
			result.prefy  = std::max(result.prefy, size.prefy + w->padY());
		}
		else {
			result.prefy += size.prefy + w->padY();
			result.prefx  = std::max(result.prefx, size.prefx + w->padX());
		}
	});
	result.sanitize();
	return result;
}
void WrappedList::onLayout() {
	// TODO: Fill rows
	float pos = 0;
	float line_pos = 0;
	float line_height = 0;

	for(Widget* child = children(); child; child = child->nextSibling()) {
		auto& prefSize = child->preferredSize();

		if(flow() & BitFlowHorizontal) {
			child->size(
				prefSize.prefx,
				std::min(height(), prefSize.prefy));

			if(height() <= (line_pos + child->height() + child->padY())) {
				pos         += line_height;
				line_pos     = 0;
				line_height  = 0;
			}

			if(flow() & BitFlowInvertLine) {
				child->offset(
					pos + child->padLeft(),
					height() - (line_pos + child->height() + child->padBottom()));
			}
			else {
				child->offset(
					pos + child->padLeft(),
					line_pos + child->padTop());
			}

			line_pos    += child->height() + child->padY();
			line_height  = std::max(line_height, child->width() + child->padX());
		}
		else {
			child->size(
				std::min(width(), prefSize.prefx),
				prefSize.prefy);

			if(width() <= (line_pos + child->width() + child->padX())) {
				pos         += line_height;
				line_pos     = 0;
				line_height  = 0;
			}

			if(flow() & BitFlowInvertLine) {
				child->offset(
					width() - (line_pos + child->width() + child->padRight()),
					pos + child->padTop());
			}
			else {
				child->offset(
					line_pos + child->padLeft(),
					pos + child->padTop());
			}

			line_pos    += child->width() + child->padX();
			line_height  = std::max(line_height, child->height() + child->padY());
		}
	}
}

} // namespace wwidget
