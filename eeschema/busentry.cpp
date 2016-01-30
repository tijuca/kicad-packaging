/*************************************/
/* Modules de creations de Bus Entry */
/*************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
#include "id.h"

#include "protos.h"


/* Routines Locales */

/* Variables locales */
static int LastShape = '\\';
static wxPoint ItemInitialPosition;

/**************************************************************/
static void ExitBusEntry( WinEDA_DrawFrame * frame, wxDC * DC )
/**************************************************************/
/* Routine de sortie des menus de trace */
{
DrawBusEntryStruct *BusEntry =
	(DrawBusEntryStruct *)frame->GetScreen()->m_CurrentItem;

	if( BusEntry)  /* trace en cours */
		{
		RedrawOneStruct(frame->DrawPanel, DC, BusEntry, g_XorMode);
		if( BusEntry->m_Flags & IS_NEW )
			{
			delete BusEntry;
			frame->GetScreen()->m_CurrentItem = NULL;
			}
		else
			{
			BusEntry->m_Pos = ItemInitialPosition;
			RedrawOneStruct(frame->DrawPanel, DC, BusEntry, GR_DEFAULT_DRAWMODE);
			BusEntry->m_Flags = 0;
			}
		}

	g_ItemToRepeat = NULL;
	frame->GetScreen()->ManageCurseur = NULL;
	frame->GetScreen()->ForceCloseManageCurseur = NULL;
}

/************************************************************************/
static void ShowWhileMoving(WinEDA_DrawPanel * panel, wxDC * DC, bool erase)
/************************************************************************/
/*  Dessin du Segment "BusEntry" lors des deplacements du curseur
*/
{
BASE_SCREEN * screen = panel->m_Parent->GetScreen();
DrawBusEntryStruct *BusEntry = (DrawBusEntryStruct *) screen->m_CurrentItem;
	
	if( BusEntry == NULL ) return;

	/* effacement apres deplacement curseur */
	if( erase )
		RedrawOneStruct(panel, DC, BusEntry, g_XorMode);

	/* Reaffichage au bon endroit */
	BusEntry->m_Pos = screen->m_Curseur;
	RedrawOneStruct(panel, DC, BusEntry, g_XorMode);
}

/**********************************************************************************/
DrawBusEntryStruct * WinEDA_SchematicFrame::CreateBusEntry(wxDC * DC, int entry_type)
/**********************************************************************************/
/* Create a new bus entry, and prepare moving function (for later place it)
*/
{
DrawBusEntryStruct * BusEntry = new DrawBusEntryStruct(GetScreen()->m_Curseur,
									LastShape , entry_type);
	BusEntry->m_Flags = IS_NEW;

	GetScreen()->CursorOff(DrawPanel, DC);	// Erase schematic cursor
	RedrawOneStruct(DrawPanel, DC, BusEntry, g_XorMode);
	GetScreen()->CursorOn(DrawPanel, DC);	// Display schematic cursor

	SetFlagModify(GetScreen());

	StartMoveBusEntry(BusEntry, DC);
	return BusEntry;
}


/**************************************************************************/
void WinEDA_SchematicFrame::StartMoveBusEntry(DrawBusEntryStruct * BusEntry,
				wxDC * DC)
/**************************************************************************/
{
	if ( BusEntry == NULL ) return;

	BusEntry->m_Flags |= IS_MOVED;

	ItemInitialPosition = BusEntry->m_Pos;

 	GetScreen()->CursorOff(DrawPanel, DC);
 	GetScreen()->m_Curseur = ItemInitialPosition;
 	DrawPanel->MouseToCursorSchema();
 
	GetScreen()->m_CurrentItem = BusEntry;
	GetScreen()->ManageCurseur = ShowWhileMoving;
	GetScreen()->ForceCloseManageCurseur = ExitBusEntry;
 
 	GetScreen()->CursorOn(DrawPanel, DC);
}


/************************************************************/
void WinEDA_SchematicFrame::SetBusEntryShape(wxDC * DC,
				DrawBusEntryStruct *BusEntry, int entry_shape)
/************************************************************/
{
	if ( BusEntry == NULL ) return;

	if ( GetScreen()->m_CurrentItem->m_StructType != DRAW_BUSENTRY_STRUCT_TYPE )
		{
		DisplayError(this, wxT("SetBusEntryType: Bad StructType") );
		return;
		}

	RedrawOneStruct(DrawPanel, DC, BusEntry, g_XorMode);

	switch( entry_shape )
		{
		case '\\' :
			LastShape = '\\';
			BusEntry->m_Size.y = 100;
			break;

		case '/' :
			LastShape = '/';
			BusEntry->m_Size.y = -100;
			break;
		}

	TestDanglingEnds(GetScreen()->EEDrawList, NULL);
	RedrawOneStruct(DrawPanel, DC, BusEntry, g_XorMode);
	SetFlagModify(GetScreen());
}


/************************************************************************/
int WinEDA_SchematicFrame::GetBusEntryShape( DrawBusEntryStruct *BusEntry)
/************************************************************************/
{
int entry_shape = '\\';

	if ( BusEntry->m_Size.y < 0 ) entry_shape = '/';
	return entry_shape;
}



