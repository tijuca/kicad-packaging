/**********************************************************/
/*	EECLASS.CPP											  */
/* fonctions relatives aux classes definies dans EESCHEMA */
/**********************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"


/************************************************************/
void SetStructFather(EDA_BaseStruct * Struct, BASE_SCREEN * Screen)
/************************************************************/
{
	switch( Struct->m_StructType )
		{
		case DRAW_POLYLINE_STRUCT_TYPE:
		case DRAW_JUNCTION_STRUCT_TYPE:
		case DRAW_TEXT_STRUCT_TYPE:
		case DRAW_LABEL_STRUCT_TYPE:
		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
		case DRAW_LIB_ITEM_STRUCT_TYPE:
		case DRAW_SEGMENT_STRUCT_TYPE:
		case DRAW_BUSENTRY_STRUCT_TYPE:
		case DRAW_SHEET_STRUCT_TYPE:
		case DRAW_MARKER_STRUCT_TYPE:
		case DRAW_NOCONNECT_STRUCT_TYPE:
			Struct->m_Parent = Screen;
			break;

		case DRAW_SHEETLABEL_STRUCT_TYPE:
		case DRAW_PICK_ITEM_STRUCT_TYPE:
			break;

		default:
			break;
		}
}

/*************************************************************/
void EDA_BaseStruct::Place(WinEDA_DrawFrame * frame, wxDC * DC)
/*************************************************************/
{
	if( m_Flags & IS_NEW)
		{
		Pnext = frame->m_CurrentScreen->EEDrawList;
		frame->m_CurrentScreen->EEDrawList = this;
		g_ItemToRepeat = this;
		}

	m_Flags = 0;
	SetFlagModify(frame->GetScreen());
	frame->GetScreen()->m_CurrentItem = NULL;
	frame->GetScreen()->ManageCurseur = NULL;
	frame->GetScreen()->ForceCloseManageCurseur = NULL;

	frame->GetScreen()->CursorOff(frame->DrawPanel, DC);	// Erase schematic cursor
	RedrawOneStruct(frame->DrawPanel, DC, this, GR_DEFAULT_DRAWMODE);
	frame->GetScreen()->CursorOn(frame->DrawPanel, DC);	// Display schematic cursor
}


	/**************************************************/
	/* Class SCH_SCREEN: classe de gestion d'un affichage pour schematique */
	/***************************************************/
static int table_zoom[] = {1,2,4,8,16,32,64,128, 0}; /* Valeurs standards du zoom */

/* Constructeur de SCREEN */
SCH_SCREEN::SCH_SCREEN(EDA_BaseStruct * parent, WinEDA_DrawFrame * frame_source, int idtype):
		BASE_SCREEN(parent, frame_source, idtype)
{
	m_RootSheet = NULL;
	m_Zoom = 32;
	m_Grid = wxSize(50,50);			/* pas de la grille */
	SetZoomList(table_zoom);
	SetGridList(g_GridList);
}

/****************************/
SCH_SCREEN::~SCH_SCREEN(void)
/****************************/
{
	ClearDrawList();
}

/*************************************/
SCH_SCREEN * SCH_SCREEN::GenCopy(void)
/*************************************/
{
// TODO
	return NULL;
}

/***********************************/
void SCH_SCREEN::ClearDrawList(void)
/***********************************/
/* Routine to clear EESchema drawing list of a screen.
*/
{
EDA_BaseStruct *DrawStruct;

	while (EEDrawList != NULL)
	{
		DrawStruct = EEDrawList;
		EEDrawList = EEDrawList->Pnext;
		delete DrawStruct;
	}
}

