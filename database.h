#ifndef DATABASE_H
#define DATABASE_H

#include "header.h"
#include "group.h"

#define BTREE 1
#define RBTREE 1
#define DEBUG_PRINT 0

template <class T>
class Database
{
private:
	static const int min_group = 100;
	static const int max_group = 600;

	Group<T> groups[max_group - min_group];
	BTree<Node<T> *> btree;
	RBTree<Node<T> *> rbtree;
	Stack<Node<T> *> stack;

	void init_stack();
	int group_hash(T&);

	int insert(T&);
	void delete_(Command&);
	void select(Command&, int = STDOUT_FILENO);
	void rm_group_from_stack();

public:
	Database() : btree(5) { init_stack(); }
	~Database() {}

	int read(FILE *);
	int apply_command(Command&, FILE * = stdout);
	int apply_command(Command&, int);
	void start(FILE * = stdin, FILE * = stdout);
	void dstart(int, int);
	void print(FILE * = stderr);
	void print(int);

};

template <class T>
void Database<T>::init_stack() {
	for (int i = 0; i < max_group - min_group; i++)
		groups[i].init_stack(&stack);
	btree.init_stack(&stack);
	rbtree.init_stack(&stack);
}

template <class T>
int Database<T>::group_hash(T& a) {
	int g = a.get_group();
	if (g < min_group || g > max_group) {
//		fprintf(stderr, "group %d???\n", g);
		return -1;
	}
	return g - min_group;
}

template <class T>
int Database<T>::read(FILE * fp) {
	T e;
	int res = 0;
	Node<T> * node = nullptr;
	fprintf(stderr, "reading ");
	double t = clock();
	while ((res = e.read(fp)) == ALL_RIGHT) {
		if ((res = group_hash(e)) < 0) return DATA_ERR;
		if ((node = groups[res].add(e)) == nullptr) continue;
		if (RBTREE) {
			res = rbtree.add(node);
			if (res == MEM_ERR) return MEM_ERR;
			else if (res == EQUAL) continue;
		}
		if (BTREE)
			if ((res = btree.add(node))) return MEM_ERR;
	}
	if (res == MEM_ERR || !feof(fp)) return res;
	t = (clock() - t) / CLOCKS_PER_SEC;
	fprintf(stderr, "[%.2lf]\n", t);
	return ALL_RIGHT;
}

template <class T>
int Database<T>::apply_command(Command& c, FILE * fp) {
	int fd = fileno(fp);
	if (fd < 0) {
		perror("apply_command() error");
		return -1;
	}
	return apply_command(c, fd);
}

template <class T>
int Database<T>::apply_command(Command& c, int fd) {
	switch (c.get_type()) {
		case SELECT:
			select(c, fd);
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
	int fd_in = fileno(in);
	int fd_out = fileno(out);
	if (fd_in < 0 || fd_out < 0) {
		perror("fileno() error");
		return;
	}
	dstart(fd_in, fd_out);
}

template <class T>
void Database<T>::dstart(int in, int out) {
	Command c;
	char buf[LEN];
	double t = clock();
	int res = 0;
	if (DEBUG_PRINT) {
		for (int i = 0; i < max_group - min_group; i++)
			groups[i].print(i + min_group);
		if (BTREE) btree.print();
		if (RBTREE) rbtree.print();
	}
	FILE * in_fp = fdopen(in, "r");
	if (in_fp == nullptr) {
		perror("fdopen() in dstart error");
		return;
	}
	while (fgets(buf, LEN, in_fp)) {
//getchar();
		res = c.parse(buf);
		if (DEBUG_PRINT) {
//printf("-----------------------------------------------------------\
----------------------------------------------------------------\n");
			fprintf(stderr, "# ");
			c.print();
		}
//getchar();
		if (res) apply_command(c, out);
		else if (c.get_type() == QUIT) break;
		if (DEBUG_PRINT) {
			print();
		}
		c.clear();
	}
	t = (clock() - t) / CLOCKS_PER_SEC;
	fprintf(stderr, "commands [%.2lf]\n", t);
}

template <class T>
int Database<T>::insert(T& a) {
	Node<T> * node = nullptr;
	int res = group_hash(a);

	if (res < 0) return DATA_ERR;
	node = groups[res].insert(a);
	if (node == nullptr) return ALL_RIGHT;

	res = 0;
	if (BTREE) {
		if (res == 0) res = btree.add(node);
		else {
			fprintf(stderr, "imp_err_1\n");
			delete node;
			return EQUAL;
		}
	}
	if (RBTREE) {
		if (res == 0) res = rbtree.add(node);
		else {
			fprintf(stderr, "imp_err_2\n");
			delete node;
			return EQUAL;
		}
	}

	return ALL_RIGHT;
}

template <class T>
void Database<T>::delete_(Command& cmd) {
	int g = 0;
	if (cmd.get_c_group() != COND_NONE
			&& cmd.get_c_group() != NE
			&& cmd.get_oper() != OR
			&& cmd.get_oper1() != OR
			&& (g = group_hash(cmd)) > 0) {
if (DEBUG_PRINT) printf("### delete from groups\n");
		switch (cmd.get_c_group()) {
			case EQ:
				groups[g].delete_(cmd);
				break;
			case GT:
				for (int i = g + 1; i < max_group - min_group; i++)
					groups[i].delete_(cmd);
				break;
			case GE:
				for (int i = g; i < max_group - min_group; i++)
					groups[i].delete_(cmd);
				break;
			case LT:
				for (int i = g - 1; i >= 0; i--)
					groups[i].delete_(cmd);
				break;
			case LE:
				for (int i = g; i >= 0; i--)
					groups[i].delete_(cmd);
				break;
			default:
				fprintf(stderr, "imp_err_3\n");
				break;
		}
		if (BTREE) btree.delete_from_stack();
		if (RBTREE) rbtree.delete_from_stack();
	} else if (BTREE
			&& cmd.get_c_phone() != COND_NONE
			&& cmd.get_c_phone() != NE
			&& cmd.get_c_name() != LIKE
			&& cmd.get_oper() != OR
			&& cmd.get_oper1() != OR) {
if (DEBUG_PRINT) printf("### delete from btree\n");
		btree.delete_(cmd);
		if (RBTREE) rbtree.delete_from_stack();
		for (int i = 0; i < max_group - min_group; i++)
			groups[i].delete_from_stack(i + min_group);
	} else if (RBTREE
			&& cmd.get_c_name() != COND_NONE
			&& cmd.get_c_name() != NE
			&& cmd.get_c_name() != LIKE
			&& cmd.get_oper() != OR
			&& cmd.get_oper1() != OR) {
if (DEBUG_PRINT) printf("### delete from rbtree\n");
		rbtree.delete_(cmd);
		if (BTREE) btree.delete_from_stack();
		for (int i = 0; i < max_group - min_group; i++)
			groups[i].delete_from_stack(i + min_group);
	} else {
if (DEBUG_PRINT) printf("### delete from all groups\n");
		for (int i = 0; i < max_group - min_group; i++)
			groups[i].delete_(cmd);
		if (BTREE) btree.delete_from_stack();
		if (RBTREE) rbtree.delete_from_stack();
	}
	stack.delete_stack();
}

template <class T>
void Database<T>::select(Command& cmd, int fd) {
	int g = 0;
	if (cmd.get_c_group() != COND_NONE
			&& cmd.get_c_group() != NE
			&& cmd.get_oper() != OR
			&& cmd.get_oper1() != OR
			&& (g = group_hash(cmd)) > 0) {
if (DEBUG_PRINT) printf("### select from groups\n");
		switch (cmd.get_c_group()) {
			case EQ:
				groups[g].select(cmd, fd);
				break;
			case GT:
				for (int i = g + 1; i < max_group - min_group; i++)
					groups[i].select(cmd, fd);
				break;
			case GE:
				for (int i = g; i < max_group - min_group; i++)
					groups[i].select(cmd, fd);
				break;
			case LT:
				for (int i = g - 1; i >= 0; i--)
					groups[i].select(cmd, fd);
				break;
			case LE:
				for (int i = g; i >= 0; i--)
					groups[i].select(cmd, fd);
				break;
			default:
				fprintf(stderr, "imp_err_3\n");
				break;
		}
	} else if (BTREE
			&& cmd.get_c_phone() != COND_NONE
			&& cmd.get_c_phone() != NE
			&& cmd.get_c_name() != LIKE
			&& cmd.get_oper() != OR
			&& cmd.get_oper1() != OR) {
if (DEBUG_PRINT) printf("### select from btree\n");
		btree.select(cmd, fd);
	} else if (RBTREE
			&& cmd.get_c_name() != COND_NONE
			&& cmd.get_c_name() != NE
			&& cmd.get_c_name() != LIKE
			&& cmd.get_oper() != OR
			&& cmd.get_oper1() != OR) {
if (DEBUG_PRINT) printf("### select from rbtree\n");
		rbtree.select(cmd, fd);
	} else {
if (DEBUG_PRINT) printf("### select from all groups\n");
		for (int i = 0; i < max_group - min_group; i++)
			groups[i].select(cmd, fd);
	}
}

template <class T>
void Database<T>::print(FILE * fp) {
	int fd = fileno(fp);
	if (fd < 0) {
		perror("print() error");
		return;
	}
	print(fd);
}

template <class T>
void Database<T>::print(int fp) {
//	for (int i = 0; i < max_group - min_group; i++) \
		groups[i].print(i + min_group);
	if (BTREE) btree.print();
	if (RBTREE) rbtree.print();
}

#endif

