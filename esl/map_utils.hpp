
#ifndef ESL_MAP_UTILS_HPP
#define ESL_MAP_UTILS_HPP

#include "exception.hpp"
#include "type_traits.hpp"

#include <utility>
#include <tuple>

namespace esl {

// map_mapped_t
template <class Map>
using map_mapped_t_ = as_const_as_t<typename Map::mapped_type, Map>;
template <class Map>
using map_mapped_t = map_mapped_t_<std::remove_reference_t<Map>>;

// map_find

// Return a copy of Map::mapped_type
template <class Map, class Key>
typename Map::mapped_type map_find(const Map& m, const Key& key) {
	auto it = m.find(key);
	if (it == m.end()) {
		return typename Map::mapped_type{};
	}
	return it->second;
}
template <class Map, class Key, class Mapped>
typename Map::mapped_type map_find(const Map& m, const Key& key, const Mapped& def_mapped) {
	auto it = m.find(key);
	return it != m.end() ? it->second : def_mapped;
}

// Return a ptr of map_mapped_t<Map>
template <class Map, class Key, class Mapped = map_mapped_t<Map>>
auto map_find_ptr(Map&& m, const Key& key, Mapped* def_mapped_ptr = nullptr) -> map_mapped_t<Map>* {
	auto it = m.find(key);
	return it != m.end() ? &it->second : def_mapped_ptr;
}

// Return a ref of map_mapped_t<Map>
// Exceptions: esl::key_not_found
template <class Map, class Key>
auto map_find_ref(Map&& m, const Key& key) -> map_mapped_t<Map>& {
	auto it = m.find(key);
	if (it == m.end()) {
		throw key_not_found("esl::map_find_ref: key not found");
	}
	return it->second;
}
template <class Map, class Key, class Mapped>
auto map_find_ref(Map&& m, const Key& key, Mapped& def_mapped) -> map_mapped_t<Map>& {
	auto it = m.find(key);
	return it != m.end() ? it->second : def_mapped;
}

} // namespace esl

#endif //ESL_MAP_UTILS_HPP

