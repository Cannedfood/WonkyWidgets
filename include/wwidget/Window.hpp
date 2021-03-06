#pragma once

#include "BasicContext.hpp"

#include <stdexcept>

namespace wwidget {

namespace exceptions {

struct FailedOpeningWindow : public std::runtime_error {
	FailedOpeningWindow(std::string const& val) : std::runtime_error(val) {}
};

} // namespace exception

/// A desktop environment window.
class Window : public Widget, public BasicContext {
	void* mWindowPtr;

	Mouse mMouse;
	uint32_t mFlags;

protected:
	PreferredSize onCalcPreferredSize(PreferredSize const& constraint) override;
	void onResized() override;
	void onDrawBackground(Canvas& c) override;
	void onDraw(Canvas& canvas) override;

public:
	enum Flags {
		FlagSinglebuffered = 1,
		FlagNoVsync        = 2,
		FlagAntialias      = 4,
		FlagRelative       = 8,
		FlagUpdateOnEvent  = 16,
		FlagDrawDebug      = 32,
		FlagShrinkFit      = 64
	};

	Window();
	Window(const char* title, unsigned width = 800, unsigned height = 600, uint32_t flags = 0);
	virtual ~Window();

	void open(const char* title, unsigned width = 800, unsigned height = 600, uint32_t flags = 0);
	void close();

	void requestClose();

	bool update() override;

	/// Blocks and updates the window until it is closed
	void keepOpen();

	/// Draws the window with it's own canvas, dpi is calculated by default
	void draw(float dpi = -1) override;

	Mouse& mouse() { return mMouse; }

	inline bool relative() const noexcept { return mFlags & FlagRelative; }

	bool doesShrinkFit() const noexcept { return mFlags & FlagShrinkFit; }
};

} // namespace wwidget
