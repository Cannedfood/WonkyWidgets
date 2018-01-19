#pragma once

#include "../include/widget/Widget.hpp"

#include <functional>

namespace widget {

class WysiwygMarker;
class Form;

class WysiwygPane : public Widget {
	Form*          mForm;
	Widget*        mSelected;
	WysiwygMarker* mMarker;
public:
	std::function<void(Widget*)> onSelect;
	std::function<void(Widget*)> onLoaded;

	WysiwygPane();
	~WysiwygPane() override;

	void select(Widget* w);

	void load(std::string const& path);
	void unload();

	void on(Dragged const& d) override;
	void on(Click const& c) override;
	void on(KeyEvent const& k) override;
};

} // namespace widget
