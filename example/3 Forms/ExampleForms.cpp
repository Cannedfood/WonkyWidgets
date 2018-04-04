#include <wwidget/Window.hpp>
#include <wwidget/widget/Form.hpp>
#include <wwidget/widget/Button.hpp>

int main(int argc, char const** argv) {
	using namespace wwidget;

	Window w("Example 3: Forms", 800, 600, Window::FlagVsync | Window::FlagDoublebuffered);

	w.add<Form>()->addDefaultFactories().parse(R"(
		<form>
			<button text='Click me!' name='button1' align='center'/>
		</form>
	)");

	w.find<Button>("button1")->onClick([](Button* b) { b->text("Thank you."); });

	w.keepOpen();

	return 0;
}
