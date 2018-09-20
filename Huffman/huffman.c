#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

int main(int argc, char const *argv[])
{
	FILE *in, *out;
	node *L = NULL; // List
	node *T = NULL; // Tree

	unsigned int symtable[256] = {0};  // initialize symbol frequencies to 0
	unsigned char C; 
	unsigned int numSymbols = 0;  // used for decoding
	int i,j;

	// checking for valid input 
	if (argc != 4) {
		printf("usage: ./lab4 [-c|-d] input_filename");
		printf(" output_filename\n"); 
		printf("\t-c compression\n\t-d decompression\n");
		return 1;
	}

	// open the input and output files
	in = fopen(argv[2], "rb");
	if(in == NULL){
		printf("Error opening input file\n");
		exit(1);
	}
	out = fopen(argv[3], "wb");

	/// if decompressing ///
	if(argv[1][1] == 'd'){
		// read the header information for symbol frequencies
		// of each character from 0-255
		for(i = 0; i < 256; i++){
			fread(&j, 4, 1, in);
			symtable[i] = j;
			numSymbols += j;
		}

	/// if compressing ///
	} else {
		// read in current byte (C) and increment frequency of 
		// read-in symbol. end loop if we're at the end of the data.
		while(fread(&C,1,1,in)) symtable[C]++;

		// write out frequencies at the top of the output file
		fwrite(&symtable, sizeof(int), 256, out);
	}

	// once we've reached the end of the data, 
	// put the symbols in a sorted linked list (L).
	L = createlist(symtable);
	//printlist(L);

	combine(&T, &L); // Combine the List nodes until they are a single Tree
	
	
	// compress or decompress here 
	// decompress
	if(argv[1][1] == 'd'){
		decompress(T, numSymbols, in, out);
	} else {
		huffTable H;
		memset(&H, 0, sizeof(huffTable));
		getHuffCodes(&H, T, 0, 0);
		/*
		for(i = 0; i < 256; i++){
			printf("%c %d %d\n", i, H.code[i], H.len[i]);
		}*/
		compress(H, in, out);
	}

	// cleanup
	destroyTree(&T);
	fclose(in);
	fclose(out);


	return 0;
}

// remove two leftmost list nodes from list, combine them, and add to tree 
void combine(node **T, node **L){

	if(*L == NULL) return;

	// node's A and B will be the left and right nodes which are
	// being combined. then the list's head is moved down two.
	node *A,*B;
	A = (*L);
	B = (*L)->next;
	(*L) = (*L)->next->next;
	
	// remove each leaf's linked list connections for tree (right ptr)
	A->next = B->next = NULL;
	
	// combined parent node is created
	node *C = (node *)calloc(1,sizeof(node));
	C->freq = A->freq + B->freq;
	C->is_leaf = 0;
	C->left = A;
	C->right = B;


	// List empty then set root of the Tree to the newly created node
	if(*L) {   // else sort new node back into List
		node *N = *L;
		node *P = NULL;
		while(N && (C->freq >= N->freq)){
			P = N;
			N = N->next;
		}

		if(P){
			P->next = C;
		} else {
			(*L) = C;
		}	
		C->next = N;
	}
	combine(T, L);
	if(*T == NULL){
		*T = C;
	}
	
}


// create a sorted list of all the symbols in the symbol table
node *createlist(unsigned int *symtable)
{
	int i;
	node *head = NULL;

	// for all symbols in the symbol table
	for (i = 0; i < 256; i++)
	{
		// if symbol is used, add to linked list
		if (symtable[i]) {
			//L->size++;  // increment # symbols in list
			// create current node and fill with info
			node *C = (node *)calloc(1,sizeof(node));
			C->symbol = i;
			C->freq = symtable[i];
			C->is_leaf = 1;

			// if first node, make head of list
			if (!head) head = C;
			else {
				node *N = head;
				node *P = NULL;
				// while next node exists, the frequency of the
				// current node is greater than or equal to the next node and
				// the symbol of the current node is greater than the next node
				// set previous to next and next to the following node.
				while(N && (C->freq >= N->freq) && (C->symbol > N->symbol)) {
					P = N;
					N = N->next;
				}

				// if there's a next node whose freq is larger than C's
				if (N) {
					// if P wasnt set, then that means head's freq <=
					// C's freq, so we can set C as the new head. 
					if (!P) {
						C->next = head;
						head = C;
					}
					// else we can put C in between P and N
					else {
						C->next = N;
						P->next = C;
					}
				}
				// if theres not an N, we've reached the end of the
				// list so just add C on the end.
				else P->next = C;
			}
		}
	}
	return head;
}

void printlist(node *head)
{
	node *current = head;
	printf("LIST:\n");
	while (current) {
		printf("\"%c\"(%d) -> ", current->symbol, current->freq);
		current = current->next;
	}
	printf("\n");
}

void printtree(node *T, int level) 
{
	int i;
	if (T == NULL) return;
	printtree(T->right, level+1);
	for (i=0; i<level; i++) printf("     ");  /* 5 spaces */
	printf("%5d-%c\n", T->freq, T->symbol);  /* field width is 5 */
	printtree(T->left, level+1);
}


void getHuffCodes(huffTable *H, node *T, unsigned short len, unsigned int code){
	
	// if at a leaf node, add code to huffTable
	if(T->is_leaf){
		H->code[T->symbol] = code;
		H->len[T->symbol] = len;
		return;
	}

	unsigned int newCode;
	newCode = (code << 1) & ~0x1; // 0b1110 going left add 0 to code
	getHuffCodes(H, T->left, len+1, newCode);
	newCode = (code << 1) | 0x1; // 0b0001 going right add 1 to code
	getHuffCodes(H, T->right, len+1, newCode);
}


void compress(huffTable H, FILE *in, FILE *out){

	unsigned int buffer = 0; // 4 bytes for buffer
	const int BUFFER_FULL = sizeof(int) * 8;
	int bitsUsed = 0, 
		bitsLeft = 0, 
		usableBits = 0,
		bufferShift = 0;
	unsigned char C, toWrite;

	rewind(in); // back to beginning
	while(fread(&C, 1, 1, in)){
		bitsLeft = H.len[C];

		while(bitsLeft > 0){					// any bits left from the previous symbol?
			if(bitsUsed == BUFFER_FULL){
				toWrite = buffer>>((BUFFER_FULL-8));	// write 1 byte 
				fwrite(&toWrite, 1, 1, out);
				bitsUsed -= 8; // free 1 byte of the buffer for use
			}
			usableBits = BUFFER_FULL - bitsUsed;

			// shift the buffer accordingly to usableBits
			bufferShift = (usableBits > bitsLeft)?bitsLeft:usableBits;
			bitsUsed += bufferShift;
			buffer <<= bufferShift;
			// update bits of current symbol remaining
			bitsLeft = (usableBits > bitsLeft)?0:(bitsLeft-usableBits);
			buffer |= H.code[C] >> bitsLeft; // write what can be written to buffer
		}
	}

	buffer <<= BUFFER_FULL-bitsUsed;
	bitsUsed = 32;
	// write out whatever is left in the buffer
	while(bitsUsed > 0){
		toWrite = buffer>>(bitsUsed-8);
		fwrite(&toWrite, 1, 1, out);
		bitsUsed -= 8;
	}
}

void decompress(node *T, int numSymbols, FILE* in, FILE* out){
	unsigned char C = 0, bit = 0;
	unsigned char Sym;
	int i;
	node * curr = T;
	while(numSymbols != 0){
		fread(&C, 1, 1, in);
		for(i = 0; i < 8; i++){
			bit = (C & (0x80>>i))>>(7-i); // read left to right mask with 0b1000 0000
			
			if(bit == 1){
				curr = curr->right;
			} else {
				curr = curr->left;
			}

			// if you reach a leaf, write out the symbol
			// and reset curr to the root of the tree
			if(curr->is_leaf){
				Sym = curr->symbol;
				fwrite(&Sym, 1, 1, out);
				curr = T;
				numSymbols--;
			}
			if(numSymbols == 0) break;
		} // end for
	} // end while

}

void destroyTree(node **T){
	if(*T == NULL) return;
	destroyTree( &((*T)->left) );
	destroyTree( &((*T)->right) );
	free(*T);
}
