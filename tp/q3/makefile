all : cypher

cypher : cypher.o
	gcc -Wall -Wextra -o cypher cypher.o

cypher.o : cypher.c
	gcc -Wall -Wextra -c cypher.c

clean :
	rm cypher *.o