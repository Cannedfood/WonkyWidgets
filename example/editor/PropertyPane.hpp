#pragma once

#include "../include/wwidget/widget/List.hpp"

namespace wwidget {

class PropertyPane : public List {
	shared<Widget> mCurrentWidget;
public:
	PropertyPane();
	~PropertyPane();

	void updateProperties();

	shared<Widget> currentWidget() const noexcept { return mCurrentWidget; }
	Widget& currentWidget(shared<Widget> w);
};

} // namespace wwidget
