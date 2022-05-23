#include "visdn.h"

extern void InfoCacheData();

typedef struct OptFlag {
	char *optname;
	char *value;
} OptFlag;

static OptFlag Opts[] =
{
	"autosave",	"a",
	"box",		"b",
	"delete",	"d",
	"donothing",	"n",
	"left",		"<",
	"right",	">",
	"shade",	"s",
	NULL,		NULL,
};

static OptFlag BindOpts[] =
{
	"LoadButtons",	"B",
	"InfoWindow",	"I",
	"HelpWindow",	"H",
	"SnarfPost",	"P",
	"Exit",		"Q",
	"Quit",		"Q",
	"Refresh",	"R",
	"Redraw",	"R",
	"SaveQuery",	"S",
	NULL,		NULL,
};

static	int	sent;

void
ReadData(f)
char *f;
{
	register char *tok;
	char *list, *rowcol, *name, *value, *keypad, *bind;
	char flags[32];
	char pathname[256];
	char buf[BUFSIZ];
	char cbuf[BUFSIZ];
	FILE *fp;
	int error;
	OptFlag *op;
	extern char *getenv();

	pathname[0] = '\0';

	if (*f=='/')
		(void) strcpy(pathname, f);
	else
	{
		if (getenv("HOME")!=NULL)
		{
			(void) strcpy(pathname, getenv("HOME"));
			(void) strcat(pathname, "/");
		}
		(void) strcat(pathname, f);
	}

	if ((fp = fopen(pathname, "r")) == NULL)
	{
		/* We probably should give an error message ... */
		return;
	}

	/* If the file is big enough, we may end up sending a lot of
	 * data to the 730 before we return and check for input from
	 * the 730.  If the 730 is trying to send us data (e.g., debugging
	 * lines) and we don't read the data, eventually the internal buffers
	 * will become full and the layers session will hang.
	 * So, keep track of how many commands we send the 730, and every
	 * so often see if there's anything to read from the 730 (and if so,
	 * read it).
	 * I'm not sure how big the internal buffer is, so I don't know how
	 * long we can go before it fills up.  So, to be safe, check somewhat
	 * often.
	 * I tried fork()ing here and having the parent return so it
	 * could process incoming requests, but the input appeared to
	 * get intermixed with other input.
	*/
	sent = 0;	/* # commands sent without checking for input */

	while (fgets(buf, sizeof(buf), fp))
	{
		if (sent >= 20) {	/* is the a good number ???? */
/* XXX FIX ?? should we also send a "CONTINUE"-type command to the 730
 * and wait for a reply before continuing?  What if the 730 side died?
 * We won't find out until we're all done trying to send info to it.
 * This happens if we've got a large speedlist file, which may take a
 * minute or two to send.
*/
			/* See if there's any input from the 730.
			 * process_input() will return 0 if there is no input.
			 * Otherwise keep processing the input until there
			 * isn't any left (i.e., the input buffer is empty).
			*/
			while (process_input(0) != 0)	/* 0 == not main() */
				;
			sent = 0;
		}

		tok = strtok(buf, "\t\n");
		if (tok==NULL || *tok=='\n' || *tok=='#')
			continue;

		error = 0;
		rowcol = name = value = list = keypad = bind = NULL;
		flags[0] = '\0';

		/* commands which must be one per line */
		if (strncmp(tok, "areacode=", 9)==0)
		{
			(void) sprintf(cbuf, "%s;;\n", &tok[9]);
			SendCmd(DIALINFO, cbuf, 0);
			sent++;
			continue;
		} else if (strncmp(tok, "exchange=", 9)==0)
		{
			(void) sprintf(cbuf, ";%s;\n", &tok[9]);
			SendCmd(DIALINFO, cbuf, 0);
			sent++;
			continue;
		} else if (strncmp(tok, "pqinfo=", 7)==0)
		{
			/* This needs to be in the same format as returned
			 * by pq(1).  See "#define FORMAT" in Info.c.
			*/
			(void) sprintf(cbuf, "%s", &tok[7]);
			(void) strtok(cbuf,";");
			InfoCacheData(cbuf,&tok[7]);
			continue;
		} else if (strncmp(tok, "music=", 6)==0) {
			(void) sprintf(cbuf, "%s\n", &tok[6]);
			SendCmd(TUNESTRING, cbuf, 0);
			sent++;
			continue;
		} else if (strncmp(tok, "escape=", 7)==0) {
			(void) sprintf(cbuf, "%s\n", &tok[7]);
			SendCmd(CAESCAPE, cbuf, 0);
			sent++;
			continue;
		} else if ((strncmp(tok, "tries=", 6)==0) ||
			(strncmp(tok, "connectseconds=", 15)==0) ||
			(strncmp(tok, "retryseconds=", 13)==0)) {
			(void) sprintf(cbuf, "%s\n", &tok[0]);
			SendCmd(DIALPARAM, cbuf, 0);
			sent++;
		}

		/* commands that are grouped per line */
		while (error==0 && tok != NULL)
		{
			if (strncmp(tok, "button=", 7)==0)
			{
				if (list)
					error = 1;
				else
					rowcol = &tok[7];
			} else if (strncmp(tok, "keypad=", 7)==0)
			{
				keypad = &tok[7];
			} else if (strncmp(tok, "speedlist=", 10)==0)
			{
				if (rowcol)
					error = 1;
				else
					list = &tok[10];
			} else if (strncmp(tok, "list=", 5)==0)
			{
				if (rowcol)
					error = 1;
				else
					list = &tok[5];
			} else if (strncmp(tok, "name=", 5)==0)
				name = &tok[5];
			else if (strncmp(tok, "value=", 6)==0)
				value = &tok[6];
			else if (strncmp(tok, "include=", 8)==0)
				ReadData(&tok[8]);
			else if (strncmp(tok, "bind=", 5)==0)
			{
				for (op=BindOpts; op->optname; op++)
				{
					if (strcmp(&tok[5], op->optname)==0)
					{
						bind = op->value;
						break;
					}
				}
			} else if (strncmp(tok, "lamp=", 5)==0)
			{
				if (tok[5]=='\0')
					error = 1;
				else
				{
					(void) strcat(flags, "L");
					(void) strcat(flags, &tok[5]);
				}
			} else
			{
				for (op=Opts; op->optname; op++)
					if (strcmp(tok, op->optname)==0)
					{
						(void) strcat(flags, op->value);
						break;
					}

				if (op==(OptFlag *)0)
					error = 1; /* silent syntax error */
			}

			tok = strtok(NULL, "\t\n");
		}

		if (!error)
		{
			if (keypad)
			{
				if (rowcol || list || name || value || bind || *flags)
					continue;
				(void) sprintf(cbuf, "%s\n", keypad);
				SendCmd(KEYPADPOS, cbuf, 0);
				sent++;
			} else if (list)
			{
				(void) sprintf(cbuf, "%s;%s;%s;\n",
						list,
						name?name:"",
						value?value:"");
				SendCmd(SPDCALL, cbuf, 0);
				sent++;
			} else if (rowcol)
			{
				if ((name && *name) &&
					!((value && *value) || (flags && *flags)))
				{
					(void) sprintf(cbuf, "%s;%s;\n", rowcol, name);
					SendCmd(FBLABEL, cbuf, 0);
					sent++;
				} else if (*rowcol)
				{
					(void) sprintf(cbuf, "%s;%s;%s;%s;%s;\n",
						rowcol,
						name?name:"",
						value?value:"",
						flags?flags:"",
						bind?bind:"");
					SendCmd(FBCREAT, cbuf, 0);
					sent++;
				}
			}
		}
	}

	(void) fclose(fp);
}
