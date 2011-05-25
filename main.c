/* $Id: main.c,v 39.1 1995/04/25 01:40:05 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 * 
 * $Log: main.c,v $
 * Revision 39.1  1995/04/25  01:40:05  mbayne
 * Initial revision.
 *
 */

#include <exec/memory.h>
#include <graphics/modeid.h>
#include <devices/inputevent.h>
#include <stdlib.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#define Move gfxMove
#include <proto/graphics.h>
#undef Move
#include <proto/dos.h>

#include "resource.h"
#include "menus.h"
#include "card.h"
#include "deck.h"
#include "board.h"
#include "moveinfo.h"
#include "opponent.h"

extern __far long RangeSeed;

int func( void );

struct NewMenu BeschNewMenu[] = {
	NM_TITLE, (STRPTR)"Game", NULL, 0, NULL, NULL,
	NM_ITEM, (STRPTR)"About", (STRPTR)"?", 0, 0L, (APTR)BeschABOUT,
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"New", (STRPTR)"N", 0, 0L, (APTR)BeschNEW,
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Quit", (STRPTR)"Q", 0, 0L, (APTR)BeschQUIT,
	NM_END, NULL, NULL, 0, 0L, NULL
	};

long IDCMP_Flags = IDCMP_REFRESHWINDOW|IDCMP_MOUSEBUTTONS|IDCMP_MENUPICK;
long Width, Height, gameInProgress;

struct VisualInfo *VisInf;
struct Screen *Scr;
struct Window *Win;
struct Menu *Menus;

void appFreeVisualInfo( void *object )
{ FreeVisualInfo( object ); }

void appClearMenuStrip( void *object )
{ ClearMenuStrip( object ); }

void appFreeMenus( void *object )
{ FreeMenus( object ); }

void appCloseWindow( void *object )
{ CloseWindow( object ); }

void appCloseScreen( void *object )
{ CloseScreen( object ); }

void appInfo( STRPTR InfoStr )
{
	struct EasyStruct InfoReq = {
		sizeof( struct EasyStruct ), 0, "Information", 0L, "Ok" };
	
	InfoReq.es_TextFormat = InfoStr;
	EasyRequestArgs( Win, &InfoReq, 0L, 0L );
}

void appExit( STRPTR OsError )
{
	int ExitVal = 0;

	if( OsError )
	{
		FPrintf( Output(), "Besch failed: %s\n" );
		ExitVal = 1;
	}

	resourceClassDestruct( ExitVal );
}

void appStopGame( void )
{
	gameInProgress = FALSE;
}

void appStartGame( void )
{
	gameInProgress = TRUE;
}

int appHandleIDCMP( void )
{
	struct IntuiMessage *m, BeschMsg;
	static Point thePreviousPoint = { ~0, ~0 };
	Point thePoint;
	long running = TRUE;
	struct MenuItem *n;
	int (*func)();
	Point aPoint;
	
	while( m = GT_GetIMsg( Win->UserPort ))
	{
		CopyMem( m, &BeschMsg, sizeof( struct IntuiMessage ));
		GT_ReplyIMsg( m );
		switch ( BeschMsg.Class )
		{
		case IDCMP_REFRESHWINDOW:
			GT_BeginRefresh( Win );
			GT_EndRefresh( Win, TRUE );
			break;
		case IDCMP_MENUPICK:
			while( BeschMsg.Code != MENUNULL )
			{
				n = ItemAddress( Menus, BeschMsg.Code );
				func = (void *)(GTMENUITEM_USERDATA( n ));
				running = func();
				BeschMsg.Code = n->NextSelect;
			}
			break;
		case IDCMP_MOUSEBUTTONS:
			if( BeschMsg.Code & IECODE_LBUTTON )
			{
				if( !gameInProgress )
					break;
				aPoint.x = BeschMsg.MouseX;
				aPoint.y = BeschMsg.MouseY;
				if( BeschMsg.Code & IECODE_UP_PREFIX )
				{
					boardHandleMouseUp( &aPoint, Win->RPort );
					IDCMP_Flags &= ~IDCMP_MOUSEMOVE;
				}
				else
				{
					if( boardHandleMouseDown( &aPoint, Win->RPort ))
					{
						IDCMP_Flags |= IDCMP_MOUSEMOVE;
						thePoint = aPoint;
						thePreviousPoint.x = ~0;
					}
				}
				ModifyIDCMP( Win, IDCMP_Flags );
			}
			break;
		case IDCMP_MOUSEMOVE:
			thePoint.x = BeschMsg.MouseX;
			thePoint.y = BeschMsg.MouseY;
			break;
		default:
			break;

		}
	}

	if( IDCMP_Flags & IDCMP_MOUSEMOVE )
		boardHandleMouseMove( &thePoint, Win->RPort );
	
	return running;
}

void appInit( void )
{
	unsigned long Seconds, Microseconds;

	CurrentTime( &Seconds, &Microseconds );
	RangeSeed = ( long )( Seconds * Microseconds );

	Scr = OpenScreenTags( 0L,
						 SA_LikeWorkbench, TRUE,
						 SA_Depth, 3,
						 SA_Title, "Besch ©1995 Michael D. Bayne",
						 SA_Type, CUSTOMSCREEN,
						 TAG_DONE );
	if( Scr )
		resourceAdd( Scr, appCloseScreen );
	else
		appExit( "OpenScreenTags Failed" );

#ifdef GREEN_BG
	SetRGB4( &Scr->ViewPort, 0, 6, 8, 5 );
#endif
	SetRGB4( &Scr->ViewPort, 3, 0, 0, 0 );
	SetRGB4( &Scr->ViewPort, 4, 15, 0, 0 );
	SetRGB4( &Scr->ViewPort, 5, 6, 8, 5 );
	SetRGB4( &Scr->ViewPort, 6, 15, 0, 0 );
	SetRGB4( &Scr->ViewPort, 7, 3, 3, 10 );

	VisInf = GetVisualInfo( Scr, TAG_DONE );

	if( VisInf )
		resourceAdd( VisInf, appFreeVisualInfo );
	else
		appExit( "GetVisualInfo Failed" );
	
	Win = OpenWindowTags( 0L,
						 WA_Left, 0L,
						 WA_Top, Scr->BarHeight + 1,
						 WA_Height, Scr->Height - Scr->BarHeight - 1,
						 WA_Width, Scr->Width,
						 WA_ReportMouse, TRUE,
						 WA_Borderless, TRUE,
						 WA_Activate, TRUE,
						 WA_NewLookMenus, TRUE,
						 WA_IDCMP, IDCMP_Flags,
						 WA_CustomScreen, Scr,
						 TAG_DONE );
	if( Win )
		resourceAdd( Win, appCloseWindow );
	else
		appExit( "OpenWindowTags Failed" );

	Width = Win->Width;
	Height = Win->Height;

	SetRast( Win->RPort, 5 );

	Menus = CreateMenus( BeschNewMenu, GTMN_FrontPen, 1L, TAG_DONE );

	if( Menus )
	{
		resourceAdd( Menus, appFreeMenus );
		resourceAdd( Win, appClearMenuStrip );
	}
	else
		appExit( "CreateMenus Failed" );
	
	LayoutMenus( Menus, VisInf, TAG_DONE );
	SetMenuStrip( Win, Menus );
	GT_RefreshWindow( Win, 0L );
}

void seedrng( void )
{
	ULONG Secs, Micros;

	CurrentTime( &Secs, &Micros );

	srand( Secs*1000 + Micros );
}

void main( int argc, char *argv[] )
{
	long RetVal, Sigs;
	
	if( argc == 2 )
	{
		HypothesisDepth = atoi( argv[1] );
		HypothesisDepth = max( 4, HypothesisDepth );
		FPrintf( Output(), "Running opponent with search depth %ld.\n",
				HypothesisDepth );
	}

	resourceClassInit();
	appInit();
	moveClassInit();
	moveInfoClassInit();
	opponentClassInit();
	cardClassInit( Win->RPort );
	boardClassInit( Win );
	boardClassReset( Win );
	appStartGame();
	
	do
	{
		Sigs = Wait( 1L << Win->UserPort->mp_SigBit | SIGBREAKF_CTRL_C );

		if( Sigs & 1L << Win->UserPort->mp_SigBit )
			RetVal = appHandleIDCMP();

		if( Sigs & SIGBREAKF_CTRL_C )
			RetVal = FALSE;
	}
	while( RetVal );

	appExit( 0L );
}
