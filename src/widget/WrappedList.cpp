#include "../../include/wwidget/widget/WrappedList.hpp"

#include <algorithm>

namespace wwidget {

WrappedList::WrappedList() :
	List()
{
	scrollable(true);
	align(AlignFill);
}
WrappedList::WrappedList(Widget* addTo) :
	WrappedList()
{
	addTo->add(*this);
}

PreferredSize WrappedList::onCalcPreferredSize(PreferredSize const& constraint) {
	PreferredSize result;
	result.min.x  = result.min.y  = 0;
	result.max.x  = result.max.y  = 0;
	result.pref.x = result.pref.y = 0;

	float width_sum = 0.f;
	size_t child_count = 0;
	eachChild([&](shared<Widget> w) {
		child_count++;

		auto& size = w->preferredSize(constraint);
		result.min.x   = std::max(result.min.x, size.min.x + w->padX());
		result.min.y   = std::max(result.min.y, size.min.y + w->padY());
		result.max.x  += size.max.x;
		result.max.y  += size.max.y;

		if(flow() & BitFlowHorizontal) {
			result.pref.x += size.pref.x + w->padX();
			result.pref.y  = std::max(result.pref.y, size.pref.y + w->padY());
			width_sum += size.pref.y + w->padY();
		}
		else {
			result.pref.y += size.pref.y + w->padY();
			result.pref.x  = std::max(result.pref.x, size.pref.x + w->padX());
			width_sum += size.pref.x + w->padX();
		}
	});

	if(flow() & BitFlowHorizontal) {
		result.pref.y = std::max(result.pref.y, width_sum / child_count * 3);
		result.pref.x /= 3.f;
	}
	else {
		result.pref.x = std::max(result.pref.x, width_sum / child_count * 3);
		result.pref.y /= 3.f;
	}

	result.sanitize();
	return result;
}
void WrappedList::onLayout() {
	// TODO: Fill rows
	float pos = scrollable() ? -scrollOffset() : 0;
	float line_pos = 0;
	float line_height = 0;

	PreferredSize constraint;

	if(flow() & BitFlowHorizontal) {
		constraint.pref.y = size().y;
		constraint.max.y  = size().y;
	}
	else {
		constraint.pref.x = size().x;
		constraint.max.x  = size().x;
	}

	for(Widget* child = children().get(); child; child = child->nextSibling().get()) {
		auto& prefSize = child->preferredSize(constraint);

		if(flow() & BitFlowHorizontal) {
			child->size(
				prefSize.pref.x,
				std::min(height(), prefSize.pref.y));

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
				std::min(width(), prefSize.pref.x),
				prefSize.pref.y);

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

	pos += line_height;
	totalLength(pos + scrollOffset());
}

} // namespace wwidget
