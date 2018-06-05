#include "PropertyPane.hpp"

#include "../include/wwidget/AttributeCollector.hpp"
#include "../include/wwidget/widget/Text.hpp"
#include "../include/wwidget/widget/Button.hpp"
#include "../include/wwidget/widget/TextField.hpp"

#include "Demangle.hpp"

#include <regex>
#include <cassert>
#include <charconv>

namespace wwidget {

class Property : public List {
	std::string mName;
	std::string mValue;

	Text      mNameField;
	TextField mValueField;
	Button    mResetButton;
public:
	Property(std::string_view name, std::string_view value) :
		mName(name),
		mValue(value),
		mNameField(this, mName + ": "),
		mValueField(this)
	{
		align(AlignMin, AlignFill);
		flow(FlowRight);

		mValueField.align(AlignMax, AlignMin);
		mValueField.content(mValue);
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

	bool startSection(std::string_view name) override {
		if(name == "debug") return false;

		mCurrent->add<Text>(std::string(name))->fontColor(rgb(90, 157, 219));
		mCurrent = mCurrent->add<List>()->padding(10, 0, 0, 0);
		return true;
	}
	void endSection() override {
		assert(mCurrent != mRoot && "Stray AttributeCollectorInterface::endSection");
		mCurrent = mCurrent->parent();
	}

	void operator()(std::string_view name, std::string_view value, bool is_default = false) override {
		mCurrent->add<Property>(name, value);
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

static
std::string hex(size_t n) {
	std::string result(' ', sizeof(n * 2) + 2);
	auto to_chars_res = std::to_chars(result.data(), result.data() + result.size(), n, 16);
	result.resize(to_chars_res.ptr - result.data());
	return result;
}

void PropertyPane::updateProperties() {
	clearChildren();

	if(mCurrentWidget) {
		add<Text>(demangle(typeid(*mCurrentWidget).name()) + " (" + hex((size_t)mCurrentWidget) + "): ");

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
