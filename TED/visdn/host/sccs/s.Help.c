h17048
s 00119/00000/00000
d D 1.1 92/07/23 13:56:36 trl 1 0
c date and time created 92/07/23 13:56:36 by trl
e
u
U
t
T
I 1
#include "visdn.h"

typedef struct Help {
	struct Help *next;
	struct Help *parent;
	struct Help *sub;
	long offset;
} Help;

#define HELPFILE	"visdn.help"

Help *TOC = NULL;
Help *CurSection = NULL;
FILE *HelpFp = NULL;

extern char *getenv();

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
		fseek(HelpFp, 0L, 0);
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
	}
}

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
		sprintf(hbuf, ";%s;%d;\n", s, hp->offset);
	else
		sprintf(hbuf, "%d;%s;%d;\n", hp->parent->offset, s, hp->offset);

	SendCmd(TOCITEM, hbuf, 0);
}

HelpTopic(tag)
char *tag;
{
	register char *cp;
	char ibuf[BUFSIZ];
	char obuf[BUFSIZ];
	long offset;

	if ((offset = strtol(tag, NULL, 10)) <= 0)
		return;

	fseek(HelpFp, offset, 0);

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

		sprintf(obuf, ";%s;%s;\n", cp, tag);

		SendCmd(HELPLINE, obuf, 0);
	}
}
E 1
