CFLAGS=-std=c99 -march=native -O2
LIBS=

CC=gcc

all: bf

.PHONY: test

bf: bf.c
	${CC} $^ -o $@ ${CFLAGS} ${LIBS}

# Depends on Perl Test::Harness
test: bf
	prove -r ./t
