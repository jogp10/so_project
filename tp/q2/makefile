all : addmx

phrases : addmx.o
	gcc -o addmx addmx.o

phrases.o : addmx.c
	gcc -c addmx.c

clean :
	rm addmx *.o