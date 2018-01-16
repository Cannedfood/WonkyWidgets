#pragma once

#include "../include/widget/widget/List.hpp"

namespace widget {

class PropertyPane : public List {
	Widget* mCurrentWidget;
public:
	PropertyPane();
	~PropertyPane();

	void updateProperties();

	Widget* currentWidget() const noexcept { return mCurrentWidget; }
	Widget* currentWidget(Widget* w);
};

} // namespace widget
