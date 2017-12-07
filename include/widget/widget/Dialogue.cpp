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
	std::function<void()>&& callback
) :
	ConfirmationDialogue()
{
	add<Label>()->content(msg);
	Button* b = add<Button>()->text("Ok");
	b->alignx(AlignFill);
	if(callback) {
		b->onClick([=](Button* b) {
			remove();
			callback();
		});
	}
	else {
		b->onClick([=](Button* b) { remove(); });
	}
}

ConfirmationDialogue::ConfirmationDialogue(
	std::string const& msg,
	std::function<void(bool b)>&& callback
) :
	ConfirmationDialogue()
{
	add<Label>()->content(msg);
	List* yesno = add<List>();
	yesno->alignx(AlignFill);
	yesno->flow(FlowRight);
	yesno->add<Button>()
		->text("Yes")
		->onClick([=](Button*) {
			remove();
			callback(true);
		})
		->alignx(AlignFill);
	yesno->add<Button>()
		->text("No")
		->onClick([=](Button*) {
			remove();
			callback(false);
		})
		->alignx(AlignFill);
}

void ConfirmationDialogue::onDraw(Canvas& c) {
	c.fillRect(0, 0, width(), height(), 0x80FFFFFF);
}

} // namespace widget
