#pragma once

namespace widget {

class Canvas;

class CanvasProvider {
public:
	virtual Canvas* canvas() = 0;
};

} // namespace widget
