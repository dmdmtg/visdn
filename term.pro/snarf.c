/* The X and DMD versions of this file are the same.
 * As such, you'll see lots of "#ifdef DMD630" code.
 *
 * Eric Claeys, 1/95
*/

#ifdef	DMD630
#include "visdn.h"
#else
#include "xisdn.h"
#endif	/* DMD630 */

#include "reggie.i"

extern int SnarfQuery();

#ifdef	DMD630
/*
 * The "it" parameter is only tested as non-zero!!!
 * It can't be an "int", though, since the menu function calls it with MainItem *.
 */
#endif
int
SnarfDial( snarfbuf, it )
char *snarfbuf;
#ifdef	DMD630
register MainItem *it;
#else
int	it;
#endif	/* DMD630 */
{
	extern char *regex(), *__loc1;
	char area[8], exch[8], ext[8];
	char *re;

	area[0] = exch[0] = ext[0] = '\0';
	re = regex(snarfstr, snarfbuf, area, exch, ext);
/*XXXYYY
MPRINTF("SnarfDial(): __loc1=0x%lx (%s), re=0x%lx (%s)\n",
(long)__loc1, __loc1 ? __loc1 : "null",
(long)re,  re ? re : "null");
XXXYYY*/

/* XXXXXXXX     See the comment in Hayes.c for weirdness about regex().  */
	if (re == (char *)NULL) {
		MPRINTF("SnarfDial(%s, %d): regex() failed\n", snarfbuf, it);
		return 0;
	}

	MPRINTF("SnarfDial(%s) regex returned: %s %s %s\n", snarfbuf, area, exch, ext);

	/* If "it" was set, then actually dial the number.
	 * Otherwise, we were just checking to see if there was a valid
	 * number in the snarf buffer.
	*/
	if (it)
	{
		char dialbuf[50];
#ifndef	DMD630	/* already defined */
		extern	DialPlanMod	DialPlan;
#endif	/* not DMD630 */

/* XXX FIX ??
 * This ignores any characters after the phone number,
 * like ",+" etc., and it ignores any additional
 * digits after the phone number.
 * If someone wants to dial "(708) 979-2886+123",
 * the "+123" are ignored.
 * IS THIS A PROBLEM?
*/
		(void) sprintf(dialbuf, "CA%s%s",
			DialPlan.MapExch(*area ? area : DialPlan.areacode, exch), ext);
		OutEnQueue(&PhoneOutQueue, dialbuf);
	}

	return 1;
}
