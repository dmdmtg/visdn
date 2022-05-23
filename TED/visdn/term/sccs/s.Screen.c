h54820
s 00116/00000/00000
d D 1.1 92/07/23 13:58:31 trl 1 0
c date and time created 92/07/23 13:58:31 by trl
e
u
U
t
T
I 1
#include "visdn.h"

Texture16 T_shade = {
	0x8888, 0x0000, 0x2222, 0x0000,
	0x8888, 0x0000, 0x2222, 0x0000,
	0x8888, 0x0000, 0x2222, 0x0000,
	0x8888, 0x0000, 0x2222, 0x0000,
};

void DispBut();

void ReDisplay()
{
	register FeatBut *fp;
	register KBrect *kp;

	(void) rectf(&display, Drect, F_CLR);
	(void) box(&display, Drect, F_STORE);

	LCD.Clear();
	LCD.Display();

	DispVersion(F_STORE);

	for (fp=FeatButs; fp; fp=fp->next)
		DispBut(fp, &BUT_FONT);

	DispKeyPad();

	for (kp=KBrects; kp; kp=kp->next)
	{
		KBclear(kp);
		KBdisplay(kp, kp->buf, F_STORE);
	}
}

void BoldBox(rect, type)
Rectangle rect;
int type;
{
	register int i;
	Rectangle r;

	for (i=1; i<=3; i++)
	{
		r.origin.x = rect.origin.x + i;
		r.origin.y = rect.origin.y + i;
		r.corner.x = rect.corner.x - i;
		r.corner.y = rect.corner.y - i;
		(void) box(&display, r, type);
	}
}

DispKeyPad()
{
	register FeatBut *fp;
	int Lx, Ly;

	for (fp=KeyPad; fp; fp=fp->next)
	{
		if (KPflags & DO_SHADE)
			(void) texture(&display, fp->rect, &T_shade, F_STORE);
		else
			(void) rectf(&display, fp->rect, F_CLR);
		(void) box(&display, fp->rect, F_STORE);
		Lx = fp->rect.origin.x + (KP_LEN-FONTWIDTH(KP_FONT))/2;
		Ly = fp->rect.origin.y + (KP_WID-FONTHEIGHT(KP_FONT))/2;
		(void) string(&KP_FONT, fp->name, &display, Pt(Lx, Ly), F_XOR);
	}
}

void DispBut(fp, fontp)
FeatBut *fp;
Font *fontp;
{
	if (fp->lamp && (fp->lamp->stat_status&LFLAG_ON))
		(void) rectf(&display, fp->rect, F_STORE);
	else
		(void) rectf(&display, fp->rect, F_CLR);
	if (fp->flags & DO_SHADE)
		(void) texture(&display, fp->rect, &T_shade, F_XOR);
	if (fp->lamp)
	{
		Rectangle r;

		r = fRect(fp->rect.origin.x+2, fp->rect.origin.y+2,
			 fp->rect.origin.x+LAMP_WID, fp->rect.corner.y-2);
		if (fp->lamp->iuse_status&LFLAG_ON)
			(void) rectf(&display, r, F_STORE);
		else
			(void) box(&display, r, F_STORE);
	}
	if (fp->flags & DO_BOX)
		(void) box(&display, fp->rect, F_STORE);
	if (fp->name)
	{
		int Lx, Ly;
		Lx = fp->rect.origin.x + 5;
		if (fp->lamp)
			Lx += LAMP_WID;
		Ly = fp->rect.origin.y + (FB_WID-FONTHEIGHT(BUT_FONT))/2;
		string(fontp, fp->name, &display, Pt(Lx, Ly), F_STORE);
	}
}

DispVersion(f)
int f;
{
	char tbuf[80];

	(void) sprintf(tbuf, "VISDN %sVersion %s/%s",
			Type==ATT7506?"7506 ":Type==ATT7507?"7507 ":"",
			TermVersion, HostVersion);
	(void) string(&VERS_FONT, tbuf, &display,
			Pt(LCD.rect.origin.x, Oy+FB_OFF), f);
}
E 1
