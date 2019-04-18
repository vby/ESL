
#ifndef ESL_MAP_UTILS_HPP
#define ESL_MAP_UTILS_HPP

#include "exception.hpp"
#include "type_traits.hpp"

#include <tuple>
#include <utility>

namespace esl {

// map_mapped, map_mapped_t
template <class Map>
struct map_mapped : const_as<typename Map::mapped_type, Map> {};
template <class Map>
using map_mapped_t = typename map_mapped<Map>::type;

// map_get
// Exceptions: esl::key_not_found
template <class Map, class Key>
inline map_mapped_t<std::remove_reference_t<Map>>& map_get(Map&& m, const Key& key) {
    auto it = m.find(key);
    if (it == m.end()) {
        throw key_not_found("esl::map_get");
    }
    return it->second;
}
template <class Map, class Key, class Mapped>
inline map_mapped_t<std::remove_reference_t<Map>>& map_get(Map&& m, const Key& key, Mapped& def) {
    auto it = m.find(key);
    return it != m.end() ? it->second : def;
}

// map_get_if
template <class Map, class Key>
inline std::add_pointer_t<map_mapped_t<Map>> map_get_if(Map* m, const Key& key) {
    if (!m) {
        return nullptr;
    }
    auto it = m->find(key);
    return it != m->end() ? &it->second : nullptr;
}

// map_contains
template <class Map, class Key>
inline bool map_contains(const Map& m, const Key& key) {
    return m.find(key) != m.end();
}

} // namespace esl

#endif //ESL_MAP_UTILS_HPP
