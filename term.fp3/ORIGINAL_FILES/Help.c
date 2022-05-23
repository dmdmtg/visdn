#include "visdn3.h"
#ifdef  USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */

HelpProc()
{
	register TermProc *tp;
	register int state;

	tp = &TermProcs[PROC_HELP];
#ifdef  USE730WINDOWS
        labelon();
        set730wins( P, 1, 0 );
#endif /* USE730WINDOWS */

	HelpCurTopic.text = (char *)0;
	HelpCurTopic.tag = (char *)0;
	HelpReLocate();
	HelpDisplay();

	if (HelpMenu.first==(HelpItem *)0)
		SendHost(PROC_HELP, HELPTOC, (char *)0);

	request(CPU | MOUSE | RESHAPED | DELETE);

	while (1)
	{
		state = wait(CPU);

#ifdef	USE730WINDOWS
		if( own() & MOUSE )
		{
			if( chk730wins() )
				continue;
		}
#endif /* USE730WINDOWS */
		if (P->state & (RESHAPED|MOVED))
		{
			HelpReLocate();
			if (!(P->state & MOVED))
				HelpDisplay();
			P->state &= ~(RESHAPED|MOVED);
		}

		/* main process keeps resetting this to 0 */
		HelpThere++;

		if ((own() & DELETE) || (GlobalEvent&GLOBAL_QUIT))
		{
			extern Proc *helpproc;
			tp->event = 0;
			local();
			helpproc = 0;
			delete();
			exit();
		}

		if (tp->event)
		{
			if (tp->event&EVENT_HELPLINE)
			{
				tp->event &= ~EVENT_HELPLINE;
				/* just in case the line needs to be displayed */
				HelpShow();
			}
			if (tp->event&EVENT_QUIT)
			{
				tp->event &= ~EVENT_QUIT;
				local();
				delete();
				exit();
			}
			if (tp->event&EVENT_MAKECURRENT)
			{
				top();
				current();
				tp->event &= ~EVENT_MAKECURRENT;
			}
		}

		if ((own() & MOUSE) != 0)
			HelpMouse();
	}
}

HelpMouse()
{
	register HelpItem *it;

	if (button1())
	{
		if (ptinrect(mouse.xy, HelpDoneRect))
		{
			(void) rectf(P->uJdisplayp, HelpDoneRect, F_XOR);
			(void) bttns(0);
			(void) rectf(P->uJdisplayp, HelpDoneRect, F_XOR);
			HelpEvent |= EVENT_QUIT;
		}
	}

	if (button2())
	{
		if (it = (HelpItem *)tmenuhit(&HelpMenu, 2, TM_EXPAND))
		{
			HelpTopic(it);
			HelpShow();
		}
	}
}

HelpShow()
{
	register int i;
	register HelpLine *lp;
	Point p;

	if (HelpCurTopic.text==(char *)0)
		return;
	(void) string(&HELP_FONT, HelpCurTopic.text, P->uJdisplayp,
			add(HelpTextRect.origin, Pt(10, 2)), F_STORE);

	for (lp=HelpLines; lp; lp=lp->next)
		if (lp->tag==HelpCurTopic.tag)
			break;
	if (lp==(HelpLine *)0)
	{
		if (HelpCurTopic.tag && *HelpCurTopic.tag)
			SendHost(PROC_HELP, HELPTOPIC, HelpCurTopic.tag);
		return;
	}

	for (i=1; lp && lp->tag==HelpCurTopic.tag && i < HelpTextLines; i++, lp=lp->next)
	{
		(void) string(&HELP_FONT, lp->text, P->uJdisplayp,
			add(HelpTextRect.origin,
				Pt(10, i*FONTHEIGHT(HELP_FONT)+10)),
			F_STORE);
	}
}

HelpTopic(it)
register HelpItem *it;
{
	if (it->tag != HelpCurTopic.tag)
	{
		(void) rectf(P->uJdisplayp, HelpTextRect, F_CLR);
		HelpCurTopic = *it;
	}
}

HelpReLocate()
{
	Rectangle r;

	r = P->uDrect;

	HelpOrigin = P->uJdisplayp->rect.origin;

	HelpDoneRect = fRect(	r.corner.x-6*FONTWIDTH(HELP_FONT)-10,
				r.origin.y+10,
				r.corner.x-10,
				r.origin.y+FB_OFF+FB_WID-3);

	HelpTextRect = fRpt(add(r.origin, Pt(2, FB_OFF+FB_WID)),
				sub(r.corner, Pt(1, 1)));

	HelpTextLines = ((HelpTextRect.corner.y-HelpTextRect.origin.y)
                                / FONTHEIGHT(HELP_FONT)) - 2;

	HPRINTF("orig=%d,%d rect=%d,%d/%d,%d lines=%d\n",
			HelpOrigin.x, HelpOrigin.y,
			HelpTextRect.origin.x, HelpTextRect.origin.y,
			HelpTextRect.corner.x, HelpTextRect.corner.y,
			HelpTextLines);
}

HelpDisplay()
{
	register Bitmap *bp;

	bp = P->uJdisplayp;

	(void) rectf(bp, HelpTextRect, F_CLR);
	(void) box(bp, P->uDrect, F_STORE);

	(void) string(&HELP_FONT, "VISDN Help Window",
			bp, add(P->uDrect.origin, Pt(10, 15)), F_STORE);

	(void) box(bp, HelpDoneRect, F_STORE);
	(void) string(&HELP_FONT, "Done", bp,
			add(HelpDoneRect.origin, Pt(FONTWIDTH(HELP_FONT), 
				(HelpDoneRect.corner.y-HelpDoneRect.origin.y-FONTHEIGHT(HELP_FONT))/2)),
			F_STORE);

	if (HelpCurTopic.text)
		HelpShow();
}
