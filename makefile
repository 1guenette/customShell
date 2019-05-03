shell : shell.o

shell.o : shell.c imports.h
	cc -c shell.c
