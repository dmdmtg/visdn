h21837
s 00014/00003/00743
d D 1.2 92/09/04 12:02:47 trl 2 1
c gregg changes for postcall
e
s 00746/00000/00000
d D 1.1 92/07/23 13:58:31 trl 1 0
c date and time created 92/07/23 13:58:31 by trl
e
u
U
t
T
I 1
#include "visdn.h"
#include <dmdproc.h>
#include <label.h>
#include <730windows.h>
#include "reggie.i"

static Texture16 rusure = {
  0X0000, 0X0EA0, 0X0AA0, 0X08A0,
  0X08A0, 0X08E0, 0X0000, 0X0000,
  0XEAEE, 0X8AAA, 0XEA8E, 0X2A88,
  0XEE8E, 0X0000, 0X0000, 0X0000,
};

extern int SnarfQuery();
extern int HelpWindow();
extern int InfoWindow();
extern void ReDisplay();
extern char* regex();

extern Proc *phoneproc, *infoproc, *helpproc;

void DoMouse()
{
	register Point mp;
	register FeatBut *fp;
	register KBrect *kp;

	if (button3())
	{
		MainItem *it;

		/* remember which button was pressed */
		SpeedListBut = BUTTON3;
		if (it = (MainItem *)tmenuhit(&MainMenu, 3, TM_EXPAND))
		{
			if (it->action)
				(*it->action)(it);
		}
		SpeedListBut = 0;
	} else if (button2())
	{
		SpeedItem *it;

		/* remember which button was pressed */
		SpeedListBut = BUTTON2;
		if (it = (SpeedItem *)tmenuhit(&SpeedLists->tmenu, 2, TM_EXPAND))
		{
			OutEnQueue(&PhoneOutQueue, it->value);
		}
		SpeedListBut = 0;
	} else if (button1())
	{
		mp = mouse.xy;

		for (fp=FeatButs; fp; fp=fp->next)
			if (ptinrect(mp, fp->rect) && fp->action)
			{
				if( FollowRect( fp->rect ) )
					(*fp->action)(fp);
				return;
			}
		for (fp=KeyPad; fp; fp=fp->next)
			if (ptinrect(mp, fp->rect))
			{
				if( FollowRect( fp->rect ) )
				{
					if (fp->action)
						(*fp->action)(fp);
				}
				return;
			}

		/* pointing at a typing area? */
		for (kp=KBrects; kp; kp=kp->next)
			if (ptinrect(mp, kp->rect))
			{
				if (kp != KBcurrent)
				{
					BoldBox(KBcurrent->rect, F_CLR);
					BoldBox(kp->rect, F_STORE);
					KBcurrent = kp;
				}
			}
	}
}


FollowRect( r )
    Rectangle r;
{
    int x;

    r = inset( r, -1 );
    lrectf( P->layer, r, F_XOR );
    x = 0;
    while( bttn(1) )
    {
        if( bttn23() )
        {
            if( x == 0 ) lrectf( P->layer, r, F_XOR );
            return( 0 );
        }
        if( !x && !ptinrect( RealMouse.xy, r ) )
        {
            lrectf( P->layer, r, F_XOR );
            x = 1;
        }
        else if( x && ptinrect( RealMouse.xy, r ) )
        {
            lrectf( P->layer, r, F_XOR );
            x = 0;
        }
    }

    if( x == 0 ) lrectf( P->layer, r, F_XOR );
    return( !x && ptinrect( RealMouse.xy, r ) );
}

SpeedItem *SpeedListGen(i, m)
int i;
Tmenu *m;
{
	register SpeedList *lp;
	register SpeedEntry *ep;
	register SpeedItem *it;

	it = &tmpSpeedItem;
	it->text = NULL;
	it->ufield.uval = 0;

	for (lp=SpeedLists; lp; lp=lp->next)
		if (m == &(lp->tmenu))
			break;
	if (lp != (SpeedList *)0)
	{
		if (SpeedListBut & BUTTON2)
		{
			/* button 2 -- count everything */
			for (ep=lp->first; ep && i > 0; ep=ep->next, i--)
				;
		} else
		{
			/* button 3 -- only count existing lists */
			for (ep=lp->first; ep; ep=ep->next)
			{
				if (ep->list != (SpeedList *)0)
					i--;
				if (i < 0)
					break;
			}
		}
		if (ep != (SpeedEntry *)0)
		{
			it->text = ep->text;
			/* if item has a list, insert its tmenu here */
			it->next = (Tmenu *)0;
			if (!(SpeedListBut & BUTTON2))
			{
				register SpeedEntry *tep;

				/* check to see if there are any submenus */
				if (ep->list)
				{
					for (tep=ep->list->first; tep; tep=tep->next)
						if (tep->list)
						{
							/* yes -- show it */
							it->next = &(ep->list->tmenu);
							break;
						}
				}
			} else if (ep->list)
			{
				it->next = &(ep->list->tmenu);
			}
			it->value = ep->value;
			it->action = SnarfQuery;
			it->entry = ep;
		}
	}

	return it;
}

/*
 * AddSpeedCall -- returns 1 if a new item was actually added,
 *		   returns 0 on failure or duplicate entry
 */
AddSpeedCall(s)
register char *s;
{
	register SpeedEntry *ep;
	register char *cp;
	SpeedList *lp;
	SpeedEntry *pep;
	char *name, *value, *list;
	char listcomp[80];
	extern char *LJString();

	if (s==NULL || *s=='\0')
		return 0;
	for (list=s; *s && *s != ';'; s++)
		;
	if (*s=='\0') return 0; else *s++ = '\0';
	for (name=s; *s && *s != ';'; s++)
		;
	if (*s=='\0') return 0; else *s++ = '\0';
	for (value=s; *s && *s != ';'; s++)
		;
	*s = '\0';
	MPRINTF("AddSpeed '%s' '%s' '%s'\n", list, name, value);
	if (*name=='\0')
		return 0;	/* no name -- ignore it */
	/*
	 * find list and entry for this item, creating as we go
	 * if list is null, this loop never executes, which is ok
	 */
	for (lp=SpeedLists, pep=(SpeedEntry *)0; *list; )
	{
		/* loop through all entries in this list */
		while (*list=='/')
			list++;
		/* copy next component, including escaped slashes */
		for (cp=listcomp; *list && cp < &listcomp[sizeof(listcomp)]; )
		{
			if (*list=='\\' && list[1]=='/')
				list++;
			else if (*list=='/')
			{
				list++;
				break;
			}
			*cp++ = *list++;
		}
		/* component is too large -- bail out */
		if (cp == &listcomp[sizeof(listcomp)])
			return 0;
		*cp = '\0';
		/* trailing '/' is ignored here */
		if (*listcomp=='\0')
			break;
		for (ep=lp->first; ep; ep=ep->next)
		{
			/* find matching list component */
			if (LJstrcmp(listcomp, ep->text)==0)
				break;
		}
		if (ep==(SpeedEntry *)0)
		{
			/* list component not found -- create it */
			if ((ep = lp->first)==(SpeedEntry *)0)
			{
				lp->first = new(SpeedEntry);
				ep = lp->first;
			} else
			{
				while (ep->next)
					ep = ep->next;
				ep->next = new(SpeedEntry);
				ep = ep->next;
			}
			ep->next = (SpeedEntry *)0;
			ep->list = (SpeedList *)0;
			ep->parent = pep;
			ep->value = NULL;
			ep->text = LJString(listcomp);
		}
		if ((lp = ep->list)==(SpeedList *)0)
		{
			/* no list -- create it */
			lp = ep->list = new(SpeedList);
			lp->first = (SpeedEntry *)0;
			lp->tmenu.item = (Titem *)0;
			lp->tmenu.generator = (Titem *(*)())SpeedListGen;
			lp->tmenu.menumap = SPEEDMAP;
			lp->next = SpeedLists->next;
			SpeedLists->next = lp;
		}
		pep = ep;
	}
	/*
	 * At this point, we have
	 *	lp refers to the list the name goes on
	 *	pep refers to the parent SpeedEntry
	 *
	 * Now we find if the name is already on the list
	 */
	for (ep=lp->first; ep; ep=ep->next)
		if (LJstrcmp(ep->text, name)==0)
			break;
	if (ep==(SpeedEntry *)0)
	{
		SpeedEntry *tep;

		/* new member of list */
		ep = new(SpeedEntry);
		ep->next = (SpeedEntry *)0;
		ep->list = (SpeedList *)0;
		ep->parent = pep;
		ep->text = LJString(name);
		ep->value = String(value);
		/* append to end of list */
		if (lp->first==(SpeedEntry *)0)
			lp->first = ep;
		else
		{
			for (tep=lp->first; tep->next; tep=tep->next)
				;
			tep->next = ep;
		}
	} else
	{
		/* duplicate name -- see if value has changed */
		if (strcmp(ep->value, value) != 0)
		{
			free(ep->value);
			ep->value = String(value);
		} else
			return 0;
	}

	/* if we get here, something changed somewhere */
	return 1;
}

ActionPhoneExit()
{
	if (RUsure())
		PhoneExit();
}

RUsure()
{
	int ret;
	Texture16 *savecursor;

	ret = 0;
	savecursor = cursswitch(&rusure);
	while (!bttn123())
		nap(2);
	if (bttn3())
		ret = 1;
	while (bttn123())
		nap(2);
	cursswitch(savecursor);
	return (ret);
}

ActionLoadButs()
{
	register TermProc *p;

	/*
	 * find a free send buffer and borrow it
	 */
	for (p=TermProcs; p < &TermProcs[TERM_PROCS]; p++)
		if ((p->event & EVENT_SENDBUF)==0)
		{
			p->sndbuf[0] = LOADREQUEST;
			p->sndbuf[1] = '\0';
			p->event |= EVENT_SENDBUF;
			break;
		}

	/* if all send bufs are in use, request is ignored */
}

MainItem *MainMenuGen(i, m)
int i;
Tmenu *m;
{
	register MainItem *it;
	extern int SnarfPost();

	it = &tmpMainItem;

	it->icon = 0;
	it->ufield.uval = 0;
	it->ufield.grey = 0;
	it->next = (Tmenu *)0;
	it->action = 0;

	switch (i)
	{
	case 0:
		it->text = "load buttons";
		it->action = ActionLoadButs;
		break;
	case 1:
		it->text = "options";
		it->next = &OptionMenu;
		break;
	case 2:
		it->text = "save query";
		if (DispMode != M_QUERY || SnarfQuery((MainItem *)0)==0)
			it->ufield.grey = 1;
		else
		{
			if (bttn2())
				SpeedListBut |= BUTTON2;
			it->next = &SpeedLists->tmenu;
		}
		break;
	case 3:
		it->text = "snarf post";
		if (SnarfPost((MainItem *)0)==0)
			it->ufield.grey = 1;
		else
			it->action = SnarfPost;
		break;
	case 4:
		it->text = "call info";
		it->action = InfoWindow;
		break;
	case 5:
		it->text = "help";
		it->action = HelpWindow;
		break;
	case 6:
		it->text = "refresh";
		it->action = (int (*)())ReDisplay;
		break;
	case 7:
		it->text = "exit";
		it->action = ActionPhoneExit;
		break;
#ifdef	DEBUG
	case 8:	/* must be the last one! */
		it->text = "debug";
		it->next = &DebugMenu;
		break;
#endif
	default:
		it->text = (char *)0;
		break;
	}

	return it;
}

MainItem *OptionItemGen(i, m)
int i;
Tmenu *m;
{
	register MainItem *it;
	extern int ToggleOption();

	it = &tmpMainItem;

	it->ufield.uval = 0;
	it->ufield.grey = 0;
	it->next = (Tmenu *)0;
	it->action = ToggleOption;

	if (i>=MAXOPTION)
		it->text = (char *)0;
	else
	{
		it->text = Options[i].text;
		it->flag = Options[i].flag;
		it->icon = *it->flag ? &B_checkmark : (Bitmap *)0;
	}

	return it;
}

ToggleOption(it)
register MainItem *it;
{
	*it->flag ^= 1;
}

int ActionSnarfQuery()
{
	MainItem itmp;

	itmp.text = "Query History";
	itmp.ufield.uval = 1;
	SnarfQuery(&itmp);
}

SnarfQuery(it)
register SpeedItem *it;
{
	register char *cp, *tp;
	register SpeedEntry *ep;
	char tbuf[128];
	char sbuf[128];

	if (it)
	{
		/*
		 * uval indicates whether this is from a button menu selection
		 * or from some other "hook" in the code (e.g. AUTOSAVE on a
		 * query)
		 */
		if (it->ufield.uval==0)
		{
			/* this is a *REAL* speed call item */
			/* find path name within speed call menu */
			/* copy from back to front -- avoids recursion */
			cp = &sbuf[sizeof(sbuf)-1];
			*cp = '\0';
			for (ep=it->entry; ep && cp != sbuf; ep=ep->parent)
			{
				/* find end of text */
				for (tp=ep->text; *tp; tp++)
					;
				/* copy backwards */
				while (tp != ep->text && cp != sbuf)
				{
					/* skip the left justification garbage */
					if (*--tp==LJCHAR && tp != ep->text)
						--tp;
					if ((*--cp = *tp)=='/' && cp != sbuf)
						*--cp = '\\';
				}
				/* insert delimiter if there is another segment */
				if (ep->parent && cp != sbuf)
					*--cp = '/';
			}
			/* if we bumped into the bottom of the buffer, bail out */
			if (cp==sbuf)
				return 0;
		} else
		{
			/* not from a real menu item -- use the item name */
			cp = it->text;
		}
		/* now copy to real buffer */
		for (tp=tbuf; *cp; )
			*tp++ = *cp++;
		*tp = '\0';
		MPRINTF("SnarfQuery %s %s\n", it->text, tbuf);
	} else
	{
		/* just testing... */
		tp = tbuf;
	}
	*tp++ = ';';

	cp = LCD.Msgstring(M_QUERY, LCD_MSG_FULLDISP);

	while (*cp>='0' && *cp<='9')
		cp++;
	if (*cp++ != '/')
		return 0;
	while (*cp>='0' && *cp<='9')
		cp++;

	/* looks good -- snarf it */
	while (*cp==' ') cp++;
	while (*cp && !(*cp==' ' && cp[1]==' '))
		*tp++ = *cp++;			/* copy last name */
	*tp++ = ',';
	*tp++ = ' ';
	while (*cp==' ') cp++;
	if (*cp=='\0') return 0;
	while (*cp)
		*tp++ = *cp++;			/* copy rest of line */
	while (*--cp==' ')
	{
		*cp = '\0';			/* eliminate trailing spaces */
		--tp;
	}
	while (*--tp != ' ')
		--cp;				/* back up to last space */
	*tp++ = ';';
	*tp++ = 'C';
	*tp++ = 'A';
	while (*++cp)
		*tp++ = *cp;			/* now copy number again */
	*tp++ = ';';
	*tp = '\0';

	if (it)
	{
		/* save the original buf in case we want to send it to host */
		(void) strcpy(sbuf, tbuf);
		/* uval==1 indicates autosave -- don't send to host */
		if (AddSpeedCall(tbuf) && it->ufield.uval==0)
			SendHost(PROC_PHONE, SCALLSAVE, sbuf);
	}
	return 1;
}

HelpWindow()
{
	Rectangle r;
	extern int HelpProc();
	extern Proc *new_proc();

	if (HelpThere)
		HelpEvent |= EVENT_MAKECURRENT;
	else
	{
#ifdef	USE730WINDOWS
		r = newrect(3, Rpt(Drect.origin, add(Drect.origin,
			Pt(81*FONTWIDTH(HELP_FONT), 26*FONTHEIGHT(HELP_FONT)))));
#else
		r = newrect(3, Rpt(Drect.origin, add(Drect.origin,
			Pt(81*FONTWIDTH(HELP_FONT),
								(26*FONTHEIGHT(HELP_FONT))+LABEL_HEIGHT))));
#endif
		if (r.origin.x != r.corner.x && r.origin.y != r.corner.y)
		{
			if( (helpproc = new_proc(HelpProc, r,
				(struct Logio *)0, STKSIZE, (char **)0))==(Proc *)0)
			{
				msgbox("CANNOT CREATE HELP WINDOW", NULL);
			}
		}
	}
}

HelpItem *HelpMenuGen(i, m)
int i;
HelpTmenu *m;
{
	register HelpItem *it;
	register HelpItem *tp;

	it = &tmpHelpItem;

	it->next = (HelpTmenu *)0;

	if (m == &HelpMenu)
	{
		if (i==0)
		{
			if (m->first==(HelpItem *)0)
				it->text = "No Help Available";
			else
				it->text = "Help Topics Available";
			it->tag = NULL;

			return it;
		} else
			--i;
	}

	for (tp=m->first; tp && i>0; tp=tp->nextitem, i--)
		;

	if (tp==(HelpItem *)0)
		it->text = NULL;
	else
	{
		*it = *tp;	/* structure copy */
	}

	return it;
}

InfoWindow()
{
	Rectangle r;
	extern int InfoProc();
	extern Proc *new_proc();

	if (InfoThere)
		InfoEvent |= EVENT_MAKECURRENT;
	else
	{
#ifdef	USE730WINDOWS
		r = newrect(3, Rpt(Drect.origin, add(Drect.origin,
												Pt(800, 120+LABEL_HEIGHT))));
#else
		r = newrect(3, Rpt(Drect.origin, add(Drect.origin, Pt(800, 120))));
#endif
		if (r.origin.x != r.corner.x && r.origin.y != r.corner.y)
		{
			if( (infoproc = new_proc(InfoProc, r,
				(struct Logio *)0, STKSIZE, (char **)0))==(Proc *)0)
			{
				msgbox("CANNOT CREATE HELP WINDOW", NULL);
			}
		}
	}
}

int ActionSnarfPost()
{
	SnarfPost(1);	/* if SnarfPost changes, this may need some work */
}

/*
 * the "it" parameter is only tested as non-zero!!!
 */
int SnarfPost(it)
register MainItem *it;
{
	int n;
	char snarfbuf[128];
D 2
	char area[8], exch[8], ext[8];
	char dialbuf[20];
E 2

D 2
	area[0] = exch[0] = ext[0] = '\0';
E 2
	if ((n = getwbuf(snarfbuf, sizeof(snarfbuf)-1)) < 7)
		return 0;
	snarfbuf[n] = '\0';
	MPRINTF("SnarfPost '%s'\n", snarfbuf);
I 2
	return( SnarfDial( snarfbuf, it ) );
}


SnarfDial( snarfbuf, it )
	char *snarfbuf;
	register MainItem *it;
{
	char area[8], exch[8], ext[8];
	char dialbuf[20];

	area[0] = exch[0] = ext[0] = '\0';

E 2
	if (!regex(snarfstr, snarfbuf, area, exch, ext))
	    return 0;
I 2

E 2
	MPRINTF("regex %s-%s-%s\n", area, exch, ext);

	if (it)
	{
		sprintf(dialbuf, "CA%s%s", DialPlan.MapExch(
			area[0] ? area : DialPlan.areacode, exch), ext);
		OutEnQueue(&PhoneOutQueue, dialbuf);
	}

	return 1;
}

#ifdef DEBUG
MainItem *DebugItemGen(i, m)
int i;
Tmenu *m;
{
	register MainItem *it;
	extern int ToggleDebug();

	it = &tmpMainItem;
	it->ufield.uval = i;
	it->ufield.grey = 0;
	it->next = (Tmenu *)0;
	it->action = ToggleDebug;

	if (i < NDEBUGFLAGS)
	{
		it->text = DebugFlags[i];
		it->icon = (Debug & (1<<i)) ? &B_checkmark : (Bitmap *)0;
	} else
		it->text = NULL;

	return it;
}

int ToggleDebug(it)
register MainItem *it;
{
	Debug ^= (1<<it->ufield.uval);
}
#endif /* DEBUG */
E 1
