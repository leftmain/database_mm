#ifndef RBNODE_H
#define RBNODE_H

template <class T>
int cmp(const T, const T);

enum color
{
	RED,
	BLACK
};

template <class T>
class RBNode
{
private:
	RBNode * right = nullptr;
	RBNode * left = nullptr;
	T data = nullptr;
	bool red = true;

public:
	RBNode(T = nullptr, color = RED);
	~RBNode();

	void operator=(const RBNode&);
	void print(FILE * = stdout);

	void set_right(RBNode * r) { right = r; }
	void set_left(RBNode * l) { left = l; }
	void set_color(color c) { red = (c == RED) ? true : false; }
	RBNode * get_right() const { return right; }
	RBNode * get_left() const { return left; }
	color get_color() const { return (red) ? RED : BLACK; }
	bool is_red() const { return red; }
	bool is_black() const { return !red; }
	bool black_right() const { return \
				(right && right->red) ? false : true; }
	bool black_left() const { return \
				(left && left->red) ? false : true; }
	T get_data() const { return data; }

};

template <class T>
RBNode<T>::RBNode(T d, color c) {
	data = d;
	red = (c == RED) ? true : false;
}

template <class T>
RBNode<T>::~RBNode() {
	RBNode * right = nullptr;
	RBNode * left = nullptr;
	T data = nullptr;
	bool red = true;
}

template <class T>
void RBNode<T>::operator=(const RBNode& a) {
	if (&a == nullptr) data = nullptr;
	else data = a.get_data();
}

template <class T>
void RBNode<T>::print(FILE * fp) {
	if (red) fprintf(fp, "-");
	else fprintf(fp, "*");
	data->draw(fp);
	fprintf(fp, "\n");
}

template <class T>
int cmp_npg(const T a, const T b) {
	if (a->get_name() && b->get_name()) {
		int res = strcmp(a->get_name(), b->get_name());
		if (res != 0) return res;
		if (a->get_phone() == b->get_phone())
			return a->get_group() - b->get_group();
		return a->get_phone() - b->get_phone();
	}
	if (!a->get_name() && b->get_name()) return -1;
	if (a->get_name() && !b->get_name()) return 1;
	if (a->get_phone() == b->get_phone())
		return a->get_group() - b->get_group();
	return a->get_phone() - b->get_phone();
}

template <class T>
int cmp_n(const T a, const Record * b) {
	return strcmp(a->get_name(), b->get_name());
}

#endif

