
#ifndef ESL_FILE_UTILS_HPP
#define ESL_FILE_UTILS_HPP

#include <memory>
#include <cstdio>
#include <cassert>

namespace esl {

// FILE_delete
// fclose std::FILE, ingore errors
struct FILE_delete {
	void operator()(std::FILE* f) const noexcept {
		[[maybe_unused]] int r = std::fclose(f);
		assert(r == 0);
	}
};

using unique_FILE = std::unique_ptr<std::FILE, FILE_delete>;

} // namespace esl

#endif //ESL_FILE_UTILS_HPP

