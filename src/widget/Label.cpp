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
	bake();
}

void Label::font(std::string const& name) { WIDGET_M_FN_MARKER
	if(mFontPath != name) {
		mFontPath = name;
		mFont = nullptr;
		mBitmapFont = nullptr;
		mBitmap = nullptr;
		mRects.clear();
		mTexRects.clear();
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
void Label::onCalculateLayout(LayoutInfo& info) { WIDGET_M_FN_MARKER
	if(mBitmapFont) {
		info.miny = info.prefy = mBitmapFont->metrics().lineHeight;
		if(!mRects.empty()) {
			info.minx = info.prefx = mRects[mRects.size() - 4];
		}
	}
	else {
		Widget::onCalculateLayout(info);
	}
}
void Label::onDraw(Canvas& canvas) {
	WIDGET_ENABLE_MARKERS

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
