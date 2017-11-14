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

	uint32_t mFlags;

	std::shared_ptr<Canvas> mCanvas;
public:
	enum Flags {
		FlagDoublebuffered = 1,
		FlagVsync          = 2,
		FlagAntialias      = 4,
		FlagRelative       = 8,
		FlagUpdateOnEvent  = 16,
		FlagAnaglyph3d     = 32 // Anaglyphic 3d support. Can be helpful for debugging. EXPERIEMENTAL
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
