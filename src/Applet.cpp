#include "../include/widget/Applet.hpp"

namespace widget {

Applet::Applet() {}
Applet::~Applet() {}

void Applet::onAdd(Widget* child) {
	child->applet(this);
}
void Applet::onRemove(Widget* child) {
	child->applet(nullptr);
}

} // namespace widget
