all : cypher

phrases : cypher.o
	gcc -Wall -Wextra -o cypher cypher.o

phrases.o : cypher.c
	gcc -Wall -Wextra -c cypher.c

clean :
	rm cypher *.o