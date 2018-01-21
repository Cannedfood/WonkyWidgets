#include "WysiwygPane.hpp"

#include "../include/wwidget/Canvas.hpp"
#include "../include/wwidget/widget/Form.hpp"

namespace wwidget {

class WysiwygMarker : public Widget {
public:
	WysiwygMarker() { align(AlignNone); }

	void copyPosition(Widget* w) {
		float x, y;
		w->absoluteOffset(x, y, parent());
		offset(x, y);
		size(w->width(), w->height());
	}

	void onDraw(Canvas& canvas) {
		canvas.box({width(), height()}, Color::eyecancer2());
	}

	void onCalculateLayout(LayoutInfo& info) {
		info.minx = info.prefx = info.maxx = width();
		info.miny = info.prefy = info.maxy = height();
	}
};

WysiwygPane::WysiwygPane() :
	mForm(nullptr),
	mMarker(new WysiwygMarker)
{}
WysiwygPane::~WysiwygPane() {
	delete mMarker;
}

void WysiwygPane::select(Widget *w) {
	if(mSelected == w) return;

	mSelected = w;
	if(onSelect)
		onSelect(w);

	if(!w) {
		mMarker->remove();
	}
	else {
		if(!mMarker->parent()) add(mMarker);
		mMarker->copyPosition(w);
	}
}

void WysiwygPane::load(std::string const& path) {
	unload();
	mForm = add<Form>(path);
	if(onLoaded)
		onLoaded(mForm);
}

void WysiwygPane::unload() {
	if(mForm) {
		if(onLoaded)
			onLoaded(nullptr);
		delete mForm;
		mForm = nullptr;
	}
}

void WysiwygPane::on(Dragged const& d) {
	if(d.buttons[2]) d.handled = true;
}
void WysiwygPane::on(Click const& c) {
	if(c.button != 2) return;
	c.handled = true;
	if(!c.down()) return;

	puts("Boom!");
}
void WysiwygPane::on(KeyEvent const& k) {
	switch (k.key) {
		default: return;
		case 256:
			if(k.state != KeyEvent::DOWN) break;
			select(nullptr);
			break;
	}
	k.handled = true;
}

} // namespace wwidget
