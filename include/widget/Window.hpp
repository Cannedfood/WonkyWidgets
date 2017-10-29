#pragma once

#include "Widget.hpp"

#include <xevent>
#include <stdexcept>

namespace widget {

namespace exceptions {

struct FailedOpeningWindow : public std::runtime_error {
	FailedOpeningWindow(std::string const& val) : std::runtime_error(val) {}
};

} // namespace exception

class Window : public Widget {
	void* mWindowPtr;

	Mouse mMouse;
public:
	enum {
		DOUBLEBUFFERED = 1,
		VSYNC          = 2
	};

	Window();
	Window(const char* title, unsigned width = 800, unsigned height = 600, uint32_t flags = 0);
	virtual ~Window();

	void open(const char* title, unsigned width = 800, unsigned height = 600, uint32_t flags = 0);
	void close();

	bool update();

	void draw();
	void onDraw(Canvas& canvas) override;

	Mouse& mouse() { return mMouse; }
};

} // namespace widget
