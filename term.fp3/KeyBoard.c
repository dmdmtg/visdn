/*
 * KeyBoard module definition
 *
 * Alan Hastings July 1990
 */

#include "visdn3.h"


/* index maps implicity to alpha char, see code below */

char AlphaMap[] =
{
  '2', '2', '2',    /* a b c */
  '3', '3', '3',    /* d e f */
  '4', '4', '4',    /* g h i */
  '5', '5', '5',    /* j k l */
  '6', '6', '6',    /* m n o */
  '7', '9', '7', '7', /* p (q) r s */
  '8', '8', '8',    /* t u v */
  '9', '9', '9', '9', /* w x y z */
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
	register FeatBut *fp;
	register char *cp;

	while ((c = kbdchar()) != -1)
	{
		if (c=='\r' || c=='\n')
		{
			if (kp->bufptr==kp->buf)
			{
				KBclear(kp);
				continue;
			/* visdn for prophone checked for query mode and ended string with # */
			/* here for fp3 we map chars, digits, * and # to keypad presses      */
			/** } else if (DispMode==M_QUERY) **/

			} else if (1)
			{
			/* this maps chars to keypad digits */
				for (cp=kp->buf; cp < kp->bufptr; cp++)
				{
					if (*cp>='A' && *cp<='Z')
						*cp += ('a'-'A'); /* translate to lower case */
					if (*cp>='a' && *cp<='z'){
						/** yanked from mouse press of key pad **/
						for (fp=KeyPad; fp; fp=fp->next){
							if (fp->name[0] == AlphaMap[*cp - 'a']) /* change this to compare with name */
							{
								(void) rectf(&display, fp->rect, F_XOR);
								if (fp->action)
									(*fp->action)(fp);
								(void) rectf(&display, fp->rect, F_XOR);
								break;
							}
						}
					}
					else if ( (*cp>='0' && *cp<='9') || *cp == '#' || *cp == '*'){
						/** yanked from mouse press of key pad **/
						for (fp=KeyPad; fp; fp=fp->next){
							if (fp->name[0] == *cp) 
							{
								(void) rectf(&display, fp->rect, F_XOR);
								if (fp->action)
									(*fp->action)(fp);
								(void) rectf(&display, fp->rect, F_XOR);
								break;
							}
						}
					}
				}
			}
			*kp->bufptr = '\0';
			KBdisplay(kp, kp->buf, F_STORE);
			kp->bufptr = kp->buf;
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
