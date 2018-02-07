
#ifndef ESL_MACROS_HPP
#define ESL_MACROS_HPP

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

#if defined(__clang__)
	#define ESL_COMPILER_CLANG
	#if defined(__GNUC__)
		#define ESL_COMPILER_GNU
	#endif
	#define ESL_COMPILER_VERSION_MAJOR __clang_major__
	#define ESL_COMPILER_VERSION_MINOR __clang_minor__
#elif defined(__GNUC__)
	#define ESL_COMPILER_GNU
	#define ESL_COMPILER_VERSION_MAJOR __GNUC__
	#define ESL_COMPILER_VERSION_MINOR __GNUC_MINOR__
#elif defined(_MSC_VER)
	#define ESL_COMPILER_MSVC
	#define ESL_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
	#define ESL_COMPILER_VERSION_MINOR (_MSC_VER % 100)
#endif

#ifdef ESL_COMPILER_MSVC
	#define ESL_FORCEINLINE __forceinline
	#define ESL_NOINLINE __declspec(noinline)
#else
	#define ESL_FORCEINLINE inline __attribute__((always_inline))
	#define ESL_NOINLINE __attribute__((noinline))
#endif

#endif // ESL_MACROS_HPP

