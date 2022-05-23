/* The X and DMD versions of this file are the same since the guts of the
 * phone logic are in here, and it's easier to maintain one file.
 * As such, you'll see lots of "#ifdef DMD" code.
 *
 * Eric Claeys, 1/95
*/

/*
 * Lamp module implementation
 *
 * Alan Hastings July 1990
 */

#ifdef	DMD630
#include "visdn.h"
#ifdef	USE730WINDOWS
#include <730windows.h>
#endif	/* USE730WINDOWS */
#else
#include "xisdn.h"
#endif	/* DMD630 */

extern void Lamp_On();
extern void Lamp_Off();
extern void Lamp_Inuse();
extern void Lamp_Flash();
extern void Lamp_Flutter();
extern ButLamp *Lamp_New();
extern void Lamp_Delete();
extern void Lamp_Ctrl();

/* called once at boot time */
#ifdef	DMD630
void Lamp_Init()
#else
void Lamp_Init(ofp)
FILE *ofp;
#endif	/* DMD630 */
{
	LAMP.On = Lamp_On;
	LAMP.Off = Lamp_Off;
	LAMP.Inuse = Lamp_Inuse;
	LAMP.Flash = Lamp_Flash;
	LAMP.Flutter = Lamp_Flutter;
	LAMP.New = Lamp_New;
	LAMP.Delete = Lamp_Delete;
	LAMP.Ctrl = Lamp_Ctrl;

	LAMP.featlamps = (ButLamp *)0;
	LAMP.localamps = (ButLamp *)0;
#ifndef	DMD630
	LAMP.ofp = ofp;
#endif	/* not DMD630 */
}

void Lamp_On(num)
int num;
{
}

void Lamp_Off(num)
int num;
{
}

#ifndef	DMD630
void Lamp_OnRed(num)
int num;
{
	fprintf(LAMP.ofp, "xtent.XtLampRed: (%d, on)\n", num);
}

void Lamp_OffRed(num)
int num;
{
	fprintf(LAMP.ofp, "xtent.XtLampRed: (%d, off)\n", num);
}

void Lamp_OnGreen(num)
int num;
{
	if (num==0 || num > CallApps) {
		/* ECC: This isn't an error (we get it for num==0, 4, and 8
		 * every few minutes).
		*/
		fprintf(LAMP.ofp, "xtent.XtLampFeature: (%d, on)\n", num);
	} else
		fprintf(LAMP.ofp, "xtent.XtLampGreen: (%d, on)\n", num);
}

void Lamp_OffGreen(num)
int num;
{
	if (num==0 || num > CallApps) {
		fprintf(LAMP.ofp, "xtent.XtLampFeature: (%d, off)\n", num);
	} else
		fprintf(LAMP.ofp, "xtent.XtLampGreen: (%d, off)\n", num);
}
#endif	/* not DMD630 */

void Lamp_Inuse(num)
int num;
{
}

/* Only green (status) lamps flash */
void Lamp_Flash(num)
int num;
{
#ifndef	DMD630
	if (num==0 || num > CallApps) {
		fprintf(LAMP.ofp, "xtent.XtLampFeature: (%d, flash)\n", num);
	} else
		fprintf(LAMP.ofp, "xtent.XtLampGreen: (%d, flash)\n", num);
#endif	/* not DMD630 */
}

/* Only green (status) lamps flutter */
void Lamp_Flutter(num)
int num;
{
#ifndef	DMD630
	if (num==0 || num > CallApps) {
		fprintf(LAMP.ofp, "xtent.XtLampFeature: (%d, flutter)\n", num);
	} else
		fprintf(LAMP.ofp, "xtent.XtLampGreen: (%d, flutter)\n", num);
#endif	/* not DMD630 */
}

ButLamp *
#ifdef	DMD630
Lamp_New(num, bp)
FeatBut *bp;
#else
Lamp_New(num)
#endif	/* DMD630 */
int num;
{
	register ButLamp *lp;

	lp = new(ButLamp);
	lp->num = num;
	lp->iuse_status = lp->stat_status = 0;
	lp->time_cnt = 0;
#ifdef	DMD630
	lp->but = bp;
	lp->timer = (TimerItem *)0;
#endif	/* DMD630 */

	lp->next = LAMP.featlamps;
	LAMP.featlamps = lp;

	return lp;
}

void Lamp_Delete(lp)
register ButLamp *lp;
{
	register ButLamp *olp, *tlp;

	if (LAMP.featlamps==(ButLamp *)0 || lp==(ButLamp *)0)
		return;

#ifdef	DMD630
	LPRINTF("Lamp_Delete %d (%s)\n", lp->num, lp->but->name);
#endif	/* DMD630 */

	if (lp==LAMP.featlamps)
		LAMP.featlamps = LAMP.featlamps->next;
	else for (olp=LAMP.featlamps, tlp=LAMP.featlamps->next;
		  tlp;
		  olp=tlp, tlp=tlp->next)
			if (lp==tlp)
			{
				olp->next = lp->next;
				break;
			}
}

void Lamp_Ctrl(ltype, num, code, duration)
int ltype;
register int num;
int code, duration;
{
	register ButLamp *lp;
	ButLamp *Lamp_Look();

	LPRINTF("Lamp_Ctrl %d %x %d %x\n", ltype, num, code, duration);

	if (duration != 0)
		return;		/* not interested in transient indications */

#ifdef	DMD630 /* XXXXXXXXXXXXXXXXXXXX why are these different ???? */
	if ((lp = Lamp_Look(num)) != (ButLamp *)NULL)
	{
		Lamp_Do(ltype, lp, num, code);
	}
#else
	if ((lp = Lamp_Look(num)) == (ButLamp *)NULL)
	{
		lp = Lamp_New(num);
	}
	Lamp_Do(ltype, lp, num, code);
#endif	/* DMD630 */
}

ButLamp *Lamp_Look(num)
register int num;
{
	register ButLamp *lp;

	for (lp=LAMP.featlamps; lp; lp=lp->next)
		if (num==lp->num)
			break;

	return lp;
}

Lamp_Do(ltype, lp, num, code)
register int ltype;
register ButLamp *lp;
int num, code;
{
	int changed = 0;

	LPRINTF("Lamp_Do ltype=%x num=%x iuse=%x stat=%x\n", ltype, num, lp->iuse_status, lp->stat_status);

#ifdef	DMD630 /* XXXXXXXXXXXXXXXXXXXXXXXXX why are these different ??? */
	if (ltype==LTYPE_INUSE || num > CallApps)
#else
	if (ltype==LTYPE_INUSE)
#endif	/* DMD630 */
	{
		switch (code)
		{
		case LAMP_ON:
			if ((lp->iuse_status&LFLAG_ON)==0)
				changed = 1;
			lp->iuse_status = LFLAG_ON;
#ifndef	DMD630
			Lamp_OnRed(num);
#endif	/* not DMD630 */
			break;
		case LAMP_OFF:
			if (lp->iuse_status&LFLAG_ON)
				changed = 1;
			lp->iuse_status = 0;
#ifndef	DMD630
			Lamp_OffRed(num);
#endif	/* not DMD630 */
			break;
		}
	} else
	{
		switch (code)
		{
		case LAMP_ON:
			if ((lp->stat_status&LFLAG_ON)==0)
				changed = 1;
			lp->stat_status = LFLAG_ON;
#ifdef	DMD630
			if (lp->timer)
				TimerRemove(lp->timer);
			lp->timer = (TimerItem *)0;
#else
			Lamp_OnGreen(num);
#endif	/* DMD630 */
			break;
		case LAMP_OFF:
			if (lp->stat_status&LFLAG_ON)
				changed = 1;
			lp->time_cnt = 0;
			lp->stat_status = 0;
#ifdef	DMD630
			if (lp->timer)
				TimerRemove(lp->timer);
			lp->timer = (TimerItem *)0;
#else
			Lamp_OffGreen(num);
#endif	/* DMD630 */
			break;
#ifdef	DMD630
		case LAMP_FLASH:
		case LAMP_FLUTTER:
			if (lp->timer==(TimerItem *)0)
			{
				extern int Lamp_Toggle();

				lp->timer = TimerAdd(Lamp_Toggle, (char *)lp);
				lp->time_cnt = 0;
				if (code==LAMP_FLASH)
				{
					lp->stat_status |= LFLAG_FLASH;
					if (((realtime()/TIMER_INT)%6)==0)
					{
						if ((lp->stat_status&LFLAG_ON)==0)
						{
							changed = 1;
							lp->stat_status |= LFLAG_ON;
						}
					} else
					{
						if (lp->stat_status&LFLAG_ON)
						{
							changed = 1;
							lp->stat_status &= ~LFLAG_ON;
						}
					}
				} else
				{
					lp->stat_status |= LFLAG_FLUTTER;
					if (((realtime()/TIMER_INT)%1)==0)
					{
						if ((lp->stat_status&LFLAG_ON)==0)
						{
							changed = 1;
							lp->stat_status |= LFLAG_ON;
						}
					} else
					{
						if (lp->stat_status&LFLAG_ON)
						{
							changed = 1;
							lp->stat_status &= ~LFLAG_ON;
						}
					}
				}
			}
			break;
#else
		case LAMP_FLASH:
			lp->stat_status = LFLAG_FLASH;
			Lamp_Flash(num);
			break;
		case LAMP_FLUTTER:
			lp->stat_status = LFLAG_FLUTTER;
			Lamp_Flutter(num);
			break;
#endif	/* DMD630 */
		case LAMP_WINK:		/* Added this for completeness */
		case LAMP_BFLUTTER:	/* Added this for completeness */
			/* There are no routines for these. */
			break;
		}
	}

#ifdef	DMD630
#ifdef	USE730WINDOWS
    /* When tiled, don't draw on the display */
    if( !ISTILED( P ) )
#endif /* USE730WINDOWS */
	if (changed)
		DispBut(lp->but, &BUT_FONT);

	if (num==MSGLAMPNO)
	{
		if (code==LAMP_ON && changed && SendAUDIXMsg)
			SendHost(PROC_PHONE, AUDIXMSG, (char *)0, EOM);
	}
#endif	/* DMD630 */
}

#ifdef	DMD630
int Lamp_Toggle(lp)
register ButLamp *lp;
{
	if (lp)
	{
		if ((lp->stat_status&LFLAG_FLASH)
		    && ((LastTime/TIMER_INT)%6)==0)
		{
			lp->stat_status ^= LFLAG_ON;
			DispBut(lp->but, &BUT_FONT);
		} else if (lp->stat_status&LFLAG_FLUTTER)
		{
			lp->stat_status ^= LFLAG_ON;
			DispBut(lp->but, &BUT_FONT);
		}
	}

	return 0;
}
#endif	/* DMD630 */
