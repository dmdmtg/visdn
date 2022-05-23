#include <stdio.h>

char *
strdup( s )
char *s;
{
        char *t, *malloc();

        if ( t = malloc( strlen( s ) + 1 ) )
                strcpy( t, s );

        if ( t == NULL )
                fprintf( stderr, "strdup: cannot malloc for \"%s\"\n", s );

        return( t );
}
