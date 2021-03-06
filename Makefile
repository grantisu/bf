CFLAGS=-std=c99 -march=native -O2
LIBS=

SRCS=bf.c qf.c
OBJS=$(SRCS:.c=.o)
MAIN=bf

all: $(MAIN)

.PHONY: test clean

${OBJS}: qf.h

.c.o:
	${CC} -c $< -o $@ ${CFLAGS} ${LIBS}

bf: $(OBJS)
	${CC} $^ -o $@ ${CFLAGS} ${LIBS}

clean:
	rm $(MAIN) $(OBJS)

# Depends on Perl Test::Harness
test: $(MAIN)
	prove -r ./t
