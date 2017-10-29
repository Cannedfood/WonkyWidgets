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
	bool  mRelative;
public:
	enum {
		DOUBLEBUFFERED = 1,
		VSYNC          = 2,
		ANTIALIASED    = 4,
		RELATIVE       = 8
	};

	Window();
	Window(const char* title, unsigned width = 800, unsigned height = 600, uint32_t flags = 0);
	virtual ~Window();

	void open(const char* title, unsigned width = 800, unsigned height = 600, uint32_t flags = 0);
	void close();

	void requestClose();

	bool update();

	void draw();
	void onDraw(Canvas& canvas) override;

	Mouse& mouse() { return mMouse; }

	inline bool relative() const noexcept { return mRelative; }
};

} // namespace widget
