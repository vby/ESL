
#ifndef ESL_LAZY_HPP
#define ESL_LAZY_HPP

#include <type_traits>
#include <new>
#include <mutex>
#include <utility>

namespace esl {

// lazy
// lazy<T> is neither copyable nor movable
template <class T, class... Args>
class lazy {
public:
	using value_type = T;

private:
	mutable std::aligned_storage_t<sizeof(T), alignof(T)> val_;
	mutable std::tuple<std::decay_t<Args>...> args_;
	mutable std::once_flag once_flag_;

	template <std::size_t... Is>
	void construct(std::index_sequence<Is...>) const {
		std::call_once(once_flag_, [this]() { new(&val_) T(std::forward<Args>(std::get<Is>(args_))...); });
	}

	T& construct() const {
		this->construct(std::index_sequence_for<Args...>{});
		return reinterpret_cast<T&>(val_);
	}

public:
	template <class... FArgs>
	explicit lazy(FArgs&&... args): args_(std::forward<FArgs>(args)...) {}

	lazy(const lazy&) = delete;
	lazy(lazy&&) = delete;
	lazy& operator=(const lazy&) = delete;
	lazy& operator=(lazy&&) = delete;

	~lazy() { reinterpret_cast<T*>(&val_)->~T(); }

	// value
	T& value() & { return this->construct(); }

	const T& value() const& { return this->construct(); }

	T&& value() && { return std::move(this->construct()); }

	const T&& value() const&& { return std::move(this->construct()); }

	// operator->
	T* operator->() { return &this->value(); }

	const T* operator->() const { return &this->value(); }

	// operator*
	T& operator*() & { return this->value(); }

	const T& operator*() const& { return this->value(); }

	T&& operator*() && { return std::move(this->value()); }

	const T&& operator*() const&& { return std::move(this->value()); }
};

} // namespace esl

#endif //ESL_LAZY_HPP

