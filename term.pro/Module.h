/* The X and DMD versions of this file are the same.
 * Eric Claeys, 1/95
*/

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
