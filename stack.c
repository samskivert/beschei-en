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
#define Move gfxMove
#include <proto/graphics.h>
#undef Move
#include <proto/intuition.h>

#include "resource.h"
#include "deck.h"
#include "board.h"
#include "stack.h"
#include "main.h"

StackClass stackClasses[] =
{{ SA_VERTICAL, SD_EQU_UNIDIRECTIONAL }, /* SC_SOURCE */
 { SA_VERTICAL, SD_EQU_UNIDIRECTIONAL }, /* SC_DISCARD */
 { SA_SKEWED, SD_ALT_DECREASING },       /* SC_TEMP */
 { SA_VERTICAL, SD_EQU_INCREASING }};    /* SC_DEST */

void stackFree( Stack *theStack )
{
	if( theStack )
	{
		FreeMem( theStack, sizeof( Stack ));
	}
}

void stackReset( Stack *theStack )
{
	BltClear( theStack->st_Cards, sizeof( Card )*52, 0L );
	theStack->st_NumCards = 0;
}

void stackInit( Stack *theStack, NewStack *theNewStack, long Index )
{
	stackReset( theStack );
	theStack->st_Left = theNewStack->ns_Left;
	theStack->st_Top = theNewStack->ns_Top;
	theStack->st_Index = ( uchar )Index;
	theStack->st_ClassIdx = theNewStack->ns_ClassIdx;
	theStack->st_Owner = theNewStack->ns_Owner;
	theStack->st_Suit = theNewStack->ns_Suit;
}

Stack *stackAlloc( uchar Index )
{
	Stack *newStack = AllocMem( sizeof( Stack ), MEMF_CLEAR );

	newStack->st_Index = ( uchar )Index;
	
	resourceAdd( newStack, stackFree );

	return newStack;
}

long stackContainsPoint( Stack *theStack, Point *thePoint )
{
	long x = thePoint->x - theStack->st_Left;
	long y = thePoint->y - theStack->st_Top;

	if( stackArrangement( theStack ) == SA_SKEWED )
	{
		if( theStack->st_Left > Scr->Width/2 )
			x -= (( stackEntries( theStack )-1 ) * CARD_WIDTH/3 );
		else
			x += (( stackEntries( theStack )-1 ) * CARD_WIDTH/3 );
	}

	return ( x >= 0 )&&( y >= 0 )&&( x < CARD_WIDTH )&&( y < CARD_HEIGHT );
}

void stackRender( Stack *theStack, struct RastPort *theRastPort, long Full )
{
	long Left, Delta, i;
	
	switch( stackArrangement( theStack ))
	{
	case SA_VERTICAL:
		cardRender( stackGetCard( theStack, 0 ), theRastPort,
				   theStack->st_Left, theStack->st_Top );
		break;
	case SA_SKEWED:
		Left = theStack->st_Left;
		Delta = ( Left > Scr->Width/2 )? CARD_WIDTH/3 : -CARD_WIDTH/3;
		i = max( stackEntries( theStack ), 1 );
		do
		{
			if( Full ||( i == 1 ))
				cardRender( stackGetCard( theStack, i-1 ), theRastPort, Left,
						   theStack->st_Top );
			if( --i )
				Left += Delta;
		}
		while( i );
		SetAPen( theRastPort, 5 );
		/* Clear out the last 1/3 card incase an old card was there */
		if( Delta < 0 )
		   RectFill( theRastPort, Left+Delta, theStack->st_Top, Left-1,
					 theStack->st_Top+CARD_HEIGHT-1 );
		else
			RectFill( theRastPort, Left+CARD_WIDTH, theStack->st_Top,
					 Left+CARD_WIDTH+Delta-1, theStack->st_Top+CARD_HEIGHT-1 );
		break;
	}

	if( !theStack->st_NumCards &&( theStack->st_Suit < 4 ))
	{	
		struct Image *theImage = SuitImages+theStack->st_Suit;
		
		theImage->PlaneOnOff = 0x0;
		DrawImage( theRastPort, theImage,
				  theStack->st_Left + CARD_WIDTH/2 - theImage->Width/2,
				  theStack->st_Top + CARD_HEIGHT/2 - theImage->Height/2 );
	}
}

void stackMakeTopCardFaceUp( Stack *theStack, struct RastPort *theRastPort )
{
	Card aCard;

	if( aCard = stackRemoveCard( theStack ))
	{
		stackAddCard( theStack, cardMakeFaceUp( aCard ));
		stackRender( theStack, theRastPort, FALSE );
	}
}

Card stackRemoveCard( Stack *theStack )
{
	Card aCard;
	
	if( !theStack->st_NumCards )
		return 0;

	aCard = theStack->st_Cards[--theStack->st_NumCards];

	theStack->st_Cards[theStack->st_NumCards] = 0;

	return aCard;
}	
