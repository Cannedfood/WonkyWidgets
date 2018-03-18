#include "../../include/wwidget/widget/Form.hpp"

#include "../../include/wwidget/widget/Button.hpp"
#include "../../include/wwidget/widget/Label.hpp"
#include "../../include/wwidget/widget/TextField.hpp"
#include "../../include/wwidget/widget/Image.hpp"

#include "../../include/wwidget/widget/List.hpp"

#include "../../include/wwidget/widget/ProgressBar.hpp"
#include "../../include/wwidget/widget/Slider.hpp"
#include "../../include/wwidget/widget/Knob.hpp"

#ifndef WWIDGET_NO_WINDOWS
	#include "../../include/wwidget/Window.hpp"
#endif

#include <iostream>

namespace wwidget {

Form& Form::addDefaultFactories() {
	factory<Button>();
	factory<Button>("button");

	factory<Label>();
	factory<Label>("label");
	factory<Label>("p");

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

#ifndef WWIDGET_NO_WINDOWS
	factory<Window>();
	factory<Window>("window");
#endif // ifndef WWIDGET_NO_WINDOWS

	// Do not misread
	auto createChildForm = [this]() -> std::unique_ptr<Widget> {
		auto p = std::make_unique<Form>();
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
