#ifndef ESL_ANY_TYPE_HPP
#define ESL_ANY_TYPE_HPP

#include "any_storage.hpp"

namespace esl {

template <class T>
class any_type {
public:
	using value_type = T;

private:
	using Storage = any_storage<>;

	Storage storage_;

public:
	// constructors
	constexpr any_type(): storage_(std::in_place_type<T>) {}

	template <class... Args, class = std::enable_if_t<!is_types_decay_to_v<any_type, Args...>>>
	any_type(Args&&... args): storage_(std::in_place_type<T>, std::forward<Args>(args)...) {}

	any_type(const any_type& other): storage_(other.storage_, std::in_place_type<T>) {}

	any_type(any_type&& other): storage_(std::move(other.storage_), std::in_place_type<T>) {}

	// operator=
	template <class Value, class = std::enable_if_t<!is_decay_to_v<Value, any_storage>>>
	any_type& assign(Value&& value) {
		storage_.assign<T>(std::forward<Value>(value));
		return *this;
	}

	any_type& operator=(const any_type& other) {
		storage_.assign<T>(other.storage_);
		return *this;
	}

	any_type& operator=(any_type&& other) {
		storage_.construct<T>(std::move(other.storage_));
		other.storage_.template construct<T>();
		return *this;
	}

	~any_type() {
		storage_.destruct<T>();
	}

	void swap(any_type& other) noexcept {
		storage_.swap<T>(other.storage_);
	}

	T& value() noexcept {
		return storage_.get<T>();
	}

	const T& value() const noexcept {
		return storage_.get<T>();
	}
};

} // namespace esl

#endif // ESL_ANY_TYPE_HPP

