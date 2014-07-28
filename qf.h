#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>

#define MAXLOOPS 1000000

int qf_run(const char *prog, FILE *in, FILE *out);
char *bf_to_qf(const char *bfp);

