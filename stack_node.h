#ifndef SNODE_H
#define SNODE_H

#include "header.h"

template <class T>
class SNode
{
private:
	SNode * next = nullptr;
	T data = nullptr;

public:
	SNode() {}
	SNode(T d) { data = d; }
	~SNode() {}

	SNode * get_next() const { return next; }
	T get_data() const { return data; }
	void set_next(SNode * n) { next = n; }
	void set_data(T d) { data = d; }
	void print() { data->print(); }

};

#endif

