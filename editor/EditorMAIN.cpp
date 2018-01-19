#include "../include/widget/Window.hpp"
#include "../include/widget/debug/Dump.hpp"
#include "../include/widget/widget/Form.hpp"
#include "../include/widget/Canvas.hpp"
// #include "../include/widget/widget/Label.hpp"

#include "PropertyPane.hpp"
#include "TreePane.hpp"
#include "WysiwygPane.hpp"

#include <fstream>

namespace widget {

class Editor {
	Window mUi;

	Widget       mRightPane;
	PropertyPane mPropertyPane;
	TreePane     mTreePane;
	WysiwygPane  mMainPane;
public:
	Editor() :
		mUi("Widget editor", 800, 600, Window::FlagConstantSize | Window::FlagVsync | Window::FlagDoublebuffered)
	{
		mTreePane.align(Widget::AlignMin, Widget::AlignMax);
		mPropertyPane.align(Widget::AlignMax, Widget::AlignMin);

		mRightPane.add(mPropertyPane);
		mRightPane.add(mTreePane);
		mRightPane.align(Widget::AlignMax, Widget::AlignFill);

		mUi.add(mMainPane);
		mUi.add(mRightPane);

		mMainPane.onSelect =
		mTreePane.onSelect = [this](Widget* w) { onSelect(w); };

		mMainPane.onLoaded = [this](Widget* w) { onLoaded(w); };
	}

	~Editor() {}

	void onSelect(Widget* w) {
		mPropertyPane.currentWidget(w);
		mTreePane.select(w);
		mMainPane.select(w);
	}

	void onLoaded(Widget* w) {
		mPropertyPane.currentWidget(w);
		mTreePane.setWidget(w);
	}

	void run() {
		mMainPane.load("example/test.form.xml");

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
