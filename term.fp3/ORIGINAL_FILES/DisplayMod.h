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

#define	LCD_MAXMODES	 6		/* fp3 does all modes */
#define LCD_MAXFIELDS	16		/* LCD message types, fp3 has 15 */
#define LCD_MAXMSGSIZE	48		/* max chars in each message */
#define DISPLAY_MAX_ROWS 2
#define DISPLAY_MAX_COLUMNS 41  /* 40 + 1 for null byte */

MODULE(DisplayMod)
{
PUBLIC
	FUNC(void, Init);	/* Init(type) */
	FUNC(void, Display);	/* Display() */
	FUNC(void, Clear);	/* Clear() */
	FUNC(int, Setmode);	/* Setmode(m) returns old mode (m==0 is no-op */
	FUNC(void, Setmsg);	/* Setmsg(callid, mode, string) */
	FUNC(void, Relocate);	/* Relocate(relorigin) */
	FUNC(void, Zap);	/* Zap(mode) */
PRIVATE
	Rectangle	rect;
	short		mode;
	char		msgs[LCD_MAXMODES][LCD_MAXFIELDS][LCD_MAXMSGSIZE];
	Rectangle	fieldrects[LCD_MAXFIELDS];
	char		phonedisplay[DISPLAY_MAX_ROWS][DISPLAY_MAX_COLUMNS];  /*+1 for null byte */
};

extern void LCD_Init();

/* DisplayMod msgs field numbers */
/* djr needs fp3 updateing, should start at 0 to remove strange case */
/* seen in Hayes.c case statment for display weird index math [2]-1 */
/* changed them to hex values */
#define LCD_MSG_CALLAPP		0x01
#define LCD_MSG_CALLEDDN	0x02
#define LCD_MSG_CALLINGDN	0x03
#define LCD_MSG_CALLEDNM	0x04
#define LCD_MSG_CALLINGNM	0x05
#define LCD_MSG_COS		0x06
#define LCD_MSG_INID		0x07
#define LCD_MSG_MISCINFO	0x08
#define LCD_MSG_FULLDISP	0x09
#define LCD_MSG_DATETIME	0x0A
#define LCD_MSG_REDIRINGNUM	0x0B
#define LCD_MSG_REDIRINGNAME	0x0C
#define LCD_MSG_REDIRDNUM	0x0D
#define LCD_MSG_REDIRDNAME	0x0E
#define LCD_MSG_CONNPRTYNUM	0x0F

/*
 * "inline" functions
 */
/* #define	Msgstring(m, f)		msgs[(m==M_NORMAL)?0:1][f] */
#define	Msgstring(m, f)		msgs[m][f] 

/* DispMode values */
/* djr--changed for fp3, in prophone these were all of form f0 10 20 */
#define M_MASK			0x0F
#define	M_NORMAL		0x01		/* display incoming messages */
#define	M_INSPECT		0x02		/* INSPECT function */
#define	M_QUERY			0x05		/* DIR QUERY function */
/* djr--added for fp3 */
#define	M_NOCHANGE			0x00		/* no change, visdn3 handles as query */
#define	M_MISCDISPLAY			0x03		/* misc. display function */
#define	M_MSGRETRIEV			0x04		/* message retrieval function */
