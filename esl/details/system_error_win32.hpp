
#ifndef ESL_DETAILS_SYSTEM_ERROR_WIN32_HPP
#define ESL_DETAILS_SYSTEM_ERROR_WIN32_HPP

#include <system_error>

#include <Windows.h>

namespace esl::details {

namespace win32 {

inline std::error_code last_error_code() noexcept {
	return std::error_code(static_cast<int>(::GetLastError()), std::system_category());
}

} // namespace win32

} // namespace esl::details

#endif //ESL_DETAILS_SYSTEM_ERROR_WIN32_HPP

