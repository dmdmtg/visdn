#define	IS_NONE		0		/* not set yet */
#define	IS_LOGGED	1		/* already logged - do not log again */
#define	IS_INCOMING	2		/* incoming call */
#define	IS_OUTGOING	3		/* outgoing call */
#define	IS_FORWARD	4		/* call was forwarded */
typedef struct LineApp {
	int status;
/*ECC*/	int line;			/* line appearance # */
/*ECC*/	int direction;			/* IS_INCOMING, etc. */
/*ECC*/	char inid[LCD_MAXMSGSIZE];	/* "InI", "OuX", etc. */
} LineApp;

/* LineApp status bits */
#define SETUP_ACK	0001		/* line setup ACK received */
#define	CONNECTED	0002		/* line is connected */
#define	HELD		0004		/* line is on hold */
#define	DIALTONE	0010		/* line has dialtone */
#define	BUSY		0020		/* line is busy */
