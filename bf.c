#include "qf.h"

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

