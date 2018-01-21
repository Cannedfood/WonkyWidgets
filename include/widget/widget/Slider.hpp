#pragma once

#include "../Widget.hpp"

#include <functional>

namespace widget {

class Slider : public Widget {
	bool  mPressed;
	float mValue;
	float mScale;
	float mOffset;
	float mExponent;

	float handleSize() const noexcept;
	float positionToValue(float x) const noexcept;
	float valueToPosition(float x) const noexcept;
protected:
	void onDrawBackground(Canvas& canvas) override;
	void on(Scroll  const& scroll) override;
	void on(Click   const& click) override;
	void on(Dragged const& click) override;

public:
	Slider();
	Slider(Widget* addTo);
	~Slider() override;

	std::function<void(Slider*,float)> valueCallback;

	inline float scale()    const noexcept { return mScale; }
	inline float offset()   const noexcept { return mOffset; }
	inline float value()    const noexcept { return mValue; }
	inline float exponent() const noexcept { return mExponent; }

	Slider* value (float f);
	Slider* scale (float f);
	Slider* exponent(float f);
	Slider* offset(float f);
	Slider* range (float min, float max);
	Slider* range (float min, float max, float exponent);
};

} // namespace widget
