
#ifndef ESL_ALLOCA_HPP
#define ESL_ALLOCA_HPP

#include "macros.hpp"

#ifdef ESL_SOURCE_WIN32
	#include <malloc.h>
	#define esl_alloca(n) _alloca(n) //FIXME: Need _resetstkoflw in xp, see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/alloca
#elif defined ESL_COMPILER_GNU
	#define esl_alloca(n) __builtin_alloca(n)
#endif

#endif // ESL_ALLOCA_HPP

