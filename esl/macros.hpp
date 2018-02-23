
#ifndef ESL_MACROS_HPP
#define ESL_MACROS_HPP

// ESL_CXX, ESL_CXX_*, ESL_CXX_MAJOR, ESL_CXX_MINOR
#define ESL_CXX __cplusplus
#define ESL_CXX_98 199711L
#define ESL_CXX_11 201103L
#define ESL_CXX_14 201402L
#define ESL_CXX_17 201703L
#define ESL_CXX_MAJOR (__cplusplus / 100)
#define ESL_CXX_MINOR (__cplusplus % 100)

// ESL_COMPILER_*, ESL_COMPILER, ESL_COMPILER_MAJOR, ESL_COMPILER_MINOR, ESL_COMPILER_PATCHLEVEL
#if defined _MSC_VER
	#define ESL_COMPILER_MSVC
	#define ESL_COMPILER _MSC_VER
	#define ESL_COMPILER_MAJOR (_MSC_VER / 100)
	#define ESL_COMPILER_MINOR (_MSC_VER % 100)
	#define ESL_COMPILER_PATCHLEVEL 0
#elif defined __clang__
	#define ESL_COMPILER_CLANG
	#ifdef __GNUC__
		#define ESL_COMPILER_GNU
	#endif
	#define ESL_COMPILER (__clang_major__ * 100 + __clang_minor__)
	#define ESL_COMPILER_MAJOR __clang_major__
	#define ESL_COMPILER_MINOR __clang_minor__
	#define ESL_COMPILER_PATCHLEVEL __clang_patchlevel__
#elif defined __GNUC__
	#define ESL_COMPILER_GNU
	#define ESL_COMPILER (__GNUC__ * 100 + __GNUC_MINOR__)
	#define ESL_COMPILER_MAJOR __GNUC__
	#define ESL_COMPILER_MINOR __GNUC_MINOR__
	#define ESL_COMPILER_PATCHLEVEL __GNUC_PATCHLEVEL__
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

#define ESL_QUOTE(name) #name
#define ESL_STRINGIFY(name) ESL_QUOTE(name)

#endif // ESL_MACROS_HPP

