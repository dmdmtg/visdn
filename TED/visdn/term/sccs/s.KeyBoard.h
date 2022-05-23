h25494
s 00014/00000/00000
d D 1.1 92/07/23 13:58:54 trl 1 0
c date and time created 92/07/23 13:58:54 by trl
e
u
U
t
T
I 1

typedef struct KBrect KBrect;
struct KBrect {
	KBrect *next;
	char startrow, startcol;
	char endrow, endcol;
	Rectangle rect;
	Point displaypt;
	Font *font;
	void (*rcv)();
	int bufsiz;
	char *buf;
	char *bufptr;
};
E 1
