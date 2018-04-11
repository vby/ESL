
#ifndef ESL_DEMANGLE_HPP
#define ESL_DEMANGLE_HPP

#if __has_include(<cxxabi.h>)
	#define ESL_HAS_DEMANGLE
	#include <cxxabi.h>
	#include <typeinfo>
#endif

#include "memory.hpp"

#include <exception>

namespace esl {

#ifdef ESL_HAS_DEMANGLE

// demangle
// Exceptions: std::bad_alloc
inline c_unique_ptr<char> demangle(const char* mangled_name) {
	int status;
	c_unique_ptr<char> name(abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status));
	if (status == 0) {
		return name;
	} else if (status == -1) {
		throw std::bad_alloc{};
	} else {
		name.reset();
	}
	return name;
}

#endif

} // namespace esl


#endif //ESL_DEMANGLE_HPP

