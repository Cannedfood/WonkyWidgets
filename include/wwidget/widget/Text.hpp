#pragma once

#include "../Widget.hpp"
#include "../Canvas.hpp"

#include <vector>

namespace wwidget {

class Font;
class Bitmap;
class BitmapFont;

class Text : public Widget {
protected:
	std::shared_ptr<Font>       mFont;
	std::shared_ptr<BitmapFont> mBitmapFont;
	std::vector<Rect>           mRects;
	std::vector<Rect>           mTexRects;

	std::string mFontPath;

	std::string mText;

protected:
	void bake();
	void reloadFont();
	void font(std::shared_ptr<Font> font);

	void onDraw(Canvas& canvas) override;
	void onCalcPreferredSize(PreferredSize& info) override;
	void onAppletChanged() override;
	void onAddTo(Widget* p) override;
	void onRemovedFrom(Widget* p) override;
public:
	Text();
	Text(std::string content);
	Text(Widget* attachTo);
	Text(Widget* attachTo, std::string content);
	~Text();

	auto&  content() const noexcept { return mText; }
	Text* content(std::string s);
	void   font   (std::string const& name);

	bool setAttribute(std::string const& name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace wwidget
