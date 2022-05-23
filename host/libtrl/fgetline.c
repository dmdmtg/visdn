#include <stdio.h>

#ifndef EOF
#define EOF	-1
#endif

	int
fgetline(s, lim, stream)	/* get line into s, return length */
char s[];
int lim;
FILE *stream;
{
	int c, i;

	i = 0;
	while (--lim > 0 && (c=fgetc(stream)) != EOF && c != '\n')
		s[i++] = c;
	if (c == '\n' && i == 0)
		s[i++] = '\0';
	s[i] = '\0';
	return(i);
}
