target : ac.o
	g++ -o ac ac.o

ac.o : ac.cpp ac.h
	g++ -c -g ac.cpp

clean :
	rm ac.o
