#include "../../include/widget/widget/List.hpp"

namespace widget {

List::List() :
	Widget()
{}
List::~List() {}

void List::onCalculateLayout(LayoutInfo& info) {
	info.minx = info.maxx = info.prefx = 0;
	info.miny = info.maxy = info.prefy = 0;
	eachChild([&](Widget* w) {
		LayoutInfo subInfo;
		w->getLayoutInfo(subInfo);
		info.minx  += subInfo.minx;
		info.maxx  += subInfo.maxx;
		info.prefx += subInfo.prefx;
		info.miny  += subInfo.miny;
		info.maxy  += subInfo.maxy;
		info.prefy += subInfo.prefy;
	});
}
void List::onLayout() {
	float y = 0;
	eachChild([&](Widget* child) {
		child->area().y = y;
		y += child->area().height;
	});
}
void List::onAdd(Widget* child) {
	requestRelayout();
}
void List::onRemove(Widget* child) {
	requestRelayout();
}
void List::onDraw(Canvas& c) {
	
}
} // namespace widget
