#pragma once

#include <string>
#include <vector>
#include <bitset>
#include <memory>
#include <functional>

#include "Events.hpp"
#include "Attributes.hpp"
#include "thirdparty/stx/shared_ptr.hpp"

#define WWIDGET_DECLARE_VARIADIC_SET_FUNCTION() \
	template<class Arg1, class Arg2, class... ArgN> \
	Widget& set(Arg1&& arg1, Arg2&& arg2, ArgN&&... argN) { \
		set(std::forward<Arg1>(arg1)); \
		set(std::forward<Arg2>(arg2)); \
		(set(std::forward<ArgN>(argN)), ...); \
		return *this; \
	}

namespace wwidget {

class AttributeCollectorInterface;
class Canvas;
class Bitmap;
class Font;
class Image;
class Context;

using namespace stx;

/**
 * Widget is the base class of all widget windows etc.
 * The Ui is build as a tree of widgets, where the children of each widget are stored as a linked list.
 */
class Widget : public enable_shared_from_this<Widget> {
private:
	TinyString              mName;
	std::vector<TinyString> mClasses;

	PreferredSize mPreferredSize;
	uint32_t      mConstraintHash;

	Padding mPadding;

	Size   mSize;
	Offset mOffset;

	Alignment mAlign;

	mutable weak<Widget>    mParent;
	mutable shared<Widget>  mNextSibling;
	mutable weak<Widget>    mPrevSibling;
	mutable shared<Widget>  mChildren;

	mutable Context*        mContext;

	struct {
		uint32_t
			childNeedsRelayout : 1,
			needsRelayout : 1,
			focused : 1,
			childFocused : 1,
			needsRedraw : 1,
			childNeedsRedraw : 1,
			recalcPrefSize : 1;
	} mFlags;

	void notifyChildAdded(Widget& newChild);
	void notifyChildRemoved(Widget& noLongerChild);

	void drawRecursive(Canvas& canvas, bool minimal);

	template<typename T>
	bool sendEvent(T const& t, bool skip_focused);
	template<typename T>
	bool sendEventToFocused(T const& t);
protected:
	// ** Overidable event receivers *******************************************************
	friend class Context;
	virtual void onContextChanged();

	virtual void onAddTo(Widget& w); //<! Called when this is added to w
	virtual void onRemoveFrom(Widget& w); //<! Called when this is removed from w

	virtual void onAdd(Widget& w); //<! Called when a child w is added, throw an exception if this isn't wanted
	virtual void onRemove(Widget& w); //<! Called when a child w is removed

	// Layout events
	virtual void onResized(); //<! Called after the size was changed
	virtual void onChildPreferredSizeChanged(Widget& child); //<! A child signaled that it would like a different size
	virtual void onChildAlignmentChanged(Widget& child); //<! A child signaled that it would like a different alignment
	virtual PreferredSize onCalcPreferredSize(PreferredSize const& constraints); //<! Calculate the preferred size of the widget
	virtual void onLayout(); //<! The widget updates the child's positions and size in here

	// Input events
	virtual void on(Click     const& c);
	virtual void on(Scroll    const& s);
	virtual void on(Moved     const& c);
	virtual void on(Dragged   const& s);
	virtual void on(KeyEvent  const& k);
	virtual void on(TextInput const& t);

	// Focus events
	virtual void onDescendendFocused(Rect const& area, Widget& w);
	virtual bool onFocus(bool b, FocusType type); //<! Returns whether strength is sufficient to focus this widget. (Always returns false by default)

	// Drawing events
	virtual void onDrawBackground(Canvas& graphics); //<! Draw background (From root to leafs)
	virtual void onDraw(Canvas& graphics); //<! Draw foreground (from root to leafs)

	// ** Layout utilities *******************************************************
	PreferredSize calcBoxAroundChildren(
		float empty_width, float empty_height,
		PreferredSize const& constraint) noexcept; //<! Calculates a box around children, or uses empty_width/height if no children are attached

	static float GetAlignmentX(Widget& child, float min, float width) noexcept;
	static float GetAlignmentY(Widget& child, float min, float height) noexcept;
	static void AlignChildX(Widget& child, float min, float width);
	static void AlignChildY(Widget& child, float min, float height);
	static void AlignChild(Widget& child, Offset min, Size size);
	static void ClipChild(Widget& child, Offset min, Size size);

public:
	// Attributes
	virtual bool setAttribute(std::string_view name, Attribute const& value);
	virtual void getAttributes(AttributeCollectorInterface& collector);

public:
	Widget() noexcept;
	virtual ~Widget();

protected:
	// ** Move *******************************************************
	Widget(Widget&& other) noexcept;
	Widget& operator=(Widget&& other) noexcept;

	// ** Copy *******************************************************
	Widget(Widget const& other) noexcept;
	Widget& operator=(Widget const& other) noexcept;

public:
	// ** Tree operations *******************************************************

	/// Adds the widget, usually as the first child and transfers ownership to this widget. It also returns a pointer to the added widget.
	shared<Widget> add(shared<Widget> w);
	/// Adds multiple widgets; returns this.
	void add(std::initializer_list<shared<Widget>> ptrs);
	/// Shortcut for Widget::add(std::make_unique<T>(...))
	template<typename T, typename... ARGS>
	shared<T> add(ARGS&&... args);
	/// Inserts a widget as next sibling and returns a pointer to it
	shared<Widget> insertNextSibling(shared<Widget> w);
	/// Inserts a widget as previous sibling and returns a pointer to it
	shared<Widget> insertPrevSibling(shared<Widget> w);
	/// Makes w take the place of this widget and makes this a child of it. Returns w.
	// shared<Widget> insertAsParent(shared<Widget> w);
	// TODO

	/// Removes this widget and makes it's children take it's place. Returns this widget. @see remove
	shared<Widget> extract();
	/// Removes this widget and its children. Returns ownership if the widget has the flag FlagOwnedByParent @see extract
	shared<Widget> remove();
	/// Removes this widget and its children. Returns ownership if the widget has the flag FlagOwnedByParent @see extract
	shared<Widget> removeQuiet();

	/// If the widet has the FlagOwnedByParent it unsets the flag and returns a unique_ptr to this widget
	shared<Widget> acquireOwnership() noexcept;
	/// Transfers the ownership of this widget to its parent. Throws when the parent already has ownership or the widget has no parent.
	void           giveOwnershipToParent();

	/// Calls remove() on all children. @see remove()
	void clearChildren();
	/// Calls removeQuiet() on all children. @see remove()
	void clearChildrenQuietly();

	/// Dynamic casts this to T&
	template<typename T>
	T&       as()       { return dynamic_cast<T&>(*this); }
	/// Dynamic casts this to T const&
	template<typename T>
	T const& as() const { return dynamic_cast<T const&>(*this); }

	/// Searches the (depth-)first widget with the specified name, and tries to cast it to T. Returns a nullptr on failure. @see Widget::search
	template<typename T = Widget> shared<T> search(const char* name) noexcept;
	/// Returns the (depth-)first widget dynamic_cast-able to T* or a nullptr.
	template<typename T = Widget> shared<T> search() noexcept;
	/// Searches the (depth-)first widget with the specified name, and tries to cast it to T. throws a WidgetNotFound if the widget wasn't found. @see Widget::search
	template<typename T = Widget> shared<T> find(const char* name);
	/// Returns the (depth-)first widget dynamic_cast-able to T* or throws a WidgetNotFound. @see Widget::search
	template<typename T = Widget> shared<T> find();

	/// Searches the first parent with the specified name, and tries to cast it to T. Returns a nullptr on failure. @see Widget::search
	template<typename T = Widget> shared<T> searchParent(const char* name) const noexcept;
	/// Returns the first parent dynamic_cast-able to T* or a nullptr.
	template<typename T = Widget> shared<T> searchParent() const noexcept;
	/// Searches the first parent with the specified name, and tries to cast it to T. throws a WidgetNotFound if the widget wasn't found. @see Widget::search
	template<typename T = Widget> shared<T> findParent(const char* name) const;
	/// Returns the first parent dynamic_cast-able to T* or throws a WidgetNotFound. @see Widget::search
	template<typename T = Widget> shared<T> findParent() const;
	/// Returns root (including this)
	shared<Widget> findRoot() const noexcept;


	// ** Events *******************************************************

	/// Sends a click event and returns whether the event was handled.
	bool send(Click const& event);
	/// Sends a scroll event and returns whether the event was handled
	bool send(Scroll const& event);
	/// Sends a move event and returns whether the event was handled.
	bool send(Moved const& event);
	/// Sends a drag event, if it wasn't handled it resends it as move event. Returns whether the event was handled
	bool send(Dragged const& event);
	/// Sends a key event and returns whether the event was handled.
	bool send(KeyEvent const& event);
	/// Sends a text input event and returns whether the event was handled.
	bool send(TextInput const& event);

	/// Draws the widget using the canvas
	void draw(Canvas& canvas, bool minimal = false);

	/// Update (primarily animations)
	void update(float dt);

	/// Update layout
	bool updateLayout(); //<! Updates layout if the FlagNeedsRelayout is set, returns false if nothing was updated. @see forceRelayout()
	bool forceRelayout(PreferredSize const& constraint = {}); //<! Makes this widget relayout NOW
	void requestRelayout(); //<! Sets the FlagNeedsRelayout @see forceRelayout
	void preferredSizeChanged(); //<! Notifies parent that this widget wants a different size
	void alignmentChanged(); //<! Notifies parent that this widget wants a different alignment
	void paddingChanged(); //<! Notifies parent that this widget wants a different padding

	/// Minimal redraw
	void requestRedraw();

	// Focus
	bool requestFocus(FocusType type = FOCUS_FORCE); //<! Try to get the focus to this widget
	bool removeFocus(FocusType type = FOCUS_FORCE); //<! Try to remove the focus from this widget
	bool clearFocus(FocusType type = FOCUS_FORCE); //<! Try to clear any focus that's in this branch

	// ** Shortcuts to add stuff**************************************************
	Widget& text(std::string const& s);
	std::string text();
	Widget& image(std::string const& s);
	shared<Image>  image();


	// ** Getters & Setters *******************************************************
	PreferredSize const& preferredSize(PreferredSize const& constraint);

	inline shared<Widget> const& nextSibling() const noexcept { return mNextSibling; }
	inline shared<Widget>        prevSibling() const noexcept { return mPrevSibling.lock(); }
	inline shared<Widget>        parent()      const noexcept { return mParent.lock(); }
	inline shared<Widget> const& children()    const noexcept { return mChildren; }
	shared<Widget>        lastChild()   const noexcept;

	Context* context() const noexcept { return mContext; }
	Widget&  context(Context* ctxt);

	inline const char* name() const noexcept { return mName.c_str(); }
	inline Widget& name(std::string const& n) noexcept { mName.reset(n.data(), n.length()); return *this; }

	std::vector<TinyString> const& classes() const noexcept { return mClasses; }
	Widget& classes(std::string const& s) noexcept;
	Widget& classes(std::initializer_list<std::string> classes) noexcept;

	inline HalfAlignment alignx() const noexcept { return mAlign.x; }
	inline HalfAlignment aligny() const noexcept { return mAlign.y; }
	inline float offsetx() const noexcept { return mOffset.x; }
	inline float offsety() const noexcept { return mOffset.y; }
	inline float width()   const noexcept { return mSize.x; }
	inline float height()  const noexcept { return mSize.y; }
	inline float paddedWidth()   const noexcept { return width() + padding().horizontal(); }
	inline float paddedHeight()  const noexcept { return height() + padding().vertical(); }
	inline float padLeft() const noexcept { return mPadding.left; }
	inline float padRight() const noexcept { return mPadding.right; }
	inline float padTop() const noexcept { return mPadding.top; }
	inline float padBottom() const noexcept { return mPadding.bottom; }
	inline Padding const& padding() const noexcept { return mPadding; }
	inline float padX() const noexcept { return padLeft() + padRight(); }
	inline float padY() const noexcept { return padTop() + padBottom(); }

	Widget&     size(float w, float h);
	Widget&     size(Size const& size);
	Size const& size() const noexcept { return mSize; }
	Widget&     width(float w);
	Widget&     height(float h);

	Widget&       offset(float x, float y);
	Offset const& offset() const noexcept { return mOffset; }
	Widget&       offsetx(float x);
	Widget&       offsety(float y);

	Widget& align(Alignment x);
	Widget& align(HalfAlignment x, HalfAlignment y);
	Widget& alignx(HalfAlignment x);
	Widget& aligny(HalfAlignment y);

	Widget& padding(float left, float top, float right, float bottom);
	Widget& padding(float left_and_right, float top_and_bottom);
	Widget& padding(float all_around);

	// ** Set functions *******************************************************
	template<class Arg1, class Arg2, class... ArgN>
	Widget& set(Arg1&& arg1, Arg2&& arg2, ArgN&&... argN) {
		set(std::forward<Arg1>(arg1));
		set(std::forward<Arg2>(arg2));
		(set(std::forward<ArgN>(argN)), ...);
		return *this;
	}

	template<class SomeWidget>
	std::enable_if_t< // We really need concepts TS :(
		std::is_move_constructible_v<SomeWidget> && std::is_base_of_v<Widget, SomeWidget>,
	Widget&> set(SomeWidget&& w) {
		return *add<std::remove_reference_t<SomeWidget>>(std::forward<SomeWidget>(w));
	}

	Widget& set(Name&& nam);
	Widget& set(Class&& cls);
	Widget& set(std::initializer_list<Class> clss);

	Widget& set(Padding const& pad);
	Widget& set(Alignment const& align);
	Widget& set(Offset const& off);
	Widget& set(Size const& size);

	// ** Queries *******************************************************

	Offset absoluteOffset(Widget const* relativeToParent = nullptr);

	inline bool needsRelayout() const noexcept { return mFlags.needsRelayout; }
	inline bool childNeedsRelayout() const noexcept { return mFlags.childNeedsRelayout; }
	inline bool focused() const noexcept { return mFlags.focused; }
	inline bool childFocused() const noexcept { return mFlags.childFocused; }

	shared<Widget> findFocused() noexcept;

	operator Widget*() noexcept { return this; }
	operator Widget const*() const noexcept { return this; }

	// ** Backend shortcuts *******************************************************
	void defer(std::function<void()> fn);
	// void deferDraw(std::function<void()> fn);

	shared<Bitmap> loadImage(std::string const& url);

	// ** Iterator utilities *******************************************************
	template<typename C> void eachChild(C&& c);
	template<typename C> void eachDescendendPreOrder(C&& c);
	template<typename C> void eachDescendendPostOrder(C&& c);
	template<typename C> void eachPreOrder(C&& c);
	template<typename C> void eachPostOrder(C&& c);

	template<typename C> void eachChildConditional(C&& c);
	template<typename C> void eachDescendendPreOrderConditional(C&& c);
	template<typename C> void eachDescendendPostOrderConditional(C&& c);;
	template<typename C> void eachPreOrderConditional(C&& c);
	template<typename C> void eachPostOrderConditional(C&& c);
};

template<>
shared<Widget> Widget::search<Widget>(const char* name) noexcept;

} // namespace wwidget

#include "Widget.inl"
