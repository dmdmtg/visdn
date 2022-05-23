h02146
s 00152/00000/00000
d D 1.1 92/07/23 13:58:30 trl 1 0
c date and time created 92/07/23 13:58:30 by trl
e
u
U
t
T
I 1
/*
 * KeyBoard module definition
 *
 * Alan Hastings July 1990
 */

#include "visdn.h"

char AlphaMap[] =
{
	'2', '2', '2',		/* a b c */
	'3', '3', '3',		/* d e f */
	'4', '4', '4',		/* g h i */
	'5', '5', '5',		/* j k l */
	'6', '6', '6',		/* m n o */
	'7', '9', '7', '7',	/* p (q) r s */
	'8', '8', '8',		/* t u v */
	'9', '9', '9', '9',	/* w x y z */
};

Point fPt();

void KBQueryRcv();

/* first item will be current by default */
struct KBdata {
	int startrow, startcol;
	int endrow, endcol;
	void (*func)();
	int bufsiz;
} KBinit[] =
{
	0, 1, 0, -1, KBQueryRcv, 256,
	0, 0, 0, 0, 0, 0,
};

KeyBoardInit()
{
	register KBrect *kp;
	register struct KBdata *kip;
	extern int FBColOff[], FBRowOff[];

	for (kip = KBinit; kip->func; kip++)
	{
		if (kp = new(KBrect))
		{
			kp->next = KBrects;
			KBrects = kp;
			/* rect and displaypt are calculated in ReSizeScreen */
			kp->startrow = kip->startrow;
			kp->startcol = kip->startcol;
			kp->endrow = kip->endrow;
			kp->endcol = kip->endcol;
			kp->font = &KB_FONT;
			kp->rcv = kip->func;
			kp->bufsiz = kip->bufsiz;
			kp->buf = alloc(kip->bufsiz);
			kp->bufptr = kp->buf;

			if (KBcurrent==(KBrect *)0)
				KBcurrent = kp;
		}
	}
}

void KBQueryRcv(kp)
KBrect *kp;
{
	register int c;
	register char *cp;

	while ((c = kbdchar()) != -1)
	{
		if (c=='\r' || c=='\n')
		{
			if (kp->bufptr==kp->buf)
			{
				KBclear(kp);
				continue;
			}
			else if (DispMode==M_QUERY)
			{
				for (cp=kp->buf; cp < kp->bufptr; cp++)
				{
					if (*cp>='A' && *cp<='Z')
						*cp += ('a'-'A');
					if (*cp>='a' && *cp<='z')
						*cp = AlphaMap[*cp-'a'];
					else if (   !(*cp>='0' && *cp<='9')
						 && *cp != '#'
						 && *cp != '*')
							*cp = '0';
				}
				*kp->bufptr++ = '#';
			}
			else if( *kp->buf != 'c' && kp->buf[1] != 'a' )
				OutEnQueue(&PhoneOutQueue, "CA" );
			*kp->bufptr = '\0';
			KBdisplay(kp, kp->buf, F_STORE);
			kp->bufptr = kp->buf;
			OutEnQueue(&PhoneOutQueue, kp->buf);
		} else if (c==CKill)
		{
			kp->bufptr = kp->buf;
			*kp->bufptr = '\0';
			KBclear(kp);
		} else if (c==CErase)
		{
			if (kp->bufptr==kp->buf)
				ringbell();
			else
			{
				KBdisplay(kp, kp->buf, F_XOR);
				*--kp->bufptr = ' ';
				KBdisplay(kp, kp->buf, F_XOR);
			}
		} else if (c < ' ')
			ringbell();
		else
		{
			if (kp->bufptr >= &kp->buf[kp->bufsiz-2])
				ringbell();
			else
			{
				if (kp->bufptr==kp->buf)
					KBclear(kp);
				*kp->bufptr++ = c;
				*kp->bufptr = '\0';
				KBdisplay(kp, kp->buf, F_STORE);
			}
		}
	}
}

KBclear(kp)
register KBrect *kp;
{
	(void) rectf(&display, kp->rect, F_CLR);
	(void) box(&display, kp->rect, F_STORE);
	if (kp==KBcurrent)
		BoldBox(kp->rect, F_STORE);
}

KBdisplay(kp, s, type)
KBrect *kp;
char *s;
int type;
{
	string(kp->font, s, &display,
		add(kp->rect.origin, kp->displaypt),
		type);
}
E 1
