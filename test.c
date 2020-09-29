#include <stdlib.h>
#include <stdio.h>

int main()
{
	char *s=malloc(10);
	free(s);
	return 0;
}	
