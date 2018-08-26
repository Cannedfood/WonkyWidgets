#ifndef STX_SHARED_PTR_HPP_INCLUDED
#define STX_SHARED_PTR_HPP_INCLUDED

#pragma once

#include <exception>
#include <utility>
#include <memory>

// TODO: optimize memory usage with enable_shared_from_this: Only needs one pointer

namespace stx {

// =============================================================
// == Forward declarations =============================================
// =============================================================

struct shared_block;

template<class T>
struct enable_shared_from_this;

template<class T>
class shared;
template<class T>
class weak;

template<class T>
class owned;

struct shared_block {
	int m_strong_refs = 1;
	int m_weak_refs   = 0;

	constexpr shared_block() noexcept {}
	constexpr shared_block(shared_block const& other) noexcept : shared_block() {}
	constexpr shared_block(shared_block&& other) noexcept : shared_block() {}
	constexpr shared_block& operator=(shared_block const& other) noexcept { return *this; }
	constexpr shared_block& operator=(shared_block&& other) noexcept { return *this; }

	virtual void shared_block_destroy() noexcept = 0;
	virtual void shared_block_free() noexcept = 0;

	bool add_strong_ref() noexcept {
		if(m_strong_refs == 0) return false;
		m_strong_refs++;
		return true;
	}
	void remove_strong_ref() noexcept {
		if(--m_strong_refs == 0) {
			shared_block_destroy();
			if(m_weak_refs == 0) {
				shared_block_free();
			}
		}
	}
	bool add_weak_ref() noexcept {
		if(m_strong_refs == 0) return false;
		++m_weak_refs;
		return true;
	}
	void remove_weak_ref() noexcept {
		if(--m_weak_refs == 0) {
			if(m_strong_refs == 0) {
				shared_block_free();
			}
		}
	}
};

template<class T>
class default_shared_block final : public shared_block {
	alignas(T) unsigned char m_data[sizeof(T)];

public:
	template<class... Args>
	default_shared_block(Args&&... args) noexcept {
		T* tmp = new(m_data) T(std::forward<Args>(args)...);
		if(!((unsigned char*)tmp == m_data)) std::terminate();
	}

	T* value() { return (T*) m_data; }

	void shared_block_destroy() noexcept override { value()->~T(); }
	void shared_block_free() noexcept override { delete this; }
};

template<class T, class Deleter = std::default_delete<T>>
class pointer_shared_block final : public shared_block {
	using Tptr = std::remove_all_extents_t<T>*;

	Tptr    m_data;
	Deleter m_deleter;
public:
	pointer_shared_block(Tptr data, Deleter deleter = Deleter()) noexcept :
		m_data(data),
		m_deleter(std::move(deleter))
	{}

	Tptr value() { return m_data; }

	void shared_block_destroy() noexcept override { m_deleter(m_data); }
	void shared_block_free() noexcept override { delete this; }
};

template<class T>
class shared {
	using Tptr = std::remove_all_extents_t<T>*;
	// using Tref = std::remove_pointer_t<Tptr>&; "Can't form reference to void"

	Tptr          m_value;
	shared_block* m_block;

	template<class>
	friend class weak;
	template<class>
	friend class shared;
public:
	~shared() noexcept { reset(); }

	shared(std::nullptr_t = nullptr) noexcept : m_value(nullptr), m_block(nullptr) {}
	shared& operator=(std::nullptr_t) noexcept { reset(nullptr); return *this; }
	void reset(std::nullptr_t = nullptr) noexcept {
		auto* block = m_block;
		m_block = nullptr;
		m_value = nullptr;
		if(block) {
			block->remove_strong_ref();
		}
	}

	shared(Tptr data) :
		m_value(data),
		m_block(new pointer_shared_block<T, std::default_delete<T>>(data))
	{}

	template<class Deleter>
	shared(Tptr data, Deleter del) :
		m_value(data),
		m_block(new pointer_shared_block<T, Deleter>(data, del))
	{}

	// Move
	shared(shared&& other) noexcept : shared() { reset(std::move(other)); }
	shared& operator=(shared&& other) noexcept { reset(std::move(other)); return *this; }
	void reset(shared&& other) noexcept {
		_move_reset(std::exchange(other.m_value, nullptr),
								std::exchange(other.m_block, nullptr));
	}

	// Copy
	shared(shared const& other) noexcept : shared() { reset(other); }
	shared& operator=(shared const& other) noexcept { reset(other); return *this; }
	void reset(shared const& other) noexcept {
		_copy_reset(other.m_value, other.m_block);
	}

	// Move related
	template<class OtherT>
	shared(shared<OtherT>&& other) noexcept : shared() { reset(std::move(other)); }
	template<class OtherT>
	shared& operator=(shared<OtherT>&& other) noexcept { reset(std::move(other)); return *this; }
	template<class OtherT>
	void reset(shared<OtherT>&& other) noexcept {
		static_assert(std::is_same_v<T, OtherT> || std::is_base_of_v<T, OtherT>, "T -> OtherT is not a trivial cast");
		_move_reset(std::exchange(other.m_value, nullptr),
		            std::exchange(other.m_block, nullptr));
	}

	// Copy related
	template<class OtherT>
	shared(shared<OtherT> const& other) noexcept : shared() { reset(other); }
	template<class OtherT>
	shared& operator=(shared<OtherT> const& other) noexcept { reset(other); return *this; }
	template<class OtherT>
	void reset(shared<OtherT> const& other) noexcept {
		static_assert(std::is_same_v<T, OtherT> || std::is_base_of_v<T, OtherT>, "T -> OtherT is not a trivial cast");
		_copy_reset(other.m_value, other.m_block);
	}

	int refcount() const noexcept { return m_block ? m_block->m_strong_refs : 0; }
	int weak_refcount() const noexcept { return m_block ? m_block->m_weak_refs : 0; }

	// Internal
	void _copy_reset(Tptr value, shared_block* block) noexcept {
		reset();
		if(block && block->add_strong_ref()) {
			m_value = value;
			m_block = block;
		}
		else {
			m_value = nullptr;
			m_block = nullptr;
		}
	}

	void _move_reset(Tptr value, shared_block* block) noexcept {
		reset();
		m_value = value;
		m_block = block;
	}

	// Operators
	operator bool() const noexcept { return m_block != nullptr; }
	bool operator==(shared const& other) const noexcept { return m_block == other.m_block; }
	bool operator!=(shared const& other) const noexcept { return m_block != other.m_block; }
	bool operator> (shared const& other) const noexcept { return m_block >  other.m_block; }
	bool operator>=(shared const& other) const noexcept { return m_block >= other.m_block; }
	bool operator< (shared const& other) const noexcept { return m_block <  other.m_block; }
	bool operator<=(shared const& other) const noexcept { return m_block <= other.m_block; }

	// Function stuff
	Tptr  operator->() const noexcept { return m_value; }
	auto& operator*()  const noexcept { return *m_value; }
	Tptr  get() const noexcept { return m_value; }

	template<class Tx>
	shared<Tx> cast_dynamic() const noexcept {
		using Txptr = typename shared<Tx>::Tptr;

		Txptr ptr = dynamic_cast<Txptr>(get());
		if(!ptr) return nullptr;
		shared<Tx> result;
		result._copy_reset(ptr, m_block);
		return result;
	}

	template<class Tx>
	shared<Tx> cast_static() const noexcept {
		using Txptr = typename shared<Tx>::Tptr;

		shared<Tx> result;
		result._copy_reset(static_cast<Txptr>(get()), m_block);
		return result;
	}
};

template<class T>
class weak {
	T*            m_value;
	shared_block* m_block;
public:
	constexpr
	weak() noexcept : m_value(nullptr), m_block(nullptr) {}
	~weak() noexcept { reset(); }
	void reset() noexcept {
		if(auto* block = m_block) {
			m_block = nullptr;
			m_value = nullptr;
			block->remove_weak_ref();
		}
	}

	// Move
	constexpr
	weak(weak&& other) noexcept : weak() { reset(std::move(other)); }
	weak& operator=(weak&& other) noexcept { reset(std::move(other)); return *this; }
	void reset(weak&& other) noexcept {
		_move_reset(std::exchange(other.m_value, nullptr),
		            std::exchange(other.m_block, nullptr));
	}

	// Copy
	weak(weak const& other) noexcept : weak() { reset(other); }
	weak& operator=(weak const& other) noexcept { reset(other); return *this; }
	void reset(weak const& other) noexcept { _copy_reset(other.m_value, other.m_block); }

	int refcount() const noexcept { return m_block ? m_block->m_strong_refs : 0; }
	int weak_refcount() const noexcept { return m_block ? m_block->m_weak_refs : 0; }

	// Internal
	void _copy_reset(T* value, shared_block* block) noexcept {
		reset();
		if(block && block->add_weak_ref()) {
			m_value = value;
			m_block = block;
		}
		else {
			m_value = nullptr;
			m_block = nullptr;
		}
	}

	void _move_reset(T* value, shared_block* block) noexcept {
		reset();
		m_value = value;
		m_block = block;
	}

	// From shared pointer
	weak(shared<T> const& other) noexcept : weak() { reset(other); }
	weak& operator=(shared<T> const& other) noexcept { reset(other); return *this; }
	void reset(shared<T> const& other) noexcept { _copy_reset(other.m_value, other.m_block); }

	// Function stuff
	shared<T> lock() const noexcept {
		shared<T> result;
		result._copy_reset(m_value, m_block);
		return result;
	}
};

template<class T>
struct enable_shared_from_this : public shared_block {
protected:
	enable_shared_from_this() {}

	T* value() const noexcept { return const_cast<T*>(static_cast<T const*>(this)); }
	shared_block* block() const noexcept { return const_cast<shared_block*>(static_cast<shared_block const*>(this)); }

	void shared_block_destroy() noexcept override {}
	void shared_block_free() noexcept override {
		delete (T*)this;
	}
public:
	shared<T> shared_from_this() const noexcept {
		shared<T> result;
		result._copy_reset(value(), block());
		return result;
	}

	weak<T> weak_from_this() const noexcept {
		weak<T> result;
		result._copy_reset(value(), block());
		return result;
	}

	operator shared<T>() const noexcept { return shared_from_this(); }
};

template<class T, class... Args>
shared<T> make_shared(Args&&... args) {
	shared<T> result;
	if constexpr(std::is_base_of_v<shared_block, T>) {
		auto* block = new T(std::forward<Args>(args)...);
		result._move_reset(block, block);
	}
	else {
		auto* block = new default_shared_block<T>(std::forward<Args>(args)...);
		result._move_reset(block->value(), block);
	}
	return result;
}

/*
template<class T>
class owned {
	T* m_value;
public:
	constexpr owned() noexcept : m_value(nullptr) {}
	~owned() { reset(); }

	explicit owned(T* value) : m_value(value) {}

	constexpr
	owned(owned&& other) noexcept : m_value(std::exchange(other.m_value, nullptr)) {}
	constexpr
	owned& operator=(owned&& other) noexcept { reset(std::exchange(other.m_value, nullptr)); return *this; }

	void reset(std::nullptr_t = nullptr) {
		if(m_value)
			delete m_value;
	}

	owned(owned const& other) = delete;
	owned& operator=(owned const& other) = delete;
};
*/

} // namespace stx

#endif // header guard STX_SHARED_PTR_HPP_INCLUDED
