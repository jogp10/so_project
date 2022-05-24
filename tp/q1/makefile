all : phrases

phrases : phrases.o
	gcc -o phrases phrases.o -Wall -Wextra

phrases.o : phrases.c
	gcc -c phrases.c -Wall -Wextra

clean :
	rm phrases *.o
