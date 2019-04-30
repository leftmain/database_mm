#ifndef BTREE_H
#define BTREE_H

#include "btree_node.h"
#include "stack.h"
#include "command.h"

#define MAX_LEVEL 2
//#define B_DESTR_TIME

template <class T>
class BTree
{
private:
	BNode<T> * root = nullptr;
	BNode<T> * curr = nullptr;
	Stack<T> * stack = nullptr;
	int m = 0;
	bool equal_flag = false;

	BNode<T> * add(T *, BNode<T> *);
	void delete_tree(BNode<T> *);

	void get_from_left(BNode<T> *, int);
	void get_from_right(BNode<T> *, int);
	void merge(BNode<T> *, int); /** merge i and i+1 */
	void balance(BNode<T> *, int);
	T delete_element(T, BNode<T> *, bool = false);
	void process(T, int, int = STDOUT_FILENO);
	void search_subtree(Command&, BNode<T> *, int, int = STDOUT_FILENO);


public:
	BTree(int, Stack<T> * = nullptr);
	~BTree();

	void init_stack(Stack<T> *);
	int add(T);
	int read(const char *, int);
	void print(FILE * = stderr, BNode<T> * = nullptr, int = 0);
	void print(int, BNode<T> * = nullptr, int = 0);

	void goto_root() { curr = root; }
	BNode<T> * get_curr() const { return curr; }
	BNode<T> * get_root() const { return root; }
	void set_curr(BNode<T> * c) { curr = c; }
	void set_root(BNode<T> * r) { root = r; }

	void select(Command&, int = STDOUT_FILENO);
	void delete_(Command&);
	void delete_from_stack(int = -1);
	void search(Command&, BNode<T> *, int, int = STDOUT_FILENO);

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
BTree<T>::BTree(int mm, Stack<T> * st) {
	m = mm;
	stack = st;
}

template <class T>
BTree<T>::~BTree() {
#ifdef B_DESTR_TIME
	fprintf(stderr, "b-tree destr ");
	double t = clock();
#endif
	if (root) {
		delete_tree(root);
		delete root;
	}
	root = nullptr;
	curr = nullptr;
#ifdef B_DESTR_TIME
	t = (clock() - t) / CLOCKS_PER_SEC;
	fprintf(stderr, "[%.2lf]\n", t);
#endif
}

template <class T>
void BTree<T>::init_stack(Stack<T> * s) {
	stack = s;
}

template <class T>
int BTree<T>::add(T a) {
	auto res = add(&a, root);
	if (equal_flag) {
		equal_flag = false;
		return -1;
	}
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
	int i = 0;
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
			auto res = r->find_child(*a, &i);
			if (i < r->get_len() &&
			cmp(*a, r->get_data()[i]) == 0) {
				equal_flag = true;
				return root;
			}
			res = add(a, res);
			if (res == nullptr) return nullptr;
			else if (res == root) return root;
			else {
				if (r->add(*a, m, i, res)) {
					return r->cut(a, m, i, res);
				}
			}
		} else {
			i = r->add(*a, m);
			if (i == 1) return r->cut(a, m);
			else if (i == -1) equal_flag = true;
		}
	}
	return root;
}

template <class T>
void BTree<T>::print(FILE * fp, BNode<T> * r, int level) {
	int fd = fileno(fp);
	if (fd < 0) {
		perror("print() in BTree error");
		return;
	}
	print(fd, r, level);
}

template <class T>
void BTree<T>::print(int fd, BNode<T> * r, int level) {
	int i = 0;
	if (r == nullptr) r = root;
	if (r == nullptr) return;
	if (level > MAX_LEVEL) return;
	if (level == 0) dprintf(fd, "BTree\n");
	for (i = 0; i < level; i++) dprintf(fd, "    ");
	r->print();
	if (r->get_child()) {
		for (i = 0; i <= r->get_len(); i++)
			print(fd, r->get_child() + i, level + 1);
	}
	if (level == 0) dprintf(fd, "_ _ _ _ _ _ _ _ _ _\n\n");
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
	int j = 0;
	dest->get_data()[l] = r->get_data()[i];
	r->get_data()[i] = src->get_data()[0];
	if (dest->get_child()) {
		dest->get_child()[l+1] = src->get_child()[0];
		for (j = 0; j < src->get_len() - 1; j++) {
			src->get_data()[j] = src->get_data()[j+1];
			src->get_child()[j] = src->get_child()[j+1];
		}
		src->get_child()[j] = src->get_child()[j+1];
	} else {
		for (j = 0; j < src->get_len() - 1; j++)
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
	if (src_ptr.get_child()) delete [] src_ptr.get_child();
	delete [] src_ptr.get_data();
	for (int j = i; j < r->get_len() - 1; j++) {
		r->get_data()[j] = r->get_data()[j+1];
		r->get_child()[j+1] = r->get_child()[j+2];
	}
	(*r)--;
	dest->set_len(2 * m);
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
		} else if (i < r->get_len() &&
		r->get_child()[i+1].get_len() > m) {
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
	if (r == nullptr) return nullptr;
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
				r->get_data()[i] = delete_element(a, r->get_child() + i, true);
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
void BTree<T>::delete_from_stack(int g) {
	stack->goto_top();
	while (stack->get_curr()) {
		auto c = stack->get_curr()->get_data();
		if (g == -1 || c->get_group() == g)
			delete_element(c, root);
		if (root && root->get_len() == 0) {
			if (root->get_child())
				delete [] root->get_child();
			if (root->get_data()) delete [] root->get_data();
			delete root;
			root = nullptr;
		}
		stack->goto_next();
	}
	stack->goto_top();
}

template <class T>
void BTree<T>::delete_(Command& cmd) {
	search(cmd, root, SAVE_IN_STACK);
	delete_from_stack();
}

template <class T>
void BTree<T>::process(T a, int flag, int fd) {
	if (flag & PRINT) a->print(fd);
	else if (flag & SAVE_IN_STACK) stack->push(a);
}

template <class T>
void BTree<T>::search_subtree(Command& cmd, BNode<T> * r, int flag, int fd) {
	if (r == nullptr) return;
	for (int i = 0; i < r->get_len(); i++)
		if (cmd.check(*(r->get_data()[i])))
			process(r->get_data()[i], flag, fd);
	if (r->get_child()) {
		for (int i = 0; i <= r->get_len(); i++)
			search_subtree(cmd, r->get_child() + i, flag, fd);
	}
}

template <class T>
void BTree<T>::select(Command& cmd, int fd) {
	search(cmd, root, PRINT, fd);
}

template <class T>
void BTree<T>::search(Command& cmd, BNode<T> * r, int flag, int fd) {
	if (r == nullptr) return;
	int i = r->bin_search(cmd.get_record(), cmp_p);
	int j = 0;
	switch (cmd.get_c_phone()) {
		case EQ:
			while (i < r->get_len() &&
					cmp_p(r->get_data()[i], cmd.get_record()) == 0) {
				if (cmd.check(*(r->get_data()[i])))
					process(r->get_data()[i], flag, fd);
				if (r->get_child())
					search(cmd, r->get_child() + i, flag, fd);
				i++;
			}
			if (r->get_child())
				search(cmd, r->get_child() + i, flag, fd);
			break;
		case GT:
			while (i < r->get_len() &&
				cmp_p(r->get_data()[i], cmd.get_record()) == 0) i++;
			if (r->get_child())
				search(cmd, r->get_child() + i, flag, fd);
			while (i < r->get_len()) {
				if (cmd.check(*(r->get_data()[i])))
					process(r->get_data()[i], flag, fd);
				if (r->get_child())
					search_subtree(cmd, r->get_child() + i + 1, flag, fd);
				i++;
			}
			break;
		case LT:
			search(cmd, r->get_child() + i, flag, fd);
			i--;
			while (i >= 0) {
				if (cmd.check(*(r->get_data()[i])))
					process(r->get_data()[i], flag, fd);
				if (r->get_child())
					search_subtree(cmd, r->get_child() + i, flag, fd);
				i--;
			}
			break;
		case GE:
			if (r->get_child())
				search(cmd, r->get_child() + i, flag, fd);
			while (i < r->get_len()) {
				if (cmd.check(*(r->get_data()[i])))
					process(r->get_data()[i], flag, fd);
				if (r->get_child())
					search_subtree(cmd, r->get_child() + i + 1, flag, fd);
				i++;
			}
			break;
		case LE:
			j = i - 1;
			while (i < r->get_len() &&
				cmp_p(r->get_data()[i], cmd.get_record()) == 0) {
				if (cmd.check(*(r->get_data()[i])))
					process(r->get_data()[i], flag, fd);
				if (r->get_child())
					search_subtree(cmd, r->get_child() + i, flag, fd);
				i++;
			}
			if (r->get_child())
				search(cmd, r->get_child() + i, flag, fd);
			i = j;
			while (j >= 0) {
				if (cmd.check(*(r->get_data()[j])))
					process(r->get_data()[j], flag, fd);
				if (r->get_child())
					search_subtree(cmd, r->get_child() + j, flag, fd);
				j--;
			}
			break;
		default: return;
//printf("i = %d r[%d] = ", i, r->get_len()); r->print();
	}
}

#endif

