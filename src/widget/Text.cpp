#include "../../include/wwidget/widget/Text.hpp"

#include "../../include/wwidget/Context.hpp"
#include "../../include/wwidget/Canvas.hpp"

#include "../../include/wwidget/AttributeCollector.hpp"

// TODO, FIXME, HACK: update for new Canvas
namespace wwidget {

Text::Text() :
	Widget(),
	mFontColor(Color::white()),
	mFontSize(0.f),
	mWrap(false)
{}
Text::Text(std::string content) :
	Text()
{
	this->content(content);
}
Text::Text(Widget* attachTo) :
	Text()
{
	attachTo->add(*this);
}
Text::Text(Widget* attachTo, std::string content) :
	Text(std::move(content))
{
	attachTo->add(*this);
}
Text::~Text() {}

Text::Text(Text&& other) noexcept :
	Widget(std::move(other)),
	mFontColor(other.mFontColor),
	mFontSize(0.f),
	mFont(std::move(other.mFont)),
	mText(std::move(other.mText))
{
	other.mFontColor = Color::white();
}
Text& Text::operator=(Text&& other) noexcept {
	Widget::operator=(std::move(other));
	mFontColor = other.mFontColor;
	mFontSize  = other.mFontSize;
	mFont      = std::move(other.mFont);
	mText      = std::move(other.mText);
	return *this;
}

Text& Text::content(std::string s) {
	if(mText != s) {
		mText = std::move(s);
		preferredSizeChanged();
		requestRedraw();
	}
	return *this;
}
Text& Text::font(std::string const& name) {
	if(mFont != name) {
		mFont = name;
		preferredSizeChanged();
		requestRedraw();
	}
	return *this;
}
Text& Text::fontColor(Color const& c) { mFontColor = c; return *this; }
Text& Text::fontSize(float f) {
	if(mFontSize != f) {
		mFontSize = f;
		preferredSizeChanged();
		requestRedraw();
	}
	return *this;
}
Text& Text::wrap(bool b) {
	if(mWrap != b) {
		mWrap = b;
		preferredSizeChanged();
		requestRedraw();
	}
	return *this;
}

bool Text::setAttribute(std::string_view name, Attribute const& value) {
	if(name == "content") { content(value.toString()); return true; }
	if(name == "font")    { font(value.toString()); return true; }
	if(name == "fontColor") { fontColor(value.toColor()); return true; }
	if(name == "fontSize") { fontSize(value.toFloat()); return true; }
	if(name == "wrap") { wrap(value.toBool()); return true; }
	return Widget::setAttribute(name, value);
}
void Text::getAttributes(AttributeCollectorInterface& collector) {
	if(collector.startSection("wwidget::Text")) {
		collector("content",   mText, "");
		collector("font",      mFont, "");
		collector("fontSize",  mFontSize, 0);
		collector("fontColor", mFontColor, Color::white());
		collector("wrap",      mWrap, false);
		collector.endSection();
	}
	Widget::getAttributes(collector);
}

void Text::onContextChanged() {
	preferredSizeChanged();
	requestRedraw();
}

PreferredSize Text::onCalcPreferredSize(PreferredSize const& constraint) {
	auto* ctxt = context();
	if(!ctxt) return {};

	auto& c = ctxt->canvas();

	c.fillColor(mFontColor)
	 .font(mFont.c_str())
	 .fontSize(mFontSize);

	PreferredSize size;

	if(mWrap) {
		size = {
			{5},
			c.textBoxBounds({}, constraint.max.x, mText).size(),
			Size::infinite()
		};
	}
	else {
		size = {
			{5},
			c.textBounds({}, mText).size(),
			Size::infinite()
		};
	}

	size.sanitize();

	return size;
}
void Text::onDraw(Canvas& c) {
	c.fillColor(mFontColor)
	 .font(mFont.c_str())
	 .fontSize(mFontSize);

	if(mWrap)
		c.textBox(Point(0, c.fontMetrics().ascend), width(), mText);
	else
		c.text(Point(0, c.fontMetrics().ascend), mText);
}

} // namespace wwidget
