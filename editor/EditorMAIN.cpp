#include "../include/widget/Window.hpp"
#include "../include/widget/debug/Dump.hpp"
#include "../include/widget/widget/Form.hpp"
#include "../include/widget/Canvas.hpp"
// #include "../include/widget/widget/Label.hpp"

#include "PropertyPane.hpp"
#include "TreePane.hpp"

#include <fstream>

namespace widget {

class Marker : public Widget {
public:
	Marker() { align(AlignNone); }

	void onDraw(Canvas& canvas) {
		canvas.box({width(), height()}, Color::eyecancer2());
	}

	void onCalculateLayout(LayoutInfo& info) {
		info.minx = info.prefx = info.maxx = width();
		info.miny = info.prefy = info.maxy = height();
	}
};

class Editor {
	Window mUi;

	Marker mMarker;

	Widget       mRightPane;
	PropertyPane mPropertyPane;
	TreePane     mTreePane;
	Form         mForm;
public:
	Editor() :
		mUi("Widget editor", 800, 600, Window::FlagConstantSize | Window::FlagVsync | Window::FlagDoublebuffered)
	{
		mTreePane.align(Widget::AlignMin, Widget::AlignMax);

		mRightPane.add(mPropertyPane);
		mRightPane.add(mTreePane);
		mRightPane.align(Widget::AlignMax, Widget::AlignFill);


		mTreePane.onSelect = [this](Widget* w) { select(w); };

		mUi.add(mForm);
		mUi.add(mRightPane);
		mUi.add(mMarker);
	}

	~Editor() {}

	void select(Widget* w) {
		mPropertyPane.currentWidget(w);
		float x, y;
		w->absoluteOffset(x, y, mUi);
		mMarker.offset(x, y);
		mMarker.size(w->width(), w->height());
	}

	void run() {
		mForm.load("example/test.form.xml");

		mPropertyPane.currentWidget(&mForm);
		mTreePane.setWidget(&mForm);

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
