
Simple Matrix Transposition:
	- Objective:
		- Create a simple program that transposes a large, SQUARE matrix
	- Hints:
		- Exploit cache memory
		- Achieve locality by transposing in square blocks
		- Initialize the matrix with:
			- rand()
		- Allocate "matrix" as one large, contiguous array
	- Usage:
		> transpose <mat_width> <block>
		- mat_width - the width of the square matrix
		- block - the width of the block chunks to transpose
		- Expect both arguments to be powers of 2
	- Requirements:
		1. main file: transp.c
		2. Time ONLY the transposition operation using `gettimeofday()`
		3. Report the best block width for each size of the input matrix and a few values of the block width around the best values
			- table or graph
		4. Question?
			- How does the code become faster due to cache hits?
	- Methodology:
		> Transpose
			1. Identify the # of blocks in matrix
			2. For each block:
				- Transpose Block(block)
			3. Swap off diagonal blocks
		> Transpose Block(block)
			1. Transpose a square block
			
Arbitrary Matrix Transposition:
	- Objective:
		- Create a separate program that will transpose any arbitrary matrix by square blocks
	- Usage:
		> transpif.c <mat_width> <mat_height> <block>
		- mat_width, mat_height - The width and height of the arbitrary matrix
		- block - the width of the block chunks to transpose
	- Hints:
		- check boundary conditions
		- check that memory addresses are legal