h39776
s 00022/00000/00000
d D 1.1 92/07/23 13:58:28 trl 1 0
c date and time created 92/07/23 13:58:28 by trl
e
u
U
t
T
I 1
#include <dmd.h>

#undef	LOG_HOST1

#include "Fw_io.h"

#define	ni_index	((NewI * (*)()) (*Sys[881]))
#define	Usysctl		(*Sys[115])
#define	C_EXIT		5

void
Exit( port )
int	port;
{
	Fw_Logio *host = (Fw_Logio *)((ni_index(port))->ni_iob);
	/* See if the host is connected still */
	if(host->numprocs == 1 && host->countprocs == 1 )
	{
		/* If it is alive, Exit it */
		Usysctl( host->chanproc[0], C_EXIT, (long) host );
	}
}
E 1
