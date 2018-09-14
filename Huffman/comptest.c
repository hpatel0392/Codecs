#include <stdio.h>
#include <stdlib.h>
	
int main(int argc, char const *argv[])
{
	FILE *in; 
	unsigned int C;
	int i;
	
	// open the input and output files w/ error checking
	if((in = fopen(argv[1], "rb")) == NULL){
		printf("Error opening input file\n");
		exit(1);
	}
	
	// read in and output frequencies
	for(i = 0; i < 256; i++) {
		fread(&C,4,1,in);
		printf("%d/%c:%d\n",i,i,C);
	}
	
	// read in and output bit pattern
	printf("OUTPUT:\n");
	while(fread(&C,2,1,in)) printf("%x",C);
	printf("\n");
	
	return 0;	
}
