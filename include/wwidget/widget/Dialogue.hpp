#pragma once

#include <functional>

#include "List.hpp"

namespace wwidget {

class Dialogue : public List {
protected:
	void onDraw(Canvas& c) override;
public:
	Dialogue();
	Dialogue(
		std::string const& msg,
		std::function<void()> ok = nullptr);
	Dialogue(
		std::string const& msg,
		std::function<void()> yes,
		std::function<void()> no);

	Dialogue& option(
		std::string const& name,
		std::function<void()> callback = nullptr
	);
};

} // namespace wwidget
