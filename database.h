#ifndef DATABASE_H
#define DATABASE_H

#include "header.h"
#include "list.h"
#include "btree.h"
#include "rbtree.h"

#define BTREE 1
#define RBTREE 1
#define DEBUG_PRINT 0

template <class T>
class Database
{
private:
	List<T> list;
	BTree<Node<T> *> btree;
	RBTree<Node<T> *> rbtree;
	Stack<Node<T> *> stack;

	int insert(T&);
	void delete_(Command&);
	void select(Command&, FILE * = stdout);

public:
	Database() : list(&stack), btree(2, &stack), rbtree(&stack) {}
	~Database() {}

	int read(FILE *);
	int apply_command(Command&, FILE * = stdout);
	void start(FILE * = stdin, FILE * = stdout);

};

template <class T>
int Database<T>::read(FILE * fp) {
	T c;
	int res = 0;
	Node<T> * node = nullptr;
	while ((res = c.read(fp)) == ALL_RIGHT) {
		if ((node = list.add(c)) == nullptr) return MEM_ERR;
		if (RBTREE) {
			res = rbtree.add(node);
			if (res == MEM_ERR) return MEM_ERR;
			else if (res == EQUAL) continue;
		}
		if (BTREE)
			if ((res = btree.add(node))) return MEM_ERR;
	}
	if (res == MEM_ERR || !feof(fp)) return res;
	return ALL_RIGHT;
}

template <class T>
int Database<T>::apply_command(Command& c, FILE * fp) {
	switch (c.get_type()) {
		case SELECT:
			select(c, fp);
			break;
		case INSERT:
			return insert(c);
		case DELETE:
			delete_(c);
			break;
		case QUIT: break;
		case STOP: break;
		case CMD_NONE: break;
	}
	return ALL_RIGHT;
}

template <class T>
void Database<T>::start(FILE * in, FILE * out) {
	Command c;
	char buf[LEN];
	double t = 0.;
	int res = 0;
if (DEBUG_PRINT) {
	list.print();
	if (BTREE) btree.print();
	if (RBTREE) rbtree.print();
}
	while (fgets(buf, LEN, in)) {
		res = c.parse(buf);
if (DEBUG_PRINT) {
	fprintf(stderr, "# ");
	c.print();
}
		if (res) apply_command(c, out);
		else if (c.get_type() == QUIT) break;
if (DEBUG_PRINT) {
	list.print();
	if (BTREE) btree.print();
	if (RBTREE) rbtree.print();
}
		c.clear();
	}
	t = (clock() - t) / CLOCKS_PER_SEC;
	fprintf(stderr, "Time: %.2lf\n", t);
}

template <class T>
int Database<T>::insert(T& a) {
	Node<T> * node = new Node<T>(a);
	int res = 0;
	if (BTREE) {
		if (res == 0) res = btree.add(node);
		else {
			delete node;
			return 0;
		}
	}
	if (RBTREE) {
		if (res == 0) res = rbtree.add(node);
		else {
			delete node;
			return 0;
		}
	}
	if (res == 0) list.add_node(node);
	else delete node;

	return 0;
}

template <class T>
void Database<T>::delete_(Command& cmd) {
	if (BTREE && cmd.get_c_phone() != COND_NONE &&
			cmd.get_c_phone() != NE &&
			cmd.get_oper() != OR &&
			cmd.get_oper1() != OR) {
		btree.delete_(cmd);
		if (RBTREE) rbtree.delete_from_stack();
		list.delete_from_stack();
	} else if (RBTREE && cmd.get_c_name() != COND_NONE &&
				cmd.get_c_name() != NE &&
				cmd.get_c_name() != LIKE &&
				cmd.get_oper() != OR &&
				cmd.get_oper1() != OR) {
		rbtree.delete_(cmd);
		if (BTREE) btree.delete_from_stack();
		list.delete_from_stack();
	} else {
		list.delete_(cmd);
		btree.delete_from_stack();
		rbtree.delete_from_stack();
	}
	stack.delete_stack();
}

template <class T>
void Database<T>::select(Command& cmd, FILE * fp) {
	if (BTREE && cmd.get_c_phone() != COND_NONE &&
			cmd.get_c_phone() != NE &&
			cmd.get_oper() != OR &&
			cmd.get_oper1() != OR) {
		btree.select(cmd, fp);
	} else if (RBTREE && cmd.get_c_name() != COND_NONE &&
				cmd.get_c_name() != NE &&
				cmd.get_c_name() != LIKE &&
				cmd.get_oper() != OR &&
				cmd.get_oper1() != OR) {
		rbtree.select(cmd);
	} else {
		list.select(cmd);
	}
}

#endif

