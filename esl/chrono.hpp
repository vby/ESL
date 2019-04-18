
#ifndef ESL_CHRONO_HPP
#define ESL_CHRONO_HPP

#include <chrono>
#include <ctime>
#include <ratio>

namespace esl {

namespace chrono {

// cast_from_duration_
template <class To>
struct cast_from_duration_ {
    template <class Rep, class Period>
    static constexpr To cast(const std::chrono::duration<Rep, Period>& d) noexcept {
        return std::chrono::duration_cast<To>(d);
    }
};
template <>
struct cast_from_duration_<std::timespec> {
    // d < 2^64 nanoseconds
    template <class Rep, class Period>
    static constexpr std::timespec cast(const std::chrono::duration<Rep, Period>& d) noexcept {
        auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
        timespec ts;
        ts.tv_sec = static_cast<std::time_t>(nsec / std::nano::den);
        ts.tv_nsec = static_cast<long>(nsec - (ts.tv_sec * std::nano::den));
        return ts;
    }
};
template <>
struct cast_from_duration_<std::time_t> {
    template <class Rep, class Period>
    static constexpr std::time_t cast(const std::chrono::duration<Rep, Period>& d) noexcept {
        return std::chrono::duration_cast<std::chrono::seconds>(d).count();
    }
};
template <class Clock, class ToDuration>
struct cast_from_duration_<std::chrono::time_point<Clock, ToDuration>> {
    template <class Rep, class Period>
    static constexpr std::chrono::time_point<Clock, ToDuration> cast(const std::chrono::duration<Rep, Period>& d) noexcept {
        return std::chrono::time_point<Clock, ToDuration>(std::chrono::duration_cast<ToDuration>(d));
    }
};
template <class To>
struct cast_from_time_point_ {
    template <class Clock, class Duration>
    static constexpr To cast(const std::chrono::time_point<Clock, Duration>& t) noexcept {
        return cast_from_duration_<To>::cast(t.time_since_epoch());
    }
};
template <class Clock, class ToDuration>
struct cast_from_time_point_<std::chrono::time_point<Clock, ToDuration>> {
    template <class Duration>
    static constexpr std::chrono::time_point<Clock, ToDuration> cast(const std::chrono::time_point<Clock, Duration>& t) noexcept {
        return std::chrono::time_point_cast<ToDuration>(t);
    }
};
// chrono_cast<T>: std::chrono::duration, std::chrono::time_point, std::timespec, std::time_t
template <class To, class Rep, class Period>
inline constexpr To chrono_cast(const std::chrono::duration<Rep, Period>& d) noexcept {
    return cast_from_duration_<To>::cast(d);
}
template <class To, class NanoSecondsRep = typename std::chrono::nanoseconds::rep>
inline constexpr To chrono_cast(const std::timespec& ts) noexcept {
    return cast_from_duration_<To>::cast(std::chrono::duration<NanoSecondsRep, std::nano>{ts.tv_sec * std::nano::den + ts.tv_nsec});
}
template <class To, class SecondsRep = typename std::chrono::seconds::rep>
inline constexpr To chrono_cast(const std::time_t& t) noexcept {
    return cast_from_duration_<To>::cast(std::chrono : duration<SecondsRep>{t});
}
template <class To, class Clock, class Duration>
inline constexpr To chrono_cast(const std::chrono::time_point<Clock, Duration>& t) noexcept {
    return cast_from_time_point_<To>::cast(t);
}
template <class ToDuration, class T>
inline constexpr typename ToDuration::rep chrono_count(const T& t) noexcept {
    return chrono_cast<ToDuration>(t).count();
}

} // namespace chrono

} // namespace esl

#endif //ESL_CHRONO_HPP
