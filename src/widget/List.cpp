#include "../../include/widget/widget/List.hpp"

#include "../../include/widget/Canvas.hpp"


namespace widget {

List::List() :
	Widget()
{}
List::~List() {}

void List::onCalculateLayout(LayoutInfo& info) { WIDGET_M_FN_MARKER
	info.minx = info.maxx = info.prefx = 0;
	info.miny = info.maxy = info.prefy = 0;
	eachChild([&](Widget* w) {
		LayoutInfo subInfo;
		w->getLayoutInfo(subInfo);
		info.minx  = std::max(info.minx, subInfo.minx);
		info.maxx  = std::min(info.maxx, subInfo.maxx);
		info.prefx = std::max(info.prefx, subInfo.prefx);
		info.miny  += subInfo.miny;
		info.maxy  += subInfo.maxy;
		info.prefy += subInfo.prefy;
	});
	if(info.maxx == 0) info.maxx = std::numeric_limits<float>::infinity();
	if(info.maxy == 0) info.maxy = std::numeric_limits<float>::infinity();
	info.sanitize();
}
void List::onLayout() { WIDGET_M_FN_MARKER
	float y = 0;

	eachChild([&](Widget* child) {
		LayoutInfo info;
		child->getLayoutInfo(info);
		child->size(
			std::min(info.prefx, area().width),
			info.prefy
		);
		child->position(child->area().x, y);
		y += child->area().height;
	});
}
void List::onAdd(Widget* child) { WIDGET_M_FN_MARKER
	requestRelayout();
}
void List::onRemove(Widget* child) { WIDGET_M_FN_MARKER
	requestRelayout();
}
void List::onDraw(Canvas& c) {
	c.outlineRect(0, 0, area().width, area().height, rgb(232, 58, 225));
}
} // namespace widget
