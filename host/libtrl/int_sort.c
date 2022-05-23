/* sort integers v[ 0 ] .. v[ n - 1 ]  into increasing order */

int_sort( v, n )
int v[];
int n;
{
	register int gap, i, j, tmp;

	for ( gap = n / 2 ; gap > 0 ; gap /= 2 )
	{
		for ( i = gap ; i < n ; i++ )
		{
			for ( j = i - gap ; j >= 0 ; j -= gap )
			{
				if ( v[ j ] <= v[ j + gap ] )
					break;

				tmp = v[ j ];
				v[ j ] = v[ j + gap ];
				v[ j + gap ] = tmp;
			}
		}
	}
}
