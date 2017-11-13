#include "../include/widget/Widget.hpp"
#include "../include/widget/Error.hpp"
#include "../include/widget/Canvas.hpp"

#include <cstring>

namespace widget {

Widget::Widget() :
	mArea(20, PX),

	mParent(nullptr),
	mNextSibling(nullptr),
	mPrevSibling(nullptr),
	mChildren(nullptr)
{
	mFlags[FlagNeedsRelayout] = true;
}

Widget::~Widget() noexcept {
	remove();
	clearChildren();
}

// ** Move *******************************************************

Widget::Widget(Widget&& other) :
	Widget()
{
	*this = std::move(other);
}
Widget& Widget::operator=(Widget&& other) {
	remove();

	mParent = other.mParent;
	other.mParent = nullptr;
	if(mParent) {
		if(mParent->mChildren == this) {
			mParent->mChildren = this;
		}
	}

	mNextSibling = other.mNextSibling;
	other.mNextSibling = nullptr;
	if(mNextSibling) {
		mNextSibling->mPrevSibling = this;
	}

	mPrevSibling = other.mPrevSibling;
	other.mNextSibling = nullptr;
	if(mPrevSibling) {
		mPrevSibling->mNextSibling = this;
	}

	mChildren = other.mChildren;
	other.mChildren = nullptr;
	if(mChildren) {
		for(auto* w = children(); w; w = w->nextSibling()) {
			w->mParent = this;
		}
	}

	mFlags = other.mFlags;
	other.mFlags = 0;

	mName    = std::move(other.mName);
	mClasses = std::move(other.mClasses);

	return *this;
}

// ** Copy *******************************************************

Widget::Widget(Widget const& other) :
	Widget()
{
	*this = other;
}
Widget& Widget::operator=(Widget const& other) {
	mName    = other.mName; // TODO: Should the copy constructor copy the name?
	mClasses = other.mClasses;
	mFlags   = other.mFlags;
	return *this;
}

// ** Tree operations *******************************************************

void Widget::notifyChildAdded(Widget* newChild) {
	newChild->onAddTo(this);
	onAdd(newChild);
	if(newChild->needsRelayout()) {
		newChild->forceRelayout();
	}
}

void Widget::notifyChildRemoved(Widget* noLongerChild) {
	noLongerChild->onRemovedFrom(this);
	onRemove(noLongerChild);
}

void Widget::add(Widget* w) { WIDGET_M_FN_MARKER
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

Widget* Widget::add(std::unique_ptr<Widget>&& w) { WIDGET_M_FN_MARKER
	w->mFlags[FlagOwnedByParent] = true;
	add(w.get());
	return w.release();
}

void Widget::insertNextSibling(Widget* w) { WIDGET_M_FN_MARKER
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

void Widget::insertPrevSibling(Widget* w) { WIDGET_M_FN_MARKER
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

void Widget::insertAsParent(Widget* w) { WIDGET_M_FN_MARKER
	insertNextSibling(w);
	w->add(this);
}

std::unique_ptr<Widget> Widget::extract() { WIDGET_M_FN_MARKER
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
	mChildren    = nullptr;

	if(mFlags[FlagOwnedByParent]) {
		return std::unique_ptr<Widget>(this);
	}

	return nullptr;
}

std::unique_ptr<Widget> Widget::remove() { WIDGET_M_FN_MARKER
	if(mParent) {
		mParent->notifyChildRemoved(this);
		return quietRemove();
	}
	return nullptr;
}

std::unique_ptr<Widget> Widget::quietRemove() { WIDGET_M_FN_MARKER
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
	return nullptr;
}

std::unique_ptr<Widget> Widget::getOwnership() { WIDGET_M_FN_MARKER
	if(!mFlags[FlagOwnedByParent])
		return nullptr;
	mFlags[FlagOwnedByParent] = false;
	return std::unique_ptr<Widget>(this);
}

template<>
Widget* Widget::search<Widget>(const char* name) noexcept { WIDGET_M_FN_MARKER
	if(mName == name) {
		return this;
	}

	for(auto* c = mChildren; c; c = c->mNextSibling) {
		if(auto* result = c->search<Widget>(name))
			return result;
	}

	return nullptr;
}

template<>
Widget* Widget::searchParent<Widget>(const char* name) noexcept { WIDGET_M_FN_MARKER
	if(!mParent) return nullptr;

	Widget* p = parent();
	while(p) {
		if(p->name() == name) {
			return p;
		}
		p = p->parent();
	}

	return nullptr;
}

void Widget::clearChildren() { WIDGET_M_FN_MARKER
	while(mChildren) {
		mChildren->remove();
	}
}

// Tree changed events
void Widget::onAddTo(Widget* w) { WIDGET_M_FN_MARKER }
void Widget::onRemovedFrom(Widget* parent) { WIDGET_M_FN_MARKER }

void Widget::onAdd(Widget* w) { WIDGET_M_FN_MARKER }
void Widget::onRemove(Widget* w) { WIDGET_M_FN_MARKER }

// Layout events
void Widget::onChildPreferredSizeChanged(Widget* child) { WIDGET_M_FN_MARKER
	requestRelayout();
}
void Widget::onCalculateLayout(LayoutInfo& info) { WIDGET_M_FN_MARKER
	if(!mChildren) {
		info.prefx = area().width;
		info.prefy = area().height;
	}
	else {
		info = LayoutInfo::MinMaxAccumulator();

		eachChild([&](Widget* child) {
			LayoutInfo subinfo;
			child->getLayoutInfo(subinfo);
			info.include(subinfo, child->area().x, child->area().y);
		});

		info.sanitize();
	}
}
void Widget::onLayout() { WIDGET_M_FN_MARKER
	eachChild([&](Widget* child) {
		LayoutInfo info;
		child->getLayoutInfo(info);
		child->size(info.prefx, info.prefy);
	});
}

void Widget::onUpdate(float dt) { WIDGET_M_FN_MARKER }

// Input events
void Widget::on(Click const& c) { WIDGET_M_FN_MARKER }

// Drawing events
void Widget::onDrawBackground(Canvas& graphics) {
	graphics.fillRect(0, 0, area().width, area().height, rgba(119, 119, 119, 1));
}
void Widget::onDraw(Canvas& graphics) {
	graphics.outlineRect(0, 0, area().width, area().height, rgb(185, 71, 142));
}

// Attributes
bool Widget::setAttribute(std::string const& s, std::string const& value) { WIDGET_M_FN_MARKER
	if(s == "name") {
		mName = value; return true;
	}
	if(s == "class") {
		mClasses.emplace(value); return true;
	}
	if(s == "width") {
		size(std::stof(value), area().height); return true;
	}
	if(s == "height") {
		size(area().width, std::stof(value)); return true;
	}
	if(s == "x") {
		position(std::stof(value), area().y); return true;
	}
	if(s == "y") {
		position(area().x, std::stof(value)); return true;
	}

	return false;
}

bool Widget::send(Click const& click) { WIDGET_M_FN_MARKER
	eachChildConditional([&](Widget* child) -> bool {
		if(child->area().contains(click.x, click.y)) {
			float x = click.x;
			float y = click.y;
			click.x -= child->area().x;
			click.y -= child->area().y;
			child->on(click);
			child->send(click);
			click.x = x;
			click.y = y;
			return !click.handled;
		}
		return true;
	});
	return click.handled;
}

void Widget::drawBackground(Canvas& canvas) {
	onDrawBackground(canvas);
	eachChild([&](Widget* w) {
		canvas.pushArea(w->area().x, w->area().y, w->area().width, w->area().height);
		w->drawBackground(canvas);
		canvas.popArea();
	});
}
void Widget::drawForeground(Canvas& canvas) {
	eachChild([&](Widget* w) {
		canvas.pushArea(w->area().x, w->area().y, w->area().width, w->area().height);
		w->drawForeground(canvas);
		canvas.popArea();
	});
	onDraw(canvas);
}

void Widget::draw(Canvas& canvas) {
	drawBackground(canvas);
	drawForeground(canvas);
}

void Widget::update(float dt) { WIDGET_M_FN_MARKER
	eachPreOrder([=](Widget* w) {
		w->onUpdate(dt);
	});
}

void Widget::forceRelayout() { WIDGET_M_FN_MARKER
	mFlags[FlagNeedsRelayout] = false;

	if(!mParent) {
		LayoutInfo info;
		getLayoutInfo(info);
		size(info.prefx, info.prefy);
	}

	onLayout();

	mFlags[FlagNeedsRelayout] = false;
}

void Widget::requestRelayout() { WIDGET_M_FN_MARKER
	mFlags[FlagNeedsRelayout] = true;
	if(mParent) {
		forceRelayout();
	}
	// else: No need to layout yet
}

void Widget::preferredSizeChanged() { WIDGET_M_FN_MARKER
	if(parent()) {
		mParent->onChildPreferredSizeChanged(this);
	}
}

void Widget::getLayoutInfo(LayoutInfo& info) { WIDGET_M_FN_MARKER
	onCalculateLayout(info);
}

} // namespace widget
