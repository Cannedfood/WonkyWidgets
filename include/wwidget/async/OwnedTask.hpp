#pragma once

#include "../Ownership.hpp"

#include <type_traits>
#include <utility>

namespace wwidget {

template<class C>
struct OwnedTask : public OwnedObject {
	std::remove_reference_t<C> c;

	OwnedTask(OwnedTask<C>& other) :
		OwnedObject(std::move(other)),
		c(std::move(other.c))
	{}
	OwnedTask& operator=(OwnedTask<C>& other) { return *this = std::move(other); }

	OwnedTask(OwnedTask<C>&& other) :
		OwnedObject(std::move(other)),
		c(std::move(other.c))
	{}
	OwnedTask& operator=(OwnedTask<C>&& other) {
		OwnedObject::operator=(std::move(other));
		c = std::move(other.c);
		return *this;
	}

	template<class... Args>
	OwnedTask(Args&&... args) :
		c(std::forward<Args>(args)...)
	{}

	void becameOrphan() override {}

	template<class... Args>
	void operator()(Args&&... args) {
		if(!isOrphan()) {
			removeFromOwner();
			c(std::forward<Args>(args)...);
		}
	}
};

template<class C>
OwnedTask<C> makeOwnedTask(Owner* o, C&& c) {
	auto task = OwnedTask<C>(std::forward<C>(c));
	o->transferOwnership(&task);
	return task;
}

} // namespace wwidget
