/* This file is shared between the 730 and X versions.
 * Eric Claeys, 1/95
*/

#include <stdio.h>
#include <malloc.h>

char *Malloc(size)
unsigned size;
{
	register char *cp;

	if ((cp = malloc(size))==NULL)
	{
		(void) fprintf(stderr, "malloc failed for size %d\n", size);
		exit(3);
	}

	return cp;
}

void
Free(s)
char *s;
{
	if (s)
	    free(s);
}
