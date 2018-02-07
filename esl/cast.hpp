
#ifndef ESL_CAST_HPP
#define ESL_CAST_HPP

#include <type_traits>
#include <utility>
#include <cstring>

namespace esl {

inline namespace cast {

template <class Dest, class Source>
inline constexpr Dest implicit_cast(Source&& f) {
	return std::forward<Source>(f);
}

template <class Dest, class Source>
inline constexpr Dest narrow_cast(Source&& f) {
	return static_cast<T>(std::forward<Source>(f));
}

template <class Dest, class Source>
inline constexpr Dest bit_cast(Source&& f) {
	static_assert(std::is_pod_v<Source>, "bit_cast source isn't POD");
	static_assert(std::is_pod_v<Dest>, "bit_cast destination isn't POD");
	static_assert(sizeof(Source) == sizeof(Dest), "bit_cast type has difference size");
	Dest dest;
	std::memcpy(&dest, &f, sizeof(Dest));
	return dest;
}

} // inline namespace cast

} // namespace esl

#endif //ESL_CAST_HPP

