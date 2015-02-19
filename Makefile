CC=gcc
CFLAGS=-Wall -g
PRGNAME=mathparse
RUN=valgrind
ARGLIST='((123-1)/(3*(12/6)))'
DEBUG=gdb

all: clean build run

clean:
	rm -f $(PRGNAME)

build:
	$(CC) $(CFLAGS) -o $(PRGNAME) $(PRGNAME).c

run:
	$(RUN) ./$(PRGNAME) $(ARGLIST)

debug:
	$(DEBUG) ./$(PRGNAME)
