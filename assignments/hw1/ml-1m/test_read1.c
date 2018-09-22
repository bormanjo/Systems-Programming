# include <stdio.h>
# include <stdlib.h>

int main()
{
	FILE *fp;
	int c;

	fp = fopen("file.txt", "r");
	
	if(fp){
		while((c = getc(fp)) != EOF){
			putchar(c);
		}
		
		fclose(fp);
	}

	return 0;
}
