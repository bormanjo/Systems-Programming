# include <stdlib.h>
# include <stdio.h>

# include "sorting.h"

int main(int argc, char *argv[]){
	
	if (argc != 3){
		perror("Usage: oocmerge <N> <output file>");
		return -1;
	}
	
	int N = atoi(argv[1]);
	
	if (N < 1){
		perror("<N> must be >= 1");
	}

	// Generate N random files
	if (get_rand_files(N) != 0){
		perror("failed to generate random float files");
		return -1;
	}

	// Sort N files
	if (oocmerge(N, argv[2]) != 0){
		perror("oocmerge failed");
		return -1;
	}
	
	FILE *fp = fopen(argv[2], "rb");
	
	// check that output file is sorted
	if (is_file_sorted(fp) != 0){
		perror("Output file was not sorted");
		return -1;
	}
	
	return 0;
}