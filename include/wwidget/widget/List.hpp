#pragma once

#include "../Widget.hpp"

namespace wwidget {

class List : public Widget {
private:
	Flow  mFlow;
	bool  mScrollable;

	float mScrollOffset;
	float mTotalLength;
protected:
	float maxScrollOffset() const;
	float totalLength() const;
	List& totalLength(float f);
	float length() const;
	void onAdd(Widget& child) override;
	void onRemove(Widget& child) override;
	PreferredSize onCalcPreferredSize(PreferredSize const& constraints) override;
	void onLayout() override;
	void onDraw(Canvas& c) override;

	void on(Scroll const& scroll) override;

	float scrollBarHeight() const noexcept;
	Rect scrollBar() const noexcept;
	Rect scrollHandle() const noexcept;

	bool onFocus(bool b, FocusType type) override;
	void on(Click const& click) override;
	void on(Dragged const& drag) override;
public:
	List();
	List(Widget* addTo);
	~List();

	void onDescendendFocused(Rect const& area, Widget& w) override;

	bool setAttribute(std::string_view name, Attribute const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;

	bool flowsRight() const noexcept { return mFlow == FlowRight; }
	bool flowsLeft()  const noexcept { return mFlow == FlowLeft; }
	bool flowsUp()    const noexcept { return mFlow == FlowUp; }
	bool flowsDown()  const noexcept { return mFlow == FlowDown; }
	Flow flow()       const noexcept { return mFlow; }
	List& flow(Flow flow);

	bool  practicallyScrollable() const noexcept { return mScrollable && maxScrollOffset() != 0; }
	bool  scrollable() const noexcept { return mScrollable; }
	List& scrollable(bool b);

	float scrollOffset() const noexcept { return mScrollOffset; }
	List& scrollOffset(float f);
	List& scrollOffset(Point cursor_pos); // Used when dragging the slider

	float scrollState() const noexcept;
	List& scrollState(float f);
};

} // namespace wwidget
