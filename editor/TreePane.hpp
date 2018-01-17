#pragma once

#include "../include/widget/widget/List.hpp"

namespace widget {

class TreePane : public List {
	Widget* mWidget;
public:
	std::function<void(Widget*)> onSelect;

	TreePane();

	void setWidget(Widget* w);
	void select(Widget* w);
};

} // namespace widget
