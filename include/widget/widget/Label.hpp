#pragma once

#include "../Widget.hpp"

#include <vector>

namespace widget {

class Font;
class Bitmap;
class BitmapFont;

class Label : public Widget {
protected:
	std::shared_ptr<Font>       mFont;
	std::shared_ptr<BitmapFont> mBitmapFont;
	std::shared_ptr<Bitmap>     mBitmap;
	std::vector<float>          mRects;
	std::vector<float>          mTexRects;

	std::string mFontPath;

	std::string mText = "Hallo hier ist text.";

protected:
	void bake();
	void onDraw(Canvas& canvas) override;
	void onCalculateLayout(LayoutInfo& info) override;
public:
	Label();
	~Label();

	void content(std::string const& s);
	void font(std::string const& name);

	bool setAttribute(std::string const& name, std::string const& value) override;
};

} // namespace widget
