#ifndef LIST_H
#define LIST_H

#define MAX_PRINT 20

template <class T>
class Node : public T
{
private:
	Node * next;

public:
	Node(const char * n = 0, int p = 0, int g = 0) : T(n, p, g) { next = 0; }
	Node(const T& i) : T(i) { next = 0; }
	~Node() { next = 0; }
	Node& operator=(const Node& rhs) {
		*((T *)this) = rhs;
		next = 0;
		return *this;
	}
	Node * get_next() const { return next; }
	void set_next(Node * n) { next = n; }

};

template <class T>
class List
{
private:
	Node<T> * head = nullptr;
	Node<T> * curr = nullptr;
	int size = 0;

	void delete_list();
	err_code insert(Record&);
	void delete_(Command&);
	void select(Command&);


public:
	List() { head = curr = 0; }
	~List() { delete_list(); }

	err_code read(FILE * fp = stdin);
	void print(FILE * fp = stdout);
	err_code apply_command(Command&);

	void goto_head() { curr = head; }
	void goto_next() { curr = curr->get_next(); }
	void set_head(Node<T> * h) { head = h; }
	Node<T> * get_head() const { return head; }
	Node<T> * get_curr() const { return curr; }
	int get_size() const { return size; }

};

template <class T>
void List<T>::delete_list() {
	auto * c = head;
	while (c) {
		curr = c->get_next();
		delete c;
		c = curr;
	}
}

template <class T>
err_code List<T>::read(FILE * fp) {
	Node<T> c;
	err_code res = ALL_RIGHT;
	while (!(res = c.read(fp))) { res = insert(c); }
	if (res == MEM_ERR || !feof(fp)) return res;
	return ALL_RIGHT;
}

template <class T>
void List<T>::print(FILE * fp) {
	auto c = head;
	for (int i = 0; c && i < MAX_PRINT; i++, c = c->get_next()) {
//		fprintf(fp, "%d) ", i);
		c->print(fp);
	}
	printf("\n");
}

template <class T>
err_code List<T>::insert(Record& a) {
	if (!head) {
		head = new Node<T>(a);
		if (!head) return MEM_ERR;
		curr = head;
	} else {
		auto tmp = head;
		if ((a < *head) == 0) return ALL_RIGHT;
		while (tmp->get_next()) {
			if ((a < *(tmp->get_next())) == 0) break;
			tmp = tmp->get_next();
		}
		if (tmp->get_next()) return ALL_RIGHT;
		tmp->set_next(new Node<T>(a));
		if (!tmp->get_next()) return MEM_ERR;
	}
	size++;
	return ALL_RIGHT;
}

template <class T>
void List<T>::delete_(Command& cmd) {
	auto tmp = head;
	while (cmd.check(*head)) {
		tmp = head;
		if (head->get_next()) head = head->get_next();
		else { delete tmp; head = curr = 0; return; }
		delete tmp;
		size--;
	}
	tmp = head;
	while (tmp->get_next()) {
		if (cmd.check(*tmp->get_next())) {
			auto c = tmp->get_next();
			tmp->set_next(c->get_next());
			if (c == curr) curr = tmp;
			delete c;
			size--;
		} else tmp = tmp->get_next();
	}
}

template <class T>
void List<T>::select(Command& cmd) {
	auto tmp = head;
	while (tmp) {
		if (cmd.check(*tmp)) {
			tmp->print();
		}
		tmp = tmp->get_next();
	}
}

template <class T>
err_code List<T>::apply_command(Command& c) {
	switch (c.get_type()) {
	case SELECT:
		select(c);
		break;
	case INSERT: return insert(c);
	case DELETE:
		delete_(c);
		break;
	case QUIT: return EXIT;
	case STOP: return EXIT;
	case CMD_NONE: return EXIT;
	}
	return ALL_RIGHT;
}

#endif

