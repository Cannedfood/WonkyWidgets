#include "../../include/wwidget/widget/Dialogue.hpp"

#include "../../include/wwidget/widget/Text.hpp"
#include "../../include/wwidget/widget/Button.hpp"
#include "../../include/wwidget/Canvas.hpp"

namespace wwidget {

Dialogue::Dialogue() {
	align(AlignCenter);
}

Dialogue::Dialogue(
	std::string const& msg,
	std::function<void()> ok
) : Dialogue()
{
	add<Text>()->content(msg);
	option("Ok", ok);
}

Dialogue::Dialogue(
	std::string const& msg,
	std::function<void()> yes,
	std::function<void()> no
) : Dialogue()
{
	add<Text>()->content(msg);
	option("Yes", std::move(yes));
	option("No", std::move(no));
}

Dialogue& Dialogue::option(std::string const& name, std::function<void()> callback) {
	auto b = add<Button>();
	b->text(name);
	if(callback) {
		b->onClick([=](Button*) { remove(); callback(); });
	}
	else {
		b->onClick([=](Button*) { remove(); });
	}
	return *this;
}

void Dialogue::onDraw(Canvas& c) {
	c.rect({0, 0, width(), height()}, 0x80FFFFFF);
}

} // namespace wwidget
