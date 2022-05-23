h17597
s 00031/00000/00000
d D 1.1 92/07/23 13:58:53 trl 1 0
c date and time created 92/07/23 13:58:53 by trl
e
u
U
t
T
I 1
/*
 * DialPlanMod object interface
 *
 * DialPlanMod.Init()			- boot time init
 * DialPlanMod.Parse(str)		- parse specifications from startup file
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
	FUNC(void, Parse);		/* Parse(str) - parse spec from host */
	FUNC(char *, MapExch);		/* MapExch(area, exch) -local mapping */
PRIVATE
	Exchange *	exchanges;	/* list of exchange mappings */
	char *		areacode;	/* local areacode */
	char		tmpbuf[80];	/* temp buffer for mapping */
};

extern void DialPlanInit();
E 1
