
#ifndef ESL_MAP_UTILS_HPP
#define ESL_MAP_UTILS_HPP

#include "exception.hpp"
#include "type_traits.hpp"

#include <utility>
#include <tuple>

namespace esl {

// map_mapped_t
template <class Map>
using map_mapped_t_ = const_as_t<typename Map::mapped_type, Map>;
template <class Map>
using map_mapped_t = map_mapped_t_<std::remove_reference_t<Map>>;

// map_get
// Return a ref of map_mapped_t<Map>
// Exceptions: esl::key_not_found
template <class Map, class Key>
auto map_get(Map&& m, const Key& key) -> map_mapped_t<Map>& {
	auto it = m.find(key);
	if (it == m.end()) {
		throw key_not_found("esl::map_get");
	}
	return it->second;
}
template <class Map, class Key, class Mapped>
inline auto map_get(Map&& m, const Key& key, Mapped& def_mapped) -> map_mapped_t<Map>& {
	auto it = m.find(key);
	return it != m.end() ? it->second : def_mapped;
}

// map_getv
// Return a copy of Map::mapped_type
template <class Map, class Key>
typename Map::mapped_type map_getv_nothrow(const Map& m, const Key& key) {
	auto it = m.find(key);
	if (it == m.end()) {
		return typename Map::mapped_type{};
	}
	return it->second;
}
// Exceptions: esl::key_not_found
template <class Map, class Key>
typename Map::mapped_type map_getv(const Map& m, const Key& key) {
	auto it = m.find(key);
	if (it == m.end()) {
		throw key_not_found("esl::map_getv");
	}
	return it->second;
}
template <class Map, class Key, class Mapped>
inline typename Map::mapped_type map_getv(const Map& m, const Key& key, const Mapped& def_mapped) {
	auto it = m.find(key);
	return it != m.end() ? it->second : def_mapped;
}

// map_getp
// Return a ptr of map_mapped_t<Map>
template <class Map, class Key>
inline auto map_getp_nothrow(Map&& m, const Key& key) -> map_mapped_t<Map>* {
	auto it = m.find(key);
	return it != m.end() ? &it->second : nullptr;
}
// Exceptions: esl::key_not_found
template <class Map, class Key>
auto map_getp(Map&& m, const Key& key) -> map_mapped_t<Map>* {
	auto it = m.find(key);
	if (it == m.end()) {
		throw key_not_found("esl::map_getp");
	}
	return &it->second;
}
template <class Map, class Key, class Mapped>
inline auto map_getp(Map&& m, const Key& key, Mapped* def_mapped_ptr) -> map_mapped_t<Map>* {
	auto it = m.find(key);
	return it != m.end() ? &it->second : def_mapped_ptr;
}

// map_contains
template <class Map, class Key>
inline bool map_contains(const Map& m, const Key& key) {
	return m.find(key) != m.end();
}

} // namespace esl

#endif //ESL_MAP_UTILS_HPP

