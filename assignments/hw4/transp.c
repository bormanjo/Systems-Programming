#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "array.h"

const int debug = 0;		// 1: Print debug info
const int consecutive = 0;	// 1: Fill matrix with sequential values

void swap(float A[], int i, int j){
	// Swap elements i, j in A via tmp
	
	// Ensure that swap occurs only once
	if(j <= i){ // Swap elements in upper diagonal
		
		if(debug) printf("Swapping elements (%d, %d)\n", i, j);
		
		float tmp = A[i];
		A[i] = A[j];
		A[j] = tmp;
	}
}

// Transpose
int transpose(float A[], int n, int m){
	// A[] 	- The 1D array containing matrix data
	// n 	- The integer denoting the square size of the matrix
	// m	- The integer denoting the square size of the block
	
	if(debug) printf("Starting transpose\n");
	
	// Loop by block
	for(int x = 0; x < n; x+=m){
		for(int y = 0; y < n; y+=m){
			
			if(debug){
				printf("Transposing block at (%d, %d):\n", x, y);
				print_matrix(A, n, n);
			}
			
			// Loop by row, col in block
			for(int c = x; c < x + m; c++){ // Cols, rows must be less than matrix bounds
				for(int r = y; r < y + m; r++){
					swap(A, (r * n) + c, (c * n) + r);
				}
			}
			
			if(debug){
				printf("Block transposed:\n");
				print_matrix(A, n, n);
				printf("+--------------------------------+\n");
			}
		}
	}
}


int main(int argc, char *argv[]){

	float *A;					// The matrix array
	int N, M;					// The matrix size, block size
	struct timeval t1, t2;		// Time 1 (start), 2 (end)
	unsigned long sec, msec;

	// Parse inputs
	if(argc != 3){
		fprintf(stderr, "Usage: transp <mat_width> <block>\n");
		return EXIT_FAILURE;
	} else {
		N = atoi(argv[1]);
		M = atoi(argv[2]);
		
		if(N < 2 || M < 2){
			fprintf(stderr, "Usage: transp <mat_width> <block>\n\t<mat_width> must be >= 2\n\t<block> must be >= 2\n");
			return EXIT_FAILURE;
		}
	}
	
	if(debug){
		printf("Params:\n\tN: %d\n\tM: %d\n", N, M);
		printf("Allocating array\n");
	}

	// Allocate Array
	A = malloc( sizeof(float) * ((N * N) + 1));

	if(debug) print_matrix(&A[0], N, N);
	
	// Fill Array with random floats (consecutive = 0)
	fill_array(A, N*N, consecutive);

	if(debug){
		printf("Check Matrix:\n");
		print_matrix(A, N, N);
		printf("Starting timer\n");	
	}

	// Start timer
	gettimeofday(&t1, NULL);
	
	// Transpose
	transpose(A, N, M);
	
	// Stop timer
	gettimeofday(&t2, NULL);
	if(debug) printf("Stopping timer\n");
	
	sec = t2.tv_sec - t1.tv_sec;
	msec = (sec * 1000000) + t2.tv_usec - t1.tv_usec;
	
	if (debug){ 
		fprintf(stdout, "Transposing (%i x %i) Matrix in (%i x %i) Blocks took: %d microseconds\n", N, N, M, M, msec);
	} else {
		fprintf(stdout, "%d\n", msec);
	}
	// Deallocate Array
	free(A);
	
	// Return
	return EXIT_SUCCESS;
}