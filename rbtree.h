#ifndef RBTREE_H
#define RBTREE_H

#include "rbtree_node.h"
#include "stack.h"
#include "command.h"

#define MAX_RB_LEVEL 3
//#define RB_DESTR_TIME

template <class T>
class RBTree
{
private:
	RBNode<T> * root = nullptr;
	RBNode<T> * curr = nullptr;
	Stack<T> * stack;

	void delete_tree(RBNode<T> *);
	int balance(RBNode<T> *, RBNode<T> *, RBNode<T> *);
	void rotate_right(RBNode<T> *);
	void rotate_left(RBNode<T> *);
	bool balance_d(RBNode<T> *, RBNode<T> *, RBNode<T> *);
	RBNode<T> * delete_element(T, RBNode<T> *, bool = false);
	void process(T, int, int = STDOUT_FILENO);
	void search_subtree(Command&, RBNode<T> *, int, int = STDOUT_FILENO);
	void search(Command&, RBNode<T> *, int, int = STDOUT_FILENO);

public:
	RBTree(Stack<T> * st = nullptr) { stack = st; }
	~RBTree();

	void init_stack(Stack<T> *);
	int add(T, RBNode<T> * = nullptr);
	void print(FILE * = stdout, RBNode<T> * = nullptr, int = 0);
	void print(int, RBNode<T> * = nullptr, int = 0);
	void delete_from_stack(int = -1);
	void delete_(Command&);
	void select(Command&, int = STDOUT_FILENO);

	RBNode<T> * get_root() const { return root; }
	RBNode<T> * get_curr() const { return curr; }

};

template <class T>
RBTree<T>::~RBTree() {
#ifdef RB_DESTR_TIME
	fprintf(stderr, "rb-tree destr ");
	double t = clock();
#endif
	delete_tree(root);
/*
	RBNode<T> * m[100];
	m[0] = root;
	int l = 1;
	int k = 0;
	while (l > 0) {
		if (l == 99) {
			fprintf(stderr, "max deep 100\n");
			break;
		}
		if (m[l-1]->get_right()) {
			m[l] = m[l-1]->get_right();
			m[l-1]->set_right(nullptr);
			l++;
		} else if (m[l-1]->get_left()) {
			m[l] = m[l-1]->get_left();
			m[l-1]->set_left(nullptr);
			l++;
		} else {
			delete m[l-1];
			l--;
			k++;
			if (k % 100000 == 0)
			fprintf(stderr, "# k = %d\n", k);
		}
	}
*/
#ifdef RB_DESTR_TIME
	t = (clock() - t) / CLOCKS_PER_SEC;
	fprintf(stderr, "[%.2lf]\n", t);
#endif
}

template <class T>
void RBTree<T>::init_stack(Stack<T> * s) {
	stack = s;
}

template <class T>
void RBTree<T>::delete_tree(RBNode<T> * r) {
	if (r == nullptr) return;
	if (r->get_right()) delete_tree(r->get_right());
	if (r->get_left()) delete_tree(r->get_left());
	delete r;
/*
static int k = 0;
k++;
if (k % 100000 == 0)
	fprintf(stderr, "# k = %d\n", k);
*/
}

/** a - new child, b - old */
template <class T>
int RBTree<T>::balance(RBNode<T> * r, RBNode<T> * a, RBNode<T> * b) {
	if (a->is_red()) {
		if (r->is_red()) return -1;
		if (b && b->is_red()) {
			a->set_color(BLACK);
			b->set_color(BLACK);
			r->set_color(RED);
			if (r == root) {
				r->set_color(BLACK);
				return ALL_RIGHT;
			}
			return -1;
		} else {
			if (a == r->get_left()) {
				if (a->get_right() && a->get_right()->is_red())
					rotate_left(a);
				rotate_right(r);
			} else {
				if (a->get_left() && a->get_left()->is_red())
					rotate_right(a);
				rotate_left(r);
			}
		}
	}
	if (r == root) {
		r->set_color(BLACK);
		return ALL_RIGHT;
	}
	return ALL_RIGHT;
}

template <class T>
void RBTree<T>::rotate_right(RBNode<T> * a) {
	auto b = a->get_left();
	RBNode<T> tmp;
	tmp.get(a);
	a->get(b);
	b->get(&tmp);
/*
	auto tmp = *a;
	*a = *b;
	*b = tmp;
*/
	auto tmp2 = b->get_right();
	b->set_right(a->get_right());
	a->set_right(b);
	a->set_left(b->get_left());
	b->set_left(tmp2);
}

template <class T>
void RBTree<T>::rotate_left(RBNode<T> * a) {
	auto b = a->get_right();
	RBNode<T> tmp;
	tmp.get(a);
	a->get(b);
	b->get(&tmp);
/*
	auto tmp = *a;
	*a = *b;
	*b = tmp;
*/
	auto tmp2 = b->get_left();
	b->set_left(a->get_left());
	a->set_left(b);
	a->set_right(b->get_right());
	b->set_right(tmp2);
}

template <class T>
bool RBTree<T>::balance_d(RBNode<T> * r, RBNode<T> * a, RBNode<T> * b) {
	if (a == nullptr && b == nullptr) {
		r->set_color(BLACK);
		return false;
	}
	if (b == nullptr) return false;
	if (b == r->get_right()) {
		if (b->is_red()) {
			rotate_left(r);
			if (a == nullptr) return false;
			b = r->get_right();
			a = r->get_left();
		}
		if (b == nullptr) return false;
		if (b->black_right() && b->black_left()) {
			b->set_color(RED);
			color c = r->get_color();
			r->set_color(BLACK);
			if (r == root || c == RED) return false;
			return true;
		}
		if (b->black_right()) rotate_right(b);
		if (!b->black_right()) {
			b->get_right()->set_color(BLACK);
			rotate_left(r);
			if (r == root) r->set_color(BLACK);
			return false;
		}
	} else {
		if (b->is_red()) {
			rotate_right(r);
			if (a == nullptr) return false;
			a = r->get_right();
			b = r->get_left();
		}
		if (b == nullptr) return false;
		if (b->black_right() && b->black_left()) {
			b->set_color(RED);
			color c = r->get_color();
			r->set_color(BLACK);
			if (r == root || c == RED) return false;
			return true;
		}
		if (b->black_left()) rotate_left(b);
		if (!b->black_left()) {
			b->get_left()->set_color(BLACK);
			rotate_right(r);
			if (r == root) r->set_color(BLACK);
			return false;
		}
	}
	perror("rb balance_d error\n");
	return true;
}

template <class T>
RBNode<T> * RBTree<T>::delete_element(T a, RBNode<T> * r, bool found) {
	int res = 0;
	auto tmp = r;
	static bool need_balance = false;
	if (r == nullptr) return nullptr;
	if (found) {
		if (r->get_left()) {
			tmp = delete_element(a, r->get_left(), found);
			if (tmp == r->get_left()) {
				r->set_left(tmp->get_right());
				if (tmp->is_black()) {
					need_balance = balance_d(r, r->get_left(), r->get_right());
				}
			}
			return tmp;
		}
		return r;
	} else {
		res = cmp_npg(a, r->get_data());
		if (res == 0) {
			if (r->get_right()) {
				if (r->get_left()) {
					tmp = delete_element(a, r->get_right(), true);
					if (tmp == r->get_right()) {
						r->set_right(tmp->get_right());
//						*r = *tmp;
						r->get(tmp);
						if (tmp->is_black()) {
							need_balance = \
								balance_d(r, r->get_right(), r->get_left());
						}
					} else {
//						*r = *tmp;
						r->get(tmp);
					}
					delete tmp;
				} else {
					if (r->is_red()) {
						tmp = r->get_right();
						delete r;
						return tmp;
					} else {
						// it's black by rules
						tmp = r->get_right();
						delete r;
						if (tmp == nullptr) need_balance = true;
						else tmp->set_color(BLACK);
						return tmp;
					}
				}
			} else {
				if (r->get_left()) {
					// than it's black
					tmp = r->get_left();
					delete r;
					if (tmp == nullptr) need_balance = true;
					else tmp->set_color(BLACK);
					return tmp;
				} else {
					if (r->is_black()) need_balance = true;
					delete r;
					return nullptr;
				}
			}
		} else if (res > 0) {
			if (r->get_right()) {
				r->set_right(delete_element(a, r->get_right()));
				if (need_balance) {
					need_balance = balance_d(r, r->get_right(), r->get_left());
				}
			}
		} else {
			if (r->get_left()) {
				r->set_left(delete_element(a, r->get_left()));
				if (need_balance) {
					need_balance = balance_d(r, r->get_left(), r->get_right());
				}
			}
		}
	}
	return r;
}

template <class T>
void RBTree<T>::process(T a, int flag, int fd) {
	if (flag & PRINT) a->print(fd);
	else if (flag & SAVE_IN_STACK) stack->push(a);
}

template <class T>
void RBTree<T>::search_subtree(Command& cmd, RBNode<T> * r, int flag, int fd) {
	if (cmd.check(*r->get_data()))
		process(r->get_data(), flag, fd);
	if (r->get_right())
		search_subtree(cmd, r->get_right(), flag, fd);
	if (r->get_left())
		search_subtree(cmd, r->get_left(), flag, fd);
}

template <class T>
void RBTree<T>::search(Command& cmd, RBNode<T> * r, int flag, int fd) {
	if (r == nullptr) return;
	int i = cmp_n(r->get_data(), cmd.get_record());
	switch (cmd.get_c_name()) {
		case EQ:
			if (i == 0 && cmd.check(*r->get_data()))
				process(r->get_data(), flag, fd);
			if (i <= 0 && r->get_right())
				search(cmd, r->get_right(), flag, fd);
			if (i >= 0 && r->get_left())
				search(cmd, r->get_left(), flag, fd);
			break;
		case GT:
			if (i > 0) {
				if (cmd.check(*r->get_data()))
					process(r->get_data(), flag, fd);
				if (r->get_right())
					search_subtree(cmd, r->get_right(), flag, fd);
				if (r->get_left())
					search(cmd, r->get_left(), flag, fd);
			} else if (r->get_right())
				search(cmd, r->get_right(), flag, fd);
			break;
		case LT:
			if (i < 0) {
				if (cmd.check(*r->get_data()))
					process(r->get_data(), flag, fd);
				if (r->get_left())
					search_subtree(cmd, r->get_left(), flag, fd);
				if (r->get_right())
					search(cmd, r->get_right(), flag, fd);
			} else if (r->get_left())
				search(cmd, r->get_left(), flag, fd);
			break;
		case GE:
			if (i >= 0) {
				if (cmd.check(*r->get_data()))
					process(r->get_data(), flag, fd);
				if (r->get_right())
					search_subtree(cmd, r->get_right(), flag, fd);
				if (r->get_left())
					search(cmd, r->get_left(), flag, fd);
			} else if (r->get_right())
				search(cmd, r->get_right(), flag, fd);
			break;
		case LE:
			if (i <= 0) {
				if (cmd.check(*r->get_data()))
					process(r->get_data(), flag, fd);
				if (r->get_left())
					search_subtree(cmd, r->get_left(), flag, fd);
				if (r->get_right())
					search(cmd, r->get_right(), flag, fd);
			} else if (r->get_left())
				search(cmd, r->get_left(), flag, fd);
			break;
		case LIKE:
			break;
		default: return;
	}
}

template <class T>
int RBTree<T>::add(T a, RBNode<T> * r) {
	int res = 0;
	auto node = r;

	if (r == nullptr) r = root;
	if (r == nullptr) {
		root = new RBNode<T>(a, BLACK);
		if (root == nullptr) return MEM_ERR;
		curr = root;
		return ALL_RIGHT;
	}

	res = cmp_npg(a, r->get_data());
	if (res > 0) {
		if (r->get_right()) {
			res = add(a, r->get_right());
			if (res == -1)
				return balance(r, r->get_right(), r->get_left());
			else if (res == EQUAL) return res;
		} else {
			node = new RBNode<T>(a, RED);
			if (node == nullptr) return MEM_ERR;
			r->set_right(node);
			if (r->is_red()) return -1;
		}
	} else if (res < 0) {
		if (r->get_left()) {
			res = add(a, r->get_left());
			if (res == -1)
				return balance(r, r->get_left(), r->get_right());
			else if (res == EQUAL) return res;
		} else {
			node = new RBNode<T>(a, RED);
			if (node == nullptr) return MEM_ERR;
			r->set_left(node);
			if (r->is_red()) return -1;
		}
	} else return EQUAL;

	return ALL_RIGHT;
}

template <class T>
void RBTree<T>::print(FILE * fp, RBNode<T> * r, int level) {
	int fd = fileno(fp);
	if (fd < 0) {
		perror("print() in RBTree error");
		return;
	}
	print(fd, r, level);
}

template <class T>
void RBTree<T>::print(int fd, RBNode<T> * r, int level) {
	if (r == nullptr) r = root;
	if (r == nullptr) return;
	if (level > MAX_RB_LEVEL) return;
	if (level == 0) dprintf(fd, "RBTree\n");
	if (r->get_right()) print(fd, r->get_right(), level + 1);
	for (int i = 0; i < level; i++) dprintf(fd, "    ");
	r->print(fd);
	if (r->get_left()) print(fd, r->get_left(), level + 1);
	if (level == 0) dprintf(fd, "_ _ _ _ _ _ _ _ _ _\n\n");
}

template <class T>
void RBTree<T>::delete_from_stack(int g) {
	stack->goto_top();
	while (stack->get_curr()) {
		if (g == -1 || stack->get_curr()->get_data()->get_group() == g)
			root = delete_element(stack->get_curr()->get_data(), root);
		stack->goto_next();
	}
	stack->goto_top();
}

template <class T>
void RBTree<T>::delete_(Command& cmd) {
	search(cmd, root, SAVE_IN_STACK);
	delete_from_stack();
}

template <class T>
void RBTree<T>::select(Command& cmd, int fd) {
	search(cmd, root, PRINT, fd);
}

#endif

