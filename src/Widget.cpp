#include "../include/widget/Widget.hpp"
#include "../include/widget/Error.hpp"
#include "../include/widget/Canvas.hpp"
#include "../include/widget/Attribute.hpp"

#include <cstring>
#include <cmath>
#include <cassert> // assert

namespace widget {

Widget::Widget() :
	mWidth(20),
	mHeight(20),
	mOffsetX(0),
	mOffsetY(0),
	mAlignX(AlignDefault),
	mAlignY(AlignDefault),

	mParent(nullptr),
	mNextSibling(nullptr),
	mPrevSibling(nullptr),
	mChildren(nullptr)
{
	mFlags[FlagNeedsRelayout] = true;
}

Widget::~Widget() noexcept {
	remove().release();
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
		onChildPreferredSizeChanged(newChild);
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
		if(auto ownership = w->remove()) {
			add(std::move(ownership));
			return;
		}
	}

	w->mParent      = this;
	Widget* end = lastChild();
	if(!end) {
		mChildren = w;
	}
	else {
		end->mNextSibling = w;
		w->mPrevSibling = end;
	}

	notifyChildAdded(w);
}

void Widget::add(Widget& w) {
	add(&w);
}

Widget* Widget::add(std::unique_ptr<Widget>&& w) { WIDGET_M_FN_MARKER
	add(w.get());
	w->mFlags[FlagOwnedByParent] = true;
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


	Widget* childrenFirst = mChildren;
	Widget* childrenLast;

	mChildren = nullptr;

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

	mParent->notifyChildRemoved(this);

	mParent      = nullptr;

	if(mFlags[FlagOwnedByParent]) {
		return std::unique_ptr<Widget>(this);
	}

	return nullptr;
}

std::unique_ptr<Widget> Widget::remove() { WIDGET_M_FN_MARKER
	if(mParent) {
		auto* parent = mParent;
		auto a       = quietRemove();
		parent->notifyChildRemoved(this);
		return a;
	}
	return nullptr;
}

std::unique_ptr<Widget> Widget::quietRemove() { WIDGET_M_FN_MARKER
	if(mParent) {
		if(!mPrevSibling) {
			assert(mParent->children() == this);
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
	else {
		assert(!mNextSibling);
		assert(!mPrevSibling);
	}

	return getOwnership();
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

Widget* Widget::findRoot() {
	Widget* root = this;
	while(root->parent()) root = root->parent();
	return root;
}

void Widget::clearChildren() { WIDGET_M_FN_MARKER
	while(mChildren) {
		mChildren->remove();
	}
}

Widget* Widget::lastChild() const noexcept {
	if(!mChildren) {
		return nullptr;
	}

	Widget* w = children();
	while(w->nextSibling()) w = w->nextSibling();
	return w;
}

// Tree changed events
void Widget::onAddTo(Widget* w) { WIDGET_M_FN_MARKER }
void Widget::onRemovedFrom(Widget* parent) { WIDGET_M_FN_MARKER }

void Widget::onAdd(Widget* w) { WIDGET_M_FN_MARKER
	preferredSizeChanged();
	requestRelayout();
}
void Widget::onRemove(Widget* w) { WIDGET_M_FN_MARKER
	preferredSizeChanged();
	requestRelayout();
}

// Layout events
void Widget::onResized() {
	requestRelayout();
}

void Widget::onChildPreferredSizeChanged(Widget* child) { WIDGET_M_FN_MARKER
	if(mParent) {
		preferredSizeChanged();
		requestRelayout();
	}
	else {
		requestRelayout();
	}
}
void Widget::onChildAlignmentChanged(Widget* child) { WIDGET_M_FN_MARKER
	AlignChild(child, 0, 0, width(), height());
}
void Widget::onCalculateLayout(LayoutInfo& info) { WIDGET_M_FN_MARKER
	info = calcOverlappingLayout(width(), height());
}
void Widget::onLayout() { WIDGET_M_FN_MARKER
	eachChild([&](Widget* child) {
		LayoutInfo info;
		child->getLayoutInfo(info);
		float x = child->alignx() == AlignFill ? width() : info.prefx;
		float y = child->aligny() == AlignFill ? height() : info.prefy;
		child->size(x, y);
		AlignChild(child, 0, 0, width(), height());
	});
}

void Widget::onUpdate(float dt) { WIDGET_M_FN_MARKER }

LayoutInfo Widget::calcOverlappingLayout(float alt_prefx, float alt_prefy) {
	LayoutInfo info;
	if(!mChildren) {
		info.prefx = alt_prefx;
		info.prefy = alt_prefy;
	}
	else {
		info = LayoutInfo::MinMaxAccumulator();

		eachChild([&](Widget* child) {
			LayoutInfo subinfo;
			child->getLayoutInfo(subinfo);
			info.include(subinfo, child->offsetx(), child->offsety());
		});

		info.sanitize();
	}
	return info;
}

float Widget::GetAlignmentX(Widget* child, float min, float width) {
	switch (child->alignx()) {
		case AlignNone: return child->offsetx();
		case AlignFill:
		case AlignMin: return min;
		case AlignMax: return min + width - child->width();
		case AlignCenter: return roundf((min + (min + width - child->width())) * .5f);
	}
}
float Widget::GetAlignmentY(Widget* child, float min, float height) {
	switch (child->aligny()) {
		case AlignNone: return child->offsety();
		case AlignFill:
		case AlignMin: return min;
		case AlignMax: return min + height - child->height();
		case AlignCenter: return roundf((min + (min + height - child->height())) * .5f);
	}
}
void Widget::AlignChildX(Widget* child, float min, float width) {
	child->offsetx(GetAlignmentX(child, min, width));
}
void Widget::AlignChildY(Widget* child, float min, float height) {
	child->offsety(GetAlignmentY(child, min, height));
}
void Widget::AlignChild(Widget* child, float x, float y, float width, float height) {
	child->offset(
		GetAlignmentX(child, x, width),
		GetAlignmentY(child, y, height)
	);
}

// Input events
void Widget::on(Click  const& c) { WIDGET_M_FN_MARKER }
void Widget::on(Scroll const& s) { WIDGET_M_FN_MARKER }

// Drawing events
void Widget::onDrawBackground(Canvas& graphics) {
	graphics.fillRect(0, 0, width(), height(), rgba(119, 119, 119, 1));
}
void Widget::onDraw(Canvas& graphics) {
	graphics.outlineRect(0, 0, width(), height(), rgb(185, 71, 142));
}

static
Widget::Alignment _ParseAlignment(const char* c) {
	switch(c[0]) {
		case 'f': return Widget::AlignFill;
		case 'c': return Widget::AlignCenter;
		case 'm': switch (c[1]) {
			case 'i': case 'n': return Widget::AlignMin;
			case 'x': case 'a': return Widget::AlignMax;
			default: return Widget::AlignDefault;
		}
		case 'n': case '\0': return Widget::AlignNone;
		default: return Widget::AlignDefault;
	}
}

static
const char* _AlignmentToString(Widget::Alignment a) {
	switch (a) {
		case Widget::AlignNone:   return "none";
		case Widget::AlignMin:    return "min";
		case Widget::AlignMax:    return "max";
		case Widget::AlignCenter: return "center";
		case Widget::AlignFill:   return "fill";
	}
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
		size(std::stof(value), height()); return true;
	}
	if(s == "height") {
		size(width(), std::stof(value)); return true;
	}
	if(s == "x") {
		offset(std::stof(value), offsety()); alignx(AlignNone); return true;
	}
	if(s == "y") {
		offset(offsetx(), std::stof(value)); aligny(AlignNone); return true;
	}
	if(s == "align") {
		align(_ParseAlignment(value.c_str())); return true;
	}
	if(s == "alignx") {
		alignx(_ParseAlignment(value.c_str())); return true;
	}
	if(s == "aligny") {
		aligny(_ParseAlignment(value.c_str())); return true;
	}

	return false;
}

void Widget::getAttributes(widget::AttributeCollectorInterface& collector) {
	if(mFlags[FlagOwnedByParent])
		collector("dbg_FlagOwnedByParent", mFlags[FlagOwnedByParent]);
	if(mFlags[FlagChildNeedsRelayout])
		collector("dbg_FlagChildNeedsRelayout", mFlags[FlagChildNeedsRelayout]);
	if(mFlags[FlagNeedsRelayout])
		collector("dbg_FlagNeedsRelayout", mFlags[FlagNeedsRelayout]);
	collector("name", mName);
	{
		std::string result;
		size_t len = 0;
		for(auto& c : mClasses) len += c.size();
		result.reserve(len);
		for(auto& c : mClasses) result += c;
		collector("class", result);
	}
	collector("width", width());
	collector("height", height());
	collector("x", offsetx());
	collector("y", offsety());
	if(alignx() == aligny()) {
		collector("align", _AlignmentToString(alignx()));
	}
	else {
		collector("alignx", _AlignmentToString(alignx()));
		collector("aligny", _AlignmentToString(aligny()));
	}
}

bool Widget::send(Click const& click) { WIDGET_M_FN_MARKER
	if(click.x < 0 || click.x > width() ||
	   click.y < 0 || click.y > height())
	{ return false; }

	on(click);

	Widget* child = children();
	while(child && !click.handled) {
		float x = click.x;
		float y = click.y;
		click.x -= child->offsetx();
		click.y -= child->offsety();
		child->send(click);
		click.x = x;
		click.y = y;
		child = child->nextSibling();
	}
	return click.handled;
}
bool Widget::send(Scroll const& scroll) { WIDGET_M_FN_MARKER
	if(scroll.x < 0 || scroll.x > width() ||
	   scroll.y < 0 || scroll.y > height())
	{ return false; }

	on(scroll);

	Widget* child = children();
	while(child && !scroll.handled) {
		float x = scroll.x;
		float y = scroll.y;
		scroll.x -= child->offsetx();
		scroll.y -= child->offsety();
		child->send(scroll);
		scroll.x = x;
		scroll.y = y;
		child = child->nextSibling();
	}
	return scroll.handled;
}

void Widget::drawBackgroundRecursive(Canvas& canvas) {
	onDrawBackground(canvas);
	eachChild([&](Widget* w) {
		if(w->offsetx() > -w->width() && w->offsety() > -w->height() && w->offsetx() < width() && w->offsety() < height()) {
			canvas.pushArea(w->offsetx(), w->offsety(), w->width(), w->height());
			w->drawBackgroundRecursive(canvas);
			canvas.popArea();
		}
	});
}
void Widget::drawForegroundRecursive(Canvas& canvas) {
	eachChild([&](Widget* w) {
		if(w->offsetx() > -w->width() && w->offsety() > -w->height() && w->offsetx() < width() && w->offsety() < height()) {
			canvas.pushArea(w->offsetx(), w->offsety(), w->width(), w->height());
			w->drawForegroundRecursive(canvas);
			canvas.popArea();
		}
	});
	onDraw(canvas);
}

void Widget::draw(Canvas& canvas) {
	updateLayout();
	canvas.begin(offsetx(), offsety(), width(), height());
	drawBackgroundRecursive(canvas);
	drawForegroundRecursive(canvas);
	canvas.end();
}

bool Widget::updateLayout() {
	bool result = false;
	if(mFlags[FlagNeedsRelayout]) {
		result = true;
		forceRelayout();
	}
	if(mFlags[FlagChildNeedsRelayout]) {
		result = true;
		mFlags[FlagChildNeedsRelayout] = false;
		eachChild([](Widget* w) {
			w->updateLayout();
		});
		return true;
	}
	return result;
}

void Widget::update(float dt) { WIDGET_M_FN_MARKER
	eachPreOrder([=](Widget* w) {
		w->onUpdate(dt);
	});
}

void Widget::forceRelayout() { WIDGET_M_FN_MARKER
	if(!mParent) {
		LayoutInfo info;
		getLayoutInfo(info);
		size(info.prefx, info.prefy);
	}

	mFlags[FlagNeedsRelayout] = false;
	onLayout();

	if(mFlags[FlagChildNeedsRelayout]) {
		mFlags[FlagChildNeedsRelayout] = false;
		eachChild([](Widget* w) {
			w->updateLayout();
		});
	}
}

void Widget::requestRelayout() { WIDGET_M_FN_MARKER
	mFlags[FlagNeedsRelayout] = true;

	Widget* p = parent();
	while(p && !p->mFlags[FlagChildNeedsRelayout]) {
		p->mFlags[FlagChildNeedsRelayout] = true;
		p = p->parent();
	}
}

void Widget::preferredSizeChanged() { WIDGET_M_FN_MARKER
	if(parent()) {
		mParent->onChildPreferredSizeChanged(this);
	}
}

void Widget::alignmentChanged() { WIDGET_M_FN_MARKER
	if(parent()) {
		mParent->onChildAlignmentChanged(this);
	}
}

void Widget::getLayoutInfo(LayoutInfo& info) { WIDGET_M_FN_MARKER
	onCalculateLayout(info);
}

Widget* Widget::size(float w, float h) { WIDGET_M_FN_MARKER
	float dif = fabs(width()  - w) + fabs(height() - h);
	if(dif > 1) {
		mWidth  = w;
		mHeight = h;
		onResized();
	}
	return this;
}
Widget* Widget::width (float w) { return size(w, height()); }
Widget* Widget::height(float h) { return size(width(), h); }

Widget* Widget::offset(float x, float y) { WIDGET_M_FN_MARKER
	if(offsetx() != x || offsety() != y) {
		mOffsetX = x;
		mOffsetY = y;
	}
	return this;
}
Widget* Widget::offsetx(float x) { return offset(x, offsety()); }
Widget* Widget::offsety(float y) { return offset(offsetx(), y); }

Widget* Widget::align(Alignment x, Alignment y) {
	if(x != alignx() || y != aligny()) {
		mAlignX = x;
		mAlignY = y;
		alignmentChanged();
	}
	return this;
}
Widget* Widget::align(Alignment xy) { return align(xy, xy); }
Widget* Widget::alignx(Alignment x) { return align(x, alignx()); }
Widget* Widget::aligny(Alignment y) { return align(alignx(), y); }

} // namespace widget
