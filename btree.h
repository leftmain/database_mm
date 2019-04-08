#ifndef BTREE_H
#define BTREE_H

#include "btree_node.h"
#include "stack.h"
#include "command.h"

#define MAX_LEVEL 5
#define SAVE_IN_STACK	(1 << 0)
#define PRINT        	(1 << 1)

template <class T>
class BTree
{
private:
	Stack<T> stack;
	BNode<T> * root = nullptr;
	BNode<T> * curr = nullptr;
	int m = 0;

	BNode<T> * add(T *, BNode<T> *);
	void delete_tree(BNode<T> *);

	void get_from_left(BNode<T> *, int);
	void get_from_right(BNode<T> *, int);
	void merge(BNode<T> *, int); /** merge i and i+1 */
	void balance(BNode<T> *, int);
	T delete_element(T, BNode<T> *, bool = false);
	void process(T, int);

	void select_subtree(Command&, FILE * = stdout, \
								BNode<T> * = nullptr);

public:
	BTree(int);
	~BTree();

	int add(T);
	int read(const char *, int);
	void print(FILE * = stderr, BNode<T> * = nullptr, int = 0);

	void goto_root() { curr = root; }
	BNode<T> * get_curr() const { return curr; }
	BNode<T> * get_root() const { return root; }
	void set_curr(BNode<T> * c) { curr = c; }
	void set_root(BNode<T> * r) { root = r; }
	Stack<T> * get_stack() { return &stack; }

	void select(Command&, FILE * = stdout);
	Stack<T> * delete_(Command&);
	void search(Command&, BNode<T> *, int, FILE * = stdout);

};

template <class T>
void BTree<T>::delete_tree(BNode<T> * r) {
	if (r == nullptr) return;
	if (r->get_child()) {
		for (int i = 0; i <= r->get_len(); i++) {
			delete_tree(r->get_child() + i);
		}
		delete [] r->get_child();
	}
	delete [] r->get_data();
}

template <class T>
BTree<T>::BTree(int mm) {
	m = mm;
}

template <class T>
BTree<T>::~BTree() {
	if (root) {
		delete_tree(root);
		delete root;
	}
	root = nullptr;
	curr = nullptr;
}

template <class T>
int BTree<T>::add(T a) {
	auto res = add(&a, root);
	if (res == nullptr) return 1;
	if (res != root) {
		auto node = new BNode<T>;
		if (!node) return 1;
		if (node->alloc_data(m)) {
			delete node;
			return 1;
		}
		if (node->alloc_child(m)) {
			delete [] node->get_data();
			delete node;
			return 1;
		}
		node->get_data()[0] = a;
		node->set_len(1);
		node->get_child()[0] = *root;
		node->get_child()[1] = *res;
		delete root;
		delete res;
		root = node;
	} 
	return 0;
}

/**
 * returns:
 * root: all is good
 * nullptr: memory allocation error
 * some_node: cut node, ...
 */
template <class T>
BNode<T> * BTree<T>::add(T * a, BNode<T> * r) {
	if (!root) {
		root = new BNode<T>;
		if (!root) return nullptr;
		if (root->alloc_data(m)) {
			delete root;
			return nullptr;
		}
		curr = root;
		root->get_data()[0] = *a;
		root->set_len(1);
	} else {
		if (r->get_child()) {
			int i = 0;
			BNode<T> * res = add(a, r->find_child(*a, &i));
			if (!res) return nullptr;
			else if (res == root) return root;
			else {
				if (r->add(*a, m, i, res)) {
					return r->cut(a, m, i, res);
				}
			}
		} else if (r->add(*a, m)) {
			return r->cut(a, m);
		}
	}
	return root;
}

template <class T>
void BTree<T>::print(FILE * fp, BNode<T> * r, int level) {
	int i = 0;
	if (r == nullptr) r = root;
	if (r == nullptr) return;
	if (level > MAX_LEVEL) return;
	for (i = 0; i < level; i++) fprintf(fp, "    ");
	r->print();
	if (r->get_child()) {
		for (i = 0; i <= r->get_len(); i++)
			print(fp, r->get_child() + i, level + 1);
	}
}

template <class T>
void BTree<T>::select_subtree(Command& cmd, FILE * fp, BNode<T> * r) {
	if (r == nullptr) return;
	for (int i = 0; i < r->get_len(); i++)
		if (cmd.check(*r->get_data()[i]->get_T()))
			r->get_data()[i]->print(fp);
	if (r->get_child()) {
		for (int i = 0; i <= r->get_len(); i++)
			select_subtree(cmd, fp, r->get_child() + i);
	}
}

template <class T>
void BTree<T>::select(Command& cmd, FILE * fp) {
	search(cmd, root, PRINT, fp);
}

template <class T>
void BTree<T>::get_from_left(BNode<T> * r, int i) {
	auto dest = r->get_child() + i;
	auto src = r->get_child() + i - 1;
	int l = dest->get_len();
	if (dest->get_child()) {
		dest->get_child()[l+1] = dest->get_child()[l];
		for (int j = l; j > 0; j--) {
			dest->get_data()[j] = dest->get_data()[j-1];
			dest->get_child()[j] = dest->get_child()[j-1];
		}
		dest->get_child()[0] = \
			src->get_child()[src->get_len()];
	} else {
		for (int j = l; j > 0; j--)
			dest->get_data()[j] = dest->get_data()[j-1];
	}
	dest->get_data()[0] = r->get_data()[i-1];
	r->get_data()[i-1] = src->get_data()[src->get_len()-1];
	(*src)--;
	(*dest)++;
}

template <class T>
void BTree<T>::get_from_right(BNode<T> * r, int i) {
	auto dest = r->get_child() + i;
	auto src = r->get_child() + i + 1;
	int l = dest->get_len();
	dest->get_data()[l] = r->get_data()[i];
	r->get_data()[i] = src->get_data()[0];
	if (dest->get_child()) {
		dest->get_child()[l+1] = src->get_child()[0];
		for (int j = 0; j < src->get_len()-1; j++) {
			src->get_data()[j] = src->get_data()[j+1];
			src->get_child()[j] = src->get_child()[j+1];
		}
	} else {
		for (int j = 0; j < src->get_len()-1; j++)
			src->get_data()[j] = src->get_data()[j+1];
	}
	(*src)--;
	(*dest)++;
}

template <class T>
void BTree<T>::merge(BNode<T> * r, int i) {
	auto dest = r->get_child() + i;
	auto src = r->get_child() + i + 1;
	auto src_ptr = *src;
	int l = dest->get_len();
	dest->get_data()[l] = r->get_data()[i];
	if (dest->get_child()) {
		dest->get_child()[l+1] = src->get_child()[0];
		for (int j = l + 1; j < 2 * m; j++) {
			dest->get_data()[j] = src->get_data()[j-l-1];
			dest->get_child()[j+1] = src->get_child()[j-l];
		}
	} else {
		for (int j = l + 1; j < 2 * m; j++)
			dest->get_data()[j] = src->get_data()[j-l-1];
	}
	for (int j = i; j < r->get_len() - 1; j++) {
		r->get_data()[j] = r->get_data()[j+1];
		r->get_child()[j+1] = r->get_child()[j+2];
	}
	(*r)--;
	dest->set_len(2 * m);
	if (src->get_child()) delete [] src_ptr.get_child();
	delete [] src_ptr.get_data();
	if (r == root && r->get_len() == 0) {
		auto c = *dest;
		delete [] r->get_data();
		delete [] r->get_child();
		*root = c;
	}
}

template <class T>
void BTree<T>::balance(BNode<T> * r, int i) {
	auto child = r->get_child() + i;
	if (child->get_len() < m) {
		if (i > 0 && r->get_child()[i-1].get_len() > m) {
			get_from_left(r, i);
		} else if (i < r->get_len() && r->get_child()[i+1].get_len() > m) {
			get_from_right(r, i);
		} else {
			if (i > 0) merge(r, i-1);
			else merge(r, i);
		}
	}
}

template <class T>
T BTree<T>::delete_element(T a, BNode<T> * r, bool found) {
	T ret = nullptr;
	int i = 0;
	if (found) {
		if (r->get_child()) {
			i = r->get_len();
			ret = delete_element(a, r->get_child() + i, true);
		} else {
			(*r)--;
			return r->get_data()[r->get_len()];
		}
	} else {
		i = r->bin_search(a);
		if (i < r->get_len() && a == r->get_data()[i]) {
			ret = r->get_data()[i];
			if (r->get_child()) {
				print();
				r->get_data()[i] = delete_element(a, r->get_child() + i, true);
				print();
			} else {
				for (int j = i; j < r->get_len() - 1; j++)
					r->get_data()[j] = r->get_data()[j+1];
				(*r)--;
				return ret;
			}
		} else {
			if (r->get_child()) ret = delete_element(a, r->get_child() + i);
			else return nullptr;
		}
	}
	balance(r, i);
	return ret;
}

template <class T>
Stack<T> * BTree<T>::delete_(Command& cmd) {
	search(cmd, root, SAVE_IN_STACK);
	stack.goto_top();
	while (stack.get_curr()) {
		delete_element(stack.get_curr()->get_data(), root);
		stack.goto_next();
	}
	stack.goto_top();
	return &stack;
}

template <class T>
void BTree<T>::process(T a, int flag) {
	if (flag & PRINT) a->print();
	else if (flag & SAVE_IN_STACK) stack.push(a);
}

template <class T>
void BTree<T>::search(Command& cmd, BNode<T> * r, int flag, FILE * fp) {
	int i = r->bin_search(cmd.get_record(), cmp_p);
	int compare = 0;
	switch (cmd.get_c_phone()) {
		case EQ:
			if (i == r->get_len()) {
				if (r->get_child()) search(cmd, r->get_child() + i, flag, fp);
				break;
			}
			compare = cmp_p(cmd.get_record(), r->get_data()[i]);
			if (compare == 0) {
				if (cmd.check(*(r->get_data()[i])))
					process(r->get_data()[i], flag);
				if (r->get_child())
					search(cmd, r->get_child() + i, flag, fp);
				i++;
				while (i < r->get_len() &&
						cmp_p(cmd.get_record(), r->get_data()[i]) == 0) {
					if (cmd.check(*(r->get_data()[i])))
						process(r->get_data()[i], flag);
					if (r->get_child())
						search(cmd, r->get_child() + i, flag, fp);
					i++;
				}
				if (r->get_child())
					search(cmd, r->get_child() + i, flag, fp);
			} else if (compare < 0) {
				if (r->get_child())
					search(cmd, r->get_child() + i, flag, fp);
			} else {
				if (r->get_child())
					search(cmd, r->get_child() + i + 1, flag, fp);
			}
			break;
		case GT:
			if (compare == 0) {
			}
			break;
		case LT:
			break;
		case GE:
			break;
		case LE:
			break;
		default: return;
	}
}

#endif
