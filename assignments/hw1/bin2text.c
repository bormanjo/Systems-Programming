#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	The purpose of this program is to transform a specified binary file into text
	and write that text to a specified output file
*/

struct Entry{
	int user;
	int item;
	int rating;
	int timestamp;
};

int main(int argc, char **argv)
{
	// Initialize Variables
	FILE *fp, *fpn;
	struct Entry entry;

	// check arguments
	if(argc != 3){
		perror("Wrong number of command line arguments\n");
		printf("usage: bin2text <input file> <output file>");
		return -1;
	}

	// open file
	fp = fopen(argv[1], "rb");
	fpn = fopen(argv[2], "w+");

	// Check if files opened successfully
	if(ferror(fp)){
		fprintf(stderr, "Error opening existing file '%s'", argv[1]);
		return -1;
	}

	if(ferror(fpn)){
		fprintf(stderr, "Error opening new file '%s'", argv[2]);
		return -1;
	}

	// Read each line of the existing file into an Entry struct
	while(fread(&entry, sizeof entry, 1, fp) > 0 ){
		fprintf(fpn, "%d\t%d\t%d\t%d\n", entry.user, entry.item, entry.rating, entry.timestamp);
	}

	//close files
	fclose(fp);
	fclose(fpn);

	return 0;
}
