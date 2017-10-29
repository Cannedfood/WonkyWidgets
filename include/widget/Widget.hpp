#pragma once

#include <xmemory>
#include <xhash>

#include <string>
#include <set>
#include <bitset>
#include <limits>

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
		kNumFlags
	};

	std::bitset<kNumFlags> mFlags;

	void notifyChildAdded(Widget* newChild);
	void notifyChildRemoved(Widget* noLongerChild);

protected:
	virtual void onAddTo(Widget* w);
	virtual void onRemovedFrom(Widget* parent);

	virtual void onAdd(Widget* w);
	virtual void onRemove(Widget* w);

	virtual void on(Click const& c);

	virtual void onDrawBackground(Canvas& graphics);
	virtual void onDraw(Canvas& graphics);

public:
	Widget();
	virtual ~Widget() noexcept;

	/// Adds the widget, usually as the first child.
	void add(Widget* w);
	/// Inserts a widget as next sibling (duh)
	void insertNextSibling(Widget* w);
	/// Inserts a widget as previous sibling (duh)
	void insertPrevSibling(Widget* w);
	/// Makes w take the place of this widget and makes it a child of it
	void insertAsParent(Widget* w);
	/// Removes this widget and makes it's children take it's place
	void extract();
	/// Removes this widget and its children. @see extract
	void remove();


	/// Searches the (depth-)first widget with the specified name, and tries to cast it to T. Returns a nullptr on failure. @see Widget::search
	template<typename T = Widget> T* search(const char* name) noexcept;
	/// Returns the (depth-)first widget dynamic_cast-able to T* or a nullptr.
	template<typename T = Widget> T* search() noexcept;

	/// Searches the (depth-)first widget with the specified name, and tries to cast it to T. throws a WidgetNotFound if the widget wasn't found. @see Widget::search
	template<typename T = Widget> T* find(const char* name);
	/// Returns the (depth-)first widget dynamic_cast-able to T* or throws a WidgetNotFound. @see Widget::search
	template<typename T = Widget> T* find();

	/// Sends a click event and returns whether the event was handled.
	bool send(Click const& click);

	/// Draws the widget using the canvas
	void draw(Canvas& canvas);

	inline Widget* nextSibling() const noexcept { return mNextSibling; }
	inline Widget* prevSibling() const noexcept { return mPrevSibling; }
	inline Widget* parent()      const noexcept { return mParent; }
	inline Widget* children()    const noexcept { return mChildren; }

	inline std::string const& name() const noexcept { return mName; }
	inline void name(std::string const& n) noexcept { mName = n; }

	inline Area&       area()       { return mArea; }
	inline Area const& area() const { return mArea; }

	inline bool ownedByParent() const noexcept { return mFlags[FlagOwnedByParent]; }

	/// Because the address of operator is annoying
	constexpr inline
	operator       Widget*()       noexcept { return this; }
	constexpr inline
	operator const Widget*() const noexcept { return this; }


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
