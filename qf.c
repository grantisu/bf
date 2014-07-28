#include "qf.h"

int qf_run(const char *prog, FILE *in, FILE *out)
{
	char *a;
	unsigned short p = 0;
	int c, ch, psize, i=-1, lcount=0;

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

	#define NEXT if(i < psize) { goto *itbl[(unsigned char)prog[++i]]; } else { goto qf_fin; }

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
		}
		NEXT;
	qf_end:
		if(a[p] != 0) {
			if(++lcount > MAXLOOPS)
				goto QF_ERR;
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

