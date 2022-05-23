h40655
s 00336/00000/00000
d D 1.1 92/07/23 13:56:36 trl 1 0
c date and time created 92/07/23 13:56:36 by trl
e
u
U
t
T
I 1
#include "visdn.h"

typedef struct InfoItem {
	struct InfoItem *next;
	char *key;
	char *userinfo;
	char *facedata;
} InfoItem;

InfoItem *InfoItems = NULL;

extern InfoItem *InfoSearch();

#ifdef POSTQUERY
#include <string.h>

#define FORMAT "%pn;%org;%tl;%com;%loc;%room;%tel;%cornet;%ema;\n"

typedef char	*strings[512];
typedef char	buffer[BUFSIZ];

extern char	*pq_open();

InfoLookup(telno)
char	*telno;
{
	char	*attr = "tel";
	char	*mods = "";
	char	ofmt[128];
	strings	vars, vals, defs;
	buffer	prog, out;
	int	rv = 0;
	int	c, v, r;
	char	*pq;
	InfoItem *ip;

	if (*telno < '0' || *telno > '9')
		return;

	if ((ip = InfoSearch(telno)) != (InfoItem *)0)
	{
		SendCmd(USERINFO, ip->userinfo, 0);
		return 0;
	}

	/* ofmt gets scrambled, so re-initialize each time */
	sprintf(ofmt, "%s;%s", telno, FORMAT);

	strvec(mods, vals, " \t\n");
	if ((pq = pq_open(vals)) == 0)
		return pqerr(telno);

	fmtcomp(prog, ofmt, vars);
	for (v = 0; vars[v]; v++)
		;
	strvec(attr, defs, "/:");
	strvec(telno, vars+v, "/:");
	if (!dodef(vars+v, defs))
	{
		SendCmd(IGNORECMD, "dodef failed\n", 0);
		return 1;
	}
	c = 0;
	if ((r = pq_write(pq, vars)) != -1)
		if ((r = pq_read(pq, vals)) > 0) {
			fmtexec(out, prog, vals);
			SendCmd(USERINFO, out, 0);
			InfoCacheData(telno, out);
			c++;
		}
	if (r == -1)
		rv += pqerr(telno);
	if (c == 0)
	{
		sprintf(out, "%s;No call information;;;;;;;;;\n", telno);
		SendCmd(USERINFO, out, 0);
		rv++;
	}
	if (pq_close(pq) == -1)
		rv += pqerr(telno);

	return rv;
}

dodef(vars, defs)
strings	vars, defs;
{
	static buffer	buf;
	register char	*bp = buf;
	int	i, m, n;

	for (m = 0; defs[m]; m++);
	for (i = 0; vars[i]; i++)
		if (!strchr(vars[i], '='))
			if (i < m) {
				n = sprintf(bp, "%s=%s", defs[i], vars[i]);
				vars[i] = bp;
				bp += n + 1;
			} else {
				return 0;
			}
	return 1;
}

pqerr(key)
char *key;
{
	char buf[128];
	extern char pq_error[];

	sprintf(buf, "%s;ERROR: %s\n", key, pq_error);
	SendCmd(USERINFO, buf, 0);

	return 1;
}
#endif /* POSTQUERY */

InfoFace(buf)
char *buf;
{
	register char *cp;
	char *mach, *name, *key;
	InfoItem *ip;

	for (mach=name=cp=buf; *cp; *cp++)
		if (*cp=='!')
		{
			*cp++ = '\0';
			name = cp;
		} else if (*cp==';')
		{
			*cp++ = '\0';
			break;
		}
	if (name==mach)
		mach = "";
	for (key=cp; *cp != ';'; cp++)
		if (*cp=='\0')
			return;
	*cp++ = '\0';

	if ((ip = InfoSearch(key)) && ip->facedata)
	{
		SendCmd(FACEDATA, ip->facedata, ICONMSGSIZE);
	} else
		ReadFace(mach, name, key);
}

InfoItem *InfoSearch(key)
{
	register InfoItem *ip;

	for (ip=InfoItems; ip; ip=ip->next)
		if (strcmp(ip->key, key)==0)
			break;

	return ip;
}

InfoCacheData(key, userinfo)
char *key, *userinfo;
{
	register InfoItem *ip;
	extern char *String();

	ip = new(InfoItem);
	ip->key = String(key);
	ip->userinfo = String(userinfo);
	ip->facedata = (char *)0;

	ip->next = InfoItems;
	InfoItems = ip;
}

InfoCacheFace(key, facedata)
char *key, *facedata;
{
	register InfoItem *ip;
	register int i;
	register char *cp;

	if (ip = InfoSearch(key))
	{
		ip->facedata = Malloc(ICONMSGSIZE);
		for (i=0, cp=facedata; i < ICONMSGSIZE; cp++, i++)
			ip->facedata[i] = *cp;
	}
}

#include <sys/types.h>
#include <sys/stat.h>

#define LBUFSIZ	128

ReadFace(mach, name, key)
char *mach, *name, *key;
{
	FILE *icon;
	char iconbuf[ICONMSGSIZE];
	register int i;
	register char *cp;
	extern FILE *getifile();

	if ((icon = getifile(mach, name))==NULL)
		return;
	i = 0;
	for (cp=key; *cp && i < ICONKEYSIZE; cp++, i++)
		iconbuf[i] = *cp;
	while (i < ICONKEYSIZE)
		iconbuf[i++] = ' ';

	i = readface(icon, &iconbuf[ICONKEYSIZE], ICONBYTES)
		+ ICONKEYSIZE;

	fclose(icon);

	/* Fill in rest with pattern */
	while (i < ICONMSGSIZE)
		iconbuf[i++] = '\252';

	SendCmd(FACEDATA, iconbuf, ICONMSGSIZE);

	InfoCacheFace(key, iconbuf);
}

int readface(fp, buf, n)
FILE *fp;
char *buf;
int n;
{
	register int c;
	register int inhex, hinib, nbytes;
	register int nib, i;

	inhex = hinib = 0;
	nbytes = 0;
	i = 0;
	while (i < n)
	{
		switch(c = getc(fp))
		{
		case EOF:
			return i;
	 	case 'x':
		case 'X':
			hinib = inhex = 1;
			break;
		case '\n':
		case '\r':
			nbytes = 0;
			/* fall through */
		case ',':
			inhex = 0;
			break;
		default:
			if (inhex && nbytes < ICONSIZE/8)
			{
				if (hinib)
					nib = HexToDec(c) << 4;
				else
				{
					nib |= HexToDec(c);
					*buf++ = nib;
					nbytes++;
					i++;
				}

				hinib ^= 1;
			}
		}
	}

	return i;
}

HexToDec(Hex)
int Hex;
{
 	if (Hex >= '0' && Hex <= '9')
		return Hex - '0';
	if (Hex >= 'A' && Hex <= 'F')
		return 10 + Hex - 'A';
	if (Hex >= 'a' && Hex <= 'f')
		return 10 + Hex - 'a';
	return 0;
}

FILE *getifile(sysname, user)
char *sysname, *user;
{
	FILE *s, *findicon();
	extern char *IconPath, *LocalIconPath;

	if (LocalIconPath && *LocalIconPath) {
		if (s = findicon(LocalIconPath, sysname, user)) return s;
		if (s = findicon(LocalIconPath, "", user)) return s;
	}
	if (IconPath && *IconPath)
	{
		if (s = findicon(IconPath, sysname, user)) return s;
		if (s = findicon(IconPath, "", user)) return s;
	}
#ifdef notdef
	if (LocalIconPath && *LocalIconPath) {
		if (s = findicon(LocalIconPath, sysname, "unknown")) return s;
		if (s = findicon(LocalIconPath, "", "unknown")) return s;
	}
	if (IconPath && *IconPath)
	{
		if (s = findicon(IconPath, sysname, "unknown")) return s;
		if (s = findicon(IconPath, "", "unknown")) return s;
	}
#endif
	return NULL;
}

FILE *findicon(path, sysname, lname)
char *path, *sysname, *lname;
{
	FILE *f;
	static char iconfile[300];

	strcpy(iconfile, path);

	if (sysname && *sysname) {
		strcat(iconfile, "/");
		strcat(iconfile, sysname);
	}
	strcat(iconfile, "/");
	strcat(iconfile, lname);

	if ((f = fopen(iconfile, "r")) != NULL)
		return f;
	else
		return NULL;
}
E 1
