#pragma once

#include <string>
#include <set>
#include <bitset>
#include <limits>
#include <memory>

#include <stdexcept>

#ifdef WIDGET_ULTRA_VERBOSE
	#include "debug/Marker.hpp"
#else
	#define WIDGET_FN_MARKER
	#define WIDGET_M_FN_MARKER
	#define WIDGET_ENABLE_MARKERS
#endif

#include "Utility.hpp"
#include "Events.hpp"

namespace widget {

class AttributeCollectorInterface;
class UserInterface;
class Canvas;

/**
 * Widget is the base class of all widget windows etc.
 * The Ui is build as a tree of widgets, where the children of each widget are stored as a linked list.
 */
class Widget {
	enum Flag {
		FlagOwnedByParent,
		FlagChildNeedsRelayout,
		FlagNeedsRelayout,
		kNumFlags
	};

	std::string           mName;
	std::set<std::string> mClasses;

	float mWidth;
	float mHeight;

	float mOffsetX;
	float mOffsetY;

	mutable Widget* mParent;
	mutable Widget* mNextSibling;
	mutable Widget* mPrevSibling;
	mutable Widget* mChildren;

	std::bitset<kNumFlags> mFlags;

	void notifyChildAdded(Widget* newChild);
	void notifyChildRemoved(Widget* noLongerChild);

	void drawBackgroundRecursive(Canvas& canvas);
	void drawForegroundRecursive(Canvas& canvas);
protected:
	// ** Overidable event receivers *******************************************************

	// Tree changed events
	virtual void onAddTo(Widget* w);
	virtual void onRemovedFrom(Widget* parent);

	virtual void onAdd(Widget* w);
	virtual void onRemove(Widget* w);

	// Layout events
	virtual void onResized();
	virtual void onChildPreferredSizeChanged(Widget* child);
	virtual void onCalculateLayout(LayoutInfo& out_info);
	virtual void onLayout();

	// Input events
	virtual void on(Click const& c);

	// Drawing events
	virtual void onDrawBackground(Canvas& graphics);
	virtual void onDraw(Canvas& graphics);

	virtual void onUpdate(float dt);

public:
	// Attributes
	virtual bool setAttribute(std::string const& name, std::string const& value);
	virtual void getAttributes(AttributeCollectorInterface& collector);

public:
	Widget();
	virtual ~Widget() noexcept;

	// ** Move *******************************************************
	Widget(Widget&& other);
	Widget& operator=(Widget&& other);

	// ** Copy *******************************************************
	Widget(Widget const& other);
	Widget& operator=(Widget const& other);

	// ** Tree operations *******************************************************

	/// Adds the widget, usually as the first child.
	void add(Widget* w);
	/// Adds the widget, usually as the first child.
	void add(Widget& w);
	/// Adds the widget, usually as the first child and transfers ownership to this widget. It also returns a pointer to the added widget.
	Widget* add(std::unique_ptr<Widget>&& w);
	/// Shortcut for Widget::add(std::make_unique<T>(...))
	template<typename T, typename... ARGS>
	T* add(ARGS&&... args);
	/// Inserts a widget as next sibling (duh)
	void insertNextSibling(Widget* w);
	/// Inserts a widget as previous sibling (duh)
	void insertPrevSibling(Widget* w);
	/// Makes w take the place of this widget and makes it a child of it
	void insertAsParent(Widget* w);
	/// Removes this widget and makes it's children take it's place
	std::unique_ptr<Widget> extract();
	/// Removes this widget and its children. Returns ownership if the widget has the flag FlagOwnedByParent @see extract
	std::unique_ptr<Widget> remove();
	/// Removes this widget and its children. Returns ownership if the widget has the flag FlagOwnedByParent @see extract
	std::unique_ptr<Widget> quietRemove();

	/// If the widet has the FlagOwnedByParent it unsets the flag and returns a unique_ptr to this widget
	std::unique_ptr<Widget> getOwnership();

	void clearChildren();

	/// Dynamic casts to T&
	template<typename T>
	T&       as()       { return dynamic_cast<T&>(*this); }
	/// Dynamic casts to T const&
	template<typename T>
	T const& as() const { return dynamic_cast<T const&>(*this); }

	/// Searches the (depth-)first widget with the specified name, and tries to cast it to T. Returns a nullptr on failure. @see Widget::search
	template<typename T = Widget> T* search(const char* name) noexcept;
	/// Returns the (depth-)first widget dynamic_cast-able to T* or a nullptr.
	template<typename T = Widget> T* search() noexcept;
	/// Searches the (depth-)first widget with the specified name, and tries to cast it to T. throws a WidgetNotFound if the widget wasn't found. @see Widget::search
	template<typename T = Widget> T* find(const char* name);
	/// Returns the (depth-)first widget dynamic_cast-able to T* or throws a WidgetNotFound. @see Widget::search
	template<typename T = Widget> T* find();

	/// Searches the first parent with the specified name, and tries to cast it to T. Returns a nullptr on failure. @see Widget::search
	template<typename T = Widget> T* searchParent(const char* name) noexcept;
	/// Returns the first parent dynamic_cast-able to T* or a nullptr.
	template<typename T = Widget> T* searchParent() noexcept;
	/// Searches the first parent with the specified name, and tries to cast it to T. throws a WidgetNotFound if the widget wasn't found. @see Widget::search
	template<typename T = Widget> T* findParent(const char* name);
	/// Returns the first parent dynamic_cast-able to T* or throws a WidgetNotFound. @see Widget::search
	template<typename T = Widget> T* findParent();
	/// Returns root
	Widget* findRoot();


	// ** Events *******************************************************

	/// Sends a click event and returns whether the event was handled.
	bool send(Click const& click);

	/// Draws the widget using the canvas
	void draw(Canvas& canvas);

	/// Update (primarily animations)
	void update(float dt);

	/// Update layout
	bool updateLayout(); //<! Updates layout if the FlagNeedsRelayout is set, returns false if nothing was updated. @see forceRelayout()
	void forceRelayout(); //<! Makes this widget relayout NOW
	void requestRelayout(); //<! Sets the FlagNeedsRelayout @see forceRelayout
	void preferredSizeChanged(); //<! Notifies parents that this widget wants a different size

	// ** Getters & Setters *******************************************************

	void getLayoutInfo(LayoutInfo& info);

	inline Widget* nextSibling() const noexcept { return mNextSibling; }
	inline Widget* prevSibling() const noexcept { return mPrevSibling; }
	inline Widget* parent()      const noexcept { return mParent; }
	inline Widget* children()    const noexcept { return mChildren; }
	Widget* lastChild()   const noexcept;

	inline std::string const& name() const noexcept { return mName; }
	inline void name(std::string const& n) noexcept { mName = n; }

	inline float offsetx() const noexcept { return mOffsetX; }
	inline float offsety() const noexcept { return mOffsetY; }
	inline float width()   const noexcept { return mWidth; }
	inline float height()  const noexcept { return mHeight; }
	Widget* size(float w, float h);
	Widget* offset(float x, float y);
	Widget* width(float w);
	Widget* height(float h);
	Widget* offsetx(float x);
	Widget* offsety(float y);

	inline bool ownedByParent() const noexcept { return mFlags[FlagOwnedByParent]; }
	inline bool needsRelayout() const noexcept { return mFlags[FlagNeedsRelayout]; }
	inline bool childNeedsRelayout() const noexcept { return mFlags[FlagChildNeedsRelayout]; }


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
Widget* Widget::search<Widget>(const char* name) noexcept;

} // namespace widget

#include "Widget.inl"
