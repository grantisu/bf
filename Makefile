CFLAGS=-std=c99 -march=native -O2
LIBS=

CC=gcc

all: bf

bf: bf.c
	${CC} $^ -o $@ ${CFLAGS} ${LIBS}

