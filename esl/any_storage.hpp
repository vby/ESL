#ifndef ESL_ANY_STORAGE_HPP
#define ESL_ANY_STORAGE_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

namespace esl {

// any_storage
// Use for any_* contianer
// Typically no direct use due to it does know which type inside

// NOTE:
// * construct: this (uninitialized) -> this (initialized)
// * from(copy): this (uninitialized), other (initialized) -> this (initialized), other (initialized)
// * from(move): this (uninitialized), other (initialized) -> this (initialized), other (uninitialized)
// * destruct: this (initialized) -> this (uninitialized)
// * swap: this (initialized), other (initialized) -> this (initialized), other (initialized)

template <std::size_t MaxSize = 4 * sizeof(void*), std::size_t MaxAlign = alignof(std::max_align_t)>
class any_storage {
private:
	union Storage {
		constexpr Storage() noexcept = default;

		Storage(const Storage&) = delete;
		Storage& operator=(const Storage&) = delete;

		void* ptr;
		std::aligned_storage_t<MaxSize, MaxAlign> obj;
	};

	template <class T>
	using InPlace = std::bool_constant<(sizeof(T) <= sizeof(Storage) && alignof(T) <= alignof(Storage) && std::is_nothrow_move_constructible_v<T>)>;

	template <class T, bool = InPlace<T>::value>
	struct Manager {
		template <class... Args>
		static T& construct(Storage& s, Args&&... args) {
			return *(new(&s) T(std::forward<Args>(args)...));
		}
		static T& from(Storage& s, const Storage& other) {
			return *(new(&s) T(reinterpret_cast<std::add_const_t<T>&>(other)));
		}
		static T& from(Storage& s, Storage&& other) noexcept {
			T& oval = reinterpret_cast<T&>(other);
			auto& val = *(new(&s) T(std::move(oval)));
			oval.~T();
			return val;
		}
		static void destruct(Storage& s) noexcept {
			reinterpret_cast<T*>(&s)->~T();
		}
		static T& get(Storage& s) noexcept {
			return reinterpret_cast<T&>(s);
		}
		static const T& get(const Storage& s) noexcept {
			return reinterpret_cast<const T&>(s);
		}
		template <class U>
		static void swap(Storage& s, Storage& other) noexcept {
			if constexpr (InPlace<U>::value) {
				std::swap(reinterpret_cast<T&>(s), reinterpret_cast<T&>(other));
			} else {
				void* ptr = other.ptr;
				from(other, std::move(s));
				s.ptr = ptr;
			}
		}
	};

	template <class T>
	struct Manager<T, false> {
		template <class... Args>
		static T& construct(Storage& s, Args&&... args) {
			T* val = new T(std::forward<Args>(args)...);
			s.ptr = val;
			return *val;
		}
		static T& from(Storage& s, const Storage& other) {
			T* val = new T(*static_cast<std::add_const_t<T>*>(other.ptr));
			s.ptr = val;
			return *val;
		}
		static T& from(Storage& s, Storage&& other) noexcept {
			s.ptr = other.ptr;
			return *static_cast<T*>(s.ptr);
		}
		static void destruct(Storage& s) noexcept {
			delete static_cast<T*>(s.ptr);
		}
		static T& get(Storage& s) noexcept {
			return *static_cast<T*>(s.ptr);
		}
		static const T& get(const Storage& s) noexcept {
			return *static_cast<const T*>(s);
		}
		template <class U>
		static void swap(Storage& s, Storage& other) noexcept {
			if constexpr (InPlace<U>::value) {
				void* ptr = s.ptr;
				Manager<U>::from(s, std::move(other));
				other.ptr = ptr;
			} else {
				std::swap(s.ptr, other.ptr);
			}
		}
	};

	Storage storage_;

public:
	constexpr any_storage() noexcept = default;

	any_storage(const any_storage&) = delete;
	any_storage& operator=(const any_storage) = delete;

	template <class T>
	any_storage(const any_storage& other, std::in_place_type_t<T>) {
		this->from<T>(other);
	}

	template <class T>
	any_storage(any_storage&& other, std::in_place_type_t<T>) noexcept {
		this->from<T>(std::move(other));
	}

	template <class T, class = std::enable_if_t<!std::is_same_v<std::decay_t<T>, any_storage>>>
	any_storage(T&& value) {
		this->construct<T>(std::forward<T>(value));
	}

	template <class T, class... Args>
	any_storage(std::in_place_type_t<T>, Args&&... args) {
		this->construct<T>(std::forward<Args>(args)...);
	}

	template <class T, class U, class... Args>
	any_storage(std::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args) {
		this->construct<T>(il, std::forward<Args>(args)...);
	}

	template <class T, class... Args>
	T& construct(Args&&... args) {
		return Manager<std::decay_t<T>>::construct(storage_, std::forward<Args>(args)...);
	}

	template <class T, class U, class... Args>
	T& construct(std::initializer_list<U> il, Args&&... args) {
		return Manager<std::decay_t<T>>::construct(storage_, il, std::forward<Args>(args)...);
	}

	template <class T>
	T& from(const any_storage& other) {
		return Manager<std::decay_t<T>>::from(storage_, other.storage_);
	}

	template <class T>
	T& from(any_storage&& other) noexcept {
		return Manager<std::decay_t<T>>::from(storage_, std::move(other.storage_));
	}

	template <class T>
	void destruct() noexcept {
		Manager<std::decay_t<T>>::destruct(storage_);
	}

	template <class T, class U>
	void swap(any_storage& other) noexcept {
		Manager<std::decay_t<T>>::template swap<std::decay_t<U>>(storage_, other.storage_);
	}

	template <class T>
	T& get() & noexcept {
		return Manager<std::decay_t<T>>::get(storage_);
	}

	template <class T>
	const T& get() const& noexcept {
		return Manager<std::decay_t<T>>::get(storage_);
	}

	template <class T>
	T&& get() && noexcept {
		return std::move(Manager<std::decay_t<T>>::get(storage_));
	}

	template <class T>
	const T&& get() const&& noexcept {
		return std::move(Manager<std::decay_t<T>>::get(storage_));
	}
};

} // namespace esl

#endif // ESL_ANY_STORAGE_HPP

