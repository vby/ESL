
#ifndef ESL_DECLSPEC_HPP
#define ESL_DECLSPEC_HPP

#include "macros.hpp"

#ifdef ESL_COMPILER_MSVC
	#define ESL_FORCEINLINE __forceinline
	#define ESL_NOINLINE __declspec(noinline)
	#define ESL_EXPORT __declspec(dllexport)
	#define ESL_IMPORT __declspec(dllimport)
#elif defined ESL_COMPILER_GNU
	#define ESL_FORCEINLINE inline __attribute__((always_inline))
	#define ESL_NOINLINE __attribute__((noinline))
	#define ESL_EXPORT __attribute__((visibility("default")))
	#define ESL_IMPORT __attribute__((visibility("default")))
#else
	#define ESL_FORCEINLINE
	#define ESL_NOINLINE
	#define ESL_EXPORT
	#define ESL_IMPORT
#endif

#endif // ESL_DECLSPEC_HPP

