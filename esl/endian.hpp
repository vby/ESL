#ifndef ESL_ENDIAN_HPP
#define ESL_ENDIAN_HPP

#include "macros.hpp"

namespace esl {
// C++20 endian in <type_traits>
enum class endian {
    little = 1234,
    big = 4321,
#ifdef ESL_ORDER_LITTLE_ENDIAN
    native = little,
#elif defined ESL_ORDER_BIG_ENDIAN
    native = big,
#else
    native = 3421,
#endif
};

} // namespace esl

#endif // ESL_ENDIAN_HPP
