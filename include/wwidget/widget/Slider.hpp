#pragma once

#include "../Widget.hpp"

#include <functional>

namespace wwidget {

class Slider : public Widget {
	bool  mPressed;
	double mValue;
	double mStart, mScale, mExponent;
	std::function<void()> mValueCallback;

	float handleSize() const noexcept;
protected:
	double positionToValue(float x) const noexcept;
	float  valueToPosition(double x) const noexcept;

	double fractionToValue(double x) const noexcept;
	double valueToFraction(double x) const noexcept;

	bool setAttribute(std::string_view name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface&) override;

	bool onFocus(bool b, FocusType type) override;
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

	inline double scale()    const noexcept { return mScale; }
	inline double start()    const noexcept { return mStart; }
	inline double value()    const noexcept { return mValue; }
	inline double fraction() const noexcept { return valueToFraction(mValue); }
	inline double exponent() const noexcept { return mExponent; }
	inline auto const& valueCallback() const noexcept { return mValueCallback; }

	Slider* value   (double f);
	Slider* fraction(double f);
	Slider* scale (double f);
	Slider* exponent(double f);
	Slider* start(double f);
	Slider* range (double min, double max);
	Slider* range (double min, double max, double exponent);
	Slider* valueCallback(std::function<void()> fn);

	template<class C> std::enable_if_t<std::is_invocable_v<C, Slider*>,
	Slider*> valueCallback(C&& c) {
		return valueCallback(std::function<void()>(
			[this, cc = std::forward<C>(c)]() -> void {
				cc(this);
			}
		));
	}
	template<class C> std::enable_if_t<std::is_invocable_v<C, Slider*, double>,
	Slider*> valueCallback(C&& c) {
		return valueCallback(std::function<void()>(
			[this, cc = std::forward<C>(c)]() -> void {
				cc(this, this->value());
			}
		));
	}
	template<class C> std::enable_if_t<std::is_invocable_v<C, double>,
	Slider*> valueCallback(C&& c) {
		return valueCallback(std::function<void()>(
			[this, cc = std::forward<C>(c)]() -> void {
				cc(this->value());
			}
		));
	}
};

} // namespace wwidget
