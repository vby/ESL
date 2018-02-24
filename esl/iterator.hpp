#ifndef ESL_ITERATOR_HPP
#define ESL_ITERATOR_HPP

#include "type_traits.hpp"

#include <iterator>

namespace esl {

template <class Iterator>
class iterator_warpper_base {
public:
	using iterator_type = Iterator;
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using pointer = Iterator;
	using value_type = typename std::iterator_traits<Iterator>::value_type;
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
	using reference = typename std::iterator_traits<Iterator>::reference;

private:
	Iterator current_;

public:
	constexpr iterator_warpper_base() = default;

	constexpr explicit iterator_warpper_base(Iterator it): current_(it) {}

	template <class U>
	constexpr iterator_warpper_base(const iterator_warpper_base<U>& other): current_(other.current_) {}

	template <class U>
	constexpr iterator_warpper_base& operator=(const iterator_warpper_base<U>& other) {
		current_ = other.current_;
		return *this;
	}

	constexpr Iterator base() const noexcept { return current_; }

	constexpr reference operator*() const { return *current_; }

	constexpr pointer operator->() const { return current_; }

	constexpr reference operator[](difference_type n) const { return current_[n]; }

	constexpr iterator_warpper_base& operator++() {
		++current_;
		return *this;
	}

	constexpr iterator_warpper_base& operator--() {
		--current_;
		return *this;
	}

	constexpr iterator_warpper_base operator++(int) {
		auto it(*this);
		++current_;
		return it;
	}

	constexpr iterator_warpper_base operator--(int) {
		auto it(*this);
		--current_;
		return it;
	}

	constexpr iterator_warpper_base operator+(difference_type n) const {
		return iterator_warpper_base(current_ + n);
	}

	constexpr iterator_warpper_base operator-(difference_type n) const {
		return iterator_warpper_base(current_ - n);
	}

	constexpr iterator_warpper_base& operator+=(difference_type n) {
		current_ += n;
		return *this;
	}

	constexpr iterator_warpper_base& operator-=(difference_type n) {
		current_ -= n;
		return *this;
	}
};

template <class Iterator1, class Iterator2>
constexpr bool operator==(const iterator_warpper_base<Iterator1>& lhs, const iterator_warpper_base<Iterator2>& rhs) {
	return lhs.base() == rhs.base();
}
template <class Iterator1, class Iterator2>
constexpr bool operator!=(const iterator_warpper_base<Iterator1>& lhs, const iterator_warpper_base<Iterator2>& rhs) {
	return lhs.base() != rhs.base();
}
template <class Iterator1, class Iterator2>
constexpr bool operator<(const iterator_warpper_base<Iterator1>& lhs, const iterator_warpper_base<Iterator2>& rhs) {
	return lhs.base() < rhs.base();
}
template <class Iterator1, class Iterator2>
constexpr bool operator<=(const iterator_warpper_base<Iterator1>& lhs, const iterator_warpper_base<Iterator2>& rhs) {
	return lhs.base() <= rhs.base();
}
template <class Iterator1, class Iterator2>
constexpr bool operator>(const iterator_warpper_base<Iterator1>& lhs, const iterator_warpper_base<Iterator2>& rhs) {
	return lhs.base() > rhs.base();
}
template <class Iterator1, class Iterator2>
constexpr bool operator>=(const iterator_warpper_base<Iterator1>& lhs, const iterator_warpper_base<Iterator2>& rhs) {
	return lhs.base() >= rhs.base();
}

template <class Iterator>
class cast_iterator: public Iterator {
public:
	explicit cast_iterator(Iterator it): Iterator(it) {}

	template <class T>
	cast_iterator& operator=(T&& value) {
		if constexpr (std::is_assignable_v<Iterator, T&&>) {
			this->Iterator::operator=(std::forward<T>(value));
		} else {
			this->Iterator::operator=(std::move(static_cast<typename Iterator::container_type::value_type>(std::forward<T>(value))));
		}
		return *this;
	}

	cast_iterator& operator*() noexcept { return *this; }

	constexpr cast_iterator& operator++() noexcept { return *this; }

	constexpr cast_iterator& operator++(int) noexcept { return *this; }
};

template <class Iterator>
inline constexpr cast_iterator<Iterator> make_cast_iterator(Iterator it) {
	return cast_iterator<Iterator>(it);
}

} // namespace esl

#endif //ESL_ITERATOR_HPP

