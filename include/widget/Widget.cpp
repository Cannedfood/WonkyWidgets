#include "Widget.hpp"

#include "WidgetGraphics.hpp"
#include "UserInterface.hpp"

#include "Error.hpp"

namespace widget {

Widget::Widget() :
	mArea(20, PX),

	mParent(nullptr),
	mNextSibling(nullptr),
	mPrevSibling(nullptr),
	mChildren(nullptr),

	mGraphics(nullptr)
{}

Widget::~Widget() noexcept {
	remove();

	if(mGraphics) delete mGraphics;
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

void Widget::extract() {
	if(!mParent) {
		throw exceptions::InvalidOperation("Tried extracting widget without parent.");
	}

	if(!mChildren) {
		remove();
		return;
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
}

void Widget::remove() {
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
	}
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

void Widget::onAddTo(Widget* w) {}
void Widget::onRemovedFrom(Widget* parent) {}

void Widget::onAdd(Widget* w) {}
void Widget::onRemove(Widget* w) {}

} // namespace widget
