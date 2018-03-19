#pragma once

#include "../Widget.hpp"

#include <functional>

namespace wwidget {

class Slider : public Widget {
	bool  mPressed;
	float mValue;
	float mScale;
	float mStart;
	float mExponent;
	std::function<void()> mValueCallback;

	float handleSize() const noexcept;
protected:
	float positionToValue(float x) const noexcept;
	float valueToPosition(float x) const noexcept;

	float fractionToValue(float x) const noexcept;
	float valueToFraction(float x) const noexcept;

	float incrementValue(float f, float by) const noexcept;

	bool setAttribute(std::string const& name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface&) override;

	bool onFocus(bool b, float strength) override;
	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;
	void on(Scroll  const& scroll) override;
	void on(Click   const& click) override;
	void on(Dragged const& click) override;

public:
	Slider();
	Slider(Widget* addTo);
	~Slider() override;

	Slider(Slider&&) = delete; // TODO: Make slider movable

	inline float scale()    const noexcept { return mScale; }
	inline float start()    const noexcept { return mStart; }
	inline float value()    const noexcept { return mValue; }
	inline float fraction() const noexcept { return valueToFraction(mValue); }
	inline float exponent() const noexcept { return mExponent; }
	inline auto const& valueCallback() const noexcept { return mValueCallback; }

	Slider* value   (float f);
	Slider* fraction(float f);
	Slider* scale (float f);
	Slider* exponent(float f);
	Slider* start(float f);
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
