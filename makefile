F = -std=c++11 -O3
H = header.h \
	record.h \
	command.h \
	list_node.h \
	list.h \
	stack_node.h \
	stack.h \
	btree_node.h \
	btree.h \
	rbtree_node.h \
	rbtree.h \
	database.h
all: a.out

a.out: main.o $H makefile
	g++ main.o -o a.out
main.o: main.cpp $H makefile
	g++ $F -c main.cpp

clean:
	rm *.o a.out
clear:
	rm *.o a.out

