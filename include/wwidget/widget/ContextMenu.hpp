#pragma once

#include "List.hpp"


namespace wwidget {

class ContextMenu : public List {
public:
	ContextMenu();

	void on(Click const& click) override;

	bool onFocus(bool b, FocusType type) override;

	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;

	static shared<ContextMenu> Create(shared<Widget> at, Point position = {});
};

} // namespace wwidget
