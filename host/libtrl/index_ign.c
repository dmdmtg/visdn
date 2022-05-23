/* index_ign() - return db_index of t in s, -1 if none */

#ifdef __cplusplus
#include <ctype.h>
#endif
#ifdef __MSDOS__
#define	OSTOUPPER	_toupper
#else
#define	OSTOUPPER	toupper
#endif
#ifndef NULLCH
#define	NULLCH	'\0'
#endif
#ifndef FAILURE
#define	FAILURE	-1
#endif
#ifndef YES
#define	YES	1
#endif

index_ign( ign_caps,s, t)
int ign_caps;
char s[], t[];
{
	register int i, j, k;

	for (i = 0; s[i] != NULLCH; i++)
	{
	if ( ign_caps == YES)
		for (j=i, k=0; t[k] != NULLCH &&
			(s[j] == t[k] || s[j] == OSTOUPPER(t[k]) ||
				OSTOUPPER(s[j]) == t[k]); j++, k++)
			;
	else
		for (j=i, k=0; t[k] != NULLCH && s[j] == t[k]; j++, k++)
			;

	if (t[k] == NULLCH)
		return(i);	/* match found */
	}

	return(FAILURE);	/* NO match found */
}
