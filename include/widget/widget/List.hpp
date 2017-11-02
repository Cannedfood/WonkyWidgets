#pragma once

#include "../Widget.hpp"

namespace widget {

class List : public Widget {
protected:
	void onAdd(Widget* child) override;
	void onRemove(Widget* child) override;
	void preLayout(LayoutInfo& info) override;
	void onLayout() override;
public:
	List();
	~List();
};

} // namespace widget
