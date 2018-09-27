# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define CHUNK 512	 // read 512 bytes at a time

int main(int argc, char **argv)
{
	// Initialize Variables
	FILE *fp, *fpn;
	int size = CHUNK;;
	char *buf;

	// Check arguments
	if(argc != 3){
		perror("Wrong number of command line arguments\n");
		printf("usage: myprog <input file> <output file>");
		return -1;
	}

	// Open File
	fp = fopen(argv[1], "rb");
	fpn = fopen(argv[2], "wb+");

	// Check if file opened successfully
	if(ferror(fp)){
		fprintf(stderr, "Error opening existing file '%s'", argv[1]);
		return -1;
	}

	if(ferror(fpn)){
		fprintf(stderr, "Error opening new file '%s'", argv[2]);
		return -1;
	}

	// Allocate memory for the file chunks
	buf = (char*) malloc(sizeof(char) * (size + 1)); // Casts allocated memory to char type

	// Check if memory allocation failed
	if(buf == NULL){
		perror("Memory could not be allocated");
		return -1;
	}

	char *token;
	const char sep[3] = "\t";
	int num;

	while(fgets(buf, size, fp) != NULL){

		token = strtok(buf, sep);
		while(token != NULL){
			num = atoi(token);

			if(fwrite(&num, sizeof(num), 1, fpn) == -1){
				return -1;
			}

			token = strtok(NULL, sep);
		}

	}

	// Close file and free mem
	fclose(fp);
	fclose(fpn);
	free(buf);

	return 0;
}
