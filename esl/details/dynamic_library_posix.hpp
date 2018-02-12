
#ifndef ESL_DETAILS_DYNAMIC_LIBRARY_POSIX_HPP
#define ESL_DETAILS_DYNAMIC_LIBRARY_POSIX_HPP

#include <system_error>
#include <dlfcn.h>

namespace esl::details {

class dynamic_library {
public:
	using native_handle_type = void*;

	static constexpr native_handle_type invalid_handle = 0;

	static native_handle_type open(const char* filename) noexcept {
		return ::dlopen(filename, RTLD_LAZY);
	}

	static constexpr bool valid(native_handle_type handle) noexcept {
		return handle;
	}

	static bool close(native_handle_type handle) noexcept {
		return ::dlclose(handle) == 0;
	}

	static void* symbol(native_handle_type handle, const char* name) noexcept {
		return ::dlsym(handle, name);
	}

	// HOWTO: dlerror to error_code ?
	// static std::error_code error_code() noexcept {}

	static std::string error_message() {
		auto s = ::dlerror();
		return s ? s : "";
	}
};

} // namespace esl::details

#endif //ESL_DETAILS_DYNAMIC_LIBRARY_POSIX_HPP

