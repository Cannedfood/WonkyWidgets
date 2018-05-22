# Manual

This is a tutorial how to use the existing widgets in the library.
You can also find tutorial for more specialized topics here:
- How to embed the library in your application: [Embedding](Embedding.md)
- How to implement your own widgets: [Custom Widgets](CustomWidgets.md)

**WIP**

This should walk you through everything you should need to know.

## Minimal tutorial

```c++
#include <widget/Window.hpp>
#include <widget/widget/Button.hpp>

using namespace wwidget;

int main(int argc, char const** argv) {
	Window window = {
		"Title", 800, 600
	};

	Button button = { window, "Click! ME!" };
	button.onClick([&]() {
		button.content("Thank you.");
	});

	window.keepOpen();

	return 0;
}
```
