#include "../../include/widget/widget/TextField.hpp"
#include "../../include/widget/Canvas.hpp"

namespace widget {

TextField::TextField() {}
TextField::~TextField() {}

void TextField::onUpdate(float dt) {
	Widget::onUpdate(dt);
}

void TextField::on(KeyEvent const& k) {
	if(k.scancode == 22) { // Backspace
		k.handled = true;

		if(k.state != Event::UP) {
			std::string c = content();
			while(!c.empty()) {
				bool wasContinuationByte = (c[c.size() - 1] & 0xC0) == 0x80;
				c.pop_back();
				if(!wasContinuationByte)
					break;
			}
			content(c);
		}
	}
	else if(k.scancode == 36) { // Enter
		k.handled = true;
		if(k.state != Event::UP) {
			if(onReturnCallback) {
				onReturnCallback(this);
			}
		}
	}
}

void TextField::on(widget::TextInput const& t) {
	content(content() + t.utf8);

	t.handled = true;
}

void TextField::onDraw(Canvas& canvas) {
	Label::onDraw(canvas);
	canvas.outlineRRect(100, 3, 0, 0, width(), height(), rgba(0, 0, 0, 0.3));
}

TextField* TextField::content(std::string c) {
	Label::content(c);
	if(onUpdateCallback) {
		onUpdateCallback(this);
	}
	return this;
}

} // namespace widget
