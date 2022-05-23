/*
 * DisplayMod implementation
 *
 * Alan Hastings July 1990
 *
 * Doug Rippe 4/7/92 changed mode stuff for fp3
 * Alan has two modes, normal and not normal
 * 
 * The fp3 implementation:
 * Display can be managed by monitoring display infor elements sent over the BRI
 * and processing them or by monitoring what the set is displaying. 
 * The fp3 version monitors what the set is displaying and mimics it.
 * It also preserves all the display messages sent from the BRI interface
 * for all the different modes and types in the msgs array. These are used
 * to send info to the host and could be used for custom display.
 * Display() was changed from the prophone implementation to mimc
 * the phone display.
 */

#include "visdn3.h"
#ifdef  USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */

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
	int i,j;

	LCD.Display = LCD_Display;
	LCD.Clear = LCD_Clear;
	LCD.Setmode = LCD_Setmode;
	LCD.Setmsg = LCD_Setmsg;
	LCD.Relocate = LCD_Relocate;
	LCD.Zap = LCD_Zap;

	LCD.mode = M_NORMAL;

	/* djr--added for fp3 init phone display */
	for(i=0;i<DISPLAY_MAX_ROWS;i++){
		for(j=0;j<DISPLAY_MAX_COLUMNS;j++){
			LCD.phonedisplay[i][j]=' ';
		}
		if (Type==ATT7507){
			LCD.phonedisplay[i][LCD7507_COLS]='\0';
		}else{
			LCD.phonedisplay[i][LCD7506_COLS]='\0';
		}
	}

	/* djr--for fp3 I changed LCD7506COLS to LCD7506COLs+2 as a multiplier rect coords */
	if (Type==ATT7506)
	{
         	LCD.rect = fRect(origin.x+LCD7506_XOFF,
                        origin.y+LCD_YOFF,
                        origin.x+LCD7506_XOFF+((LCD7506_COLS+2)*FONTWIDTH(LCD_FONT))+(2*LCD_GAP), origin.y+LCD_YOFF+(LCD_ROWS*(FONTHEIGHT(LCD_FONT)+LCD_GAP))+LCD_GAP+2);
         	LCD_InitFields(DField7506, LCD7506_COLS);
	} else if (Type==ATT7507)
	{
		LCD.rect = fRect(origin.x+LCD7507_XOFF,
			origin.y+LCD_YOFF,
			origin.x+LCD7507_XOFF+((LCD7507_COLS+2)*FONTWIDTH(LCD_FONT))+(2*LCD_GAP),
			origin.y+LCD_YOFF+(LCD_ROWS*(FONTHEIGHT(LCD_FONT)+LCD_GAP))+LCD_GAP+2);
		LCD_InitFields(DField7507, LCD7507_COLS);
	} else
	{
		LCD.rect = fRect(origin.x+LCD7506_XOFF,
			origin.y+LCD_YOFF,
			origin.x+LCD7506_XOFF+((LCD7506_COLS+2)*FONTWIDTH(LCD_FONT))+(2*LCD_GAP),
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

/* djr--changed for fp3 */
/* did away with line like this... */
	/** LCD.mode = (DispMode==M_NORMAL) ? 0 : 1; **/
void LCD_Zap(mode)
int mode;
{
	register int i;

	for (i=0; i<=LCD_MAXFIELDS; i++) /* max fields=display types, acutally elements 1-15*/
/**		if (i != LCD_MSG_DATETIME) **/	/* save latest date/time */
			LCD.msgs[mode][i][0] = '\0'; /* clear all other types for mode */
}

int LCD_Setmode(m)
short m;
{
	int oldmode;

	/* m==0 is query */
/* djr-- 0 is no change mode */
	if (m==0)
		return LCD.mode;

	DispMode = (m & M_MASK);
	oldmode = LCD.mode;
	/** LCD.mode = (DispMode==M_NORMAL) ? 0 : 1; **/
	/* djr--changed for fp3 **/
	LCD.mode = DispMode;


	/* keypad shading? */
	if (DispMode==M_NORMAL)
	{
		/* always zap the old Query Mode or Inspect Mode display */
		/** djr--probalby not necessary now with fp3 */
		/* LCD.Zap(1); */

		/* unshade to indicate keypad connected */
		if (KPflags&DO_SHADE)
		{
		 	KPflags &= ~DO_SHADE;
			DispKeyPad();
		}
	} else if (DispMode==M_INSPECT)
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
	extern char *strncpy();
 

	 if (f < 0 || f >= LCD_MAXFIELDS)
		return;

	 (void) strncpy(LCD.msgs[mode][f], s, LCD_MAXMSGSIZE);
	DPRINTF("LCD.msgs[%d][%d]=%s\n",mode,f,s);

	 if (f==LCD_MSG_CALLINGDN)
	 {
			InfoEvent |= EVENT_NEWCALL;
	 }
}


void LCD_Relocate(relorigin)
Point relorigin;
{
	register int i;

	LCD.rect = raddp(LCD.rect, relorigin);

	for (i=0; i <= LCD_MAXFIELDS; i++)
		LCD.fieldrects[i] = raddp(LCD.fieldrects[i], relorigin);
}

/*
 * This has changed radically from fp3.
 *
 *
 */
void LCD_Display(hrow, hcol, mstring)
int hrow, hcol; /* row and col hex bytes, bcd, eg, 0x24 => 24, ie twenty four */
char *mstring;
{
	register int rlen,row,col,trow,tcol,i,j;

#ifdef  USE730WINDOWS
        if( ISTILED( P ) )
                return;
#endif /* USE730WINDOWS */
	/* convert hex bcd to int */
	trow=hrow;
	tcol=hcol;
	row = ((trow&0x00f0)/16)*10 + (hrow&0x000f);
	col = ((tcol&0x00f0)/16)*10 + (hcol&0x000f);

	DPRINTF("Display recv'd string=%s\n",mstring);

	/* parse out quotes */
	i=1; /* mstring[0] assumed to be '"' character */
	for(i=1;i<strlen(mstring);i++){
		if(mstring[i]=='"'){
 			mstring[i]='\0';
			break;
		}
	}

	/* check if row is valid    */
	if(row > DISPLAY_MAX_ROWS){
		DPRINTF("display row invalid\n");
		return;
	}

	/* check if column is valid */
	if(col > DISPLAY_MAX_COLUMNS){
		DPRINTF("display column invalid\n");
		return;
	}

	/* update phonedisplay data */
	i=col-1;
	/* j=1;  mstring[0]= '"' */
	for(j=1;j<strlen(mstring);j++,i++){
		if(i>=(DISPLAY_MAX_COLUMNS-1)) break;
		LCD.phonedisplay[row-1][i]=mstring[j];
	}

	/* display */
	if(row==1){
		(void) string(&LCD_FONT, LCD.phonedisplay[0], &display, add(LCD.rect.origin, Pt(LCD_GAP,LCD_GAP)), F_STORE);
	}else if(row==2){
		(void) string(&LCD_FONT, LCD.phonedisplay[1], &display, add(LCD.rect.origin, Pt(LCD_GAP,FONTHEIGHT(LCD_FONT)+2+LCD_GAP)), F_STORE);
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
		(void) string(&LCD_FONT, cp, &display, add(r.origin, Pt(0, FONTHEIGHT(LCD_FONT)+2)), F_XOR);
	} else
	{
		/* string fits on one line */
		(void) string(&LCD_FONT, s, &display, r.origin, F_XOR);
	}
}
