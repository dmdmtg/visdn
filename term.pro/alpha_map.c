/* The X and DMD versions of this file are the same.
 *
 * Eric Claeys, 1/95
*/

/* index maps implicity to alpha char, see code below */

char AlphaMap[] =
{
	'2', '2', '2',		/* a b c */
	'3', '3', '3',		/* d e f */
	'4', '4', '4',		/* g h i */
	'5', '5', '5',		/* j k l */
	'6', '6', '6',		/* m n o */
	'7', '7', '7', '7',	/* p (q) r s */
	'8', '8', '8',		/* t u v */
	'9', '9', '9', '9',	/* w x y (z) */
};

/* Convert [a-zA-Z] to the corresponding AlphaMap[] char.
 * This allows phone numbers to contain letters, like "1800CALLATT".
 * Leave non-letters as is.
*/
char
az_to_09(c)
char c;
{
	if (c>='A' && c<='Z')
		c += ('a'-'A');		/* convert to lowercase */
	if (c>='a' && c<='z')
		c = AlphaMap[c-'a'];
	/* else leave as is */
	return(c);
}
