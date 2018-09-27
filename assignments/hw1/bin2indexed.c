#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
	The purpose of this program is to take a binary file containing a user id, item id
	rating and timestamp and re-index the item id to the offset index of the movie
	corresponding to the original item id adn re-write the file
*/

// A structure for storing the line entries of the binary file
struct Entry{
	int user;
	int item;
	int rating;
	int timestamp;
};

// A function that returns the number preceding the next pipe (|)
const int get_item(FILE *fpc)
{
	// Initialize variables
	FILE *fp = fpc; // copy fpc
	char *item;     // Get pointer to item array
	size_t n = 0;	// Size of the item array
	int c;		// The char to read one at a time
	int result;	// The num to return

	// Allocate mem
	item = malloc(100);

	// Read each char in fp until the next pipe '|'
	while((c = fgetc(fp)) != '|'){
		
		item[n++] = (char) c; // store the chare in the item array
	}

	// Add terminating character
	item[n] = '\0';

	// Get result
	// printf("\t\tget_item: '%s'\n", item);
	result = atoi(item);

	// De-allocate memory
	free(item);

	// Return result
	return result;
}

// A function that returns the number of digits in a number
const int get_num_digits(long num)
{
	int count = 0;
	
	while(num > 9){
		num = num / 10;
	}
	
	return (int) num + 1;
}

// A function that returns the index of the provided item from the file
const long get_item_index(long item, FILE *fp)
{
	// Initialize file location to start
	fseek(fp, 0, SEEK_SET);

	// Initialize variables
	const int file_start = ftell(fp); 			// File location at start
	long cur_item = -1, cur_idx = file_start; 	// Curent item and index of current item
	int c;										// Char to be read
	char cur_c;									// Char to be compared with '\n'

	// To find the item index:
	do{
		
		// If at file start
		if(ftell(fp) == file_start){
			
			// Get the first item
			cur_item = get_item(fp);
		} else {
			
			// Otherwise -> somewhere else in file
			// Loop through chars until \n is found
			while((c = fgetc(fp)) != EOF){
				
				// Searching
				cur_c = (char) c;

				if(cur_c == '\n'){
					// Newline found, stop searching
					break;
				}
			}

			// If EOF was reached, throw error
			if(c == EOF){
				perror("Index not found");
				return -1;
			}

			// \n was found, now check for the current item
			cur_item = get_item(fp);
			
			// Get the current index
			cur_idx = ftell(fp);
			
		}
	} while (cur_item != item);

	if(cur_idx > 0){
		// subtract 1 for the pipe char and num_digits for the number of digits 
		cur_idx = cur_idx - get_num_digits(cur_item) - 1;
	}

	// return
	return cur_idx;
}

int main(int argc, char **argv)
{
	// Initialize Variables
	FILE *fp_bin, *fp_idx, *fpn_bin;
	struct Entry entry;

	// Check arguments
	if(argc != 4){
		perror("Wrong number of command line arguments\n");
		printf("usage: bin2indexed <binary file> <item file> <output filename>\n");
		return -1;
	}
	// Open files
	fp_bin = fopen(argv[1], "rb");
	fp_idx = fopen(argv[2], "r");
	fpn_bin = fopen(argv[3], "wb+");

	// Check if files opened successfully
	if(ferror(fp_bin)){
		fprintf(stderr, "Error opening existing file '%s'", argv[1]);
		return -1;
	}
	if(ferror(fp_idx)){
		fprintf(stderr, "Error opening existing file '%s'", argv[2]);
		return -1;
	}
	if(ferror(fpn_bin)){
		fprintf(stderr, "Error opening new file '%s'", argv[3]);
		return -1;
	}

	// Loop through binary file
	while(fread(&entry, sizeof entry, 1, fp_bin) > 0){ // Get the current entry

		//printf("\nCurrent Entry: %d\t%d\t%d\t%d\n", entry.user, entry.item, entry.rating, entry.timestamp);

		// Get the item's entry index
		entry.item = get_item_index(entry.item, fp_idx);

		//printf("\tIndex Item: %d\n", entry.item);

		// Write the indexed entry to the output binary file
		if(fwrite(&entry, sizeof(entry), 1, fpn_bin) == -1){
			perror("Error writing to file");
			return -1;
		}
	}

	// Close files
	fclose(fp_bin);
	fclose(fp_idx);
	fclose(fpn_bin);

	// Return
	return 0;
}
