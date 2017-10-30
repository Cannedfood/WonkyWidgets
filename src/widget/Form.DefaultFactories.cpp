#include "../../include/widget/widget/Form.hpp"

#include "../../include/widget/widget/Button.hpp"
#include "../../include/widget/widget/Label.hpp"

#include "../../include/widget/widget/List.hpp"

#include "../../include/widget/widget/Form.hpp"

namespace widget {

Form& Form::addDefaultFactories() {
	factory<Button>(); factory<Button>("button");
	factory<Label>(); factory<Label>("label");
	factory("form", [this]() -> std::unique_ptr<Widget> {
		auto p = std::make_unique<Form>();
		p->mFactories = this->mFactories;
		return p;
	});

	return *this;
}

} // namespace widget
