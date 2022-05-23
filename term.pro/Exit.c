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
