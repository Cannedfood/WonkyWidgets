#pragma once

#include "Label.hpp"

#include <functional>

namespace wwidget {

class Button : public Widget {
protected:
	bool mPressed;
	std::function<void()> mOnClick;

	void on(Click const& click) override;
	void onCalcPreferredSize(PreferredSize& info) override;
	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;
public:

	Button();
	Button(std::string txt);
	Button(Widget* addTo);
	Button(Widget* addTo, std::string txt);
	~Button();
	Button(Button&&) = delete; // TODO: make movable

	Button*     onClick(std::function<void()> c);
	template<class C> std::enable_if_t<std::is_invocable_v<C, Button*>,
	Button*> onClick(C&& c) {
		onClick(std::function<void()>([this, cc = std::forward<C>(c)]() { cc(this); }));
		return this;
	}

	inline bool pressed() const noexcept { return mPressed; }

	bool setAttribute(std::string const& name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace wwidget
