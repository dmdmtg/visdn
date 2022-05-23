h06704
s 00314/00000/00000
d D 1.1 92/07/23 13:58:28 trl 1 0
c date and time created 92/07/23 13:58:28 by trl
e
u
U
t
T
I 1
/*
 * DisplayMod implementation
 *
 * Alan Hastings July 1990
 */

#include "visdn.h"
#ifdef	USE730WINDOWS
#include <730windows.h>
#endif

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

extern void	LCD_Display();
extern void	LCD_Clear();
extern int	LCD_Setmode();
extern void	LCD_Setmsg();
extern void	LCD_Relocate();
extern void	LCD_Zap();

void LCD_Init(origin)
Point origin;
{
	LCD.Display = LCD_Display;
	LCD.Clear = LCD_Clear;
	LCD.Setmode = LCD_Setmode;
	LCD.Setmsg = LCD_Setmsg;
	LCD.Relocate = LCD_Relocate;
	LCD.Zap = LCD_Zap;

	LCD.mode = 0;
	if (Type==ATT7506)
	{
         	LCD.rect = fRect(origin.x+LCD7506_XOFF,
                        origin.y+LCD_YOFF,
                        origin.x+LCD7506_XOFF+(LCD7506_COLS*FONTWIDTH(LCD_FONT))+(2*LCD_GAP),
       			origin.y+LCD_YOFF+(LCD_ROWS*(FONTHEIGHT(LCD_FONT)+LCD_GAP))+LCD_GAP+2);
         	LCD_InitFields(DField7506, LCD7506_COLS);
	} else if (Type==ATT7507)
	{
		LCD.rect = fRect(origin.x+LCD7507_XOFF,
			origin.y+LCD_YOFF,
			origin.x+LCD7507_XOFF+(LCD7507_COLS*FONTWIDTH(LCD_FONT))+(2*LCD_GAP),
			origin.y+LCD_YOFF+(LCD_ROWS*(FONTHEIGHT(LCD_FONT)+LCD_GAP))+LCD_GAP+2);
		LCD_InitFields(DField7507, LCD7507_COLS);
	} else
	{
		LCD.rect = fRect(origin.x+LCD7506_XOFF,
			origin.y+LCD_YOFF,
			origin.x+LCD7506_XOFF+(LCD7506_COLS*FONTWIDTH(LCD_FONT))+(2*LCD_GAP),
			origin.y+LCD_YOFF+(LCD_ROWS*(FONTHEIGHT(LCD_FONT)+LCD_GAP))+LCD_GAP+2);
		LCD_InitFields(DField7507, LCD7506_COLS);
	}
}

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

void LCD_Clear()
{
	(void) rectf(&display, LCD.rect, F_CLR);
	(void) box(&display, LCD.rect, F_STORE);
}

void LCD_Zap(mode)
int mode;
{
	register int i;
	register int m = (mode==M_NORMAL) ? 0 : 1;

	for (i=0; i<LCD_MAXFIELDS; i++)
		if (i != LCD_MSG_DATETIME)	/* save latest date/time */
			LCD.msgs[m][i][0] = '\0';
	if (m==LCD.mode)
		LCD.Clear();
}

int LCD_Setmode(m)
short m;
{
	int oldmode;

	/* m==0 is query */
	if (m==0)
		return LCD.mode;

	DispMode = (m & M_MASK);
	oldmode = LCD.mode;
	LCD.mode = (DispMode==M_NORMAL) ? 0 : 1;

	if (oldmode != LCD.mode)
	{
		LCD.Clear();
		LCD.Display();
	}

	/* keypad shading? */
	if (DispMode==M_NORMAL)
	{
		/* always zap the old Query Mode or Inspect Mode display */
		LCD.Zap(1);

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

	return oldmode;
}

void LCD_Setmsg(f, mode, s)
int f, mode;
char *s;
{
	register int m;
	extern char *strncpy();

	if (f < 0 || f >= LCD_MAXFIELDS)
		return;
	if ((mode & M_MASK)==M_NORMAL)
		m = 0;
	else
		m = 1;
	(void) strncpy(LCD.msgs[m][f], s, LCD_MAXMSGSIZE);
	if (f==LCD_MSG_CALLAPP)
	{
		/* kludge #2: line appearance messages get munged into x= form */
		LCD.msgs[m][LCD_MSG_CALLAPP][0] = ' ';
		LCD.msgs[m][LCD_MSG_CALLAPP][1] =
			LCD.msgs[m][LCD_MSG_CALLAPP][1]-'1'+'a';
		LCD.msgs[m][LCD_MSG_CALLAPP][2] = '=';
		LCD.msgs[m][LCD_MSG_CALLAPP][3] = '\0';
		LCD.msgs[m][LCD_MSG_FULLDISP][0] = '\0';	/* kludge #3 */
		if (m==LCD.mode)
			LCD.Clear();
	}
	if (f==LCD_MSG_CALLINGDN)
	{
		InfoEvent |= EVENT_NEWCALL;
	}
	if (m==LCD.mode)
	{
		LCD.Display();
	}
}

void LCD_Relocate(relorigin)
Point relorigin;
{
	register int i;

	LCD.rect = raddp(LCD.rect, relorigin);

	for (i=0; i < LCD_MAXFIELDS; i++)
		LCD.fieldrects[i] = raddp(LCD.fieldrects[i], relorigin);
}

/*
 * Display messages for current line appearance
 */
void LCD_Display()
{
	register int f;

#ifdef	USE730WINDOWS
	if( ISTILED( P ) )
		return;
#endif

	for (f=0; f<LCD_MAXFIELDS; f++)
	{
	    if (LCD.msgs[LCD.mode][f][0])
		switch (f)
		{
		case LCD_MSG_DATETIME:
			/* ignore for now */
			break;
		case LCD_MSG_FULLDISP:
			LCD_rectstring(LCD.fieldrects[f], LCD.msgs[LCD.mode][f]);
			break;
		default:
			(void) rectf(&display, LCD.fieldrects[f], F_CLR);
			(void) string(&LCD_FONT,
					LCD.msgs[LCD.mode][f],
					&display,
					LCD.fieldrects[f].origin,
					F_XOR);
			break;
		}
	}
}

/*
 * display a string in a recatngle, possibly splitting it to fit
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
E 1
