
#ifndef ESL_LAZY_HPP
#define ESL_LAZY_HPP

#include <type_traits>
#include <new>
#include <mutex>

namespace esl {

// lazy
// lazy<T> is neither copyable nor movable
// T is default constructible
template <class T>
class lazy {
public:
	using element_type = T;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

private:
	mutable std::aligned_storage<sizeof(T), alignof(T)> val_;
	mutable std::once_flag once_flag_;

	pointer construct_once_() const {
		std::call_once(once_flag_, [this]() { return new(&val_) T(); });
		return reinterpret_cast<pointer>(&val_);
	}

public:
	constexpr lazy() noexcept = default;

	lazy(const lazy&) = delete;

	~lazy() { reinterpret_cast<pointer>(&val_)->~T(); }

	lazy& operator=(const lazy&) = delete;

	pointer address() { return this->construct_once_(); }

	const_pointer address() const { return this->construct_once_(); }

	pointer operator&() { return this->address(); }

	const_pointer operator&() const { return this->address(); }

	explicit operator reference() { return *this->address(); }

	explicit operator const_reference() const { return *this->address(); }
};

} // namespace esl

#endif //ESL_LAZY_HPP

