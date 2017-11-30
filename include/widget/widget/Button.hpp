#pragma once

#include "Label.hpp"

#include <functional>

namespace widget {

class Button : public Widget {
protected:
	bool mPressed;

	void on(Click const& click) override;
	void onCalculateLayout(LayoutInfo& info) override;
	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;
public:
	std::function<void(Button*)> onClickCallback;

	Button();
	~Button();

	Button*     text(std::string const& s);
	std::string text();

	inline bool pressed() const noexcept { return mPressed; }

	bool setAttribute(std::string const& name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace widget
