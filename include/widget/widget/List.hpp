#pragma once

#include "../Widget.hpp"

namespace widget {

class List : public Widget {
public:
	List();
	List(Widget* parent);
	~List();
};

} // namespace widget
