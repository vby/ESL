
#ifndef ESL_STD_EXT_HPP
#define ESL_STD_EXT_HPP

#ifdef _MSC_VER

#define ESL_FORCEINLINE __forceinline

#define ESL_NOINLINE __declspec(noinline)

#define ESL_ALLOCA(n) _alloca(n)

#else

#define ESL_FORCEINLINE inline __attribute__((always_inline))

#define ESL_NOINLINE __attribute__((noinline))

#define ESL_ALLOCA(n) __builtin_alloca(n)

#endif

#endif //ESL_STD_EXT_HPP

