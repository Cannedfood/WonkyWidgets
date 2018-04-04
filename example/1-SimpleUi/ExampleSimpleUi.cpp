#include <wwidget/Window.hpp>
#include <wwidget/widget/List.hpp>
#include <wwidget/widget/Button.hpp>
#include <wwidget/widget/Slider.hpp>
#include <wwidget/widget/TextField.hpp>

#define WIDGETS_ON_STACK

int main(int argc, char const** argv) {
	using namespace wwidget;

	Window w;
	w.open(
		"Hello", 800, 600,
		Window::FlagDoublebuffered |
		Window::FlagVsync
	);

#ifdef WIDGETS_ON_STACK
	// Variant 1: Widgets on stack

	List rootLayout(w);
	rootLayout.align(Widget::AlignCenter);

	Button btn(rootLayout, "Click me!");
	btn.onClick([&]() {
		btn.text("Thank you.");
	});

#else // WIDGETS_ON_STACK
	// Variant 2: Widgets on heap

	List* rootLayout = w.add<List>();

	rootLayout->add<Button>("Click me!")
		->onClick([](Button* b) {
			b->text("Thank you.");
		});

#endif // WIDGETS_ON_STACK


	w.keepOpen();

	// Variant 2: Widgets on heap

	return 0;
}
