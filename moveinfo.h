#ifndef MOVEINFO_H
#define MOVEINFO_H

/* $Id: moveinfo.h,v 39.1 1995/04/25 01:48:51 mbayne Exp mbayne $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 * 
 * $Log: moveinfo.h,v $
 * Revision 39.1  1995/04/25  01:48:51  mbayne
 * Initial revision.
 *
 */

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif

#ifndef STATE_H
#include "stack.h"
#endif

#ifndef MOVE_H
#include "move.h"
#endif

typedef struct _MoveInfo
{
	struct MinNode    mi_Node;
	struct List       mi_Moves;
	Move             *mi_CurMove;
	Stack            *mi_Stacks;
	struct _MoveInfo *mi_Parent;
	int               mi_Score;
	short             mi_SrcStack;
	short             mi_DestStack;
	long              mi_Order[8];
} MoveInfo;

#define moveInfoHead(l)        (IsListEmpty(l)?0L:(MoveInfo *)(l)->lh_Head)
#define moveInfoHeadNoCheck(l) ((MoveInfo *)(l)->lh_Head)
#define moveInfoTail(l)        (IsListEmpty(l)?0L:(MoveInfo *)(l)->lh_TailPred)
#define moveInfoTailNoCheck(l) ((MoveInfo *)(l)->lh_TailPred)
#define moveInfoSucc(mi)       ((MoveInfo *)(mi)->mi_Node.mln_Succ)
#define moveInfoPred(mi)       ((MoveInfo *)(mi)->mi_Node.mln_Pred)
#define moveInfoEmpty(mi)      IsListEmpty(&((mi)->mi_Moves))
#define moveInfoAddHead(l,mi)  AddHead((l),(struct Node *)(mi))
#define moveInfoRemHead(l)     (MoveInfo *)RemHead(l)
#define moveInfoAddTail(l,mi)  AddTail((l),(struct Node *)(mi))
#define moveInfoRemTail(l)     (MoveInfo *)RemTail(l)

#ifndef CRITERION_H
#include "criterion.h"
#endif

#include "protos/moveinfo_protos.h"

#endif /* MOVEINFO_H */
