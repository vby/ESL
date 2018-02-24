
#ifndef ESL_TYPEINFO_HPP
#define ESL_TYPEINFO_HPP

#if __has_include(<cxxabi.h>)
#define ESL_HAS_DEMANGLE

#include "string.hpp"
#include <cxxabi.h>
#include <typeinfo>

namespace esl {

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

inline const std::string& typeid_name(const std::type_info& type_info) {
#ifdef ESL_HAS_DEMANGLE
	static std::string name_ = demangle(type_info.name());
#else
	static std::string name_ = type_info.name();
#endif
	return name_;
}

template <class T>
inline const std::string& typeid_name(T&& o) {
	return typeid_name(typeid(o));
}

template <class T>
inline const std::string& typeid_name() {
#ifdef ESL_HAS_DEMANGLE
	static std::string name_ = demangle(typeid(T).name());
#else
	static std::string name_ = typeid(T).name();
#endif
	return name_;
}

} // namespace esl

#endif //ESL_TYPEINFO_HPP

