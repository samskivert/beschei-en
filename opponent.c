/* $Id: opponent.c,v 39.1 1995/04/25 01:50:12 mbayne Exp mbayne $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 *
 * $Log: opponent.c,v $
 * Revision 39.1  1995/04/25  01:50:12  mbayne
 * Initial revision (needs a lot of work, even I can beat it sometimes).
 *
 */

#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>

#include "resource.h"
#include "board.h"
#include "moveinfo.h"
#include "main.h"
#include "opponent.h"

int tempStacks[] = { 3, 6, 7, 10, 11, 14, 15, 18, 22 };
int skippedMoves, equivalentStacks;
#define NUM_TEMP_STACKS 8

int MoveHighest, EmptyStacks, Found, CurMainSource, CurAltSource, CurDiscard;
struct List *MoveSequence, Depths[MAX_HYPOTHESIS_DEPTH];
int DepthLens[MAX_HYPOTHESIS_DEPTH];
MoveInfo *reqMoveInfo;
int HypothesisDepth = 10;

void opponentFreeVec( void *object )
{ FreeVec( object ); }

void opponentClassDestruct( void *Dummy )
{
}

int opponentClassInit( void )
{
    int i;

    for( i = 0; i < HypothesisDepth; i++ )
    {
        NewList( Depths+i );
        DepthLens[i] = 0;
    }

    if( MoveSequence = AllocVec( sizeof( struct List ), MEMF_CLEAR ))
    {
        NewList( MoveSequence );
        resourceAdd( MoveSequence, opponentFreeVec );
    }

    if( reqMoveInfo = moveInfoAlloc())
        resourceAdd( reqMoveInfo, moveInfoReallyFree );

    resourceAdd( 0L, opponentClassDestruct );

    return !( MoveSequence && reqMoveInfo );
}

void opponentPrintSequence( MoveInfo *theMoveInfo )
{
    FPrintf( Output(), "[" );
    while( theMoveInfo->mi_Parent )
    {
        FPrintf( Output(), "(%ld->%ld)", theMoveInfo->mi_SrcStack,
                theMoveInfo->mi_DestStack );
        theMoveInfo = theMoveInfo->mi_Parent;
    }
    FPrintf( Output(), "]\n" );
}

void opponentPrintStacks( Stack *theStacks )
{
    Card loCard, hiCard;
    int i, *stackNoPtr;

    for( i = *(stackNoPtr = tempStacks); i < NUM_STACKS; i = *(++stackNoPtr))
    {
        if( theStacks[i].st_NumCards )
        {
            loCard = theStacks[i].st_Cards[0];
            hiCard = theStacks[i].st_Cards[theStacks[i].st_NumCards-1];
            FPrintf( Output(), "[%s%s,%s%s]", RankToStr[cardRank(loCard)],
                    SuitToStr[cardSuit(loCard)], RankToStr[cardRank(hiCard)],
                    SuitToStr[cardSuit(hiCard)] );
        }
        else
            FPrintf( Output(), "[EMPTY]" );
    }
}

int opponentListLength( struct List *theList )
{
    struct Node *aNode;
    int length = 0;

    for( aNode = theList->lh_Head;
        aNode != theList->lh_TailPred->ln_Succ;
        aNode = aNode->ln_Succ )
        length++;

    return length;
}

int opponentCheckWin( void )
{
    int numCards = 0, i;

    /* Check to see if the player has no cards left */
    for( i = 0; i < NUM_STACKS; i++ )
    {
        if( Stacks[i].st_Owner == PlNo )
            numCards += stackEntries( Stacks+i );
    }

    return !numCards;
}

int opponentNoPossibleFlips( struct RastPort *theRastPort )
{
    if( stackEntries( AltSource[PlNo] ) &&
       !cardFaceUp( stackTopCard( AltSource[PlNo] )))
        return FALSE;

    if( stackEntries( MainSource[PlNo] ))
    {
        if( !cardFaceUp( stackTopCard( MainSource[PlNo] )))
            return FALSE;
    }
    else if( stackEntries( Discard[PlNo] ))
    {
        Card aCard;

        while( aCard = stackRemoveCard( Discard[PlNo] ))
        {
            cardMakeFaceDown( aCard );
            stackAddCard( MainSource[PlNo], aCard );
        }
        stackRender( Discard[PlNo], theRastPort, FALSE );
        stackRender( MainSource[PlNo], theRastPort, FALSE );
        Delay( PLACE_DELAY );
        return FALSE;
    }

    return TRUE;
}

int opponentMakeRequiredMoves( struct RastPort *theRastPort )
{
    int numMoves = 0;
    Move *aMove;

    do
    {
        moveInfoInit( reqMoveInfo );
        moveInfoApplyCriterion( reqMoveInfo, NonEmptyTemp, SuccSame );
        moveInfoApplyCriterion( reqMoveInfo, OwnNonEmptySource, SuccSame );
        if( aMove = moveRemHead( &reqMoveInfo->mi_Moves ))
        {
            boardApplyMove( aMove, theRastPort );
            moveFree( aMove );
            numMoves++;
        }
    }
    while( aMove );

    return numMoves;
}

int opponentMakePossibleMoves( struct RastPort *theRastPort )
{
    Move *aMove, *move1 = NULL, *move2 = NULL, *move3 = NULL;

    moveInfoInit( reqMoveInfo );
    moveInfoApplyCriterion( reqMoveInfo, OwnNonEmptySource, PredAlt );
    for (aMove = (Move *)reqMoveInfo->mi_Moves.lh_Head; moveSucc(aMove);
         aMove = moveSucc(aMove)) {
        if( aMove->mv_SourceStack == CurAltSource )
            move1 = aMove;
        else if( aMove->mv_SourceStack == CurMainSource )
            move2 = aMove;
        else if( aMove->mv_SourceStack == CurDiscard )
            move3 = aMove;
    }

    if (move1) boardApplyMove(move1, theRastPort);
    else if (move2) boardApplyMove(move2, theRastPort);
    else if (move3) boardApplyMove(move3, theRastPort);
    else return FALSE;

    return TRUE;
}

void opponentApplyAnalysis( struct RastPort *theRastPort )
{
    Move *aMove;

    while( aMove = moveRemHead( MoveSequence ))
    {
        boardApplyMove( aMove, theRastPort );
        moveFree( aMove );
    }
}

void opponentStoreMoveSequence( MoveInfo *theMoveInfo )
{
    MoveInfo *aMoveInfo;
    Move *aMove;

#ifdef DEBUG
    FPrintf( Output(), "Best sequence now: " );
    opponentPrintSequence( theMoveInfo );
#endif

    while( aMove = moveRemHead( MoveSequence ))
        moveFree( aMove );

    for( aMoveInfo = theMoveInfo; aMoveInfo->mi_Parent;
        aMoveInfo = aMoveInfo->mi_Parent )
    {
        if( aMove = moveAlloc( aMoveInfo->mi_SrcStack,
                              aMoveInfo->mi_DestStack ))
            moveAddHead( MoveSequence, aMove );
    }
}

int opponentStatesEquivalent (MoveInfo *leftInfo, MoveInfo *rightInfo)
{
    Stack *leftStacks, *rightStacks;
    long  *leftOrder, *rightOrder;
    int    failure = 0, i;

    leftStacks = leftInfo->mi_Stacks;
    rightStacks = rightInfo->mi_Stacks;

    /* check source stacks */
    failure += (stackTopCard(leftStacks+ 0) != stackTopCard(rightStacks+ 0));
    failure += (stackTopCard(leftStacks+ 1) != stackTopCard(rightStacks+ 1));
    failure += (stackTopCard(leftStacks+ 2) != stackTopCard(rightStacks+ 2));
    failure += (stackTopCard(leftStacks+19) != stackTopCard(rightStacks+19));
    failure += (stackTopCard(leftStacks+20) != stackTopCard(rightStacks+20));
    failure += (stackTopCard(leftStacks+21) != stackTopCard(rightStacks+21));
    if (failure) return FALSE;

    /* check destination stacks */
    failure += (stackTopCard(leftStacks+ 4) != stackTopCard(rightStacks+ 4));
    failure += (stackTopCard(leftStacks+ 5) != stackTopCard(rightStacks+ 5));
    failure += (stackTopCard(leftStacks+ 8) != stackTopCard(rightStacks+ 8));
    failure += (stackTopCard(leftStacks+ 9) != stackTopCard(rightStacks+ 9));
    failure += (stackTopCard(leftStacks+12) != stackTopCard(rightStacks+12));
    failure += (stackTopCard(leftStacks+13) != stackTopCard(rightStacks+13));
    failure += (stackTopCard(leftStacks+16) != stackTopCard(rightStacks+16));
    failure += (stackTopCard(leftStacks+17) != stackTopCard(rightStacks+17));
    if (failure) return FALSE;

    leftOrder = leftInfo->mi_Order;
    rightOrder = rightInfo->mi_Order;

    /* check the temp stacks */
    for (i = 0; i < NUM_TEMP_STACKS; i++)
        if (stackCompare(leftStacks+leftOrder[i], rightStacks+rightOrder[i]))
            return FALSE;

    return TRUE;
#ifdef SLOW_METHOD
    int i, j, leftEmpties = 0, rightEmpties = 0, matches = 0, entries;
    short miniStacks[NUM_STACKS], curLeft;
    int *stackNoPtr, *stackNoPtrJ;
    Card loCard, hiCard;

    /* Check player one's source stacks for equivalence */
    if( memcmp( leftStacks, rightStacks, 3 * sizeof( Stack )))
        return FALSE;

    /* Check player two's source stacks for equivalence */
    if( memcmp( leftStacks+19, rightStacks+19, 3 * sizeof( Stack )))
        return FALSE;

    memset( miniStacks, 0, NUM_STACKS * sizeof( short ));

    for( i = *(stackNoPtr = tempStacks); i < NUM_STACKS; i = *(++stackNoPtr))
    {
        if( !leftStacks[i].st_NumCards )
            leftEmpties++;
        if( !rightStacks[i].st_NumCards )
            rightEmpties++;
    }

    if( leftEmpties != rightEmpties )
        return FALSE;

    for( i = *(stackNoPtr = tempStacks); i < NUM_STACKS; i = *(++stackNoPtr))
    {
        if( rightStacks[i].st_NumCards )
        {
            loCard = cardSimple( rightStacks[i].st_Cards[0] );
            entries = rightStacks[i].st_NumCards-1;
            hiCard = cardSimple( rightStacks[i].st_Cards[entries] );
            miniStacks[i] = ( loCard << 8 ) | hiCard;
        }
    }

    for( i = *(stackNoPtr = tempStacks); i < NUM_STACKS; i = *(++stackNoPtr))
    {
        if( leftStacks[i].st_NumCards )
        {
            loCard = cardSimple( leftStacks[i].st_Cards[0] );
            entries = leftStacks[i].st_NumCards-1;
            hiCard = cardSimple( leftStacks[i].st_Cards[entries] );
            curLeft = ( loCard << 8 ) | hiCard;
            for( j = *(stackNoPtrJ = tempStacks); j < NUM_STACKS;
                j = *(++stackNoPtrJ))
            {
                if( curLeft == miniStacks[j] )
                {
                    matches++;
                    miniStacks[j] = 0;
                    break;
                }
            }
        }
    }

    if( matches == NUM_TEMP_STACKS - leftEmpties )
        return TRUE;

    return FALSE;
#endif
}

int opponentAlreadySeenMove( MoveInfo *root, MoveInfo *new, int Depth )
{
    MoveInfo *aMoveInfo;
    int i;

#ifdef DEBUG
    FPrintf( Output(), "Comparing: " );
    opponentPrintStacks( new->mi_Stacks );
    FPrintf( Output(), "\nTo       : " );
    opponentPrintStacks( root->mi_Stacks );
    FPrintf( Output(), "\n" );
#endif

    if( opponentStatesEquivalent( new, root )) {
        equivalentStacks++;
        return TRUE;
    }

    for( i = 0; i < Depth; i++ )
    {
        if( !IsListEmpty( Depths+i ))
        {
            for( aMoveInfo = moveInfoHead( Depths+i );;
                aMoveInfo = moveInfoSucc( aMoveInfo ))
            {
                if( opponentStatesEquivalent( new, aMoveInfo )) {
                    equivalentStacks++;
                    return TRUE;
                }
                if( aMoveInfo == moveInfoTailNoCheck( Depths+i )) break;
            }
        }
    }

    return FALSE;
}

void opponentAnalyzeHypothesis( MoveInfo *root, MoveInfo *current, int Depth )
{
    struct List *DepthList;
    MoveInfo *new;
    Move *aMove;

    if( Depth >= HypothesisDepth )
        goto FINAL_ANALYSIS;

    DepthList = Depths + Depth;

    /* required moves from temp stacks to destination stacks */
    moveInfoApplyCriterion( current, NonEmptyTemp, SuccSame );
    if( !moveInfoEmpty( current ))
        goto FURTHER_ANALYSIS;

    /* required moves from source stacks to destination stacks */
    moveInfoApplyCriterion( current, OwnNonEmptySource, SuccSame );
    if( !moveInfoEmpty( current ))
        goto FURTHER_ANALYSIS;

    /* all possible legal moves */
    moveInfoApplyCriterion( current, LegalSourceUniq, PredAltUniq );
    moveInfoApplyCriterion( current, LegalSource, UnidirSame );
    if( moveInfoEmpty( current ))
        goto FINAL_ANALYSIS;

 FURTHER_ANALYSIS:
    for( aMove = moveHead( &current->mi_Moves );
        aMove != moveSucc( moveTailNoCheck( &current->mi_Moves ));
        aMove = moveSucc( aMove ))
    {
        if( DepthLens[Depth] > 99 ) {
            skippedMoves++;
            continue;
        }

        new = moveInfoCopyWithMove( current, aMove );
        if( !new )
            continue;

        if( !opponentAlreadySeenMove( root, new, Depth+1 ))
        {
#ifdef DEBUG
            FPrintf( Output(), "Examining: " );
            opponentPrintSequence( new );
#endif
            DepthLens[Depth]++;
            if( aMove->mv_SourceStack == CurAltSource )
                new->mi_Score += 3;
            else if( aMove->mv_SourceStack == CurMainSource )
                new->mi_Score += 2;
            else if( aMove->mv_SourceStack == CurDiscard )
                new->mi_Score += 1;
            moveInfoAddTail( DepthList, new );
        }
        else
        {
#ifdef DEBUG
            FPrintf( Output(), "Rejecting: " );
            opponentPrintSequence( new );
#endif
            moveInfoFree( new );
        }
    }

 FINAL_ANALYSIS:
    if (
#ifdef CHECK_FOR_WIN
        (current->mi_Stacks[CurMainSource].st_NumCards +
         current->mi_Stacks[CurAltSource].st_NumCards +
         current->mi_Stacks[CurDiscard].st_NumCards == 0 ) ||
#endif
        ( moveInfoEmptyTempStacks( current ) > EmptyStacks ))
    {
        opponentStoreMoveSequence( current );
        Found = TRUE;
    }
    else if(( moveInfoEmptyTempStacks( current ) == EmptyStacks )&&
            ( current->mi_Score > MoveHighest ))
    {
        MoveHighest = current->mi_Score;
        opponentStoreMoveSequence( current );
        switch (current->mi_Score) {
        case 1: if (current->mi_Stacks[CurMainSource].st_NumCards) break;
        case 2: if (current->mi_Stacks[CurAltSource].st_NumCards) break;
        case 3: Found = TRUE;
        }
    }
}

void opponentAnalyzeBoard( struct RastPort *theRastPort )
{
    MoveInfo *root, *current;
    int i, j;

    root = moveInfoAlloc();
    moveInfoInit( root );
    EmptyStacks = moveInfoEmptyTempStacks( root );
    MoveHighest = 0;
    Found = FALSE;
    opponentAnalyzeHypothesis( root, root, 0 );

    for( i = 0; i < HypothesisDepth; i++ )
        boardRenderMax( theRastPort, i, 0 );

    for( i = 0; !Found &&( i < HypothesisDepth ); i++ )
    {
        boardRenderMax( theRastPort, i, opponentListLength( Depths+i ));
        skippedMoves = equivalentStacks = 0;
        if( !IsListEmpty( Depths+i ))
        {
            for( j = 0, current = moveInfoHead( Depths+i ); !Found;
                j++, current = moveInfoSucc( current ))
            {
                if( j%10 == 0 )
                    boardRenderIndicator( theRastPort, i, j );
                opponentAnalyzeHypothesis( root, current, i+1 );
                if( current == moveInfoTail( Depths+i ))
                    break;
            }
            boardRenderIndicator( theRastPort, i, j );
            /*FPrintf( Output(), "D%ld: S%ld, E%ld K%ld.\n", i, skippedMoves,
                    equivalentStacks, j );*/
        }
        else
        {
            break;
        }
    }
    for( i = 0; i < HypothesisDepth; i++ )
    {
        DepthLens[i] = 0;
        while( current = moveInfoRemHead( Depths+i ))
            moveInfoFree( current );
    }
    moveInfoFree( root );
}

void opponentMakeNextMove( struct RastPort *theRastPort )
{
    CurMainSource = MainSource[PlNo]->st_Index;
    CurAltSource = AltSource[PlNo]->st_Index;
    CurDiscard = Discard[PlNo]->st_Index;

    for(;;)
    {
        if( opponentCheckWin() ||
           ( SetSignal( 0L, SIGBREAKF_CTRL_C ) & SIGBREAKF_CTRL_C ))
            break;
        opponentMakeRequiredMoves( theRastPort );
        do /* Otherwise, flip our cards */
        {
            if( !cardFaceUp( stackTopCard( AltSource[PlNo] )))
            {
                stackMakeTopCardFaceUp( AltSource[PlNo], theRastPort );
                Delay( 10 );
                opponentMakeRequiredMoves( theRastPort );
            }
            if( !cardFaceUp( stackTopCard( MainSource[PlNo] )))
            {
                stackMakeTopCardFaceUp( MainSource[PlNo], theRastPort );
                Delay( 10 );
                opponentMakeRequiredMoves( theRastPort );
            }
        }
        while( !opponentNoPossibleFlips( theRastPort ));
        opponentAnalyzeBoard( theRastPort );
        if( IsListEmpty( MoveSequence ))
        {
            if( !opponentMakePossibleMoves( theRastPort ))
                break;
        }
        else
            opponentApplyAnalysis( theRastPort );
    }

    /* Inform the player of their loss if we've no cards left */
    if( opponentCheckWin()) {
        appInfo( "You lose. :)" );
        appStopGame();
    } else if( stackEntries( MainSource[PlNo] )) {
        /* Move our main source stack card to discard stack to end our turn */
        Move *tmpMove = moveAlloc( CurMainSource, CurDiscard );
        boardApplyMove( tmpMove, theRastPort );
        moveFree( tmpMove );
    }
}
