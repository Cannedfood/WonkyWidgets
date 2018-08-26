#include "../include/wwidget/Widget.hpp"

#include "../include/wwidget/Context.hpp"

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

	mContext(nullptr)
{
	mFlags.childNeedsRelayout = false;
	mFlags.needsRelayout = true;
	mFlags.focused = false;
	mFlags.childFocused = false;
	mFlags.needsRedraw = true;
	mFlags.childNeedsRedraw = true;
	mFlags.recalcPrefSize = true;
}

Widget::~Widget() {
	remove();
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

	mName          = std::move(other.mName);
	mClasses       = std::move(other.mClasses);
	mPreferredSize = other.mPreferredSize; other.mPreferredSize = {};
	mPadding       = other.mPadding; other.mPadding = {};
	mSize          = other.mSize; other.mSize = {};
	mOffset        = other.mOffset; other.mOffset = {};
	mAlign         = other.mAlign; other.mAlign = {};
	mParent        = std::move(other.mParent);
	if(auto parent = mParent.lock()) {
		if(parent->mChildren.get() == &other) {
			parent->mChildren = *this;
		}
	}
	mNextSibling = std::move(other.mNextSibling);
	if(mNextSibling) {
		mNextSibling->mPrevSibling = this->weak_from_this();
	}
	mPrevSibling = std::move(other.mPrevSibling);
	if(auto prev = mPrevSibling.lock()) {
		prev->mNextSibling = *this;
	}
	mChildren = other.mChildren; other.mChildren = nullptr;
	if(mChildren) {
		for(auto w = children(); w; w = w->nextSibling()) {
			w->mParent = *this;
		}
	}
	mContext = other.mContext; other.mContext = nullptr;
	mFlags   = other.mFlags;
	other.mFlags.childNeedsRelayout = false;
	other.mFlags.needsRelayout = true;
	other.mFlags.focused = false;
	other.mFlags.childFocused = false;
	other.mFlags.needsRedraw = true;
	other.mFlags.childNeedsRedraw = true;
	other.mFlags.recalcPrefSize = true;

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

void Widget::notifyChildAdded(Widget& newChild) {
	newChild.context(context());
	newChild.onAddTo(*this);
	onAdd(newChild);
	if(newChild.needsRelayout()) {
		onChildPreferredSizeChanged(newChild);

		shared<Widget> p = *this;
		while(p && !p->mFlags.childNeedsRelayout) {
			p->mFlags.childNeedsRelayout = true;
			p = p->parent();
		}
	}
}

void Widget::notifyChildRemoved(Widget& noLongerChild) {
	noLongerChild.onRemoveFrom(*this);
	onRemove(noLongerChild);
}

shared<Widget> Widget::add(shared<Widget> w) {
	if(!w) {
		throw exceptions::InvalidPointer("w");
	}

	w->remove();

	w->mParent = *this;
	shared<Widget> end = lastChild();
	if(!end) {
		mChildren = w;
	}
	else {
		end->mNextSibling = w;
		w->mPrevSibling   = end;
	}

	assert(!(!w->mPrevSibling.lock() && mChildren != w));

	notifyChildAdded(*w);

	return w;
}

void Widget::add(std::initializer_list<shared<Widget>> ptrs) {
	for(shared<Widget> const& w : ptrs) add(w);
}

shared<Widget> Widget::insertNextSibling(shared<Widget> w) {
	if(!mParent.lock()) {
		throw exceptions::RootNodeSibling();
	}

	if(w->mParent.lock()) {
		w->remove();
	}

	w->mNextSibling = mNextSibling;
	if(mNextSibling) {
		mNextSibling->mPrevSibling = w;
	}

	w->mPrevSibling = shared_from_this();
	mNextSibling = w;

	w->mParent = mParent;

	mParent.lock()->notifyChildAdded(*w);

	return w;
}

shared<Widget> Widget::insertPrevSibling(shared<Widget> w) {
	if(!mParent.lock()) {
		throw exceptions::RootNodeSibling();
	}

	if(w->mParent.lock()) {
		w->remove();
	}

	w->mPrevSibling = mPrevSibling;
	if(auto prev = mPrevSibling.lock()) {
		prev->mNextSibling = w;
	}
	else {
		parent()->mChildren = w;
	}

	w->mNextSibling = *this;
	mPrevSibling = w;

	w->mParent = mParent;

	mParent.lock()->notifyChildAdded(*w);

	return w;
}

void Widget::extract() {
	if(!mParent.lock()) {
		throw exceptions::InvalidOperation("Tried extracting widget without parent.");
	}

	eachChild([this](shared<Widget> w) {
		mParent.lock()->add(w);
	});

	remove();
}

shared<Widget> Widget::remove() {
	auto parent = this->parent();

	if(!parent) return *this;

	auto result = removeQuiet();
	parent->notifyChildRemoved(*this);
	return result;
}

shared<Widget> Widget::removeQuiet() {
	shared<Widget> result = *this;

	removeFocus();
	if(auto parent = this->parent()) {
		mParent.reset();

		if(auto prev = mPrevSibling.lock()) {
			if(mNextSibling) {
				mNextSibling->mPrevSibling = std::move(mPrevSibling);
			}
			else {
				mPrevSibling.reset();
			}
			prev->mNextSibling = std::move(mNextSibling);
		}
		else {
			assert(parent->children().get() == (Widget*)this);
			if(mNextSibling) {
				mNextSibling->mPrevSibling.reset();
			}
			parent->mChildren = std::move(mNextSibling);
		}
	}

	assert(!mNextSibling);
	assert(!mPrevSibling.lock());
	assert(!mParent.lock());

	return result;
}

template<>
shared<Widget> Widget::search<Widget>(const char* name) noexcept {
	if(mName == name) {
		return shared_from_this();
	}

	for(auto c = mChildren; c; c = c->mNextSibling) {
		if(auto result = c->search<Widget>(name))
			return result;
	}

	return nullptr;
}

template<>
shared<Widget> Widget::searchParent<Widget>(const char* name) const noexcept {
	auto p = parent();

	if(!p) return nullptr;

	while(p) {
		if(p->name() == name) {
			return p;
		}
		p = p->parent();
	}

	return nullptr;
}

shared<Widget> Widget::findRoot() const noexcept {
	shared<Widget> root = shared_from_this();
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

shared<Widget> Widget::lastChild() const noexcept {
	if(!mChildren) {
		return nullptr;
	}

	shared<Widget> w = children();
	while(w->nextSibling()) w = w->nextSibling();
	return w;
}

// Tree changed events
void Widget::onContextChanged() { }

void Widget::onAddTo(Widget& w) { }
void Widget::onRemoveFrom(Widget& parent) { }

void Widget::onAdd(Widget& w) {
	preferredSizeChanged();
	requestRelayout();
}
void Widget::onRemove(Widget& w) {
	preferredSizeChanged();
	requestRelayout();
}

// Layout events
void Widget::onResized() {
	requestRelayout();
}

void Widget::onChildPreferredSizeChanged(Widget& child) {
	preferredSizeChanged();
	requestRelayout();
}
void Widget::onChildAlignmentChanged(Widget& child) {
	AlignChild(child, {}, size());
}
PreferredSize Widget::onCalcPreferredSize() {
	return calcBoxAroundChildren(1, 1);
}
void Widget::onLayout() {
	eachChild([&](shared<Widget> child) {
		auto& info = child->preferredSize();
		child->size(
			child->alignx() == AlignFill ? width() : info.pref.x,
			child->aligny() == AlignFill ? height() : info.pref.y
		);
		AlignChild(*child, {}, size());
		ClipChild(*child, {}, size());
	});
}

PreferredSize Widget::calcBoxAroundChildren(float alt_prefx, float alt_prefy) noexcept {
	PreferredSize info;
	if(!mChildren) {
		info.pref.x = (alignx() == AlignFill) ? 0 : alt_prefx;
		info.pref.y = (aligny() == AlignFill) ? 0 : alt_prefy;
	}
	else if(alignx() == AlignNone && aligny() == AlignNone) {
		info.min.x = info.max.x = info.pref.x = width();
		info.min.y = info.max.y = info.pref.y = height();
	}
	else {
		info = PreferredSize::MinMaxAccumulator();

		eachChild([&](shared<Widget> child) {
			auto& subinfo = child->preferredSize();

			float x = (child->alignx() == AlignNone) ? std::max(0.f, child->offsetx()) : 0;
			float y = (child->aligny() == AlignNone) ? std::max(0.f, child->offsety()) : 0;
			info.include(subinfo, x, y);
		});

		info.sanitize();

		if(alignx() == AlignNone)
			info.min.x = info.max.x = info.pref.y = width();

		if(aligny() == AlignNone)
			info.min.y = info.max.y = info.pref.y = height();
	}
	return info;
}

float Widget::GetAlignmentX(Widget& child, float min, float width) noexcept {
	switch (child.alignx()) {
		case AlignNone: return child.offsetx();
		case AlignFill:
		default:
		case AlignMin: return min + child.padLeft();
		case AlignMax: return min + width - (child.width() + child.padRight());
		case AlignCenter:
			return roundf((
				min + child.padLeft() +
				(min + width - (child.width() + child.padRight()))
			) * .5f);
	}
}
float Widget::GetAlignmentY(Widget& child, float min, float height) noexcept {
	switch (child.aligny()) {
		case AlignNone: return child.offsety();
		case AlignFill:
		default:
		case AlignMin: return min + child.padTop();
		case AlignMax: return min + height - (child.height() + child.padBottom());
		case AlignCenter:
			return roundf((
				min + child.padTop() +
				(min + height - (child.height() + child.padBottom()))
			) * .5f);
	}
}
void Widget::AlignChildX(Widget& child, float min, float width) {
	child.offsetx(std::max(min, GetAlignmentX(child, min, width)));
	child.width(std::min(child.width(), width - child.offsetx()));
}
void Widget::AlignChildY(Widget& child, float min, float height) {
	child.offsety(std::max(min, GetAlignmentY(child, min, height)));
	child.height(std::min(child.height(), height - child.offsety()));
}
void Widget::AlignChild(Widget& child, Offset min, Size size) {
	child.offset(
		GetAlignmentX(child, min.x, size.x),
		GetAlignmentY(child, min.y, size.y)
	);
}
void Widget::ClipChild(Widget& child, Offset min, Size size) {
	child.offset(
		std::max(min.x, child.offsetx()),
		std::max(min.y, child.offsety())
	);
	child.size(
		std::min(child.width(),  size.x - (child.offsetx() - min.x)),
		std::min(child.height(), size.y - (child.offsety() - min.y))
	);
}

// Input events
void Widget::on(Click const& c) {
	if(!focused()) {
		c.handled = requestFocus(FOCUS_CLICK);
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

void Widget::onDescendendFocused(Rect const& area, Widget& w) {}
bool Widget::onFocus(bool b, FocusType type) { return !b; }

// Drawing events
void Widget::onDrawBackground(Canvas& graphics) {}
void Widget::onDraw(Canvas& graphics) {}

// Attributes
bool Widget::setAttribute(std::string_view s, Attribute const& value) {
	switch(fnv1a(s)) {
	case fnv1a("name"):
		mName.reset(value.toString());
		return true;
	case fnv1a("class"):
		classes(value.toString());
		return true;
	case fnv1a("width"):
		size(value.toFloat(), height());
		return true;
	case fnv1a("height"):
		size(width(), value.toFloat());
		return true;
	case fnv1a("offset"):
		set((Offset)value.toPoint()); // TODO: Attribute::toOffset
		align(AlignNone);
		return true;
	case fnv1a("x"):
		offset(value.toFloat(), offsety());
		alignx(AlignNone);
		return true;
	case fnv1a("y"):
		offset(offsetx(), value.toFloat());
		aligny(AlignNone);
		return true;
	case fnv1a("align"):
		align(value.toAlignment());
		return true;
	case fnv1a("alignx"):
		alignx(value.toAlignment().x); // TODO: Attribute::toHalfAlignment
		return true;
	case fnv1a("aligny"):
		aligny(value.toAlignment().y); // TODO: Attribute::toHalfAlignment
		return true;
	case fnv1a("padding"):
		set(value.toPadding());
		return true;
	case fnv1a("text"):
		text(value.toString());
		return true;
	case fnv1a("image"):
		image(value.toString());
		return true;
	}

	return false;
}

void Widget::getAttributes(wwidget::AttributeCollectorInterface& collector) {
	if(!collector.startSection("wwidget::Widget")) return;

	if(collector.startSection("debug")) {
		{
			std::stringstream ss;
			ss << this;
			collector("ptr", ss.str(), "");
		}
		if(mFlags.childNeedsRelayout)
			collector("child needs relayout", mFlags.childNeedsRelayout, false);
		if(mFlags.needsRelayout)
			collector("needs relayout", mFlags.needsRelayout, false);
		if(mFlags.focused)
			collector("focused", mFlags.focused, false);
		if(mFlags.childFocused)
			collector("focused ind.", mFlags.childFocused, false);

		{
			auto& info = preferredSize();
			collector("min",  info.min, {});
			collector("pref", info.pref, {});
			collector("max",  info.max, {});
		}
		collector.endSection();
	}

	collector("name", mName, "");
	{
		std::string result;
		size_t len = 0;
		for(auto& c : mClasses) len += c.length();
		result.reserve(len);
		for(auto& c : mClasses) result += c;
		collector("class", result, "");
	}
	collector("width",   width(), 0);
	collector("height",  height(), 0);
	collector("offset",  offset(), { alignx() == AlignNone ? offsetx() : 0, aligny() == AlignNone ? offsety() : 0 });
	collector("align",   mAlign, Alignment{AlignDefault});
	collector("padding", mPadding, {});
	// TODO: text() and image()

	collector.endSection();
}

template<typename T>
bool Widget::sendEvent(T const& t, bool skip_focused) {
	if(t.handled) return t.handled;

	if(T::positional && !Rect(size()).contains(t.position)) return t.handled;

	if(skip_focused && focused()) return t.handled;

	t.direction = Event::DIR_DOWN;
	on(t);

	for(shared<Widget> child = lastChild(); !t.handled && child; child = child->prevSibling()) {
		Point old_pos = t.position;
		t.position.x -= child->offsetx();
		t.position.y -= child->offsety();
		child->sendEvent(t, skip_focused);
		t.position = old_pos;
	}

	if(t.handled) return t.handled;

	t.direction = Event::DIR_UP;
	on(t);

	return t.handled;
};

template<typename T>
bool Widget::sendEventToFocused(T const& t) {
	t.direction = Event::DIR_UP_AND_DOWN;
	if(shared<Widget> f = findFocused()) {
		Point old_p = t.position;
		Offset off = f->absoluteOffset(this);
		t.position.x -= off.x;
		t.position.y -= off.y;
		f->sendEvent(t, false);
		t.position = old_p;
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

void Widget::drawRecursive(Canvas& canvas, bool minimal) {
	// TODO: don't ignore minimal
	onDrawBackground(canvas);

	eachChild([&](shared<Widget> w) {
		if(w->offsetx() > -w->width() && w->offsety() > -w->height() && w->offsetx() < width() && w->offsety() < height()) {
			canvas.pushState();
			canvas.scissorIntersect({w->offset(), w->size()});
			canvas.translate(w->offsetx(), w->offsety());
			w->drawRecursive(canvas, minimal);
			canvas.popState();
		}
	});

	onDraw(canvas);

	mFlags.needsRedraw = false;
	mFlags.childNeedsRedraw = false;
}

void Widget::draw(Canvas& canvas, bool minimal) {
	updateLayout();
	canvas.pushState();
	canvas.scissorIntersect({offset(), size()});
	canvas.translate(offsetx(), offsety());
	drawRecursive(canvas, minimal);
	canvas.popState();
}

bool Widget::updateLayout() {
	bool result = false;
	if(mFlags.needsRelayout) {
		result = true;
		forceRelayout();
	}
	else if(mFlags.childNeedsRelayout) {
		result = true;
		mFlags.childNeedsRelayout = false;
		eachChild([](shared<Widget> w) {
			w->updateLayout();
		});
	}
	return result;
}

bool Widget::forceRelayout() {
	if(!mParent.lock()) {
		auto& info = preferredSize();
		size(info.pref);
	}

	mFlags.needsRelayout = false;
	onLayout();

	if(!mFlags.childNeedsRelayout) return false;

	mFlags.childNeedsRelayout = false;
	eachChild([](shared<Widget> w) {
		w->updateLayout();
	});
	return true;
}

void Widget::requestRelayout() {
	mFlags.needsRelayout = true;

	shared<Widget> p = parent();
	while(p && !p->mFlags.childNeedsRelayout) {
		p->mFlags.childNeedsRelayout = true;
		p = p->parent();
	}
}

void Widget::preferredSizeChanged() {
	mFlags.recalcPrefSize = true;
	if(auto parent = this->parent()) {
		parent->onChildPreferredSizeChanged(*this);
	}
}

void Widget::alignmentChanged() {
	if(auto parent = this->parent()) {
		parent->onChildAlignmentChanged(*this);
	}
}

void Widget::paddingChanged() {
	preferredSizeChanged(); // TODO: is this really equal?
}


void Widget::requestRedraw() {
	if(!mFlags.needsRedraw) {
		for(shared<Widget> p = parent(); p && !p->mFlags.childNeedsRedraw; p = p->parent()) {
			if(p->mFlags.childNeedsRedraw)
				break;
			p->mFlags.childNeedsRedraw = true;
		}
	}
}


bool Widget::clearFocus(FocusType type) {
	bool success = true;
	eachPreOrderConditional([&](shared<Widget> w) -> bool {
		if(!w->focused() || w->childFocused()) return false;
		if(w->focused())
			success = success && w->removeFocus(type);
		return success;
	});
	return success;
}
bool Widget::requestFocus(FocusType type) {
	if(focused()) return true; // We already are focused

	if(!onFocus(true, type)) goto FAIL; // Appearently this shouldn't be focused

	if(shared<Widget> focused_w = findRoot()->findFocused()) { // Remove existing focus
		if(!focused_w->removeFocus(type))
			goto FAIL;
	}

	mFlags.focused = true;
	for(shared<Widget> p = parent(); p; p = p->parent())
		p->mFlags.childFocused = true;

	{
		Rect area = { offset(), size() };
		for(shared<Widget> p = parent(); p; p = p->parent()) {
			p->onDescendendFocused(area, *this);
			area.min.x -= p->offsetx();
			area.min.y -= p->offsety();
			area.max.x -= p->offsetx();
			area.max.y -= p->offsety();
		}
	}

	return true;

FAIL:
		mFlags.focused = false;
		onFocus(false, FOCUS_FORCE);
		return false;
}
bool Widget::removeFocus(FocusType type) {
	if(!focused()) return false;

	mFlags.focused = false;
	if(!onFocus(false, type)) {
		mFlags.focused = true;
		return false;
	}

	if(!mFlags.childFocused) {
		shared<Widget> p = parent();
		while(p && p->mFlags.childFocused) {
			p->mFlags.childFocused = false;
			if(p->focused()) break;
			p = p->parent();
		}
	}

	return true;
}

shared<Widget> Widget::findFocused() noexcept {
	if(!mFlags.childFocused) return nullptr;

	shared<Widget> result = nullptr;

	eachDescendendPreOrderConditional([&](shared<Widget> w) -> bool {
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

Widget& Widget::text(std::string const& s) {
	if(auto l = search<Text>()) {
		if(s.empty())
			l->remove();
		else
			l->content(s);
	}
	else if(!s.empty()) {
		auto l = add<Text>();
		l->content(s).align(AlignCenter).classes(".generated");
	}
	return *this;
}
std::string Widget::text() {
	if(auto l = search<Text>())
		return l->content();
	else
		return "";
}
Widget& Widget::image(std::string const& s) {
	if(auto l = search<Image>()) {
		if(s.empty())
			l->remove();
		else
			l->image(s);
	}
	else if(!s.empty()) {
		add<Image>(s)->align(AlignCenter).classes(".generated");
	}
	return *this;
}
shared<Image> Widget::image() {
	if(auto i = search<Image>())
		return i;
	return nullptr;
}


PreferredSize const& Widget::preferredSize() {
	if(mFlags.recalcPrefSize) {
		mFlags.recalcPrefSize = false;
		mPreferredSize = onCalcPreferredSize();

		mPreferredSize.min.x  = std::ceil(mPreferredSize.min.x);
		mPreferredSize.min.y  = std::ceil(mPreferredSize.min.y);
		mPreferredSize.pref.x = std::ceil(mPreferredSize.pref.x);
		mPreferredSize.pref.y = std::ceil(mPreferredSize.pref.y);
		mPreferredSize.max.x  = std::ceil(mPreferredSize.max.x);
		mPreferredSize.max.y  = std::ceil(mPreferredSize.max.y);
	}
	return mPreferredSize;
}

Widget& Widget::classes(
	std::string const& s) noexcept
{
	auto iter = std::lower_bound(mClasses.begin(), mClasses.end(), s.c_str());
	if(iter == mClasses.end() || *iter != s.c_str()) {
		mClasses.emplace(iter, s.data(), s.length());
	}
	return *this;
}
Widget& Widget::classes(
	std::initializer_list<std::string> classes) noexcept
{
	for(auto& s : classes)
		this->classes(s);
	return *this;
}

Widget& Widget::size(float w, float h) {
	return size({w, h});
}
Widget& Widget::size(Size const& size) {
	float dif = fabs(width() - size.x) + fabs(height() - size.y);
	if(dif > 1) {
		mSize = size;
		onResized();
	}
	return *this;
}
Widget& Widget::width (float w) { return size(w, height()); }
Widget& Widget::height(float h) { return size(width(), h); }

Widget& Widget::offset(float x, float y) {
	return set(Offset{x, y});
}
Widget& Widget::offsetx(float x) { return offset(x, offsety()); }
Widget& Widget::offsety(float y) { return offset(offsetx(), y); }

Offset Widget::absoluteOffset(Widget const* relativeToParent) {
	Offset off = offset();
	for(shared<Widget> p = parent(); p.get() != relativeToParent; p = p->parent()) {
		if(p == nullptr) throw std::runtime_error("absoluteOffset: relativeTo argument is neither a nullptr nor a parent of this widget!");
		off.x += p->offsetx();
		off.y += p->offsety();
	}
	return off;
}

Widget& Widget::align(Alignment a) {
	return set(a);
}
Widget& Widget::align(HalfAlignment x, HalfAlignment y) { return set(Alignment{x, y}); }
Widget& Widget::alignx(HalfAlignment x) { return set(Alignment{x, alignx()}); }
Widget& Widget::aligny(HalfAlignment y) { return set(Alignment{alignx(), y}); }

Widget& Widget::padding(float left, float top, float right, float bottom) {
	return set(Padding{left, top, right, bottom});
}
Widget& Widget::padding(float left_and_right, float top_and_bottom) {
	return set(Padding{left_and_right, top_and_bottom});
}
Widget& Widget::padding(float all_around) {
	return set(Padding{all_around});
}

// ** Set-functions *******************************************************
Widget& Widget::set(Name&& nam) {
	mName = std::move(nam);
	return *this;
}
Widget& Widget::set(Class&& cls) {
	classes(cls.c_str());
	return *this;
}
Widget& Widget::set(std::initializer_list<Class> clss) {
	for(auto& cls : clss) {
		classes(cls.c_str());
	}
	return *this;
}

Widget& Widget::set(Padding const& pad) {
	if(pad != mPadding) {
		mPadding = pad;
		preferredSizeChanged();
	}
	return *this;
}
Widget& Widget::set(Alignment const& align) {
	if(mAlign != align) {
		mAlign = align;
		alignmentChanged();
	}
	return *this;
}
Widget& Widget::set(Offset const& off) {
	if(mOffset != off) {
		mOffset = off;
	}
	return *this;
}
Widget& Widget::set(Size const& size) {
	if(mSize != size) {
		mSize = size;
		onResized();
	}
	return *this;
}

// ** Backend shortcuts *******************************************************
Widget& Widget::context(Context* app) {
	if(mContext != app) {
		Context* oldContext = mContext;
		mContext = app;
		eachChild([&](shared<Widget> w) {
			if(w->context() == oldContext || w->context() == nullptr) {
				w->context(mContext);
			}
		});
		onContextChanged();
	}
	return *this;
}

void Widget::defer(std::function<void()> fn) {
	auto* a = context();
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
// 	auto* a = context();
// 	assert(a);
// 	a->deferDraw(std::move(fn));
// }

void Widget::loadImage(Owner* taskOwner, std::function<void(shared<Bitmap>)> fn, std::string const& url) {
	auto* a = context();
	if(!a)
		fn(nullptr);
	else
		a->loadImage(makeOwnedTask(taskOwner, std::move(fn)), url);
}
void Widget::loadImage(Owner* taskOwner, shared<Bitmap>& to, std::string const& url) {
	auto* a = context();
	if(!a)
		to = nullptr;
	else
		a->loadImage(makeOwnedTask(taskOwner, [&](auto p) { to = std::move(p); }), url);
}
shared<Bitmap> Widget::loadImage(std::string const& url) {
	auto* a = context();
	shared<Bitmap> result;
	if(a) {
		result = a->loadImage(url);
	}
	return result;
}

} // namespace wwidget
