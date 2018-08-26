#pragma once

#include "../include/wwidget/Widget.hpp"

#include <functional>

namespace wwidget {

class WysiwygMarker;
class Form;

class WysiwygPane : public Widget {
	shared<Form>          mForm;
	shared<Widget>        mSelected;
	shared<WysiwygMarker> mMarker;
public:
	std::function<void(shared<Widget>)> onSelect;
	std::function<void(shared<Widget>)> onLoaded;

	WysiwygPane();
	~WysiwygPane() override;

	void select(shared<Widget> w);

	void load(std::string const& path);
	void unload();

	void on(Dragged const& d) override;
	void on(Click const& c) override;
	void on(KeyEvent const& k) override;
};

} // namespace wwidget
