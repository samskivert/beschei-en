#ifndef OPPONENT_H
#define OPPONENT_H

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

#define PLACE_DELAY 25
#define MAX_HYPOTHESIS_DEPTH 20

typedef struct _moveInfoList
{
	struct MinNode mil_Node;
	struct List mil_List;
} moveInfoList;

#include "protos/opponent_protos.h"

#endif /* OPPONENT_H */
