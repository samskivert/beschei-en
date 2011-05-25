/* $Id: board.c,v 39.1 1995/04/25 01:37:48 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 *
 * $Log: board.c,v $
 * Revision 39.1  1995/04/25  01:37:48  mbayne
 * Created the module and added all the functionality
 *
 */

#include <exec/memory.h>
#include <proto/exec.h>
#define Move gfxMove
#include <proto/graphics.h>
#undef Move
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/dos.h>

#include <stdio.h>

#include "main.h"
#include "moveinfo.h"
#include "resource.h"
#include "opponent.h"
#include "board.h"

#define INDIC_WID 64
#define INDIC_HEI 10

NewStack InitStacks[] =
{{0,-2,SC_SOURCE,1,NONE},
 {0,0,SC_DISCARD,1,NONE},
 {0,1,SC_SOURCE,1,NONE},
 {1,-2,SC_TEMP,2,NONE},
 {1,-1,SC_DEST,2,SPADES},
 {1,0,SC_DEST,2,SPADES},
 {1,1,SC_TEMP,2,NONE},
 {2,-2,SC_TEMP,2,NONE},
 {2,-1,SC_DEST,2,HEARTS},
 {2,0,SC_DEST,2,HEARTS},
 {2,1,SC_TEMP,2,NONE},
 {3,-2,SC_TEMP,2,NONE},
 {3,-1,SC_DEST,2,CLUBS},
 {3,0,SC_DEST,2,CLUBS},
 {3,1,SC_TEMP,2,NONE},
 {4,-2,SC_TEMP,2,NONE},
 {4,-1,SC_DEST,2,DIAMONDS},
 {4,0,SC_DEST,2,DIAMONDS},
 {4,1,SC_TEMP,2,NONE},
 {5,-2,SC_SOURCE,0,NONE},
 {5,-1,SC_DISCARD,0,NONE},
 {5,1,SC_SOURCE,0,NONE}};
MoveInfo *reqTempMoves, *reqSourceMoves, *legalMoves;
Card *Deck1, *Deck2;
Stack Stacks[NUM_STACKS], *MouseDown, *MouseUp;
Stack *Discard[2], *MainSource[2], *AltSource[2];
Move currentMove;
long PlNo = 0;

Point offsetPoint, storePoint;
struct Layer *cardLayer;
Card windowCard = 0;

void boardFree( void )
{
}

void boardDeleteLayer( struct Layer *theLayer )
{
    DeleteLayer( 0L, theLayer );
}

void boardClassInit( struct Window *theWin )
{
    long i, TopOffset;

    cardLayer = CreateBehindLayer( &Scr->LayerInfo, Scr->RastPort.BitMap,
                                  0, Scr->BarHeight + 1, CARD_WIDTH - 1,
                                  Scr->BarHeight + CARD_HEIGHT, LAYERSMART,
                                  0L );
    resourceAdd( cardLayer, boardDeleteLayer );

    reqTempMoves = moveInfoAlloc();
    resourceAdd( reqTempMoves, moveInfoReallyFree );
    reqSourceMoves = moveInfoAlloc();
    resourceAdd( reqSourceMoves, moveInfoReallyFree );
    legalMoves = moveInfoAlloc();
    resourceAdd( legalMoves, moveInfoReallyFree );

    Deck1 = deckAlloc( TRUE );
    Deck2 = deckAlloc( TRUE );

    TopOffset = ( theWin->Height%CARD_HEIGHT )/2 + Scr->BarHeight + 1;

    for( i = 0; i < NUM_STACKS; i++ )
    {
        InitStacks[i].ns_Left *= (CARD_WIDTH+5);
        InitStacks[i].ns_Left += theWin->Width/2;
        InitStacks[i].ns_Top *= (CARD_HEIGHT+5);
        InitStacks[i].ns_Top += TopOffset;
        stackInit( &Stacks[i], &InitStacks[i], i );
    }

    MainSource[0] = &Stacks[19];
    Discard[0] = &Stacks[20];
    AltSource[0] = &Stacks[21];

    MainSource[1] = &Stacks[2];
    Discard[1] = &Stacks[1];
    AltSource[1] = &Stacks[0];
}

int boardClassReset( struct Window *theWin )
{
    int value = 0, i;

    for( i = 0; i < NUM_STACKS; i++ )
        stackReset( Stacks+i );

    deckDeal( theWin->RPort );
    SetRast( Win->RPort, 5 );
    boardRender( theWin );
    boardSwitchPlayer( theWin->RPort );
    boardPrepareForMove();

    return value;
}

void boardRender( struct Window *theWin )
{
    long i;

    for( i = 0; i < NUM_STACKS; i++ )
        stackRender( Stacks+i, theWin->RPort, TRUE );
}

Stack *boardStackFromPoint( Point *thePoint )
{
    int i;

    for( i = 0; i < NUM_STACKS; i++ )
    {
        if( stackContainsPoint( Stacks+i, thePoint ))
            return Stacks+i;
    }

    return 0L;
}

void boardIllegalMove( struct RastPort *theRastPort, Move *theMove )
{
    char FailStr[64];

    if( !theMove )
        sprintf( FailStr, "I'm smoking crack." );
    else
        sprintf( FailStr, "You failed to make a required move. (%ld->%ld)",
                theMove->mv_SourceStack, theMove->mv_DestStack );
    appInfo( FailStr );
    if( cardFaceUp( stackTopCard( MainSource[PlNo] )))
    {
        stackAddCard( Discard[PlNo], stackRemoveCard( MainSource[PlNo] ));
        stackRender( MainSource[PlNo], theRastPort, FALSE );
        stackRender( Discard[PlNo], theRastPort, FALSE );
    }
}

long boardHandleMouseDown( Point *thePoint, struct RastPort *theRastPort )
{
    MouseDown = boardStackFromPoint( thePoint );

    if( !ValidStack( MouseDown ))
        return ( long )( MouseDown = 0L );

    if( !stackTopCard( MouseDown ))
    {
        if( MouseDown == MainSource[PlNo] )
        {
            Card aCard;

            if( stackEntries( Discard[PlNo] ))
            {
                while( aCard = stackRemoveCard( Discard[PlNo] ))
                {
                    cardMakeFaceDown( aCard );
                    stackAddCard( MouseDown, aCard );
                }
                stackRender( Discard[PlNo], theRastPort, FALSE );
                stackRender( MouseDown, theRastPort, FALSE );
                if( !moveInfoEmpty( reqTempMoves ))
                {
                    boardIllegalMove( theRastPort,
                                     moveHead( &reqTempMoves->mi_Moves ));
                    boardSwitchPlayer( theRastPort );
                }
                if( !moveInfoEmpty( reqSourceMoves ))
                {
                    boardIllegalMove( theRastPort,
                                     moveHead( &reqSourceMoves->mi_Moves ));
                    boardSwitchPlayer( theRastPort );
                }
            }
            else
                boardSwitchPlayer( theRastPort );
            boardPrepareForMove();
        }

        return ( long )( MouseDown = 0L );
    }

    if( !cardFaceUp( stackTopCard( MouseDown )))
    {
        Card tempCard = stackRemoveCard( MouseDown );

        stackAddCard( MouseDown, cardMakeFaceUp( tempCard ));
        stackRender( MouseDown, theRastPort, FALSE );
        if( !moveInfoEmpty( reqTempMoves ))
        {
            boardIllegalMove( theRastPort,
                             moveHead( &reqTempMoves->mi_Moves ));
            boardSwitchPlayer( theRastPort );
        }
        if( !moveInfoEmpty( reqSourceMoves ))
        {
            boardIllegalMove( theRastPort,
                             moveHead( &reqSourceMoves->mi_Moves ));
            boardSwitchPlayer( theRastPort );
        }
        boardPrepareForMove();

        return ( long )( MouseDown = 0L );
    }

    offsetPoint.x = thePoint->x - MouseDown->st_Left;
    offsetPoint.y = thePoint->y - MouseDown->st_Top - Scr->BarHeight - 1;

    if( stackArrangement( MouseDown ) == SA_SKEWED )
    {
        if( MouseDown->st_Left > Scr->Width/2 )
            offsetPoint.x -=
                (( stackEntries( MouseDown ) - 1 ) * CARD_WIDTH/3 );
        else
            offsetPoint.x +=
                (( stackEntries( MouseDown ) - 1 ) * CARD_WIDTH/3 );
    }

    windowCard = stackRemoveCard( MouseDown );
    stackRender( MouseDown, theRastPort, FALSE );
    cardRender( windowCard, cardLayer->rp, 0, 0 );
    boardHandleMouseMove( thePoint, theRastPort );
    UpfrontLayer( 0L, cardLayer );

    return TRUE;
}

void boardHandleMouseUp( Point *thePoint, struct RastPort *theRastPort )
{
    long reqTemp, reqSource, legal;
    long numCards = 0, i;

    if( !windowCard )
        return;

    if( !MouseDown )
        goto NOMOVE;

    BehindLayer( 0L, cardLayer );
    MouseUp = boardStackFromPoint( thePoint );
    if( !MouseUp ||( MouseUp == MouseDown ))
    {
        stackAddCard( MouseDown, windowCard );
        stackRender( MouseDown, theRastPort, FALSE );
        goto NOMOVE;
    }

    if( EndingMove( MouseDown, MouseUp ))
    {
        stackAddCard( MouseUp, windowCard );
        stackRender( MouseUp, theRastPort, FALSE );
        boardSwitchPlayer( theRastPort );
        boardPrepareForMove();
        goto NOMOVE;
    }

    currentMove.mv_SourceStack = MouseDown->st_Index;
    currentMove.mv_DestStack = MouseUp->st_Index;

    reqTemp = moveInfoContainsMove( reqTempMoves, &currentMove );
    reqSource = moveInfoContainsMove( reqSourceMoves, &currentMove );
    legal = moveInfoContainsMove( legalMoves, &currentMove );

    if( reqTemp || reqSource || legal )
    {
        stackAddCard( MouseUp, windowCard );
        stackRender( MouseUp, theRastPort, FALSE );

        if( !reqTemp && !moveInfoEmpty( reqTempMoves ))
        {
            boardIllegalMove( theRastPort,
                             moveHead( &reqTempMoves->mi_Moves ));
            boardSwitchPlayer( theRastPort );
        }
        else if( !reqTemp && !reqSource && !moveInfoEmpty( reqSourceMoves ))
        {
            boardIllegalMove( theRastPort,
                             moveHead( &reqSourceMoves->mi_Moves ));
            boardSwitchPlayer( theRastPort );
        }
        else
        {
            /* Check to see if the player has no cards left */
            for( i = 0; i < NUM_STACKS; i++ )
            {
                if( Stacks[i].st_Owner == PlNo )
                    numCards += stackEntries( Stacks+i );
            }
            if( !numCards )
            {
                appInfo( "You win!" );
                appStopGame();
            }
        }
        boardPrepareForMove();
    }
    else
    {
        stackAddCard( MouseDown, windowCard );
        stackRender( MouseDown, theRastPort, FALSE );
        DisplayBeep( 0L );
    }

 NOMOVE:
    windowCard = 0;
}

void boardHandleMouseMove( Point *thePoint, struct RastPort *theRastPort )
{
    storePoint.x = thePoint->x-offsetPoint.x;
    storePoint.y = thePoint->y-offsetPoint.y;
    storePoint.x = max( storePoint.x, 0 );
    storePoint.x = min( storePoint.x, Scr->Width-CARD_WIDTH );
    storePoint.y = max( storePoint.y, Scr->BarHeight+1 );
    storePoint.y = min( storePoint.y, Scr->Height-CARD_HEIGHT );
    MoveLayer( 0L, cardLayer, storePoint.x-cardLayer->bounds.MinX,
              storePoint.y-cardLayer->bounds.MinY );
}

void boardPrepareForMove( void )
{
    moveInfoInit( reqTempMoves );
    moveInfoApplyCriterion( reqTempMoves, NonEmptyTemp, SuccSame );
    moveInfoInit( reqSourceMoves );
    moveInfoApplyCriterion( reqSourceMoves, OwnNonEmptySource, SuccSame );
    moveInfoInit( legalMoves );
    moveInfoApplyCriterion( legalMoves, LegalSource, PredAlt );
    moveInfoApplyCriterion( legalMoves, LegalSource, UnidirSame );
}

void boardSwitchPlayer( struct RastPort *theRastPort )
{
    PlNo = 1-PlNo;
    SetDrMd( theRastPort, JAM2 );
    SetAPen( theRastPort, 1 );
    SetBPen( theRastPort, 5 );
    gfxMove( theRastPort, 10, 10 );
    Text( theRastPort, ( PlNo ? "Player One" : "Player Two" ), 10 );
    if( PlNo )
    {
        opponentMakeNextMove( theRastPort );
        boardSwitchPlayer( theRastPort );
    }
}

void boardApplyMove( Move *theMove, struct RastPort *theRastPort )
{
    int sx, sy, dx, dy, ex, ey, i, offx;
    Stack *src, *dest;
    Card aCard;

    src = Stacks+theMove->mv_SourceStack;
    dest = Stacks+theMove->mv_DestStack;

    aCard = stackRemoveCard( src );
    stackRender( src, theRastPort, FALSE );

    if( stackArrangement( src ) == SA_SKEWED )
        offx = (( src->st_Left > Scr->Width/2 ) ? 1 : -1 ) *
            ( stackEntries( src ) * CARD_WIDTH/3 );
    else
        offx = 0;

    sx = src->st_Left + offx;
    sy = src->st_Top+Scr->BarHeight+1;

    if( stackArrangement( dest ) == SA_SKEWED )
        offx = (( dest->st_Left > Scr->Width/2 ) ? 1 : -1 ) *
            ( stackEntries( dest ) * CARD_WIDTH/3 );
    else
        offx = 0;

    ex = dest->st_Left + offx;
    ey = dest->st_Top+Scr->BarHeight+1;

    dx = ( ex - sx ) / 10;
    dy = ( ey - sy ) / 10;

    cardRender( aCard, cardLayer->rp, 0, 0 );
    MoveLayer( 0L, cardLayer, sx-cardLayer->bounds.MinX,
              sy-cardLayer->bounds.MinY );
    UpfrontLayer( 0L, cardLayer );

    for( i = 0; i < 10; i++ )
        MoveLayer( 0L, cardLayer, dx, dy );

    MoveLayer( 0L, cardLayer, ex-cardLayer->bounds.MinX,
              ey-cardLayer->bounds.MinY );
    stackAddCard( dest, aCard );
    stackRender( dest, theRastPort, FALSE );
    BehindLayer( 0L, cardLayer );
}

void boardRenderMax( struct RastPort *theRastPort, LONG Depth, LONG MaxPos )
{
    LONG Left = Win->Width-INDIC_WID-10, Top = Depth*(INDIC_HEI+4)+10;
    LONG Wid = (MaxPos*INDIC_WID)/100;

    SetAPen( theRastPort, 7 );
    RectFill( theRastPort, Left, Top, Left+Wid-1, Top+INDIC_HEI-1 );

    SetAPen( theRastPort, 0 );
    RectFill( theRastPort, Left+Wid, Top, Left+INDIC_WID-1, Top+INDIC_HEI-1 );
}

void boardRenderIndicator( struct RastPort *theRastPort, LONG Depth, LONG Pos )
{
    LONG Left = Win->Width-INDIC_WID-10, Top = Depth*(INDIC_HEI+4)+10;
    LONG Wid = (Pos*INDIC_WID)/100;

    SetAPen( theRastPort, 4 );
    RectFill( theRastPort, Left, Top, Left+Wid-1, Top+INDIC_HEI-1 );
}
