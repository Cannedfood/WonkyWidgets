#pragma once

#include "../Widget.hpp"

#include <functional>

namespace wwidget {

class Slider : public Widget {
	bool  mPressed;
	float mValue;
	float mScale;
	float mOffset;
	float mExponent;
	std::function<void()> mValueCallback;

	float handleSize() const noexcept;
	float positionToValue(float x) const noexcept;
	float valueToPosition(float x) const noexcept;
protected:
	bool onFocus(bool b, float strength) override;
	void onDrawBackground(Canvas& canvas) override;
	void on(Scroll  const& scroll) override;
	void on(Click   const& click) override;
	void on(Dragged const& click) override;

public:
	Slider();
	Slider(Widget* addTo);
	~Slider() override;



	inline float scale()    const noexcept { return mScale; }
	inline float offset()   const noexcept { return mOffset; }
	inline float value()    const noexcept { return mValue; }
	inline float exponent() const noexcept { return mExponent; }
	inline auto const& valueCallback() const noexcept { return mValueCallback; }

	Slider* value (float f);
	Slider* scale (float f);
	Slider* exponent(float f);
	Slider* offset(float f);
	Slider* range (float min, float max);
	Slider* range (float min, float max, float exponent);
	Slider* valueCallback(std::function<void()> fn);

	template<class C> std::enable_if_t<std::is_invocable_v<C, Slider*>,
	Slider*> valueCallback(C&& c) {
		return valueCallback(std::function<void()>(
			[this, cc = std::forward<C>(c)]() -> void {
				cc(this);
			}
		));
	}
	template<class C> std::enable_if_t<std::is_invocable_v<C, Slider*, float>,
	Slider*> valueCallback(C&& c) {
		return valueCallback(std::function<void()>(
			[this, cc = std::forward<C>(c)]() -> void {
				cc(this, this->value());
			}
		));
	}
	template<class C> std::enable_if_t<std::is_invocable_v<C, float>,
	Slider*> valueCallback(C&& c) {
		return valueCallback(std::function<void()>(
			[this, cc = std::forward<C>(c)]() -> void {
				cc(this->value());
			}
		));
	}
};

} // namespace wwidget
