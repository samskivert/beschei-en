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
#include <proto/dos.h>
#define Move gfxMove
#include <proto/graphics.h>
#undef Move
#include <proto/intuition.h>

#include "card.h"

UWORD chip SpadeData[] = { 0x1000,0x3800,0x7C00,0xFE00,0x7C00,0x1000,0x3800, };
UWORD chip HeartData[] = { 0x6C00,0xFE00,0xFE00,0xFE00,0x7C00,0x3800,0x1000, };
UWORD chip ClubData[] =  { 0x1000,0x3800,0x5400,0xFE00,0x5400,0x1000,0x3800, };
UWORD chip DiamData[] =  { 0x1000,0x3800,0x7C00,0xFE00,0x7C00,0x3800,0x1000, };
struct Image SuitImages[] = {{ 0, 0, 7, 7, 1, SpadeData, 0x1, 0x2, NULL },
							 { 0, 0, 7, 7, 1, HeartData, 0x4, 0x2, NULL },
							 { 0, 0, 7, 7, 1, ClubData , 0x1, 0x2, NULL },
							 { 0, 0, 7, 7, 1, DiamData , 0x4, 0x2, NULL }};
char *RankToStr[] = {
	"!", "A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K" };
char *SuitToStr[] = { "S", "H", "C", "D" };

void cardClassInit( struct RastPort *theRastPort )
{
	long i, y;

	if( theRastPort->TxHeight > 8 )
		y = ( theRastPort->TxHeight - 7 )/2;
	else
		y = 0;

	for( i = 0; i < 4; i++ )
		SuitImages[i].TopEdge = y;
}

void cardRender( Card theCard, struct RastPort *Rast, long xPos, long yPos )
{
	SetDrMd( Rast, JAM2 );
	if( theCard )
	{
		if( cardFaceUp( theCard ))
		{
			SetAPen( Rast, 2 );
			RectFill( Rast, xPos, yPos, xPos+CARD_WIDTH-1,
					 yPos+CARD_HEIGHT-1 );
			SetBPen( Rast, 2 );
			switch( cardSuit( theCard ))
			{
			case HEARTS:
			case DIAMONDS:
				SetAPen( Rast, 6 );
				break;
			default:
				SetAPen( Rast, 1 );
				break;
			}
			SuitImages[cardSuit( theCard )].PlaneOnOff = 0x2;
			DrawImage( Rast, &SuitImages[cardSuit( theCard )],
					  xPos + Rast->TxWidth + 3, yPos + 3 );
			DrawImage( Rast, &SuitImages[cardSuit( theCard )],
					  xPos + CARD_WIDTH - Rast->TxWidth - 10,
					  yPos + CARD_HEIGHT - Rast->TxHeight - 2 );
			gfxMove( Rast, xPos + 2, yPos + Rast->TxBaseline + 3 );
			Text( Rast, RankToStr[cardRank( theCard )], 1 );
			gfxMove( Rast, xPos + CARD_WIDTH - Rast->TxWidth - 3,
				 yPos + CARD_HEIGHT - Rast->TxHeight + Rast->TxBaseline - 2 );
			Text( Rast, RankToStr[cardRank( theCard )], 1 );
		}
		else
		{
			SetAPen( Rast, 7 );
			RectFill( Rast, xPos, yPos, xPos+CARD_WIDTH-1,
					 yPos+CARD_HEIGHT-1 );
		}
	}
	else
	{
		SetAPen( Rast, 0 );
		RectFill( Rast, xPos, yPos, xPos+CARD_WIDTH-1, yPos+CARD_HEIGHT-1 );
	}
	SetAPen( Rast, 1 );
	gfxMove( Rast, xPos, yPos );
	Draw( Rast, xPos+CARD_WIDTH-1, yPos );
	Draw( Rast, xPos+CARD_WIDTH-1, yPos+CARD_HEIGHT-1 );
	Draw( Rast, xPos, yPos+CARD_HEIGHT-1 );
	Draw( Rast, xPos, yPos );
	SetAPen( Rast, 5 );
	WritePixel( Rast, xPos, yPos );
	WritePixel( Rast, xPos+CARD_WIDTH-1, yPos );
	WritePixel( Rast, xPos, yPos+CARD_HEIGHT-1 );
	WritePixel( Rast, xPos+CARD_WIDTH-1, yPos+CARD_HEIGHT-1 );
}

long cardCompareRank( Card cardOne, Card cardTwo )
{
	int rankOne = cardRank( cardOne );
	int rankTwo = cardRank( cardTwo );
	
	if( rankOne == rankTwo )
		return 0;

	if( rankOne == ACE )
		rankOne = 15;
	if( rankTwo == ACE )
		rankTwo = 15;

	if( rankOne > rankTwo )
		return 1;
	
	return -1;
}
