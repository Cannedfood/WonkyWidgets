#include "../../include/wwidget/widget/Form.hpp"

#include "../../include/wwidget/widget/Button.hpp"
#include "../../include/wwidget/widget/Text.hpp"
#include "../../include/wwidget/widget/TextField.hpp"
#include "../../include/wwidget/widget/Image.hpp"

#include "../../include/wwidget/widget/List.hpp"

#include "../../include/wwidget/widget/ProgressBar.hpp"
#include "../../include/wwidget/widget/Slider.hpp"
#include "../../include/wwidget/widget/Knob.hpp"

#include "../../include/wwidget/widget/FileBrowser.hpp"

#ifndef WWIDGET_NO_WINDOWS
	#include "../../include/wwidget/Window.hpp"
#endif

#include <iostream>

namespace wwidget {

Form& Form::addDefaultFactories() {
	factory<Widget>();
	factory<Widget>("widget");

	factory<Button>();
	factory<Button>("button");

	factory<Text>();
	factory<Text>("text");
	factory<Text>("p");

	factory<Image>();
	factory<Image>("image");

	factory<List>();
	factory<List>("list");

	factory<Slider>();
	factory<Slider>("slider");

	factory<Knob>();
	factory<Knob>("knob");

	factory<ProgressBar>();
	factory<ProgressBar>("progressbar");

	factory<TextField>();
	factory<TextField>("textfield");

	factory<FileBrowser>();
	factory<FileBrowser>("filebrowser");

#ifndef WWIDGET_NO_WINDOWS
	factory<Window>();
	factory<Window>("window");
#endif // ifndef WWIDGET_NO_WINDOWS

	// Do not misread
	auto createChildForm = [this]() -> shared<Widget> {
		auto p = make_shared<Form>();
		p->mFactories = this->mFactories;
		return p;
	};

	factory("wwidget::Form", createChildForm);
	factory("form", createChildForm);

	// std::cout << "Registered factories:" << std::endl;
	// for(auto& pair : mFactories) {
	// 	std::cout << "\t" << pair.first << std::endl;
	// }

	return *this;
}

} // namespace wwidget
