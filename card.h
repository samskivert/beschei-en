#ifndef CARD_H
#define CARD_H

/* $Id: card.h,v 39.1 1995/04/25 01:44:29 mbayne Exp mbayne $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 *
 * $Log: card.h,v $
 * Revision 39.1  1995/04/25  01:44:29  mbayne
 * Initial revision.
 *
 */

typedef enum { SPADES = 0, HEARTS, CLUBS, DIAMONDS, NONE } Suit;

typedef enum { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
               JACK, QUEEN, KING } Rank;

typedef unsigned char Card;

#define cardRank( c ) ((c)&15)
#define cardSuit( c ) (((c)>>4)&3)
#define cardFaceUp( c ) ((c)>>6)
#define cardMakeFaceDown( c ) (c)&=(~(1<<6))
#define cardMakeFaceUp( c ) (c)|=(1<<6)
#define cardSimple( c ) ((c)&(~(1<<5)))
#define cardSimplifySuit( c ) (c)&=(~(1<<5))
#define cardConstruct( r, s, d ) ((Card)((((d)&1)<<6)|(((s)&3)<<4)|((r)&15)))
#define cardValue(c) ((cardSuit(c)*14)+cardRank(c))

#define CARD_WIDTH 48
#define CARD_HEIGHT 60

#ifndef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif

#include "protos/card_protos.h"

/* Since I don't feel like changing the Move type to something else, I'll
   just change the graphics library call to gfxMove. */
#pragma libcall GfxBase gfxMove f0 10903

#endif /* CARD_H */
