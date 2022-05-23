/*
 * DialPlanMod implementation	- 730 version
 */

#include "visdn.h"

static void
DPparse(s)
register char *s;
{
	char *area, *exch, *mapto;
	register Exchange *xp;

	if (s==NULL || *s=='\0')
		return;		/* probably an internal coding error */

	/* Format of "s":
	 *	[areacode];[exchange];\n
	 * where "exchange" is:
	 *	exchange[->mapto]
	 * e.g.,
	 *	979->9
	*/
	for (area=s; *s && *s != ';'; s++)
		;
	if (*s=='\0')
		return;		/* no ";" found -- probably an internal error */

	*s++ = '\0';
	for (exch=s, mapto=NULL; *s && *s != ';'; s++)
	{
		if (*s=='-')
		{
			*s++ = '\0';
			if (*s=='>')
				s++;
			mapto = s;
		}
	}
	*s = '\0';

	DPRINTF("DialInfo '%s' '%s'->'%s'\n", area, exch, mapto?mapto:"");

	if (*area)
	{
		if (DialPlan.areacode)
			free(DialPlan.areacode);
		DialPlan.areacode = String(area);
	}

	if (*exch)
	{
		for (xp=DialPlan.exchanges; xp; xp=xp->next)
			if (strcmp(xp->exch, exch)==0)
			{
				if (strcmp(xp->mapto, mapto)==0)
					return;	/* nothing new here */
			}
		if (xp==(Exchange *)0)
		{
			/* new exchange */
			xp = new(Exchange);
			xp->exch = String(exch);
			xp->next = DialPlan.exchanges;
			DialPlan.exchanges = xp;
		}
		else
		{
			/* mapping was changed */
			free(xp->mapto);
		}
		xp->mapto = String(mapto);
	}
}

static char *
DPmapexch(area, exch)
char *area, *exch;
{
	register Exchange *xp;

	if (strcmp(area, DialPlan.areacode)==0)
	{
		for (xp=DialPlan.exchanges; xp; xp=xp->next)
			if (strcmp(exch, xp->exch)==0)
				break;
		if (xp)
		{
			/* mapping found -- no need to go outside */
			(void) sprintf(DialPlan.tmpbuf, "%s", xp->mapto);
		} else
		{
			/* no mapping -- go outside for exchange */
			(void) sprintf(DialPlan.tmpbuf, "%s%s", Escape, exch);
		}
	} else
	{
		/* different areacode -- return the whole thing */
		(void) sprintf(DialPlan.tmpbuf, "%s1%s%s", Escape, area, exch);
	}

	return (DialPlan.tmpbuf);
}

void DialPlanInit()
{
	DialPlan.Parse = DPparse;
	DialPlan.MapExch = DPmapexch;

	DialPlan.areacode = String(AREACODE);
	DialPlan.exchanges = (Exchange *)0;
}
