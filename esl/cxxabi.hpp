
#ifndef ESL_CXXABI_HPP
#define ESL_CXXABI_HPP

#if __has_include(<cxxabi.h>)
#define ESL_HAS_DEMANGLE

#include "string.hpp"
#include <cxxabi.h>
#include <typeinfo>

namespace esl {

// Exceptions: std::bad_alloc
inline c_unique_string demangle(const char* abi_name) {
	int status;
	char* name = abi::__cxa_demangle(abi_name, nullptr, nullptr, &status);
	if (status == 0) {
		return c_unique_string(name);
	} else if (status == -1) {
		throw std::bad_alloc{};
	} else {
		if (name) {
			std::free(name);
		}
	}
	return c_unique_string{};
}

} // namespace esl

#endif 

namespace esl {

template <class T>
inline const std::string& object_name(T&& o) { 
#ifdef ESL_HAS_DEMANGLE
	static std::string name_ = demangle(typeid(o).name()); 
#else
	static std::string name_ = typeid(o).name(); 
#endif // ESL_HAS_DEMANGLE
	return name_; 
} 

template <class T>
inline const std::string& type_name() { 
#ifdef ESL_HAS_DEMANGLE
	static std::string name_ = demangle(typeid(T).name()); 
#else
	static std::string name_ = typeid(T).name(); 
#endif // ESL_HAS_DEMANGLE
	return name_; 
} 

} // namespace esl

#endif //ESL_CXXABI_HPP

