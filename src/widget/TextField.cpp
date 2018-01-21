#include "../../include/wwidget/widget/TextField.hpp"
#include "../../include/wwidget/Canvas.hpp"

namespace wwidget {

TextField::TextField() {
	align(AlignFill, AlignMin);
}
TextField::TextField(Widget* addTo) :
	TextField()
{
	addTo->add(this);
}

TextField::~TextField() {}

void TextField::onUpdate(float dt) {
	Widget::onUpdate(dt);
}

bool TextField::onFocus(bool b, float strength) { return true; }

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
			if(mOnReturn) {
				defer(mOnReturn);
			}
		}
	}
	Widget::on(k);
}

void TextField::on(wwidget::TextInput const& t) {
	content(content() + t.utf8);

	t.handled = true;
}

void TextField::onDraw(Canvas& canvas) {
	Label::onDraw(canvas);
	canvas.box(100, {0, 0, width(), height()}, focused() ? rgb(215, 150, 0) : rgba(0, 0, 0, 0.3));
}

TextField* TextField::content(std::string c) {
	Label::content(c);
	if(mOnUpdate) {
		defer(mOnUpdate);
	}
	return this;
}

TextField* TextField::onReturn(std::function<void()> ret) {
	mOnReturn = std::move(ret); return this;
}
TextField* TextField::onUpdate(std::function<void()> update) {
	mOnUpdate = std::move(update); return this;
}

} // namespace wwidget
