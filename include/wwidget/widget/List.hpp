#pragma once

#include "../Widget.hpp"

namespace wwidget {

class List : public Widget {
private:
	Flow  mFlow;
	bool  mScrollable;
	float mScrollOffset;
	float mTotalLength;

	float maxScrollOffset() const;
	float totalLength() const;
	float length() const;
protected:
	void onAdd(Widget* child) override;
	void onRemove(Widget* child) override;
	void onCalcPreferredSize(PreferredSize& info) override;
	void onLayout() override;
	void onDraw(Canvas& c) override;
	void on(Scroll const& scroll) override;
public:
	List();
	List(Widget* addTo);
	~List();

	bool setAttribute(std::string const& name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;

	bool flowsRight() const noexcept { return mFlow == FlowRight; }
	bool flowsLeft()  const noexcept { return mFlow == FlowLeft; }
	bool flowsUp()    const noexcept { return mFlow == FlowUp; }
	bool flowsDown()  const noexcept { return mFlow == FlowDown; }
	Flow flow()       const noexcept { return mFlow; }
	List* flow(Flow flow);

	bool scrollable() const noexcept { return mScrollable; }
	List* scrollable(bool b);

	float scrollOffset() const noexcept { return mScrollOffset; }
	List* scrollOffset(float f);

	float scrollState() const noexcept;
	List* scrollState(float f);
};

} // namespace wwidget
