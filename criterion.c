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

#include <exec/types.h>

#include "board.h"
#include "criterion.h"

                       /*  Sp    He    Cl     Di  */
long SuitMatrix[][] = {{ FALSE, TRUE, FALSE, TRUE },  /* Sp */
					   { TRUE, FALSE, TRUE, FALSE },  /* He */
					   { FALSE, TRUE, FALSE, TRUE },  /* Cl */
					   { TRUE, FALSE, TRUE, FALSE }}; /* Di */

int EmptyStackTried;

int NonEmptyTemp( Stack *theStack )
{
	if( theStack->st_ClassIdx != SC_TEMP )
		return FALSE;
	
	if( !stackTopCard( theStack ))
		return FALSE;
	
	return TRUE;
}

int OwnNonEmptySource( Stack *theStack )
{
	Card aCard;
	
	if(( theStack->st_ClassIdx != SC_SOURCE )&&
	   ( theStack->st_ClassIdx != SC_DISCARD ))
		return FALSE;
	
	if( theStack->st_Owner != PlNo )
		return FALSE;
	
	aCard = stackTopCard( theStack );
	if( !aCard || !cardFaceUp( aCard ))
		return FALSE;
	
	return TRUE;
}

int LegalSource( Stack *theStack )
{
	Card aCard;
	
	if( theStack->st_Owner == (1-PlNo))
		return FALSE;
	
	if( theStack->st_ClassIdx == SC_DEST )
		return FALSE;
	
	aCard = stackTopCard( theStack );
	if( !aCard || !cardFaceUp( aCard ))
		return FALSE;
	
	return TRUE;
}

int LegalSourceUniq( Stack *theStack )
{
	Card aCard;
	
	EmptyStackTried = FALSE;

	if( theStack->st_Owner == (1-PlNo))
		return FALSE;
	
	if( theStack->st_ClassIdx == SC_DEST )
		return FALSE;
	
	aCard = stackTopCard( theStack );
	if( !aCard || !cardFaceUp( aCard ))
		return FALSE;
	
	return TRUE;
}

int SuccSame( Stack *theSrcStack, Stack *theDestStack )
{
	Card theSrcCard, theDestCard;
	
	if( theDestStack->st_ClassIdx != SC_DEST )
		return FALSE;
	
	theSrcCard = stackTopCard( theSrcStack );
	theDestCard = stackTopCard( theDestStack );

	if( cardSuit( theSrcCard ) != theDestStack->st_Suit )
		return FALSE;
	
	if( !theDestCard &&( cardRank( theSrcCard ) == ACE ))
		return TRUE;
	
	if( cardRank( theSrcCard ) != cardRank( theDestCard )+1 )
		return FALSE;
	
	return TRUE;
}

int PredAlt( Stack *theSrcStack, Stack *theDestStack )
{
	Card theSrcCard, theDestCard;
	
	if( theDestStack->st_ClassIdx != SC_TEMP )
		return FALSE;
	
	theDestCard = stackTopCard( theDestStack );
	
	if( !theDestCard )
		return ( theSrcStack->st_ClassIdx != SC_TEMP )?	TRUE :
			stackEntries( theSrcStack )-1;
	
	theSrcCard = stackTopCard( theSrcStack );
	
	if( !SuitMatrix[cardSuit( theSrcCard )][cardSuit( theDestCard )] )
		return FALSE;
	
	if( cardRank( theSrcCard ) != cardRank( theDestCard )-1 )
		return FALSE;
	
	return TRUE;
}

int PredAltUniq( Stack *theSrcStack, Stack *theDestStack )
{
	Card theSrcCard, theDestCard;
	
	if( theDestStack->st_ClassIdx != SC_TEMP )
		return FALSE;
	
	theDestCard = stackTopCard( theDestStack );
	
	if( !theDestCard )
	{
		if( !EmptyStackTried )
		{
			EmptyStackTried = TRUE;
			return ( theSrcStack->st_ClassIdx != SC_TEMP )?	TRUE :
				stackEntries( theSrcStack )-1;
		}
		return FALSE;
	}
	
	theSrcCard = stackTopCard( theSrcStack );
	
	if( !SuitMatrix[cardSuit( theSrcCard )][cardSuit( theDestCard )] )
		return FALSE;
	
	if( cardRank( theSrcCard ) != cardRank( theDestCard )-1 )
		return FALSE;
	
	return TRUE;
}

int UnidirSame( Stack *theSrcStack, Stack *theDestStack )
{
	Card theSrcCard, theDestCard, theUnderCard;
	long Difference;
	
	if( theDestStack->st_Owner != (1-PlNo))
		return FALSE;
	
	if(( theDestStack->st_ClassIdx != SC_SOURCE )&&
	   ( theDestStack->st_ClassIdx != SC_DISCARD ))
		return FALSE;
	
	theDestCard = stackTopCard( theDestStack );
	if( !theDestCard || !cardFaceUp( theDestCard ))
		return FALSE;
	
	theSrcCard = stackTopCard( theSrcStack );
	if( cardSuit( theSrcCard ) != cardSuit( theDestCard ))
		return FALSE;
	
	if( stackEntries( theDestStack ) > 1 )
	{
		theUnderCard = stackGetCard( theDestStack, 1 );
		if(( cardFaceUp( theUnderCard ))&&
		   ( cardRank( theSrcCard ) == cardRank( theUnderCard ))&&
		   ( cardSuit( theSrcCard ) == cardSuit( theUnderCard )))
			return FALSE;
	}
	
	Difference = cardRank( theSrcCard ) - cardRank( theDestCard );
	if( Difference*Difference != 1 )
		return FALSE;
	
	return TRUE;
}

int ValidStack( Stack *theStack )
{
	if( !theStack )
		return FALSE;
	
	if( theStack->st_ClassIdx == SC_DEST )
		return FALSE;
	
	if( theStack->st_Owner == ( 1 - PlNo ))
		return FALSE;
	
	return TRUE;
}

int EndingMove( Stack *theSourceStack, Stack *theDestStack )
{
	if( theSourceStack != MainSource[PlNo] )
		return FALSE;
	
	if( theDestStack != Discard[PlNo] )
		return FALSE;
	
	return TRUE;
}
