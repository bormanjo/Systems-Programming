#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "array.h"

const int debug = 1;		// 1: Print debug info
const int consecutive = 1;	// 1: Fill matrix with sequential values

int min(int a, int b){
	if(a < b){
		return a;
	} else {
		return b;
	}
}

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
int transpose(float A[], int w, int h, int m){
	// A[] 	- The 1D array containing matrix data
	// w 	- The integer denoting the width size of the matrix
	// h 	- The integer denoting the height size of the matrix
	// m	- The integer denoting the square size of the block
	
	if(debug) printf("Starting transpose\n");
	
	// Loop by block
	for(int x = 0; x < h; x+=m){
		for(int y = 0; y < w; y+=m){
			
			if(debug){
				printf("Transposing block at (%d, %d):\n", x, y);
				print_matrix(A, w, h);
			}
			
			// Loop by row, col in block
			for(int c = x; c < min(x + m, w); c++){ 	// Cols, rows must be less than matrix bounds
				for(int r = y; r < min(y + m, h); r++){
					swap(A, (r * w) + c, (c * h) + r);
				}
			}
			
			if(debug){
				printf("Block transposed:\n");
				print_matrix(A, h, w);
				printf("+--------------------------------+\n");
			}
		}
	}
}


int main(int argc, char *argv[]){

	float *A;					// The matrix array
	int W, H, M;				// The matrix width, height, and block size
	struct timeval t1, t2;		// Time 1 (start), 2 (end)
	unsigned long sec, msec;

	// Parse inputs
	if(argc != 4){
		fprintf(stderr, "Usage: transp <mat_width> <mat_height> <block>\n");
		return EXIT_FAILURE;
	} else {
		W = atoi(argv[1]);
		H = atoi(argv[2]);
		M = atoi(argv[3]);
		
		if( W < 1 || H < 1 || H < 2){
			fprintf(stderr, "Usage: transp <mat_width> <mat_height> <block>\n\t<mat_width>, <mat_height> must be >= 1\n\t<block> must be >= 2\n");
			return EXIT_FAILURE;
		}
		
		if((W * H) % (M * M)){
			fprintf(stderr, "Usage: transp <mat_width> <mat_height> <block>\n\t<block>s of size (%d, %d) do not fill matrix of size (%d, %d)\n", M, M, W, H);
			return EXIT_FAILURE;
		}
	}
	
	if(debug){
		printf("Params:\n\tW: %d\n\tH: %d\n\tM: %d\n", W, H, M);
		printf("Allocating array\n");
	}

	// Allocate Array
	A = malloc( sizeof(float) * ((W * H) + 1));

	if(debug) print_matrix(&A[0], W, H);
	
	// Fill Array with random floats (consecutive = 0)
	fill_array(A, W*H, consecutive);

	if(debug){
		printf("Check Matrix:\n");
		print_matrix(A, W, H);
		printf("Starting timer\n");	
	}
	
	// Start timer
	gettimeofday(&t1, NULL);
	
	// Transpose
	transpose(A, W, H, M);
	
	// Stop timer
	gettimeofday(&t2, NULL);
	if(debug) printf("Stopping timer\n");
	
	sec = t2.tv_sec - t1.tv_sec;
	msec = (sec * 1000000) + t2.tv_usec - t1.tv_usec;
	
	if (debug){ 
		fprintf(stdout, "Transposing (%i x %i) Matrix in (%i x %i) Blocks took: %d microseconds\n", W, H, M, M, msec);
	} else {
		fprintf(stdout, "%d\n", msec);
	}
	// Deallocate Array
	free(A);
	
	// Return
	return EXIT_SUCCESS;
}