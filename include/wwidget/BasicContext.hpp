#pragma once

#include "Context.hpp"

namespace wwidget {

class Font;

class BasicContext : public Context {
	struct Implementation;

	Implementation* mImpl;
public:
	BasicContext();
	~BasicContext();

	void cleanCache();

	void defer(std::function<void()>) override;

	void loadImage(std::function<void(std::shared_ptr<Bitmap>)>, std::string const& url) override;
	void loadFont(std::function<void(std::shared_ptr<Font>)>, std::string const& url) override;

	std::shared_ptr<Bitmap> loadImage(std::string const& url) override;
	std::shared_ptr<Font>   loadFont(std::string const& url) override;

	void execute(Widget* from, std::string_view cmd) override;
	void execute(Widget* from, std::string_view const* cmds, size_t count) override;

	bool update() override;
	void draw() override;

	void rootWidget(Widget* w);
	Widget* rootWidget();

	void canvas(std::shared_ptr<Canvas> c) noexcept;
	std::shared_ptr<Canvas> const& canvas() const noexcept;
};

} // namespace wwidget
