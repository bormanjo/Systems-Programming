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
		return(-1);
	}
	
	// Read next float into num
	while(fread(&num, 1, sizeof(num), f_in) == sizeof(num)){
		if (debug) printf("\t\t%f\n", num);
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

int is_file_sorted(FILE *fp){
	/* Checks that fp is sorted	(ascending)*/
	
	if(fp == NULL){
		perror("File could not be opened for verification");
		return -1;
	}
	
	// Go to file beginning
	rewind(fp);
	
	float num1, num2;
	
	// Read first num
	fread(&num1, 1, sizeof(num1), fp);
	
	// Read second num
	while (fread(&num2, 1, sizeof(num2), fp) == sizeof(num2)){
		
		// check ascending
		if (num1 <= num2){
			// File is still sorted
			num1 = num2;
		} else {
			// File is not sorted
			return -1;
		}
	}
	
	// File is sorted
	return 0;
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
			
			round += 1; 					// increment the round by 1
			cr_file_idx = 1;	 			// reset the file idx to starting positions
			nr_file_idx = 1;
		
		} else { 							// File fp1 Exists:
			
			// At least one file exists, so:
			
			chmod(s1, S_IRUSR);				// Change to read perms
			fp1 = fopen(s1, "rb");			// Read file
			
			chmod(s_next, S_IWUSR);			// Change to write perms
			fp_next = fopen(s_next, "wb");	// Create new fp_next file
			
			if( access(s2, R_OK) != 0){ 	// File fp2 DNE
				if (debug) printf("\tfp2 DNE: -> writing fp1 to f_next\n");
			
				// Write fp1 to fp_next
				fwr_float(fp1, fp_next);
			
				cr_file_idx += 1;			// increment by 1 for round-terminating base case
				
				// close files
				fclose(fp1);
				fclose(fp_next);
				
				// Delete file fp1
				remove(s1);

			} else { 						// File fp2 Exists:
				
				// If here, both files can assumed to be read at least once
				if (debug) printf("\tfp1 and fp2 Exist:\n");
				
				chmod(s2, S_IRUSR);		// Change to read perms
				fp2 = fopen(s2, "rb"); 	// Read 
					
				// get first number from both files
				fread(&fp1_num, sizeof(fp1_num), 1, fp1);
				fread(&fp2_num, sizeof(fp2_num), 1, fp2);
					
				while (1){ // Workhorse merging algorithm

					// Check if EOF after file read					
					if (feof(fp1) && feof(fp2)){
						// both files are EOF
						if (debug) printf("\tBoth files are EOF\n");
						break;
						
					} else {	// At least one file is not EOF
						
						if (feof(fp1)){ 				// End of fp1
							// Stop reading and write fp2 to fp_next
							if (debug) printf("\tfp1 EOF: -> writing fp2 to f_next\n");
							
							fwrite(&fp2_num, sizeof(fp2_num), 1, fp_next);		// Write current fp2_num
							fwr_float(fp2, fp_next);							// Write rest of fp2
							
							break;
			
						} else if (feof(fp2)){ 			// End of fp2
							// Stop reading and write fp1 to fp_next
							if (debug) printf("\tfp2 EOF: -> writing fp1 to f_next\n");
							
							fwrite(&fp1_num, sizeof(fp1_num), 1, fp_next);		// Write current fp1_num 
							fwr_float(fp1, fp_next);							// Write rest of fp1
							
							break;	
							
						} else { 						// Both files not at EOF
							if (fp1_num <= fp2_num){
								// write fp1_num to fp_next
								fwrite(&fp1_num, sizeof(fp1_num), 1, fp_next);
								
								if (debug) printf("\t%f < %f\n", fp1_num, fp2_num);
					
								// Read next fp1_num
								fread(&fp1_num, sizeof(fp1_num), 1, fp1);
					
							} else { // fp1_num > fp2_num
								// write fp2_num to fp_next
								fwrite(&fp2_num, sizeof(fp2_num), 1, fp_next);
						
								if (debug) printf("\t%f < %f\n", fp2_num, fp1_num);
								
								// Read next fp2_num
								fread(&fp2_num, sizeof(fp2_num), 1, fp2);

							}
						}
					}
				} // end reading loop
				
				if (debug) printf("\tOutput written to: %s\n", s_next);
			
				// close files
				fclose(fp1);
				fclose(fp2);
				fclose(fp_next);
				
				// Delete files fp1, fp2
				remove(s1);
				remove(s2);
			
				cr_file_idx += 2; 		// increment by 2 for next pair of files
				nr_file_idx += 1;		// increment by 1 for the next output file
			}
			
			//decrement by 1 as one loop processes 2 files into 1 file
			num_files--;
		}

		if (debug) printf("\tnum_files: %u\n", num_files);
	}
	
	sprintf(s_last, FNAME_TEMPLATE, round + 1, 1);
	
	FILE *fp = fopen(s_last, "rb");				// Open the last temp file
	FILE *fp_out = fopen(output_file, "wb"); 	// Open the output file
	
	chmod(s_last, S_IRUSR);
	chmod(output_file, S_IWUSR);
	
	fwr_float(fp, fp_out); // Write the contents of the last temp file to output file
	
	// close files
	fclose(fp);
	fclose(fp_out);
	
	// change perms
	chmod(output_file, S_IRUSR);
	
	// Delete file fp
	remove(s_last);
	
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
	
	FILE *fp = fopen(argv[2], "rb");
	
	if (is_file_sorted(fp) != 0){
		perror("Output file was not sorted");
		return -1;
	}
	
	return 0;
}