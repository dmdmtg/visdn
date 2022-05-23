/*
 * Lamp module implementation
 *
 * Alan Hastings July 1990
 */

#include "visdn3.h"
#ifdef  USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */

extern void Lamp_On();
extern void Lamp_Off();
extern void Lamp_Inuse();
extern void Lamp_Flash();
extern void Lamp_Flutter();
extern ButLamp *Lamp_New();
extern void Lamp_Delete();
extern void Lamp_Ctrl();

/* called once at boot time */
void Lamp_Init()
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
}

void Lamp_On(num)
int num;
{
}

void Lamp_Off(num)
int num;
{
}

void Lamp_Inuse(num)
int num;
{
}

void Lamp_Flash(num)
int num;
{
}

void Lamp_Flutter(num)
int num;
{
}

ButLamp *Lamp_New(num, bp)
int num;
FeatBut *bp;
{
	register ButLamp *lp;

	lp = new(ButLamp);
	lp->num = num;
	lp->but = bp;
	lp->iuse_status = lp->stat_status = 0;
	lp->timer = (TimerItem *)0;
	lp->time_cnt = 0;

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
	LPRINTF("Lamp_Delete %d (%s)\n", lp->num, lp->but->name);
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

	if(ltype==LTYPE_LOCAL){
		/* fp3 local lamps include mute, select, speaker */
		/* this translates the type(local) and number to the */
		/* defines used for these lamps */
		if(num==100){
			num=SPKRLAMPNO; 
			LPRINTF("spkrlamp numb set to %d\n",num);
		}else if (num==1){
			num=MUTELAMPNO; 
		}else if (num==2){
			num=SHIFTLAMPNO; 
		}
 	}

	if (lp = Lamp_Look(num))
	{
		Lamp_Do(ltype, lp, num, code);
	}
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

	if (ltype==LTYPE_INUSE || num > CallApps)
	{
		switch (code)
		{
		case LAMP_ON:
			if ((lp->iuse_status&LFLAG_ON)==0)
				changed = 1;
			lp->iuse_status = LFLAG_ON;
			break;
		case LAMP_OFF:
			if (lp->iuse_status&LFLAG_ON)
				changed = 1;
			lp->iuse_status = 0;
			break;
		}
	} else if (ltype==LTYPE_LOCAL){
		/* local class, eg, select, mute, speaker */
		switch (code)
		{
		case LAMP_ON:
			if ((lp->iuse_status&LFLAG_ON)==0)
				changed = 1;
			lp->iuse_status = LFLAG_ON;
			break;
		case LAMP_OFF:
			if (lp->iuse_status&LFLAG_ON)
				changed = 1;
			lp->iuse_status = 0;
			break;
		}
	}else {
		switch (code)
		{
		case LAMP_ON:
			if ((lp->stat_status&LFLAG_ON)==0)
				changed = 1;
			lp->stat_status = LFLAG_ON;
			if (lp->timer)
				TimerRemove(lp->timer);
			lp->timer = (TimerItem *)0;
			break;
		case LAMP_OFF:
			if (lp->stat_status&LFLAG_ON)
				changed = 1;
			if (lp->timer)
				TimerRemove(lp->timer);
			lp->timer = (TimerItem *)0;
			lp->time_cnt = 0;
			lp->stat_status = 0;
			break;
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
		}
	}

#ifdef  USE730WINDOWS
	/* When tiled, don't draw on the display */
	if( !ISTILED( P ) )
#endif /* USE730WINDOWS */
	if (changed)
		DispBut(lp->but, &BUT_FONT);

	if (num==MSGLAMPNO)
	{
		if (code==LAMP_ON && changed && SendAUDIXMsg)
			SendHost(PROC_PHONE, AUDIXMSG, (char *)0);
	}
}

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
