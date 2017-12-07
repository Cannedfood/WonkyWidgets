#include "Dialogue.hpp"

#include "Label.hpp"
#include "Button.hpp"
#include "../Canvas.hpp"

namespace widget {

ConfirmationDialogue::ConfirmationDialogue() {
	align(AlignCenter);
}

ConfirmationDialogue::ConfirmationDialogue(
	std::string const& msg,
	std::function<void()> ok
) :
	ConfirmationDialogue()
{
	add<Label>()->content(msg);
	option("Ok", ok);
}

ConfirmationDialogue::ConfirmationDialogue(
	std::string const& msg,
	std::function<void()> yes,
	std::function<void()> no
) :
	ConfirmationDialogue()
{
	add<Label>()->content(msg);
	option("Yes", std::move(yes));
	option("No", std::move(no));
}

ConfirmationDialogue* ConfirmationDialogue::option(std::string const& name, std::function<void()> callback) {
	auto* b = add<Button>()->text(name);
	if(callback) {
		b->onClick([=](Button*) { remove(); callback(); });
	}
	else {
		b->onClick([=](Button*) { remove(); });
	}
	return this;
}

void ConfirmationDialogue::onDraw(Canvas& c) {
	c.fillRect(0, 0, width(), height(), 0x80FFFFFF);
}

} // namespace widget
