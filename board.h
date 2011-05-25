#ifndef BOARD_H
#define BOARD_H

/* $Id: board.h,v 39.1 1995/04/25 01:42:46 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 * 
 * $Log: board.h,v $
 * Revision 39.1  1995/04/25  01:42:46  mbayne
 * Initial revision.
 *
 */

#ifndef min
#define min( x, y ) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max( x, y ) ((x) > (y) ? (x) : (y))
#endif

#define NUM_STACKS 22

#define boardNumStacks() (NUM_STACKS)
#define boardStackArray() (Stacks)

#ifndef DECK_H
#include "deck.h"
#endif

#ifndef MOVE_H
#include "moveinfo.h"
#endif

#include "protos/board_protos.h"

#endif /* BOARD_H */
