#include <wwidget/Window.hpp>
#include <wwidget/widget/Form.hpp>
#include <wwidget/widget/List.hpp>
#include <wwidget/widget/Button.hpp>
#include <wwidget/widget/Slider.hpp>
#include <wwidget/widget/TextField.hpp>

#define WIDGETS_ON_STACK

// =============================================================
// == Widgets on stack =============================================
// =============================================================
void widgets_on_stack() {
	using namespace wwidget;

	Window w;
	w.open("Hello", 800, 600);

	List rootLayout(w);
	rootLayout.align(Widget::AlignCenter);

	Button btn(rootLayout, "Click me!");
	btn.onClick([&]() {
		btn.text("Thank you.");
	});

	w.keepOpen();
}

// =============================================================
// == Widgets on heap =============================================
// =============================================================
void widgets_on_heap1() {
	using namespace wwidget;

	Window w;
	w.open("Hello", 800, 600);

	List* rootLayout = w.add<List>();

	rootLayout->add<Button>("Click me!")
		->onClick([](Button* b) {
			b->text("Thank you.");
		});

	w.keepOpen();
}

// =============================================================
// == Forms =============================================
// =============================================================
void widgets_via_forms() {
	using namespace wwidget;

	Window w;
	w.open("Hello", 800, 600);

	// Parse a layout via parse, you can also read it from a file via Form::load
	Form& form = w.add<Form>()->addDefaultFactories().parse(R"(
		<form>
			<button text='Click me!' name='button1' align='center'/>
		</form>
	)");

	form.find<Button>("button1")->onClick([](Button* b) { b->text("Thank you."); });

	w.keepOpen();

	w.keepOpen();
}

// =============================================================
// == On heap: Advanced =============================================
// =============================================================
void widgets_on_heap2() {
	using namespace wwidget;

	Window w;
	w.open("Hello", 800, 600);

	List* rootLayout = w.add<List>();

	rootLayout->add<Button>("Click me!")
		->onClick([](Button* b) {
			b->text("Thank you.");
		});

	w.keepOpen();
}

#include <cstring>

int main(int argc, char const** argv) {
	using namespace wwidget;

	if(argc == 1 || !strcmp(argv[1], "stack"))
		widgets_on_stack();
	else if(!strcmp(argv[1], "heap"))
		widgets_on_heap1();
	else if(!strcmp(argv[1], "forms"))
		widgets_via_forms();
	else if(!strcmp(argv[1], "heap2"))
		widgets_on_heap1();
	else
		return -1;

	return 0;
}
