#include "../../include/wwidget/widget/Text.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/Bitmap.hpp"
#include "../../include/wwidget/Font.hpp"
#include "../../include/wwidget/fonts/BitmapFont.hpp"

#include "../../include/wwidget/AttributeCollector.hpp"

#include <cmath>

namespace wwidget {

Text::Text() :
	Widget(),
	mFontColor(Color::white())
{}
Text::Text(std::string content) :
	Text()
{
	this->content(content);
}
Text::Text(Widget* attachTo) :
	Text()
{
	attachTo->add(this);
}
Text::Text(Widget* attachTo, std::string content) :
	Text(std::move(content))
{
	attachTo->add(this);
}
Text::~Text() {}

Text::Text(Text&& other) noexcept :
	Widget(std::move(other)),
	mFont(std::move(other.mFont)),
	mBitmapFont(std::move(other.mBitmapFont)),
	mRects(std::move(other.mRects)),
	mTexRects(std::move(other.mTexRects)),
	mFontColor(other.mFontColor),
	mFontPath(std::move(other.mFontPath)),
	mText(std::move(other.mText))
{
	other.mFontColor = Color::white();
}
Text& Text::operator=(Text&& other) noexcept {
	Widget::operator=(std::move(other));
	mFont = std::move(other.mFont);
	mBitmapFont = std::move(other.mBitmapFont);
	mRects = std::move(other.mRects);
	mTexRects = std::move(other.mTexRects);
	mFontColor = other.mFontColor;
	mFontPath = std::move(other.mFontPath);
	mText = std::move(other.mText);
	other.mFontColor = Color::white();
	return *this;
}

Text* Text::content(std::string s) {
	mText = std::move(s);
	bake();
	return this;
}
void Text::font(std::string const& name) {
	mFontPath = name;
	reloadFont();
}
void Text::font(std::shared_ptr<Font> font) {
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
Text* Text::fontColor(Color c) { mFontColor = c; return this; }
Color Text::fontColor() { return mFontColor; }
bool Text::setAttribute(std::string const& name, std::string const& value) {
	if(name == "content") {
		content(value); return true;
	}
	if(name == "font") {
		font(value); return true;
	}
	return Widget::setAttribute(name, value);
}
void Text::getAttributes(AttributeCollectorInterface& collector) {
	if(collector.startSection("wwidget::Text")) {
		collector("content", mText);
		collector("font", mFontPath, mFontPath.empty());
		collector("fontColor", mFontColor, mFontColor == Color::white());
		collector.endSection();
	}
	Widget::getAttributes(collector);
}
void Text::reloadFont() {
	// printf("Reload %p: Has context? %p\n", this, context());
	loadFont(&mFontLoadTasks, [this](auto f) { font(f); }, mFontPath);
}
void Text::bake() {
	mRects.clear();
	mTexRects.clear();

	if(mBitmapFont) {
		float x = 0, y = mBitmapFont->metrics().ascend + mBitmapFont->metrics().lineGap * .5f;
		mBitmapFont->render(mText, x, y, mRects, mTexRects);
		preferredSizeChanged();
	}
	// printf("Baked %s (%u rects)\n", mText.c_str(), (unsigned) mRects.size());
}
void Text::onContextChanged() {
	mBitmapFont.reset();
	mFont.reset();
	reloadFont();
}
void Text::onAddTo(Widget* p) {
	reloadFont();
}
void Text::onRemovedFrom(Widget* p) {
	reloadFont();
}
PreferredSize Text::onCalcPreferredSize() {
	if(!mBitmapFont) return Widget::onCalcPreferredSize();

	PreferredSize info;
	info.min.y = info.pref.y = std::ceil(mBitmapFont->metrics().lineHeight);
	if(!mRects.empty()) {
		info.min.x = info.pref.x = std::ceil(mRects.back().max.x);
	}
	return info;
}
void Text::onDraw(Canvas& canvas) {
	if(mBitmapFont) {
		canvas.rects(
			mRects.size(),
			mRects.data(), mTexRects.data(),
			mBitmapFont, fontColor()
		);
	}
}

} // namespace wwidget
