/* random number generator
 *
 * usage: call init_rand() once before calling get_rand()
 *
 *	coin_toss = get_rand( 0, 1 )
 *	decimal_digit = get_rand( 0, 9 )
 *	die_roll = get_rand( 1, 6 )
 */

init_rand()
{
	long seed;

	seed = time( (long *) 0);

	srand48( seed );
}
get_rand( min, max )
int min, max;
{
	long lrand48();

	return( ( lrand48() % ( max - min + 1 ) ) + min );
}
