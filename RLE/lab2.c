#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

	FILE *in, *out;
	unsigned char count = 0;
	int i;
	char curr, prev;

	if (argc != 4) {
		printf("Not enough arguments! Usage: lab2 [in file] [out file] ['c' or 'd']\n");
		exit(1);
	}

	if (argv[3][0] != 'c' && argv[3][0] != 'd') {
		printf("Incorrect arguments! Usage: lab2 [file] ['c' or 'd']\n");
		exit(1);
	}

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		printf("Cannot open file!\n");
		exit(1);
	}

	if (argv[3][0] == 'c') {
		out = fopen(argv[2], "wb+");
	
		count = 0;
		fread(&curr, 1, 1, in);
		while (!feof(in)) {
			prev = curr;
			fread(&curr, 1, 1, in);
			count++;
			if (prev != curr || count == 255) {
				fwrite(&count, 1, 1, out);
				fwrite(&prev, 1, 1, out);
				count = 0;
			}
		}

		if (count != 0) {
			fwrite(&count, 1, 1, out);
			fwrite(&curr, 1, 1, out);
		}

	} else {
		out = fopen(argv[2], "wb+");

		fread(&count, 1, 1, in);
		while (!feof(in)) {
			fread(&curr, 1, 1, in);
			for (i = 0; i < count; i++) {
				fwrite(&curr, 1, 1, out);
			}
			fread(&count, 1, 1, in);
		}
	}

	fclose(in);
	fclose(out);
	return 0;
}
