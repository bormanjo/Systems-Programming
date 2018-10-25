/*
	Header file containing function and constant declarations
*/

// Func to copy the f_in to f_out starting from the given pointer
int fwr_float(FILE *f_in, FILE *f_out);
// Counts the number of digits in the given integer
unsigned int ndigits(unsigned int num);
// Checks if a file pointer is sorted
int is_file_sorted(FILE *fp);
// Generates N files containing N
int get_rand_files(unsigned int N);
// Perform the Out of core Mergesort and output to output_file
int oocmerge(unsigned int N, char* output_file);