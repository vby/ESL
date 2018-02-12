
#ifndef ESL_DETAILS_DYNAMIC_LIBRARY_WIN32_HPP
#define ESL_DETAILS_DYNAMIC_LIBRARY_WIN32_HPP

#include <system_error>
#include <Windows.h>

namespace esl::details {

class dynamic_library {
public:
	using native_handle_type = HANDLE;

	static constexpr native_handle_type invalid_handle = 0;

	static native_handle_type open(const char* filename) noexcept {
		return ::LoadLibrary(filename);
	}

	static constexpr bool valid(native_handle_type handle) noexcept {
		return handle;
	}

	static bool close(native_handle_type handle) noexcept {
		return ::FreeLibrary(handle);
	}

	static void* symbol(native_handle_type handle, const char* name) noexcept {
		return ::GetProcAddress(handle, name);
	}

	static std::error_code error_code() noexcept {
		return std::error_code(static_cast<int>(::GetLastError(), std::system_category());
	}

	static std::string error_message() {
		return error_code().message();
	}
};

} // namespace esl::details

#endif //ESL_DETAILS_DYNAMIC_LIBRARY_WIN32_HPP

