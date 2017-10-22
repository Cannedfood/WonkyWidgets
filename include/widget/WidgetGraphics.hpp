#pragma once

namespace widget {

class Widget;

class WidgetGraphics {
public:
	virtual void draw(Widget*);
	virtual ~WidgetGraphics();
};

} // namespace widget
