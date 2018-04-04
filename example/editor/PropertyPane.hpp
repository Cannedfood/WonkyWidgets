#pragma once

#include "../include/wwidget/widget/List.hpp"

namespace wwidget {

class PropertyPane : public List {
	Widget* mCurrentWidget;
public:
	PropertyPane();
	~PropertyPane();

	void updateProperties();

	Widget* currentWidget() const noexcept { return mCurrentWidget; }
	Widget* currentWidget(Widget* w);
};

} // namespace wwidget
