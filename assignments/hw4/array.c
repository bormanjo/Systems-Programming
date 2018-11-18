#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"

void fill_array(float array[], size_t N, int consecutive){
	// Fills an array with random floating point numbers
	
	if(consecutive == 1){
		for(float i = 0.0; i < N; i++){
			array[(int) i] = i;
		}
	} else {		
		// initialize rand with time-based seed
		srand((unsigned) time(0));
		
		// fill array
		for(int i = 0; i < N; i++){
			array[i] = rand();
		}
	}
}

void print_matrix(float *array, int n_rows, int n_cols){
	// Prints an array in matrix form provided the number of rows and columns
	// 	- Assumes the size of the array N = n_rows * n_cols
	
	for(int row = 0; row < n_rows; row++){
		printf("row %d: ", row);
		for(int col = 0; col < n_cols; col++){
			printf("%0.2f \t", array[(row * n_cols) + col]);
		}
		printf("\n");
	}
}