#pragma once

#include <functional>

#include "List.hpp"

namespace widget {

class ConfirmationDialogue : public List {
protected:
	void onDraw(Canvas& c) override;
public:
	ConfirmationDialogue();
	ConfirmationDialogue(std::string const& msg, std::function<void()>&& callback = nullptr);
	ConfirmationDialogue(std::string const& msg, std::function<void(bool b)>&& callback);
};

} // namespace widget
