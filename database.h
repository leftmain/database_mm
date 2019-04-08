#ifndef DATABASE_H
#define DATABASE_H

#include "header.h"
#include "list.h"
#include "btree.h"

template <class T>
class Database
{
private:
	List<T> list;
	BTree<Node<T> *> btree;

	int insert(T&);
	void delete_(Command&);
	void select(Command&, FILE * = stdout);

public:
	Database() : btree(2) {}
	~Database() {}

	int read(FILE *);
	int apply_command(Command&, FILE * = stdout);
	void start(FILE * = stdin, FILE * = stderr);

};

template <class T>
int Database<T>::read(FILE * fp) {
	T c;
	int res = 0;
	Node<T> * node = nullptr;
	while ((res = c.read(fp)) == ALL_RIGHT) {
		if ((node = list.add(c)) == nullptr) return MEM_ERR;
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
list.print();
btree.print();
//return;
/*
btree.delete_(list.get_head()->get_next());
btree.delete_(list.get_head());
btree.get_stack()->print();
list.delete_from_stack(btree.get_stack());
//btree.delete_element(list.get_head(), btree.get_root());
list.print();
btree.print();
return;
*/
	while (fgets(buf, LEN, in)) {
		res = c.parse(buf);
		if (res) apply_command(c);
		else if (c.get_type() == QUIT) break;
		c.clear();
list.print();
btree.print();
	}
	t = (clock() - t) / CLOCKS_PER_SEC;
	fprintf(out, "Time: %.2lf\n", t);
}

template <class T>
int Database<T>::insert(T& a) {
	list.insert(a);
}

template <class T>
void Database<T>::delete_(Command& cmd) {
	if (cmd.get_c_phone() != COND_NONE && cmd.get_c_phone() != NE)
		list.delete_from_stack(btree.delete_(cmd));
	else list.delete_(cmd);
}

template <class T>
void Database<T>::select(Command& cmd, FILE * fp) {
	if (cmd.get_c_phone() == COND_NONE || cmd.get_c_phone() == NE) {
		list.select(cmd, fp);
	} else {
		btree.select(cmd, fp);
	}
}

#endif

