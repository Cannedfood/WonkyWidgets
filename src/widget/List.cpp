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
}
void List::onLayout() { WIDGET_M_FN_MARKER
	float y = 0;

	eachChild([&](Widget* child) {
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
	c.outlineRect(0, 0, area().width, area().height, rgb(58, 59, 232));
}
} // namespace widget
