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
	group.h \
	database.h
M = Makefile
C = clang++
#C = g++

#all: a.out
all: server client

a.out: main.o $H $M
	$C main.o -o a.out
main.o: main.cpp $H $M
	$C $F -c main.cpp

c: tc.cpp
	g++ tc.cpp -o c
s: ts.cpp
	g++ ts.cpp -o s

client: client.o $M header.h
	$C client.o -o client
client.o: client.cpp $M header.h
	$C $F -c client.cpp

server: server.o $H $M
	$C server.o -o server
server.o: server.cpp $H $M
	$C $F -c server.cpp

clean:
	rm *.o *.out
clear:
	rm *.o *.out

