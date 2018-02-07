
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

#endif // ESL_MACROS_HPP

