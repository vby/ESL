
#ifndef ESL_SHARED_LIBRARY_HPP
#define ESL_SHARED_LIBRARY_HPP

// posix -ldl

#include "macros.hpp"

#include <system_error>
#include <memory>
#include <cassert>

#ifdef ESL_SOURCE_WIN32
	#include "details/shared_library_win32.hpp"
#elif defined ESL_SOURCE_POSIX
	#include "details/shared_library_posix.hpp"
#else
	#error "Not implemented."
#endif

namespace esl {

class shared_library {
private:
	using dl = details::shared_library;

public:
	using native_handle_type = typename dl::native_handle_type;

private:
	native_handle_type handle_;

public:
	shared_library() noexcept: handle_(dl::invalid_handle) {}

	// Exceptions: std::system_error
	shared_library(const char* filename): shared_library() { this->open(filename); }

	// Exceptions: std::system_error
	shared_library(const std::string& filename): shared_library(filename.c_str()) {}

	shared_library(const shared_library&) = delete;

	shared_library(shared_library&& other) noexcept: handle_(other.handle_) {
		other.handle_ = dl::invalid_handle;
	}

	~shared_library() noexcept {
		if (dl::valid(handle_)) {
			std::error_code ec;
			[[maybe_unused]] bool r = dl::close(handle_, ec);
			assert(r);
		}
	}

	shared_library& operator=(const shared_library&) = delete;

	shared_library& operator=(shared_library&& other) noexcept {
		handle_ = other.handle_;
		other.handle_ = dl::invalid_handle;
		return *this;
	}

	// Exceptions: std::system_error
	void open(const char* filename);

	// Exceptions: std::system_error
	void open(const std::string& filename) { this->open(filename.c_str()); }

	// Exceptions: std::system_error
	void close();

	bool is_open() const noexcept { return dl::valid(handle_); }

	operator bool() const noexcept { return this->is_open(); }

	native_handle_type native_handle() const noexcept { return handle_; }

	void swap(shared_library& other) noexcept { std::swap(*this, other); }

	// has_symbol
	bool has_symbol(const char* name) const noexcept { return dl::symbol(handle_, name); }

	bool has_symbol(const std::string& name) const noexcept { return this->has_symbol(name.c_str()); }

	// get_symbol, get_symbol<T>

	// Exceptions: std::bad_alloc, std::system_error
	template <class T = void*>
	T get_symbol(const char* name) const {
		std::error_code ec;
		void* sym = dl::symbol(handle_, name, ec);
		if (!sym) {
			dl::throw_exceptions(ec);
		}
		return reinterpret_cast<T>(sym);
	}

	// Exceptions: std::bad_alloc, std::system_error
	template <class T = void*>
	T get_symbol(const std::string& name) const { return this->get_symbol<T>(name.c_str()); }

	// Exceptions: std::bad_alloc
	template <class T = void*>
	T get_symbol(const char* name, std::string& err_msg) const {
		void* sym = dl::symbol(handle_, name);
		if (!sym) {
			dl::error_message(err_msg);
		}
		return reinterpret_cast<T>(sym);
	}

	// Exceptions: std::bad_alloc
	template <class T = void*>
	T get_symbol(const std::string& name, std::string& err_msg) const {
		return this->get_symbol<T>(name.c_str(), err_msg);
	}
};

inline void shared_library::open(const char* filename) {
	this->close();

	if (!filename || filename[0] == '\0') {
		throw std::system_error(std::make_error_code(std::errc::invalid_argument));
	}

	std::error_code ec;
	handle_ = dl::open(filename, ec);
	if (!dl::valid(handle_)) {
		dl::throw_exceptions(ec);
	}
}

inline void shared_library::close() {
	if (!dl::valid(handle_)) {
		return;
	}

	std::error_code ec;
	if (dl::close(handle_, ec)) {
		handle_ = dl::invalid_handle;
	} else {
		dl::throw_exceptions(ec);
	}
}

} // namespace esl

#endif //ESL_SHARED_LIBRARY_HPP

