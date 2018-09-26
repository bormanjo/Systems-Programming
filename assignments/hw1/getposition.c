#include <stdio.h>
#include <stdlib.h>

/*
	The purpose of this program is to get the data located at the specified point in a given file
*/


int main(int argc, char **argv)
{
	// check arguments
	if(argc != 2){
		perror("Expected two arguments");
		return 1;
	}

	// Initialize variables
	FILE *fp;
	char *item;
	size_t n = 0;
	int c;
	long pos = 0;

	// Open and check file
	fp = fopen(argv[1], "r");
	if(ferror(fp)){
		perror("Error file could not be opened");
		return 1;
	}

	// Allocate memory
	item = malloc(100);

	// Get location in file:
	while((c = fgetc(stdin)) != 'e'){
		item[n++] = (char) c;
	}

	item[n] = '\0';
	pos = atoi(item);

	fseek(fp, pos, SEEK_SET);

	n = 0;

	while((c = fgetc(fp)) != '|'){
		if(c == EOF){
			perror("Error reached EOF");
			return 1;
		}

		item[n++] = (char) c;
	}

	printf("The data at: %d is:\n  '%s'\n", pos, item);

	// Free allocated mem
	free(item);

	return 0;
}
