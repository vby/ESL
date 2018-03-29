#ifndef ESL_YAML_HPP
#define ESL_YAML_HPP

#include <yaml.h>

#include "variant_any.hpp"

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
using map = std::unordered_map<std::string, node>;

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

class node: public variant_any<null_t, bool_t, int_t, float_t, str, seq, map> {
public:
    using variant_any::variant_any;

    using variant_any::operator=;
};

class parse_error: public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

namespace details {

class parser: public yaml_parser_t {
public:
	parser() {
		if (!yaml_parser_initialize(this)) {
			throw std::bad_alloc{};
		}
	}
	parser(const parser&) = delete;
	~parser() { yaml_parser_delete(this); }
	parser& operator=(const parser&) = delete;

protected:
	class event: public yaml_event_t {
	private:
		bool f_;
	public:
		event() noexcept: f_(false) {}
		event(const event&) = delete;
		event& operator=(const event&) = delete;
		~event() { this->reset(); }
		void reset() noexcept {
			if (f_) {
				yaml_event_delete(this);
			} else {
				f_ = true;
			}
		}
	};

	void parse_event(event& ev) {
		ev.reset();
		if (!yaml_parser_parse(this, &ev)) {
			throw parse_error("parse error");
		}
	}

	node parse_scalar(yaml_event_t& ev) {
		// TODO type resolve
		auto& scalar = ev.data.scalar;
		std::string_view sv(reinterpret_cast<char*>(scalar.value), scalar.length);
		return node(std::in_place_type<str>, std::string(sv));
	}

	void parse_seq(node& n) {
		auto& s = n.emplace<seq>();
		event ev;
		while (this->parse_event(ev), ev.type != YAML_SEQUENCE_END_EVENT) {
			switch (ev.type) {
			case YAML_MAPPING_START_EVENT:
				parse_map(s.emplace_back());
				break;
			case YAML_SEQUENCE_START_EVENT:
				parse_seq(s.emplace_back());
				break;
			case YAML_SCALAR_EVENT:
				s.emplace_back(parse_scalar(ev));
				break;
			default:
				throw parse_error("unexpect type in seq");
			}
		}
	}

	void parse_map_value(event& ev, node& n) {
		switch (ev.type) {
		case YAML_MAPPING_START_EVENT:
			parse_map(n);
			break;
		case YAML_SEQUENCE_START_EVENT:
			parse_seq(n);
			break;
		case YAML_SCALAR_EVENT:
			n = parse_scalar(ev);
			break;
		default:
			throw parse_error("unexpect type in map");
		}
	}

	void parse_map(node& n) {
		auto& m = n.emplace<map>();
		event ev;
		node* vn = nullptr;
		while (this->parse_event(ev), ev.type != YAML_MAPPING_END_EVENT) {
			if (vn) {
				parse_map_value(ev, *vn);
				vn = nullptr;
				continue;
			}
			if (ev.type == YAML_SCALAR_EVENT) {
				auto ok_it = m.emplace(std::piecewise_construct, std::forward_as_tuple(std::get<str>(parse_scalar(ev))), std::forward_as_tuple());
				vn = &(ok_it.first->second);
			} else {
				throw parse_error("error map key type");
			}
		}
	}

	void parse_doc(node& n) {
		event ev;
		while (this->parse_event(ev), ev.type != YAML_DOCUMENT_END_EVENT) {
			switch (ev.type) {
			case YAML_MAPPING_START_EVENT:
				parse_map(n);
				break;
			case YAML_SEQUENCE_START_EVENT:
				parse_seq(n);
				break;
			case YAML_SCALAR_EVENT:
				n = parse_scalar(ev);
				break;
			default:
				throw parse_error("unexpect type in doc");
			}
		}
	}

public:
	std::vector<node> parse(const char* data, std::size_t size) {
		yaml_parser_set_input_string(this, reinterpret_cast<const unsigned char*>(data), size);
		//yaml_parser_set_encoding
		std::vector<node> docs;
		event ev;
		while (this->parse_event(ev), ev.type != YAML_STREAM_END_EVENT) {
			switch (ev.type) {
			case YAML_DOCUMENT_START_EVENT:
				parse_doc(docs.emplace_back());
				break;
			case YAML_STREAM_START_EVENT:
				break;
			default:
				throw parse_error("unexpect type in stream");
			}
		}
		return docs;
	}
};

} // namespace details

inline std::vector<node> parse(const char* data, std::size_t size) {
	details::parser psr;
	return psr.parse(data, size);
}

} // namespace yaml

} // namespace esl

#endif // ESL_YAML_HPP

