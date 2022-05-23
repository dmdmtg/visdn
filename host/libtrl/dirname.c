/* dirname - find the directory name of a pathname
 *
 * written by:	TR Lowe 890927
 */

#include <stdio.h>

#ifndef NULLCH
#define	NULLCH	'\0'
#endif

#ifndef FILE_SEP
#ifdef __MSDOS__
#define	FILE_SEP	'\\'
#else
#define	FILE_SEP	'/'
#endif
#endif

dirname( full, dir )
char *full, *dir;
{
	char *p, *strrchr();

	strcpy( dir, full );

	if  ( ( p = strrchr( dir, FILE_SEP ) ) == NULL )
		strcpy( dir, "." );
	else
		*p = NULLCH;
}
