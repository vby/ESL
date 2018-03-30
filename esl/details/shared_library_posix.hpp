
#ifndef ESL_DETAILS_SHARED_LIBRARY_POSIX_HPP
#define ESL_DETAILS_SHARED_LIBRARY_POSIX_HPP

#if __has_include(<dlfcn.h>)
	#include <dlfcn.h>
#else
	#error "Not implemented."
#endif

#include <system_error>

namespace esl::details {

class shared_library {
public:
	using native_handle_type = void*;

	static constexpr native_handle_type invalid_handle = nullptr;

	static constexpr bool valid(native_handle_type handle) noexcept { return handle; }

	static native_handle_type open(const char* filename, std::error_code& ec) noexcept {
		auto handle = ::dlopen(filename, RTLD_LAZY);
		if (!handle) {
			ec = std::make_error_code(std::errc::bad_file_descriptor);
		}
		return handle;
	}

	static bool close(native_handle_type handle, std::error_code& ec) noexcept {
		if (::dlclose(handle) != 0) {
			ec = std::make_error_code(std::errc::bad_file_descriptor);
			return false;
		}
		return true;
	}

	static void* symbol(native_handle_type handle, const char* name) noexcept {
		return ::dlsym(handle, name);
	}

	static void* symbol(native_handle_type handle, const char* name, std::error_code& ec) noexcept {
		void* sym = symbol(handle, name);
		if (!sym) {
			ec = std::make_error_code(std::errc::invalid_seek);
		}
		return sym;
	}

	static void throw_exceptions(std::error_code ec) {
		throw std::system_error(ec, ::dlerror());
	}

	static void error_message(std::string& err_msg) {
		err_msg = ::dlerror();
	}
};

} // namespace esl::details

#endif //ESL_DETAILS_SHARED_LIBRARY_POSIX_HPP

