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

template<typename C>
void Widget::eachChildConditional(C&& c) {
	for(auto* w = children(); w; w = w->nextSibling()) {
		if(!c(w)) break;
	}
}

} // namespace widget
