# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define CHUNK 256	 // read 1024 bytes at a time

int main(int argc, char **argv)
{
	// Initialize Variables
	FILE *fp;
	long int size;
	char *buf;
	size_t result;

	// Check arguments
	if(argc != 2){
		perror("Wrong number of command line arguments\n");
		printf("usage: myprog <file>");
		return -1;
	}

	// Open File
	fp = fopen(argv[1], "r");

	// Check if file opened successfully
	if(ferror(fp)){
		perror("File error encountered");
		return -1;
	}

	// Get file size
	fseek(fp, 0, SEEK_END);
	size = CHUNK; //ftell(fp);
	rewind(fp);

	// Allocate memory for the file chunks
	buf = (char*) malloc(sizeof(char) * (size + 1)); // Casts allocated memory to char type

	// Check if memory allocation failed
	if(buf == NULL){
		perror("Memory could not be allocated");
		return -1;
	}

	// Read a chunk of the file
	result = fread(buf, 1, size, fp);

	char *token;
	const char sep[3] = "\t\n";

	printf("Size of buf: %d\n", sizeof buf);
	printf("Result: %d\n", result);
	printf("Size: %d\n", size);
	printf("String: '%s'\n", buf);
	printf("Tokens: \n");

	token = strtok(buf, sep);
	while(token != NULL){
		printf(" %s\n", token);
		token = strtok(NULL, sep);
	}

	// Close file and free mem
	fclose(fp);
	free(buf);

	return 0;
}
