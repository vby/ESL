
#ifndef ESL_LINKED_LIST_HPP
#define ESL_LINKED_LIST_HPP

#include <list>

namespace esl {

template <class T, class Allocator = std::allocator<T>>
class linked_list: public std::list<T, Allocator> {
private:
	using list_type = std::list<T, Allocator>;

public:
	using typename list_type::iterator;
	using typename list_type::const_iterator;
	using typename list_type::reverse_iterator;
	using typename list_type::const_reverse_iterator;

private:
	class node_handle {
		friend class linked_list<T, Allocator>;
	private:
		mutable list_type list_;

	public:
		using value_type = typename list_type::value_type;
		using allocator_type = typename list_type::allocator_type;

	public:
		node_handle() = default;

		node_handle(node_handle&& nh): list_(std::move(nh.list_)) {}

		node_handle& operator=(node_handle&& nh) {
			list_ = std::move(nh.list_);
			return *this;
		}

		bool empty() const noexcept { return list_.empty(); }

		explicit operator bool() const noexcept { return !list_.empty(); }

		value_type& value() const noexcept { return list_.front(); }

		allocator_type get_allocator() const noexcept { return list_.get_allocator(); }

	protected:
		node_handle(list_type& list, const_iterator it) {
			list_.splice(list_.cbegin(), list, it);
		}

		iterator move_insert(list_type& list, const_iterator pos) {
			auto it = list_.begin();
			list.splice(pos, list_, it);
			return it;
		}
	};

public:
	using node_type = node_handle;

private:
	list_type extract_list_;

public:
	using list_type::list_type;

	using list_type::insert;

	iterator insert(const_iterator pos, node_handle&& nh) {
		return nh.move_insert(*this, pos);
	}

	using list_type::push_back;

	void push_back(node_handle&& nh) {
		nh.move_insert(*this, this->cend());
	}

	using list_type::push_front;

	void push_front(node_handle&& nh) {
		nh.move_insert(*this, this->cbegin());
	}

	node_type extract(const_iterator it) {
		if (it == this->cend()) {
			return node_type{};
		}
		return node_type(*this, it);
	}
};

} // namespace esl

#endif //ESL_LINKED_LIST_HPP

