/*
 * DialPlanMod implementation
 */

#include "visdn3.h"

void DPparse();
char *DPmapexch();

void DialPlanInit()
{
	DialPlan.Parse = DPparse;
	DialPlan.MapExch = DPmapexch;

	DialPlan.areacode = String(AREACODE);
	DialPlan.exchanges = (Exchange *)0;
}

void DPparse(s)
register char *s;
{
	char *area, *exch, *mapto;
	register Exchange *xp;

	if (s==NULL || *s=='\0')
		return;

	for (area=s; *s && *s != ';'; s++)
		;
	if (*s=='\0') return; else *s++ = '\0';
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

char *DPmapexch(area, exch)
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
			sprintf(DialPlan.tmpbuf, "%s", xp->mapto);
		} else
		{
			/* no mapping -- go outside for exchange */
			sprintf(DialPlan.tmpbuf, "%s+%s",Escape, exch);
	DPRINTF("\n\nEEEEEEEEEEEEEEEEscape = %s\n\n",Escape);
		}
	} else
	{
		/* different areacode -- return the whole thing */
		sprintf(DialPlan.tmpbuf, "%s+1%s%s",Escape, area, exch);
	DPRINTF("\n\nEEEEEEEEEEEEEEEEscape = %s\n\n",Escape);
	}

	return DialPlan.tmpbuf;
}
