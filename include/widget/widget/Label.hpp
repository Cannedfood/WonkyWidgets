#pragma once

#include "../Widget.hpp"

namespace widget {

class Label : public Widget {
public:
	Label();
	~Label();

	void content(std::string const& s);
};

} // namespace widget
