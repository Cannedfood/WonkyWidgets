#pragma once

#include "Label.hpp"

#include <functional>

namespace widget {

class Button : public Label {
protected:
	bool mPressed;

	void on(Click const& click) override;
	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;
public:
	std::function<void(Button*)> onClickCallback;

	Button();
	~Button();

	inline bool pressed() const noexcept { return mPressed; }
};

} // namespace widget
