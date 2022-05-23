h21148
s 00015/00000/00000
d D 1.1 92/07/23 13:58:56 trl 1 0
c date and time created 92/07/23 13:58:56 by trl
e
u
U
t
T
I 1
/*
 * preprocessor definitions for modules
 */

#define MODULE(x) \
	typedef struct x x; \
	struct x

#define FUNC(t, f)	t (*f)()

/*
 * placeboes
 */
#define	PRIVATE
#define PUBLIC
E 1
