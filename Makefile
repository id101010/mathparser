CC=gcc
CFLAGS=-Wall -g
PRGNAME=mathparse
RUN=valgrind

all: clean build run

clean:
	rm -f $(PRGNAME)

build:
	$(CC) $(CFLAGS) -o $(PRGNAME) $(PRGNAME).c

run:
	$(RUN) ./$(PRGNAME) '(123/(12/6))'
