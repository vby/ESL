#ifndef ESL_FLEX_HPP
#define ESL_FLEX_HPP

#include "flex_storage.hpp"

namespace esl {

template <class T>
class flex {
public:
    using value_type = T;

private:
    using Storage = flex_storage<>;

    Storage storage_;

public:
    // Constructors

    constexpr flex() : storage_(std::in_place_type<T>) {}

    //template <class... Args, class = std::enable_if_t<!is_types_decay_to_v<flex, Args...> && std::is_constructible_v<T, Args&&...>>>
    template <class... Args>
    flex(std::in_place_t, Args&&... args) : storage_(std::in_place_type<T>, std::forward<Args>(args)...) {}

    template <class U, class... Args>
    flex(std::in_place_t, std::initializer_list<U> il, Args&&... args) : storage_(std::in_place_type<T>, il, std::forward<Args>(args)...) {}

    flex(const flex& other) : storage_(other.storage_, std::in_place_type<T>) {}

    flex(flex&& other) noexcept : storage_(std::move(other.storage_), std::in_place_type<T>) {}

    ~flex() {
        storage_.destruct<T>();
    }

    // operator=

    template <class Value = T, class = std::enable_if_t<!is_decay_to_v<Value, flex>>>
    flex& operator=(Value&& value) {
        storage_.get<T>() = std::forward<Value>(value);
        return *this;
    }

    flex& operator=(const flex& other) {
        storage_.get<T>() = other.storage_.template get<T>();
        return *this;
    }

    flex& operator=(flex&& other) noexcept {
        storage_.get<T>() = std::move(other.storage_.template get<T>());
        return *this;
    }

    // Observers

    // value
    T& value() & noexcept {
        return storage_.get<T>();
    }

    const T& value() const& noexcept {
        return storage_.get<T>();
    }

    T&& value() && noexcept {
        return std::move(this->value());
    }

    const T& value() const&& noexcept {
        return std::move(this->value());
    }

    // operator->
    T* operator->() noexcept {
        return &this->value();
    }

    const T* operator->() const noexcept {
        return &this->value();
    }

    // operator*
    T& operator*() & noexcept {
        return this->value();
    }

    const T& operator*() const& noexcept {
        return this->value();
    }

    T&& operator*() && noexcept {
        return std::move(this->value());
    }

    const T&& operator*() const&& noexcept {
        return std::move(this->value());
    }

    // Modifiers

    void swap(flex& other) noexcept {
        storage_.swap<T>(other.storage_);
    }

    // emplace
    template <class... Args>
    T& emplace(Args&&... args) {
        storage_.destruct<T>();
        return storage_.construct<T>(std::forward<Args>(args)...);
    }

    template <class U, class... Args>
    T& emplace(std::initializer_list<U> il, Args&&... args) {
        storage_.destruct<T>();
        return storage_.construct<T>(il, std::forward<Args>(args)...);
    }
};

} // namespace esl

#endif // ESL_FLEX_HPP
