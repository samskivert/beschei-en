/* $Id: menus.c,v 39.1 1995/04/25 01:41:24 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 *
 * $Log: menus.c,v $
 * Revision 39.1  1995/04/25  01:41:24  mbayne
 * Initial revision.
 *
 */

#include <exec/types.h>

#include "board.h"
#include "main.h"

int BeschABOUT( void )
{
    appInfo( "Bescheißen\n\nby Michael D. Bayne\nmbayne@qualcomm.com" );

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
