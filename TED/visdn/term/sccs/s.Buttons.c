h16769
s 00325/00000/00000
d D 1.1 92/07/23 13:57:27 trl 1 0
c date and time created 92/07/23 13:57:27 by trl
e
u
U
t
T
I 1
#include "visdn.h"

FBut *ButParse();

extern int ButtonPress();

int FBColOff[MAXFBCOL] =
{
	0*(FB_LEN+FB_GAP),
	1*(FB_LEN+FB_GAP),
	2*(FB_LEN+FB_GAP),
	3*(FB_LEN+FB_GAP),
	4*(FB_LEN+FB_GAP),
	5*(FB_LEN+FB_GAP),
	6*(FB_LEN+FB_GAP),
	7*(FB_LEN+FB_GAP),
	8*(FB_LEN+FB_GAP),
};

/* the extra row is for KBrects anchored to the bottom line */
int FBRowOff[MAXFBROW+1] =
{
	0*(FB_WID+FB_OFF),
	1*(FB_WID+FB_OFF),
	2*(FB_WID+FB_OFF),
	3*(FB_WID+FB_OFF),
	4*(FB_WID+FB_OFF),
	5*(FB_WID+FB_OFF),
	6*(FB_WID+FB_OFF),
	7*(FB_WID+FB_OFF),
	8*(FB_WID+FB_OFF),
	9*(FB_WID+FB_OFF),
	10*(FB_WID+FB_OFF),
	11*(FB_WID+FB_OFF),
	12*(FB_WID+FB_OFF),
	13*(FB_WID+FB_OFF),
	14*(FB_WID+FB_OFF),
};

int KPColOff[MAXKPCOL] =
{
	0*(KP_LEN+KP_GAP),
	1*(KP_LEN+KP_GAP),
	2*(KP_LEN+KP_GAP)
};

int KPRowOff[MAXKPROW] =
{
	0*(KP_WID+KP_GAP),
	1*(KP_WID+KP_GAP),
	2*(KP_WID+KP_GAP),
	3*(KP_WID+KP_GAP)
};

FeatBut *ButAdd();

extern int ActionLoadButs();
extern int InfoWindow();
extern int HelpWindow();
extern int ActionSnarfPost();
extern int ActionPhoneExit();
extern void ReDisplay();
extern int ActionSnarfQuery();

FeatBut *AddFeatBut(s)
char *s;
{
	return ButAdd(FB_OFF, 2*FB_OFF+LCD_WID,
			&FeatButs, s, FBRowOff, FBColOff,
			MAXFBROW, MAXFBCOL, FB_LEN, FB_WID);
}

FeatBut *AddKeyBut(s)
char *s;
{
	return ButAdd(KeyPadX, KeyPadY,
			&KeyPad, s, KPRowOff, KPColOff,
			MAXKPROW, MAXKPCOL, KP_LEN, KP_WID);
}

FeatBut *ButAdd(xoff, yoff, listp, s, rowvec, colvec, maxrow, maxcol, len, wid)
int xoff, yoff;
FeatBut **listp;
char *s;
int rowvec[], colvec[];
int maxrow, maxcol;
int len, wid;
{
	FeatBut *fp;
	FBut *fbp;
	FeatBut *LookupBut();

	xoff += Ox;
	yoff += Oy;

	if ((fbp = ButParse(s)) != 0
	    && fbp->row>0 && fbp->row<=maxrow
	    && fbp->col>0 && fbp->col<=maxcol)
	{
		if (fp = LookupBut(*listp, fbp->row, fbp->col, fbp->flags))
		{
			if (fp->flags&DO_NOCHANGE)
				return 0;
			if (fbp->flags&DO_DELETE)
			{
				ButKiss(fp);
				return 0;
			}
			free(fp->name);
			free(fp->arg);
			if (fp->lamp)
			{
				LAMP.Delete(fp->lamp);
				free(fp->lamp);
			}
		} else
		{
			if (fbp->flags&DO_DELETE)
				return 0;
			if (fp = new(FeatBut))
			{
				fp->next = *listp;
				*listp = fp;
			}
		}

		if (fp != 0)
		{
			fp->flags = fbp->flags;
			fp->name = String(fbp->label);
			fp->arg = String(fbp->value);
			if ((fp->row = fbp->row) > MAXFBROW)
				fp->row = MAXFBROW;
			if (fp->row > MaxFBRow)
			{
				NeedReSize = 1;
				MaxFBRow = fp->row;
			}
			if ((fp->col = fbp->col) > MAXFBCOL)
				fp->col = MAXFBCOL;
			if (fp->col > MaxFBCol)
			{
				NeedReSize = 1;
				MaxFBCol = fp->col;
			}
			if (fp->flags&DO_RIGHT)
				fp->rect.origin.x = xoff+colvec[fbp->col-1]+len/2;
			else
				fp->rect.origin.x = xoff+colvec[fbp->col-1];
			if (fp->flags&(DO_LEFT|DO_RIGHT))
				fp->rect.corner.x = fp->rect.origin.x + len/2;
			else
				fp->rect.corner.x = fp->rect.origin.x + len;
			fp->rect.origin.y = yoff+rowvec[fbp->row-1];
			fp->rect.corner.y = fp->rect.origin.y + wid;
			if (fp->flags&DO_LAMP)
			{
				fp->lamp = LAMP.New(fbp->lampno, fp);
			} else
				fp->lamp = (ButLamp *)0;
			if ((fp->flags&DO_NOTHING)==0)
				fp->action = ButtonPress;
			fp->bound = fbp->bind;
		}

		return fp;
	}

	return (FeatBut *)0;
}

FeatBut *ButLabel(s)
register char *s;
{
	int row, col;
	register char *cp;
	char label[FBLABELSIZ];
	register FeatBut *fp;
	FeatBut *LookupBut();

	for (row=0; *s>='0' && *s<='9'; s++)
		row = row*10 + *s-'0';
	if (*s++ != ',') return 0;
	for (col=0; *s>='0' && *s<='9'; s++)
		col = col*10 + *s-'0';
	if (*s++ != ';') return 0;
	for (cp=label; cp < &label[sizeof(label)-1] && *s && *s != ';'; )
		*cp++ = *s++;
	*cp = '\0';
	if (*s++ != ';') return 0;

	if (fp = LookupBut(FeatButs, row, col, 0))
	{
		free(fp->name);
		fp->name = String(label);
	}

	return fp;
}

FBut *ButParse(s)
register char *s;
{
	register FBut *fbp = &tmpFBut;
	register int n;
	register char *cp;

	for (n=0; *s>='0' && *s<='9'; s++)
		n = n*10 + *s-'0';
	fbp->row = n;
	if (*s++ != ',') return 0;
	for (n=0; *s>='0' && *s<='9'; s++)
		n = n*10 + *s-'0';
	fbp->col = n;
	if (*s++ != ';') return 0;
	for (cp=fbp->label; cp < &fbp->label[sizeof(fbp->label)-1] && *s && *s != ';'; )
		*cp++ = *s++;
	*cp = '\0';
	if (*s++ != ';') return 0;
	for (cp=fbp->value; cp < &fbp->value[sizeof(fbp->value)-1] && *s && *s != ';'; )
		*cp++ = *s++;
	*cp = '\0';
	if (*s++ != ';') return 0;
	for (fbp->flags=0; *s && *s != ';'; s++)
	{
		switch (*s)
		{
		case 'a': fbp->flags |= DO_AUTOSAVE; break;
		case 'b': fbp->flags |= DO_BOX; break;
		case 'd': fbp->flags |= DO_DELETE; break;
		case 's': fbp->flags |= DO_SHADE; break;
		case '<': fbp->flags |= DO_LEFT; break;
		case '>': fbp->flags |= DO_RIGHT; break;
		case 'n': fbp->flags |= DO_NOTHING; break;
		case 'x': fbp->flags |= DO_NOCHANGE; break;
		case 'L':
			s++;
			fbp->flags |= DO_LAMP;
			if (*s=='m' || *s=='M')
				fbp->lampno = MSGLAMPNO;
			else if (*s=='s' || *s=='S')
				fbp->lampno = SPKRLAMPNO;
			else if (*s=='u' || *s=='U')
				fbp->lampno = MUTELAMPNO;
			else if (*s=='x' || *s=='X')
				fbp->lampno = DATALAMPNO;
			else if (*s=='t' || *s=='T')
				fbp->lampno = SHIFTLAMPNO;
			else if (s[0] && s[1] && s[0] != ';' && s[1] != ';')
			{
				fbp->lampno = HexNibble(s);
				s++;
			} else
				fbp->lampno = 0;
			break;
		}
	}
	if (*s++ != ';') return 0;
	switch (*s)
	{
	case 'B': fbp->bind = ActionLoadButs; break;
	case 'I': fbp->bind = InfoWindow; break;
	case 'H': fbp->bind = HelpWindow; break;
	case 'P': fbp->bind = ActionSnarfPost; break;
	case 'Q': fbp->bind = ActionPhoneExit; break;
	case 'R': fbp->bind = (int (*)())ReDisplay; break;
	case 'S': fbp->bind = ActionSnarfQuery; break;
	case ';': fbp->bind = (int (*)())NULL; break;
	default:  return 0;
	}

	return fbp;
}

FeatBut *LookupBut(fp, row, col, f)
register FeatBut *fp;
register int row, col;
register int f;
{

	f &= (DO_LEFT|DO_RIGHT);
	for (; fp; fp=fp->next)
		if (fp->row==row && fp->col==col
		    && f==(fp->flags&(DO_LEFT|DO_RIGHT)))
			break;

	return fp;
}

ButKiss(fp)
register FeatBut *fp;
{
	register FeatBut *ofp, *nfp;

	if (fp==FeatButs)
		FeatButs = fp->next;
	else
		for (ofp=FeatButs, nfp=FeatButs->next; nfp; ofp=nfp, nfp=nfp->next)
			if (nfp==fp)
			{
				ofp->next = fp->next;
				break;
			}

	/* find out if we need to shrink the window? */

	(void) rectf(&display, fp->rect, F_CLR);

	/* free memory */
	free(fp->name);
	free(fp->arg);
	free(fp);
}

int ButtonPress(fp)
FeatBut *fp;
{
	if (fp->bound)
		(*fp->bound)();

	if (fp->arg==(char *)0 || *fp->arg=='\0')
		return 0;
	else
		return OutEnQueue(&PhoneOutQueue, fp->arg);
}
E 1
