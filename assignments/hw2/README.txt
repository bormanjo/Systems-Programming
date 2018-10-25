OUT OF CORE MERGESORT (OOC MERGESORT)

by John-Craig Borman (10402229)

Objective:
	- Create a program that implements the merge sort algorithm without storing all the data in memory
	
Usage:
	oocmerge <N> <output filename>
	- N - the number of floats to be sorted
	- output filename - the file to store the array of floats in binary
	
Process:
	X. Generate N floating point numbers between -100 and 100
		X). Use rand() to generate integers
		X). Divide integers by (RAND_MAX / floating point constant)
	X. Create a directory named 'temp'
		X). Change permissions to allow accessibility to ONLY the user
	X). Write temporary files as the medium to store intermediate arrays
		X). Change permissions of these files:
			+ Can only be written when used as output
			+ Can only be read when used as input
			+ Only the user has access
		X). Delete temporary files when no longer needed
	X. Verify that numbers in the output file are sorted
		X). Should be done without storing more than the minimum amount of data necessary
			+ Hold only two numbers in memory at a time
			
Steps:
	1. Implement flow of algorithms without actual sorting
		a). Print debugging messages
		b). Start with 8 numbers
	2. Identify method for using sprintf() to generate temp file names
		
		
Assumptions:
	1. Assume N is positive and less than or equal to 1000
	2. Sorting will be in ASCENDING order
	3. Arrays of any kind are not allowed
	4. All data is held in binary formats
