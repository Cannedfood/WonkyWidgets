# One Page Manual

**WIP**

This should walk you through everything you should need to know.

## Minimal tutorial

```c++
#include <widget/Window.hpp>

using namespace widget;

int main(int argc, char const** argv) {
	Window window ("Title", 800, 600, Window::VSYNC | Window::DOUBLEBUFFERED);

	Button button (window, "Click! ME!");
	button.onClick = [&]() {
		button.content("Thank you.");
	};

	while(window.update()) {
		window.draw();
	}

	return 0;
}
```
