#ifndef ESL_LIMITS_HPP
#define ESL_LIMITS_HPP

#include "macros.hpp"

#include <climits>
#include <cstdint>

// NOTE: Data model: LP32, ILP32, LLP64, LP64

// ESL_SHRT_WIDTH, ESL_USHRT_WIDTH (16)
#define ESL_SHRT_WIDTH 16
#define ESL_USHRT_WIDTH 16

// ESL_INT_WIDTH, ESL_UINT_WIDTH (16, 32)
#if (UINT_MAX == UINT32_MAX)
	#define ESL_INT_WIDTH 32
	#define ESL_UINT_WIDTH 32
#else
	#define ESL_INT_WIDTH 16
	#define ESL_UINT_WIDTH 16
#endif

// ESL_LONG_WIDTH, ESL_ULONG_WIDTH (32, 64)
#if (ULONG_MAX == UINT64_MAX)
	#define ESL_LONG_WIDTH 64
	#define ESL_ULONG_WIDTH 64
#else
	#define ESL_LONG_WIDTH 32
	#define ESL_ULONG_WIDTH 32
#endif

// ESL_LLONG_WIDTH, ESL_ULLONG_WIDTH (64)
#define ESL_LLONG_WIDTH 64
#define ESL_ULLONG_WIDTH 64

// ESL_SIZE_WIDTH (32, 64)
#if (SIZE_MAX == UINT64_MAX)
	#define ESL_SIZE_WIDTH 64
#else
	#define ESL_SIZE_WIDTH 32
#endif

// ESL_INTPTR_WIDTH, ESL_UINTPTR_WIDTH (32, 64)
#if (UINTPTR_MAX == UINT64_MAX)
	#define ESL_INTPTR_WIDTH 64
	#define ESL_UINTPTR_WIDTH 64
#else
	#define ESL_INTPTR_WIDTH 32
	#define ESL_UINTPTR_WIDTH 32
#endif

// ESL_PTRDIFF_WIDTH (32, 64)
#if (PTRDIFF_MAX == INT64_MAX)
	#define ESL_PTRDIFF_WIDTH 64
#else
	#define ESL_PTRDIFF_WIDTH 32
#endif

// ESL_INTMAX_WIDTH (64)
#define ESL_INTMAX_WIDTH 64
#define ESL_UINTMAX_WIDTH 64

// ESL_LP32, ESL_ILP32, ESL_LLP64, ESL_LP64
#if (ESL_INTPTR_WIDTH == 32)
	#if (ESL_INT_WIDTH == 32)
		#define ESL_ILP32
	#else
		#define ESL_LP32
	#endif
#else
	#if (ESL_LONG_WIDTH == 64)
		#define ESL_LP64
	#else
		#define ESL_LLP64
	#endif
#endif

#endif // ESL_LIMITS_HPP

