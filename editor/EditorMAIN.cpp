#include "../include/widget/Window.hpp"
#include "../include/widget/debug/Dump.hpp"
// #include "../include/widget/widget/Label.hpp"

#include "PropertyPane.hpp"

#include <fstream>

namespace widget {

class Editor {
	Window mUi;
	PropertyPane mPropertyPane;
public:
	Editor() :
		mUi("Widget editor", 800, 600, Window::FlagConstantSize | Window::FlagVsync)
	{
		mPropertyPane.currentWidget(&mPropertyPane);
		mUi.add(&mPropertyPane);
			// ->align(Widget::AlignMax, Widget::AlignMin);
		// mUi.add<Label>()->content("Hi");
	}

	~Editor() {}

	void run() {
		mUi.keepOpen();
		if(auto file = std::ofstream("editor.tmp.txt"))
			PrintDump(file, mUi);
	}
};

} // namespace widget

int main(int argc, char const** argv) {
	widget::Editor().run();
	return 0;
}
