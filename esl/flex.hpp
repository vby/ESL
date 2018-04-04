#ifndef ESL_FLEX_HPP
#define ESL_FLEX_HPP

#include "flex_storage.hpp"

namespace esl {

template <class T>
class flex {
public:
	using value_type = T;

private:
	using Storage = flex_storage<>;

	Storage storage_;

public:
	// constructors
	constexpr flex(): storage_(std::in_place_type<T>) {}

	template <class... Args, class = std::enable_if_t<!is_types_decay_to_v<flex, Args...>>>
	flex(Args&&... args): storage_(std::in_place_type<T>, std::forward<Args>(args)...) {}

	flex(const flex& other): storage_(other.storage_, std::in_place_type<T>) {}

	flex(flex&& other): storage_(std::move(other.storage_), std::in_place_type<T>) {}

	// operator=
	template <class Value, class = std::enable_if_t<!is_decay_to_v<Value, flex_storage>>>
	flex& assign(Value&& value) {
		storage_.assign<T>(std::forward<Value>(value));
		return *this;
	}

	flex& operator=(const flex& other) {
		storage_.assign<T>(other.storage_);
		return *this;
	}

	flex& operator=(flex&& other) {
		storage_.construct<T>(std::move(other.storage_));
		other.storage_.template construct<T>();
		return *this;
	}

	~flex() {
		storage_.destruct<T>();
	}

	void swap(flex& other) noexcept {
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

#endif // ESL_FLEX_HPP

