# Manual

This is a tutorial how to use the existing widgets in the library.
You can also find tutorial for more specialized topics here:
- How to embed the library in your application: [Embedding](Embedding.md)
- How to implement your own widgets: [Custom Widgets](Embedding.md)

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
