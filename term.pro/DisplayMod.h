/* The X and DMD versions of this file are the same.
 * As such, you'll see some "#ifdef DMD630" code.
 *
 * Eric Claeys, 1/95
*/

/*
 * DisplayMod object interface
 *
 * DisplayMod.Display()		- display messages for current mode
 * DisplayMod.Clear()		- clear the LCD
 * DisplayMod.Zap(m)		- erase+clear LCD for mode m
 * DisplayMod.Setmode()		- switch/set display mode (normal or other)
 * DisplayMod.Setmsg()		- display/record a message string
 * DisplayMod.Relocate(p)	- relocate rectangles relative to point p
 *
 * The ProPhone model of the display is a collection of fields associated
 * with one of two display modes: M_NORMAL or M_OTHER.  M_NORMAL is
 * associated with call appearances and M_OTHER covers directory query mode,
 * inspect mode, and anything else.  The display is in one or the other mode,
 * and messages for either mode can come in at any time, so incoming display
 * strings are either displayed or saved, depending on which mode is active.
 * Numerous events can cause the display to be cleared.
 */

#define	LCD_MAXMODES	 2		/* M_NORMAL and other */
#define LCD_MAXFIELDS	16		/* LCD message types */
#define LCD_MAXMSGSIZE	48		/* max chars in each message */

MODULE(DisplayMod)
{
PUBLIC
	FUNC(void, Init);	/* Init(type) */
	FUNC(void, Display);	/* Display() */
	FUNC(void, Clear);	/* Clear() */
	FUNC(int, Setmode);	/* Setmode(m) returns old mode (m==0 is no-op) */
	FUNC(void, Setmsg);	/* Setmsg(callid, mode, string, ca) */
	FUNC(void, Relocate);	/* Relocate(relorigin) */
	FUNC(void, Zap);	/* Zap(mode) */
PRIVATE
	short		mode;
	char		msgs[LCD_MAXMODES][LCD_MAXFIELDS][LCD_MAXMSGSIZE];
#ifdef	DMD630
	Rectangle	rect;
	Rectangle	fieldrects[LCD_MAXFIELDS];
#else
	FILE *		ofp;
	char		disp[2][LCD_MAXMSGSIZE];
	int		maxline;
#endif	/* DMD630 */
};

extern void LCD_Init();

/* DisplayMod msgs field numbers */
#define LCD_MSG_CALLAPP		0
#define LCD_MSG_CALLEDDN	1
#define LCD_MSG_CALLINGDN	2
#define LCD_MSG_CALLEDNM	3
#define LCD_MSG_CALLINGNM	4
#define LCD_MSG_COS		5
#define LCD_MSG_INID		6
#define LCD_MSG_MISCINFO	7
#define LCD_MSG_FULLDISP	8
#define LCD_MSG_DATETIME	9

/*
 * "inline" functions
 */
#define	Msgstring(m, f)		msgs[(m==M_NORMAL)?0:1][f]

/* DispMode values */
#define M_MASK			0xF0
#define	M_NORMAL		0x10		/* display incoming messages */
#define	M_INSPECT		0x20		/* INSPECT function */
#define	M_QUERY			0x50		/* DIR QUERY function */

#ifndef	DMD630
extern DisplayMod LCD;
#endif	/* not DMD630 */
