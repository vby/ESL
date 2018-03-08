
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
	using element_type = T;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

private:
	mutable std::aligned_storage_t<sizeof(T), alignof(T)> val_;
	mutable std::tuple<std::decay_t<Args>...> args_;
	mutable std::once_flag once_flag_;

	template <std::size_t... Is>
	void construct_(std::index_sequence<Is...>) const {
		std::call_once(once_flag_, [this]() { return new(static_cast<void*>(&val_)) T(std::forward<Args>(std::get<Is>(args_))...); });
	}

	pointer construct_() const {
		this->construct_(std::index_sequence_for<Args...>{});
		return reinterpret_cast<pointer>(&val_);
	}

public:
	template <class... FArgs>
	constexpr lazy(FArgs&&... args): args_(std::forward<FArgs>(args)...) {}

	lazy(const lazy&) = delete;

	lazy(lazy&&) = delete;

	~lazy() { reinterpret_cast<pointer>(&val_)->~T(); }

	lazy& operator=(const lazy&) = delete;

	lazy& operator=(lazy&&) = delete;

	pointer address() { return this->construct_(); }

	const_pointer address() const { return this->construct_(); }

	pointer operator&() { return this->address(); }

	const_pointer operator&() const { return this->address(); }

	explicit operator reference() { return *this->address(); }

	explicit operator const_reference() const { return *this->address(); }
};

} // namespace esl

#endif //ESL_LAZY_HPP

