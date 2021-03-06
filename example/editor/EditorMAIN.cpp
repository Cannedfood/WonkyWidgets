#include "../include/wwidget/Window.hpp"
#include "../include/wwidget/widget/Form.hpp"
#include "../include/wwidget/Canvas.hpp"
// #include "../include/wwidget/widget/Text.hpp"

#include "PropertyPane.hpp"
#include "TreePane.hpp"
#include "WysiwygPane.hpp"

#include <fstream>

namespace wwidget {

class Editor {
	Window mUi;

	Widget       mLeftPane;
	PropertyPane mPropertyPane;
	TreePane     mTreePane;
	WysiwygPane  mMainPane;
public:
	Editor() :
		mUi("Widget editor", 800, 600, Window::FlagAntialias)
	{
		mTreePane.align(AlignMin, AlignMax);
		mPropertyPane.align(AlignMin, AlignMin);

		mLeftPane.add({
			mPropertyPane,
			mTreePane
		});
		mLeftPane.align(AlignMin, AlignFill);

		mMainPane.align(AlignFill);

		mUi.add({
			mLeftPane,
			mMainPane
		});

		mMainPane.onSelect =
		mTreePane.onSelect = [this](shared<Widget> w) { onSelect(w); };

		mMainPane.onLoaded = [this](shared<Widget> w) { onLoaded(w); };
	}

	~Editor() {}

	void onSelect(shared<Widget> w) {
		mPropertyPane.currentWidget(w);
		mTreePane.select(w);
		mMainPane.select(w);
	}

	void onLoaded(shared<Widget> w) {
		mPropertyPane.currentWidget(w);
		mTreePane.setWidget(w);
	}

	void run() {
		mMainPane.load("example/test.form.xml");

		mUi.keepOpen();
	}
};

} // namespace wwidget

int main(int argc, char const** argv) {
	wwidget::Editor().run();
	return 0;
}
