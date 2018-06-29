#include <wwidget.hpp>

using namespace wwidget;

int main(int argc, char const* argv[]) {
	// Load form
	Form form;
	form.align(AlignFill);

	const char* path = "example/test.form.xml";
	if(argc > 1)
		path = argv[1];
	form.load(path);

	// Open window
	Window window;
	window.open("Heyho");
	window.add(form);

	window.keepOpen();
}