#ifndef STACK_H
#define STACK_H

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

/* Stack arrangements (to describe how the stack is drawn) */
#define SA_VERTICAL 0
#define SA_SKEWED   1

/* Stack disciplines (to describe which cards can be legally placed on the
   stack) */
#define SD_EQU_INCREASING     0
#define SD_ALT_DECREASING     1
#define SD_EQU_UNIDIRECTIONAL 2

/* Stack classes */
#define SC_SOURCE  0
#define SC_DISCARD 1
#define SC_TEMP    2
#define SC_DEST    3

#ifndef CARD_H
#include "card.h"
#endif

typedef unsigned char uchar;

typedef struct _StackClass
{
	uchar sc_Arrangement;
	uchar sc_Discipline;
} StackClass;

typedef struct _NewStack
{
	short ns_Top;
	short ns_Left;
	uchar ns_ClassIdx;
	uchar ns_Owner;
	long  ns_Suit;
} NewStack;

typedef struct _Stack
{
	Card st_Cards[52];
	short st_Top;
	short st_Left;
	uchar st_Index;
	uchar st_ClassIdx;
	uchar st_Owner;
	uchar st_NumCards;
	long  st_Suit;
} Stack;

#define stackArrangement( s ) (stackClasses[(s)->st_ClassIdx].sc_Arrangement)
#define stackDiscipline( s ) (stackClasses[(s)->st_ClassIdx].sc_Discipline)

#define stackGetCard(s,cn)\
	(Card)(stackEntries(s)>(cn) ? (s)->st_Cards[stackEntries(s)-(cn)-1] : 0)

#define stackTopCard(s) stackGetCard(s,0)

#define stackBottomCard(s)\
	(Card)((s)->st_NumCards ? (Card)(s)->st_Cards[0] : 0)

#define stackEntries(s) ((s)->st_NumCards)

#define stackAddCard(s,c) ((s)->st_Cards[(s)->st_NumCards++] = (c))

#ifndef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif

#include "protos/stack_protos.h"

#endif /* STACK_H */
