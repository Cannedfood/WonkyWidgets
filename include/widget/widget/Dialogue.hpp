#pragma once

#include <functional>

#include "List.hpp"

namespace widget {

class ConfirmationDialogue : public List {
protected:
	void onDraw(Canvas& c) override;
public:
	ConfirmationDialogue();
	ConfirmationDialogue(
		std::string const& msg,
		std::function<void()> ok = nullptr);
	ConfirmationDialogue(
		std::string const& msg,
		std::function<void()> yes,
		std::function<void()> no);

	ConfirmationDialogue* option(
		std::string const& name,
		std::function<void()> callback = nullptr
	);
};

} // namespace widget
