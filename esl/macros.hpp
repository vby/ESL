
#ifndef ESL_MACROS_HPP
#define ESL_MACROS_HPP

// ESL_CXX_STD, ESL_CXX_STD_*
#if __cplusplus >= 199711L
	#define ESL_CXX_STD_98
	#define ESL_CXX_STD 98
#endif
#if __cplusplus >= 201103L
	#define ESL_CXX_STD_11
	#undef ESL_CXX_STD
	#define ESL_CXX_STD 11
#endif
#if __cplusplus >= 201402L
	#define ESL_CXX_STD_14
	#undef ESL_CXX_STD
	#define ESL_CXX_STD 14
#endif
#if __cplusplus >= 201703L
	#define ESL_CXX_STD_17
	#undef ESL_CXX_STD
	#define ESL_CXX_STD 17
#endif

// ESL_COMPILER_*, ESL_COMPILER_VERSION_MAJOR, ESL_COMPILER_VERSION_MINOR
#if defined __clang__
	#define ESL_COMPILER_CLANG
	#if defined __GNUC__
		#define ESL_COMPILER_GNU
	#endif
	#define ESL_COMPILER_VERSION_MAJOR __clang_major__
	#define ESL_COMPILER_VERSION_MINOR __clang_minor__
#elif defined __GNUC__
	#define ESL_COMPILER_GNU
	#define ESL_COMPILER_VERSION_MAJOR __GNUC__
	#define ESL_COMPILER_VERSION_MINOR __GNUC_MINOR__
#elif defined _MSC_VER
	#define ESL_COMPILER_MSVC
	#define ESL_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
	#define ESL_COMPILER_VERSION_MINOR (_MSC_VER % 100)
#endif

// ESL_SOURCE_*
#ifdef _WIN32
	#define ESL_SOURCE_WIN32
	#ifdef _WIN64
		#define ESL_SOURCE_WIN64
	#else
		#define ESL_SOURCE_WIN32_NOT_WIN64
	#endif
#endif
#ifdef _POSIX_SOURCE
	#define ESL_SOURCE_POSIX
#endif
#ifdef _GNU_SOURCE
	#define ESL_SOURCE_GNU
#endif
#ifdef _BSD_SOURCE
	#define ESL_SOURCE_BSD
#endif

// Compiler extensions
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
#ifdef ESL_API_EXPORT
	#define ESL_API ESL_EXPORT
#else
	#define ESL_API ESL_IMPORT
#endif

#define ESL_QUOTE(name) #name
#define ESL_STRING(name) ESL_QUOTE(name)

#endif // ESL_MACROS_HPP

