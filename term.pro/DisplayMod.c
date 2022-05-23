/* The X and DMD versions of this file are the same since the guts of the
 * phone logic are in here, and it's easier to maintain one file.
 * As such, you'll see lots of "#ifdef DMD630" code.
 *
 * Eric Claeys, 1/95
*/
/*
 * DisplayMod implementation
 *
 * Alan Hastings June 1991
 */

#ifdef	DMD630
#include "visdn.h"
#ifdef	USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */
#else
#include "xisdn.h"
#endif	/* DMD630 */

#ifdef	DMD630
#define	PhoneType	Type	/* to be same as X version */

typedef struct DField {
	int start_row;
	int start_col;
	int stop_row;
	int stop_col;
} DField;

DField DField7506[] =
{
	1, 2, 1, 4,	/* call appearance */
	1, 5, 1, -5,	/* called dn */
	1, 5, 1, -5,	/* calling dn */
	2, 5, 2, -5,	/* called name */
	2, 5, 2, -5,	/* calling name */
	2, 1, 2, 4,	/* class of service -- guess */
	1, -4, 1, -1,	/* incoming id */
	2, 1, 2, -1,	/* misc call info -- guess */
	1, 1, 2, -1,	/* entire display */
	1, 2, 1, -1,	/* date and time */
	1, 5, 1, -5,	/* redirecting party dn */
	2, 5, 2, -5,	/* redirecting party name */
	1, 5, 1, -5,	/* redirected party dn */
	2, 5, 2, -5,	/* redirected party name */
	1, 5, 1, -5,	/* connected party dn */
	0, 0, 0, 0,
};

DField DField7507[] =
{
	1, 2, 1, 4,	/* call appearance */
	1, 5, 1, -5,	/* called dn */
	1, 5, 1, -5,	/* calling dn */
	2, 5, 2, -5,	/* called name */
	2, 5, 2, -5,	/* calling name */
	2, 1, 2, 4,	/* class of service -- guess */
	1, -4, 1, -1,	/* incoming id */
	2, 1, 2, -1,	/* misc call info -- guess */
	1, 1, 2, -1,	/* entire display */
	1, 2, 1, -1,	/* date and time */
	1, 5, 1, -5,	/* redirecting party dn */
	2, 5, 2, -5,	/* redirecting party name */
	1, 5, 1, -5,	/* redirected party dn */
	2, 5, 2, -5,	/* redirected party name */
	1, 5, 1, -5,	/* connected party dn */
	0, 0, 0, 0,
};
#else
extern	int	PhoneType;

struct LCD_dispinfo {
	int line, start, count;
};
struct LCD_dispinfo LCD_dispinfo7506[] =
{
        1, 0, 2,     /* call appearance */
        1, 3, 17,    /* called dn */
        1, 3, 17,    /* calling dn */
        2, 3, 17,    /* called name */
        2, 3, 17,    /* calling name */
        2, 0, 3,     /* class of service -- guess */
        1, 19, 4,    /* incoming id */
        2, 0, 24,     /* misc call info -- guess */
        1, 0, 24,     /* entire display */
        1, 1, 23,     /* date and time */
        1, 3, 17,     /* redirecting party dn */
        2, 3, 17,     /* redirecting party name */
        1, 3, 17,     /* redirected party dn */
        2, 3, 17,     /* redirected party name */
        1, 3, 17,     /* connected party dn */
        0, 0, 0,
};
struct LCD_dispinfo LCD_dispinfo7507[] =
{
        1, 0, 2,     /* call appearance */
/* The called/calling data normally overlaps, which means if it's both there,
 * you'll only see the last one, not both.
 * So, I moved the called data over so you can see both.
 * This, however, means that when you call someone, their number (and name, if
 * available) won't be next to the callapp.
 * I need to see how a real 7507 does it.
*/
        1, 22,17,    /* called dn */
        1, 3, 17,    /* calling dn */
        2, 22,17,    /* called name */
        2, 3, 17,    /* calling name */
        2, 0, 3,     /* class of service -- guess */
        1, 43, 4,    /* incoming id */
        2, 0, 24,     /* misc call info -- guess */
        1, 0, 48,     /* entire display */
        1, 1, 47,     /* date and time */
        1, 3, 17,     /* redirecting party dn */
        2, 3, 17,     /* redirecting party name */
        1, 22,17,     /* redirected party dn */
        2, 22,17,     /* redirected party name */
        1, 3, 17,     /* connected party dn */
        0, 0, 0,
};
struct LCD_dispinfo *LCD_dispinfo;

void	LCD_Wipe();
void	LCD_Show();
void	LCD_splitstring();
void	LCD_string();
#endif	/* DMD630 */

extern void	LCD_Display();
extern void	LCD_Clear();
extern int	LCD_Setmode();
extern void	LCD_Setmsg();
extern void	LCD_Zap();
#ifdef	DMD630
extern void	LCD_Relocate();
#endif	/* DMD630 */

#ifndef	DMD630
int DispMode = M_NORMAL;
#endif	/* DMD630 */

#ifdef	DMD630
void LCD_Init(origin)
Point origin;
#else
void LCD_Init(ofp)
FILE *ofp;
#endif	/* DMD630 */
{
	LCD.Display = LCD_Display;
	LCD.Clear = LCD_Clear;
	LCD.Setmode = LCD_Setmode;
	LCD.Setmsg = LCD_Setmsg;
	LCD.Zap = LCD_Zap;
#ifdef	DMD630
	LCD.Relocate = LCD_Relocate;
#else
	LCD.ofp = ofp;
#endif	/* DMD630 */

	LCD.mode = 0;

	switch (PhoneType) {
	case ATT7507:
#ifdef	DMD630
		LCD.rect = fRect(origin.x+LCD7507_XOFF,
			origin.y+LCD_YOFF,
			origin.x+LCD7507_XOFF+(LCD7507_COLS*FONTWIDTH(LCD_FONT))+(2*LCD_GAP),
			origin.y+LCD_YOFF+(LCD_ROWS*(FONTHEIGHT(LCD_FONT)+LCD_GAP))+LCD_GAP+2);
		LCD_InitFields(DField7507, LCD7507_COLS);
#else
		LCD.maxline = 48;
		LCD_dispinfo = LCD_dispinfo7507;
#endif	/* DMD630 */
		break;
	case ATT7506:	/* use default */
	default:
#ifdef	DMD630
		LCD.rect = fRect(origin.x+LCD7506_XOFF,
			origin.y+LCD_YOFF,
			origin.x+LCD7506_XOFF+(LCD7506_COLS*FONTWIDTH(LCD_FONT))+(2*LCD_GAP),
			origin.y+LCD_YOFF+(LCD_ROWS*(FONTHEIGHT(LCD_FONT)+LCD_GAP))+LCD_GAP+2);
		LCD_InitFields(DField7506, LCD7506_COLS);
#else
		LCD.maxline = 24;
		LCD_dispinfo = LCD_dispinfo7506;
#endif	/* DMD630 */
		break;
	}
#ifndef	DMD630
	LCD.Zap(-1);
	LCD_Wipe();
#endif	/* DMD630 */
}

#ifdef	DMD630
LCD_InitFields(dp, cols)
register DField *dp;
int cols;
{
	register int i;
	int drowheight = FONTHEIGHT(LCD_FONT);
	int dcolwidth = FONTWIDTH(LCD_FONT);

	for (i=0; dp->start_row; i++, dp++)
	{
		LCD.fieldrects[i].origin = add(LCD.rect.origin, Pt(LCD_GAP, LCD_GAP));
		/* <= 0 means relative to end of LCD display */
		if (dp->start_col <= 0)
		    LCD.fieldrects[i].origin = add(LCD.fieldrects[i].origin,
			Pt((cols+dp->start_col)*dcolwidth,
				((dp->start_row-1)*(drowheight+LCD_GAP))));
		else
			LCD.fieldrects[i].origin = add(LCD.fieldrects[i].origin,
				Pt((dp->start_col-1)*dcolwidth,
					((dp->start_row-1)*(drowheight+LCD_GAP))));
		LCD.fieldrects[i].corner = add(LCD.rect.origin, Pt(LCD_GAP, LCD_GAP));
		if (dp->stop_col <= 0)
			LCD.fieldrects[i].corner = add(LCD.fieldrects[i].corner,
				Pt((cols+dp->stop_col+1)*dcolwidth,
					(dp->stop_row*(drowheight+LCD_GAP))));
		else
			LCD.fieldrects[i].corner = add(LCD.fieldrects[i].corner,
				Pt(dp->stop_col*dcolwidth,
					(dp->stop_row*(drowheight+LCD_GAP))));
	}
}

#else	/* DMD630 */
void LCD_Wipe()
{
	register int i;

	for (i=0; i<LCD.maxline; i++)
	{
		LCD.disp[0][i] = ' ';
		LCD.disp[1][i] = ' ';
	}
}
#endif	/* DMD630 */

void LCD_Clear()
{
#ifdef	DMD630
	(void) rectf(&display, LCD.rect, F_CLR);
	(void) box(&display, LCD.rect, F_STORE);
#else
	LCD_Wipe();
	LCD_Show(1);
	LCD_Show(2);
#endif	/* DMD630 */
}

#ifndef	DMD630
void LCD_Show(line)
int line;
{
	char tbuf[BUFSIZ];
	register char *cp, *tp;
	char *first_space = NULL;

	/* backslash escape spaces in display message */
	for (cp=LCD.disp[line-1], tp=tbuf; *cp && cp < &LCD.disp[line-1][LCD.maxline]; )
	{
		if (*cp==' ')
		{
			if (first_space==NULL) first_space = tp;
			*tp++ = '\\';
		} else
			first_space = NULL;
		*tp++ = *cp++;
	}
	*tp = '\0';
	if (first_space != NULL)
		*first_space = '\0';
	fprintf(LCD.ofp, "xtent.XtDisplay: (%d, %s)\n", line, tbuf);
}
#endif	/* DMD630 */

/* Clear out the saved call appearance information */
void Zap_ca(ca, null_it)
register struct LineApp *ca;
int	null_it;
{
	/* Only NULL this out when told to, because sometimes when
	 * a new DISPLAY message is sent, it doesn't have the inid
	 * even though the old one did (and we want to use the old one.
	*/
	if (null_it != 0)
		ca->inid[0] = '\0';

	ca->callapp[0] = '\0';
	ca->calleddn[0] = '\0';
	ca->callednm[0] = '\0';
	ca->callingdn[0] = '\0';
	ca->callingnm[0] = '\0';
}

void LCD_Zap(mode)
int mode;
{
	register int i;
	register int m = (mode==M_NORMAL) ? 0 : 1;

	for (i=0; i<LCD_MAXFIELDS; i++)
#ifdef NO_LEAVE_DATETIME
		if (i != LCD_MSG_DATETIME)	/* save latest date/time */
#endif
			LCD.msgs[m][i][0] = '\0';
	if (m==LCD.mode)
		LCD.Clear();
}

int LCD_Setmode(m)
short m;
{
	int oldmode;

	/* m==0 is nochange */
	if (m==0)
		return LCD.mode;

	DispMode = m & M_MASK;
	oldmode = LCD.mode;
	LCD.mode = (DispMode==M_NORMAL) ? 0 : 1;

	if (oldmode != LCD.mode)
	{
		LCD.Clear();
		LCD.Display();
	}

#ifdef	DMD630
	/* keypad shading? */
	if (DispMode==M_NORMAL)
	{
		/* always zap the old Query Mode or Inspect Mode display */
		LCD.Zap(M_QUERY);

		/* unshade to indicate keypad connected */
		if (KPflags&DO_SHADE)
		{
		 	KPflags &= ~DO_SHADE;
			DispKeyPad();
		}
	} else
	{
		/* shade to indicate keypad disconnected */
		if (!(KPflags&DO_SHADE))
		{
		 	KPflags |= DO_SHADE;
			DispKeyPad();
		}
	}
#endif	/* DMD630 */

	return oldmode;
}


void LCD_Setmsg(f, mode, s, ca)
int f, mode;
char *s;
struct LineApp *ca;
{
	register int m;
	char	*n;
	extern	char	*lookup_number();
#ifndef	__STDC__
	extern char *strncpy();
#endif

	if (f < 0 || f >= LCD_MAXFIELDS)
		return;
	if ((mode & M_MASK)==M_NORMAL)
		m = 0;
	else
		m = 1;
	(void) strncpy(LCD.msgs[m][f], s, LCD_MAXMSGSIZE);
	switch(f)
	{
	case LCD_MSG_CALLAPP:
		/* The CallApp is the first LCD field, so if we're in the right
		 * mode, clear the LCD since a new message is coming in.
		*/
		if (m==LCD.mode) {
			/* Zap clears the in-core LCD and the LCD on the window */
			LCD.Zap(mode&M_MASK);
		}

		/* kludge #2: line appearance messages get munged into x= form.
		 * Since we may have just cleared LCD.msgs[m][LCD_MSG_CALLAPP]
		 * above, use "s[1]" since "s" is the callapp.
		*/
		LCD.msgs[m][LCD_MSG_CALLAPP][0] = ' ';
		LCD.msgs[m][LCD_MSG_CALLAPP][1] = s[1]-'1'+'a';
		LCD.msgs[m][LCD_MSG_CALLAPP][2] = '=';
		LCD.msgs[m][LCD_MSG_CALLAPP][3] = '\0';

		LCD.msgs[m][LCD_MSG_FULLDISP][0] = '\0';	/* kludge #3 */

		/* Clear out this call appearance's info, and save the ca. */
		if (ca != (struct LineApp *) NULL) {
			Zap_ca(ca, 0);	/* 0 == don't null the inid */
			(void) strncpy(ca->callapp, LCD.msgs[m][LCD_MSG_CALLAPP], LCD_CALLAPPSIZE);
		}
		break;
	case LCD_MSG_CALLEDDN:		/* called # (the one you called) */
		if (ca != (struct LineApp *)NULL)
			(void) strncpy(ca->calleddn, s, LCD_MAXMSGSIZE);
		break;
	case LCD_MSG_CALLINGDN:		/* calling # (the one calling you) */
		if (ca != (struct LineApp *)NULL)
			(void) strncpy(ca->callingdn, s, LCD_MAXMSGSIZE);

#ifdef	DMD630
		/* Let the info process know there is a new incoming call.
		 * It looks up the number to get more information.
		*/
		InfoLineApp = ca;	/* may be NULL */
		InfoEvent |= EVENT_NEWCALL;
#endif
		break;
	case LCD_MSG_CALLEDNM:		/* called name (the one you called) */
		if (ca != (struct LineApp *)NULL)
			(void) strncpy(ca->callednm, s, LCD_MAXMSGSIZE);
		break;
	case LCD_MSG_CALLINGNM:		/* calling name (the one calling you) */
		if (ca != (struct LineApp *)NULL)
			(void) strncpy(ca->callingnm, s, LCD_MAXMSGSIZE);
		break;
	case LCD_MSG_INID:		/* type of call (ICI) */
		/* The Inid always comes after the incoming and/or outgoing
		 * numbers and names, so we can safely use them here.
		*/

		if (ca == (struct LineApp *)NULL) {
			/* We can't do anything if we don't have a call app. */
			break;
		}

		(void) strncpy(ca->inid, s, LCD_INIDSIZE);

		/* If there IS a number but there is NOT a name,
		 * look up the number.
		 * XXXXX Should we look up the number even if there IS a name?
		*/
		if (ca->calleddn[0] != '\0' && ca->callednm[0] == '\0'){
			n = lookup_number(ca->calleddn);
			if (n != (char *)NULL) {
				/* Found a name so use it */
				(void) strncpy(ca->callednm, n, LCD_MAXMSGSIZE);
				/* Since there's nothing on the LCD for this
				 * name, set it.
				*/
				(void) strncpy(LCD.msgs[m][LCD_MSG_CALLEDNM],
					n, LCD_MAXMSGSIZE);
			}
		}
		/* The calling number may be "INCOMING CALL".
		 * If so, don't bother looking it up since it won't
		 * be found.  If there IS a number,
		 * look it up even if there is a name,
		 * since the invoker may have a different name they'd
		 * like displayed.  If there is already a name and a
		 * new one is NOT found in the database, use the one
		 * we got from the phone.
		*/
		if (ca->callingdn[0] != '\0' &&
		    strcmp(ca->callingdn, "INCOMING CALL") != 0) {
			n = lookup_number(ca->callingdn);
			if (n != (char *)NULL) {
				(void) strncpy(ca->callingnm, n, LCD_MAXMSGSIZE);
				(void) strncpy(LCD.msgs[m][LCD_MSG_CALLINGNM],
					n, LCD_MAXMSGSIZE);
			}
		}
		break;

	default:
		/* don't need to do anything */
		break;
	}
	if (m==LCD.mode)
	{
		LCD.Display();
	}
}

#ifdef	DMD630
void LCD_Relocate(relorigin)
Point relorigin;
{
	register int i;

	LCD.rect = raddp(LCD.rect, relorigin);

	for (i=0; i < LCD_MAXFIELDS; i++)
		LCD.fieldrects[i] = raddp(LCD.fieldrects[i], relorigin);
}
#endif	/* DMD630 */

/*
 * Display messages for current line appearance
 */
void LCD_Display()
{
	register int f;
	register char *s;	/* string to be displayed */

#ifdef	USE730WINDOWS
	if( ISTILED( P ) )
		return;
#endif /* USE730WINDOWS */

	for (f=0; f<LCD_MAXFIELDS; f++)
	{
	    s = LCD.msgs[LCD.mode][f];
	    if (s[0]) {
		switch (f)
		{
		case LCD_MSG_DATETIME:
			/* Ignore this since there's no room on the display
			 * for it (it would overwrite the number, and
			 * besides, it's not displayed on the 7506 phone anyhow.
			*/
			break;
		case LCD_MSG_FULLDISP:
#ifdef	DMD630
			LCD_rectstring(LCD.fieldrects[f], s);
#else
			LCD_splitstring(s);
#endif	/* DMD630 */
			break;
		default:
#ifdef	DMD630
			(void) rectf(&display, LCD.fieldrects[f], F_CLR);
			(void) string(&LCD_FONT,
					s,
					&display,
					LCD.fieldrects[f].origin,
					F_XOR);
#else
			LCD_string(s,
				LCD_dispinfo[f].line,
				LCD_dispinfo[f].start,
				LCD_dispinfo[f].count);
#endif	/* DMD630 */
			break;
		}
	    }
	}
}

#ifdef	DMD630
/*
 * display a string in a rectangle, possibly splitting it to fit
 */
LCD_rectstring(r, s)
Rectangle r;
register char *s;
{
	register int rlen, i, c;
	register char *cp;

	(void) rectf(&display, r, F_CLR);

	/* calculate number of chars that fit */
	rlen = (r.corner.x - r.origin.x)/FONTWIDTH(LCD_FONT);
	/* find end of (sub-)string that fits */
	for (i=0, cp=s; *cp && i < rlen; i++, cp++)
		;
	if (*cp)
	{
		/* string too long -- split it? */
		/* look for last white space */
		while (cp > s && *cp != ' ')
			cp--;
		c = *cp;	/* remember that character */
		*cp = '\0';
		(void) string(&LCD_FONT, s, &display, r.origin, F_XOR);
		if ((*cp = c)==' ')	/* just skip it if it was a space */
			cp++;
		/* don't bother to split the second half */
		(void) string(&LCD_FONT, cp, &display,
			add(r.origin, Pt(0, FONTHEIGHT(LCD_FONT)+2)), F_XOR);
	} else
	{
		/* string fits on one line */
		(void) string(&LCD_FONT, s, &display, r.origin, F_XOR);
	}
}
#else
void LCD_string(str, line, start, count)
char *str;
int line, start, count;
{
	register char *cp;

	for (cp = &LCD.disp[line-1][start]; cp <= &LCD.disp[line-1][start+count]; cp++)
	{
		if (*str=='\0')
			*cp = ' ';
		else
			*cp = *str++;
	}

	LCD_Show(line);
}

/*
 * display a string, splitting it across lines
 */
void LCD_splitstring(s)
register char *s;
{
	register int i, c;
	register char *cp, *dp;

	LCD_Wipe();

	/* find end of (sub-)string that fits */
	for (i=0, cp=s, dp=LCD.disp[0]; *cp && i < LCD.maxline; i++, cp++, dp++)
	{
		*dp = *cp;
	}
	if (*cp)
	{
		/* string too long -- split it? */
		/* look for last white space */
		while (cp > s && *cp != ' ')
		{
			*--dp = ' ';
			cp--;
		}
		c = *cp;	/* remember that character */
		*cp = '\0';
		LCD_Show(1);
		if ((*cp = (char)c)==' ')	/* just skip it if it was a space */
			cp++;
		for (i=0, dp=LCD.disp[1]; i < LCD.maxline; i++)
		{
			if (*cp)
				*dp++ = *cp++;
			else
				*dp++ = ' ';
		}
	}
	LCD_Show(1);
	LCD_Show(2);
}
#endif	/* DMD630 */
