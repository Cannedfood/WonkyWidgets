#include "PropertyPane.hpp"

#include "../include/widget/Attribute.hpp"
#include "../include/widget/widget/Label.hpp"
#include "../include/widget/widget/Button.hpp"
#include "../include/widget/widget/TextField.hpp"

namespace widget {

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
		flow(FlowRight);
		scrollable(true);
		mValueField.content(value);
		mValueField.onReturn([this]() {
			findParent<PropertyPane>()->currentWidget()->setAttribute(mName, mValueField.content());
		});
	}
};

PropertyPane::PropertyPane() :
	mCurrentWidget(nullptr)
{
	add<Label>()->content("Stuffs");
}

PropertyPane::~PropertyPane() {}

void PropertyPane::updateProperties() {
	clearChildren();

	if(mCurrentWidget) {
		auto collector = StringAttributeCollector(
			[this](std::string const& a, std::string const& b, bool isDefault) {
				printf("%s %s\n", a.c_str(), b.c_str());
				add<Property>(a, b);
			});
		mCurrentWidget->getAttributes(collector);
	}
}

Widget* PropertyPane::currentWidget(Widget* w) {
	mCurrentWidget = w;
	updateProperties();
	return this;
}

} // namespace widget
