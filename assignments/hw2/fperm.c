#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


int main(int argc, char *argv[]){
	/* A simple program for changing the permissions of a file	*/

	for(int i = argc - 1; i > 0; i--){

		if (access(argv[i], R_OK) != 0){
			printf("Could not read: %s\n\tWriting file...\n", argv[i]);
			FILE *fp = fopen(argv[i], "wb");
			fclose(fp);
			chmod(argv[i], S_IRWXU);
		} else {
			printf("Chaning file perms: %s\n", argv[i]);
			struct stat st;
	    	mode_t mode;

		    stat(argv[i], &st);

	    	mode = st.st_mode & 0000;

 	   		// modify mode
 		   	//mode &= ~(S_IRUSR);    /* Clear this bit */
    		//mode |= S_IWUSR;       /* Set this bit   */

			chmod(argv[i], mode);
		}
	}

	return 0;
}