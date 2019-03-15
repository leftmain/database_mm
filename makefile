F = -g
all: a

a: main.o record.h command.h
	g++ main.o -o a
main.o: main.cpp record.h command.h errors.h
	g++ $F -c main.cpp

clear:
	rm *.o a

