
#define MAXCALLAPPS	8		/* # line appearances */

typedef struct LineApp {
	int status;
} LineApp;

/* LineApp status bits */
#define SETUP_ACK	0001		/* line setup ACK received */
#define	CONNECTED	0002		/* line is connected */
#define	HELD		0004		/* line is on hold */
#define	DIALTONE	0010		/* line has dialtone */
