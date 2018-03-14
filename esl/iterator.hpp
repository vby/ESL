#ifndef ESL_ITERATOR_HPP
#define ESL_ITERATOR_HPP

#include "utils.hpp"

#include <type_traits>
#include <iterator>
#include <memory>

namespace esl {

template <class DIter>
class noop_output_iterator {
public:
	using value_type = void;
	using difference_type = void;
	using pointer = void;
	using reference = void;
	using iterator_category = std::output_iterator_tag;

public:
	DIter& operator*() noexcept { return static_cast<DIter&>(*this); }

	constexpr DIter& operator++() noexcept { return static_cast<DIter&>(*this); }

	constexpr DIter& operator++(int) noexcept { return static_cast<DIter&>(*this); }
};

// cast_iterator
template <class Iterator>
class cast_iterator: public noop_output_iterator<cast_iterator<Iterator>> {
private:
	Iterator current_;

public:
	explicit cast_iterator(Iterator it): current_(it) {}

	cast_iterator& operator=(const cast_iterator&) noexcept = default;

	template <class T>
	constexpr cast_iterator& operator=(T&& value) {
		if constexpr (std::is_assignable_v<Iterator, T&&>) {
			current_ = std::forward<T>(value);
		} else {
			current_ = typename Iterator::container_type::value_type(std::forward<T>(value));
		}
		return *this;
	}
};

template <class Iterator>
inline constexpr cast_iterator<Iterator> make_cast_iterator(Iterator it) {
	return cast_iterator<Iterator>(it);
}

// add_iterator
template <class Container>
class add_iterator: public noop_output_iterator<add_iterator<Container>> {
private:
	Container* c_;

public:
	explicit add_iterator(Container& c): c_(std::addressof(c)) {}

	add_iterator& operator=(const add_iterator&) noexcept = default;

	template <class T>
	add_iterator& operator=(T&& value) {
		add_emplace(*c_, std::forward<T>(value));
		return *this;
	}
};

template <class Container>
inline constexpr add_iterator<Container> make_add_iterator(Container& c) {
	return add_iterator<Container>(c);
}

template <class Container>
inline constexpr cast_iterator<add_iterator<Container>> make_cast_add_iterator(Container& c) {
	return make_cast_iterator(make_add_iterator(c));
}

} // namespace esl

#endif //ESL_ITERATOR_HPP

