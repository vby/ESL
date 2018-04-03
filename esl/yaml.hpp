#ifndef ESL_YAML_HPP
#define ESL_YAML_HPP

#include <yaml.h>

#include "any_variant.hpp"
#include "functional.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <string_view>

namespace esl {

// See http://www.yaml.org/spec/1.2/spec.html

namespace yaml {

class node;

// types
using null_t = std::monostate;
using bool_t = bool;
using int_t = std::int64_t;
using float_t  = double;
using str = std::string;
using seq = std::vector<node>;
using map = std::unordered_map<node, node>;

// TODO additional types: binary, omap, set

// null
inline constexpr null_t null{};

// zero-base type index
enum index {
    null_index = 0,
    bool_index,
    int_index,
    float_index,
    str_index,
    seq_index,
    map_index,
};

using node_base = any_variant<null_t, bool_t, int_t, float_t, str, seq, map>;

class node: public node_base {
public:
    using any_variant::any_variant;

    using any_variant::operator=;

	// Enforce const char* deduce to std::string

	template <class T, class = std::enable_if_t<!std::is_convertible_v<T&&, const char*>>>
	node(T&& v): any_variant(std::forward<T>(v)) {}

	node(const char* s): any_variant(std::in_place_index<str_index>, s) {}
};

} // namespace yaml

} // namespace esl

namespace std {

template <>
struct hash<::esl::yaml::node> {
	size_t operator()(const ::esl::yaml::node& n) const {
		return hash<::esl::yaml::node_base>{}(n);
	}
};

} // namespace std

namespace esl {

namespace yaml {

class parse_error: public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

namespace details {

struct yaml_event: public yaml_event_t {
private:
	bool f_;
public:
	yaml_event() noexcept: f_(false) {}

	yaml_event(const yaml_event&) = delete;
	yaml_event& operator=(const yaml_event&) = delete;

	~yaml_event() { this->reset(); }

	void reset() noexcept {
		if (f_) {
			yaml_event_delete(this);
		} else {
			f_ = true;
		}
	}
};

struct yaml_parser: yaml_parser_t {
	yaml_parser() {
		if (!yaml_parser_initialize(this)) {
			throw std::bad_alloc{};
		}
	}

	yaml_parser(const yaml_parser&) = delete;
	yaml_parser& operator=(const yaml_parser&) = delete;

	~yaml_parser() { yaml_parser_delete(this); }

	void parse(yaml_event& ev) {
		ev.reset();
		if (!yaml_parser_parse(this, &ev)) {
			throw parse_error("parse error");
		}
	}
};

node parse_map(yaml_parser& parser);
node parse_seq(yaml_parser& parser);
node parse_scalar(yaml_event& ev);

inline node parse_node(yaml_parser& parser, yaml_event& ev) {
	switch (ev.type) {
	case YAML_MAPPING_START_EVENT:
		return parse_map(parser);
	case YAML_SEQUENCE_START_EVENT:
		return parse_seq(parser);
	case YAML_SCALAR_EVENT:
		return parse_scalar(ev);
	default:
		throw parse_error("unexpect type");
	}
}

inline node parse_scalar(yaml_event& ev) {
	// TODO type resolve
	auto& scalar = ev.data.scalar;
	std::string_view sv(reinterpret_cast<char*>(scalar.value), scalar.length);
	return node(std::in_place_type<str>, std::string(sv));
}

inline node parse_seq(yaml_parser& parser) {
	node n;
	auto& s = n.emplace<seq>();
	yaml_event ev;
	while (parser.parse(ev), ev.type != YAML_SEQUENCE_END_EVENT) {
		s.emplace_back(parse_node(parser, ev));
	}
	return n;
}

inline node parse_map(yaml_parser& parser) {
	node n;
	auto& m = n.emplace<map>();
	yaml_event ev;
	node* vn = nullptr;
	while (parser.parse(ev), ev.type != YAML_MAPPING_END_EVENT) {
		if (vn) {
			*vn = parse_node(parser, ev);
			vn = nullptr;
		} else {
			auto ok_it = m.emplace(parse_node(parser, ev), node{});
			vn = &(ok_it.first->second);
		}
	}
	return n;
}

inline node parse_doc(yaml_parser& parser) {
	node n;
	yaml_event ev;
	if (parser.parse(ev), ev.type != YAML_DOCUMENT_END_EVENT) {
		n = parse_node(parser, ev);
		if (parser.parse(ev), ev.type != YAML_DOCUMENT_END_EVENT) {
			throw parse_error("unexpect type");
		}
	}
	return n;
}

inline std::vector<node> load(const char* data, std::size_t size) {
	yaml_parser parser;
	yaml_parser_set_input_string(&parser, reinterpret_cast<const unsigned char*>(data), size);
	//yaml_parser_set_encoding
	std::vector<node> docs;
	yaml_event ev;
	while (parser.parse(ev), ev.type != YAML_STREAM_END_EVENT) {
		switch (ev.type) {
		case YAML_STREAM_START_EVENT:
			break;
		case YAML_DOCUMENT_START_EVENT:
			docs.emplace_back(parse_doc(parser));
			break;
		default:
			throw parse_error("unexpect type");
		}
	}
	return docs;
}

} // namespace details

using details::load;

} // namespace yaml

} // namespace esl

#endif // ESL_YAML_HPP

