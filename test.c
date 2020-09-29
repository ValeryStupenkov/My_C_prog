#include <stdlib.h>
#include <stdio.h>

int main()
{
	char *s=malloc(10);
	printf("f");
	free(s);
	return 0;
}	
