#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

int debug = 1;

char FNAME_TEMPLATE[] = "./temp/temp_%u_%u.bin";	
int MAX_LEN = 100;
/*
"temp_<r>_<idx>.bin" 
 - <r> current round number 
 - <idx> file number in round <r> of mergesort
*/

int fwr_float(FILE *f_in, FILE *f_out){
	/* Given two file pointers, writes the remaining floats of f_in to f_out */
	
	float num;	//Auxilliary variable for transferring floats
	
	// Check if files open
	if (f_in == NULL){
		perror("Input file could not be opened for copying");
		return(-1);
	}
	
	if(f_out == NULL){
		perror("Output file could not be opened for copying");
	}

	// While not end of file
	while(0){
		fread(&num, sizeof(num), 1, f_in);		// Read next float into num
		
		if(feof(f_in) != 0){
			break;
		}
		
		fwrite(&num, sizeof(num), 1, f_out);	// Write the float into f_out
	}
	
	// Return success
	return 0;
}

int ndigits(unsigned int num){
	/* Counts the number of digits in an (unsigned) integer */
	int count = 1;
	
	while(num > 9){
		num = num / 10;
		count++;
	}
	
	return count;
}

int oocmerge(unsigned int num_files, char* output_file){
	/* 
	Merges <num_files> in ./temp containing floating point numbers a writes the sorted numbers into one output_file
	Does so in an out-of-core fashion such that minimal memory is utilized
	*/ 
	
	// Define:
	unsigned int round = 1;								//  - starting round number
	unsigned int cr_file_idx = 1, nr_file_idx = 1;		//  - current and next round file indices
	int reading;										//  - 1 for currently reading, 0 otherwise
	float fp1_num, fp2_num; 							//  - storage for file numbers

	// file strings must be dynamically allocated for largest values
	char s1[MAX_LEN], s2[MAX_LEN], s_next[MAX_LEN], s_last[MAX_LEN];			

	// file pointers
	FILE *fp1, *fp2, *fp_next;
	
	while (num_files > 0){
		
		// Generate file names:
		sprintf(s1, FNAME_TEMPLATE, round, cr_file_idx);
		sprintf(s2, FNAME_TEMPLATE, round, cr_file_idx + 1);
		sprintf(s_next, FNAME_TEMPLATE, round + 1, nr_file_idx);
		
		if (debug) printf("Round %u: (fp1: %s, fp2: %s):\n", round, s1, s2);
		
		if( access(s1, R_OK) != 0) { 	// File fp1 DNE
			if (debug) printf("\tfp1 DNE\n");
		
			// We are at round end (since the num files is still > 1)
			
			round += 1; 				// increment the round by 1
			cr_file_idx = 1;	 		// reset the file idx to starting positions
			nr_file_idx = 1;
		
		} else { 						// File fp1 Exists:
			
			// At least one file exists, so:
			fp1 = fopen(s1, "rb");
			fp_next = fopen(s_next, "wb");	// Create new fp_next file
			
			if( access(s2, R_OK) != 0){ // File fp2 DNE
				if (debug) printf("\tfp2 DNE\n");
			
				// Write fp1 to fp_next
				fwr_float(fp1, fp_next);
			
				cr_file_idx += 1;	// increment by 1 for round-terminating base case
				
				// close files
				fclose(fp1);
				fclose(fp_next);

			} else { 					// File fp2 Exists:
				
				// If here, both files can assumed to be read at least once
				fp2 = fopen(s2, "rb");
				reading = 1;
				
				// get first number from both files
				fread(&fp1_num, sizeof(fp1_num), 1, fp1);
				fread(&fp2_num, sizeof(fp2_num), 1, fp2);
			
				while (reading == 1){ // Workhorse merging algorithm
				
					// Compare file arrays one by one
				
					if (fp1_num <= fp2_num){
						// write fp1_num to fp_next
						fwrite(&fp1_num, sizeof(fp1_num), 1, fp_next);
						
						if (debug) printf("\t%f < %f\n", fp1_num, fp2_num);
					
						if (feof(fp1) != 0){ // End of fp1
							// Stop reading and write fp2 to fp_next
							reading = 0; 
							fwr_float(fp2, fp_next);
			
						} else {
							// Read next float from fp1
							fread(&fp1_num, sizeof(fp1_num), 1, fp1);
						}
					} else { // fp1_num > fp2_num
						// write fp2_num to fp_next
						fwrite(&fp2_num, sizeof(fp2_num), 1, fp_next);
						
						if (debug) printf("\t%f < %f\n", fp2_num, fp1_num);
					
						if (feof(fp2)){
							// Stop reading and write fp1 to fp_next
							reading = 0;
							fwr_float(fp1, fp_next);
							
						} else {
							// Read next float from fp2
							fread(&fp2_num, sizeof(fp2_num), 1, fp2);
						}
					}
						
				} // end reading loop
				
				if (debug) printf("\tOutput written to: %s\n", s_next);
			
				// close files
				fclose(fp1);
				fclose(fp2);
				fclose(fp_next);
			
				cr_file_idx += 2; 		// increment by 2 for next pair of files
				nr_file_idx += 1;		// increment by 1 for the next output file
			}
		}
		
		//decrement by 1 as one loop processes 2 files into 1 file
		num_files--;
		if (debug) printf("\tnum_files: %u\n", num_files);
	}
	
	sprintf(s_last, FNAME_TEMPLATE, round + 1, 1);
	
	FILE *fp = fopen(s_last, "rb");
	
	if (fp == NULL){
		fprintf(stderr, "Could not read file at: %s\n", s_last);
	}
	
	// get first number of the last file
	fread(&fp1_num, sizeof(fp1_num), 1, fp);
	
	if (debug) printf("Checking output file:\n");
	
	while (feof(fp) == 0){
		// get the next number
		fread(&fp2_num, sizeof(fp2_num), 1, fp);
		
		// check that first < second
		if (fp1_num > fp2_num){
			fprintf(stderr, "Final temp file was not sorted: (%f, %f)\n", fp1_num, fp2_num);
			return -1;
		} else {
			// Move to the next number
			if (debug) printf("\t%f\n", fp1_num);
			fp1_num = fp2_num;
		}
	}
	
	if (debug) printf("Final temp file was sorted!\n");
	
	// file is sorted
	rewind(fp); // move fp back to start
	
	FILE *fp_out = fopen(output_file, "wb"); // Open the output file
	
	fwr_float(fp, fp_out); // Write the contents of the last temp file to output file
	
	// close files
	fclose(fp);
	fclose(fp_out);
	
	return 0;
}

int get_rand_files(unsigned int N){
	/*
	Writes <N> random floating point integers each to its own binary file in ./temp
	*/
	
	const char tmpdir[] = "./temp";
	struct stat sb;
	
	// check if directory exists
	if(stat(tmpdir, &sb) == -1){
		// dir doesnt exist -> make directory
		mkdir(tmpdir, S_IRWXU); // user only access settings
	}
	
	srand(time(0)); // set seed for rand()
	
	char s[MAX_LEN];
	float rnum;
	float min = -100.0, max = 100.0;
	FILE *fp;
	
	if (debug) printf("Writing random files to: %s\n", FNAME_TEMPLATE);
	
	for (unsigned int i = 1; i <= N; i++){
		
		// random num
		rnum = min + (rand() / (float) RAND_MAX) * (max - min); // rand() / RAND_MAX -> [0.0, 1.0] -> [min, max]
		
		// file names:
		sprintf(s, FNAME_TEMPLATE, 1, i);
		
		// open file
		fp = fopen(s, "wb");

		if (fp == NULL){
			fprintf(stderr, "Could not open file at: %s", s);
			return -1;
		}
		
		if (debug) printf("\t%s <- %f\n", s, rnum);
		
		// write random float to file
		fwrite(&rnum, sizeof(rnum), 1, fp);
		
		// close file
		fclose(fp);
	}	
	
	return 0;
}

int main(int argc, char *argv[]){
	
	if (argc != 3){
		perror("Usage: oocmerge <N> <output file>");
		return -1;
	}
	
	unsigned int N = atoi(argv[1]);
	
	if (N < 1){
		perror("<N> must be a positive integer");
	}
	
	if (debug) printf("N: %u, output_file: %s\n", N, argv[2]);
	
	if (debug) printf("Generating random files\n");
	
	if (get_rand_files(N) != 0){
		perror("failed to generate random float files");
		return -1;
	}

	if (debug) printf("Starting OOC Mergesort\n");
	
	if (oocmerge(N, argv[2]) != 0){
		perror("oocmerge failed");
		return -1;
	}
	
	return 0;
}