#pragma once

#include "Widget.hpp"

namespace widget {

class Font;

class Applet {
public:
	Applet();
	virtual ~Applet();

	virtual void defer(std::function<void()>) = 0;

	virtual void loadImage(std::function<void(std::shared_ptr<Bitmap>)>, std::string const& url) = 0;
	virtual void loadFont(std::function<void(std::shared_ptr<Font>)>, std::string const& url) = 0;

	virtual bool update() = 0;
	virtual void draw() = 0;
};

} // namespace widget
