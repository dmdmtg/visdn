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

	if ((fp = fopen(pathname, "r")) != NULL)
	{
		while (fgets(buf, sizeof(buf), fp))
		{
			error = 0;
			rowcol = name = value = list = keypad = bind = NULL;
			flags[0] = '\0';
			tok = strtok(buf, "\t\n");
			if (tok==NULL || *tok=='\n' || *tok=='#')
				continue;
			/* commands which must be one per line */
			if (strncmp(tok, "areacode=", 9)==0)
			{
				(void) sprintf(cbuf, "%s;;\n", &tok[9]);
				SendCmd(DIALINFO, cbuf, 0);
				continue;
			} else if (strncmp(tok, "exchange=", 9)==0)
			{
				(void) sprintf(cbuf, ";%s;\n", &tok[9]);
				SendCmd(DIALINFO, cbuf, 0);
				continue;
			} else if (strncmp(tok, "pqinfo=", 7)==0)
			{
				(void) sprintf(cbuf, "%s", &tok[7]);
				(void) strtok(cbuf,";");
				InfoCacheData(cbuf,&tok[7]);
				continue;
			} else if (strncmp(tok, "music=", 6)==0) {
				(void) sprintf(cbuf, "%s\n", &tok[6]);
				SendCmd(TUNESTRING, cbuf, 0);
				continue;
			} else if (strncmp(tok, "escape=", 7)==0) {
				(void) sprintf(cbuf, "%s\n", &tok[7]);
				SendCmd(CAESCAPE, cbuf, 0);
				continue;
			} else if ((strncmp(tok, "tries=", 6)==0) ||
				(strncmp(tok, "connectseconds=", 15)==0) ||
				(strncmp(tok, "retryseconds=", 13)==0)) {
				(void) sprintf(cbuf, "%s\n", &tok[0]);
				SendCmd(DIALPARAM, cbuf, 0);
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
				} else if (list)
				{
					(void) sprintf(cbuf, "%s;%s;%s;\n",
							list,
							name?name:"",
							value?value:"");
					SendCmd(SPDCALL, cbuf, 0);
				} else if (rowcol)
				{
					if ((name && *name) &&
						!((value && *value) || (flags && *flags)))
					{
						(void) sprintf(cbuf, "%s;%s;\n", rowcol, name);
						SendCmd(FBLABEL, cbuf, 0);
					} else if (*rowcol)
					{
						(void) sprintf(cbuf, "%s;%s;%s;%s;%s;\n",
							rowcol,
							name?name:"",
							value?value:"",
							flags?flags:"",
							bind?bind:"");
						SendCmd(FBCREAT, cbuf, 0);
					}
				}
			}
		}

		(void) fclose(fp);
	}
}
