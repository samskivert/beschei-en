/* $Id$
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 * 
 * $Log$
 */

#include <exec/memory.h>
#include <proto/exec.h>
#include <stdlib.h>

#include "resource.h"

struct List Resources;

void resourceClassDestruct( int value )
{
	PvtResource *aRes;

	while( aRes = ( PvtResource * )RemHead( &Resources ))
	{
		aRes->rs_Destructor( aRes->rs_Resource );
		FreeMem( aRes, sizeof( PvtResource ));
	}
	
	exit( value );
}

void resourceClassInit( void )
{
	NewList( &Resources );
}

int resourceDestroy( void *theResource )
{
	PvtResource *aRes;

	if( IsListEmpty( &Resources ))
		return FALSE;

	for( aRes = resourceHead( &Resources );; aRes = resourceSucc( aRes ))
	{
		if( aRes->rs_Resource == theResource )
		{
			Remove(( struct Node * )aRes );
			aRes->rs_Destructor( aRes->rs_Resource );
			FreeMem( aRes, sizeof( PvtResource ));
			return TRUE;
		}
		if( aRes == resourceTail( &Resources ))
			break;
	}

	return FALSE;
}

void resourceAdd( void *Resource, FREEFUNC Destructor )
{
	PvtResource *theRes;
	
	if( theRes = AllocMem( sizeof( PvtResource ), MEMF_CLEAR|MEMF_PUBLIC ))
	{
		theRes->rs_Resource = Resource;
		theRes->rs_Destructor = Destructor;
		AddHead( &Resources, ( struct Node * )theRes );
	}
	else
		resourceClassDestruct( 2 );
}
