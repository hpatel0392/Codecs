#ifndef HUFFMAN_H
#define HUFFMAN_H

#include<stdio.h>

// data structures
typedef struct NODE {
	unsigned char  symbol;
	int			   freq, is_leaf;
	struct NODE *left;
	struct NODE *right;
	struct NODE *next;
} node;

// a 'bit' (Ha!) wasteful but allows O(1) access time when compressing
typedef struct HUFF {
	unsigned int code[256];
	unsigned short len[256];
} huffTable;

// function prototypes
node *createlist(unsigned int *symtable);
void printlist(node *L);
void combine(node **T, node **L);
void printtree(node *N, int level);
void getHuffCodes(huffTable *H, node *T, unsigned short len, unsigned int code);
void compress(huffTable H, FILE *in, FILE *out);
void decompress(node *T, int numSymbols, FILE* in, FILE* out);
void destroyTree(node **T);

#endif
