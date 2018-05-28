#pragma once

#include "Context.hpp"
#include "async/Queue.hpp"

namespace wwidget {

class Font;

class BasicContext : public Context, public Widget {
	struct Cache;

	Cache* mCache;

	TaskQueue mUpdateTasks;
	std::shared_ptr<Canvas> mCanvas;

	std::string mDefaultFont;
public:
	BasicContext();
	~BasicContext();

	void cleanCache();

	void defer(std::function<void()>) override;

	void loadImage(std::function<void(std::shared_ptr<Bitmap>)>, std::string const& url) override;
	void loadFont(std::function<void(std::shared_ptr<Font>)>, std::string const& url) override;

	bool update() override;
	void draw() override;

	void canvas(std::shared_ptr<Canvas> c) noexcept { mCanvas = std::move(c); }
	std::shared_ptr<Canvas> const& canvas() const noexcept { return mCanvas; }
};

} // namespace wwidget
