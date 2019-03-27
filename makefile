F = -std=c++11 -O3
all: a

a: main.o record.h command.h makefile
	g++ main.o -o a
main.o: main.cpp record.h command.h errors.h list.h
	g++ $F -c main.cpp

clean:
	rm *.o a
clear:
	rm *.o a

