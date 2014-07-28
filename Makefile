CFLAGS=-std=c99 -march=native -O2
LIBS=

SRCS=bf.c qf.c
OBJS=$(SRCS:.c=.o)

all: bf

.PHONY: test

${OBJS}: qf.h

.c.o:
	${CC} -c $< -o $@ ${CFLAGS} ${LIBS}

bf: $(OBJS)
	${CC} $^ -o $@ ${CFLAGS} ${LIBS}

# Depends on Perl Test::Harness
test: bf
	prove -r ./t
