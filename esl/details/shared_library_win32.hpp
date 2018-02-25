
#ifndef ESL_DETAILS_SHARED_LIBRARY_WIN32_HPP
#define ESL_DETAILS_SHARED_LIBRARY_WIN32_HPP

#include "system_error_win32.hpp"

#include <Windows.h>

namespace esl::details {

class shared_library {
public:
	using native_handle_type = HMODULE;

	static constexpr native_handle_type invalid_handle = 0;

	static constexpr bool valid(native_handle_type handle) noexcept {
		return handle;
	}

	static native_handle_type open(const char* filename, std::error_code& ec) noexcept {
		auto handle = ::LoadLibrary(filename);
		if (handle == 0) {
			ec = win32::last_error_code();
		}
		return handle;
	}

	static bool close(native_handle_type handle, std::error_code& ec) noexcept {
		if (!::FreeLibrary(handle)) {
			ec = win32::last_error_code();
			return false;
		}
		return true;
	}

	static void* symbol(native_handle_type handle, const char* name) noexcept {
		return ::GetProcAddress(handle, name);
	}

	static void* symbol(native_handle_type handle, const char* name, std::error_code& ec) noexcept {
		void* sym = symbol(handle, name);
		if (!sym) {
			ec = win32::last_error_code();
		}
		return sym;
	}

	static void throw_exceptions(std::error_code ec) {
		throw std::system_error(ec);
	}

	static void error_message(std::string& err_msg) {
		err_msg = win32::last_error_code().message();
	}
};

} // namespace esl::details

#endif //ESL_DETAILS_SHARED_LIBRARY_WIN32_HPP

