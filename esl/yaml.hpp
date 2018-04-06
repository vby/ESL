#ifndef ESL_YAML_HPP
#define ESL_YAML_HPP

#include <yaml.h>

#include "flex_variant.hpp"
#include "functional.hpp"

#include <cassert>
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
// TODO
//using map = std::unordered_map<node, node>;
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

using node_base = flex_variant<null_t, bool_t, int_t, float_t, str, seq, map>;

class node: public node_base {
public:
	using node_base::node_base;

	// TODO universal type deduction guide
	// Enforce const char* deduce to std::string
};

} // namespace yaml

} // namespace esl

namespace std {

// std::variant_size
template <>
struct variant_size<::esl::yaml::node>: variant_size<::esl::yaml::node_base> {};

// std::variant_alternative
template <std::size_t I>
struct variant_alternative<I, ::esl::yaml::node>: variant_alternative<I, ::esl::yaml::node_base> {};

// std::hash
template <>
struct hash<::esl::yaml::node> {
	size_t operator()(const ::esl::yaml::node& n) const {
		return hash<::esl::yaml::node_base>{}(n);
	}
};

} // namespace std

namespace esl {

namespace yaml {

class load_error: public std::runtime_error {
private:
	std::size_t position_;
	std::size_t line_;
	std::size_t column_;

public:
	load_error(const char* msg, std::size_t position, std::size_t line, std::size_t column):
		runtime_error(msg), position_(position), line_(line), column_(column) {}

	load_error(const char* msg, const yaml_mark_t& mark): load_error(msg, mark.index, mark.line, mark.column) {}

	std::size_t position() const noexcept { return position_; }

	std::size_t line() const noexcept { return line_; }

	std::size_t column() const noexcept { return column_; }
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
			f_ = false;
		}
	}

	void attach() noexcept { f_ = true; }
};

struct yaml_parser: yaml_parser_t {
private:
	using NodeStroage = std::aligned_storage_t<sizeof(node), alignof(node)>;
	std::unordered_map<std::string, NodeStroage> anchor_map_;

public:
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
			throw load_error(this->problem ? this->problem : "parse error", this->problem_mark);
		}
		ev.attach();
	}

	void anchor(const yaml_char_t* anchor, const node& n) {
		if (anchor) {
			NodeStroage storage(reinterpret_cast<const NodeStroage&>(n));
			anchor_map_.insert_or_assign(reinterpret_cast<const char*>(anchor), storage);
		}
	}

	const node& alias(const yaml_event& ev) const {
		auto it = anchor_map_.find(reinterpret_cast<const char*>(ev.data.alias.anchor));
		if (it == anchor_map_.end()) {
			throw load_error("no such anchor", ev.start_mark);
		}
		return reinterpret_cast<const node&>(it->second);
	}

	void reset_anchors() {
		anchor_map_.clear();
	}
};

void parse_map(yaml_parser& parser, node& node);
void parse_seq(yaml_parser& parser, node& node);
void parse_scalar(const yaml_event& ev, node& node);

inline void parse_node(yaml_parser& parser, const yaml_event& ev, node& n) {
	switch (ev.type) {
	case YAML_MAPPING_START_EVENT:
		parse_map(parser, n);
		parser.anchor(ev.data.mapping_start.anchor, n);
		break;
	case YAML_SEQUENCE_START_EVENT:
		parse_seq(parser, n);
		parser.anchor(ev.data.sequence_start.anchor, n);
		break;
	case YAML_SCALAR_EVENT:
		parse_scalar(ev, n);
		parser.anchor(ev.data.scalar.anchor, n);
		break;
	case YAML_ALIAS_EVENT:
		n = parser.alias(ev);
		break;
	default:
		assert(false);
	}
}

inline void parse_scalar(const yaml_event& ev, node& n) {
	// TODO type resolve
	auto& scalar = ev.data.scalar;
	std::string_view sv(reinterpret_cast<const char*>(scalar.value), scalar.length);
	n.emplace<str>(sv);
}

inline void parse_seq(yaml_parser& parser, node& n) {
	auto& s = n.emplace<seq>();
	yaml_event ev;
	while (parser.parse(ev), ev.type != YAML_SEQUENCE_END_EVENT) {
		parse_node(parser, ev, s.emplace_back());
	}
}

inline void parse_map(yaml_parser& parser, node& n) {
	auto& m = n.emplace<map>();
	yaml_event ev;
	while (parser.parse(ev), ev.type != YAML_MAPPING_END_EVENT) {
		node kn;
		parse_node(parser, ev, kn);
		auto it_st = m.insert_or_assign(std::get<str>(std::move(kn)), node{});
		parser.parse(ev);
		parse_node(parser, ev, it_st.first->second);
	}
}

inline std::vector<node> load(std::string_view sv) {
	yaml_parser parser;
	yaml_parser_set_input_string(&parser, reinterpret_cast<const yaml_char_t*>(sv.data()), sv.size());
	//yaml_parser_set_encoding
	std::vector<node> docs;
	yaml_event ev;
	while (parser.parse(ev), ev.type != YAML_STREAM_END_EVENT) {
		switch (ev.type) {
		case YAML_STREAM_START_EVENT:
			break;
		case YAML_DOCUMENT_START_EVENT:
			parser.parse(ev);
			parse_node(parser, ev, docs.emplace_back());
			break;
		case YAML_DOCUMENT_END_EVENT:
			parser.reset_anchors();
			break;
		default:
			assert(false);
		}
	}
	return docs;
}

struct yaml_emitter: yaml_emitter_t {
	yaml_emitter() {
		if (!yaml_emitter_initialize(this)) {
			throw std::bad_alloc{};
		}
	}
	yaml_emitter(const yaml_emitter&) = delete;
	yaml_emitter& operator=(const yaml_emitter&) = delete;
	~yaml_emitter() { yaml_emitter_delete(this); }

private:
	yaml_event_t ev_;

	void emit() {
		if (!yaml_emitter_emit(this, &ev_)) {
			throw std::bad_alloc{};
		}
	}

public:
	void document_start() {
		if (!yaml_document_start_event_initialize(&ev_, nullptr, nullptr, nullptr, 1)) {
			throw std::bad_alloc{};
		}
		this->emit();
	}
	void document_end() {
		if (!yaml_document_end_event_initialize(&ev_, 1)) {
			throw std::bad_alloc{};
		}
		this->emit();
	}
	void sequence_start() {
		if (!yaml_sequence_start_event_initialize(&ev_, nullptr, nullptr, 1, YAML_ANY_SEQUENCE_STYLE)) {
			throw std::bad_alloc{};
		}
		this->emit();
	}
	void sequence_end() {
		if (!yaml_sequence_end_event_initialize(&ev_)) {
			throw std::bad_alloc{};
		}
		this->emit();
	}
	void mapping_start() {
		if (!yaml_mapping_start_event_initialize(&ev_, nullptr, nullptr, 1, YAML_ANY_MAPPING_STYLE)) {
			throw std::bad_alloc{};
		}
		this->emit();
	}
	void mapping_end() {
		if (!yaml_mapping_end_event_initialize(&ev_)) {
			throw std::bad_alloc{};
		}
		this->emit();
	}
	void scalar(std::string_view sv) {
		yaml_char_t* value = const_cast<yaml_char_t*>(reinterpret_cast<const yaml_char_t*>(sv.data()));
		if (!yaml_scalar_event_initialize(&ev_, nullptr, nullptr, value, static_cast<int>(sv.size()), 1, 1, YAML_ANY_SCALAR_STYLE)) {
			throw std::bad_alloc{};
		}
		this->emit();
	}

	void node(const node& n) {
		visit(*this, n);
	}

	void document(const class node& n) {
		this->document_start();
		this->node(n);
		this->document_end();
	}

	// visitor
	void operator()(null_t) { this->scalar("null"); }
	void operator()(bool_t b) { this->scalar(b ? "true" : "false"); }
	void operator()(int_t n) {
		auto s = std::to_string(n);
		this->scalar(s);
	}
	void operator()(float_t f) {
		auto s = std::to_string(f);
		this->scalar(s);
	}
	void operator()(const str& s) { this->scalar(s); }
	void operator()(const seq& sq) {
		this->sequence_start();
		for (auto& n: sq) {
			this->node(n);
		}
		this->sequence_end();
	}
	void operator()(const map& m) {
		this->mapping_start();
		for (auto& kn: m) {
			this->scalar(kn.first);
			this->node(kn.second);
		}
		this->mapping_end();
	}
};

inline int write_handler(void* data, yaml_char_t* buffer, std::size_t size) {
	static_cast<std::string*>(data)->append(reinterpret_cast<char*>(buffer), size);
	return 1;
}

inline std::string dump(const std::vector<node>& docs) {
	std::string s;
	yaml_emitter emitter;
	yaml_emitter_set_output(&emitter, write_handler, &s);
	//yaml_emitter_set_encoding
	//yaml_emitter_set_canonical
	//yaml_emitter_set_indent
	//yaml_emitter_set_width
	//yaml_emitter_set_unicode
	//yaml_emitter_set_break
	if (!yaml_emitter_open(&emitter)) {
		throw std::bad_alloc{};
	}
	for (auto& doc: docs) {
		emitter.document(doc);
		if (!yaml_emitter_flush(&emitter)) {
			assert(false);
		}
	}
	if (!yaml_emitter_close(&emitter)) {
		assert(false);
	}
	return s;
}

} // namespace details

using details::load;
using details::dump;

} // namespace yaml

} // namespace esl

#endif // ESL_YAML_HPP

