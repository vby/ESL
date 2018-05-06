
#ifndef ESL_MEMORY_HPP
#define ESL_MEMORY_HPP

#include "macros.hpp"

#include <cstdlib>
#include <type_traits>
#include <memory>

#ifdef ESL_SOURCE_WIN32
#include <malloc.h>
#endif

namespace esl {

#ifdef ESL_SOURCE_WIN32

//FIXME: Need _resetstkoflw in xp, see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/alloca
ESL_ATTR_FORCEINLINE void* alloca(std::size_t n) noexcept { return _alloca(n); }

#elif defined ESL_COMPILER_GNU

ESL_ATTR_FORCEINLINE void* alloca(std::size_t n) noexcept { return ::__builtin_alloca(n); }

#endif

// c_free_delete
// A deleter that use std::free with no destructor called
struct c_free_delete {
	void operator()(void* p) const noexcept { std::free(p); }
};

// c_unique_ptr
// c_unique_ptr<T> <=> c_unique_ptr<T[]> <=> std::unique_ptr<T[], c_free_delete>
template <class T>
struct c_unique_ptr_ { using type = std::unique_ptr<T[], c_free_delete>; };
template <class T>
struct c_unique_ptr_<T[]> { using type = std::unique_ptr<T[], c_free_delete>; };
template <class T>
using c_unique_ptr = typename c_unique_ptr_<T>::type;

// c_allocator
// A allocator that use std::malloc and std::free
template <class T>
class c_allocator {
public:
	using value_type = T;
	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

	T* allocate(std::size_t n) {
		void* p = std::malloc(n * sizeof(T));
		if (!p) {
			throw std::bad_alloc{};
		}
		return static_cast<T*>(p);
	}

	void deallocate(T* p, std::size_t) noexcept { std::free(p); }
};

template <class T, class DefaultDeleter = std::default_delete<T>>
class default_deleter_shared_ptr : public std::shared_ptr<T> {
	using base_type = std::shared_ptr<T>;
public:
	using default_deleter_type = DefaultDeleter;

public:
	// ctors
	constexpr default_deleter_shared_ptr() noexcept = default;

	constexpr default_deleter_shared_ptr(std::nullptr_t) noexcept : base_type() {}

	template <class Y>
	explicit default_deleter_shared_ptr(Y* ptr) : base_type(ptr, DefaultDeleter{}) {}

	template <class Y, class Deleter>
	default_deleter_shared_ptr(Y* ptr, Deleter d) : base_type(ptr, d) {}

	template <class Deleter>
	default_deleter_shared_ptr(std::nullptr_t, Deleter d) : base_type(nullptr, d) {}

	template <class Y, class Deleter, class Alloc>
	default_deleter_shared_ptr(Y* ptr, Deleter d, Alloc alloc) : base_type(ptr, d, alloc) {}

	template <class Alloc, class Deleter>
	default_deleter_shared_ptr(std::nullptr_t, Deleter d, Alloc alloc) : base_type(nullptr, d, alloc) {}

	template <class Y>
	default_deleter_shared_ptr(const std::shared_ptr<Y>& r, element_type* ptr) noexcept : base_type(r, ptr) {}

	default_deleter_shared_ptr(const default_deleter_shared_ptr& r) noexcept = default;

	template <class Y>
	default_deleter_shared_ptr(const std::shared_ptr<Y>& r) noexcept : base_type(r) {}

	default_deleter_shared_ptr(default_deleter_shared_ptr&& r) noexcept = default;

	template <class Y>
	default_deleter_shared_ptr(std::shared_ptr<Y>&& r) noexcept : base_type(std::move(r)) {}

	template <class Y>
	explicit default_deleter_shared_ptr(const std::weak_ptr<Y>& r) : base_type(r) {}

	//template <class Y>
	//default_deleter_shared_ptr(std::auto_ptr<Y>&& r);

	template <class Y, class Deleter>
	default_deleter_shared_ptr(std::unique_ptr<Y, Deleter>&& r) : base_type(std::move(r)) {}

	// reset
	template <class Y >
	void reset(Y* ptr) {
		this->base_type::reset(ptr, Deleter{});
	}
	template <class Y, class Deleter>
	void reset(Y* ptr, Deleter d) {
		this->base_type::reset(ptr, d);
	}
	template <class Y, class Deleter, class Alloc>
	void reset(Y* ptr, Deleter d, Alloc alloc) {
		this->base_type::reset(ptr, d, alloc);
	}
};


} // namespace esl

#endif //ESL_MEMORY_HPP

