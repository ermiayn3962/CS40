###
### Makefile for code lab
###
### Note:  Compiling with -O0 turns off most optimizations, which mades
###        the assembly code easier to read.
###
### aspect.c is not intended to be part of an executable:  Just
### compile it to a .o file and objdump the .o file to see the
### dispatch table.
###
### The tul (three unsigned longs) program requires the student to
### write tul.c.
###


CC     = gcc
IFLAGS = -I.
CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic \
         -O0 $(IFLAGS)

all:  aspect.o tul

tul:  tul.o tuluser.o
	$(CC) $(CFLAGS) -o $@ $^

tul.o:      tul.c     tul.h
tuluser.o:  tuluser.c tul.h

aspect.o:   aspect.c

clean:
	rm -f *.o tul
