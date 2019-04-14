#ifndef STACK_H
#define STACK_H

#include "stack_node.h"

#define MAX_STACK_PRINT 10
#define SAVE_IN_STACK	(1 << 0)
#define PRINT        	(1 << 1)
#define REMOVE       	(1 << 2)

template <class T>
class Stack
{
private:
	SNode<T> * top = nullptr;
	SNode<T> * curr = nullptr;

public:
	Stack() {}
	~Stack() { delete_stack(); }

	int push(T);
	T pop();
	bool is_empty();
	bool is_not_empty();
	void print();
	void delete_stack();

	void goto_next() { curr = curr->get_next(); }
	void goto_top() { curr = top; }
	SNode<T> * get_curr() const { return curr; }
	SNode<T> * get_top() const { return top; }

};

template <class T>
int Stack<T>::push(T a) {
	if (a == nullptr) return 0;
	auto t = top;
	t = new SNode<T>(a);
	if (!t) return 1;
	t->set_next(top);
	top = t;
	return 0;
}

template <class T>
T Stack<T>::pop() {
	if (top == nullptr) return nullptr;
	T a = top->get_data();
	auto t = top;
	top = top->get_next();
	delete t;
	return a;
}

template <class T>
void Stack<T>::delete_stack() {
	auto c = top;
	while (top) {
		c = top;
		top = top->get_next();
		delete c->get_data();
		delete c;
	}
	top = nullptr;
}

template <class T>
bool Stack<T>::is_empty() {
	if (top == nullptr) return true;
	return false;
}

template <class T>
bool Stack<T>::is_not_empty() {
	if (top == nullptr) return false;
	return true;
}

template <class T>
void Stack<T>::print() {
	auto t = top;
	int i = 0;
	printf("stack--------------\\\n");
	while (t && i < MAX_STACK_PRINT) {
		t->print();
		t = t->get_next();
		i++;
	}
	printf("-------------------/\n");
}

#endif

