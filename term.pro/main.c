#include "visdn.h"
#include <object.h>
#include <dmdproc.h>
#ifdef	USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */

extern Proc *new_proc();
extern int PhoneProc();
extern void UnixRcv();

#define		dellayer(l)		((*Sys[243])( l ))
#define		proclayer(p,r)		((*Sys[573])( p, r ))
#define		setdata(p)		((*Sys[667])( p ))

Proc *phoneproc, *infoproc, *helpproc, *mainproc;
Rectangle oldrec;
char	*Progname;

main(argc, argv)
int argc;
char *argv[];
{
	register TermProc *proc, *start;

	Progname = argv[0];
	cache(Progname, A_NO_SHOW|A_SHARED);

	InitData(argc, argv);

	mainproc = P;
	if ((phoneproc = new_proc(PhoneProc, display.rect, (struct Logio *)0,
				STKSIZE, (char **)0))==(Proc *)0)
	{
		decache();
		msgbox("NO MEMORY", NULL);
		sendchar(QUITCMD);
		exit();
	}
#ifdef	FOR_TESTING
	/* We often can't ucache ourselve, even if -C was NOT specified. */
	if (CacheOnExit == 0)
		decache();
#endif

	proc = TermProcs;			/* initialize round robin */
	oldrec = P->layer->rect;
	dellayer( P->layer );
	P->layer = 0;

	P->state |= LOCKLAYER;
	(void) request( CPU | RCV | SEND | DELETE | RESHAPED );

	while (1)
	{
		wait( CPU | RCV | SEND | RESHAPED );

		UnixThere = 5;	/* we get 5 clock ticks before they give up */

		if (GlobalEvent)
		{
			if (GlobalEvent&GLOBAL_QUIT)
			{
				UnixQuit();
			}
		}

		if( own() & RESHAPED )
		{
#ifdef USE730WINDOWS
			if( ! ISTILED( phoneproc ) )
#endif /* USE730WINDOWS */
			{
				ReShaped = 1;
				ReLocate();
				if (!(P->state & MOVED))
					ReDisplay();
			}
			P->state &= ~(RESHAPED | MOVED);
		}

		if (UnixEvent)
		{
			/*
			 * must take care of EVENT_RCVBUF
			 * before calling UnixRcv in case we need our rcvbuf
			 */
			if (UnixEvent & EVENT_RCVBUF)
				UnixRcvMsg();
		}

		if (own() & RCV)
			UnixRcv();

		if ( own() & DELETE )
		{
#if 0
			msgbox( "VISDN", "is still", "active", (char *) 0);
			/* you can't delete me */
			P->state &= ~USERDELETE;
			bttns( 0 );	/* let go of the mouse button */
#else
			UnixQuit();
#endif
		}

		/* round robin schedule for outgoing requests */
		start = proc;
		do {
			if (proc->event & EVENT_SENDBUF)
			{
				/* Assume there's already a trailing \n. */
				UPRINTF("SND 0x%x %s", *proc->sndbuf&0xff, proc->sndbuf+1);

				/* bump up heartbeat in case we get blocked */
				UnixThere = -1;
				sendnchars(strlen(proc->sndbuf), proc->sndbuf);
				proc->event &= ~EVENT_SENDBUF;
				if (++proc >= &TermProcs[TERM_PROCS])
					proc = TermProcs;
				break;
			} else
				proc++;

			if (proc >= &TermProcs[TERM_PROCS])
				proc = TermProcs;

		} while (proc != start);
	}
}

void UnixRcv()
{
	register int c;

	if (UnixRcvProc != (TermProc *)0)
	{
		if (UnixRcvProc->event & EVENT_RCVBUF)
		{
			if (++UnixRcvBlockCnt > 2*TERM_PROCS) {
				return;		/* waiting for rcvbuf to free */
			}

			/*
			 * A process may have died prematurely... ignore its
			 * message.
			 */
			UnixRcvProc = &TermProcs[PROC_UNIX];
			UnixCurCmd = IGNORECMD;
			UPRINTF(" IGNORED");
		}
	}

	UnixRcvBlockCnt = 0;

	while ((c = rcvchar()) != -1)
	{
		if (UnixRcvProc==(TermProc *)0)
		{
			if (c=='\n') {
				UPRINTF("got sync \\n.\n");
				continue;
			}
			/* c is a process id + message # */
			UnixCurCmd = c;
			UPRINTF("CMD 0x%x", c);
			if (((c = (c&PROCMASK)>>PROCSHIFT)) >= TERM_PROCS)
			{
				/* bad message ... ignore it */
				c = 0;
			}
			UnixRcvProc = &TermProcs[c];
			if (UnixRcvProc->event & EVENT_RCVBUF) {
				return;		/* wait for buf to clear */
			}
		}

		if (UnixBufPtr==NULL)
		{
			UnixBufPtr = UnixRcvProc->rcvbuf;

			switch (*UnixBufPtr++ = UnixCurCmd)
			{
			case ERASECHAR:
			case KILLCHAR:
				UnixExpectCnt = 1;
				break;
			case FACEDATA:
				UnixExpectCnt = ICONMSGSIZE;
				break;
			default:
				UnixExpectCnt = -1;
				break;
			}
			UPRINTF(" BUF %d", UnixExpectCnt);
			*UnixBufPtr = '\0';
		} else if (UnixExpectCnt<0 && c=='\n')
		{
			*UnixBufPtr = '\0';
			UnixBufPtr = NULL;
			UnixExpectCnt = -1;
			UnixRcvProc->event |= EVENT_RCVBUF;
			UPRINTF(" GOT '%s'\n", UnixRcvProc->rcvbuf);
			UnixRcvProc = (TermProc *)0;
			return;
		} else
		{
			*UnixBufPtr++ = c;
			*UnixBufPtr = '\0';
			if (--UnixExpectCnt==0)
			{
				UnixExpectCnt = -1;
				UnixRcvProc->event |= EVENT_RCVBUF;
				UnixBufPtr = NULL;
				UPRINTF(" GOTCNT '%s'\n", UnixRcvProc->rcvbuf);
				UnixRcvProc = (TermProc *)0;
			} else if (UnixBufPtr >= &UnixRcvProc->rcvbuf[RCVBUFSIZE-2])
			{
				/*
				 * handle buffer overflow: send what you have,
				 * ignore the rest.
				 */
				UPRINTF(" OVERFLOW '%s'\n", UnixRcvProc->rcvbuf);
				UnixRcvProc = &TermProcs[PROC_UNIX];
				UnixBufPtr = UnixRcvProc->rcvbuf;
				*UnixBufPtr++ = IGNORECMD;
				*UnixBufPtr = '\0';
				return;
			}
		}
	}
}

char *String(s)
register char *s;
{
	register char *t, *cp;
	register int len;
	extern char *alloc();

	if (s==NULL)
		len = 0;
	else
		for (len=0, t=s; *t; t++)
			len++;
	if ((cp = t = alloc(len+1))==NULL)
	{
		msgbox("NO MEMORY", NULL);
		delete();
	}

	if (s)
		while (*s)
			*t++ = *s++;
	*t = '\0';

	return cp;
}

/*
 * Left Justified String()
 *
 * For menu strings, appending a LJCHAR character causes left justification
 */
char *LJString(s)
register char *s;
{
	register char *t, *cp;
	register int len;
	extern char *alloc();

	if (s==NULL)
		len = 0;
	else
		for (len=0, t=s; *t; t++)
			len++;
	if ((cp = t = alloc(len+2))==NULL)
	{
		msgbox("NO MEMORY", NULL);
		delete();
	}

	if (s)
		while (*s)
			*t++ = *s++;
	*t++ = LJCHAR;
	*t = '\0';
	return cp;
}

/*
 * Left Justified strcmp()
 *
 * For menu strings, the LJCHAR pad character must be ignored during strcmp()
 */
LJstrcmp(s1, s2)
register char *s1, *s2;
{
	if (s1==NULL)
		return s2==NULL ? 0 : 1;
	else if (s2==NULL)
		return 1;
	while (*s1 && *s2 && *s1==*s2)
	{
		++s1;
		++s2;
	}

	if (*s1 != *s2)
	{
		if (!((*s1=='\0' && *s2==LJCHAR) || (*s2=='\0' && *s1==LJCHAR)))
			return 1;
	}

	return 0;
}

UnixRcvMsg()
{
	UnixEvent &= ~EVENT_RCVBUF;

	switch (TermProcs[PROC_UNIX].rcvbuf[0])
	{
	case TOCITEM:
	case HELPLINE:
		HelpParse(TermProcs[PROC_UNIX].rcvbuf);
		break;
	default:
		break;
	}
}

UnixQuit()
{
	if( phoneproc != 0 )
		GlobalEvent |= GLOBAL_QUIT;

	/* leave GlobalEvent for clients to find */
	UnixEvent = 0;		/* clear my semaphore */
	HelpEvent = 0;		/* clear client semaphores */

	if( infoproc )
	{
		infoproc->state |= USERDELETE;
		setrun( infoproc );
	}

	if( helpproc )
	{
		helpproc->state |= USERDELETE;
		setrun( helpproc );
	}

	wait(CPU);	/* let info proc run */
	sleep( 15 );
	wait(CPU);	/* let help proc run */

	proclayer( P, oldrec );	/* gives process layer back */
	setdata( P );

	if (!CacheOnExit)
	{
		sleep(60);		/* wait for co-process to die */
		decache();		/* should be able to decache by now */
	}

/*XXXXXXXXXX only send this if the UNIX side didn't tell us
 * it was leaving (it's probably already gone).
*/
	sendchar(QUITCMD);

wait(CPU);	/* XXX: Ted Lowe added this -- is it needed? */
	Exit( HostPort );
wait(CPU);	/* XXX: Ted Lowe added this -- is it needed? */
	exit();
}

HelpParse(s)
register char *s;
{
	register HelpItem *it, *pt;
	register HelpTmenu *tp;
	char *parentag, *tag, *text, *savetag;
	char cmd;
	register HelpLine *lp, *newlp, *olp;
	HelpItem *lastitem;
	extern HelpItem *HelpMenuGen();

	cmd = *s++;

	for (parentag=s; *s && *s != ';'; s++)
		;
	if (*s != ';')
	{
		HPRINTF("bad parentag string '%s'\n", parentag);
		return;
	}
	*s++ = '\0';
	for (text=s; *s && *s != ';'; s++)
		;
	if (*s != ';' || s==text)
	{
		HPRINTF("bad text string '%s'\n", text);
		return;
	}
	*s++ = '\0';
	for (tag=s; *s && *s != ';'; s++)
		;
	if (*s != ';' || s==tag)
	{
		HPRINTF("bad tag string '%s'\n", tag);
		return;
	}
	*s++ = '\0';

	switch (cmd)
	{
	case TOCITEM:
		/* locate parent item */
		/* this code relies on preserving order of requests */
		if (*parentag=='\0')
		{
			pt = (HelpItem *)0;
			tp = &HelpMenu;
		} else
		{
			for (pt=HelpItems; pt; pt=pt->link)
				if (strcmp(pt->tag, parentag)==0)
					break;
			if (pt==(HelpItem *)0)
			{
				HPRINTF("parent tag %s not found\n", parentag);
				break;
			}
			tp = pt->next;
		}
		
		if (tp==(HelpTmenu *)0)
		{
			/* pt must be valid and not have a submenu yet */
			HPRINTF("add menu to item %s\n", it->text);
			pt->next = tp = new(HelpTmenu);
			tp->item = (HelpItem *)0;
			tp->prevhit = tp->prevtop = 0;
			tp->generator = HelpMenuGen;
			tp->menumap = HELPMAP;
			tp->first = (HelpItem *)0;
		}

		/* check for dup tag and remember last one in chain */
		for (lastitem=it=HelpItems; it; lastitem=it, it=it->link)
			if (strcmp(it->tag, tag)==0)
				break;

		if (it != (HelpItem *)0)
		{
			HPRINTF("dup tag %s\n", tag);
			break;
		}

		/* new item to add to tp menu */
		HPRINTF("add item %s (tag %s)\n", text, tag);
		it = new(HelpItem);
		it->link = (HelpItem *)0;
		it->text = LJString(text);
		it->next = (HelpTmenu *)0;
		it->tag = String(tag);
		if (lastitem)
			lastitem->link = it;
		else
			HelpItems = it;
		if (tp->first==(HelpItem *)0)
			tp->first = it;
		else
		{
			for (pt=tp->first; pt->nextitem; pt=pt->nextitem)
				;
			pt->nextitem = it;
		}
		break;

	case HELPLINE:
		for (savetag=NULL, olp=lp=HelpLines; lp; olp=lp, lp=lp->next)
		{
			if (strcmp(lp->tag, tag)==0)
			{
				for (savetag=lp->tag, olp=lp; lp && lp->tag==savetag;
					olp=lp, lp=lp->next)
						;
				break;
			}
		}

		if (savetag==NULL)
		{
			/* must go look for the tag in the menu structure */
			for (it=HelpItems; it; it=it->link)
				if (strcmp(it->tag, tag)==0)
				{
					savetag = it->tag;
					break;
				}
		}

		newlp = new(HelpLine);
		newlp->text = String(text);
		newlp->tag = savetag != NULL ? savetag : String(tag);

		/* insert new line after last matching tag */
		if (olp)
		{
			HPRINTF("append %s %s after %s %s\n",
					newlp->tag, newlp->text,
					olp->tag, olp->text);
			newlp->next = olp->next;
			olp->next = newlp;
		} else
		{
			/* must be the first item on the list */
			HPRINTF("first help line %s %s\n", newlp->tag, newlp->text);
			newlp->next = (HelpLine *)0;
			HelpLines = newlp;
		}

		/* notify help process */
		HelpEvent |= EVENT_HELPLINE;

		break;
	}
}

/*
 * post a message for the UNIX host side
 */
SendHost(p, cmd, str, eom)
int p;
char cmd;
register char *str;
char eom;
{
	register char *s;
	register TermProc *tp;

	tp = &TermProcs[p];

	while (tp->event&EVENT_SENDBUF)
	{
		/* keep our heartbeat going while we wait */
		tp->alive++;
		wait(CPU);
	}

/* XXX FIX: this doesn't check for overflowing the
 *	sndbuf (SNDBUFSIZ-1 (eom) -1 (\0) -1 (cmd),
 *	but I doubled the size of sndbuf, so hopefully
 *	we'll never overflow.
*/
	tp->sndbuf[0] = cmd;
	s = &tp->sndbuf[1];
	if (str) {
		for (; *str; str++ )
			*s++ = *str;
	}

	/* Except for a special case of debugging, the \n signals
	 * the end of the message we're sending.
	 * This implies that (except for the special debugging case),
	 * messages can't have \n in them.
	*/
	*s++ = eom;

	/* The NULL doesn't actually get sent. */
	*s = '\0';

	tp->event |= EVENT_SENDBUF;
}

#ifdef	DEBUG
/* Format and send a debugging message to the UNIX host.
 * There currently aren't any *PRINTF() calls with more than 5 arguments.
 * We really need varargs on the 730.
*/
/*VARARGS*/
debug_printf(fmt, a1, a2, a3, a4, a5)
char	*fmt;
char	*a1, *a2, *a3, *a4, *a5;
{
	char	msg[500];

	(void) sprintf(msg, fmt, a1, a2, a3, a4, a5);
	/* We need a different End of Message (eom) character because "msg"
	 * may contain a \n, which is the default eom.
	*/
	SendHost(PROC_PHONE, DEBUGLINE, msg, DEBUG_EOM);

/* XXXXX FIX ??
 *	This is kinda kludgy, but it works.
 *	If the debug level is > 1000, write to the visdn window in addition
 *	to sending to the UNIX side.
*/
	if (Debug > 1000)
		lprintf(msg);
}
#endif	/* DEBUG */
