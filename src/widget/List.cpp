#include "../../include/wwidget/widget/List.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/Attribute.hpp"


namespace wwidget {

List::List() :
	Widget(),
	mFlow(FlowDown),
	mScrollable(false),
	mScrollOffset(0),
	mTotalLength(0)
{}
List::List(Widget* addTo) : List() { addTo->add(this); }
List::~List() {}

void List::onCalcPreferredSize(PreferredSize& info) {
	info.minx = info.maxx = info.prefx = 0;
	info.miny = info.maxy = info.prefy = 0;
	eachChild([&](Widget* w) {
		PreferredSize subInfo;
		w->getPreferredSize(subInfo);
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

	mTotalLength = mFlow & FlowHorizontalBit ? info.prefx : info.prefy;
}
void List::onLayout() {
	using namespace std;

	float pos;
	switch (mFlow) {
		case FlowDown:
		case FlowRight: pos = 0; break;
		case FlowUp:    pos = height(); break;
		case FlowLeft:  pos = width();  break;
	}

	pos -= mScrollOffset;

	eachChild([&](Widget* child) {
		PreferredSize info;
		child->getPreferredSize(info);

		if(mFlow & FlowHorizontalBit) {
			child->size(
				(child->nextSibling() == nullptr && child->alignx() == AlignFill) ?
					std::min(info.maxx, width() - pos) : info.prefx,
				(child->aligny() == AlignFill) ?
					min(info.maxy, height()) : min(info.prefy, height())
			);
		}
		else {
			child->size(
				(child->alignx() == AlignFill) ?
					min(info.maxx, width()) : min(info.prefx, width()),
				((child->nextSibling() == nullptr) && (child->aligny() == AlignFill)) ?
					std::min(info.maxy, height() - pos) : info.prefy
			);
		}

		float alignx = GetAlignmentX(child, 0, width());
		float aligny = GetAlignmentY(child, 0, height());

		switch (mFlow) {
			case FlowRight: {
				child->offset(pos, aligny);
				pos += child->paddedWidth();
			} break;
			case FlowDown: {
				child->offset(alignx, pos);
				pos += child->paddedHeight();
			} break;
			case FlowLeft: {
				pos -= child->paddedWidth();
				child->offset(pos, aligny);
			} break;
			case FlowUp: {
				pos -= child->paddedHeight();
				child->offset(alignx, pos);
			} break;
		}
	});
}
void List::on(Scroll const& scroll) {
	if(!mScrollable) return;
	float f;
	if(mFlow & FlowHorizontalBit)
		f = scroll.pixels_x;
	else
		f = scroll.pixels_y;
	if(f > 0)
		scroll.handled = true;
	scrollOffset(scrollOffset() - f);
}
void List::onAdd(Widget* child) {
	preferredSizeChanged();
	requestRelayout();
}
void List::onRemove(Widget* child) {
	preferredSizeChanged();
	requestRelayout();
}
void List::onDraw(Canvas& c) {
	// c.outlineRect(0, 0, width(), height(), rgb(232, 58, 225));
	if(mScrollable) {
		float sliderHeight = height() / 20;
		float sliderWidth  = 2;

		float maxSliderOffset = length() - sliderHeight;
		float sliderOffset    = maxSliderOffset * scrollState();

		uint32_t color = rgba(255, 255, 255, .19f);

		if(mFlow & FlowHorizontalBit) {
			c.rect(
				Rect{
					sliderOffset,
					height() - sliderWidth,
					sliderWidth,
					sliderHeight
				},
				color
			);
		}
		else {
			c.rect(
				Rect {
					width() - sliderWidth,
					maxSliderOffset * scrollState(),
					sliderWidth,
					sliderHeight
				},
				color
			);
		}
	}
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
	if(!b) {
		scrollOffset(0);
	}
	return this;
}
List* List::scrollOffset(float f) {
	f = std::max(std::min(f, maxScrollOffset()), 0.f);
	if(f != mScrollOffset) {
		mScrollOffset = f;
		requestRelayout();
	}
	return this;
}
float List::scrollState() const noexcept {
	if(!mScrollable) return 0;
	return scrollOffset() / maxScrollOffset();
}
List* List::scrollState(float f) {
	if(f >= 0) {
		scrollOffset(maxScrollOffset() * f);
	}
	return this;
}

float List::totalLength() const {
	return mTotalLength;
}
float List::length() const {
	return (mFlow & FlowHorizontalBit ? width() : height());
}
float List::maxScrollOffset() const {
	return std::max(0.f, totalLength() - length());
}

} // namespace wwidget
