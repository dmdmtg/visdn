h30969
s 00012/00000/00000
d D 1.1 92/07/23 13:57:28 trl 1 0
c date and time created 92/07/23 13:57:28 by trl
e
u
U
t
T
I 1

#define MAXCALLAPPS	8		/* # line appearances */

typedef struct LineApp {
	int status;
} LineApp;

/* LineApp status bits */
#define SETUP_ACK	0001		/* line setup ACK received */
#define	CONNECTED	0002		/* line is connected */
#define	HELD		0004		/* line is on hold */
#define	DIALTONE	0010		/* line has dialtone */
E 1
