# Comp 40: Lab 7

CFLAGS := -g -O1 -std=c99 -Wall -Wextra -Werror -Wfatal-errors

EXECUTABLES = linkedlist loop do_times16

all: $(EXECUTABLES)

%.o : %.c linkedlist.h times16.h

loop: loop.c

linkedlist: list-main.o linkedlist.o linkedlist.h

do_times16: do_times16.o times16.o times16.h

clean:
	rm -f $(EXECUTABLES) *.o
