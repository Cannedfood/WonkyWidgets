#pragma once

#include "../Widget.hpp"

#include <functional>

namespace widget {

class Slider : public Widget {
	float mValue;
	float mScale;
	float mExponent;
protected:
	void onDrawBackground(Canvas& canvas) override;
	void on(Scroll const& scroll) override;
	void on(Click const& click) override;
public:
	Slider();
	~Slider() override;

	std::function<void(Slider*,float)> valueCallback;

	inline float value() const noexcept { return mValue; }
	inline float scale() const noexcept { return mScale; }
	inline float exponent() const noexcept { return mExponent; }
	Slider* value   (float f);
	Slider* scale   (float f);
	Slider* exponent(float f);
};

} // namespace widget
