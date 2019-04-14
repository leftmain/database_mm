#ifndef LIST_H
#define LIST_H

#include "header.h"
#include "command.h"
#include "list_node.h"
#include "stack.h"

#define MAX_PRINT 20

template <class T>
class List
{
private:
	Node<T> * head = nullptr;
	Node<T> * curr = nullptr;
	Stack<Node<T> *> * stack = nullptr;
	int size = 0;

	void delete_list();

public:
	List(Stack<Node<T> *> * st) { stack = st; }
	~List() { delete_list(); }

	Node<T> * add(T&);
	int read(FILE * fp = stdin);
	void print(FILE * fp = stdout);
	void print_back(FILE * fp = stdout);
	Node<T> * delete_node(Node<T> *, int = REMOVE);
	void delete_from_stack();
	void add_node(Node<T> *);

	Node<T> * insert(T&);
	void delete_(Command&);
	void select(Command&, FILE * fp = stdout);
	void remove(Node<T> *);

	void goto_head() { curr = head; }
	void goto_next() { curr = curr->get_next(); }
	void goto_prev() { curr = curr->get_prev(); }
	void set_head(Node<T> * h) { head = h; }
	void set_curr(Node<T> * c) { curr = c; }
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
	head = curr = nullptr;
	size = 0;
}

template <class T>
Node<T> * List<T>::add(T& n) {
	auto c = new Node<T>;
	if (!c) return nullptr;
	c->move(n);
	c->set_next(head);
	if (head) head->set_prev(c);
	head = c;
	size++;
	return head;
}

template <class T>
int List<T>::read(FILE * fp) {
	T c;
	int res = 0;
	if (head) delete_list();
	while ((res = c.read(fp)) == ALL_RIGHT) {
		if (!add(c)) return MEM_ERR;
	}
	if (res == MEM_ERR || !feof(fp)) return res;
	return ALL_RIGHT;
}

template <class T>
void List<T>::print(FILE * fp) {
printf("list-------------------------\\\n");
	auto c = head;
	for (int i = 0; c && i < MAX_PRINT; i++, c = c->get_next()) {
		c->print(fp);
	}
	fprintf(fp, "\n");
printf("-----------------------------/\n");
}

template <class T>
void List<T>::print_back(FILE * fp) {
	auto c = head;
	if (!c) return;
	while (c->get_next()) c = c->get_next();
	for (int i = 0; c && i < MAX_PRINT; i++, c = c->get_prev()) {
		c->print(fp);
	}
	fprintf(fp, "\n");
}

template <class T>
Node<T> * List<T>::delete_node(Node<T> * n, int flag) {
	if (n == nullptr) return nullptr;
	if (n == head) {
		head = head->get_next();
		if (head) head->set_prev(nullptr);
		if (flag & REMOVE) delete n;
		size--;
		return head;
	}
	auto ret = n->get_next();
	if (n->get_prev()) n->get_prev()->set_next(ret);
	else head = ret;
	if (n->get_next()) ret->set_prev(n->get_prev());
	if (flag & REMOVE) delete n;
	size--;
	return ret;
}

template <class T>
void List<T>::delete_from_stack() {
	stack->goto_top();
	while (stack->get_curr()) {
		delete_node(stack->get_curr()->get_data(), SAVE_IN_STACK);
		stack->goto_next();
	}
	stack->goto_top();
}

template <class T>
void List<T>::add_node(Node<T> * n) {
	n->set_next(head);
	head->set_prev(n);
	head = n;
	size++;
}

template <class T>
Node<T> * List<T>::insert(T& a) {
	if (!head) {
		head = new Node<T>(a);
		if (!head) return nullptr;
		curr = head;
		size++;
		return head;
	} else {
		auto tmp = head;
		if ((a < *head) == 0) return nullptr;
		while (tmp->get_next()) {
			if ((a < *(tmp->get_next())) == 0) break;
			tmp = tmp->get_next();
		}
		if (tmp->get_next()) return nullptr;
		tmp->set_next(new Node<T>(a));
		if (!tmp->get_next()) return nullptr;
		tmp->get_next()->set_prev(tmp);
		size++;
		return tmp->get_next();
	}
}

template <class T>
void List<T>::delete_(Command& cmd) {
	auto tmp = head;
	while (tmp) {
		if (cmd.check(*tmp)) {
			if (stack->push(tmp)) {
				perror("memory error in stack\n");
				return;
			}
			tmp = delete_node(tmp, SAVE_IN_STACK);
		} else tmp = tmp->get_next();
	}
}

template <class T>
void List<T>::select(Command& cmd, FILE * fp) {
	auto tmp = head;
	while (tmp) {
		if (cmd.check(*tmp)) {
			tmp->print(fp);
		}
		tmp = tmp->get_next();
	}
}

#endif

