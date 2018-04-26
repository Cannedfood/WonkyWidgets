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

	void onCalcPreferredSize(PreferredSize& info) {
		info.minx = info.prefx = info.maxx = width();
		info.miny = info.prefy = info.maxy = height();
	}
};

WysiwygPane::WysiwygPane() :
	mForm(nullptr),
	mMarker(new WysiwygMarker),
	mSelected(nullptr)
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

template<class C>
Widget* deepestChild(Widget* w, C&& callback) {
	if(!w) return nullptr;

	Widget* result = nullptr;

	Widget* child = w->children();
	while(child) {
		if(callback(child)) {
			result = child;
			child = child->children();
		}
		else {
			child = child->nextSibling();
		}
	}

	return result;
}
template<class C>
Widget* nextSibling(Widget* w, C&& callback) {
	if(!w) return nullptr;

	for(Widget* next = w->nextSibling(); next; next = next->nextSibling()) {
		if(callback(next)) {
			return next;
		}
	}

	return nullptr;
}

template<class C>
Widget* depthFirstNext(Widget* w, C&& callback) {
	if(!w || !callback(w))
		return nullptr;

	if(auto* result = nextSibling(w, callback)) return result;

	return w->parent();
}

void WysiwygPane::on(Click const& c) {
	if(c.button != 2) return;
	c.handled = true;
	if(!c.down()) return;

	auto containsCursor = [this, &c](Widget* w) -> bool {
		float x, y;
		w->absoluteOffset(x, y, this);
		return
			x <= c.x && c.x <= x + w->width() &&
			y <= c.y && c.y <= y + w->height();
	};

	if(mSelected) {
		Widget* result = depthFirstNext(mSelected, containsCursor);
		select(result == mForm ? nullptr : result);
	}

	if(!mSelected) {
		select(deepestChild(this, containsCursor));
	}
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
