#include "../include/wwidget/Window.hpp"
#include "../include/wwidget/debug/Dump.hpp"
#include "../include/wwidget/widget/Form.hpp"
#include "../include/wwidget/Canvas.hpp"
// #include "../include/wwidget/widget/Label.hpp"

#include "PropertyPane.hpp"
#include "TreePane.hpp"
#include "WysiwygPane.hpp"

#include <fstream>

namespace wwidget {

class Editor {
	Window mUi;

	Widget       mRightPane;
	PropertyPane mPropertyPane;
	TreePane     mTreePane;
	WysiwygPane  mMainPane;
public:
	Editor() :
		mUi("Widget editor", 800, 600)
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

} // namespace wwidget

int main(int argc, char const** argv) {
	wwidget::Editor().run();
	return 0;
}
