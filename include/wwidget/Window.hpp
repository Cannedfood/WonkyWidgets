#pragma once

#include "BasicApplet.hpp"

#include <stdexcept>

namespace wwidget {

namespace exceptions {

struct FailedOpeningWindow : public std::runtime_error {
	FailedOpeningWindow(std::string const& val) : std::runtime_error(val) {}
};

} // namespace exception

class Window : public BasicApplet {
	void* mWindowPtr;

	Mouse mMouse;
	uint32_t mFlags;

	void onCalculateLayout(LayoutInfo& info) override;
	void onResized() override;
	void onDrawBackground(Canvas& c) override;
public:
	enum Flags {
		FlagDoublebuffered = 1,
		FlagVsync          = 2,
		FlagAntialias      = 4,
		FlagRelative       = 8,
		FlagUpdateOnEvent  = 16,
		FlagAnaglyph3d     = 32, //!< Anaglyphic 3d support. Can be helpful for debugging. EXPERIEMENTAL
		FlagDrawDebug      = 64,
		FlagConstantSize   = 128
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

	void draw() override;
	void onDraw(Canvas& canvas) override;

	Mouse& mouse() { return mMouse; }

	inline bool relative() const noexcept { return mFlags & FlagRelative; }

	bool hasConstantSize() { return mFlags & FlagConstantSize; }
};

} // namespace wwidget
