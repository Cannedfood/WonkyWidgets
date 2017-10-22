#pragma once

#include "Label.hpp"

#include <functional>

namespace widget {

class Button : public Label {
protected:
	WIDGET_MAGIC_CONSTRUCTOR_ATTRIBUTE(std::function<void(Button*)>&&, onClickCallback = std::move(value));

public:
	std::function<void(Button*)> onClickCallback;

	WIDGET_MAGIC_CONSTRUCTOR(Button)

	Button();
	~Button();
};

} // namespace widget
