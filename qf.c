#include "qf.h"

int qf_run(const char *prog, FILE *in, FILE *out)
{
	char *a, imm=0;
	unsigned short p = 0;
	int c, ch, psize, i=-1, lcount=0, ocount=0, end_of_input=0;
	const unsigned char *uprog = prog;

	a = malloc(1<<16);
	bzero(a, 1<<16);

	psize = strlen(prog);

	/* Implement an indirect threaded BF variant: QF
	 * 0x1i: + add imm
	 * 0x2i: - sub imm
	 * 0x3i: > seek next imm
	 * 0x4i: < seek prev imm
	 * 0x50: . output
	 * 0x60: , input
	 * 0x70: [ begin loop
	 * 0x80: ] end loop
	 * 0x90: nop
	 */
	static void *itbl[] = {&&qf_fin, &&qf_inc, &&qf_dec, \
		&&qf_nxt, &&qf_prv, &&qf_out, &&qf_inp, \
		&&qf_beg, &&qf_end, &&qf_nop};

	#define NEXT if(i < psize) { \
		unsigned char instr = uprog[++i]; \
		imm = instr & 0xf; \
		goto *itbl[instr >> 4]; \
	} else { goto qf_fin; }

	/* Start the program */
	qf_nop:
		NEXT;
	qf_inc:
		a[p] += imm;
		NEXT;
	qf_dec:
		a[p] -= imm;
		NEXT;
	qf_nxt:
		p += imm;
		NEXT;
	qf_prv:
		p -= imm;
		NEXT;
	qf_out:
		if (ocount < MAXWRITE) {
			fputc(a[p], out);
			ocount++;
		} else {
			goto QF_ERR;
		}
		NEXT;
	qf_inp:
		if (!end_of_input) {
			c = fgetc(in);
			if (c == EOF) {
				c = 0;
				end_of_input = 1;
			}
		}
		a[p] = c;
		NEXT;
	qf_beg:
		if(a[p] == 0) {
			c = 1;
			while(c) {
				if(i == psize-1)
					goto QF_ERR;
				ch = uprog[++i];
				if(ch == 0x70)
					c++;
				else if(ch == 0x80)
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
				ch = uprog[--i];
				if(ch == 0x70)
					c++;
				else if(ch == 0x80)
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
	int i=0, j=0, k=0, maxlen=128, plen;
	char *qfp;
	
	plen = strlen(bfp);

	qfp = malloc(maxlen);

	#define CONSUME(ch) \
		for (k = 1; bfp[i] == ch && k < 15 && i < plen; k++) ++i

	while(i < plen) {
		switch(bfp[i++]) {
			case '+':
				CONSUME('+');
				qfp[j++] = 0x10 + k;
				break;
			case '-':
				CONSUME('-');
				qfp[j++] = 0x20 + k;
				break;
			case '>':
				CONSUME('>');
				qfp[j++] = 0x30 + k;
				break;
			case '<':
				CONSUME('<');
				qfp[j++] = 0x40 + k;
				break;
			case '.':
				qfp[j++] = 0x50;
				break;
			case ',':
				qfp[j++] = 0x60;
				break;
			case '[':
				qfp[j++] = 0x70;
				break;
			case ']':
				qfp[j++] = 0x80;
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

