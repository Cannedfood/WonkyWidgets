#include "../include/widget/Applet.hpp"

namespace widget {

Applet::Applet() {}
Applet::~Applet() {}

void Applet::onAdd(Widget* child) {
	child->eachPreOrder([this](Widget* w) {
		w->onAppletChanged(this);
	});
}
void Applet::onRemove(Widget* child) {
	child->eachPreOrder([](Widget* w) {
		w->onAppletChanged(nullptr);
	});
}

} // namespace widget
