#include "../../include/widget/widget/Form.hpp"

#include "../../include/widget/widget/Button.hpp"
#include "../../include/widget/widget/Label.hpp"
#include "../../include/widget/widget/Image.hpp"

#include "../../include/widget/widget/List.hpp"

#include <iostream>

namespace widget {

Form& Form::addDefaultFactories() {
	factory<Button>();
	factory<Button>("button");

	factory<Label>();
	factory<Label>("label");

	factory<Image>();
	factory<Image>("image");

	factory<List>();
	factory<List>("list");

	// Do not misread
	auto createChildForm = [this]() -> std::unique_ptr<Widget> {
		auto p = std::make_unique<Form>();
		p->mFactories = this->mFactories;
		return p;
	};

	factory("widget::Form", createChildForm);
	factory("form", createChildForm);

	// std::cout << "Registered factories:" << std::endl;
	// for(auto& pair : mFactories) {
	// 	std::cout << "\t" << pair.first << std::endl;
	// }

	return *this;
}

} // namespace widget
