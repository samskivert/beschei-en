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

#include <exec/types.h>

#include "board.h"
#include "main.h"

int BeschABOUT( void )
{
	appInfo( "Bescheiﬂen\n\nby Michael D. Bayne\nmbayne@qualcomm.com" );

	return TRUE;
}

int BeschNEW( void )
{
	boardClassReset( Win );
	appStartGame();
	return TRUE;
}

int BeschQUIT( void )
{
	return FALSE;
}
