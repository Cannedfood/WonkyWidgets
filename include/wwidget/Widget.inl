#include "Error.hpp"

namespace wwidget {

template<typename T, typename... ARGS>
shared<T> Widget::add(ARGS&&... args) {
	return add(stx::make_shared<T>(std::forward<ARGS>(args)...)).template cast_static<T>();
}
template<>
shared<Widget> Widget::search<Widget>(const char* name) noexcept;
template<typename T>
shared<T> Widget::search(const char* name) noexcept {
	return search<Widget>(name).template cast_dynamic<T>();
}

template<typename T>
shared<T> Widget::search() noexcept {
	for(auto c = mChildren; c; c = c->mNextSibling) {
		if(shared<T> result = c.template cast_dynamic<T>()) {
			return result;
		}
		if(auto result = c->search<T>()) {
			return result;
		}
	}
	return nullptr;
}

template<typename T>
shared<T> Widget::find(const char* name) {
	if(auto w = search<T>(name))
		return w;
	throw exceptions::WidgetNotFound(this, mName.c_str(), typeid(T).name(), name);
}

template<typename T>
shared<T> Widget::find() {
	if(auto w = search<T>())
		return w;
	throw exceptions::WidgetNotFound(this, mName.c_str(), typeid(T).name(), "");
}

template<>
shared<Widget> Widget::searchParent<Widget>(const char* name) const noexcept;
template<typename T>
shared<T> Widget::searchParent(const char* name) const noexcept {
	return std::dynamic_pointer_cast<T>(searchParent<Widget>(name));
}
template<typename T>
shared<T> Widget::searchParent() const noexcept {
	for(shared<Widget> p = parent(); p; p = p->parent()) {
		if(shared<T> t = p.template cast_dynamic<T>()) {
			return t;
		}
	}
	return nullptr;
}

template<typename T>
shared<T> Widget::findParent(const char* name) const {
	if(auto w = searchParent<T>(name))
		return w;
	throw exceptions::WidgetNotFound(this, mName.c_str(), typeid(T).name(), name);
}
template<typename T>
shared<T> Widget::findParent() const {
	if(auto w = searchParent<T>())
		return w;
	throw exceptions::WidgetNotFound(this, mName.c_str(), typeid(T).name(), "");
}

template<typename C>
void Widget::eachChild(C&& c) {
	shared<Widget> next = mChildren;
	while(shared<Widget> child = next) {
		next = child->nextSibling();
		c(child);
	};
}
template<typename C>
void Widget::eachDescendendPreOrder(C&& c) {
	eachChild([&](shared<Widget> w) {
		c(w);
		w->eachDescendendPreOrder(c);
	});
}
template<typename C>
void Widget::eachDescendendPostOrder(C&& c) {
	eachChild([&](shared<Widget> w) {
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
void Widget::eachDescendendPreOrderConditional(C&& c) {
	eachChild([&](shared<Widget> w) {
		if(c(w)) {
			w->eachDescendendPreOrder(c);
		}
	});
}
template<typename C>
void Widget::eachPreOrderConditional(C&& c) {
	eachDescendendPreOrderConditional(c);
	c(*this);
}

template<typename C>
void Widget::eachChildConditional(C&& c) {
	shared<Widget> next = mChildren;
	while(shared<Widget> child = next) {
		next = child->nextSibling();
		if(!c(child)) return;
	};
}

} // namespace wwidget
