#pragma once

#include <wwidget/Canvas.hpp>

extern "C" {
	#include <cairo/cairo.h>
}

namespace wwidget {

class CanvasCairo : public Canvas {
	cairo_t* m_ctxt;
public:
};

} // namespace wwidget