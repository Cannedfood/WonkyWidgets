#include "../../include/widget/widget/Label.hpp"

#include "../../include/widget/Canvas.hpp"
#include "../../include/widget/Bitmap.hpp"
#include "../../include/widget/Font.hpp"
#include "../../include/widget/fonts/BitmapFont.hpp"

#include "../../include/widget/Attribute.hpp"


namespace widget {

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

Label* Label::content(std::string s) { WIDGET_M_FN_MARKER
	mText = std::move(s);
	bake();
	return this;
}
void Label::font(std::string const& name) { WIDGET_M_FN_MARKER
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
bool Label::setAttribute(std::string const& name, std::string const& value) { WIDGET_M_FN_MARKER
	if(name == "content") {
		content(value); return true;
	}
	if(name == "font") {
		font(value); return true;
	}
	return Widget::setAttribute(name, value);
}
void Label::getAttributes(AttributeCollectorInterface& collector) { WIDGET_M_FN_MARKER
	Widget::getAttributes(collector);
	collector("content", mText);
	collector("font", mFontPath, mFontPath.empty());
}
void Label::reloadFont() {
	// printf("Reload %p: Has applet? %p\n", this, applet());
	loadFont([this](auto f) { font(f); }, mFontPath);
}
void Label::bake() { WIDGET_M_FN_MARKER
	mRects.clear();
	mTexRects.clear();

	if(mBitmapFont) {
		float x = 0, y = mBitmapFont->metrics().ascend + mBitmapFont->metrics().lineGap * .5f;
		mBitmapFont->render(mText, x, y, mRects, mTexRects);
		preferredSizeChanged();
	}
	// printf("Baked %s (%u rects)\n", mText.c_str(), (unsigned) mRects.size());
}
void Label::onAppletChanged(Applet* a) {
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
void Label::onCalculateLayout(LayoutInfo& info) { WIDGET_M_FN_MARKER
	if(mBitmapFont) {
		info.miny = info.prefy = mBitmapFont->metrics().lineHeight;
		if(!mRects.empty()) {
			info.minx = info.prefx = mRects.back().x1;
		}
	}
	else {
		Widget::onCalculateLayout(info);
	}
}
void Label::onDraw(Canvas& canvas) {
	WIDGET_ENABLE_MARKERS
	if(mBitmapFont) {
		canvas.rects(
			mRects.size(),
			mRects.data(), mTexRects.data(),
			mBitmapFont, rgb(255, 255, 255)
		);
	}
}

} // namespace widget
