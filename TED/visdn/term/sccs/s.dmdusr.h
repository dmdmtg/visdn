h04818
s 00129/00000/00000
d D 1.1 92/07/23 13:58:57 trl 1 0
c date and time created 92/07/23 13:58:57 by trl
e
u
U
t
T
I 1
/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


/* dmdusr.h -	630 User Interface
**
**	This include file contains structure declarations
**	to support the NewI subsystem and the Usrenv subsystem.
*/


/* 
**	NewI subsystem:
**
**	This include file contains structure declarations
**	to support the dynamic New submenu from the global
**	button 3 menu.
**
**	In the 630 Release 1.1, the New submenu is defined
**	statiscally with two items "Host1" and "Host2". 
**	Thus, this include file does not exist.
**
**	In Release 1.7 and beyond, the New submenu is
**	dynamically created to support as many logical
**	hosts as there are physical ports. Subitems is
**	also a new concept.
*/

#define	NINAMELEN	40	/* should be enough */

typedef struct NewI {
	char ni_name[NINAMELEN];	/* name of logical host */
	unsigned int ni_state;		/* NEWI_MOUNT, NEWI_UMOUNT */
	unsigned int ni_major;		/* major number */
	unsigned int ni_minor;		/* minor number */
	struct Bitmap *ni_bmap;		/* Bitmap to be displayed */
	struct Ioblock *ni_iob;		/* I/O block start of stream */
	struct NewI *ni_next;		/* linked list */
} NewI;

/* Possible states: */
#define NEWI_MOUNT	0
#define NEWI_UMOUNT	1

/* Global variables
**
*/
extern struct NewI *Nihead;	/* $libsys/newi.c */


/* Functions
**
*/
struct NewI *ni_index();	/* $libsys/newi.c */
struct NewI *ni_sethost();	/* $libsys/newi.c */
struct NewI *ni_iob();		/* $libsys/newi.c */
struct NewI *ni_unlink();	/* $libsys/newi.c */
struct NewI *ni_findmajor();	/* $libsys/newi.c */
char *ni_name();		/* $libsys/newi.c */





/*
**	Usrenv subsystem:
**
**	The user environment has been accessed in a very
**	ad-hoc manner in release 1.1. It was possible since
**	all the parameters are basically stored in BRAM.
**
**	In Release 1.7 and beyond, the way to access those
**	user environment parameters are through the private
**	section of the structure Usrenv.
**
**	The structure Usrenv can be linked so a logical host
**	can jump from one environment to another. For example,
**	from nonmux to mux or vice versa.
*/

typedef struct Usrenv {
	/* public */
	void *(*info)();		/* typeless info function */
	void *(*nego)();		/* typeless negotiate function */

	/* private */
	char *data;			/* typeless data storage */
	struct Usrenv *next;		/* next environment */
} Usrenv;


/* Requests for info function */
#define U_FONT		1
#define U_COLUMNS	2
#define U_ROWS		3
#define U_SWEEP		4
#define U_SHOW		5
#define U_MAPPING	6
#define U_SAVRECT	7
#define U_LOG		8
#define U_XORIGIN	9
#define U_YORIGIN	10
#define U_EMUL		11

#define F_NEW		16
#define F_EXIT		17
#define F_RSHAPE	18
#define F_DLETE		19

/* Responses */

/* to U_SHOW */
#define IS_SESAME	0	/* assertive, will appear on New and POR */
#define IS_MOUNT	1	/* easy going, will appear on New */
#define IS_UNMOUNT	2	/* shy, don't want to appear anywhere */


/* External functions */
void env_link();		/* $libsys/newi.c */
Usrenv *env_back();		/* $libsys/newi.c */


/* Some useful macros */
/* The macros xset and yset have been restored to procedure calls */

E 1
