#include "../../include/widget/widget/Label.hpp"

#include "../../include/widget/Canvas.hpp"
#include "../../include/widget/Bitmap.hpp"
#include "../../include/widget/Font.hpp"
#include "../../include/widget/fonts/BitmapFont.hpp"


namespace widget {

Label::Label() :
	Widget()
{}

Label::~Label() {}

void Label::content(std::string const& s) { WIDGET_M_FN_MARKER
	mText = s;
}

void Label::font(std::string const& name) {
	if(mFontPath != name) {
		mFontPath = name;
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
void Label::bake() {
	mRects.clear();
	mTexRects.clear();
	mBitmapFont->render(mText, mRects, mTexRects);
	// printf("Baked %s (%u rects)\n", mText.c_str(), (unsigned) mRects.size());
}
void Label::onCalculateLayout(LayoutInfo& info) {
	Widget::onCalculateLayout(info);
	/*
	if(!mRects.empty()) {
		info.minx = info.prefx = mRects[mRects.size() - 2];
		info.miny = info.prefy = mRects[mRects.size() - 1];
	}
	*/
}
void Label::onDraw(Canvas& canvas) {
	bool needsBake = false;
	if(!mFont) {
		if(!canvas.loadFont(this, mFontPath, mFont)) return;
		needsBake = true;
	}
	if(!mBitmapFont) {
		mBitmapFont = mFont->get(32);
		needsBake = true;
	}
	if(!mBitmap) {
		mBitmap = canvas.loadTextureNow(
			mBitmapFont->bitmap().data(),
			mBitmapFont->width(), mBitmapFont->height(), 1
		);
		needsBake = true;
	}

	if(needsBake) {
		bake();
	}

	canvas.fillRects(mRects.data(), mTexRects.data(), mRects.size() / 8, mBitmap.get(), rgb(255, 255, 255));
}

} // namespace widget
