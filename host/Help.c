#include <stdio.h>
#include "visdn.h"

typedef struct Help {
	struct Help *next;
	struct Help *parent;
	struct Help *sub;
	long offset;
} Help;

#define HELPFILE	"visdn.help"

static Help *TOC = NULL;
static Help *CurSection = NULL;
static FILE *HelpFp = NULL;

extern char *getenv();

static void
HelpSection(s)
char *s;
{
	register Help *hp;
	char hbuf[128];

	while (*s==' ' || *s=='\t')
		s++;
	s[strlen(s)-1] = '\0';


	hp = new(Help);
	hp->next = NULL;
	hp->sub = NULL;
	hp->parent = CurSection;
	if (CurSection==NULL)
	{
		hp->next = TOC;
		TOC = hp;
	} else
	{
		hp->next = CurSection->sub;
		CurSection->sub = hp;
	}

	CurSection = hp;

	hp->offset = ftell(HelpFp);

	if (hp->parent==NULL)
		(void) sprintf(hbuf, ";%s;%ld;\n", s, hp->offset);
	else
		(void) sprintf(hbuf, "%ld;%s;%ld;\n", hp->parent->offset, s, hp->offset);

	SendCmd(TOCITEM, hbuf, 0);
}

void
HelpTOC()
{
	register char *cp;
	char buf[BUFSIZ];

	(void) sprintf(buf, "%s/lib/visdn/%s",
			(cp = getenv("TOOLS")) ? cp : getenv("HOME"),
			HELPFILE);

	if (HelpFp==NULL)
		HelpFp = fopen(buf, "r");

	if (HelpFp)
	{
		(void) fseek(HelpFp, 0L, 0);
		while (fgets(buf, sizeof(buf), HelpFp))
		{
			for (cp=buf; *cp=='\t'; cp++)
				;
			if (*cp=='\n' || *cp=='#')
				continue;
			if (strncmp(cp, "SECTION", 7)==0)
			{
				HelpSection(cp+7);
			} else if (strncmp(cp, "ENDSECTION", 10)==0)
			{
				if (CurSection)
					CurSection = CurSection->parent;
			}
		}
		/* leave HelpFp open */
	} else {
		(void) fprintf(stderr, "Unable to open help file %s: ", buf);
		perror("");
		SendCmd(ERRORMSG, "No help available (file not found)\n", 0);
	}
}


void
HelpTopic(tag)
char *tag;
{
	register char *cp;
	char ibuf[BUFSIZ];
	char obuf[BUFSIZ];
	long offset, strtol();

	if ((offset = strtol(tag, NULL, 10)) <= 0)
		return;

	(void) fseek(HelpFp, offset, 0);

	while (fgets(ibuf, sizeof(ibuf), HelpFp))
	{
		for (cp=ibuf; *cp && *cp=='\t'; cp++)
			if (*cp=='\n')
				break;
		if (strncmp(cp, "ENDSECTION", 10)==0
		    || strncmp(cp, "SECTION", 7)==0)
			break;
		if (*cp=='\n')
			*cp = ' ';			/* blank line */
		else
			cp[strlen(cp)-1] = '\0';	/* zap newline */

		(void) sprintf(obuf, ";%s;%s;\n", cp, tag);

		SendCmd(HELPLINE, obuf, 0);
	}
}
