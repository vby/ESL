
#ifndef ESL_MACROS_HPP
#define ESL_MACROS_HPP

// ESL_CXX_*, ESL_CXX_VERSION, ESL_CXX_VERSION_MAJOR, ESL_CXX_VERSION_MINOR
#define ESL_CXX_98 199711L
#define ESL_CXX_11 201103L
#define ESL_CXX_14 201402L
#define ESL_CXX_17 201703L
#define ESL_CXX_VERSION __cplusplus
#define ESL_CXX_VERSION_MAJOR (__cplusplus / 100)
#define ESL_CXX_VERSION_MINOR (__cplusplus % 100)

// ESL_COMPILER_*, ESL_COMPILER_VERSION, ESL_COMPILER_VERSION_MAJOR, ESL_COMPILER_VERSION_MINOR, ESL_COMPILER_VERSION_PATCHLEVEL
#if defined _MSC_VER
	#define ESL_COMPILER_MSVC
	#define ESL_COMPILER_VERSION _MSC_VER
	#define ESL_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
	#define ESL_COMPILER_VERSION_MINOR (_MSC_VER % 100)
	#define ESL_COMPILER_VERSION_PATCHLEVEL (_MSC_FULL_VER % 100000)
#elif defined __clang__
	#define ESL_COMPILER_CLANG
	#ifdef __GNUC__
		#define ESL_COMPILER_GNU
	#endif
	#define ESL_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
	#define ESL_COMPILER_VERSION_MAJOR __clang_major__
	#define ESL_COMPILER_VERSION_MINOR __clang_minor__
	#define ESL_COMPILER_VERSION_PATCHLEVEL __clang_patchlevel__
#elif defined __GNUC__
	#define ESL_COMPILER_GNU
	#define ESL_COMPILER_GNU_NOT_CLANG
	#define ESL_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
	#define ESL_COMPILER_VERSION_MAJOR __GNUC__
	#define ESL_COMPILER_VERSION_MINOR __GNUC_MINOR__
	#define ESL_COMPILER_VERSION_PATCHLEVEL __GNUC_PATCHLEVEL__
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
#ifdef __APPLE__
	#define ESL_SOURCE_APPLE
#endif

// ESL_COUNTER, ESL_PRETTY_FUNCTION
#ifdef ESL_COMPILER_MSVC
	#define ESL_COUNTER __COUNTER__
	#define ESL_PRETTY_FUNCTION __FUNCSIG__
#else
	#ifdef __COUNTER__
		#define ESL_COUNTER __COUNTER__
	#endif
	#ifdef __PRETTY_FUNCTION__
		#define ESL_PRETTY_FUNCTION __PRETTY_FUNCTION__
	#endif
#endif

// ESL_ARCH_*
#ifdef ESL_COMPILER_MSVC
	#if defined _M_X64
		#define ESL_ARCH_X64
	#elif defined _M_IX86
		#define ESL_ARCH_X86
	#elif defined _M_ARM64
		#define ESL_ARCH_ARM64
	#elif defined _M_ARM
		#define ESL_ARCH_ARM
	#endif
	#ifdef _M_IX86_FP
		#if _M_IX86_FP == 1
			#define ESL_ARCH_SSE
		#elif _M_IX86_FP == 2
			#define ESL_ARCH_SSE2
		#endif
	#endif
#else
	#if defined __x86_64__
		#define ESL_ARCH_X64
	#elif defined __i386__
		#define ESL_ARCH_X86
	#elif defined __aarch64__
		#define ESL_ARCH_ARM64
	#elif defined __ARMEL__
		#define ESL_ARCH_ARM
	#endif
	#ifdef __SSE__
		#define ESL_ARCH_SSE
	#elif defined __SSE2__
		#define ESL_ARCH_SSE2
	#endif
#endif
#ifdef __AVX__
	#define ESL_ARCH_AVX
#endif
#ifdef __AVX2__
	#define ESL_ARCH_AVX2
#endif

// ESL_STRINGIFY
#define ESL_QUOTE(name) #name
#define ESL_STRINGIFY(name) ESL_QUOTE(name)

#ifdef ESL_COMPILER_MSVC
	#define ESL_WARNING_PUSH() __pragma(warning(push))
	#define ESL_WARNING_POP() __pragma(warning(pop))
	#define ESL_WARNING_DISABLE(number) __pragma(warning(disable: number))
	#define ESL_WARNING_ERROR(number) __pragma(warning(error: number))
	#define ESL_WARNING_WARNING(number) __pragma(warning(default: number))
#elif defined ESL_COMPILER_GNU
	#define ESL_WARNING_PUSH() _Pragma("GCC diagnostic push")
	#define ESL_WARNING_POP() _Pragma("GCC diagnostic pop")
	#define ESL_WARNING_GNU_(level, name) _Pragma(ESL_QUOTE(GCC diagnostic level name))
	#define ESL_WARNING_DISABLE(name) ESL_WARNING_GNU_(ignored, ESL_QUOTE(-W##name))
	#define ESL_WARNING_ERROR(name) ESL_WARNING_GNU_(error, ESL_QUOTE(-W##name))
	#define ESL_WARNING_WARNING(name) ESL_WARNING_GNU_(warning, ESL_QUOTE(-W##name))
#else
	#define ESL_WARNING_PUSH()
	#define ESL_WARNING_POP()
	#define ESL_WARNING_DISABLE(x)
	#define ESL_WARNING_ERROR(x)
	#define ESL_WARNING_WARNING(x)
#endif

#include <cstdint>

// ESL_SIZE_BITS, ESL_SIZE_*
#if (SIZE_MAX == UINT64_MAX)
	#define ESL_SIZE_BITS 64
	#define ESL_SIZE_64
#elif (SIZE_MAX == UINT32_MAX)
	#define ESL_SIZE_BITS 32
	#define ESL_SIZE_32
#endif

// ESL_ROT*
#define ESL_ROTL32(x, s) (((x) << (s)) | ((x) >> (32 - (s))))
#define ESL_ROTL64(x, s) (((x) << (s)) | ((x) >> (64 - (s))))
#define ESL_ROTR32(x, s) (((x) >> (s)) | ((x) << (32 - (s))))
#define ESL_ROTR64(x, s) (((x) >> (s)) | ((x) << (64 - (s))))

// ESL_FAST_ROT*
#ifdef ESL_COMPILER_MSVC
	#include <cstdlib>
	#define ESL_FAST_ROTL32(x, s) _rotl(x, s)
	#define ESL_FAST_ROTL64(x, s) _rotl64(x, s)
	#define ESL_FAST_ROTR32(x, s) _rotr(x, s)
	#define ESL_FAST_ROTR64(x, s) _rotr64(x, s)
#endif

#endif // ESL_MACROS_HPP

