all : addmx

addmx : addmx.o
	gcc -o addmx addmx.o

addmx.o : addmx.c
	gcc -c addmx.c

clean :
	rm addmx *.o