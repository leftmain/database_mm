#ifndef BNODE_H
#define BNODE_H

#include "header.h"
#include "list_node.h"

#define EQUAL_ON_RIGHT 1
#define MAX_NODE_PRINT 5

template <class T>
int cmp(const T, const T);

template <class T>
class BNode {
private:
	T * data = nullptr;
	BNode * child = nullptr;
	int len = 0;

public:
	BNode() {}
	~BNode();
	
	void operator=(const BNode&);
	void operator++(int);
	void operator--(int);
	int alloc_data(int);
	int alloc_child(int);
	int add(T, int);
	BNode * cut(T * , int);
	int add(T, int, int, BNode *);
	void rm(int);
	BNode * cut(T *, int, int, BNode *);
	BNode * find_child(T, int * = nullptr);
	int bin_search(T, int (*)(const T, const T) = cmp);
	int bin_search(Record *, int (*)(const Record *, const T) = cmp);
	void clear();
	void print(FILE * = stdout);
	void print(int);

	T * get_data() const { return data; }
	BNode * get_child() const { return child; }
	int get_len() const { return len; }
	void set_len(int l) { len = l; }

};

template <class T>
BNode<T>::~BNode() {
	data = nullptr;
	child = nullptr;
	len = 0;
}

template <class T>
void BNode<T>::operator=(const BNode<T>& b) {
	data = b.data;
	child = b.child;
	len = b.len;
}

template <class T>
void BNode<T>::operator++(int) {
	len++;
}

template <class T>
void BNode<T>::operator--(int) {
	len--;
}

template <class T>
int BNode<T>::alloc_data(int m) {
	data = new T[2 * m];
	if (!data) return 1;
	return 0;
}

template <class T>
int BNode<T>::alloc_child(int m) {
	child = new BNode<T>[2 * m + 1];
	if (!child) return 1;
	return 0;
}

template <class T>
int BNode<T>::add(T a, int m) {
	int i = bin_search(a);
	if (i < len && cmp(a, data[i]) == 0) return -1;
	if (len == 2 * m) return 1;
	for (int j = len; j > i; j--) data[j] = data[j-1];
	data[i] = a;
	len++;
	return 0;
}

template <class T>
void BNode<T>::rm(int i) {
}

template <class T>
BNode<T> * BNode<T>::cut(T * a, int m) {
	BNode<T> * new_node = new BNode<T>;
	if (new_node == nullptr) return nullptr;
	if (new_node->alloc_data(m)) {
		delete new_node;
		return nullptr;
	}
	T tmp;
	int j = 0, i = 0;
	i = bin_search(*a);
	if (i <= m) {
		if (i == m) tmp = *a;
		else {
			tmp = data[m-1];
			for (j = m-1; j > i; j--) data[j] = data[j-1];
			data[i] = *a;
		}
		for (j = m; j < len; j++) {
			new_node->data[j-m] = data[j];
		}
		new_node->len = len = m;
	} else {
		tmp = data[m];
		for (j = m+1; j < i; j++) {
			new_node->data[j-m-1] = data[j];
		}
		new_node->data[i-m-1] = *a;
		for (j = i; j < len; j++) {
			new_node->data[j-m] = data[j];
		}
		new_node->len = len = m;
	}
	*a = tmp;
	return new_node;
}

template <class T>
int BNode<T>::add(T a, int m, int i, BNode<T> * new_node) {
	if (len == 2 * m) return 1;
	for (int j = len; j > i; j--) {
		data[j] = data[j-1];
		child[j+1] = child[j];
	}
	data[i] = a;
	child[i+1] = *new_node;
	delete new_node;
	len++;
	return 0;
}

template <class T>
BNode<T> * BNode<T>::cut(T * a, int m, int i, BNode<T> * res) {
	BNode<T> * new_node = new BNode<T>;
	if (new_node == nullptr) return nullptr;
	if (new_node->alloc_data(m)) {
		delete new_node;
		return nullptr;
	}
	if (new_node->alloc_child(m)) {
		delete [] new_node->data;
		delete new_node;
		return nullptr;
	}
	int j = 0;
	if (i < m) {
		T tmp = data[m-1];
		BNode<T> n_tmp = child[m];
		for (j = m-1; j > i; j--) {
			data[j] = data[j-1];
			child[j+1] = child[j];
		}
		data[i] = *a;
		child[i+1] = *res;
		*a = tmp;
		*res = n_tmp;
	} else if (i > m) {
		new_node->child[0] = child[m+1];
		for (j = m+1; j < i; j++) {
			new_node->data[j-m-1] = data[j];
			new_node->child[j-m] = child[j+1];
		}
		new_node->data[i-m-1] = *a;
		new_node->child[i-m] = *res;
		delete res;
		for (j = i; j < len; j++) {
			new_node->data[j-m] = data[j];
			new_node->child[j-m+1] = child[j+1];
		}
		new_node->len = len = m;
		*a = data[m];
		return new_node;
	}
	new_node->child[0] = *res;
	delete res;
	for (j = m; j < len; j++) {
		new_node->data[j-m] = data[j];
		new_node->child[j-m+1] = child[j+1];
	}
	new_node->len = len = m;
	return new_node;
}

template <class T>
BNode<T> * BNode<T>::find_child(T a, int * i) {
	if (i == nullptr) return child + bin_search(a);
	*i = bin_search(a);
	return child + *i;
}

template <class T>
int BNode<T>::bin_search(T a, int (cmp)(const T, const T)) {
	int k = 0;
	int i = 0;
	int j = len;
	if (len == 0) return 0;
	if (cmp(a, data[0]) <= 0) return 0;
	while (i != j && i != j - 1) {
		k = (i + j) >> 1;
		if (cmp(a, data[k]) <= 0) j = k;
		else i = k;
	}
	return j;
}

template <class T>
int BNode<T>::bin_search(Record * a, int (cmp)(const Record *, const T)) {
	int k = 0;
	int i = 0;
	int j = len;
	if (cmp(a, data[0]) <= 0) return 0;
	while (i != j && i != j - 1) {
		k = (i + j) >> 1;
		if (cmp(a, data[k]) <= 0) j = k;
		else i = k;
	}
	return j;
}

template <class T>
void BNode<T>::clear() {
	data = nullptr;
	child = nullptr;
	len = 0;
}

template <class T>
void BNode<T>::print(FILE * fp) {
	int fd = fileno(fp);
	if (fd < 0) {
		perror("print() in BNode error");
		return;
	}
	print(fd);
}

template <class T>
void BNode<T>::print(int fd) {
	if (data == nullptr) return;
	for (int i = 0; i < MAX_NODE_PRINT && i < len; i++)
		if (data[i]) data[i]->draw(fd);
	dprintf(fd, "\n");
}

template <class T>
int cmp_p(const Record * a, const T b) {
	return a->get_phone() - b->get_phone();
}

template <class T>
int cmp(const T a, const T b) {
//	return a->get_phone() - b->get_phone();
	if (a->get_phone() == b->get_phone()) {
		if (a->get_group() == b->get_group()) {
			if (a->get_name() && b->get_name()) {
				return strcmp(a->get_name(), b->get_name());
			}
			if (a->get_name() && !b->get_name()) return 1;
			if (!a->get_name() && b->get_name()) return -1;
		}
		return a->get_group() - b->get_group();
	}
	return a->get_phone() - b->get_phone();
}

#endif

