#include "PropertyPane.hpp"

#include "../include/wwidget/Attribute.hpp"
#include "../include/wwidget/widget/Label.hpp"
#include "../include/wwidget/widget/Button.hpp"
#include "../include/wwidget/widget/TextField.hpp"

#include <regex>

namespace wwidget {

static std::regex DebugPropertyRegex("dbg_.*");

class Property : public List {
	std::string mName;
	std::string mValue;

	Label     mNameField;
	TextField mValueField;
	Button    mResetButton;
public:
	Property(std::string const& name, std::string const& value) :
		mName(name),
		mValue(value),
		mNameField(this, name + ": "),
		mValueField(this)
	{
		mValueField.align(AlignMax, AlignMin);
		align(AlignMin, AlignFill);
		flow(FlowRight);
		mValueField.content(value);
		mValueField.onReturn([this]() {
			auto* pp = findParent<PropertyPane>();
			pp->currentWidget()->setAttribute(mName, mValueField.content());
			pp->updateProperties();
		});
	}
};

PropertyPane::PropertyPane() :
	mCurrentWidget(nullptr)
{
	add<Label>()->content("Stuffs");
	scrollable(true);
}

PropertyPane::~PropertyPane() {}

void PropertyPane::updateProperties() {
	clearChildren();

	if(mCurrentWidget) {
		add<Label>(std::string(typeid(*mCurrentWidget).name()) + ": ");

		auto collector = StringAttributeCollector(
			[this](std::string const& name, std::string const& value, bool isDefault) {
				if(!std::regex_match(name, DebugPropertyRegex))
					add<Property>(name, value);
			});
		mCurrentWidget->getAttributes(collector);
	}
}

Widget* PropertyPane::currentWidget(Widget* w) {
	mCurrentWidget = w;
	updateProperties();
	return this;
}

} // namespace wwidget
