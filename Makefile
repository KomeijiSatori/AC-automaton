target : ac.o avltree.o
	g++ -o ac ac.o avltree.o

ac.o : ac.cpp ac.h avltree.h
	g++ -c -g ac.cpp -std=c++11

avltree.o: avltree.c avltree.h
	g++ -c -g avltree.c

clean :
	rm ac.o avltree.o
