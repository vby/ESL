
#ifndef ESL_SCOPE_HPP
#define ESL_SCOPE_HPP

#include <cstdio>
#include <functional>
#include <memory>

namespace esl {

// unique_function
class unique_function {
private:
    std::function<void()> f_;

    void invoke() const {
        if (f_) {
            f_();
        }
    }

public:
    unique_function() noexcept = default;

    template <class F>
    unique_function(F&& f) : f_(std::forward<F>(f)) {}

    unique_function(const unique_function&) = delete;

    unique_function(unique_function&& other) : f_(std::move(other.f_)) {}

    ~unique_function() {
        this->invoke();
    }

    unique_function& operator=(const unique_function&) = delete;

    unique_function& operator=(unique_function&& other) {
        f_ = std::move(other.f_);
        return *this;
    }

    void swap(unique_function& other) noexcept {
        f_.swap(other.f_);
    }

    template <class F>
    void reset(F&& f) {
        this->invoke();
        f_ = std::forward<F>(f);
    }

    void reset() {
        this->reset(nullptr);
    }

    void release() noexcept {
        f_ = nullptr;
    }

    operator bool() const noexcept {
        return static_cast<bool>(f_);
    }
};

} // namespace esl

#endif //ESL_SCOPE_HPP
