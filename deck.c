/* $Id: deck.c,v 39.1 1995/04/25 01:44:41 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 *
 * $Log: deck.c,v $
 * Revision 39.1  1995/04/25  01:44:41  mbayne
 * Initial revision.
 *
 */

#include <exec/memory.h>
#include <proto/exec.h>
#define Move gfxMove
#include <proto/graphics.h>
#undef Move

#include <stdlib.h>

#include "resource.h"
#include "card.h"
#include "board.h"
#include "main.h"
#include "deck.h"

void deckFree( Card *theDeck )
{
    FreeMem( theDeck, 52 * sizeof( Card ));
}

void deckInit( Card *theDeck )
{
    int i;
    Rank r;
    Suit s;

    for( i = 0, s = SPADES; s <= DIAMONDS; s++ )
    {
        for( r = ACE; r <= KING; r++ )
        {
            theDeck[i++] = cardConstruct( r, s, 0 );
        }
    }
}

Card *deckAlloc( long WithCards )
{
    Card *theDeck;

    if( theDeck = AllocMem( sizeof( Card ) * 52, MEMF_CLEAR ))
    {
        if( WithCards )
            deckInit( theDeck );
        resourceAdd( theDeck, deckFree );
    }

    return theDeck;
}

void deckShuffle( Card *theDeck, long RemainingShuffles )
{
    int sd1Pos = 26, sd2Pos = 26, tdPos = 0;
    Card subDeck1[26], subDeck2[26];
    long i, r, done = FALSE;

    CopyMem( theDeck, subDeck1, 26 * sizeof( Card ));
    CopyMem( theDeck+26, subDeck2, 26 * sizeof( Card ));
    while( !done )
    {
        r = (rand()>>(VBeamPos()%20))%4;
        for( i = 0; ( i < r )&& !done; i++ )
        {
            theDeck[tdPos++] = subDeck1[--sd1Pos];
            done = !sd1Pos;
        }
        r = (rand()>>(VBeamPos()%20))%4;
        for( i = 0; ( i < r )&& !done; i++ )
        {
            theDeck[tdPos++] = subDeck2[--sd2Pos];
            done = !sd2Pos;
        }
    }
    while( sd1Pos )
        theDeck[tdPos++] = subDeck1[--sd1Pos];
    while( sd2Pos )
        theDeck[tdPos++] = subDeck2[--sd2Pos];
    if( --RemainingShuffles > 0 )
        deckShuffle( theDeck, RemainingShuffles );
}

void deckDeal( struct RastPort *theRastPort )
{
    int i, d1Pos = 0, d2Pos = 0;
    Card tempCard;

    deckInit( Deck1 );
    deckInit( Deck2 );

    do
    {
        /* Shuffle both decks */
        deckShuffle( Deck1, 15 );
        deckShuffle( Deck2, 15 );

        /* Check to make sure we can actually play with this shuffle. */
        if(( Deck1[9] != Deck2[9] )||( Deck1[19] != Deck2[19] )||
           ( Deck1[29] != Deck2[29] )||( Deck1[39] != Deck2[39] ))
            break;
        appInfo( "Whoa!" );
    }
    while( 1 );

    /* Deal out ten cards in each player's right stack */
    while( 1 )
    {
        for( i = 0; i < 10; i++ )
        {
            stackAddCard( AltSource[0], Deck1[d1Pos++] );
        }
        for( i = 0; i < 10; i++ )
        {
            stackAddCard( AltSource[1], Deck2[d2Pos++] );
        }
        if( cardCompareRank( stackTopCard( AltSource[0] ),
                            stackTopCard( AltSource[1] )))
        {
            /* We set the player num to the opposite player because first thing
               we do is swap players in order to render things the first time.
               The player with the highest card in their right stack goes
               first. */
            PlNo = ( cardCompareRank( stackTopCard( AltSource[0] ),
                                     stackTopCard( AltSource[1] )) > 0 );
            break;
        }
        else
        {
            tempCard = stackRemoveCard( AltSource[0] );
            stackAddCard( AltSource[0], cardMakeFaceUp( tempCard ));
            tempCard = stackRemoveCard( AltSource[1] );
            stackAddCard( AltSource[1], cardMakeFaceUp( tempCard ));
            stackRender( AltSource[0], theRastPort, FALSE );
            stackRender( AltSource[1], theRastPort, FALSE );
            appInfo( "Tie, dealing ten more cards." );
            tempCard = stackRemoveCard( AltSource[0] );
            stackAddCard( AltSource[0], cardMakeFaceDown( tempCard ));
            tempCard = stackRemoveCard( AltSource[1] );
            stackAddCard( AltSource[1], cardMakeFaceDown( tempCard ));
        }
    }

    tempCard = stackRemoveCard( AltSource[0] );
    stackAddCard( AltSource[0], cardMakeFaceUp( tempCard ));

    stackAddCard( Stacks+6, cardMakeFaceUp( Deck1[d1Pos++] ));
    stackAddCard( Stacks+10, cardMakeFaceUp( Deck1[d1Pos++] ));
    stackAddCard( Stacks+14, cardMakeFaceUp( Deck1[d1Pos++] ));
    stackAddCard( Stacks+18, cardMakeFaceUp( Deck1[d1Pos++] ));

    tempCard = stackRemoveCard( AltSource[1] );
    stackAddCard( AltSource[1], cardMakeFaceUp( tempCard ));

    stackAddCard( Stacks+3, cardMakeFaceUp( Deck2[d2Pos++] ));
    stackAddCard( Stacks+7, cardMakeFaceUp( Deck2[d2Pos++] ));
    stackAddCard( Stacks+11, cardMakeFaceUp( Deck2[d2Pos++] ));
    stackAddCard( Stacks+15, cardMakeFaceUp( Deck2[d2Pos++] ));

    do stackAddCard( MainSource[0], Deck1[d1Pos++] );
    while( d1Pos < 52 );
    do stackAddCard( MainSource[1], Deck2[d2Pos++] );
    while( d2Pos < 52 );
}
