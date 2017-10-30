#include "../include/widget/Widget.hpp"
#include "../include/widget/Error.hpp"
#include "../include/widget/Canvas.hpp"

namespace widget {

Widget::Widget() :
	mArea(20, PX),

	mParent(nullptr),
	mNextSibling(nullptr),
	mPrevSibling(nullptr),
	mChildren(nullptr)
{}

Widget::~Widget() noexcept {
	remove();
}

void Widget::notifyChildAdded(Widget* newChild) {
	newChild->onAddTo(this);
	onAdd(newChild);
}

void Widget::notifyChildRemoved(Widget* noLongerChild) {
	noLongerChild->onRemovedFrom(this);
	onRemove(noLongerChild);
}

void Widget::add(Widget* w) {
	if(!w) {
		throw exceptions::InvalidPointer("w");
	}

	if(w->mParent) {
		w->remove();
	}

	w->mParent      = this;
	w->mNextSibling = mChildren;
	if(mChildren) {
		mChildren->mPrevSibling = w;
	}
	mChildren = w;

	notifyChildAdded(w);
}

Widget* Widget::add(std::unique_ptr<Widget>&& w) {
	w->mFlags[FlagOwnedByParent] = true;
	add(w.get());
	return w.release();
}

void Widget::insertNextSibling(Widget* w) {
	if(!mParent) {
		throw exceptions::RootNodeSibling();
	}

	if(w->mParent) {
		w->remove();
	}

	w->mNextSibling = mNextSibling;
	if(mNextSibling) {
		mNextSibling->mPrevSibling = w;
	}

	w->mPrevSibling = this;
	mNextSibling = w;

	w->mParent = mParent;

	mParent->notifyChildAdded(w);
}

void Widget::insertPrevSibling(Widget* w) {
	if(!mParent) {
		throw exceptions::RootNodeSibling();
	}

	if(w->mParent) {
		w->remove();
	}

	w->mPrevSibling = mPrevSibling;
	if(mPrevSibling) {
		mPrevSibling->mNextSibling = w;
	}
	else {
		mParent->mChildren = w;
	}

	w->mNextSibling = this;
	mPrevSibling = w;

	w->mParent = mParent;

	mParent->notifyChildAdded(w);
}

void Widget::insertAsParent(Widget* w) {
	insertNextSibling(w);
	w->add(this);
}

std::unique_ptr<Widget> Widget::extract() {
	if(!mParent) {
		throw exceptions::InvalidOperation("Tried extracting widget without parent.");
	}

	if(!mChildren) {
		return remove();
	}

	mParent->notifyChildRemoved(this);

	Widget* childrenFirst = mChildren;
	Widget* childrenLast;

	for(childrenLast = childrenFirst; childrenLast->mNextSibling; childrenLast = childrenLast->mNextSibling) {
		notifyChildRemoved(childrenLast);
		childrenLast->mParent = mParent;
	}
	notifyChildRemoved(childrenLast);
	childrenLast->mParent = mParent;

	if(mPrevSibling) {
		mPrevSibling->mNextSibling  = childrenFirst;
		childrenFirst->mPrevSibling = mPrevSibling;
		mNextSibling->mPrevSibling  = childrenLast;
		childrenLast->mNextSibling  = mNextSibling;
	}
	else {
		mParent->mChildren          = childrenFirst;
		childrenFirst->mPrevSibling = nullptr;
		childrenLast->mNextSibling  = nullptr;
	}

	for(auto* p = childrenFirst; p != childrenLast->mNextSibling; p = p->mNextSibling) {
		notifyChildAdded(p);
	}

	mNextSibling = nullptr;
	mPrevSibling = nullptr;
	mParent      = nullptr;

	if(mFlags[FlagOwnedByParent]) {
		return std::unique_ptr<Widget>(this);
	}

	return nullptr;
}

std::unique_ptr<Widget> Widget::remove() {
	if(mParent) {
		mParent->notifyChildRemoved(this);

		if(!mPrevSibling) {
			mParent->mChildren = mNextSibling;
			if(mNextSibling) {
				mNextSibling->mPrevSibling = nullptr;
			}
		}
		else {
			if(mNextSibling) {
				mNextSibling->mPrevSibling = mPrevSibling;
			}
			mPrevSibling->mNextSibling = mNextSibling;
		}

		mNextSibling = nullptr;
		mPrevSibling = nullptr;
		mParent      = nullptr;

		if(mFlags[FlagOwnedByParent]) {
			return std::unique_ptr<Widget>(this);
		}
	}
	return nullptr;
}

template<>
Widget* Widget::search<Widget>(const char* name) noexcept {
	if(mName == name) {
		return this;
	}

	for(auto* c = mChildren; c; c = c->mNextSibling) {
		if(auto* result = c->search<Widget>(name))
			return result;
	}

	return nullptr;
}

// Tree changed events
void Widget::onAddTo(Widget* w) {}
void Widget::onRemovedFrom(Widget* parent) {}

void Widget::onAdd(Widget* w) {}
void Widget::onRemove(Widget* w) {}

// Layout events
void Widget::onChildRequestRelayout(Widget* child) {}
void Widget::onResized(float w, float h) {}

// Input events
void Widget::on(Click const& c) {}

// Drawing events
void Widget::onDrawBackground(Canvas& graphics) {
	graphics.fillRect(area().x, area().y, area().width, area().height, rgba(119, 119, 119, 1));
}
void Widget::onDraw(Canvas& graphics) {
	graphics.outlineRect(area().x, area().y, area().width, area().height, rgb(185, 71, 142));
}

bool Widget::send(Click const& click) {
	on(click);
	if(click.handled) return true;

	eachChildConditional([&](Widget* w) {
		if(!w->area().contains(click.x, click.y))
			return true;
		else
			return !w->send(click);
	});

	return click.handled;
}

void Widget::drawBackground(Canvas& canvas) {
	onDrawBackground(canvas);
	canvas.pushArea(area().x, area().y, area().width, area().height);
	eachChild([&](Widget* w) {
		w->drawBackground(canvas);
	});
	canvas.popArea();
}
void Widget::drawForeground(Canvas& canvas) {
	canvas.pushArea(area().x, area().y, area().width, area().height);
	eachChild([&](Widget* w) {
		w->drawForeground(canvas);
	});
	canvas.popArea();
	onDraw(canvas);
}

void Widget::draw(Canvas& canvas) {
	drawBackground(canvas);
	drawForeground(canvas);
}

} // namespace widget
