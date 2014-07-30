#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>

#ifndef MAXLOOPS
#define MAXLOOPS 1000000
#endif

#ifndef MAXWRITE
#define MAXWRITE 4095
#endif

int qf_run(const char *prog, FILE *in, FILE *out);
char *bf_to_qf(const char *bfp);

