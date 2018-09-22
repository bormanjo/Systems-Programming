# include <stdio.h>
# include <stdlib.h>

# define CHUNK 1024 // read 1024 bytes at a time

int main()
{
	FILE *fp;		// Allocate a file pointer (fp)
	char buf[CHUNK];	// Allocate a character vestor of size CHUNK
	size_t nread;		// Number of bytes read so far

	fp = fopen("file.txt", "r");
	
	if(fp){
		while((nread = fread(buf, 1, sizeof buf, fp)) > 0){
			fwrite(buf, 1, nread, stdout);
		}
		
		if(ferror(fp)){
			printf("File error encountered");
		}
		fclose(fp);
	}

	return 0;
}
