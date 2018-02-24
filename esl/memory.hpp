
#ifndef ESL_MEMORY_HPP
#define ESL_MEMORY_HPP

#include "macros.hpp"

#include <cstdlib>
#include <type_traits>
#include <memory>

namespace esl {

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

} // namespace esl

#endif //ESL_MEMORY_HPP

