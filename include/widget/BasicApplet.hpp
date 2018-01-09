#pragma once

#include "Applet.hpp"
#include "async/Queue.hpp"

namespace widget {

class Font;

class BasicApplet : public Applet {
	struct Cache;

	Cache* mCache;

	TaskQueue mUpdateTasks;
	std::shared_ptr<Canvas> mCanvas;

	std::string mDefaultFont;
protected:
	void canvas(std::shared_ptr<Canvas> c) noexcept { mCanvas = std::move(c); }
	std::shared_ptr<Canvas> const& canvas() const noexcept { return mCanvas; }
public:
	BasicApplet();
	~BasicApplet();

	void cleanCache();

	void defer(std::function<void()>) override;

	void loadImage(std::function<void(std::shared_ptr<Bitmap>)>, std::string const& url) override;
	void loadFont(std::function<void(std::shared_ptr<Font>)>, std::string const& url) override;

	bool update() override;
	void draw() override;
};

} // namespace widget
