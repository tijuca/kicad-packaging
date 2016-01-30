	/*********************************/
	/* Module de nettoyage du schema */
	/*********************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
#include "netlist.h"
#include "macros.h"
#include "protos.h"


/* Routines locales */
static int TstAlignSegment(EDA_DrawLineStruct* RefSegm, EDA_DrawLineStruct* TstSegm);

/* Variables definies et initialisees dans netlist.cc */

/* Variable locales */


/*******************************/
void SchematicCleanUp(wxDC * DC)
/*******************************/
/* Routine de nettoyage:
	- regroupe les segments de fils (ou de bus) alignes en 1 seul segment
	- Detecte les objets identiques superposes
*/
{
SCH_SCREEN * screen;
EDA_BaseStruct *DrawList, * TstDrawList;
int flag;


	screen = ScreenSch;
	for ( ; screen != NULL; screen = (SCH_SCREEN*)screen->Pnext )
	{
		DrawList = screen->EEDrawList;
		while ( DrawList )
		{
			if( DrawList->m_StructType == DRAW_SEGMENT_STRUCT_TYPE )
			{
				TstDrawList = DrawList->Pnext;
				while ( TstDrawList )
				{
					if( TstDrawList->m_StructType == DRAW_SEGMENT_STRUCT_TYPE )
					{
						flag = TstAlignSegment( (EDA_DrawLineStruct*)DrawList,
											(EDA_DrawLineStruct*)TstDrawList);
						if (flag )	/* Suppression de TstSegm */
						{
							/* keep the bits set in .m_Flags, because the deleted segment can be flagged */
							DrawList->m_Flags |= TstDrawList->m_Flags;
							EraseStruct(TstDrawList, screen);
							screen->SetRefreshReq();
							TstDrawList = screen->EEDrawList;
						}
						else TstDrawList = TstDrawList->Pnext;
					}
					else TstDrawList = TstDrawList->Pnext;
				}
			}
			DrawList = DrawList->Pnext;
		}
		EDA_Appl->SchematicFrame->TestDanglingEnds(screen->EEDrawList, DC);
	}
}


/***********************************************/
void BreakSegmentOnJunction( BASE_SCREEN * Screen )
/************************************************/
/* Routine creant des debuts / fin de segment (BUS ou WIRES) sur les jonctions
et les raccords
	Si Screen = NULL: traitement de la hierarchie complete
*/
{
BASE_SCREEN * screen = Screen;
EDA_BaseStruct *DrawList;

	if( screen == NULL ) screen = ScreenSch;

	for ( ; screen != NULL; screen = (BASE_SCREEN*)screen->Pnext )
		{
		DrawList = screen->EEDrawList;
		while ( DrawList )
			{
			switch( DrawList->m_StructType )
				{
				case DRAW_JUNCTION_STRUCT_TYPE :
					#undef STRUCT
					#define STRUCT ((DrawJunctionStruct*)DrawList)
					BreakSegment(screen, STRUCT->m_Pos);
					break;

				case DRAW_BUSENTRY_STRUCT_TYPE :
					#undef STRUCT
					#define STRUCT ((DrawBusEntryStruct*)DrawList)
					BreakSegment(screen, STRUCT->m_Pos);
					BreakSegment(screen, STRUCT->m_End());
					break;

				case DRAW_SEGMENT_STRUCT_TYPE :
				case DRAW_NOCONNECT_STRUCT_TYPE :
				case DRAW_LABEL_STRUCT_TYPE :
				case DRAW_GLOBAL_LABEL_STRUCT_TYPE :
				case DRAW_LIB_ITEM_STRUCT_TYPE :
				case DRAW_PICK_ITEM_STRUCT_TYPE :
				case DRAW_POLYLINE_STRUCT_TYPE :
				case DRAW_MARKER_STRUCT_TYPE :
				case DRAW_TEXT_STRUCT_TYPE :
				case DRAW_SHEET_STRUCT_TYPE :
				case DRAW_SHEETLABEL_STRUCT_TYPE :
					break;

				default :
					break;
				}
			DrawList = DrawList->Pnext;
			}
		}
}


/*********************************************************/
void BreakSegment(BASE_SCREEN * screen, wxPoint breakpoint)
/*********************************************************/
/* Coupe un segment ( BUS, WIRE ) en 2 au point breakpoint,
	- si ce point est sur le segment
	- extremites non comprises
*/
{
EDA_BaseStruct *DrawList;
EDA_DrawLineStruct * segment, * NewSegment;
int ox, oy, fx, fy;

	DrawList = screen->EEDrawList;
	while ( DrawList )
		{
		switch( DrawList->m_StructType )
			{
			case DRAW_SEGMENT_STRUCT_TYPE :
				segment = (EDA_DrawLineStruct*)DrawList;
				ox = segment->m_Start.x; oy = segment->m_Start.y;
				fx = segment->m_End.x; fy = segment->m_End.y;
				if( distance( fx - ox, fy - oy, breakpoint.x - ox, breakpoint.y - oy, 0 ) == 0 )
					break;
				/* Segment connecte: doit etre coupe en 2 si px,py n'est
					pas une extremite */
				if( (ox == breakpoint.x) && (oy == breakpoint.y ) ) break;
				if( (fx == breakpoint.x) && (fy == breakpoint.y ) ) break;
				/* Ici il faut couper le segment en 2 */
				NewSegment = segment->GenCopy();
				NewSegment->m_Start = breakpoint;
				segment->m_End = NewSegment->m_Start;
				NewSegment->Pnext = segment->Pnext;
				segment->Pnext = NewSegment;
				DrawList = NewSegment;
				break;

			case DRAW_JUNCTION_STRUCT_TYPE :
			case DRAW_BUSENTRY_STRUCT_TYPE :
			case DRAW_NOCONNECT_STRUCT_TYPE :
			case DRAW_LABEL_STRUCT_TYPE :
			case DRAW_GLOBAL_LABEL_STRUCT_TYPE :
			case DRAW_LIB_ITEM_STRUCT_TYPE :
			case DRAW_PICK_ITEM_STRUCT_TYPE :
			case DRAW_POLYLINE_STRUCT_TYPE :
			case DRAW_MARKER_STRUCT_TYPE :
			case DRAW_TEXT_STRUCT_TYPE :
			case DRAW_SHEET_STRUCT_TYPE :
			case DRAW_SHEETLABEL_STRUCT_TYPE :
				break;

			default :
				break;
			}
		DrawList = DrawList->Pnext;
		}
}



/***********************************************************/
static int TstAlignSegment( EDA_DrawLineStruct* RefSegm, 
							EDA_DrawLineStruct* TstSegm)
/***********************************************************/

/* Search if the 2 segments RefSegm and TstSegm are on a line.
	Retourn 0 if no
		1 if yes, and RefSegm is modified to be the equivalent segment
*/
{
	if( RefSegm == TstSegm ) return(0);
	if( RefSegm->m_Layer != TstSegm->m_Layer ) return(0);
	
	// search for a common end, ande modify coordinates to ensure RefSegm->m_End == TstSegm->m_Start
	if ( RefSegm->m_Start == TstSegm->m_Start )
	{
		if ( RefSegm->m_End == TstSegm->m_End )			// trivial case: RefSegm and TstSegm are identical
			return 1;
		EXCHG(RefSegm->m_Start, RefSegm->m_End);	// at this point, RefSegm->m_End == TstSegm->m_Start
	}
	else if ( RefSegm->m_Start == TstSegm->m_End )
		{
			EXCHG(RefSegm->m_Start, RefSegm->m_End);
			EXCHG(TstSegm->m_Start, TstSegm->m_End);	// at this point, RefSegm->m_End == TstSegm->m_Start
		}
	else if ( RefSegm->m_End == TstSegm->m_End )
	{
			EXCHG(TstSegm->m_Start, TstSegm->m_End);	// at this point, RefSegm->m_End == TstSegm->m_Start
	}
	else if ( RefSegm->m_End != TstSegm->m_Start )		// No common end point, segments cannot be merged
			return 0;

	/* Test alignment: */
	if ( RefSegm->m_Start.y == RefSegm->m_End.y )		// Horizontal segment
	{
		if ( TstSegm->m_Start.y == TstSegm->m_End.y )
		{
			RefSegm->m_End = TstSegm->m_End;
			return 1;
		}
	}
	
	else if ( RefSegm->m_Start.x == RefSegm->m_End.x )	// Vertical segment
	{
		if ( TstSegm->m_Start.x == TstSegm->m_End.x )
		{
			RefSegm->m_End = TstSegm->m_End;
			return 1;
		}
	}
	
	else
	{
		if (atan2((RefSegm->m_Start.x - RefSegm->m_End.x), RefSegm->m_Start.y - RefSegm->m_End.y) ==
				atan2((TstSegm->m_Start.x - TstSegm->m_End.x), TstSegm->m_Start.y - TstSegm->m_End.y) )
		{
			RefSegm->m_End = TstSegm->m_End;
			return 1;
		}
	}
	
	return(0);
}

