#pragma once

#include "../Widget.hpp"

namespace widget {

class List : public Widget {
public:
	constexpr static uint32_t FlowHorizontalBit = 1;
	constexpr static uint32_t FlowNegativeBit   = 2;

	enum Flow { // TODO: find a better word for flow
		FlowDown  = 0,
		FlowUp    = FlowNegativeBit,
		FlowRight = FlowHorizontalBit,
		FlowLeft  = FlowHorizontalBit | FlowNegativeBit
	};

private:
	Flow mFlow;

protected:
	void onAdd(Widget* child) override;
	void onRemove(Widget* child) override;
	void onCalculateLayout(LayoutInfo& info) override;
	void onLayout() override;
	void onDraw(Canvas& c) override;
public:
	List();
	~List();

	bool setAttribute(std::string const& name, std::string const& value) override;

	bool flowsRight() const noexcept { return mFlow == FlowRight; }
	bool flowsLeft()  const noexcept { return mFlow == FlowLeft; }
	bool flowsUp()    const noexcept { return mFlow == FlowUp; }
	bool flowsDown()  const noexcept { return mFlow == FlowDown; }
	Flow flow()       const noexcept { return mFlow; }
	List* flow(Flow flow);
};

} // namespace widget
