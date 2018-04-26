#include "PropertyPane.hpp"

#include "../include/wwidget/AttributeCollector.hpp"
#include "../include/wwidget/widget/Text.hpp"
#include "../include/wwidget/widget/Button.hpp"
#include "../include/wwidget/widget/TextField.hpp"

#include "Demangle.hpp"

#include <regex>


namespace wwidget {

static std::regex DebugPropertyRegex("dbg_.*");

class Property : public List {
	std::string mName;
	std::string mValue;

	Text      mNameField;
	TextField mValueField;
	Button    mResetButton;
public:
	Property(std::string const& name, std::string const& value) :
		mName(name),
		mValue(value),
		mNameField(this, name + ": "),
		mValueField(this)
	{
		align(AlignMin, AlignFill);
		flow(FlowRight);

		mValueField.align(AlignMax, AlignMin);
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
	add<Text>()->content("Stuffs");
	scrollable(true);
}

PropertyPane::~PropertyPane() {}

void PropertyPane::updateProperties() {
	clearChildren();

	if(mCurrentWidget) {
		add<Text>(demangle(typeid(*mCurrentWidget).name()) + ": ");

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
