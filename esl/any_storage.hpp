#ifndef ESL_ANY_STORAGE_HPP
#define ESL_ANY_STORAGE_HPP

#include "type_traits.hpp"

namespace esl {

// any_storage
// Use for any_* contianer
// Typically no direct use due to it does know which type inside

// NOTE:
// * construct: this (unconstructed) -> this (constructed)
// * construct(copy): this (unconstructed), other (constructed) -> this (constructed), other (constructed)
// * construct(move): this (unconstructed), other (constructed) -> this (constructed), other (unconstructed)
// * assign: this (constructed) -> this (constructed)
// * assign_copy_move(copy): this (constructed), other (constructed) -> this (constructed), other (constructed)
// * assign_copy_move(move): this (constructed), other (constructed) -> this (constructed), other (unconstructed)
// * destruct: this (constructed) -> this (unconstructed)
// * swap: this (constructed), other (constructed) -> this (constructed), other (constructed)

template <std::size_t MaxSize = 4 * sizeof(void*), std::size_t MaxAlign = alignof(void*)>
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
	using InPlace = std::bool_constant<(sizeof(T) <= sizeof(Storage) && alignof(T) <= alignof(Storage)
			&& std::is_nothrow_move_constructible_v<T>) && std::is_nothrow_move_assignable_v<T>>;

	template <class T, bool = InPlace<T>::value>
	struct Manager {
		// construct
		template <class... Args, class = std::enable_if_t<!is_types_decay_to_v<Storage, Args...>>>
		static T& construct(Storage& s, Args&&... args) {
			return *(new(&s) T(std::forward<Args>(args)...));
		}

		static T& construct(Storage& s, const Storage& other) {
			return *(new(&s) T(reinterpret_cast<std::add_const_t<T>&>(other)));
		}

		static T& construct(Storage& s, Storage&& other) noexcept {
			T& oval = reinterpret_cast<T&>(other);
			auto& val = *(new(&s) T(std::move(oval)));
			oval.~T();
			return val;
		}

		// destruct
		static void destruct(Storage& s) noexcept {
			reinterpret_cast<T*>(&s)->~T();
		}

		// assign
		template <class Value>
		static T& assign(Storage& s, Value&& value) {
			T& val = reinterpret_cast<T&>(s);
			val = std::forward<Value>(value);
			return val;
		}
		static T& assign_copy_move(Storage& s, const Storage& other) {
			T& val = reinterpret_cast<T&>(s);
			val = reinterpret_cast<std::add_const_t<T>&>(other);
			return val;
		}
		static T& assign_copy_move(Storage& s, Storage&& other) noexcept {
			T& val = reinterpret_cast<T&>(s);
			val = std::move(reinterpret_cast<T&>(other));
			return val;
		}

		// swap
		template <class U>
		static void swap(Storage& s, Storage& other) noexcept {
			if constexpr (InPlace<U>::value) {
				std::swap(reinterpret_cast<T&>(s), reinterpret_cast<T&>(other));
			} else {
				void* ptr = other.ptr;
				construct(other, std::move(s));
				s.ptr = ptr;
			}
		}

		// get
		static T& get(Storage& s) noexcept {
			return reinterpret_cast<T&>(s);
		}
		static const T& get(const Storage& s) noexcept {
			return reinterpret_cast<const T&>(s);
		}
	};

	template <class T>
	struct Manager<T, false> {
		// construct
		template <class... Args>
		static T& construct(Storage& s, Args&&... args) {
			T* val = new T(std::forward<Args>(args)...);
			s.ptr = val;
			return *val;
		}

		// construct
		static T& construct(Storage& s, const Storage& other) {
			T* val = new T(*static_cast<std::add_const_t<T>*>(other.ptr));
			s.ptr = val;
			return *val;
		}
		static T& construct(Storage& s, Storage&& other) noexcept {
			s.ptr = other.ptr;
			return *static_cast<T*>(s.ptr);
		}

		// destruct
		static void destruct(Storage& s) noexcept {
			delete static_cast<T*>(s.ptr);
		}

		// assign
		template <class Value>
		static T& assign(Storage& s, Value&& value) {
			T& val = *static_cast<T*>(s.ptr);
			val = std::forward<Value>(value);
			return val;
		}
		static T& assign_copy_move(Storage& s, const Storage& other) {
			T& val = *static_cast<T*>(s.ptr);
			val = *static_cast<std::add_const_t<T>*>(other.ptr);
			return val;
		}
		static T& assign_copy_move(Storage& s, Storage&& other) noexcept {
			static_cast<T*>(s.ptr)->~T();
			s.ptr = other.ptr;
			return *static_cast<T*>(s.ptr);
		}

		// swap
		template <class U>
		static void swap(Storage& s, Storage& other) noexcept {
			if constexpr (InPlace<U>::value) {
				void* ptr = s.ptr;
				Manager<U>::construct(s, std::move(other));
				other.ptr = ptr;
			} else {
				std::swap(s.ptr, other.ptr);
			}
		}

		// get
		static T& get(Storage& s) noexcept {
			return *static_cast<T*>(s.ptr);
		}
		static const T& get(const Storage& s) noexcept {
			return *static_cast<const T*>(s.ptr);
		}
	};

	Storage storage_;

public:
	constexpr any_storage() noexcept = default;

	any_storage(const any_storage&) = delete;
	any_storage& operator=(const any_storage) = delete;

	// constructors

	template <class T>
	any_storage(const any_storage& other, std::in_place_type_t<T>) {
		this->construct<T>(other);
	}

	template <class T>
	any_storage(any_storage&& other, std::in_place_type_t<T>) noexcept {
		this->construct<T>(std::move(other));
	}

	template <class Value, class = std::enable_if_t<!is_decay_to_v<Value, any_storage>>>
	any_storage(Value&& value) {
		this->construct<Value>(std::forward<Value>(value));
	}

	template <class T, class... Args>
	any_storage(std::in_place_type_t<T>, Args&&... args) {
		this->construct<T>(std::forward<Args>(args)...);
	}

	template <class T, class U, class... Args>
	any_storage(std::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args) {
		this->construct<T>(il, std::forward<Args>(args)...);
	}

	// construct

	template <class T, class... Args, class = std::enable_if_t<!is_types_decay_to_v<any_storage, Args...>>>
	std::decay_t<T>& construct(Args&&... args) {
		return Manager<std::decay_t<T>>::construct(storage_, std::forward<Args>(args)...);
	}

	template <class T, class U, class... Args>
	std::decay_t<T>& construct(std::initializer_list<U> il, Args&&... args) {
		return Manager<std::decay_t<T>>::construct(storage_, il, std::forward<Args>(args)...);
	}

	template <class T>
	std::decay_t<T>& construct(const any_storage& other) {
		return Manager<std::decay_t<T>>::construct(storage_, other.storage_);
	}

	template <class T>
	std::decay_t<T>& construct(any_storage&& other) noexcept {
		return Manager<std::decay_t<T>>::construct(storage_, std::move(other.storage_));
	}

	// destruct

	template <class T>
	void destruct() noexcept {
		Manager<std::decay_t<T>>::destruct(storage_);
	}

	// assign

	template <class T, class Value, class = std::enable_if_t<!is_decay_to_v<Value, any_storage>>>
	std::decay_t<T>& assign(Value&& value) {
		return Manager<std::decay_t<T>>::assign(storage_, std::forward<Value>(value));
	}

	template <class T>
	std::decay_t<T>& assign(const any_storage& other) {
		return Manager<std::decay_t<T>>::assign_copy_move(storage_, other.storage_);
	}

	template <class T>
	std::decay_t<T>& assign(any_storage&& other) {
		return Manager<std::decay_t<T>>::assign_copy_move(storage_, std::move(other.storage_));
	}

	// swap

	template <class T, class U = T>
	void swap(any_storage& other) noexcept {
		Manager<std::decay_t<T>>::template swap<std::decay_t<U>>(storage_, other.storage_);
	}

	// get

	template <class T>
	std::decay_t<T>& get() & noexcept {
		return Manager<std::decay_t<T>>::get(storage_);
	}

	template <class T>
	const std::decay_t<T>& get() const& noexcept {
		return Manager<std::decay_t<T>>::get(storage_);
	}

	template <class T>
	std::decay_t<T>&& get() && noexcept {
		return std::move(Manager<std::decay_t<T>>::get(storage_));
	}

	template <class T>
	const std::decay_t<T>&& get() const&& noexcept {
		return std::move(Manager<std::decay_t<T>>::get(storage_));
	}

public:
	template <class T>
	struct copy_construct_function {
		static void value(any_storage& to, const any_storage& other) { to.template construct<T>(other); }
	};
	template <class T>
	struct move_construct_function {
		static void value(any_storage& to, any_storage&& other) { to.template construct<T>(std::move(other)); }
	};
	template <class T>
	struct destruct_function {
		static void value(any_storage& s) { s.template destruct<T>(); }
	};
	template <class T>
	struct copy_assign_function {
		static void value(any_storage& to, const any_storage& other) { to.template assign<T>(other); }
	};
	template <class T>
	struct move_assign_function {
		static void value(any_storage& to, any_storage&& other) { to.template assign<T>(std::move(other)); }
	};
	template <class T, class U>
	struct swap_function {
		static void value(any_storage& s, any_storage& other) { s.template swap<T, U>(other); }
	};
};

} // namespace esl

#endif // ESL_ANY_STORAGE_HPP

