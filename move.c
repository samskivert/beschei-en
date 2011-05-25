/* $Id: move.c,v 39.1 1995/04/25 01:47:37 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 *
 * $Log: move.c,v $
 * Revision 39.1  1995/04/25  01:47:37  mbayne
 * Initial revision.
 *
 */

#include <exec/memory.h>
#include <proto/exec.h>

#include "resource.h"
#include "move.h"

struct List FreeMoves;

void moveClassDestruct( void *Dummy )
{
    Move *aMove;

    while( aMove = ( Move * )RemHead( &FreeMoves ))
        moveReallyFree( aMove );
}

int moveClassInit( void )
{
    NewList( &FreeMoves );
    resourceAdd( 0L, moveClassDestruct );

    return 0;
}

void moveReallyFree( Move *theMove )
{
    if( theMove )
    {
        FreeMem( theMove, sizeof( Move ));
    }
}

Move *moveAlloc( int sourceStack, int destStack )
{
    Move *theMove;

    if( IsListEmpty( &FreeMoves ))
        theMove = AllocMem( sizeof( Move ), MEMF_CLEAR );
    else
        theMove = ( Move * )RemHead( &FreeMoves );

    theMove->mv_SourceStack = sourceStack;
    theMove->mv_DestStack = destStack;

    return theMove;
}
