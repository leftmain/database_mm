#ifndef GROUP_H
#define GROUP_H

#include "header.h"
#include "list.h"
#include "btree.h"
#include "rbtree.h"

#define G_BTREE 1
#define G_RBTREE 1

template <class T>
class Group
{
private:
	List<T> list;
	BTree<Node<T> *> btree;
	RBTree<Node<T> *> rbtree;
	Stack<Node<T> *> * stack;

public:
	Group() : btree(5) { }
	~Group() {}

	void init_stack(Stack<Node<T> *> *);
	int add(FILE *);
	Node<T> * add(T&);
	Node<T> * insert(T&);
	void delete_(Command&);
	void delete_from_stack(int);
	void select(Command&, int = STDOUT_FILENO);
	void print(int, FILE * = stderr);
	void print(int, int);

};

template <class T>
void Group<T>::init_stack(Stack<Node<T> *> * s) {
	stack = s;
	list.init_stack(s);
	btree.init_stack(s);
	rbtree.init_stack(s);
}

template <class T>
Node<T> * Group<T>::add(T& e) {
	Node<T> * node = nullptr;
	int res = 0;
	if ((node = list.add(e)) == nullptr)
		return nullptr;
	if (G_RBTREE) {
		res = rbtree.add(node);
		if (res == MEM_ERR || res == EQUAL)
			return nullptr;
	}
	if (G_BTREE)
		if ((res = btree.add(node))) return nullptr;
	return node;
}

template <class T>
Node<T> * Group<T>::insert(T& a) {
	int res = 0;
	Node<T> * node = new Node<T>(a);
	if (node == nullptr) return nullptr;
//	node->move(a);

	if (G_BTREE) {
		if (res == 0) res = btree.add(node);
		else {
			delete node;
			return nullptr;
		}
	}
	if (G_RBTREE) {
		if (res == 0) res = rbtree.add(node);
		else {
			delete node;
			return nullptr;
		}
	}
	if (res == 0) {
		if (G_BTREE || G_RBTREE)
			list.add_node(node);
		else if (list.insert(node) == nullptr) {
			delete node;
			return nullptr;
		}
	} else {
		delete node;
		return nullptr;
	}

	return node;
}

template <class T>
void Group<T>::delete_(Command& cmd) {
	if (G_BTREE && cmd.get_c_phone() != COND_NONE &&
			cmd.get_c_phone() != NE &&
			cmd.get_oper() != OR &&
			cmd.get_oper1() != OR) {
		btree.delete_(cmd);
		if (G_RBTREE) rbtree.delete_from_stack();
		list.delete_from_stack();
	} else if (G_RBTREE && cmd.get_c_name() != COND_NONE &&
				cmd.get_c_name() != NE &&
				cmd.get_c_name() != LIKE &&
				cmd.get_oper() != OR &&
				cmd.get_oper1() != OR) {
		rbtree.delete_(cmd);
		if (G_BTREE) btree.delete_from_stack();
		list.delete_from_stack();
	} else {
		list.delete_(cmd);
		if (G_BTREE) btree.delete_from_stack();
		if (G_RBTREE) rbtree.delete_from_stack();
	}
}

template <class T>
void Group<T>::delete_from_stack(int g) {
	list.delete_from_stack(g);
	if (G_BTREE) btree.delete_from_stack(g);
	if (G_RBTREE) rbtree.delete_from_stack(g);
}

template <class T>
void Group<T>::select(Command& cmd, int fd) {
	if (G_BTREE && cmd.get_c_phone() != COND_NONE &&
			cmd.get_c_phone() != NE &&
			cmd.get_oper() != OR &&
			cmd.get_oper1() != OR) {
		btree.select(cmd, fd);
	} else if (G_RBTREE && cmd.get_c_name() != COND_NONE &&
				cmd.get_c_name() != NE &&
				cmd.get_c_name() != LIKE &&
				cmd.get_oper() != OR &&
				cmd.get_oper1() != OR) {
		rbtree.select(cmd, fd);
	} else {
		list.select(cmd, fd);
	}
}

template <class T>
void Group<T>::print(int g, FILE * fp) {
	int fd = fileno(fp);
	if (fd < 0) {
		perror("print() in Group error");
		return;
	}
	print(g, fd);
}

template <class T>
void Group<T>::print(int g, int fd) {
	if (list.get_size() == 0) return;
	dprintf(fd, "Group %d [%d] ------------------\\\n", g, list.get_size());
	list.print(fd);
	btree.print(fd);
	rbtree.print(fd);
	dprintf(fd, "--------------------------------/\n");
}

#endif

