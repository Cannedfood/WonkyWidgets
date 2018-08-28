#pragma once

#include "List.hpp"

namespace wwidget {

class WrappedList : public List {
public:
	WrappedList();
	WrappedList(Widget* addTo);

	PreferredSize onCalcPreferredSize(PreferredSize const& constraints) override;
	void onLayout() override;
};

} // namespace wwidget
