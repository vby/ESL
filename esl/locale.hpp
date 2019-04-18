#ifndef ESL_LOCALE_HPP
#define ESL_LOCALE_HPP

#include <locale>
#include <stdexcept>
#include <utility>

namespace esl {

namespace details {

inline std::pair<std::locale, bool> make_locale(const char* name) {
    try {
        return {std::locale(name), true};
    } catch (const std::runtime_error&) {
    }
    return {std::locale{}, false};
}

} // namespace details

// preferred_locale
// locale("") or locale(), locale("") only check once
inline std::locale preferred_locale() {
    static std::pair<std::locale, bool> loc_st = details::make_locale("");
    return loc_st.second ? loc_st.first : std::locale{};
}

} //namespace esl

#endif // ESL_LOCALE_HPP
