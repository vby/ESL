
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
		list_type* list_;
		iterator it_;

	public:
		using value_type = typename list_type::value_type;
		using allocator_type = typename list_type::allocator_type;

	public:
		node_handle(): list_(nullptr) {}

		node_handle(node_handle&& nh): list_(nh.list_), it_(nh.it_) { nh.list_ = nullptr; }

		~node_handle() {
			if (list_) {
				list_->erase(it_);
			}
		}

		node_handle& operator=(node_handle&& nh) {
			list_ = nh.list_;
			it_ = nh.it_;
			nh.list_ = nullptr;
			return *this;
		}

		bool empty() const noexcept { return list_; }

		explicit operator bool() const noexcept { return list_; }

		value_type& value() const noexcept { return *it_; }

		allocator_type get_allocator() const noexcept { return list_->get_allocator(); }

	protected:
		node_handle(list_type& list, iterator it): list_(&list), it_(it) {}

		iterator insert(list_type& list, const_iterator pos) {
			list.splice(pos, *list_, it_);
			list_ = nullptr;
			return it_;
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
		return nh.insert(*this, pos);
	}

	using list_type::push_back;

	void push_back(node_handle&& nh) {
		nh.insert(*this, this->cend());
	}

	using list_type::push_front;

	void push_front(node_handle&& nh) {
		nh.insert(*this, this->cbegin());
	}

	node_type extract(const_iterator it) {
		if (it == this->cend()) {
			return node_type{};
		}
		extract_list_.splice(extract_list_.cbegin(), *this, it);
		return node_type(extract_list_, extract_list_.begin());
	}
};

} // namespace esl

#endif //ESL_LINKED_LIST_HPP

