#include "../../include/widget/widget/List.hpp"

#include "../../include/widget/Canvas.hpp"
#include "../../include/widget/Attribute.hpp"


namespace widget {

List::List() :
	Widget(),
	mFlow(FlowDown),
	mScrollable(false),
	mScrollOffset(0)
{}
List::~List() {}

void List::onCalculateLayout(LayoutInfo& info) { WIDGET_M_FN_MARKER
	info.minx = info.maxx = info.prefx = 0;
	info.miny = info.maxy = info.prefy = 0;
	eachChild([&](Widget* w) {
		LayoutInfo subInfo;
		w->getLayoutInfo(subInfo);
		if(mFlow & FlowHorizontalBit) {
			info.miny  = std::max(info.miny, subInfo.miny);
			info.maxy  = std::min(info.maxy, subInfo.maxy);
			info.prefy = std::max(info.prefy, subInfo.prefy);
			info.minx  += subInfo.minx;
			info.maxx  += subInfo.maxx;
			info.prefx += subInfo.prefx;
		}
		else {
			info.minx  = std::max(info.minx, subInfo.minx);
			info.maxx  = std::min(info.maxx, subInfo.maxx);
			info.prefx = std::max(info.prefx, subInfo.prefx);
			info.miny  += subInfo.miny;
			info.maxy  += subInfo.maxy;
			info.prefy += subInfo.prefy;
		}
	});
	if(info.maxx == 0) info.maxx = std::numeric_limits<float>::infinity();
	if(info.maxy == 0) info.maxy = std::numeric_limits<float>::infinity();
	info.sanitize();
}
void List::onLayout() { WIDGET_M_FN_MARKER
	float pos;
	switch (mFlow) {
		case FlowDown:
		case FlowRight: pos = 0; break;
		case FlowUp:    pos = height(); break;
		case FlowLeft:  pos = width();  break;
	}

	pos += mScrollOffset;

	eachChild([&](Widget* child) {
		LayoutInfo info;
		child->getLayoutInfo(info);
		if(mFlow & FlowHorizontalBit) {
			child->size(
				info.prefx,
				std::min(info.prefy, height())
			);
		}
		else {
			child->size(
				std::min(info.prefx, width()),
				info.prefy
			);
		}

		switch (mFlow) {
			case FlowRight: {
				child->offset(pos, 0);
				pos += child->width();
			} break;
			case FlowDown: {
				child->offset(0, pos);
				pos += child->height();
			} break;
			case FlowLeft: {
				pos -= child->width();
				child->offset(pos, 0);
			} break;
			case FlowUp: {
				pos -= child->height();
				child->offset(0, pos);
			} break;
		}
	});
}
void List::on(Scroll const& scroll) {
	if(!mScrollable) return;
	if(mFlow & FlowHorizontalBit)
		mScrollOffset += scroll.amountx;
	else
		mScrollOffset += scroll.amounty;
	requestRelayout();
}
void List::onAdd(Widget* child) { WIDGET_M_FN_MARKER
	preferredSizeChanged();
	requestRelayout();
}
void List::onRemove(Widget* child) { WIDGET_M_FN_MARKER
	preferredSizeChanged();
	requestRelayout();
}
void List::onDraw(Canvas& c) {
	c.outlineRect(0, 0, width(), height(), rgb(232, 58, 225));
}
bool List::setAttribute(std::string const& name, std::string const& value) {
	if(name == "flow") {
		if(value.empty()) return false;
		switch (value[0]) {
			case 'u': flow(FlowUp);    return true;
			case 'd': flow(FlowDown);  return true;
			case 'l': flow(FlowLeft);  return true;
			case 'r': flow(FlowRight); return true;
			default: return false;
		}
	}

	if(name == "scrollable") {
		scrollable(value == "true");
		return true;
	}

	return Widget::setAttribute(name, value);
}
void List::getAttributes(AttributeCollectorInterface& collector) {
	Widget::getAttributes(collector);
	switch(mFlow) {
		case FlowUp: collector("flow", "up"); break;
		case FlowDown: collector("flow", "down"); break;
		case FlowLeft: collector("flow", "left"); break;
		case FlowRight: collector("flow", "right"); break;
	}
	collector("scrollable", mScrollable);
}
List* List::flow(Flow f) {
	if(mFlow != f) {
		mFlow = f;

		bool orientationChange = bool(mFlow & FlowHorizontalBit) == bool(f & FlowHorizontalBit);

		if(orientationChange) {
			requestRelayout();
			preferredSizeChanged();
		}
		else {
			requestRelayout();
		}
	}
	return this;
}
List* List::scrollable(bool b) {
	mScrollable = b;
	return this;
}

} // namespace widget
