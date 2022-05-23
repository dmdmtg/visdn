#include <termio.h>

struct termio oldtty;

raw_term( fd )
int fd;
{
	struct termio newtty;

	ioctl( fd, TCGETA, &oldtty );
	newtty = oldtty;

	newtty.c_lflag &= ~(ECHO|ICANON); /* turn off echo&newline processing */
	/* once cannon off, c_cc stores the following data */
	newtty.c_cc[VMIN]=1;	/* read at least one character */
	newtty.c_cc[VTIME]=0;	/* wait forever for the character */

	ioctl( fd, TCSETA, &newtty );
}
unraw_term( fd )
int fd;
{
	ioctl( fd, TCSETA, &oldtty );
}
