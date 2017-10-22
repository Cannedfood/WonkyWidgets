#pragma once

#include <xmemory>
#include <xhash>

#include <string>
#include <bitset>
#include <limits>

#include <stdexcept>

#include "Utility.hpp"

namespace widget {

class UserInterface;
class WidgetGraphics;

using Size = Area;

#define WIDGET_MAGIC_CONSTRUCTOR_ATTRIBUTE(TYPE, SETTER_BODY) void __Widget_constructorAttribute(TYPE value) { SETTER_BODY; }

/// Declares a constructor which forwards the arguments to the WIDGET_MAGIC_CONSTRUCTOR_ATTRIBUTE with the corresponding type
#define WIDGET_MAGIC_CONSTRUCTOR(CLASS) \
	void __Widget_constructorAttribute() {} \
	template<typename T, typename... ARGS> \
	void __Widget_constructorAttribute(T&& t, ARGS&&... args) {  \
		__Widget_constructorAttribute(std::forward<T>(t)); \
		__Widget_constructorAttribute(std::forward<ARGS>(args)...); \
	} \
	template<typename T, typename... ARGS> \
	CLASS(T&& t, ARGS&&... args) : \
		CLASS() \
	{ \
		__Widget_constructorAttribute(std::forward<T>(t)); \
		__Widget_constructorAttribute(std::forward<ARGS>(args)...); \
	}

/**
 * Widget is the base class of all widget windows etc.
 * The Ui is build as a tree of widgets, where the children of each widget are stored as a linked list.
 */
class Widget {
	std::string mName;

	Area mArea;

	mutable Widget* mParent;
	mutable Widget* mNextSibling;
	mutable Widget* mPrevSibling;
	mutable Widget* mChildren;

	WidgetGraphics* mGraphics;

	enum {
		FlagOwnedByParent,
		kNumFlags
	};

	std::bitset<kNumFlags> mFlags;

	void notifyChildAdded(Widget* newChild);
	void notifyChildRemoved(Widget* noLongerChild);

protected:
	WIDGET_MAGIC_CONSTRUCTOR_ATTRIBUTE(Name&&,  mName = std::move(value));
	WIDGET_MAGIC_CONSTRUCTOR_ATTRIBUTE(Area&&,  mArea = std::move(value));
	WIDGET_MAGIC_CONSTRUCTOR_ATTRIBUTE(Widget*, value->add(this));

	virtual void onAddTo(Widget* w);
	virtual void onRemovedFrom(Widget* parent);

	virtual void onAdd(Widget* w);
	virtual void onRemove(Widget* w);

public:
	Widget();
	virtual ~Widget() noexcept;

	WIDGET_MAGIC_CONSTRUCTOR(Widget)

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
};

template<>
Widget* Widget::search<Widget>(const char* name) noexcept;

} // namespace widget


// =============================================================
// == Inline implementation =============================================
// =============================================================

#include "Error.hpp"

namespace widget {

template<typename T>
T* Widget::search(const char* name) noexcept {
	return dynamic_cast<T*>(search<Widget>(name));
}

template<typename T>
T* Widget::search() noexcept {
	for(auto* c = mChildren; c; c = c->mNextSibling) {
		if(T* result = dynamic_cast<T*>(c)) {
			return result;
		}
		if(T* result = c->search<T>()) {
			return result;
		}
	}
	return nullptr;
}

template<typename T>
T* Widget::find(const char* name) {
	auto* w = search<T>(name);
	if(w == nullptr) {
		throw exceptions::WidgetNotFound(this, mName.c_str(), typeid(T).name(), name);
	}
	return w;
}

template<typename T>
T* Widget::find() {
	auto* w = search<T>();
	if(w == nullptr) {
		throw exceptions::WidgetNotFound(this, mName.c_str(), typeid(T).name(), "");
	}
	return w;
}


template<typename C>
void Widget::eachChild(C&& c) {
	for(auto* w = children(); w; w = w->nextSibling()) {
		c(w);
	}
}
template<typename C>
void Widget::eachDescendendPreOrder(C&& c) {
	eachChild([&](Widget* w) {
		c(w);
		w->eachDescendendPreOrder(c);
	});
}
template<typename C>
void Widget::eachDescendendPostOrder(C&& c) {
	eachChild([&](Widget* w) {
		w->eachDescendendPostOrder(c);
		c(w);
	});
}
template<typename C>
void Widget::eachPreOrder(C&& c) {
	c(this);
	eachDescendendPreOrder(c);
}
template<typename C>
void Widget::eachPostOrder(C&& c) {
	eachDescendendPostOrder(c);
	c(this);
}

template<typename C>
void Widget::eachChild(C&& c) const {
	for(const auto* w = children(); w; w = w->nextSibling()) {
		c(w);
	}
}
template<typename C>
void Widget::eachDescendendPreOrder(C&& c) const {
	eachChild([&](Widget* w) {
		c(w);
		w->eachDescendendPreOrder(c);
	});
}
template<typename C>
void Widget::eachDescendendPostOrder(C&& c) const {
	eachChild([&](Widget* w) {
		w->eachDescendendPostOrder(c);
		c(w);
	});
}
template<typename C>
void Widget::eachPreOrder(C&& c) const {
	c(this);
	eachDescendendPreOrder(c);
}
template<typename C>
void Widget::eachPostOrder(C&& c) const {
	eachDescendendPostOrder(c);
	c(this);
}

} // namespace widget
