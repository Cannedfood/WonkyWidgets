#pragma once

#include "List.hpp"

namespace wwidget {

class WrappedList : public List {
public:
	WrappedList();
	WrappedList(Widget* addTo);

	PreferredSize onCalcPreferredSize() override;
	void onLayout() override;
};

} // namespace wwidget
