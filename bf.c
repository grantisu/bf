#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>

#define MAXLOOPS 1000000

int qf_run(const char *prog, FILE *in, FILE *out)
{
	char *a;
	unsigned short p = 0;
	int c, ch, psize, i=0, lcount=0;

	a = malloc(1<<16);
	bzero(a, 1<<16);

	psize = strlen(prog);

	/* Implement an indirect threaded BF variant: QF
	 * 1: + inc
	 * 2: - dec
	 * 3: > seek next
	 * 4: < seek prev
	 * 5: . output
	 * 6: , input
	 * 7: [ begin loop
	 * 8: ] end loop
	 * 9: nop
	 */
	static void *itbl[] = {&&qf_fin, &&qf_inc, &&qf_dec, \
		&&qf_nxt, &&qf_prv, &&qf_out, &&qf_inp, \
		&&qf_beg, &&qf_end, &&qf_nop};

	#define NEXT if(i < psize) { goto *itbl[(unsigned char)prog[i++]]; } else { goto qf_fin; }

	/* Start the program */
	qf_nop:
		NEXT;
	qf_inc:
		a[p]++;
		NEXT;
	qf_dec:
		a[p]--;
		NEXT;
	qf_nxt:
		p++;
		NEXT;
	qf_prv:
		p--;
		NEXT;
	qf_out:
		fputc(a[p], out);
		NEXT;
	qf_inp:
		c = fgetc(in);
		a[p] = (c != EOF) ? c : 0;
		NEXT;
	qf_beg:
		if(a[p] == 0) {
			i--;
			c = 1;
			while(c) {
				if(i == psize-1)
					goto QF_ERR;
				ch = prog[++i];
				if(ch == 7)
					c++;
				else if(ch == 8)
					c--;
			}
			i++;
		}
		NEXT;
	qf_end:
		if(a[p] != 0) {
			if(++lcount > MAXLOOPS)
				goto QF_ERR;
			i--;
			c = -1;
			while(c) {
				if(i == 0)
					goto QF_ERR;
				ch = prog[--i];
				if(ch == 7)
					c++;
				else if(ch == 8)
					c--;
			}
			i++;
		}
		NEXT;
	qf_fin:
		free(a);
		return 1;

	QF_ERR:
		fprintf(out, "\nERR: problems.\n");
		free(a);
		return -1;
}

/* Convert bf to qf */
char *bf_to_qf(const char *bfp)
{
	int i=0, j=0, maxlen=128, plen;
	char *qfp;
	
	plen = strlen(bfp);

	qfp = malloc(maxlen);

	while(i < plen) {
		switch(bfp[i++]) {
			case '+':
				qfp[j++] = 1;
				break;
			case '-':
				qfp[j++] = 2;
				break;
			case '>':
				qfp[j++] = 3;
				break;
			case '<':
				qfp[j++] = 4;
				break;
			case '.':
				qfp[j++] = 5;
				break;
			case ',':
				qfp[j++] = 6;
				break;
			case '[':
				qfp[j++] = 7;
				break;
			case ']':
				qfp[j++] = 8;
				break;
		}

		if(j == maxlen) {
			maxlen *= 2;
			qfp = realloc(qfp, maxlen);
		}
	}

	qfp[j] = '\0';

	return qfp;
}

int main(int argc, char **argv)
{
	int fsz, rval;
	char *bfp=NULL, *qfp;
	FILE *bfd;

	if(argc < 2 || argc > 3) {
		fprintf(stderr, "\nUsage:\n\n%s {filename}\n%s -e {program}\n\n", argv[0], argv[0]);
		return -1;
	}

	if(strncmp(argv[1], "-e", 3) == 0) {
		qfp = bf_to_qf(argv[2]);
	} else {
		bfd = fopen(argv[1], "r");
		if(bfd == NULL) {
			fprintf(stderr, "\nERROR: can't open file %s.\n", argv[1]);
			exit(-1);
		}

		fseek(bfd, 0, SEEK_END);
		fsz = ftell(bfd);
		fseek(bfd, 0, SEEK_SET);

		bfp = malloc(fsz + 1);
		if(bfp == NULL) {
			fprintf(stderr, "\nERROR: can't allocate %d bytes.\n", fsz + 1);
			exit(-1);
		}

		if(fgets(bfp, fsz + 1, bfd) == NULL) {
			fprintf(stderr, "\nERROR: can't read file %s.\n", argv[1]);
			exit(-1);
		}

		fclose(bfd);

		qfp = bf_to_qf(bfp);
	}

	rval = qf_run(qfp, stdin, stdout);

	if(bfp != NULL)
		free(bfp);
	free(qfp);

	return rval;
}

