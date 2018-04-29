#include "../include/wwidget/Widget.hpp"

#include "../include/wwidget/Applet.hpp"

#include "../include/wwidget/Canvas.hpp"

#include "../include/wwidget/Error.hpp"
#include "../include/wwidget/AttributeCollector.hpp"

#include "../include/wwidget/async/OwnedTask.hpp"

#include "../include/wwidget/widget/Image.hpp"
#include "../include/wwidget/widget/Text.hpp"

#include <cstring>
#include <cmath>
#include <cassert> // assert
#include <sstream>

namespace wwidget {

Widget::Widget() noexcept :
	mPadding{0, 0, 0, 0},
	mSize(20),

	mParent(nullptr),
	mNextSibling(nullptr),
	mPrevSibling(nullptr),
	mChildren(nullptr),

	mApplet(nullptr)
{
	mFlags[FlagNeedsRelayout] = true;
	mFlags[FlagChildNeedsRedraw] = true;
	mFlags[FlagNeedsRedraw] = true;
}

Widget::~Widget() {
	remove().release();
	clearChildrenQuietly();
}

// ** Move *******************************************************

Widget::Widget(Widget&& other) noexcept :
	Widget()
{
	*this = std::move(other);
}
Widget& Widget::operator=(Widget&& other) noexcept {
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

Widget::Widget(Widget const& other) noexcept :
	Widget()
{
	*this = other;
}
Widget& Widget::operator=(Widget const& other) noexcept {
	mName    = other.mName; // TODO: Should the copy constructor copy the name?
	mClasses = other.mClasses;
	mFlags   = other.mFlags;
	return *this;
}

// ** Tree operations *******************************************************

void Widget::notifyChildAdded(Widget* newChild) {
	newChild->applet(applet());
	newChild->onAddTo(this);
	onAdd(newChild);
	if(newChild->needsRelayout()) {
		onChildPreferredSizeChanged(newChild);

		Widget* p = this;
		while(p && !p->mFlags[FlagChildNeedsRelayout]) {
			p->mFlags[FlagChildNeedsRelayout] = true;
			p = p->parent();
		}
	}
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
		if(auto ownership = w->remove()) {
			add(std::move(ownership));
			return;
		}
	}

	w->mParent  = this;
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

void Widget::add(std::initializer_list<Widget*> ptrs) {
	for(Widget* w : ptrs) add(w);
}

Widget* Widget::add(std::unique_ptr<Widget>&& w) {
	add(w.get());
	w->mFlags[FlagOwnedByParent] = true;
	return w.release();
}

Widget* Widget::insertNextSibling(Widget* w) {
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

	return w;
}

Widget* Widget::insertPrevSibling(Widget* w) {
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

	return w;
}

Widget* Widget::insertAsParent(Widget* w) {
	insertNextSibling(w);
	w->add(this);
	return w;
}

std::unique_ptr<Widget> Widget::extract() {
	if(!mParent) {
		throw exceptions::InvalidOperation("Tried extracting widget without parent.");
	}

	if(!mChildren) {
		return remove();
	}

	removeFocus();
	Owner::clearOwnerships();

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

std::unique_ptr<Widget> Widget::remove() {
	if(mParent) {
		auto* parent = mParent;
		auto a       = removeQuiet();
		parent->notifyChildRemoved(this);
		return a;
	}
	return nullptr;
}

std::unique_ptr<Widget> Widget::removeQuiet() {
	removeFocus();
	Owner::clearOwnerships();
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

	return acquireOwnership();
}

std::unique_ptr<Widget> Widget::acquireOwnership() noexcept {
	if(!mFlags[FlagOwnedByParent])
		return nullptr;
	mFlags[FlagOwnedByParent] = false;
	return std::unique_ptr<Widget>(this);
}

void Widget::giveOwnershipToParent() {
	if(mFlags[FlagOwnedByParent]) throw std::runtime_error("Already owned by parent");
	if(!parent()) throw std::runtime_error("No parent to give the ownership to");
	mFlags[FlagOwnedByParent] = true;
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

template<>
Widget* Widget::searchParent<Widget>(const char* name) const noexcept {
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

Widget* Widget::findRoot() const noexcept {
	Widget* root = const_cast<Widget*>(this);
	while(root->parent()) root = root->parent();
	return root;
}

void Widget::clearChildren() {
	while(mChildren) {
		mChildren->remove();
	}
}
void Widget::clearChildrenQuietly() {
	while(mChildren) {
		mChildren->removeQuiet();
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
void Widget::onAppletChanged() { }

void Widget::onAddTo(Widget* w) { }
void Widget::onRemovedFrom(Widget* parent) { }

void Widget::onAdd(Widget* w) {
	preferredSizeChanged();
	requestRelayout();
}
void Widget::onRemove(Widget* w) {
	preferredSizeChanged();
	requestRelayout();
}

// Layout events
void Widget::onResized() {
	requestRelayout();
}

void Widget::onChildPreferredSizeChanged(Widget* child) {
	if(mParent) {
		preferredSizeChanged();
		requestRelayout();
	}
	else {
		requestRelayout();
	}
}
void Widget::onChildAlignmentChanged(Widget* child) {
	AlignChild(child, 0, 0, width(), height());
}
PreferredSize Widget::onCalcPreferredSize() {
	return calcBoxAroundChildren(1, 1);
}
void Widget::onLayout() {
	eachChild([&](Widget* child) {
		PreferredSize info = child->getPreferredSize();
		float x = child->alignx() == AlignFill ? width() : std::min(width(), info.prefx);
		float y = child->aligny() == AlignFill ? height() : std::min(height(), info.prefy);
		child->size(x, y);
		AlignChild(child, 0, 0, width(), height());
	});
}

PreferredSize Widget::calcBoxAroundChildren(float alt_prefx, float alt_prefy) noexcept {
	PreferredSize info;
	if(!mChildren) {
		info.prefx = (alignx() == AlignFill) ? 0 : alt_prefx;
		info.prefy = (aligny() == AlignFill) ? 0 : alt_prefy;
	}
	else if(alignx() == AlignNone && aligny() == AlignNone) {
		info.minx = info.maxx = info.prefx = width();
		info.miny = info.maxy = info.prefy = height();
	}
	else {
		info = PreferredSize::MinMaxAccumulator();

		eachChild([&](Widget* child) {
			PreferredSize subinfo = child->getPreferredSize();

			float x = (child->alignx() == AlignNone) ? child->offsetx() : 0;
			float y = (child->aligny() == AlignNone) ? child->offsety() : 0;
			info.include(subinfo, x, y);
		});

		info.sanitize();

		if(alignx() == AlignNone)
			info.minx = info.maxx = info.prefy = width();

		if(aligny() == AlignNone)
			info.miny = info.maxy = info.prefy = height();
	}
	return info;
}

float Widget::GetAlignmentX(Widget* child, float min, float width) noexcept {
	switch (child->alignx()) {
		case AlignNone: return child->offsetx();
		case AlignFill:
		default:
		case AlignMin: return min + child->padLeft();
		case AlignMax: return min + width - (child->width() + child->padRight());
		case AlignCenter:
			return roundf((
				min + child->padLeft() +
				(min + width - (child->width() + child->padRight()))
			) * .5f);
	}
}
float Widget::GetAlignmentY(Widget* child, float min, float height) noexcept {
	switch (child->aligny()) {
		case AlignNone: return child->offsety();
		case AlignFill:
		default:
		case AlignMin: return min + child->padTop();
		case AlignMax: return min + height - (child->height() + child->padBottom());
		case AlignCenter:
			return roundf((
				min + child->padTop() +
				(min + height - (child->height() + child->padBottom()))
			) * .5f);
	}
}
void Widget::AlignChildX(Widget* child, float min, float width) noexcept {
	child->offsetx(GetAlignmentX(child, min, width));
}
void Widget::AlignChildY(Widget* child, float min, float height) noexcept {
	child->offsety(GetAlignmentY(child, min, height));
}
void Widget::AlignChild(Widget* child, float x, float y, float width, float height) noexcept {
	child->offset(
		GetAlignmentX(child, x, width),
		GetAlignmentY(child, y, height)
	);
}

// Input events
void Widget::on(Click const& c) {
	if(!focused()) {
		c.handled = requestFocus();
	}
}
void Widget::on(Scroll  const& s) { }
void Widget::on(Moved   const& c) { }
void Widget::on(Dragged const& s) { }
void Widget::on(KeyEvent  const& k) {
	if(k.scancode == 9 && focused()) {
		removeFocus();
	}
}
void Widget::on(TextInput const& t) { }

bool Widget::onFocus(bool b, float strength) { return !b; }

// Drawing events
void Widget::onDrawBackground(Canvas& graphics) {}
void Widget::onDraw(Canvas& graphics) {}

static
HalfAlignment _ParseHalfAlignment(const char* c) {
	switch(c[0]) {
		case 'f': return AlignFill;
		case 'c': return AlignCenter;
		case 'm': switch (c[1]) {
			case 'i': case 'n': return AlignMin;
			case 'x': case 'a': return AlignMax;
			default: return AlignDefault;
		}
		case 'n': case '\0': return AlignNone;
		default: return AlignDefault;
	}
}

static
Alignment _ParseAlignment(const char* c)
{
	Alignment result;
	switch (c[0]) {
		default: return _ParseHalfAlignment(c);
		case 'b': result.y = AlignMax; break;
		case 't': result.y = AlignMin; break;
		case 'f': result.y = AlignFill; break;
		case 'c': result.y = AlignCenter; break;
	}

	switch (c[1]) {
		default: return _ParseHalfAlignment(c);
		case 'l': result.x = AlignMin; break;
		case 'r': result.x = AlignMax; break;
		case 'f': result.x = AlignFill; break;
		case 'c': result.x = AlignCenter; break;
	}
	return result;
}

static
const char* _AlignmentToString(HalfAlignment a) {
	switch(a) {
		case AlignNone:   return "none";
		case AlignMin:    return "min";
		case AlignMax:    return "max";
		case AlignCenter: return "center";
		case AlignFill:   return "fill";
	}
	return "";
}

// Attributes
bool Widget::setAttribute(std::string const& s, std::string const& value) {
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
		align(_ParseHalfAlignment(value.c_str())); return true;
	}
	if(s == "alignx") {
		alignx(_ParseHalfAlignment(value.c_str())); return true;
	}
	if(s == "aligny") {
		aligny(_ParseHalfAlignment(value.c_str())); return true;
	}
	if(s == "padding") {
		char* cs = const_cast<char*>(value.c_str());

		// All around padding
		float a = strtof(cs, &cs);
		if(!*cs) { padding(a); return true; }
		cs++;
		if(!*cs) { padding(a); return true; }

		// X, Y padding
		float b = strtof(cs, &cs);
		if(!*cs) { padding(a, b); return true; }
		cs++;
		if(!*cs) { padding(a, b); return true; }

		float c = strtof(cs, &cs);
		if(!*cs) return false;
		cs++;
		if(!*cs) return false;

		// All around padding
		float d = strtof(cs, &cs);
		padding(a, b, c, d);

		return true;
	}
	if(s == "text") {
		text(value);
		return true;
	}
	if(s == "image") {
		image(value);
		return true;
	}

	return false;
}

void Widget::getAttributes(wwidget::AttributeCollectorInterface& collector) {
	if(!collector.startSection("wwidget::Widget")) return;

	{
		std::stringstream ss;
		ss << this;
		collector("dbg_Pointer", ss.str());
	}
	if(mFlags[FlagOwnedByParent])
		collector("dbg_FlagOwnedByParent", mFlags[FlagOwnedByParent], true);
	if(mFlags[FlagChildNeedsRelayout])
		collector("dbg_FlagChildNeedsRelayout", mFlags[FlagChildNeedsRelayout], true);
	if(mFlags[FlagNeedsRelayout])
		collector("dbg_FlagNeedsRelayout", mFlags[FlagNeedsRelayout], true);
	if(mFlags[FlagFocused])
		collector("dbg_FlagFocused", mFlags[FlagFocused], true);
	if(mFlags[FlagChildFocused])
		collector("dbg_FlagFocusedIndirectly", mFlags[FlagChildFocused], true);

	{
		PreferredSize info = getPreferredSize();
		collector("dbg_MinW", info.minx, true);
		collector("dbg_PrefW", info.prefx, true);
		collector("dbg_MaxW", info.maxx, true);
		collector("dbg_MinH", info.miny, true);
		collector("dbg_PrefH", info.prefy, true);
		collector("dbg_MaxH", info.maxy, true);
	}

	collector("name", mName, mName.empty());
	{
		std::string result;
		size_t len = 0;
		for(auto& c : mClasses) len += c.size();
		result.reserve(len);
		for(auto& c : mClasses) result += c;
		collector("class", result, result.empty());
	}
	collector("width", width());
	collector("height", height());
	collector("x", offsetx(), alignx() == AlignNone);
	collector("y", offsety(), aligny() == AlignNone);
	if(alignx() == aligny()) {
		collector("align", _AlignmentToString(alignx()));
	}
	else {
		collector("alignx", _AlignmentToString(alignx()));
		collector("aligny", _AlignmentToString(aligny()));
	}

	{
		bool padx = mPadding.left == mPadding.right;
		bool pady = mPadding.top == mPadding.bottom;

		if(padx && pady) {
			if(mPadding.left == mPadding.top)
				collector("padding", mPadding.left, mPadding.left == 0);
			else
				collector("padding", mPadding.left, mPadding.top);
		}
		else {
			collector(
				"padding",
				mPadding.left, mPadding.top,
				mPadding.right, mPadding.bottom,
				false
			);
		}
	}

	// TODO: text() and image()

	collector.endSection();
}

template<typename T>
bool Widget::sendEvent(T const& t, bool skip_focused) {
	if(t.x < 0 || t.x > width() ||
		 t.y < 0 || t.y > height())
	{ return false; }

	if(!(skip_focused && focused()))
		on(t);

	eachChildConditional([&](auto* child) -> bool {
		if(t.handled) return false;
		float x = t.x;
		float y = t.y;
		t.x -= child->offsetx();
		t.y -= child->offsety();
		child->sendEvent(t, skip_focused);
		t.x = x;
		t.y = y;
		return true;
	});
	return t.handled;
};

template<typename T>
bool Widget::sendEventToFocused(T const& t) {
	if(Widget* f = findFocused()) {
		float oldx = t.x, oldy = t.y;
		float x, y;
		f->absoluteOffset(x, y, this);
		t.x -= x;
		t.y -= y;
		f->on(t);
		t.x = oldx;
		t.y = oldy;
		return true;
	}
	return false;
}

bool Widget::send(Click const& click) {
	return sendEvent(click, sendEventToFocused(click));
}
bool Widget::send(Scroll const& scroll) {
	return sendEvent(scroll, sendEventToFocused(scroll));
}
bool Widget::send(Dragged const& drag) {
	return sendEvent(drag, sendEventToFocused(drag)) || send((Moved const&)drag);
}
bool Widget::send(Moved const& move) {
	return sendEvent(move, sendEventToFocused(move));
}
bool Widget::send(KeyEvent const& keyevent) {
	return sendEvent(keyevent, sendEventToFocused(keyevent));
}
bool Widget::send(TextInput const& character) {
	return sendEvent(character, sendEventToFocused(character));
}

void Widget::drawBackgroundRecursive(Canvas& canvas, bool minimal) {
	// TODO: don't ignore minimal
	if(minimal) {
		if(mFlags[FlagNeedsRedraw])
			onDrawBackground(canvas);
		if(!mFlags[FlagChildNeedsRedraw])
			return;
	}
	else {
		onDrawBackground(canvas);
	}

	eachChild([&](Widget* w) {
		if(w->offsetx() > -w->width() && w->offsety() > -w->height() && w->offsetx() < width() && w->offsety() < height()) {
			canvas.pushClipRect(w->offsetx(), w->offsety(), w->width(), w->height());
			w->drawBackgroundRecursive(canvas, minimal);
			canvas.popClipRect();
		}
	});
}
void Widget::drawForegroundRecursive(Canvas& canvas, bool minimal) {
	// TODO: don't ignore minimal
	// TODO: clear FlagChildNeedsRedraw and FlagNeedsRedraw
	if(!minimal || mFlags[FlagChildNeedsRedraw]) {
		eachChild([&](Widget* w) {
			if(w->offsetx() > -w->width() && w->offsety() > -w->height() && w->offsetx() < width() && w->offsety() < height()) {
				canvas.pushClipRect(w->offsetx(), w->offsety(), w->width(), w->height());
				w->drawForegroundRecursive(canvas, minimal);
				canvas.popClipRect();
			}
		});
		mFlags[FlagChildNeedsRedraw] = false;
	}

	if(minimal && !mFlags[FlagNeedsRedraw]) return;
	else mFlags[FlagNeedsRedraw] = false;

	onDraw(canvas);
}

void Widget::draw(Canvas& canvas, bool minimal) {
	updateLayout();
	canvas.pushClipRect(offsetx(), offsety(), width(), height());
	drawBackgroundRecursive(canvas, minimal);
	drawForegroundRecursive(canvas, minimal);
	canvas.popClipRect();
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
	}
	return result;
}

void Widget::forceRelayout() {
	if(!mParent) {
		PreferredSize info = getPreferredSize();
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

void Widget::requestRelayout() {
	mFlags[FlagNeedsRelayout] = true;

	Widget* p = parent();
	while(p && !p->mFlags[FlagChildNeedsRelayout]) {
		p->mFlags[FlagChildNeedsRelayout] = true;
		p = p->parent();
	}
}

void Widget::preferredSizeChanged() {
	if(parent()) {
		mParent->onChildPreferredSizeChanged(this);
	}
}

void Widget::alignmentChanged() {
	if(parent()) {
		parent()->onChildAlignmentChanged(this);
	}
}

void Widget::paddingChanged() {
	preferredSizeChanged(); // TODO: is this really equal?
}


void Widget::requestRedraw() {
	if(!mFlags[FlagNeedsRedraw]) {
		for(Widget* p = parent(); p && !p->mFlags[FlagChildNeedsRedraw]; p = p->parent()) {
			if(!p->mFlags[FlagChildNeedsRedraw])
				p->mFlags[FlagChildNeedsRedraw] = true;
			else
				break;
		}
	}
}


bool Widget::clearFocus(float strength) {
	bool success = true;
	eachPreOrderConditional([&](Widget* w) -> bool {
		if(!w->focused() || w->childFocused()) return false;
		if(w->focused())
			success = success && w->removeFocus(strength);
		return success;
	});
	return success;
}
bool Widget::requestFocus(float strength) {
	if(focused()) return true; // We already are focused

	if(!onFocus(true, strength)) goto FAIL; // Appearently this shouldn't be focused

	if(Widget* focused_w = findRoot()->findFocused()) { // Remove existing focus
		if(!focused_w->removeFocus(strength))
			goto FAIL;
	}

	mFlags[FlagFocused] = true;
	for(Widget* p = parent(); p; p = p->parent())
		p->mFlags[FlagChildFocused] = true;

	return true;

FAIL:
		mFlags[FlagFocused] = false;
		onFocus(false, 1e7);
		return false;
}
bool Widget::removeFocus(float strength) {
	if(!focused()) return false;

	mFlags[FlagFocused] = false;
	if(!onFocus(false, strength)) {
		mFlags[FlagFocused] = true;
		return false;
	}

	if(!mFlags[FlagChildFocused]) {
		Widget* p = parent();
		while(p && p->mFlags[FlagChildFocused]) {
			p->mFlags[FlagChildFocused] = false;
			if(p->focused()) break;
			p = p->parent();
		}
	}

	return true;
}

Widget* Widget::findFocused() noexcept {
	if(!mFlags[FlagChildFocused]) return nullptr;

	Widget* result = nullptr;

	eachDescendendPreOrderConditional([&](Widget* w) -> bool {
		if(result) return false;
		if(w->focused()) {
			result = w;
			return false;
		}
		if(!w->childFocused()) return false;
		return true;
	});

	return result;
}

Widget* Widget::text(std::string const& s) {
	if(auto* l = search<Text>()) {
		if(s.empty())
			l->remove();
		else
			l->content(s);
	}
	else if(!s.empty()) {
		auto* l = add<Text>();
		l->content(s)->align(AlignCenter)->classes(".generated");
	}
	return this;
}
std::string Widget::text() {
	if(auto* l = search<Text>())
		return l->content();
	else
		return "";
}
Widget* Widget::image(std::string const& s) {
	if(auto* l = search<Image>()) {
		if(s.empty())
			l->remove();
		else
			l->image(s);
	}
	else if(!s.empty()) {
		add<Image>(s)->align(AlignCenter)->classes(".generated");
	}
	return this;
}
Image* Widget::image() {
	if(auto* i = search<Image>())
		return i;
	return nullptr;
}


PreferredSize Widget::getPreferredSize() {
	PreferredSize result = onCalcPreferredSize();
	float padx = mPadding.left + mPadding.right;
	float pady = mPadding.top + mPadding.bottom;
	// info.minx  += padx;
	result.prefx += padx;
	result.maxx  += padx;
	// info.miny  += pady;
	result.prefy += pady;
	result.maxy  += pady;
	return result;
}

Widget* Widget::size(float w, float h) {
	float dif = fabs(width()  - w) + fabs(height() - h);
	if(dif > 1) {
		mSize = {w, h};
		onResized();
	}
	return this;
}
Widget* Widget::width (float w) { return size(w, height()); }
Widget* Widget::height(float h) { return size(width(), h); }

Widget* Widget::offset(float x, float y) {
	if(offsetx() != x || offsety() != y) {
		mOffset = {x, y};
	}
	return this;
}
Widget* Widget::offsetx(float x) { return offset(x, offsety()); }
Widget* Widget::offsety(float y) { return offset(offsetx(), y); }

void Widget::absoluteOffset(float& x, float& y, Widget* relativeToParent) {
	x = offsetx();
	y = offsety();
	for(Widget* p = parent(); p != relativeToParent; p = p->parent()) {
		if(p == nullptr) throw std::runtime_error("absoluteOffset: relativeTo argument is neither a nullptr nor a parent of this widget!");
		x += p->offsetx();
		y += p->offsety();
	}
}

Widget* Widget::align(Alignment a) {
	if(a.x != alignx() || a.y != aligny()) {
		mAlign = a;
		alignmentChanged();
	}
	return this;
}
Widget* Widget::align(HalfAlignment x, HalfAlignment y) { return align({x, y}); }
Widget* Widget::alignx(HalfAlignment x) { return align({x, alignx()}); }
Widget* Widget::aligny(HalfAlignment y) { return align({alignx(), y}); }

Widget* Widget::padding(float left, float top, float right, float bottom) {
	bool changed =
		mPadding.left != left ||
		mPadding.top != top ||
		mPadding.right != right ||
		mPadding.bottom != bottom;
	if(changed) {
		mPadding.left = left;
		mPadding.top = top;
		mPadding.right = right;
		mPadding.bottom = bottom;
		requestRelayout();
	}
	return this;
}
Widget* Widget::padding(float left_and_right, float top_and_bottom) {
	return padding(left_and_right, top_and_bottom, left_and_right, top_and_bottom);
}
Widget* Widget::padding(float all_around) {
	return padding(all_around, all_around, all_around, all_around);
}

// ** Backend shortcuts *******************************************************
Widget* Widget::applet(Applet* app) {
	if(mApplet != app) {
		Applet* oldApplet = mApplet;
		mApplet = app;
		eachChild([&](Widget* w) {
			if(w->applet() == oldApplet || w->applet() == nullptr) {
				w->applet(mApplet);
			}
		});
		onAppletChanged();
	}
	return this;
}

void Widget::defer(std::function<void()> fn) {
	auto* a = applet();
	if(a) {
		a->defer(std::move(fn));
	}
	else {
		// HACK: This could brake if fn() removes the widget which calls defer etc.
		// But it has to be called.
		fn();
	}
}
// void Widget::deferDraw(std::function<void()> fn) {
// 	auto* a = applet();
// 	assert(a);
// 	a->deferDraw(std::move(fn));
// }

void Widget::loadImage(std::function<void(std::shared_ptr<Bitmap>)> fn, std::string const& url) {
	auto* a = applet();
	if(!a)
		fn(nullptr);
	else
		a->loadImage(makeOwnedTask(this, std::move(fn)), url);
}
void Widget::loadImage(std::shared_ptr<Bitmap>& to, std::string const& url) {
	auto* a = applet();
	if(!a)
		to = nullptr;
	else
		a->loadImage(makeOwnedTask(this, [&](auto p) { to = std::move(p); }), url);
}
void Widget::loadFont(std::function<void(std::shared_ptr<Font>)> fn, std::string const& url) {
	auto* a = applet();
	if(!a)
		fn(nullptr);
	else
		a->loadFont(makeOwnedTask(this, std::move(fn)), url);
}
void Widget::loadFont(std::shared_ptr<Font>& to, std::string const& url) {
	auto* a = applet();
	if(!a)
		to = nullptr;
	else
		a->loadFont(makeOwnedTask(this, [&](auto p) { to = std::move(p); }), url);
}

} // namespace wwidget
