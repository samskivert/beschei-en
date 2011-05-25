#ifndef MOVE_H
#define MOVE_H

/* $Id: move.h,v 39.1 1995/04/25 01:47:25 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 *
 * $Log: move.h,v $
 * Revision 39.1  1995/04/25  01:47:25  mbayne
 * Initial revision.
 *
 */

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif

typedef struct _Move
{
    struct MinNode mv_Node;
    short          mv_SourceStack;
    short          mv_DestStack;
} Move;

#define moveHead(l)        (IsListEmpty(l)?0L:(Move *)(l)->lh_Head)
#define moveHeadNoCheck(l) ((Move *)(l)->lh_Head)
#define moveTail(l)        (IsListEmpty(l)?0L:(Move *)(l)->lh_TailPred)
#define moveTailNoCheck(l) ((Move *)(l)->lh_TailPred)
#define moveSucc(m)        ((Move *)(m)->mv_Node.mln_Succ)
#define movePred(m)        ((Move *)(m)->mv_Node.mln_Pred)
#define moveAddHead(l,m)   AddHead((l),(struct Node *)(m))
#define moveRemHead(l)     (Move *)RemHead(l)
#define moveAddTail(l,m)   AddTail((l),(struct Node *)(m))
#define moveRemTail(l)     (Move *)RemTail(l)
#define moveCopy(m)        moveAlloc((m)->mv_SourceStack,(m)->mv_DestStack)
#define moveFree(m)        AddHead(&FreeMoves,(struct Node *)(m))

#include "protos/move_protos.h"

#endif /* MOVE_H */
