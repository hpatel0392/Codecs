#include <stdlib.h>
#include "table.h"

/*
* -------------------------------- Function definitions --------------------------------------
*/

//
int isEqual(hashkey_t * K1, hashkey_t * K2){
	if(K1->len != K2->len) return FALSE;
	int i;
	for(i = 0; i < K1->len; i++){
		if(K1->key[i] != K2->key[i])
			return FALSE;
	}
	return TRUE;
}


//
hashkey_t * makeKey(unsigned char * key, int len){
	hashkey_t * K = (hashkey_t*)calloc(1, sizeof(hashkey_t));
	K->key = (unsigned char*)calloc(len, 1);
	int i;
	for(i = 0; i < len; i++){
		K->key[i] = key[i];
	}
	K->len = len;
	return K;
}


//
dictionary_t * makeDict(){
	int i;
	dictionary_t * D;
	D = (dictionary_t*)calloc(1, sizeof(dictionary_t));
	D->entries = (entry_t**)calloc(SIZE, sizeof(entry_t*));
	for(i = 0; i < SIZE; i++){
		D->entries[i] = (entry_t*)calloc(1, sizeof(entry_t));
	}
	return D;
}


//
int H(hashkey_t * K){
    int h = 0;
    int i;

    for (i = 0; i < K->len; i++)
    {
        h += K->key[i];
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    if(h < 0) h *=-1;
    return h % SIZE;
}


//
void insert(dictionary_t * D, entry_t * E, hashkey_t * K){
	E->next = (entry_t*)calloc(1, sizeof(entry_t));
	E->next->key = K;
	E->next->outCode = ++D->num_entries;
}


//
entry_t * isInDict(hashkey_t * K, dictionary_t * D){
	int hash = H(K);
	entry_t * E;

	E = D->entries[hash];
    while( E->next != NULL && isEqual(E->next->key, K) == FALSE){
    	E = E->next;
    }
    return E;
}

//
void keyNCat(hashkey_t *dest, hashkey_t *src, int n){
	int i;
	for(i=0; i<n; i++){
		dest->key[dest->len] = src->key[i];
		dest->len++;
	}
}


//
void destroyKey(hashkey_t* K){
	free(K->key);
	free(K);
}



//
void destroyDict(dictionary_t * D){
	int i;
	entry_t* E;
    for(i = 0; i < SIZE; i++){
       E = D->entries[i]->next;
       while(E != NULL){    
          destroyKey(E->key);
          D->entries[i]->next = E->next;
          free(E);
          E = D->entries[i]->next;
       }
       free(D->entries[i]);
   }
   free(D->entries);
   free(D);
}

void debug_print(dictionary_t *D){
	int i, j;
  	entry_t* E;
  	hashkey_t * K;
  	puts("\nDictionary Printout");
  	puts("-------------------------------------");
  	for(i = 0; i < SIZE; i++){
    	printf("%d: ", i);
     	E = D->entries[i]->next;
     	while(E != NULL){
     		printf("---> ");
     		K = E->key;
     		for(j = 0; j < K->len;j++){
        		printf("%c", K->key[j]);
        	}
        	printf(" [%d]", E->outCode);
        	E = E->next;
    	}
    	puts("");
	}
	puts("-------------------------------------\n");
}