target : ac.o
	gcc -o ac ac.o

ac.o : ac.cpp ac.h
	gcc -c -g ac.cpp

clean :
	rm ac.o
