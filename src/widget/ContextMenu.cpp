#include "../../include/wwidget/widget/ContextMenu.hpp"

#include "../../include/wwidget/Context.hpp"
#include "../../include/wwidget/Canvas.hpp"

namespace wwidget {

ContextMenu::ContextMenu() {
	align(AlignNone);
}
void ContextMenu::on(Click const& click) {
	if(!Rect(size()).contains(click.position)) {
		if(click.down())
			defer([this]() { removeFocus(); });
		click.handled = true;
	}
}
bool ContextMenu::onFocus(bool b, FocusType type) {
	if(!b) defer([this]() { remove(); });
	return true;
}

void ContextMenu::onDrawBackground(Canvas& c) {
	c.fillColor(rgba(55, 55, 55, .8f))
	 .rect(size())
	 .fill();
}
void ContextMenu::onDraw(Canvas& c) {
	c.strokeColor(rgb(215, 150, 0))
	 .rect(size())
	 .stroke();
}

} // namespace wwidget