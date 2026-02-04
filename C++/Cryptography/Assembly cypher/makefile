all: assemble link run

build: assemble link

assemble: cipher.s
	g++ -Wall -c cipher.s -o cipher.o

link: interface.cpp cipher.o cipher.h
	g++ -Wall interface.cpp cipher.o -o test

run:
	./test