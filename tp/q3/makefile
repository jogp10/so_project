all : cypher

phrases : cypher.o
	gcc -o cypher cypher.o

phrases.o : cypher.c
	gcc -c cypher.c

clean :
	rm cypher *.o