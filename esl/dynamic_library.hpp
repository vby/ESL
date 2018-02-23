
#ifndef ESL_DYNAMIC_LIBRARY_HPP
#define ESL_DYNAMIC_LIBRARY_HPP

// posix -ldl

#include "macros.hpp"

#ifdef ESL_SOURCE_WIN32
	#include "details/dynamic_library_win32.hpp"
#elif defined ESL_SOURCE_POSIX
	#include "details/dynamic_library_posix.hpp"
#else
	#error "Not implemented."
#endif

#include <string>
#include <cassert>

namespace esl {

class dynamic_library {
private:
	using dl = details::dynamic_library;

public:
	using native_handle_type = typename dl::native_handle_type;

private:
	native_handle_type handle_;
	std::string error_message_;

public:
	dynamic_library() noexcept: handle_(dl::invalid_handle) {}

	dynamic_library(const char* filename): dynamic_library() { this->open(filename); }

	dynamic_library(const std::string& filename): dynamic_library() { this->open(filename); }

	dynamic_library(const dynamic_library&) = delete;

	dynamic_library(dynamic_library&& other) noexcept
			: handle_(other.handle_), error_message_(std::move(other.error_message_)) {
		other.handle_ = dl::invalid_handle;
	}

	~dynamic_library() noexcept {
		if (dl::valid(handle_)) {
			assert(dl::close(handle_));
		}
	}

	dynamic_library& operator=(const dynamic_library&) = delete;

	dynamic_library& operator=(dynamic_library&& other) noexcept {
		handle_ = other.handle_;
		error_message_ = std::move(other.error_message_);
		other.handle_ = dl::invalid_handle;
		return *this;
	}

	void open(const char* filename);

	void open(const std::string& filename) { this->open(filename.c_str()); }

	void close();

	template <class T = void>
	T* symbol(const char* name) const noexcept { return reinterpret_cast<T*>(dl::symbol(handle_, name)); }

	template <class T = void>
	T* symbol(const std::string& name) const noexcept { return symbol<T>(name.c_str()); }

	template <class T = void>
	T* symbol(const char* name, std::string& error_message) const {
		void* sym = dl::symbol(handle_, name);
		if (!sym) {
			error_message = dl::error_message();
		}
		return reinterpret_cast<T*>(sym);
	}

	template <class T = void>
	T* symbol(const std::string& name, std::string& error_message) const {
		return symbol<T>(name.c_str(), error_message);
	}

	void swap(dynamic_library& other) noexcept {
		std::swap(*this, other);
	}

	bool is_open() const noexcept { return dl::valid(handle_); }

	bool good() const noexcept { return error_message_.empty(); }

	operator bool() const noexcept { return this->is_open() && this->good(); }

	std::string& error_message() noexcept { return error_message_; }

	const std::string& error_message() const noexcept { return error_message_; }
};

inline void dynamic_library::open(const char* filename) {
	if (dl::valid(handle_)) {
		return;
	}
	handle_ = dl::open(filename);
	if (dl::valid(handle_)) {
		error_message_.clear();
	} else {
		error_message_ = dl::error_message();
	}
}

inline void dynamic_library::close() {
	if (!dl::valid(handle_)) {
		return;
	}
	if (dl::close(handle_)) {
		error_message_.clear();
		handle_ = dl::invalid_handle;
	} else {
		error_message_ = dl::error_message();
	}
}

} // namespace esl

#endif //ESL_DYNAMIC_LIBRARY_HPP

