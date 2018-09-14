#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>

#define SIZE 80000
#define TRUE 1
#define FALSE 0

typedef struct key_tag {
	unsigned char * key;
	int len;
} hashkey_t;

typedef struct entry_tag{
	hashkey_t * key;
	unsigned short outCode;
	struct entry_tag *next;
} entry_t;

typedef struct dict_tag{
	int num_entries;
	entry_t ** entries;
} dictionary_t;

/*
* Function prototypes
*/
int isEqual(hashkey_t *, hashkey_t *);
hashkey_t * makeKey(unsigned char*, int);
dictionary_t * makeDict();
int H(hashkey_t*);
void insert(dictionary_t *, entry_t *, hashkey_t*);
entry_t * isInDict(hashkey_t *, dictionary_t *);
void keyNCat(hashkey_t*, hashkey_t*, int);
void destroyKey(hashkey_t*);
void destroyDict(dictionary_t *);

void debug_print(dictionary_t *);

#endif