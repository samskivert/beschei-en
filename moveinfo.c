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

#include "resource.h"
#include "board.h"
#include "moveinfo.h"

#ifdef DEBUG
#include <proto/dos.h>
#endif

struct List FreeMoveInfos;

void moveInfoClassDestruct( void *Dummy )
{
	MoveInfo *aMoveInfo;

	while( aMoveInfo = ( MoveInfo * )RemHead( &FreeMoveInfos ))
		moveInfoReallyFree( aMoveInfo );
}

int moveInfoClassInit( void )
{
	NewList( &FreeMoveInfos );
	resourceAdd( 0L, moveInfoClassDestruct );

	return 0;
}

void moveInfoReallyFree( MoveInfo *theMoveInfo )
{
	Move *aMove;

	if( theMoveInfo )
	{
		while( aMove = ( Move * )RemHead( &theMoveInfo->mi_Moves ))
			moveFree( aMove );
		if( theMoveInfo->mi_Stacks )
			FreeMem( theMoveInfo->mi_Stacks, sizeof( Stack )*boardNumStacks());
		FreeMem( theMoveInfo, sizeof( MoveInfo ));
	}
}

void moveInfoFree( MoveInfo *theMoveInfo )
{
	Move *aMove;

	while( aMove = ( Move * )RemHead( &theMoveInfo->mi_Moves ))
		moveFree( aMove );
	AddHead( &FreeMoveInfos, ( struct Node * )theMoveInfo );
}

MoveInfo *moveInfoAlloc( void )
{
	MoveInfo *theMoveInfo;

	if( IsListEmpty( &FreeMoveInfos ))
	{
		if( theMoveInfo = AllocMem( sizeof( MoveInfo ), MEMF_CLEAR ))
		{
			NewList( &theMoveInfo->mi_Moves );
			if(!( theMoveInfo->mi_Stacks =
				 AllocMem( sizeof( Stack )*boardNumStacks(), MEMF_CLEAR )))
			{
				moveInfoReallyFree( theMoveInfo );
				return 0L;
			}
		}
	}
	else
	{
		theMoveInfo = ( MoveInfo * )RemHead( &FreeMoveInfos );
		theMoveInfo->mi_SrcStack = 0;
		theMoveInfo->mi_DestStack = 0;
		theMoveInfo->mi_Parent = 0L;
	}
	
	return theMoveInfo;
}

int moveInfoContainsMove( MoveInfo *theMoveInfo, Move *theMove )
{
	Move *aMove;

	if( IsListEmpty( &theMoveInfo->mi_Moves ))
		return FALSE;

	for( aMove = moveHead( &theMoveInfo->mi_Moves );
		aMove != moveSucc( moveTail( &theMoveInfo->mi_Moves ));
		aMove = moveSucc( aMove ))
	{
		if(( theMove->mv_SourceStack == aMove->mv_SourceStack )&&
		   ( theMove->mv_DestStack == aMove->mv_DestStack ))
			return TRUE;
	}

	return FALSE;
}

void moveInfoStrip( MoveInfo *theMoveInfo )
{
	Move *aMove;

	while( aMove = moveRemHead( &theMoveInfo->mi_Moves ))
		moveFree( aMove );
}

void moveInfoInit( MoveInfo *theMoveInfo )
{
	moveInfoStrip( theMoveInfo );
	CopyMemQuick( boardStackArray(), theMoveInfo->mi_Stacks,
				 boardNumStacks()*sizeof( Stack ));
}

void moveInfoApplyCriterion( MoveInfo *theMoveInfo,
							SourceCriterionFunc theSourceCriterion,
							CriterionFunc theCriterion )
{
	Stack *sourceStack, *destStack;
	Move *theMove;
	int i, j;
	
#ifdef DEBUG
	FPrintf( Output(), "0x%lx: ", theMoveInfo );
#endif
	for( i = 0; i < boardNumStacks(); i++ )
	{
		if( !theSourceCriterion( sourceStack = &theMoveInfo->mi_Stacks[i] ))
			continue;
		for( j = 0; j < boardNumStacks(); j++ )
		{
			if( i == j )
				continue;
			destStack = &theMoveInfo->mi_Stacks[j];
			if(( theCriterion( sourceStack, destStack ))&&
			   ( theMove = moveAlloc( i, j )))
			{
				moveAddTail( &theMoveInfo->mi_Moves, theMove );
#ifdef DEBUG
				FPrintf( Output(), "(%ld->%ld)", i, j );
#endif
			}
		}
	}
#ifdef DEBUG
	FPrintf( Output(), "\n" );
#endif
}

MoveInfo *moveInfoCopyWithMove( MoveInfo *src, Move *theMove )
{
	MoveInfo *new;

	new = moveInfoAlloc();

	if( !new )
		return 0L;

	new->mi_Parent = src;
	new->mi_SrcStack = theMove->mv_SourceStack;
	new->mi_DestStack = theMove->mv_DestStack;
	new->mi_Score = src->mi_Score;
	CopyMemQuick( src->mi_Stacks, new->mi_Stacks,
				 sizeof( Stack )*boardNumStacks());
	stackAddCard( new->mi_Stacks+theMove->mv_DestStack,
				 stackRemoveCard( new->mi_Stacks+theMove->mv_SourceStack ));

	return new;
}

int moveInfoEmptyTempStacks( MoveInfo *theMoveInfo )
{
	int emptyStacks = 0;
	Stack *aStack;

	aStack = theMoveInfo->mi_Stacks;

	if( !aStack[3].st_NumCards )
		emptyStacks++;
	if( !aStack[6].st_NumCards )
		emptyStacks++;
	if( !aStack[7].st_NumCards )
		emptyStacks++;
	if( !aStack[10].st_NumCards )
		emptyStacks++;
	if( !aStack[11].st_NumCards )
		emptyStacks++;
	if( !aStack[14].st_NumCards )
		emptyStacks++;
	if( !aStack[15].st_NumCards )
		emptyStacks++;
	if( !aStack[18].st_NumCards )
		emptyStacks++;

	return emptyStacks;
}
