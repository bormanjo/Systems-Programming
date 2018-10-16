#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[]){
	
	if (argc != 2){
		perror("Usage: bin2float <filename>");
		return -1;
	}
	
	FILE *fp = fopen(argv[1], "rb");
	float num;
	int count = 1;
	if (fp == NULL){
		fprintf(stderr, "Could not open file at: %s\n", argv[1]);
		return -1;
	}
	
	printf("Floating numbers:\n");
	while(0){
		fread(&num, sizeof(num), 1, fp);
		
		if(feof(f_in) != 0){
			break;
		}
		
		printf("\t%f, %d\n", num, count++);
	}
	
	return 0;
}