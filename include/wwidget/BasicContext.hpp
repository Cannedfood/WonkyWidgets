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

	void loadImage(std::function<void(shared<Bitmap>)>, std::string const& url) override;

	shared<Bitmap> loadImage(std::string const& url) override;

	void execute(Widget* from, std::string_view cmd) override;
	void execute(Widget* from, std::string_view const* cmds, size_t count) override;

	bool update() override;
	void draw() override;

	void rootWidget(Widget* w);
	Widget* rootWidget();

	void canvas(shared<Canvas> c) noexcept;
	Canvas& canvas() const noexcept override;
};

} // namespace wwidget
