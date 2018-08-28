#pragma once

#include "Slider.hpp"

namespace wwidget {

class Knob : public Slider {
public:
	Knob();
	~Knob();

	PreferredSize onCalcPreferredSize(PreferredSize const& constraint) override;

	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;

	void on(Click const& click) override;
	void on(Dragged const& drag) override;
};

} // namespace wwidget
