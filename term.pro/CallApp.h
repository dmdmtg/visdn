/* The X and DMD versions of this file are the same.
 * Eric Claeys, 1/95
*/

/* Define the values for the LineApp->direction field. */
#define	IS_NONE		0		/* not set yet */
#define	IS_LOGGED	1		/* already logged - do not log again */
#define	IS_INCOMING	2		/* incoming call */
#define	IS_OUTGOING	3		/* outgoing call */

/* The INID is never > 4 chars (3+optional "*"), so don't use LCD_MAXMSGSIZE */
#define LCD_INIDSIZE	(4+1)		/* +1 for NULL */
/* The call app (e.g., " a=") is never > 3 chars, so don't use LCD_MAXMSGSIZE */
#define LCD_CALLAPPSIZE	(3+1)		/* +1 for NULL */
typedef struct LineApp {
	int status;			/* see below for possible values */
	int line;			/* line appearance # or -1 if none */
	int direction;			/* IS_INCOMING, etc. */
	unsigned long start_time;	/* starting time of current call */

	/* Save some of the fields in the LCD that can be out of sync
	 * with reality.  Only save the fields we can write to the log file.
	*/

	char inid[LCD_INIDSIZE];	/* "InI", "OuX", etc. */
	char callapp[LCD_CALLAPPSIZE];	/* e.g., " a=" */
	char calleddn[LCD_MAXMSGSIZE];	/* called number */
	char callednm[LCD_MAXMSGSIZE];	/* called name */
	char callingdn[LCD_MAXMSGSIZE];	/* calling number */
	char callingnm[LCD_MAXMSGSIZE];	/* calling name */
} LineApp;

/* LineApp status bits */
#define SETUP_ACK	0001		/* line setup ACK received */
#define	CONNECTED	0002		/* line is connected */
#define	HELD		0004		/* line is on hold */
#define	DIALTONE	0010		/* line has dialtone */
#define	BUSY		0020		/* line is busy */
