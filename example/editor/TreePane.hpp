#pragma once

#include "../include/wwidget/widget/List.hpp"

namespace wwidget {

class TreePane : public List {
	shared<Widget> mSelected;
public:
	std::function<void(shared<Widget>)> onSelect;

	TreePane();

	void setWidget(shared<Widget> w);
	void select(shared<Widget> w);
	void signalSelect(shared<Widget> w);

	shared<Widget> const& selected() { return mSelected; }
};

} // namespace wwidget
