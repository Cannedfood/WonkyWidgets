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

Label::~Label() {}

Label* Label::content(std::string const& s) { WIDGET_M_FN_MARKER
	mText = s;
	bake();
	return this;
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
void Label::getAttributes(AttributeCollectorInterface& collector) { WIDGET_M_FN_MARKER
	Widget::getAttributes(collector);
	collector("content", mText);
	collector("font", mFontPath);
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
	if(mBitmap) {
		// canvas.rects(mRects.data(), mTexRects.data(), mRects.size() / 8, mBitmap, rgb(255, 255, 255));
	}
}

} // namespace widget
