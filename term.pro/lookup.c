/* The X and DMD versions of this file are the same since the guts of the
 * phone logic are in here, and it's easier to maintain one file.
 * As such, you'll see lots of "#ifdef DMD630" code.
 *
 * Eric Claeys, 1/95
*/

#ifdef	DMD630
#include "visdn.h"

#else

#include "xisdn.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <malloc.h>

extern	DialPlanMod	DialPlan;

#endif	/* DMD630 */

struct	number_info {
	char	*number;	/* the phone number as it appears in the file */
	char	*cnumber;	/* the converted phone number */
	char	*list;		/* speedcall list the number is on */
	char	*name;		/* speedcall name for the number */
};
#define	MAX_NUMBERS	500	/* is this enough? */
static	struct	number_info	numbers[MAX_NUMBERS];
static	int	num_numbers = 0;

#ifndef	DMD630

/* Convert the given number to what ISDN will convert it to. */
static	char *
convert_number(number)
char	*number;
{
	static	char	converted_number[50];	/* the converted number */
	char	temp[50], *n, *t;
	int	length, i;

	*converted_number = '\0';

	/* Ignore special chars since ISDN never sees them and hence
	 * they won't be displayed on the LCD.
	 * Also ignore leading "CA" and the escape sequence if found.
	*/
	n = number;
	t = temp;
	if (*n == 'C' && *(n+1) == 'A') {
		n += 2;		/* skip "CA" */
	}
	for (; *n != '\0'; ) {
		/* First check for escape sequence before checking other chars,
		 * as the escape sequence may contain other "ignore" chars.
		*/
		if (strncmp(n, DialPlan.prefix, strlen(DialPlan.prefix)) == 0) {
			n += strlen(DialPlan.prefix);
			continue;
		}
		switch (*n) {
		case ';':	/* command separator */
		case '>':	/* wait for connect */
			/* This is usually used to interact with services.
			 * Since the ">" and everything after
			 * it isn't part of the phone number, ignore them.
			 * If the 1st char of the line is a ">",
			 * then it redials the last phone number.
			*/
			*n = '\0';
			break;
		case '+':	/* wait for secondary dial tone */
		case '-':	/* separates exch from extension */
		case ',':	/* pause 1/2 second */
		case '~':	/* pause 1/2 second */
		case '(':	/* used in area codes */
		case ')':	/* used in area codes */
			n++;	/* skip these */
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			*t++ = *n++;	/* use these */
			break;
		default:	/* skip anything else */
			n++;
			break;
		}
	}
	*t = '\0';

	/* temp[] should now hold a stripped version of the number.
	 * Expand it if necessary.
	*/
	length = strlen(temp);
	switch (length) {
	case 5:
		/* Assume the 1st digit might be an exchange.
		 * If so, replace it with the (probably longer) full exchange.
		*/
		{ register Exchange *xp;
		  char e[2];

		e[0] = temp[0];
		e[1] = '\0';
		for (xp=DialPlan.exchanges; xp; xp=xp->next) {
			if (strcmp(xp->mapto, e) == 0) {
				(void)sprintf(converted_number, "%s%s", xp->exch, &temp[1]);
				break;
			}
		}
		if ( ! xp) {
			/* I guess it wasn't an exchange... */
			(void) strcpy(converted_number, temp);
		}
		}
		break;
	case 7:		/* Fine as is */
		(void) strcpy(converted_number, temp);
		break;
	case 10:
		/* The number includes the area code; if it's the local
		 * areacode, delete it.
		*/
		if (strncmp(temp, DialPlan.areacode, strlen(DialPlan.areacode)) == 0)
			(void) strcpy(converted_number, &temp[strlen(DialPlan.areacode)]);
		else
			(void) strcpy(converted_number, temp);
		break;
	case 11:
		/* The number includes "1" plus the area code.
		 * Delete the "1".
		*/
		(void) strcpy(converted_number, &temp[1]);
		break;
	case 0:
		DPRINTF("convert_number(%s): no number given.\n", number);
		return((char *)NULL);
	default:
		DPRINTF("convert_number(%s): bad number (%d digits).\n", number, length);
		return((char *)NULL);
	}

	return(converted_number);
}
#endif	/* DMD630 */

/* Process a speedlist entry.
 * Since the Xtent program handles the speedlist menu,
 * we just need to convert all the phone numbers to a format that we
 * can later compare to when we make or receive a call.
 *
 * The format of a line is "token1=_value1_ \t token2=_value2_ ...".
 * The format of the line is:
 *	_speedlist_ \t name=_name_ \t value=_value_ [ \t ...]
 * The "speedlist" value will be the 1st one; the others may be
 * in any order.
 * The tokens we're interested in are "speedlist", "name", and "value".
 *
 * XXXXX We should also accept "post=_pq_name_" instead of "value",
 *	 which requires a post(1) lookup.
*/
void
speedlist(line)
char	*line;
{
#ifdef	DMD630
	return;
#else
	char	*l, *p, *cn;
	struct	number_info *num;

/* FIX: XXX This really should be dynamic via malloc() */
	if (++num_numbers > MAX_NUMBERS) {
		static	int	printed = 0;
		if (printed++ == 0)
			(void) fprintf(stderr, "Too many speedlist numbers; max is %d.\n", MAX_NUMBERS);
		num_numbers--;
		return;
	}

	/* Since our structure contains char pointers into "line",
	 * we need to first make a copy of "line" and use it.
	*/
	if ((l = (char *) malloc(strlen(line)+1)) == NULL) {
		perror("malloc l in speedlist()");
		return;
	}
	p = l;	/* save "l" in case we need to free() it */
	(void) strcpy(p, line);

	num = &numbers[num_numbers-1];
	num->list = p;

	/* Find the end of the speedlist. */
	p = strchr(p, '\t');
	if (p == NULL) {
		/* Only a speedlist was given, so there is
		 * no number to compare, so ignore this line.
		*/
		num_numbers--;
		free(l);
		return;
	}
	*p++ = '\0';	/* ends the speedlist */

	/* Skip any white space after the speedlist. */
	while (*p == '\t' || *p == ' ')
		p++;

	/* Now search for the "name" and "value" tokens. */
	for (;;) {
		if (strncmp(p, "name=", 5) == 0) {
			p += 5;		/* start of the value */
			num->name = p;
		} else if (strncmp(p, "value=", 6) == 0) {
			p += 6;		/* start of the value */
			num->number = p;
		} /* else skip the token */

		/* Move to the start of the next token, if present. */
		while (*p != '\t' && *p != '\0')
			p++;
		if (*p == '\0') {
			break;
		}
		*p++ = '\0';
	}

	if (num->number[0] == '\0') {
		return;
	}

	/* There was a number, so convert it. */
	cn = convert_number(num->number);
	if (cn == (char *)NULL) {
		/* bad number, so ignore it */
		num_numbers--;
		free(l);
		return;
	}
	if ((num->cnumber = (char *) malloc(strlen(cn)+1)) == NULL) {
		perror("malloc cn in speedlist()");
		return;
	}
	(void) strcpy(num->cnumber, cn);

/*
	DPRINTF("list=='%s', name='%s', number='%s', cn='%s'\n",
		num->list,num->name,num->number,num->cnumber);
*/

	return;
#endif	/* DMD630 */
}

/* Given a phone number, look it up and return its information, if found. */
char *
lookup_number(number)
char	*number;
{
	register int	i;
	register struct number_info *ni;
	register char	*cn;

#ifndef	DMD630	/* XXXX FIX: NEED TO IMPLEMENT THIS FOR DMD630 ALSO */

	if ((cn = convert_number(number)) == (char *)NULL) {
		return((char *)NULL);
	}
	for (i=0, ni=numbers; i<num_numbers; i++, ni++) {
		if (strcmp(cn, ni->cnumber) == 0) {
			return(ni->name);
		}
	}
#endif	/* DMD630 */
	return((char *)NULL);
}
