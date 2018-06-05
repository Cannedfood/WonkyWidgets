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
	Owner                       mFontLoadTasks;
	std::shared_ptr<Font>       mFont;
	std::shared_ptr<BitmapFont> mBitmapFont;
	std::vector<Rect>           mRects;
	std::vector<Rect>           mTexRects;

	Color                       mFontColor;

	std::string mFontPath;

	std::string mText;

protected:
	void bake();
	void reloadFont();
	void font(std::shared_ptr<Font> font);

	void onDraw(Canvas& canvas) override;
	PreferredSize onCalcPreferredSize() override;
	void onContextChanged() override;
	void onAddTo(Widget* p) override;
	void onRemovedFrom(Widget* p) override;
public:
	Text();
	Text(std::string content);
	Text(Widget* attachTo);
	Text(Widget* attachTo, std::string content);
	~Text();

	Text(Text&& other) noexcept;
	Text& operator=(Text&& other) noexcept;

	auto&  content() const noexcept { return mText; }
	Text*  content(std::string s);
	void   font   (std::string const& name);

	Text* fontColor(Color c);
	Color fontColor();

	bool setAttribute(std::string_view name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace wwidget
