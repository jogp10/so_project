all : phrases

phrases : phrases.o
	gcc -o phrases phrases.o

phrases.o : phrases.c
	gcc -c phrases.c

clean :
	rm phrases *.o
