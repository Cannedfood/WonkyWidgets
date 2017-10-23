#pragma once

#include "../Widget.hpp"

namespace widget {

class Label : public Widget {
protected:
	WIDGET_MAGIC_CONSTRUCTOR_ATTRIBUTE(std::string const&, ) // TODO
public:
	Label();
	~Label();

	void content(std::string const& s);
};

} // namespace widget
