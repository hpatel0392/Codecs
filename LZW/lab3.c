#include <stdio.h>
#include <stdlib.h>
#include "table.h"

#define DEC_SIZE 65535

void rootDict(dictionary_t *);
void compress(FILE *, FILE*);
void decompress(FILE *, FILE*);

int main(int argc, char* argv[]){

	FILE				*in, *out;

	if(argc != 4){
		printf("Error! Usage: ./lab3 [in-file] [out-file] ['c' or 'd']\n");
		exit(1);
	}

	in = fopen(argv[1], "rb");
	if(in == NULL){
		printf("Error! cannot open file [%s]\n", argv[1]);
		exit(1);
	}

	out = fopen(argv[2], "wb+");
	if(out == NULL){
		printf("Error! cannot open file [%s]\n", argv[2]);
		exit(1);
	}

	if(argv[3][0] != 'c' && argv[3][0] != 'd'){
		printf("Error! Third argument must be either 'c' or 'd'\n");
		exit(1);
	}

	if(argv[3][0] == 'c'){
		compress(in, out);
	} else {
		decompress(in, out);
	}

	fclose(out);
	fclose(in);
	return 0;
}

/*
* --------------------------------------------------------------------------------------------
*/

void rootDict(dictionary_t* D){
	unsigned char C[0];
	hashkey_t *K;

	for(C[0] = 0; C[0] < 256; C[0]++){
		K = makeKey(C, 1);
		insert(D, isInDict(K, D), K);
		K = NULL;
		if(C[0] == 255) break;
	}
}



void compress(FILE *in, FILE *out){

	// Variables for Compression
	dictionary_t 		*coD;
	hashkey_t			*P, *newKey;
	entry_t				*newE, *PE;
	unsigned char		C;

	coD = makeDict();
	rootDict(coD);

	/* Start compression  */
	P = (hashkey_t*)calloc(1, sizeof(hashkey_t));
	P->key = (unsigned char*)calloc(512, 1);
	P->len = 0;
	fread(&C, 1, 1, in);	// Read C
	while(!feof(in)){
		
		// Check if P + C is in D
		P->key[P->len] = C;
		newKey = makeKey(P->key, P->len + 1);
		newE = isInDict(newKey, coD);

		// P + C not in D
		if(newE->next == NULL){
			// output outcode for P
			PE = isInDict(P, coD);
			if(PE->next == NULL){
				printf("Fatal Error...exiting out!\n");
				exit(1);
			}
			fwrite(&(PE->next->outCode), 2, 1, out);
			PE = NULL;

			// add P + C to dict
			insert(coD, newE, newKey);

			// P = C
			P->key[0] = C;
			P->len = 1;

		} else { 		// P + C is in D
			P->len++;  // P = P + C
			destroyKey(newKey);
		}
		fread(&C, 1, 1, in);	// Read C

		newKey = NULL;
		newE = NULL;
	}

	PE = isInDict(P, coD);
	if(PE->next == NULL){
		printf("Fatal Error...exiting out!\n");
		exit(1);
	}
	fwrite(&(PE->next->outCode), 2, 1, out);
	PE = NULL;

	free(P->key);
	free(P);
	/* End compression */

	destroyDict(coD);
}


void decompress(FILE *in, FILE *out){
	// Variables for decompression
	hashkey_t 			*Dec, *X, *newKey;
	unsigned short		decEntries;
	unsigned short		C, P, i;

	// build root Dictionary
	Dec = (hashkey_t*)calloc(DEC_SIZE, sizeof(hashkey_t));
	decEntries = 0;
	for(i = 0; i < 256; i++){
		Dec[i].key = (unsigned char*)calloc(1, 1);
		*(Dec[i].key) = i;
		Dec[i].len = 1;
		decEntries++;
	}

	/* Start decompression */
	fread(&C, 2, 1, in);  						// Read C
	if(C < decEntries){
		C--;
		fwrite(Dec[C].key, 1, Dec[C].len, out);	// Write pattern for C
	} else {
		printf("Fatal Error! Input File is Invalid!\n");
		exit(1);
	}

	P = C;
	fread(&C, 2, 1, in);  					// Read C
	while(!feof(in)){
		C--;

		// Check if C in Dec
		if(C >= decEntries){  // No
			// X = P
			X = &Dec[P];

			// output X + Z
			fwrite(X->key, 1, X->len, out);
			fwrite(X->key, 1, 1, out);

			// Add X + Z to Dec
			newKey = &Dec[decEntries++];
			newKey->len = 0;
			newKey->key = (unsigned char*)calloc(X->len+1, 1);
			keyNCat(newKey, X, X->len);
			keyNCat(newKey, X, 1);

		} else {			// Yes
			// out put pattern for C
			fwrite(Dec[C].key, 1, Dec[C].len, out);

			// X = P
			X = &Dec[P];

			// Add X + Y to Dec
			newKey = &Dec[decEntries++];
			newKey->len = 0;
			newKey->key = (unsigned char*)calloc(X->len+1, 1);
			keyNCat(newKey, X, X->len);
			keyNCat(newKey, &Dec[C], 1);
		}

		P = C;
		fread(&C, 2, 1, in);  					// Read C
	}

	/* End decompression  */

	for(i = 0; i < decEntries; i++){
		free(Dec[i].key);
	}	
	free(Dec);
}