#pragma once

#include "../Widget.hpp"

#include <vector>

namespace wwidget {

class Text : public Widget {
protected:
	Color       mFontColor;
	float       mFontSize;
	std::string mFont;
	std::string mText;

protected:
	void onContextChanged() override;

	PreferredSize onCalcPreferredSize() override;
	void onDraw(Canvas& canvas) override;
public:
	Text();
	Text(std::string content);
	Text(Widget* attachTo);
	Text(Widget* attachTo, std::string content);
	~Text();

	Text(Text&& other) noexcept;
	Text& operator=(Text&& other) noexcept;

	Text* content(std::string s);
	auto& content() const noexcept { return mText; }
	Text* font   (std::string const& name);
	auto& font() const noexcept { return mFont; }
	Text* fontColor(Color const& c);
	auto& fontColor() const noexcept { return mFontColor; }
	Text* fontSize(float f);
	auto  fontSize() const noexcept { return mFontSize; }

	bool setAttribute(std::string_view name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace wwidget
