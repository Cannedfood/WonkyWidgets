#pragma once

#include "../Widget.hpp"

#include <functional>

namespace widget {

class Slider : public Widget {
	float mValue;
	float mScale;
	float mOffset;

	float handleSize() const noexcept;
	float positionToValue(float x) const noexcept;
protected:
	void onDrawBackground(Canvas& canvas) override;
	void on(Scroll  const& scroll) override;
	void on(Click   const& click) override;
	void on(Dragged const& click) override;

public:
	Slider();
	~Slider() override;

	std::function<void(Slider*,float)> valueCallback;

	inline float scale()       const noexcept { return mScale; }
	inline float offset()      const noexcept { return mOffset; }
	inline float value()       const noexcept { return mValue; }

	Slider* value (float f);
	Slider* scale (float f);
	Slider* offset(float f);
	Slider* range (float min, float max);
};

} // namespace widget
