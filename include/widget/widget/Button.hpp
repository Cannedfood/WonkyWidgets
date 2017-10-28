#pragma once

#include "Label.hpp"

#include <functional>

namespace widget {

class Button : public Label {
protected:
	bool mPressed;

	void on(Click const& click) override;
	void onDraw();
public:
	std::function<void(Button*)> onClickCallback;

	Button();
	~Button();
};

} // namespace widget
