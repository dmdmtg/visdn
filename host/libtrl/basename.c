/* basename - find the base name of a pathname
 *
 * type:	fcn
 *
 * written by:	TR Lowe 851114
 */

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

basename( full, file )
char *full, *file;
{
	register int i, lastsep;

	lastsep = -1;

	for ( i = 0 ; full[i] != NULLCH ; i++ )
	{
		if ( full[i] == FILE_SEP )
			lastsep = i;
	}

	if ( lastsep < 0 )
		strcpy(file, full);
	else
		strcpy(file, &(full[lastsep+1]));
}
