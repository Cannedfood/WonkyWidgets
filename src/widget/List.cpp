#include "../../include/wwidget/widget/List.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/AttributeCollector.hpp"

#include <cmath>

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

PreferredSize List::onCalcPreferredSize() {
	PreferredSize info = PreferredSize::Zero();
	eachChild([&](Widget* w) {
		auto& subInfo = w->preferredSize();
		if(mFlow & BitFlowHorizontal) {
			info.min.y  = std::max(info.min.y, subInfo.min.y);
			info.max.y  = std::min(info.max.y, subInfo.max.y);
			info.pref.y = std::max(info.pref.y, subInfo.pref.y);
			info.min.x  += subInfo.min.x;
			info.max.x  += subInfo.max.x;
			info.pref.x += subInfo.pref.x;
		}
		else {
			info.min.x  = std::max(info.min.x, subInfo.min.x);
			info.max.x  = std::min(info.max.x, subInfo.max.x);
			info.pref.x = std::max(info.pref.x, subInfo.pref.x);
			info.min.y  += subInfo.min.y;
			info.max.y  += subInfo.max.y;
			info.pref.y += subInfo.pref.y;
		}
	});
	if(info.max.x == 0) info.max.x = std::numeric_limits<float>::infinity();
	if(info.max.y == 0) info.max.y = std::numeric_limits<float>::infinity();
	info.sanitize();

	totalLength(mFlow & BitFlowHorizontal ? info.pref.x : info.pref.y);

	return info;
}
void List::onLayout() {
	using namespace std;

	float pos;
	switch(mFlow & (BitFlowHorizontal | BitFlowInvert)) {
		default:
		case FlowDown:  pos = 0; break;
		case FlowRight: pos = 0; break;
		case FlowUp:    pos = height(); break;
		case FlowLeft:  pos = width();  break;
	}

	if(practicallyScrollable())
		pos -= mScrollOffset;

	eachChild([&](Widget* child) {
		auto& info = child->preferredSize();

		if(mFlow & BitFlowHorizontal) {
			child->size(
				(child->nextSibling() == nullptr && child->alignx() == AlignFill) ?
					std::min(info.max.x, width() - pos) : info.pref.x,
				(child->aligny() == AlignFill) ?
					min(info.max.y, height()) : min(info.pref.y, height())
			);
		}
		else {
			child->size(
				(child->alignx() == AlignFill) ?
					min(info.max.x, width()) : min(info.pref.x, width()),
				((child->nextSibling() == nullptr) && (child->aligny() == AlignFill)) ?
					std::min(info.max.y, height() - pos) : info.pref.y
			);
		}

		float alignx = GetAlignmentX(child, 0, width());
		float aligny = GetAlignmentY(child, 0, height());

		switch(mFlow & (BitFlowHorizontal | BitFlowInvert)) {
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

constexpr static
float sliderWidth = 4;
constexpr static
float sliderMinHeight = 4;

float List::scrollBarHeight() const noexcept {
	return std::max(
		sliderMinHeight,
		length() * length() / totalLength()
	);
}

Rect List::scrollBar() const noexcept {
	if(mFlow & BitFlowHorizontal) {
		return Rect(
			0,
			height() - sliderWidth,
			width(),
			sliderWidth
		);
	}
	else {
		return Rect(
			width() - sliderWidth,
			0,
			sliderWidth,
			height()
		);
	}
}

Rect List::scrollHandle() const noexcept {
	float len = length();

	float sliderHeight = scrollBarHeight();

	// float maxSliderOffset = ;
	float sliderOffset    = (len - sliderHeight) * scrollState();

	bool ivtLine = flow() & BitFlowInvertLine;

	if(flow() & BitFlowHorizontal) {
		return Rect(
			sliderOffset,
			ivtLine ? 0 : height() - sliderWidth,
			sliderHeight,
			sliderWidth
		);
	}
	else {
		return Rect(
			ivtLine ? 0 : width() - sliderWidth,
			sliderOffset,
			sliderWidth,
			sliderHeight
		);
	}
}

void List::onDescendendFocused(Rect const& area, Widget& w) {
	if(!scrollable()) return;

	// Scroll to the position of the focused widget
	// TODO: smarter scrolling
	if(flow() & BitFlowHorizontal) {
		if(area.max.x < 0 || area.min.x > width()) {
			scrollOffset(scrollOffset() - area.min.y);
		}
	}
	else {
		float over_top = std::max(-area.min.y, 0.f);
		// float over_bot = std::max(area.max.y - height(), 0.f);

		if(over_top > 0) {
			scrollOffset(scrollOffset() - over_top);
		}
	}
}
bool List::onFocus(bool b, FocusType type) {
	return true;
}
void List::on(Click const& click) {
	if(!practicallyScrollable()) return;
	if(focused() || scrollBar().contains(click.position)) {
		click.handled = true;
		if(click.down()) {
			requestFocus();
			scrollOffset(click.position);
		}
		else {
			removeFocus();
		}
	}
}
void List::on(Dragged const& drag) {
	if(!practicallyScrollable()) return;
	if(focused()) {
		drag.handled = true;

		scrollOffset(drag.position);
	}
}

void List::on(Scroll const& scroll) {
	if(!practicallyScrollable()) return;
	if(!scroll.upwards()) return;
	float f;
	if(mFlow & BitFlowHorizontal)
		f = scroll.pixels_x;
	else
		f = scroll.pixels_y;

	float old_scroll_offset = scrollOffset();
	scrollOffset(old_scroll_offset - f);

	if(old_scroll_offset != scrollOffset()) {
		scroll.handled = true;
	}
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
	// TODO: check hovered()
	if(practicallyScrollable()) {
		if(focused()) {
			auto bar = scrollBar();
			c.fillColor(Color::black())
			 .rect(bar, bar.width() * .5f)
			 .fill();
		}

		auto handle = scrollHandle();
		c.fillColor(rgba(255, 255, 255, .19f))
		 .rect(handle, handle.width() * .5f)
		 .fill();
	}
}
bool List::setAttribute(std::string_view name, std::string const& value) {
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
	if(collector.startSection("wwidget::List")) {
		collector("flow", flow(), Flow::FlowDownRight);
		collector("scrollable", mScrollable, true);
		collector.endSection();
	}
	Widget::getAttributes(collector);
}
List* List::flow(Flow f) {
	if(flow() != f) {
		mFlow = f;

		bool orientationChange = bool(flow() & BitFlowHorizontal) == bool(f & BitFlowHorizontal);

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
	f = std::clamp(f, 0.f, maxScrollOffset());
	if(f != mScrollOffset) {
		mScrollOffset = f;
		requestRelayout();
	}
	return this;
}
List* List::scrollOffset(Point cursor_pos) {
	float len = length();
	float barHeight = scrollBarHeight();
	if(flow() & BitFlowHorizontal)
		scrollState((cursor_pos.x - barHeight * .5f) / (len - barHeight));
	else
		scrollState((cursor_pos.y - barHeight * .5f) / (len - barHeight));
	return this;
}
float List::scrollState() const noexcept {
	if(!mScrollable) return 0;
	return scrollOffset() / maxScrollOffset();
}
List* List::scrollState(float f) {
	scrollOffset(maxScrollOffset() * std::clamp(f, 0.f, 1.f));
	return this;
}

float List::totalLength() const {
	return mTotalLength;
}
List* List::totalLength(float f) {
	mTotalLength = f;
	return this;
}
float List::length() const {
	return (flow() & BitFlowHorizontal) ? width() : height();
}
float List::maxScrollOffset() const {
	return std::max(0.f, totalLength() - length());
}

} // namespace wwidget
