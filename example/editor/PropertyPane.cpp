#include "PropertyPane.hpp"

#include "../include/wwidget/AttributeCollector.hpp"
#include "../include/wwidget/widget/Text.hpp"
#include "../include/wwidget/widget/Button.hpp"
#include "../include/wwidget/widget/TextField.hpp"

#include "Demangle.hpp"

#include <regex>
#include <cassert>

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

namespace {
class MyPropertyBuilder : public wwidget::StringAttributeCollector {
	Widget* mRoot;
	Widget* mCurrent;
public:
	MyPropertyBuilder(Widget* into) :
	mRoot(into),
	mCurrent(into)
	{}

	bool startSection(std::string const& name) override {
		mCurrent->add<Text>(name)->fontColor(rgb(90, 157, 219));
		mCurrent = mCurrent->add<List>()->padding(10, 0, 0, 0);
		return true;
	}
	void endSection() override {
		assert(mCurrent != mRoot && "Stray AttributeCollectorInterface::endSection");
		mCurrent = mCurrent->parent();
	}

	void operator()(std::string const& name, std::string const& value, bool is_default = false) override {
		if(!std::regex_match(name, DebugPropertyRegex)) {
			mCurrent->add<Property>(name, value);
		}
	}
};
} // anon namespace

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

		auto collector = MyPropertyBuilder(this);
		mCurrentWidget->getAttributes(collector);
	}
}

Widget* PropertyPane::currentWidget(Widget* w) {
	mCurrentWidget = w;
	updateProperties();
	return this;
}

} // namespace wwidget
