#include "visdn3.h"
#ifdef USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */

/* copy y into z, remembering starting place in x */
#define SNARFSTRING(x,y,z)	for (x=z; *y != ';'; ) \
					if ((*z++ = *y++)=='\0') \
						return; \
				*z++ = '\0'; \
				y++;

/* stuff for discovering the background color on a 630 */
struct bram_mem {unsigned char not_there, byte;};
struct Pref_opts {struct bram_mem screencolor; /*...*/};
struct bram {struct bram_mem csum[2]; struct Pref_opts pref_opts; /*...*/};
#define BRAM ((struct bram *)0xe00000)
#define VALSCREENCOLOR BRAM->pref_opts.screencolor.byte

InfoProc()
{
	register int state;
	register TermProc *tp;

	tp = &TermProcs[PROC_INFO];

	tp->event = 0;
	*InfoBuf = *tp->rcvbuf = *tp->sndbuf = '\0';
	InfoData.key = "";
	InfoData.face = (Word *)0;
#ifdef	USE730WINDOWS
/*ECC*/	if( has730wins() )
	{
		labelon();
		set730wins( P, 1, 0 );
	}
#endif /* USE730WINDOWS */

	InfoReLocate();
	InfoDisplay();

	request(CPU | MOUSE | RESHAPED | DELETE);

	while (1)
	{
		state = wait(CPU);/* | MOUSE | RESHAPED | DELETE );*/
#ifdef	USE730WINDOWS
		if( own() & MOUSE )
		{
			if( chk730wins() )
				continue;
		}
#endif /* USE730WINDOWS */

		if (P->state & (RESHAPED|MOVED))
		{
			InfoReLocate();
			if (!(P->state & MOVED))
				InfoDisplay();
			P->state &= ~(RESHAPED|MOVED);
		}

		tp->alive++;

		if( (own() & DELETE) || (GlobalEvent&GLOBAL_QUIT))
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
			if (tp->event&EVENT_QUIT)
			{
				tp->event = 0;
				local();
				delete();
				exit();
			}

			if (tp->event & EVENT_MAKECURRENT)
			{
				top();
				current();
				tp->event &= ~EVENT_MAKECURRENT;
			}

			if (tp->event & EVENT_RCVBUF)
			{
				InfoGetMsg(tp->rcvbuf);
				tp->event &= ~EVENT_RCVBUF;
			}

			if (tp->event & EVENT_NEWCALL)
			{
				if (InfoData.key==(char *)0
				    || strcmp(LCD.Msgstring(M_NORMAL, LCD_MSG_CALLINGDN), InfoData.key))
				{
					IPRINTF("NEWCALL '%s'\n", LCD.Msgstring(M_NORMAL, LCD_MSG_CALLINGDN));
					InfoClear();
					SendHost(PROC_INFO, INFOREQ,
						LCD.Msgstring(M_NORMAL, LCD_MSG_CALLINGDN));
				} else
				{
					IPRINTF("OLDCALL '%s'\n", LCD.Msgstring(M_NORMAL, LCD_MSG_CALLINGDN));
				}
				InfoDispTime(LCD.Msgstring(M_NORMAL, LCD_MSG_DATETIME));
				tp->event &= ~EVENT_NEWCALL;
			}
		}

		if (own() & MOUSE)
			InfoMouse();
	}
}

InfoGetMsg(buf)
register char *buf;
{
	UserInfo udata;
	register char *cp, *fp;
	char reqbuf[128];

	switch (*buf++)
	{
	case USERINFO:
		IPRINTF("USERINFO %s\n", buf);
		cp = InfoBuf;
		SNARFSTRING(udata.key, buf, cp);
		SNARFSTRING(udata.name, buf, cp);
		SNARFSTRING(udata.org, buf, cp);
		SNARFSTRING(udata.tl, buf, cp);
		SNARFSTRING(udata.com, buf, cp);
		SNARFSTRING(udata.loc, buf, cp);
		SNARFSTRING(udata.room, buf, cp);
		SNARFSTRING(udata.tel, buf, cp);
		SNARFSTRING(udata.ema, buf, cp);
		top();
		InfoClear();
		InfoData = udata;	/* structure copy */
		InfoData.face = (Word *)0;
		if (*udata.ema)
		{
			sprintf(reqbuf, "%s;%s;", udata.ema, udata.key);
			SendHost(PROC_INFO, FACEREQ, reqbuf);
		}
		InfoShow();
		InfoDispTime(LCD.Msgstring(M_NORMAL, LCD_MSG_DATETIME));
		break;

	case FACEDATA:
		IPRINTF("FACEDATA received\n");
		if (InfoData.key==(char *)0 || *InfoData.key=='\0')
		{
			IPRINTF("No active display\n");
			return;
		}
		if (strncmp(InfoData.key, buf, strlen(InfoData.key)))
		{
			IPRINTF("face/key mismatch\n");
			return;		/* must be someone else's face */
		}
		if (VALSCREENCOLOR)
		{
			IPRINTF("white background\n");
			for (cp = &buf[ICONKEYSIZE], fp=(char *)InfoFace;
						cp < &buf[ICONBYTES+ICONKEYSIZE]; )
				*fp++ = *cp++;
		} else
		{
			IPRINTF("black background\n");
			for (cp = &buf[ICONKEYSIZE], fp=(char *)InfoFace;
						cp < &buf[ICONBYTES+ICONKEYSIZE]; )
				*fp++ = ~(*cp++);
		}
		InfoData.face = InfoFace;
		InfoShow();
		break;
	}
}

InfoReLocate()
{
	Rectangle r;

	r = P->uDrect;

        InfoRect = fRpt(add(r.origin, Pt(2, FB_OFF+FB_WID)),
                                sub(r.corner, Pt(1, 1)));

	InfoDoneRect = fRect(	r.corner.x-6*FONTWIDTH(mediumfont)-10,
				r.origin.y+10,
				r.corner.x-10,
				r.origin.y+FB_OFF+FB_WID-3);

	InfoTimeRect = fRect(	InfoDoneRect.origin.x-15*FONTWIDTH(mediumfont),
				r.origin.y+15,
				InfoDoneRect.origin.x-10,
				r.origin.y+15+FONTHEIGHT(mediumfont)+4);
}

InfoDisplay()
{
	register Bitmap *bp;

	bp = P->uJdisplayp;

	(void) rectf(bp, InfoRect, F_CLR);
	(void) box(bp, P->uDrect, F_STORE);

	(void) string(&mediumfont, "VISDN Call Information Window",
			bp, add(P->uDrect.origin, Pt(10, 15)), F_STORE);

	(void) box(bp, InfoDoneRect, F_STORE);
	(void) string(&mediumfont, "Done", bp,
			add(InfoDoneRect.origin, Pt(FONTWIDTH(mediumfont),
                                (InfoDoneRect.corner.y-InfoDoneRect.origin.y-FONTHEIGHT(mediumfont))/2)),
			F_STORE);

	InfoShow();
}

InfoShow()
{
	register UserInfo *ip;
	char buf[128];
	Bitmap face;
	Point p;
	Rectangle r;

	ip = &InfoData;

	if (ip->face)
	{
		IPRINTF("drop in face\n");
		p = add(InfoRect.origin, Pt(10,10));
		face.base = ip->face;
		face.width = 3;
		face.rect = r = fRect(0, 0, ICONSIZE, ICONSIZE);
		face._null = (char *)0;
		for (	r.origin.y += ICONSIZE-1;
			r.origin.y >= face.rect.origin.y;
			r.origin.y--)
		{
			(void) bitblt(&face, r, P->uJdisplayp, p, F_STORE);
			sleep(2);
		}
	}

	if (*ip->key)
	{
		string(&INFO_FONT, ip->key, P->uJdisplayp,
			add(InfoRect.origin, Pt(70,10+0*FONTHEIGHT(INFO_FONT)+2)),
			F_STORE);
		Sprintf(buf, "%-24.24s %-10.10s %6.6s %s", ip->name, ip->org,
				ip->loc, ip->room);
		string(&INFO_FONT, buf, P->uJdisplayp,
			add(InfoRect.origin, Pt(70,10+1*FONTHEIGHT(INFO_FONT)+2)),
			F_STORE);
/*ECC*/		Sprintf(buf, "%-24.24s  %12.12s %s", "", ip->tel, ip->ema);
		string(&INFO_FONT, buf, P->uJdisplayp,
			add(InfoRect.origin, Pt(70,10+2*FONTHEIGHT(INFO_FONT)+2)),
			F_STORE);
	}
}

InfoDispTime(str)
char *str;
{
	if (InfoTimeRect.origin.x != 0 && *str)
	{
		(void) rectf(P->uJdisplayp, InfoTimeRect, F_CLR);
		(void) string(&mediumfont, str,
			P->uJdisplayp, InfoTimeRect.origin, F_STORE);
	}
}

InfoMouse()
{
        if (button1())
	{
         	if (ptinrect(mouse.xy, InfoDoneRect))
		{
                 	(void) rectf(P->uJdisplayp, InfoDoneRect, F_XOR);
			(void) bttns(0);
			(void) rectf(P->uJdisplayp, InfoDoneRect, F_XOR);
			InfoEvent |= EVENT_QUIT;
		}
	}
}

InfoClear()
{
	InfoData.key = "";
	InfoData.face = (Word *)0;
	(void) rectf(P->uJdisplayp, InfoRect, F_CLR);
}
