/* This file is shared between the 730 and X versions.
 * Hence, you'll see some "#ifdef DMD630" lines.
 * Eric Claeys, 1/95
*/

#include "pro_strings.h"

#ifdef	DMD630
#include "visdn.h"
/* USE730WINDOWS is only used with DMD630, so to lessen the number of
 * #ifdef's, don't include USE730WINDOWS in DMD630's.
*/
#ifdef	USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */

extern int OutEnQueue();

#else	/* not DMD630 */

#include "xisdn.h"

#include <stdio.h>

extern	FILE	*Xofp,	/* Xtent output */
		*Pofp;	/* Phone line output */

void
_OutEnQueue(string)	/* is #defined to OutEnQueue() in xisdn.h */
char	*string;
{
	register char	*cp, *tcp;
	char	buf[100];	/* writeable copy of "string" */
	int	dialit;		/* only keypad digits seen? */
	int	n;
	extern	void	HayesSend();

	/* Since we shouldn't overwrite "string", save it in a temp buffer. */
	if (string[0] == 'C' && string[1] == 'A') {
		/* skip the "CA" */
		(void) strcpy(buf, &string[2]);

		/* Now get the 1st free call appearance */
		HayesSend(FreeLinestring, strlen(FreeLinestring), 0);

/* XXX FIX:  really need to wait for DIALTONE instead of sleeping */
		DPRINTF("    sleep(2) instead of waiting for dialtone...");(void) sleep(2); DPRINTF("done\n");
	} else {
		(void) strcpy(buf, string);
	}

/* XXXX FIX: simulate part of what HayesXmit() does. */

do_it:
	dialit = 1;
	n = 0;
	cp = buf;
	while (*cp) {
		switch(*cp) {
		case '+':	/* send first part and wait for dialtone */
			if (cp != buf) {
				/* we have some characters to send */
				HayesSend(buf, n, dialit);
			} /* else the '+' was the 1st char */

/* XXX FIX:  really need to wait for 2ndary DIALTONE instead of sleeping */
			DPRINTF("    sleep(2) instead of waiting for 2ndary dialtone...");(void) sleep(2); DPRINTF("done\n");

			/* skip '+' and copy string to front of buffer */
			while (*cp=='+') cp++;
			for (tcp=buf; *tcp = *cp; tcp++, cp++)
				;
			goto do_it;
			break;

		case '>':	/* If the 1st char is ">", send redial,
				 * otherwise send first part and wait
				 * for connect.
				*/
			if (cp != buf) {
				/* we have some characters to send */
				HayesSend(buf, n, dialit);
			} else {
				/* The '>' was the 1st char of the string. */
				HayesSend(Redialstring, strlen(Redialstring), 0);
			}

			/* Now wait for connect. */
/* XXX FIX:  really need to wait for CONNECT instead of sleeping */
			DPRINTF("    sleep(2) instead of waiting for connect...");(void) sleep(2); DPRINTF("done\n");

			/* skip '>' and copy string to front of buffer */
			while (*cp=='>') cp++;
			for (tcp=buf; *tcp = *cp; tcp++, cp++)
				;
			goto do_it;
			break;

		case ',':	/* send first part and wait for 1/2 second */
		case '~':	/* to be compatible with FP3 version */
			if (cp != buf) {
				/* we have some characters to send */
				HayesSend(buf, n, dialit);
			}

			/* Now pause.  Since in UNIX there's no generic way
			 * to pause for 1/2 second, sleep() instead.
			 * However, for every 2 pause characters, sleep 1 second,
			 * rounding up for odd-number of pause characters.
			*/
			{
				int sec, num;
				num = 0;
			 	while (*cp == ',' || *cp == '~') {
					num++;
					cp++;
				}
				sec = (num+1) / 2;
/* XXX FIX:  really need to pause for 1/2 sec instead of sleeping */
				DPRINTF("    pausing %d sec...", sec);(void) sleep(sec); DPRINTF("done\n");
			}
#ifdef	NO
			/* copy string to front of buffer, incl. more ','s */
			for (cp++, tcp=buf; *tcp = *cp; tcp++, cp++)
				;
#else
			/* copy string to front of buffer, NOT incl. ',' and/or '~' */
			for (tcp=buf; *tcp = *cp; tcp++, cp++)
				;
#endif	/* NO */
			goto do_it;
			break;

		case ';':	/* send first part, wait for ack, continue */
			if (cp != buf)
			{
				/* we have some characters to send */
				HayesSend(buf, n, dialit);
			}

/* XXX FIX:  really need to wait for ACK instead of sleeping */
			DPRINTF("    sleep(1) instead of waiting for ack...");(void) sleep(1); DPRINTF("done\n");

			/* copy string to front of buffer */
			while (*cp==';') cp++;
			for (tcp=buf; *tcp = *cp; tcp++, cp++)
				;
			goto do_it;
			break;

		default:
			if (!((*cp>='0' && *cp<='9') || *cp=='*' || *cp=='#'))
				dialit = 0;	/* non-keypad char seen */
			n++;
			cp++;
		}
	}

	if (n != 0) {
		HayesSend(buf, n, dialit);
	}
}

#endif	/* DMD630 */

/* Convert the given number to a legal phone number.
 * The given number may have a leading "=" which means it's a pq-style
 * phone number (areacode exchange extension) that doesn't have any
 * alphabetic characters, so az_to_09() shouldn't be called.
 * Otherwise, az_to_09() it, and convert it to the appropriate
 * length number (e.g., if it's an inside number, just
 * dial the extension, not the whole number).
*/
static char *
get_phone_number(num)
char	*num;
{
	static	char	new_number[50];		/* converted phone number */
	char	*o, *n, *re;
	char	area[8], exch[8], ext[8];
	extern	char		snarfstr[], *regex(), *__loc1;
#ifndef	DMD630	/* already defined */
	extern  DialPlanMod     DialPlan;
#endif	/* not DMD630 */

	if (num[0] == '=') {
		n = &num[1];		/* Skip the leading '=' */
	} else {
		for (o=num, n=new_number; *o; n++, o++)
			*n = az_to_09(*o);
		*n = '\0';
		n = new_number;
	}

	area[0] = exch[0] = ext[0] = '\0';
	re = regex(snarfstr, n, area, exch, ext);

/* XXXXXXXX	See the comment in Hayes.c for weirdness about regex().  */
/*XXXYYY
MPRINTF("get_phone_number(%s): __loc1=0x%lx (%s), re=0x%lx (%s)\n", num,
(long)__loc1, __loc1 ? __loc1 : "null",
(long)re,  re ? re : "null");
XXXYYY*/

/* XXXX	Don't ask me why, but this regex often returns NULL on SUCCESS,
 *	but it's supposed to return it on FAILURE.
*/
	if (re == (char *)NULL) {
		/* This shouldn't happen for pq-style numbers (num[0] == '='),
		 * since they should have an area code, exchange, and extension.
		*/
		MPRINTF("get_phone_number(%s): regex() failed\n", n);
		return(n);
	}
	MPRINTF("get_phone_number(%s): regex returned: %s %s %s\n", n, area, exch, ext);
	(void) sprintf(new_number, "%s%s",
		DialPlan.MapExch(*area ? area : DialPlan.areacode, exch), ext);

	return(new_number);
}

/* Perform remote actions (rmtact) based on the specified string. */
void
rmtact( s )
char *s;
{
#ifdef	DMD630
#ifdef	USE730WINDOWS
	if ( strcmp( "toggle", s ) == 0 )
	{
		if( ISTILED( P ) )
			untile730wins( P );
		else
			tile730wins( P );
	}
	else if ( strcmp( "close", s ) == 0 )
	{
/* XXXXX FIX:	Mike Sheehan reported a bug on the 486 where
 *		if the window hadn't been iconized yet, doing this
 *		here would hang the 730.  This hasn't been seen on the Sun,
 *		so perhaps it's a bug in the 730WINDOWS library???
*/
		if( ! ISTILED( P ) )
			tile730wins( P );
	}
	else if ( strcmp( "open", s ) == 0 )
	{
		if( ISTILED( P ) )
			untile730wins( P );
	}
	else
#endif	/* USE730WINDOWS */

	if ( strcmp( "exit", s ) == 0 )
	{
		PhoneExit();
	}
	else if ( strcmp( "uncache/exit", s ) == 0 )
	{
		CacheOnExit = 0;
		PhoneExit();
	}
#else
	if ( strcmp( "exit", s ) == 0 )
	{
		/* Tell the X program to exit.
		 * That will then cause us to exit also.
		*/
		(void) fprintf(Xofp, "xtent.XtExit: 0\n");
		(void) fflush(Xofp);
	}
#endif	/* DMD630 */

	else if ( strncmp( "conference", s, 10 ) == 0 )
	{
		if (s[10] == ' ') {	/* a number is specified */
			char	string[100];

			/* Steps:
			 *
			 * Must be on-line already (assume this).
			 * Press conference button.
			 * Wait for 2ndary dial tone.
			 * Dial the number.
			 * The user must then:
			 *	Wait for connect (may not be necessary).
			 *	Press conference button again when ready.
			*/
			OutEnQueue(&PhoneOutQueue, Conferencestring);
			(void) sprintf(string, "+%s", get_phone_number(&s[11]));
			OutEnQueue(&PhoneOutQueue, string);
		} else {
			/* Just push the button */
			OutEnQueue(&PhoneOutQueue, Conferencestring);
		}
	}
	else if ( strcmp( "drop", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, Dropstring);
	}
	else if ( strcmp( "hangup", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, Hangupstring);
	}
	else if ( strncmp( "transfer", s, 8 ) == 0 )
	{
		if (s[8] == ' ') {	/* a number is specified */
			char	string[100];

			/* Steps:
			 *
			 * Must be on-line already (assume this).
			 * Press transfer button.
			 * Wait for 2ndary dial tone (use "+").
			 * Dial the number.
			 * The user must then:
			 *	Wait for connect (may not be necessary).
			 *	Press transfer button again when ready.
			*/
			OutEnQueue(&PhoneOutQueue, Transferstring);
			(void) sprintf(string, "+%s", get_phone_number(&s[9]));
			OutEnQueue(&PhoneOutQueue, string);
		} else {
			/* Just push the button */
			OutEnQueue(&PhoneOutQueue, Transferstring);
		}
	}
	else if ( strncmp( "button", s, 6 ) == 0 )
	{
		char	string[20];
		(void) strcpy(string, Buttonstring);
		if (s[6] == ' ')
			(void) strcat(string, &s[7]);
		OutEnQueue(&PhoneOutQueue, string);
	}
	else if ( strcmp( "hold", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, Holdstring);
	}
	else if ( strcmp( "mute", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, Mutestring);
	}
	else if ( strcmp( "shift", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, Shiftstring);
	}
	else if ( strcmp( "redial", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, Redialstring);
	}
	else if ( strcmp( "speaker", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, Speakerstring);
	}
	else if ( strcmp( "adjunct", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, Adjunctstring);
	}
	else if ( strncmp( "CFA", s, 3 ) == 0 )
	{
		if (s[3] == ' ') {
			char	string[100], *p;
			char	*number;
			extern	int	speaker_on, adjunct_on, remote_on;

			/* Steps to program CFA:
			 *
			 * Must be on-hook (assume this).
			 * Get a free CA (turns speaker phone on).
			 * Dial #8.
			 * Wait for 2ndary dial tone (use "+").
			 * Send the number followed by a "#"
			 * Wait for confirmation tone.
			 * Turn speaker off.
			 * The user must manually activate CFA when ready.
			 *
			 * Step to activate CFA:
			 *
			 * Toggle the CFA button.
			*/

			number = get_phone_number(&s[4]);
			(void) sprintf(string, "CA#8+");    /* 5 chars long */

			/* Append the phone number.
			 * If it includes the escape (e.g., for an outside line)
			 * and the escape sequence includes a "+" to wait
			 * for 2ndary dialtone, then we'll hang waiting since
			 * we won't get dialtone (we're not dialing a
			 * number -- we're just programming the CFA button.
			 * So, we need to strip out any "+" from the number;
			*/
			for (p=&string[5]; *number != '\0'; p++, number++) {
				if (*number == '+')
					number++;
				*p = *number;
			}
			/* End the number and wait for confirmation tone.
			 * It just so happens that we get dialtone after
			 * the confirmation tone, so we can just wait for
			 * normal dialtone and it will work.
			 * The code to wait for confirmation tone doesn't exist.
			 * If you want to add it, go for it!
			*/
			(void) strcpy(p, "#+");

			/* Have to send Speakerstring out separately,
			 * or else the alphabetic characters get converted
			 * to numbers.
			*/
			OutEnQueue(&PhoneOutQueue, string);
/*XXX FIX ??? -- should it be "Speakerstring" or "Adjunctstring"??
 * XXX if they have an adjunct phone, it should be Adjunctstring, else Speakerstring.
*/
/* adjuct_on and speaker_on will be 0, since all OutEnQueue()'s done is
 * queued the requests.  They won't actually get sent to the phone,
 * and it's response handled, until later.
 * Perhaps we could wait here until they're handled????
*/
/* DPRINTF("adjunct_on=%d, speaker_on=%d\n", adjunct_on, speaker_on);/* FIX: for testing */
			OutEnQueue(&PhoneOutQueue, Speakerstring);
		} else {
			/* For the 'else' part, this just toggles the button.
		 	* Don't do this for this 'if' part also,
		 	* since it will toggle CFA, and if it was on,
			* it will now be off.
		 	* Let the user turn CFA on if they want.
			*/
			OutEnQueue(&PhoneOutQueue, CFAstring);
		}
	}
	else if ( strncmp( "line", s, 4 ) == 0 )
	{
		if (s[4] == ' ') {
			char	l, string[10];
			int	ca;
#ifndef	DMD630	/* it's a global on the 630 */
			extern	int	CallApps;
#endif

			/* Can be from 1 - 30 CAs. */
			l = s[5];
			if (l >= 'a' && l <= 'z')
				ca = l - 'a' + 1;
			else if (l >= 'A' && l <= 'D')
				ca = l - 'A' + 1 + 26;
			else
				ca = atoi(&s[5]);

			/* Make sure they're not asking for a CA that
			 * they don't have.
			*/
			if (ca > CallApps) {
#ifdef	DMD630
				lprintf(
#else
				(void) fprintf(stderr,
#endif	/* DMD630 */
				"You only have %d call appearances, and you asked for CA %d!\n", CallApps, ca);
				return;
			}
			(void) sprintf(string, "%s%d", Linestring, ca);
			OutEnQueue(&PhoneOutQueue, string);
		} else {
			/* Get the next free CA */
			OutEnQueue(&PhoneOutQueue, FreeLinestring);
		}
	}
	else if ( strcmp( "volup", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, VolUPstring);
	}
	else if ( strcmp( "voldown", s ) == 0 )
	{
		OutEnQueue(&PhoneOutQueue, VolDOWNstring);
	}
	else if ( strncmp( "debug ", s, 6 ) == 0 )
	{
		int	d;
		char	*p;
#ifdef	DMD630
		char	Debug_char[10];
#endif

		/* If the 1st character is "+" or "-", then add it to the current
		 * debug level.
		 * Otherwise, set the debug level with the new value.
		*/
		p = &s[6];
		if (*p == '+' || *p == '-')
#ifdef	DMD630
			Debug += atoi(p);
		else
			Debug = atoi(p);
#else
			debug += atoi(p);
		else
			debug = atoi(p);
#endif	/* DMD630 */

#ifdef	DMD630
		/* Convert the new value to a string and send it to the host. */
		(void) sprintf(Debug_char, "%d", Debug);
		SendHost(PROC_PHONE, DEBUG_VALUE, Debug_char, EOM);
#endif	/* DMD630 */
	}
	else
		OutEnQueue(&PhoneOutQueue, s);
}
