/* The X and DMD versions of this file are the same.
 * Eric Claeys, 1/95
*/

/*
 * DialPlanMod object interface
 *
 * DialPlanMod.Init()			- boot time init
 * DialPlanMod.SetArea(str)		- set areacode
 * DialPlanMod.SetMap(str)		- setup an exchange mapping
 * DialPlanMod.SetPrefix(str)		- set outside line prefix
 * DialPlanMod.MapExch(area, exch)	- map exch for dialing plan
 *
 * The dialing plan includes the notion of the local areacode and possibly
 * 4 or 5 digit dialing within local exchanges.  This module keeps track
 * of the local areacode and any mappings for local exchanges.
 */

typedef struct Exchange {
	struct Exchange *	next;
	char *			exch;
	char *			mapto;
} Exchange;

MODULE(DialPlanMod)
{
PUBLIC
	FUNC(void, Init);		/* Init() - boot time initialization */
#ifdef	DMD630
	FUNC(void, Parse);		/* Parse(str) - parse spec from host */
#else
	FUNC(void, SetArea);		/* SetArea(str) - set areacode */
	FUNC(void, SetMap);		/* SetMap(str) - set exhange map */
	FUNC(void, SetPrefix);		/* SetPrefix(str) - set prefix */
#endif	/* DMD630 */
	FUNC(char *, MapExch);		/* MapExch(area, exch) -local mapping */
PRIVATE
	Exchange *	exchanges;	/* list of exchange mappings */
	char *		areacode;	/* local areacode */
#ifndef	DMD630
	char *		prefix;		/* prefix for escaping to outside net */
#endif	/* not DMD630 */
	char		tmpbuf[80];	/* temp buffer for mapping */
};

#ifdef	DMD630
extern void DialPlanInit();
#endif
