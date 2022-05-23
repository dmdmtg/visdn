h23168
s 00017/00000/00000
d D 1.1 92/07/23 13:58:54 trl 1 0
c date and time created 92/07/23 13:58:54 by trl
e
u
U
t
T
I 1

typedef struct UserInfo {
	char	*key,
		*name,
		*org,
		*tl,
		*com,
		*loc,
		*room,
		*tel,
		*cornet,
		*ema;
	Word	*face;
} UserInfo;

#define FACECHARS	(48*48/(sizeof(char)*8))
#define FACEWORDS	(48*48/(sizeof(Word)*8))
E 1
