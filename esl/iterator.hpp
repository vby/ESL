#ifndef ESL_ITERATOR_HPP
#define ESL_ITERATOR_HPP

#include "type_traits.hpp"

#include <iterator>

namespace esl {

template <class DIter, class UIter>
class noop_output_iterator: public UIter {
public:
	explicit noop_output_iterator(UIter it): UIter(it) {}

	DIter& operator*() noexcept { return static_cast<DIter&>(*this); }

	constexpr DIter& operator++() noexcept { return static_cast<DIter&>(*this); }

	constexpr DIter& operator++(int) noexcept { return static_cast<DIter&>(*this); }
};

template <class UIter>
class cast_iterator: public noop_output_iterator<cast_iterator<UIter>, UIter> {
public:
	using noop_output_iterator<cast_iterator<UIter>, UIter>::noop_output_iterator;

	template <class T>
	cast_iterator& operator=(T&& value) {
		if constexpr (std::is_assignable_v<UIter, T&&>) {
			this->UIter::operator=(std::forward<T>(value));
		} else {
			this->UIter::operator=(std::move(static_cast<typename UIter::container_type::value_type>(std::forward<T>(value))));
		}
		return *this;
	}
};

template <class Iterator>
inline constexpr cast_iterator<Iterator> make_cast_iterator(Iterator it) {
	return cast_iterator<Iterator>(it);
}

} // namespace esl

#endif //ESL_ITERATOR_HPP

