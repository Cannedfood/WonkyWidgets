#ifndef STX_SHARED_PTR_HPP_INCLUDED
#define STX_SHARED_PTR_HPP_INCLUDED

#pragma once

#include <exception>
#include <utility>
#include <memory>
#include <atomic>
#include <cassert>

// TODO: optimize memory usage with enable_shared_from_this: Only needs one pointer

namespace stx {

// =============================================================
// == Forward declarations =============================================
// =============================================================

template<class T> class shared; //<! Reference to a refcounted object
template<class T> class weak; //<! Weak reference to a refcounted object (To break circular references)

class shared_block; //<! Manages reference counting and object destruction.
template<class T> class default_shared_block; //<! A shared_block allocated like "struct { refcount refs; T value; }"
template<class T, class Deleter> class pointer_shared_block; class dummy_shared_block; //<! A shared_block for pointers (can take a deleter as argument)

template<class T> class enable_shared_from_this; //<! Make an object track it's shared block, so you can generate shared- and weak pointers directly from the object

// =============================================================
// == shared_block =============================================
// =============================================================

class shared_block {
public:
	/// Largest signed fundemental type, that is lock free
	using refcount =
		std::conditional_t<std::atomic<long long>::is_always_lock_free, long long,
		std::conditional_t<std::atomic<long>     ::is_always_lock_free, long,
		std::conditional_t<std::atomic<int>      ::is_always_lock_free, int,
		std::conditional_t<std::atomic<short>    ::is_always_lock_free, short,
		void>>>>;

	constexpr shared_block() noexcept {}
	constexpr shared_block(shared_block const& other)            = delete;
	constexpr shared_block(shared_block&& other)                 = delete;
	constexpr shared_block& operator=(shared_block const& other) = delete;
	constexpr shared_block& operator=(shared_block&& other)      = delete;

	/// To be overwritten
	virtual void shared_block_destroy() noexcept = 0;
	virtual void shared_block_free() noexcept = 0;

	refcount strong_refs() const noexcept { return std::max<refcount>(0, m_strong_refs); }
	refcount weak_refs() const noexcept { return m_weak_refs; }

	bool add_strong_ref() noexcept {
		return _increment_if_larger_zero(m_strong_refs);
	}
	void remove_strong_ref() noexcept {
		refcount strong_refs = --m_strong_refs;
		if(strong_refs == 0) {
			shared_block_destroy();
			if(m_weak_refs == 0) {
				m_weak_refs = -1;
				shared_block_free();
			}
			else {
				m_strong_refs = -1;
			}
		}
	}
	bool add_weak_ref() noexcept {
		if(m_strong_refs <= 0) return false;
		++m_weak_refs;
		return true;
	}
	void remove_weak_ref() noexcept {
		refcount weak_refs = --m_weak_refs;
		if(weak_refs == 0) {
			if(m_strong_refs == 0 && !_destruction_in_progress()) {
				m_weak_refs = -1;
				shared_block_free();
			}
		}
	}
private:
	/// 0 while object destruction in progress, -1 after
	/// (Necessary to prevent double frees when a object has a weak reference to itself)
	std::atomic<refcount> m_strong_refs = 1;

	/// 0 while block is up for deletion, is set to -1 just before free is called
	std::atomic<refcount> m_weak_refs   = 0;

	static
	bool _increment_if_larger_zero(std::atomic<refcount>& v) {
		refcount val = v.load();
		do if(val <= 0) return false;
		while(!v.compare_exchange_weak(val, val+1));
		return true;
	}

	bool _destruction_in_progress() const noexcept {
		return m_strong_refs.load() == 0;
	}
};

// ** Dummy shared_block *******************************************************

class dummy_shared_block final : public shared_block {
public:
	dummy_shared_block() { add_strong_ref(); }

	void shared_block_destroy() noexcept final override {}
	void shared_block_free() noexcept final override {}

	static dummy_shared_block* instance() {
		static dummy_shared_block _instance;
		return &_instance;
	}
};

// =============================================================
// == enable_shared_from_this ==================================
// =============================================================

namespace detail {

class enable_shared_from_this_base {
	mutable shared_block* m_shared_block = nullptr;

protected:
	shared_block* get_shared_block() const noexcept {
		return m_shared_block ? m_shared_block : dummy_shared_block::instance();
	}
public:
	constexpr enable_shared_from_this_base() noexcept {}
	constexpr enable_shared_from_this_base(enable_shared_from_this_base const&) noexcept {}
	constexpr enable_shared_from_this_base(enable_shared_from_this_base&&) noexcept {}
	constexpr enable_shared_from_this_base& operator=(enable_shared_from_this_base const&) noexcept { return *this; }
	constexpr enable_shared_from_this_base& operator=(enable_shared_from_this_base&&) noexcept { return *this; }

	template<class T, class Tptr>
	friend void handle_enable_shared_from_this(Tptr t, shared_block* b) noexcept;
};

template<class T, class Tptr>
void handle_enable_shared_from_this(Tptr object, shared_block* block) noexcept {
	static_assert(std::is_pointer_v<Tptr>, "Tptr needs to be a pointer");
	if constexpr(std::is_base_of_v<std::remove_all_extents_t<enable_shared_from_this_base>, T>) {
		static_assert(!std::is_array_v<T>, "enable_shared_from_this does not currently work with arrays");
		assert((object->m_shared_block == nullptr) && "Trying to attach a shared_block to an object that already has one");
		object->m_shared_block = block;
	}
	(void) object;
	(void) block;
}

} // namespace detail

template<class T>
class enable_shared_from_this : public detail::enable_shared_from_this_base {
protected:
	T* value() const noexcept { return const_cast<T*>(static_cast<T const*>(this)); }
public:
	shared<T> shared_from_this() const noexcept {
		shared<T> result;
		result._copy_reset(value(), get_shared_block());
		return result;
	}

	weak<T> weak_from_this() const noexcept {
		weak<T> result;
		result._copy_reset(value(), get_shared_block());
		return result;
	}

	operator shared<T>() const noexcept { return shared_from_this(); }
};

// =============================================================
// == shared_block implementations =============================
// =============================================================

// ** default_shared_block *******************************************************

template<class T>
class default_shared_block final : public shared_block {
	using Tptr = std::remove_all_extents_t<T>*;

	alignas(T) unsigned char m_data[sizeof(T)];

public:
	template<class... Args>
	default_shared_block(Args&&... args) noexcept {
		T* tmp = new(m_data) T(std::forward<Args>(args)...);
		if(!((unsigned char*)tmp == m_data)) std::terminate();
		detail::handle_enable_shared_from_this<T, Tptr>(tmp, this);
	}

	T* value() { return (T*) m_data; }

	void shared_block_destroy() noexcept override { value()->~T(); }
	void shared_block_free() noexcept override { delete this; }
};

// ** pointer_shared_block *******************************************************

template<class T, class Deleter = std::default_delete<T>>
class pointer_shared_block final : public shared_block {
	using Tptr = std::remove_all_extents_t<T>*;

	Tptr    m_data;
	Deleter m_deleter;
public:
	pointer_shared_block(Tptr data, Deleter deleter = Deleter()) noexcept :
		m_data(data),
		m_deleter(std::move(deleter))
	{
		detail::handle_enable_shared_from_this<T, Tptr>(data, this);
	}

	Tptr value() { return m_data; }

	void shared_block_destroy() noexcept override { m_deleter(m_data); }
	void shared_block_free() noexcept override { delete this; }
};

// =============================================================
// == shared<T> =============================================
// =============================================================

template<class T>
class shared {
	using Tptr = std::remove_all_extents_t<T>*;

	Tptr          m_value = nullptr;
	shared_block* m_block = nullptr;

	template<class>
	friend class weak;
	template<class>
	friend class shared;
public:
	~shared() noexcept { reset(); }

	shared(std::nullptr_t = nullptr) noexcept {}
	shared& operator=(std::nullptr_t) noexcept { reset(nullptr); return *this; }
	void reset(std::nullptr_t = nullptr) noexcept {
		shared_block* block = m_block;
		m_block = nullptr;
		m_value = nullptr;
		if(block) {
			block->remove_strong_ref();
		}
	}

	// Move
	shared(shared&& other) noexcept { reset(std::move(other)); }
	shared& operator=(shared&& other) noexcept { reset(std::move(other)); return *this; }
	void reset(shared&& other) noexcept {
		_move_reset(std::exchange(other.m_value, nullptr),
								std::exchange(other.m_block, nullptr));
	}

	// Copy
	shared(shared const& other) noexcept { reset(other); }
	shared& operator=(shared const& other) noexcept { reset(other); return *this; }
	void reset(shared const& other) noexcept { _copy_reset(other.m_value, other.m_block); }

	// Construct from pointer
	explicit
	shared(Tptr data) noexcept :
		m_value(data),
		m_block(!data ? nullptr : new pointer_shared_block<T, std::default_delete<T>>(data))
	{}

	template<class Deleter>
	shared(Tptr data, Deleter del) :
		m_value(data),
		m_block(!data ? nullptr : new pointer_shared_block<T, Deleter>(data, del))
	{}

	// Move related
	template<class OtherT>
	shared(shared<OtherT>&& other) noexcept { reset(std::move(other)); }
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
	shared(shared<OtherT> const& other) noexcept { reset(other); }
	template<class OtherT>
	shared& operator=(shared<OtherT> const& other) noexcept { reset(other); return *this; }
	template<class OtherT>
	void reset(shared<OtherT> const& other) noexcept {
		static_assert(std::is_same_v<T, OtherT> || std::is_base_of_v<T, OtherT>, "T -> OtherT is not a trivial cast");
		_copy_reset(other.m_value, other.m_block);
	}

	shared_block::refcount refcount() const noexcept { return m_block ? m_block->strong_refs() : 0; }
	shared_block::refcount weak_refcount() const noexcept { return m_block ? m_block->weak_refs() : 0; }

	// Internal
	void _copy_reset(Tptr value, shared_block* block) noexcept {
		if(block && block->add_strong_ref())
			_move_reset(value, block);
		else
			reset();
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
	Tptr  get()        const noexcept { return m_value; }

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

// =============================================================
// == weak<T> =============================================
// =============================================================

template<class T>
class weak {
	using Tptr = std::remove_all_extents_t<T>*;

	mutable Tptr          m_value = nullptr;
	mutable shared_block* m_block = nullptr;
public:
	constexpr
	weak(std::nullptr_t = nullptr) noexcept {}
	~weak() noexcept { reset(); }
	void reset(std::nullptr_t = nullptr) noexcept {
		auto* block = m_block;
		m_block = nullptr;
		m_value = nullptr;
		if(block) {
			block->remove_weak_ref();
		}
	}

	// Move
	constexpr
	weak(weak&& other) noexcept { reset(std::move(other)); }
	weak& operator=(weak&& other) noexcept { reset(std::move(other)); return *this; }
	void reset(weak&& other) noexcept {
		_move_reset(std::exchange(other.m_value, nullptr),
		            std::exchange(other.m_block, nullptr));
	}

	// Copy
	weak(weak const& other) noexcept { reset(other); }
	weak& operator=(weak const& other) noexcept { reset(other); return *this; }
	void reset(weak const& other) noexcept { _copy_reset(other.m_value, other.m_block); }

	shared_block::refcount refcount()      const noexcept { return m_block ? m_block->strong_refs() : 0; }
	shared_block::refcount weak_refcount() const noexcept { return m_block ? m_block->weak_refs() : 0; }

	// Internal
	void _copy_reset(T* value, shared_block* block) noexcept {
		if(block && block->add_weak_ref())
			_move_reset(value, block);
		else
			reset();
	}

	void _move_reset(T* value, shared_block* block) noexcept {
		reset();
		m_value = value;
		m_block = block;
	}

	// From shared pointer
	weak(shared<T> const& other) noexcept { reset(other); }
	weak& operator=(shared<T> const& other) noexcept { reset(other); return *this; }
	void reset(shared<T> const& other) noexcept { _copy_reset(other.m_value, other.m_block); }

	Tptr  get_unchecked() noexcept { return m_value; }
	operator bool() const noexcept { return m_block != nullptr; }

	// Function stuff
	shared<T> lock() const noexcept {
		shared<T> result;
		result._copy_reset(m_value, m_block);
		return result;
	}
};

// =============================================================
// == make_shared =============================================
// =============================================================

template<class T, class... Args>
shared<T> make_shared(Args&&... args) {
	shared<T> result;

	auto* block = new default_shared_block<T>(std::forward<Args>(args)...);
	result._move_reset(block->value(), block);

	return result;
}

} // namespace stx

#endif // header guard STX_SHARED_PTR_HPP_INCLUDED
