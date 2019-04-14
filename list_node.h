#ifndef NODE_H
#define NODE_H

#include "header.h"

template <class T>
class Node : public T
{
private:
	Node * next = nullptr;
	Node * prev = nullptr;

public:
	Node() : T() {}
	Node(const T& t) : T(t) {}
	~Node() {}
	Node& operator=(const Node& rhs) {
		*((T *)this) = rhs;
		next = prev = 0;
		return *this;
	}
	T * get_T(void) const { return (T *)this; }
	Node * get_next() const { return next; }
	Node * get_prev() const { return prev; }
	void set_next(Node * n) { next = n; }
	void set_prev(Node * p) { prev = p; }

};



#endif

