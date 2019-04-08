F = -std=c++11 -g
H = header.h \
	record.h \
	command.h \
	list_node.h \
	list.h \
	stack_node.h \
	stack.h \
	btree_node.h \
	btree.h \
	database.h
all: a

a: main.o $H makefile
	g++ main.o -o a
main.o: main.cpp $H
	g++ $F -c main.cpp

clean:
	rm *.o a
clear:
	rm *.o a

