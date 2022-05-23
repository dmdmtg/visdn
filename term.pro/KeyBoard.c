/*
 * KeyBoard module definition
 *
 * Alan Hastings July 1990
 */

#include "visdn.h"

Point fPt();

void KBQueryRcv();

#define KB_BUFSIZ	256
/* first item will be current by default */
struct KBdata {
	int startrow, startcol;
	int endrow, endcol;
	void (*func)();
	int bufsiz;
} KBinit[] =
{
	0, 1,
	0, -1,
	KBQueryRcv,
	KB_BUFSIZ,

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
	extern	char	az_to_09();

	while ((c = kbdchar()) != -1)
	{
		if (c=='\r' || c=='\n')
		{
			char	string[KB_BUFSIZ], *s;

			if (kp->bufptr==kp->buf)
			{
				KBclear(kp);
				continue;
			}

			s = kp->buf;	/* string to send to phone */
			if (DispMode==M_QUERY)
			{
				for (cp=kp->buf; cp < kp->bufptr; cp++)
				{
					*cp = az_to_09(*cp);
					if (  ! (*cp>='0' && *cp<='9')
						 && *cp != '#' && *cp != '*') {
							/* unknown char */
							*cp = '0';
					}
				}
				*kp->bufptr++ = '#';
			} else {
				/* We're in NORMAL mode, so we're sending a
				 * string to the phone.
				 * If the string starts with "CA", assume it's
				 * a phone number to dial, and convert any
				 * a-z charactes to keypad digits.
				 * Don't convert the "CA", though,
				 * and don't convert the keyboard buffer.
				 * Leave it alone (it can be confusing if you
				 * enter "1800callatt" and see "18002255288"
				 * appear on the screen).
				*/
/* XXX FIX: if there's no "CA", but we've got dialtone,
 *	we should probably convert also.
*/
				if (kp->buf[0] == 'C' && kp->buf[1] == 'A') {
					char	*n;
					s = string;	/* send this instead */
					(void) strcpy(string, "CA");
					for (cp=&kp->buf[2], n=&string[2]; cp < kp->bufptr; cp++, n++)
						*n = az_to_09(*cp);
					*n = '\0';
				}
			}
			*kp->bufptr = '\0';
			KBdisplay(kp, kp->buf, F_STORE);
			kp->bufptr = kp->buf;
			OutEnQueue(&PhoneOutQueue, s);
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
