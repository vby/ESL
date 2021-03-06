
#ifndef ESL_IO_HPP
#define ESL_IO_HPP

#include <cassert>
#include <cstdio>
#include <memory>

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

#endif //ESL_IO_HPP
