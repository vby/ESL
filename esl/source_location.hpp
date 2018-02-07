
#ifndef ESL_SOURCE_LOCATION_HPP
#define ESL_SOURCE_LOCATION_HPP

#include <cstdint>

namespace esl {

struct source_location {
private:
	const char* file_;
	const char* func_;
	std::uint_least32_t line_;
	std::uint_least32_t column_;

public:
	constexpr source_location() noexcept: file_(""), func_(""), line_(0), column_(0) {} 

	constexpr source_location(const source_location&) = default;

	constexpr source_location(source_location&&) = default;

	constexpr std::uint_least32_t line() const noexcept { return line_; }

	constexpr std::uint_least32_t column() const noexcept { return column_; }

	constexpr const char* file_name() const noexcept { return file_; }

	constexpr const char* function_name() const noexcept { return func_; }

	static constexpr source_location current(
		#ifdef ESL_COMPILER_GNU
			#define ESL_HAS_SOURCE_LOCATION
			const char* file = __builtin_FILE(),
			const char* func = __builtin_FUNCTION(),
			std::uint_least32_t line = __builtin_LINE(),
			std::uint_least32_t column = 0
		#else
			const char* file,
			const char* func,
			std::uint_least32_t line,
			std::uint_least32_t column
		#endif
			) noexcept {
		source_location sl;
		sl.file_ = file;
		sl.func_ = func;
		sl.line_ = line;
		sl.column_ = column;
		return sl;
	}   
}; 

} // namespace esl

#define ESL_SOURCE_LOCATION_CURRENT() (::esl::source_location::current(__FILE__, __func__, __LINE__, 0))

#endif // ESL_SOURCE_LOCATION_HPP

