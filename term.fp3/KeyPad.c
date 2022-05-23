#include "visdn3.h"

char *KButs[] = {
	"1,1;1;&&x2,1,1|&&x2,1,0;b;;",
	"1,2;2;&&x2,2,1|&&x2,2,0;b;;",
	"1,3;3;&&x2,3,1|&&x2,3,0;b;;",
	"2,1;4;&&x2,4,1|&&x2,4,0;b;;",
	"2,2;5;&&x2,5,1|&&x2,5,0;b;;",
	"2,3;6;&&x2,6,1|&&x2,6,0;b;;",
	"3,1;7;&&x2,7,1|&&x2,7,0;b;;",
	"3,2;8;&&x2,8,1|&&x2,8,0;b;;",
	"3,3;9;&&x2,9,1|&&x2,9,0;b;;",
	"4,1;*;&&x2,*,1|&&x2,*,0;b;;",
	"4,2;0;&&x2,0,1|&&x2,0,0;b;;",
	"4,3;#;&&x2,#,1|&&x2,#,0;b;;",
	NULL,
};

KeyPadInit()
{
	register char **fbp;

	for (fbp=KButs; *fbp; fbp++)
		(void) AddKeyBut(*fbp);
}

KeyPadMove(s)
register char *s;
{
	register FeatBut *kp;
	int oldx, oldy;
	Point p;

	oldx = KeyPadX;
	oldy = KeyPadY;

	KeyPadPos(s);

	if (oldx != KeyPadX || oldy != KeyPadY)
	{
		if (oldx && oldy)
		{
			/* was displayed, so clear it first */
			for (kp=KeyPad; kp; kp=kp->next)
				(void) rectf(&display, kp->rect, F_CLR);

			p = sub(Pt(KeyPadX, KeyPadY), Pt(oldx, oldy));

			for (kp=KeyPad; kp; kp=kp->next)
				kp->rect = raddp(kp->rect, p);
		} else
		{
			KeyPadInit();
		}

		DispKeyPad();
	}
}

KeyPadPos(s)
register char *s;
{
	register int x, y;

	for (x=0; *s>='0' && *s<='9'; s++)
		x = x*10 + *s-'0';
	if (*s++ != ',')
		return;
	for (y=0; *s>='0' && *s<='9'; s++)
		y = y*10 + *s-'0';

	if (x < SCREEN_OFFSET) x = SCREEN_OFFSET;
	if (y < SCREEN_OFFSET) y = SCREEN_OFFSET;

	KeyPadX = x;
	KeyPadY = y;
}
