#pragma once

#include "Slider.hpp"

namespace wwidget {

class Knob : public Slider {
public:
	Knob();
	~Knob();

	PreferredSize onCalcPreferredSize() override;

	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;

	bool onFocus(bool b, float strength) override;

	void on(Scroll const& scroll) override;
	void on(Click const& click) override;
	void on(Dragged const& drag) override;
};

} // namespace wwidget
