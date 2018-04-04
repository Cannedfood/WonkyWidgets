#pragma once

#include "../include/wwidget/widget/List.hpp"

namespace wwidget {

class TreePane : public List {
	Widget* mSelected;
	Widget* mWidget;
public:
	std::function<void(Widget*)> onSelect;

	TreePane();

	void setWidget(Widget* w);
	void select(Widget* w);
	void signalSelect(Widget* w);

	Widget* selected() { return mSelected; }
};

} // namespace wwidget
