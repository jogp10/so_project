all : addmx

addmx : addmx.o
	gcc -o addmx addmx.o -Wall -Wextra

addmx.o : addmx.c
	gcc -c addmx.c -Wall -Wextra

clean :
	rm addmx *.o