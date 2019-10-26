#ifndef RYULIB_WAITFREELIST_HPP
#define RYULIB_WAITFREELIST_HPP

using namespace std;

class Node {
public:
	Node(Node *aleft, void *aitem, Node *aright)
		:left(aleft), item(aitem), right(aright)
	{
	}

	Node *left;
	void *item;
	Node *right;
};

template <typename T>
class WaitFreeList {
public:
	WaitFreeList()
		: count_(0), header_(nullptr)
	{
	}

	void clear()
	{
		// TODO: release

		header_ = nullptr;
		count_ = 0;
	}

	void add(T *item)
	{
		if (header_ == nullptr) {
			header_ = new Node(nullptr, item, nullptr);
			return;
		}

		auto node = new Node(header_, item, nullptr);
		header_->right = node;
		header_ = node;

		count_++;
	}

	void remove(Node *node)
	{
		auto node_right = node->right;
		auto node_left = node->left;

		if (node_right != nullptr) {
			node_right->left = node_left;
		}

		if (node_left != nullptr) {
			node_left->right = node_right;
		}

		if (node == header_) {
			header_ = node_left;
		}

		// TODO: release
	}

	Node* get_first()
	{
		return header_;
	}

	Node* get_next(Node* current)
	{
		return current->left;
	}

	T get_item(Node* node)
	{
		T *item = (T *) node->item;
		return *item;
	}

private:
	int count_;
	Node *header_;
};


#endif  // RYULIB_WAITFREELIST_HPP