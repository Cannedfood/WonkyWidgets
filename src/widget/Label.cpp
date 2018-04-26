#include "../../include/wwidget/widget/Label.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/Bitmap.hpp"
#include "../../include/wwidget/Font.hpp"
#include "../../include/wwidget/fonts/BitmapFont.hpp"

#include "../../include/wwidget/Attribute.hpp"

#include <cmath>

namespace wwidget {

Label::Label() :
	Widget()
{}

Label::Label(std::string content) :
	Label()
{
	this->content(content);
}

Label::Label(Widget* attachTo) :
	Label()
{
	attachTo->add(this);
}
Label::Label(Widget* attachTo, std::string content) :
	Label(std::move(content))
{
	attachTo->add(this);
}

Label::~Label() {}

Label* Label::content(std::string s) {
	mText = std::move(s);
	bake();
	return this;
}
void Label::font(std::string const& name) {
	mFontPath = name;
	reloadFont();
}
void Label::font(std::shared_ptr<Font> font) {
	if(mFont != font) {
		// printf("%p: Changed font: %p\n", this, font.get());
		mFont = std::move(font);
		if(mFont) {
			mBitmapFont = mFont->get(16);
			bake();
		}
		else {
			mBitmapFont = nullptr;
			mRects.clear();
			mTexRects.clear();
		}
	}
}
bool Label::setAttribute(std::string const& name, std::string const& value) {
	if(name == "content") {
		content(value); return true;
	}
	if(name == "font") {
		font(value); return true;
	}
	return Widget::setAttribute(name, value);
}
void Label::getAttributes(AttributeCollectorInterface& collector) {
	if(collector.startSection("wwidget::Label")) {
		collector("content", mText);
		collector("font", mFontPath, mFontPath.empty());
		collector.endSection();
	}
	Widget::getAttributes(collector);
}
void Label::reloadFont() {
	// printf("Reload %p: Has applet? %p\n", this, applet());
	loadFont([this](auto f) { font(f); }, mFontPath);
}
void Label::bake() {
	mRects.clear();
	mTexRects.clear();

	if(mBitmapFont) {
		float x = 0, y = mBitmapFont->metrics().ascend + mBitmapFont->metrics().lineGap * .5f;
		mBitmapFont->render(mText, x, y, mRects, mTexRects);
		preferredSizeChanged();
	}
	// printf("Baked %s (%u rects)\n", mText.c_str(), (unsigned) mRects.size());
}
void Label::onAppletChanged() {
	mBitmapFont.reset();
	mFont.reset();
	reloadFont();
}
void Label::onAddTo(Widget* p) {
	reloadFont();
}
void Label::onRemovedFrom(Widget* p) {
	reloadFont();
}
void Label::onCalcPreferredSize(PreferredSize& info) {
	if(mBitmapFont) {
		info.miny = info.prefy = std::ceil(mBitmapFont->metrics().lineHeight);
		if(!mRects.empty()) {
			info.minx = info.prefx = std::ceil(mRects.back().max.x);
		}
	}
	else {
		Widget::onCalcPreferredSize(info);
	}
}
void Label::onDraw(Canvas& canvas) {
	if(mBitmapFont) {
		canvas.rects(
			mRects.size(),
			mRects.data(), mTexRects.data(),
			mBitmapFont, rgb(255, 255, 255)
		);
	}
}

} // namespace wwidget
