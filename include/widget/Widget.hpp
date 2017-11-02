#pragma once

#include <xmemory>
#include <xhash>

#include <string>
#include <set>
#include <bitset>
#include <limits>
#include <memory>

#include <stdexcept>

#include "Utility.hpp"
#include "Events.hpp"

namespace widget {

class UserInterface;
class Canvas;

/**
 * Widget is the base class of all widget windows etc.
 * The Ui is build as a tree of widgets, where the children of each widget are stored as a linked list.
 */
class Widget {
	std::string           mName;
	std::set<std::string> mClasses;

	Area mArea;

	mutable Widget* mParent;
	mutable Widget* mNextSibling;
	mutable Widget* mPrevSibling;
	mutable Widget* mChildren;

	enum {
		FlagOwnedByParent,
		FlagNeedsRelayout,
		kNumFlags
	};

	std::bitset<kNumFlags> mFlags;

	void notifyChildAdded(Widget* newChild);
	void notifyChildRemoved(Widget* noLongerChild);

	void drawBackground(Canvas& canvas);
	void drawForeground(Canvas& canvas);
protected:
	// ** Overidable event receivers *******************************************************

	// Tree changed events
	virtual void onAddTo(Widget* w);
	virtual void onRemovedFrom(Widget* parent);

	virtual void onAdd(Widget* w);
	virtual void onRemove(Widget* w);

	// Layout events
	virtual void onChildRequestRelayout(Widget* child);
	virtual void preLayout(LayoutInfo& out_info);
	virtual void onLayout();

	// Input events
	virtual void on(Click const& c);

	// Drawing events
	virtual void onDrawBackground(Canvas& graphics);
	virtual void onDraw(Canvas& graphics);

public:
	// Attributes
	virtual bool setAttribute(std::string const& s, std::string const& value);

public:
	Widget();
	virtual ~Widget() noexcept;


	// ** Tree operations *******************************************************

	/// Adds the widget, usually as the first child.
	void add(Widget* w);
	/// Adds the widget, usually as the first child and transfers ownership to this widget. It also returns a pointer to the added widget.
	Widget* add(std::unique_ptr<Widget>&& w);
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

	/// Searches the (depth-)first widget with the specified name, and tries to cast it to T. Returns a nullptr on failure. @see Widget::search
	template<typename T = Widget> T* search(const char* name) noexcept;
	/// Returns the (depth-)first widget dynamic_cast-able to T* or a nullptr.
	template<typename T = Widget> T* search() noexcept;

	/// Searches the (depth-)first widget with the specified name, and tries to cast it to T. throws a WidgetNotFound if the widget wasn't found. @see Widget::search
	template<typename T = Widget> T* find(const char* name);
	/// Returns the (depth-)first widget dynamic_cast-able to T* or throws a WidgetNotFound. @see Widget::search
	template<typename T = Widget> T* find();


	// ** Events *******************************************************

	/// Sends a click event and returns whether the event was handled.
	bool send(Click const& click);

	/// Draws the widget using the canvas
	void draw(Canvas& canvas);

	/// Update layout
	void forceRelayout();
	void requestRelayout();

	// ** Getters & Setters *******************************************************

	void getLayoutInfo(LayoutInfo& info);

	inline Widget* nextSibling() const noexcept { return mNextSibling; }
	inline Widget* prevSibling() const noexcept { return mPrevSibling; }
	inline Widget* parent()      const noexcept { return mParent; }
	inline Widget* children()    const noexcept { return mChildren; }

	inline std::string const& name() const noexcept { return mName; }
	inline void name(std::string const& n) noexcept { mName = n; }

	inline Area&       area()       { return mArea; }
	inline Area const& area() const { return mArea; }

	inline bool ownedByParent() const noexcept { return mFlags[FlagOwnedByParent]; }
	inline bool needsRelayout() const noexcept { return mFlags[FlagNeedsRelayout]; }

	/// Who needs & amiright? right?
	constexpr inline
	operator       Widget*()       noexcept { return this; }
	constexpr inline
	operator const Widget*() const noexcept { return this; }


	// ** Iterator utilities *******************************************************

	template<typename C> void eachChild(C&& c);
	template<typename C> void eachChild(C&& c) const;
	template<typename C> void eachDescendendPreOrder(C&& c);
	template<typename C> void eachDescendendPreOrder(C&& c) const;
	template<typename C> void eachDescendendPostOrder(C&& c);
	template<typename C> void eachDescendendPostOrder(C&& c) const;
	template<typename C> void eachPreOrder(C&& c);
	template<typename C> void eachPreOrder(C&& c) const;
	template<typename C> void eachPostOrder(C&& c);
	template<typename C> void eachPostOrder(C&& c) const;

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
