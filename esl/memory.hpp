
#ifndef ESL_MEMORY_HPP
#define ESL_MEMORY_HPP

#include <memory>
#include <cstdlib> 

namespace esl {

struct c_default_delete {
	void operator()(void* p) const noexcept { std::free(p); }
};

template <class T>
struct c_unique_ptr_ { using type = std::unique_ptr<T[], c_default_delete>; };
template <class T>
struct c_unique_ptr_<T[]> { using type = std::unique_ptr<T[], c_default_delete>; };
template <class T>
using c_unique_ptr = typename c_unique_ptr_<T>::type; 

template <class T>
class c_allocator {
public:
	//TODO	
};

} // namespace esl

#endif //ESL_MEMORY_HPP

