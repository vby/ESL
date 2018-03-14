
#ifndef ESL_DEMANGLE_HPP
#define ESL_DEMANGLE_HPP

#if __has_include(<cxxabi.h>)
#define ESL_HAS_DEMANGLE

#include "string.hpp"

#include <cxxabi.h>
#include <typeinfo>

namespace esl {

// demangle
// Exceptions: std::bad_alloc
inline c_unique_chars demangle(const char* abi_name) {
	int status;
	char* name = abi::__cxa_demangle(abi_name, nullptr, nullptr, &status);
	if (status == 0) {
		return c_unique_chars(name);
	} else if (status == -1) {
		throw std::bad_alloc{};
	} else {
		if (name) {
			std::free(name);
		}
	}
	return c_unique_chars{};
}

} // namespace esl

#endif

namespace esl {

inline std::string type_name(const std::type_info& type_info) {
#ifdef ESL_HAS_DEMANGLE
	return demangle(type_info.name());
#else
	return type_info.name();
#endif
}

} // namespace esl

#endif //ESL_DEMANGLE_HPP

