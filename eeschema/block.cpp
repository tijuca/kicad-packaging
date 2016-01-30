/****************************************************/
/*	BLOCK.CPP										*/
/* Gestion des Operations sur Blocks et Effacements */
/****************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"

/* Variables Locales */

/* Fonctions exportees */

/* Fonctions Locales */
static EDA_BaseStruct * DuplicateStruct(EDA_BaseStruct *DrawStruct);
static bool CopyStruct(WinEDA_DrawPanel * panel, wxDC * DC, BASE_SCREEN * screen,
			EDA_BaseStruct *DrawStruct);
static void CollectStructsToDrag(BASE_SCREEN * screen);
static void AddPickedItem(BASE_SCREEN * screen, int px, int py );
static LibEDA_BaseStruct * GetNextPinPosition( EDA_SchComponentStruct * DrawLibItem,
												int * px, int * py );
static void DrawMovingBlockOutlines(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);
static EDA_BaseStruct * SaveStructListForPaste(EDA_BaseStruct *DrawStruct);

/*************************************************************************/
int WinEDA_SchematicFrame::ReturnBlockCommand(int key)
/*************************************************************************/
/* Return the block command (BLOCK_MOVE, BLOCK_COPY...) corresponding to
	the key (ALT, SHIFT ALT ..)
*/
{
int cmd;

	switch ( key )
		{
		default:
			cmd = key & 0x255;
		break;

		case 0:
			cmd = BLOCK_MOVE;
			break;

		case GR_KB_ALT:
		case GR_KB_SHIFT:
			cmd = BLOCK_COPY;
			break;

		case GR_KB_CTRL:
			cmd = BLOCK_DRAG;
			break;

		case GR_KB_SHIFTCTRL:
			cmd = BLOCK_DELETE;
			break;

		case MOUSE_MIDDLE:
			cmd = BLOCK_ZOOM;
			break;
		}

	return cmd;
}

/*************************************************/
void WinEDA_SchematicFrame::InitBlockPasteInfos()
/*************************************************/
/* Init the parameters used by the block paste command
*/
{
	GetScreen()->BlockLocate.m_BlockDrawStruct = g_BlockSaveDataList;
	GetScreen()->ManageCurseur = DrawMovingBlockOutlines;
}



/******************************************************/
void WinEDA_SchematicFrame::HandleBlockPlace(wxDC * DC)
/******************************************************/
/* Routine to handle the BLOCK PLACE commande
	Last routine for block operation for:
	- block move & drag
	- block copie & paste
*/
{
bool err = FALSE;

	if(GetScreen()->ManageCurseur == NULL)
		{
		err = TRUE;
		DisplayError(this, wxT("HandleBlockPLace() : ManageCurseur = NULL") );
		}

	if(GetScreen()->BlockLocate.m_BlockDrawStruct == NULL)
		{
		wxString msg;
		err = TRUE;
		msg.Printf( wxT("HandleBlockPLace() : m_BlockDrawStruct = NULL (cmd %d, state %d)"),
			GetScreen()->BlockLocate.m_Command, GetScreen()->BlockLocate.m_State);
		DisplayError(this, msg );
		}

	GetScreen()->BlockLocate.m_State = STATE_BLOCK_STOP;

	switch(GetScreen()->BlockLocate.m_Command )
		{
		case  BLOCK_IDLE:
			err = TRUE;
			break;
		case BLOCK_DRAG: /* Drag */
		case BLOCK_MOVE: /* Move */
			if ( GetScreen()->ManageCurseur )
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
			MoveStruct(DrawPanel, DC, GetScreen()->BlockLocate.m_BlockDrawStruct);
			GetScreen()->BlockLocate.m_BlockDrawStruct = NULL;
			break;

		case BLOCK_COPY: /* Copy */
		case BLOCK_PRESELECT_MOVE: /* Move with preselection list*/
			if ( GetScreen()->ManageCurseur )
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
			CopyStruct(DrawPanel, DC, GetScreen(), GetScreen()->BlockLocate.m_BlockDrawStruct);
			GetScreen()->BlockLocate.m_BlockDrawStruct = NULL;
			break;

		case BLOCK_PASTE: /* Paste (recopie du dernier bloc sauve */
			if ( GetScreen()->ManageCurseur )
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
			PasteStruct(DC);
			GetScreen()->BlockLocate.m_BlockDrawStruct = NULL;
			break;

		case BLOCK_ZOOM:	// Handled by HandleBlockEnd()
		case BLOCK_DELETE:
		case BLOCK_SAVE:
		case BLOCK_ROTATE:
		case BLOCK_INVERT:
		case BLOCK_ABORT:
		case BLOCK_SELECT_ITEMS_ONLY:
			break;
		}

	SetFlagModify(GetScreen());

	/* clear struct.m_Flags  */
	EDA_BaseStruct * Struct;
	for(Struct = GetScreen()->EEDrawList; Struct != NULL; Struct=Struct->Pnext)
		Struct->m_Flags = 0;

	GetScreen()->ManageCurseur = NULL;
	GetScreen()->ForceCloseManageCurseur = NULL;
	GetScreen()->BlockLocate.m_Flags = 0;
	GetScreen()->BlockLocate.m_State = STATE_NO_BLOCK;
	GetScreen()->BlockLocate.m_Command =  BLOCK_IDLE;
	GetScreen()->m_CurrentItem = NULL;

	TestDanglingEnds(GetScreen()->EEDrawList, DC);

	if ( GetScreen()->BlockLocate.m_BlockDrawStruct )
		{
		DisplayError(this, wxT("HandleBlockPLace() error: DrawStruct != Null") );
		GetScreen()->BlockLocate.m_BlockDrawStruct = NULL;
		}

	SetToolID(m_ID_current_state, DrawPanel->m_PanelDefaultCursor, wxEmptyString );
}

/****************************************************/
int WinEDA_SchematicFrame::HandleBlockEnd(wxDC * DC)
/****************************************************/
/* Routine de gestion de la commande BLOCK END
	retourne :
	0 si aucun composant selectionne
	1 sinon
	-1 si commande terminée et composants trouvés (block delete, block save)
*/
{
int ii = 0;
bool zoom_command = FALSE;

	if ( GetScreen()->BlockLocate.m_BlockDrawStruct )
	{
		BlockState state = GetScreen()->BlockLocate.m_State;
		CmdBlockType command = GetScreen()->BlockLocate.m_Command;
		if ( GetScreen()->ForceCloseManageCurseur )
			GetScreen()->ForceCloseManageCurseur(this, DC);
		GetScreen()->BlockLocate.m_State =  state;
		GetScreen()->BlockLocate.m_Command = command;
		GetScreen()->ManageCurseur = DrawAndSizingBlockOutlines;
		GetScreen()->ForceCloseManageCurseur = AbortBlockCurrentCommand;
		GetScreen()->m_Curseur.x = GetScreen()->BlockLocate.GetRight();
		GetScreen()->m_Curseur.y = GetScreen()->BlockLocate.GetBottom();
		if ( GetScreen()->BlockLocate.m_Command != BLOCK_ABORT )
			DrawPanel->MouseToCursorSchema();
	}

	if(GetScreen()->ManageCurseur != NULL)
	  switch( GetScreen()->BlockLocate.m_Command )
		{
		case  BLOCK_IDLE:
			DisplayError(this, wxT("Error in HandleBlockPLace()") );
			break;

		case BLOCK_DRAG: /* Drag */
			BreakSegmentOnJunction(GetScreen() );
		case BLOCK_MOVE: /* Move */
		case BLOCK_COPY: /* Copy */
			GetScreen()->BlockLocate.m_BlockDrawStruct =
				PickStruct(GetScreen()->BlockLocate,
					GetScreen()->EEDrawList, SEARCHALL);
		case BLOCK_PRESELECT_MOVE: /* Move with preselection list*/
			if( GetScreen()->BlockLocate.m_BlockDrawStruct != NULL)
			{
				ii = 1;
				CollectStructsToDrag(GetScreen());
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
				GetScreen()->ManageCurseur = DrawMovingBlockOutlines;
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
				GetScreen()->BlockLocate.m_State = STATE_BLOCK_MOVE;
			}
			else
			{
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
				GetScreen()->ManageCurseur = NULL;
				GetScreen()->ForceCloseManageCurseur = NULL;
			}
			break;

		case BLOCK_DELETE: /* Delete */
			GetScreen()->BlockLocate.m_BlockDrawStruct =
				PickStruct(GetScreen()->BlockLocate,
					GetScreen()->EEDrawList, SEARCHALL);
			DrawAndSizingBlockOutlines(DrawPanel, DC, FALSE);
			if( GetScreen()->BlockLocate.m_BlockDrawStruct != NULL)
			{
				ii = -1;
				DeleteStruct(DrawPanel, DC, GetScreen()->BlockLocate.m_BlockDrawStruct);
				SetFlagModify(GetScreen());
			}
			GetScreen()->BlockLocate.m_BlockDrawStruct = NULL;
			TestDanglingEnds(GetScreen()->EEDrawList, DC);
			break;

		case BLOCK_SAVE: /* Save */
			GetScreen()->BlockLocate.m_BlockDrawStruct =
				PickStruct(GetScreen()->BlockLocate,
					GetScreen()->EEDrawList, SEARCHALL);
			DrawAndSizingBlockOutlines(DrawPanel, DC, FALSE);
			if( GetScreen()->BlockLocate.m_BlockDrawStruct != NULL)
			{
				wxPoint oldpos = GetScreen()->m_Curseur;
				GetScreen()->m_Curseur = wxPoint(0,0);
				EDA_BaseStruct *DrawStructCopy =
					SaveStructListForPaste(GetScreen()->BlockLocate.m_BlockDrawStruct);
				PlaceStruct( GetScreen(), DrawStructCopy);
				GetScreen()->m_Curseur = oldpos;
				ii = -1;
			}
			GetScreen()->BlockLocate.m_BlockDrawStruct = NULL;
			break;

		case BLOCK_PASTE:
			GetScreen()->BlockLocate.m_State = STATE_BLOCK_MOVE;
			break;

		case BLOCK_ROTATE:
		case BLOCK_INVERT: /* pcbnew only! */
			break;

		case BLOCK_ZOOM: /* Window Zoom */
			zoom_command = TRUE;
			break;
		
		case BLOCK_SELECT_ITEMS_ONLY:	/* Not used */
		case BLOCK_ABORT: /* not executed here */
			break;
		}

	if ( GetScreen()->BlockLocate.m_Command  == BLOCK_ABORT )
	{	/* clear struct.m_Flags  */
		EDA_BaseStruct * Struct;
		for(Struct = GetScreen()->EEDrawList; Struct != NULL; Struct=Struct->Pnext)
			Struct->m_Flags = 0;
	}
	
	if ( ii <= 0 )
	{
		GetScreen()->BlockLocate.m_Flags = 0;
		GetScreen()->BlockLocate.m_State = STATE_NO_BLOCK;
		GetScreen()->BlockLocate.m_Command =  BLOCK_IDLE;
		GetScreen()->ManageCurseur = NULL;
		GetScreen()->ForceCloseManageCurseur = NULL;
		GetScreen()->m_CurrentItem = NULL;
		SetToolID(m_ID_current_state, DrawPanel->m_PanelDefaultCursor, wxEmptyString );
	}

	if ( zoom_command)
		Window_Zoom( GetScreen()->BlockLocate);

	return(ii);
}


/***********************************************************************/
void WinEDA_SchematicFrame::HandleBlockEndByPopUp(int Command, wxDC * DC)
/***********************************************************************/
/* Routine de gestion de la commande BLOCK END by PopUp
	Appelee apres HandleBlockEnd.
	A partir de la commande bloc move, peut executer une commande autre que bloc move.
*/
{
int ii = 0;

	if( GetScreen()->BlockLocate.m_Command != BLOCK_MOVE )  return;
	if( Command == BLOCK_MOVE ) return;

	GetScreen()->BlockLocate.m_Command = (CmdBlockType) Command;
	GetScreen()->BlockLocate.SetMessageBlock(this);

	switch( GetScreen()->BlockLocate.m_Command )
		{
		case BLOCK_COPY: /* move to copy */
			GetScreen()->BlockLocate.m_State = STATE_BLOCK_MOVE;
			ii = 1;
			break;

		case BLOCK_DRAG: /* move to Drag */
			/* Effacement de la liste des structures de pointage,
				qui est devenue erronnee */
			if(GetScreen()->ManageCurseur)
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
			if( GetScreen()->BlockLocate.m_BlockDrawStruct )
			{
				if(GetScreen()->BlockLocate.m_BlockDrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
				{	/* Delete the picked wrapper if this is a picked list. */
					DrawPickedStruct * PickedList;
					PickedList = (DrawPickedStruct*)GetScreen()->BlockLocate.m_BlockDrawStruct;
					PickedList->DeleteWrapperList();
				}
				GetScreen()->BlockLocate.m_BlockDrawStruct = NULL;
			}
			BreakSegmentOnJunction(GetScreen() );
			GetScreen()->BlockLocate.m_BlockDrawStruct =
				PickStruct(GetScreen()->BlockLocate,
					GetScreen()->EEDrawList, SEARCHALL);
			if( GetScreen()->BlockLocate.m_BlockDrawStruct != NULL)
			{
				ii = 1;
				CollectStructsToDrag(GetScreen());
				if(GetScreen()->ManageCurseur)
					GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
				GetScreen()->BlockLocate.m_State = STATE_BLOCK_MOVE;
			}
			break;

		case BLOCK_DELETE: /* move to Delete */
			if(GetScreen()->ManageCurseur)
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
			if( GetScreen()->BlockLocate.m_BlockDrawStruct != NULL)
			{
				ii = -1;
				DeleteStruct(DrawPanel, DC, GetScreen()->BlockLocate.m_BlockDrawStruct);
				SetFlagModify(GetScreen());
			}
			TestDanglingEnds(GetScreen()->EEDrawList, DC);
			break;

		case BLOCK_SAVE: /* Save */
			if(GetScreen()->ManageCurseur)
				GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
			if( GetScreen()->BlockLocate.m_BlockDrawStruct != NULL)
				{
				wxPoint oldpos = GetScreen()->m_Curseur;
				GetScreen()->m_Curseur = wxPoint(0,0);
				EDA_BaseStruct *DrawStructCopy =
					SaveStructListForPaste(GetScreen()->BlockLocate.m_BlockDrawStruct);
				PlaceStruct( GetScreen(), DrawStructCopy);
				GetScreen()->m_Curseur = oldpos;
				ii = -1;
				}
			break;

		case BLOCK_ZOOM: /* Window Zoom */
			GetScreen()->ForceCloseManageCurseur(this, DC);
			DrawPanel->SetCursor(DrawPanel->m_PanelCursor = DrawPanel->m_PanelDefaultCursor);
			Window_Zoom( GetScreen()->BlockLocate);
			break;

		default:
			break;
		}

	if ( ii <= 0 )
	{
		GetScreen()->BlockLocate.m_BlockDrawStruct = NULL;
		GetScreen()->BlockLocate.m_Flags = 0;
		GetScreen()->BlockLocate.m_State = STATE_NO_BLOCK;
		GetScreen()->BlockLocate.m_Command =  BLOCK_IDLE;
		GetScreen()->ManageCurseur = NULL;
		GetScreen()->ForceCloseManageCurseur = NULL;
		GetScreen()->m_CurrentItem = NULL;
		SetToolID(m_ID_current_state, DrawPanel->m_PanelDefaultCursor, wxEmptyString );
	}

}


/************************************************************************/
static void DrawMovingBlockOutlines(WinEDA_DrawPanel * panel, wxDC * DC,
	bool erase )
/************************************************************************/
/* Retrace le contour du block de recherche de structures
	L'ensemble du block suit le curseur
*/
{
DrawBlockStruct * PtBlock;
DrawPickedStruct *PickedList;
BASE_SCREEN * screen = panel->m_Parent->GetScreen();

	PtBlock = &panel->GetScreen()->BlockLocate;
	GRSetDrawMode(DC, g_XorMode);

	/* Effacement ancien cadre */
	if( erase && PtBlock->m_BlockDrawStruct)
	{
		PtBlock->Offset(PtBlock->m_MoveVector);
		PtBlock->Draw(panel, DC);
		PtBlock->Offset( -PtBlock->m_MoveVector.x, -PtBlock->m_MoveVector.y);

		/* Effacement ancien affichage */
		if(PtBlock->m_BlockDrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
		{
			PickedList  = (DrawPickedStruct *) PtBlock->m_BlockDrawStruct;
			while (PickedList)
			{
				DrawStructsInGhost(panel, DC, PickedList->m_PickedStruct, PtBlock->m_MoveVector.x, PtBlock->m_MoveVector.y);
				PickedList = (DrawPickedStruct *)PickedList->Pnext;
			}
		}
		else DrawStructsInGhost(panel, DC, PtBlock->m_BlockDrawStruct, PtBlock->m_MoveVector.x, PtBlock->m_MoveVector.y);
	}

	/* Redessin nouvel affichage */

	PtBlock->m_MoveVector.x = screen->m_Curseur.x - PtBlock->m_BlockLastCursorPosition.x;
	PtBlock->m_MoveVector.y = screen->m_Curseur.y - PtBlock->m_BlockLastCursorPosition.y;

	GRSetDrawMode(DC, g_XorMode);
	PtBlock->Offset(PtBlock->m_MoveVector);
	PtBlock->Draw(panel, DC);
	PtBlock->Offset( -PtBlock->m_MoveVector.x, -PtBlock->m_MoveVector.y);

	if(PtBlock->m_BlockDrawStruct )
	{
		if(PtBlock->m_BlockDrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
		{
		PickedList  = (DrawPickedStruct *) PtBlock->m_BlockDrawStruct;
		while (PickedList)
			{
			DrawStructsInGhost(panel, DC, PickedList->m_PickedStruct, PtBlock->m_MoveVector.x, PtBlock->m_MoveVector.y);
			PickedList = (DrawPickedStruct *)PickedList->Pnext;
			}
		}
		else DrawStructsInGhost(panel, DC, PtBlock->m_BlockDrawStruct, PtBlock->m_MoveVector.x, PtBlock->m_MoveVector.y);
	}
}


/*****************************************************************************
* Routine to move an object(s) to a new position.							 *
* If DrawStruct is of type DrawPickedStruct, a list of objects picked is	 *
* assumed, otherwise exactly one structure is assumed been picked.			 *
*****************************************************************************/
bool MoveStruct(WinEDA_DrawPanel * panel, wxDC * DC, EDA_BaseStruct *DrawStruct)
{
DrawPickedStruct *PickedList = NULL;

	if ( !DrawStruct ) return FALSE;

	if(DrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
	{
		if ( DC )
			RedrawStructList(panel, DC, DrawStruct, g_XorMode);
		PlaceStruct(panel->GetScreen(), DrawStruct);	/* Place it in its new position. */
		if ( DC )
			RedrawStructList(panel, DC, DrawStruct, GR_DEFAULT_DRAWMODE);

		/* Free the wrapper DrawPickedStruct chain: */
		PickedList = (DrawPickedStruct *) DrawStruct;
		PickedList->DeleteWrapperList();
	}

	else
	{
		if ( DC )
			RedrawOneStruct(panel, DC, DrawStruct, g_XorMode);
		PlaceStruct(panel->GetScreen(), DrawStruct);		/* Place it in its new position. */
		if ( DC )
			RedrawOneStruct(panel, DC, DrawStruct, GR_DEFAULT_DRAWMODE);
	}
	return TRUE;
}

/*****************************************************************************
* Routine to copy a new entity of an object and reposition it.				 *
* If DrawStruct is of type DrawPickedStruct, a list of objects picked is	 *
* assumed, otherwise exactly one structure is assumed been picked.			 *
*****************************************************************************/
static bool CopyStruct(WinEDA_DrawPanel * panel, wxDC * DC, BASE_SCREEN * screen, EDA_BaseStruct *DrawStruct)
{
bool RetVal;
EDA_BaseStruct *NewDrawStruct;
DrawPickedStruct *PickedItem, *PickedList = NULL;

	if ( !DrawStruct ) return FALSE;

	NewDrawStruct = DuplicateStruct(DrawStruct);
	if ( NewDrawStruct == NULL ) return FALSE;

	if ( (RetVal = PlaceStruct( screen, NewDrawStruct)) == TRUE)
	{
		/* Draw the new structure and chain it in: */
		if (NewDrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
		{
			PickedList = (DrawPickedStruct *) NewDrawStruct;
			while (PickedList)	// Clear annotation for new components
			{
				EDA_BaseStruct * Struct = PickedList->m_PickedStruct;
				if ( Struct->m_StructType == DRAW_LIB_ITEM_STRUCT_TYPE )
				{
					((EDA_SchComponentStruct*)Struct)->m_TimeStamp = GetTimeStamp();
					((EDA_SchComponentStruct*)Struct)->ClearAnnotation();
				}
				PickedList = (DrawPickedStruct *)PickedList->Pnext;
			}
			RedrawStructList(panel, DC, NewDrawStruct, GR_DEFAULT_DRAWMODE);
			PickedList = (DrawPickedStruct *) NewDrawStruct;
			while (PickedList)
			{
				PickedList->m_PickedStruct->Pnext = panel->GetScreen()->EEDrawList;
				screen->EEDrawList = PickedList->m_PickedStruct;
				PickedItem = PickedList;
				PickedList = (DrawPickedStruct *)PickedList->Pnext;
				delete PickedItem;
			}
		}

		else
		{
			switch(NewDrawStruct->m_StructType)
			{
				case DRAW_POLYLINE_STRUCT_TYPE:
				case DRAW_JUNCTION_STRUCT_TYPE:
				case DRAW_SEGMENT_STRUCT_TYPE:
				case DRAW_BUSENTRY_STRUCT_TYPE:
				case DRAW_TEXT_STRUCT_TYPE:
				case DRAW_LABEL_STRUCT_TYPE:
				case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
				case DRAW_SHEETLABEL_STRUCT_TYPE:
				case DRAW_PICK_ITEM_STRUCT_TYPE:
				case DRAW_MARKER_STRUCT_TYPE:
				case DRAW_NOCONNECT_STRUCT_TYPE:
				default:
					break;

				case DRAW_SHEET_STRUCT_TYPE:
					break;

				case DRAW_LIB_ITEM_STRUCT_TYPE:
					((EDA_SchComponentStruct*)NewDrawStruct)->m_TimeStamp = GetTimeStamp();
					((EDA_SchComponentStruct*)NewDrawStruct)->ClearAnnotation();
					break;

			}

			RedrawOneStruct(panel, DC, NewDrawStruct, GR_DEFAULT_DRAWMODE);

			NewDrawStruct->Pnext = screen->EEDrawList;
			screen->EEDrawList = NewDrawStruct;
		}
	}
	else delete NewDrawStruct;

	/* Free the original DrawPickedStruct chain: */
	if (DrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
	{
		PickedList = (DrawPickedStruct *) DrawStruct;
		PickedList->DeleteWrapperList();
	}

	return RetVal;
}

/*********************************************************************************/
void DeleteStruct(WinEDA_DrawPanel * panel, wxDC * DC, EDA_BaseStruct *DrawStruct)
/*********************************************************************************/
/* Routine to delete an object from global drawing object list.
	Object is put in Undelete Stack
*/
{
int i;
EDA_BaseStruct *DrawList = panel->GetScreen()->EEDrawList;
DrawPickedStruct *PickedList = NULL;

	if ( !DrawStruct ) return;

	if (DrawStruct->m_StructType == DRAW_SHEETLABEL_STRUCT_TYPE)
	{	/* Cette stucture est rattachee a une feuille, et n'est pas
		accessible par la liste globale directement */
		WinEDA_SchematicFrame * frame = (WinEDA_SchematicFrame *) panel->m_Parent;
		frame->DeleteSheetLabel(DC, (DrawSheetLabelStruct*) DrawStruct);
		return;
	}

	if (DrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
	{
		/* Unlink all picked structs from current EEDrawList */
		PickedList = (DrawPickedStruct *) DrawStruct;
		while (PickedList)
		{
			if (PickedList->m_PickedStruct == panel->GetScreen()->EEDrawList)
				panel->GetScreen()->EEDrawList = panel->GetScreen()->EEDrawList->Pnext;
			else
			{
				DrawList = panel->GetScreen()->EEDrawList;
				while (DrawList->Pnext)
				{
					if (DrawList->Pnext == PickedList->m_PickedStruct)
					{
						DrawList->Pnext = DrawList->Pnext->Pnext;
						break;
					}
					DrawList = DrawList->Pnext;
				}
			}
			PickedList = (DrawPickedStruct *)PickedList->Pnext;
		}
		RedrawStructList(panel, DC, DrawStruct, g_XorMode);
	}

	else	/* structure classique */
	{
		if (DrawStruct == panel->GetScreen()->EEDrawList)
			panel->GetScreen()->EEDrawList = panel->GetScreen()->EEDrawList->Pnext;
		else
		{
			DrawList = panel->GetScreen()->EEDrawList;
			while (DrawList->Pnext)
			{
				if (DrawList->Pnext == DrawStruct)
				{
					DrawList->Pnext = DrawList->Pnext->Pnext;
					break;
				}
				DrawList = DrawList->Pnext;
			}
		}
		RedrawOneStruct(panel, DC, DrawStruct, g_XorMode);
		/* Unlink the structure */
		DrawStruct->Pnext = DrawStruct->Pback = NULL;	// Only one struct -> no link
	}

	/* Si c'est une SHEET qui doit etre effacee, on doit supprimer la hierarchie
		correspondante
		Si undelete: il faudra recharger la hierarchie correspondante.
	*/
	if (DrawStruct->m_StructType == DRAW_SHEET_STRUCT_TYPE)
	{
		DrawSheetStruct * Sheet = (DrawSheetStruct *) DrawStruct;
		DrawStruct = DuplicateStruct(Sheet);	/* Pour Undelete */
		DeleteSubHierarchy(Sheet);
	}

	if (g_UnDeleteStackPtr >= UNDELETE_STACK_SIZE )
	{
		/* Delete last deleted item, and shift stack. */
		EDA_BaseStruct * item = g_UnDeleteStack[0], * next_item;
		while (item)
		{
			next_item = item->Pnext;
			delete item;
			item = next_item;
		}
		for (i = 0; i < (g_UnDeleteStackPtr-1); i++)
		{
			g_UnDeleteStack[i] = g_UnDeleteStack[i + 1];
		}
		g_UnDeleteStackPtr--;
	}
	g_UnDeleteStack[g_UnDeleteStackPtr++] = DrawStruct;
	DrawStruct->m_Parent = panel->GetScreen();
}

/*****************************************************************/
EDA_BaseStruct * SaveStructListForPaste(EDA_BaseStruct *DrawStruct)
/*****************************************************************/

/* Routine to Save an object from global drawing object list.
	This routine is the same as delete but:
	- the original list is NOT removed.
	- List is saved in g_BlockSaveDataList
*/
{
DrawPickedStruct *PickedList;
EDA_BaseStruct *DrawStructCopy;

	if ( !DrawStruct ) return NULL;

	/* Make a copy of the original picked item. */
	DrawStructCopy = DuplicateStruct(DrawStruct);

	if (DrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
	{
		/* Delete the picked wrapper if this is a picked list. */
		PickedList = (DrawPickedStruct *) DrawStruct;
		PickedList->DeleteWrapperList();
	}

	/* And delete old list and save the new list: */
	if (g_BlockSaveDataList ) /* Delete last deleted item or item list */
	{
		EDA_BaseStruct * item = g_BlockSaveDataList, * next_item;
		while (item)
		{
			next_item = item->Pnext;
			delete item;
			item = next_item;
		}
	}

	g_BlockSaveDataList = DrawStructCopy;
	DrawStructCopy->m_Parent = NULL;

	return DrawStructCopy;
}

/*****************************************************************************
* Routine to undelete a structure from the undelete stack.					 *
*****************************************************************************/
bool WinEDA_SchematicFrame::UnDeleteStruct(wxDC * DC)
{
EDA_BaseStruct *DrawStruct;
DrawPickedStruct  *PickedItem, *PickedList = NULL;

	if (g_UnDeleteStackPtr == 0)
		{
		DisplayError(this, wxT("No Data to undelete") );
		return FALSE;
		}

	DrawStruct = g_UnDeleteStack[--g_UnDeleteStackPtr];
	if (DrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
		{
		RedrawStructList(DrawPanel, DC, DrawStruct, GR_DEFAULT_DRAWMODE);

		for(PickedList = (DrawPickedStruct *) DrawStruct; PickedList != NULL; )
			{
			EDA_BaseStruct * Struct = PickedList->m_PickedStruct;
			Struct->Pnext =  GetScreen()->EEDrawList;
			GetScreen()->EEDrawList = Struct;
			SetStructFather(Struct, GetScreen());
			PickedItem = PickedList;
			PickedList = (DrawPickedStruct *)PickedList->Pnext;
			delete PickedItem;
			}
		}
	else
		{
		SetStructFather(DrawStruct, GetScreen());
		RedrawOneStruct(DrawPanel, DC, DrawStruct, GR_DEFAULT_DRAWMODE);
		DrawStruct->Pnext = GetScreen()->EEDrawList;
		GetScreen()->EEDrawList = DrawStruct;
		}

	SetFlagModify(GetScreen());
	return TRUE;
}

/*****************************************************************************
* Routine to paste a structure from the g_BlockSaveDataList stack.						 *
*	This routine is the same as undelete but original list is NOT removed.	 *
*****************************************************************************/
void WinEDA_SchematicFrame::PasteStruct(wxDC * DC)
{
EDA_BaseStruct *DrawStruct;
DrawPickedStruct  *PickedItem, *PickedList = NULL;

	if (g_BlockSaveDataList == NULL)
	{
		DisplayError(this, wxT("No struct to paste") );
		return;
	}

	DrawStruct = DuplicateStruct(g_BlockSaveDataList);

	PlaceStruct( GetScreen(), DrawStruct);
	RedrawStructList(DrawPanel, DC, DrawStruct, GR_DEFAULT_DRAWMODE);

	// Clear annotation and init new time stamp for the new components:
	if (DrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
	{
		for(PickedList = (DrawPickedStruct *) DrawStruct; PickedList != NULL; )	// Clear annotation for new components
		{
			EDA_BaseStruct * Struct = PickedList->m_PickedStruct;
			if ( Struct->m_StructType == DRAW_LIB_ITEM_STRUCT_TYPE )
			{
				((EDA_SchComponentStruct*)Struct)->m_TimeStamp = GetTimeStamp();
				((EDA_SchComponentStruct*)Struct)->ClearAnnotation();
			}
			PickedList = (DrawPickedStruct *)PickedList->Pnext;
		}
		RedrawStructList(DrawPanel, DC, DrawStruct, GR_DEFAULT_DRAWMODE);
		for(PickedList = (DrawPickedStruct *) DrawStruct; PickedList != NULL; )
		{
			EDA_BaseStruct * Struct = PickedList->m_PickedStruct;
			Struct->Pnext =  GetScreen()->EEDrawList;
			SetStructFather(Struct, GetScreen());
			GetScreen()->EEDrawList = Struct;
			PickedItem = PickedList;
			PickedList = (DrawPickedStruct *)PickedList->Pnext;
			delete PickedItem;
		}
	}
	else
	{
		SetStructFather(DrawStruct, GetScreen());
		if ( DrawStruct->m_StructType == DRAW_LIB_ITEM_STRUCT_TYPE )
		{
			((EDA_SchComponentStruct*)DrawStruct)->m_TimeStamp = GetTimeStamp();
			((EDA_SchComponentStruct*)DrawStruct)->ClearAnnotation();
		}
		RedrawOneStruct(DrawPanel, DC, DrawStruct, GR_DEFAULT_DRAWMODE);
		DrawStruct->Pnext = GetScreen()->EEDrawList;
		GetScreen()->EEDrawList = DrawStruct;
	}

	/* clear .m_Flags member for all items */
	EDA_BaseStruct * Struct;
	for(Struct = GetScreen()->EEDrawList; Struct != NULL; Struct=Struct->Pnext)
		Struct->m_Flags = 0;
	SetFlagModify(GetScreen());

	return;
}


/*****************************************************************************
* Routine to place a given object.											 *
*****************************************************************************/
bool PlaceStruct( BASE_SCREEN * screen, EDA_BaseStruct *DrawStruct)
{
DrawPickedStruct * DrawStructs;
int dx, dy;

	if ( !DrawStruct ) return FALSE;

	dx = screen->m_Curseur.x -
			screen->BlockLocate.m_BlockLastCursorPosition.x;
	dy = screen->m_Curseur.y -
			screen->BlockLocate.m_BlockLastCursorPosition.y;

	switch (DrawStruct->m_StructType)
		{
		default:
		case TYPE_NOT_INIT:
			return FALSE;

		case DRAW_POLYLINE_STRUCT_TYPE:
		case DRAW_JUNCTION_STRUCT_TYPE:
		case DRAW_SEGMENT_STRUCT_TYPE:
		case DRAW_BUSENTRY_STRUCT_TYPE:
		case DRAW_TEXT_STRUCT_TYPE:
		case DRAW_LABEL_STRUCT_TYPE:
		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
		case DRAW_LIB_ITEM_STRUCT_TYPE:
		case DRAW_SHEET_STRUCT_TYPE:
		case DRAW_SHEETLABEL_STRUCT_TYPE:
		case DRAW_MARKER_STRUCT_TYPE:
		case DRAW_NOCONNECT_STRUCT_TYPE:
			MoveOneStruct(DrawStruct, dx, dy);
			break;

		case DRAW_PICK_ITEM_STRUCT_TYPE:
			DrawStructs  = (DrawPickedStruct *) DrawStruct;
			while (DrawStructs)
				{
				MoveOneStruct(DrawStructs->m_PickedStruct, dx, dy);
				DrawStructs = (DrawPickedStruct *)DrawStructs->Pnext;
				}
			break;
		}
	return TRUE;
}

/**************************************************************/
void MoveOneStruct(EDA_BaseStruct *DrawStruct, int Dx, int Dy)
/**************************************************************/
/* Given a structure move it by Dx, Dy.
*/
{
int ii, *Points;
DrawPolylineStruct *DrawPoly;
DrawJunctionStruct *DrawConnect;
EDA_DrawLineStruct *DrawSegment;
DrawBusEntryStruct *DrawRaccord;
EDA_SchComponentStruct *DrawLibItem;
DrawSheetStruct *DrawSheet;
DrawSheetLabelStruct *DrawSheetLabel;
DrawMarkerStruct * DrawMarker;
DrawNoConnectStruct * DrawNoConnect;

	if ( !DrawStruct ) return;

	switch (DrawStruct->m_StructType)
		{
		case TYPE_NOT_INIT:
		    break;

		case DRAW_POLYLINE_STRUCT_TYPE:
			DrawPoly = (DrawPolylineStruct *) DrawStruct;
			Points = DrawPoly->m_Points;
			for (ii = 0; ii < DrawPoly->m_NumOfPoints; ii++)
				{
				Points[ii * 2] += Dx; Points[ii * 2 + 1] += Dy;
				}
			break;

		case DRAW_SEGMENT_STRUCT_TYPE:
			DrawSegment = (EDA_DrawLineStruct *) DrawStruct;
			if( (DrawSegment->m_Flags & STARTPOINT) == 0 )
				{
				DrawSegment->m_Start.x += Dx; DrawSegment->m_Start.y += Dy;
				}
			if( (DrawSegment->m_Flags & ENDPOINT) == 0 )
				{
				DrawSegment->m_End.x += Dx; DrawSegment->m_End.y += Dy;
				}
			break;

		case DRAW_BUSENTRY_STRUCT_TYPE:
			 DrawRaccord = (DrawBusEntryStruct *) DrawStruct;
			 DrawRaccord->m_Pos.x += Dx; DrawRaccord->m_Pos.y += Dy;
			 break;

		case DRAW_JUNCTION_STRUCT_TYPE:
			 DrawConnect = (DrawJunctionStruct *) DrawStruct;
			 DrawConnect->m_Pos.x += Dx; DrawConnect->m_Pos.y += Dy;
			 break;

		case DRAW_MARKER_STRUCT_TYPE:
			 DrawMarker = (DrawMarkerStruct *) DrawStruct;
			 DrawMarker->m_Pos.x += Dx; DrawMarker->m_Pos.y += Dy;
			 break;

		case DRAW_NOCONNECT_STRUCT_TYPE:
			 DrawNoConnect = (DrawNoConnectStruct *) DrawStruct;
			 DrawNoConnect->m_Pos.x += Dx; DrawNoConnect->m_Pos.y += Dy;
			 break;

		case DRAW_TEXT_STRUCT_TYPE:
			 #define DrawText ((DrawTextStruct *) DrawStruct)
			 DrawText->m_Pos.x += Dx; DrawText->m_Pos.y += Dy;
			 break;

		case DRAW_LABEL_STRUCT_TYPE:
			 #define DrawLabel ((DrawLabelStruct *) DrawStruct)
			 DrawLabel->m_Pos.x += Dx; DrawLabel->m_Pos.y += Dy;
			 break;

		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
			 #define DrawGlobalLabel ((DrawGlobalLabelStruct *) DrawStruct)
			 DrawGlobalLabel->m_Pos.x += Dx; DrawGlobalLabel->m_Pos.y += Dy;
			 break;

		case DRAW_LIB_ITEM_STRUCT_TYPE:
			DrawLibItem = (EDA_SchComponentStruct *) DrawStruct;
			DrawLibItem->m_Pos.x += Dx; DrawLibItem->m_Pos.y += Dy;
			for( ii = 0; ii < NUMBER_OF_FIELDS; ii++ )
				{
				DrawLibItem->m_Field[ii].m_Pos.x += Dx;
				DrawLibItem->m_Field[ii].m_Pos.y += Dy;
				}
			break;

		case DRAW_SHEET_STRUCT_TYPE:
			DrawSheet = (DrawSheetStruct *) DrawStruct;
			DrawSheet->m_Pos.x += Dx; DrawSheet->m_Pos.y += Dy;
			DrawSheetLabel = DrawSheet->m_Label;
			while( DrawSheetLabel != NULL )
				{
				DrawSheetLabel->m_Pos.x += Dx;
				DrawSheetLabel->m_Pos.y += Dy;
				DrawSheetLabel = (DrawSheetLabelStruct*)DrawSheetLabel->Pnext;
				}
			break;

		case DRAW_SHEETLABEL_STRUCT_TYPE:
			DrawSheetLabel = (DrawSheetLabelStruct *) DrawStruct;
			DrawSheetLabel->m_Pos.x += Dx;
			DrawSheetLabel->m_Pos.y += Dy;
			break;

		case DRAW_PICK_ITEM_STRUCT_TYPE : break;

		default: break;
		}
}


/******************************************************************/
static EDA_BaseStruct * DuplicateStruct(EDA_BaseStruct *DrawStruct)
/******************************************************************/
/* Routine to create a new copy of given struct.
	The new object is not put in draw list (not linked)
*/
{
EDA_BaseStruct *NewDrawStruct = NULL;

	if ( DrawStruct == NULL )
	{
		DisplayError(NULL, wxT("DuplicateStruct error: NULL struct") );
		return NULL;
	}

	switch (DrawStruct->m_StructType)
	{
		case DRAW_POLYLINE_STRUCT_TYPE:
			NewDrawStruct = ((DrawPolylineStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_SEGMENT_STRUCT_TYPE:
			NewDrawStruct = ((EDA_DrawLineStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_BUSENTRY_STRUCT_TYPE:
			NewDrawStruct = ((DrawBusEntryStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_JUNCTION_STRUCT_TYPE:
			NewDrawStruct = ((DrawJunctionStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_MARKER_STRUCT_TYPE:
			NewDrawStruct = ((DrawMarkerStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_NOCONNECT_STRUCT_TYPE:
			NewDrawStruct = ((DrawNoConnectStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_TEXT_STRUCT_TYPE:
			NewDrawStruct = ((DrawTextStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_LABEL_STRUCT_TYPE:
			NewDrawStruct = ((DrawLabelStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
			NewDrawStruct = ((DrawGlobalLabelStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_LIB_ITEM_STRUCT_TYPE:
			NewDrawStruct = ((EDA_SchComponentStruct*)DrawStruct)->GenCopy();
			break;
		case DRAW_SHEET_STRUCT_TYPE:
			NewDrawStruct = ((DrawSheetStruct*)DrawStruct)->GenCopy();
			break;

		case DRAW_PICK_ITEM_STRUCT_TYPE:
		{
			DrawPickedStruct *NewPickedItem, *PickedList = NULL,
							*LastPickedItem = NULL;
			PickedList = (DrawPickedStruct *) DrawStruct;
			while (PickedList)
			{
				NewPickedItem = new DrawPickedStruct();
				if (NewDrawStruct == NULL) NewDrawStruct = NewPickedItem;
				if (LastPickedItem ) LastPickedItem->Pnext = NewPickedItem;
				LastPickedItem = NewPickedItem;
				NewPickedItem->m_PickedStruct =
					DuplicateStruct(PickedList->m_PickedStruct);
				PickedList = (DrawPickedStruct *)PickedList->Pnext;
			}
			 break;
		}

		case DRAW_SHEETLABEL_STRUCT_TYPE:
		case DRAW_PART_TEXT_STRUCT_TYPE:
		case SCREEN_STRUCT_TYPE:
		default:
		{
			wxString msg;
			msg << wxT("DuplicateStruct error: unexpected StructType ") <<
				DrawStruct->m_StructType << wxT(" ") << DrawStruct->ReturnClassName();
			DisplayError(NULL, msg);
		}
			break;
	}

	return NewDrawStruct;
}


/****************************************************/
static void CollectStructsToDrag(BASE_SCREEN * screen)
/****************************************************/
{
DrawPickedStruct * DrawStructs, *FirstPicked;
EDA_BaseStruct * Struct;
EDA_DrawLineStruct* SegmStruct;
int ox, oy, fx, fy;

	/* Set membre .m_Flags des segments */
	for(Struct = screen->EEDrawList; Struct != NULL; Struct=Struct->Pnext)
		Struct->m_Flags = 0;

	if(screen->BlockLocate.m_BlockDrawStruct->m_StructType == DRAW_SEGMENT_STRUCT_TYPE )
		screen->BlockLocate.m_BlockDrawStruct->m_Flags = SELECTED;

	else if (screen->BlockLocate.m_BlockDrawStruct->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE )
	{
		DrawStructs  = (DrawPickedStruct *) screen->BlockLocate.m_BlockDrawStruct;
		while (DrawStructs)
		{
			Struct = DrawStructs->m_PickedStruct;
			DrawStructs = (DrawPickedStruct *)DrawStructs->Pnext;
			Struct->m_Flags = SELECTED;
		}
	}

	if( screen->BlockLocate.m_Command != BLOCK_DRAG ) return;

	ox = screen->BlockLocate.GetX();
	oy = screen->BlockLocate.GetY();
	fx = screen->BlockLocate.GetRight();
	fy = screen->BlockLocate.GetBottom();

	if (fx < ox) EXCHG(fx, ox);
	if (fy < oy) EXCHG(fy, oy);

	/* Pour Drag Block: remise sous forme de liste de structure, s'il n'y
	a qu'un seul element ( pour homogeneiser les traitements ulterieurs */
	if(screen->BlockLocate.m_BlockDrawStruct->m_StructType != DRAW_PICK_ITEM_STRUCT_TYPE )
	{
		DrawStructs = new DrawPickedStruct(screen->BlockLocate.m_BlockDrawStruct);
		screen->BlockLocate.m_BlockDrawStruct = DrawStructs;
	}

	/* Suppression du deplacement des extremites de segments hors cadre
	de selection */
	DrawStructs  = (DrawPickedStruct *) screen->BlockLocate.m_BlockDrawStruct;
	while (DrawStructs)
	{
		Struct = DrawStructs->m_PickedStruct;
		DrawStructs = (DrawPickedStruct *)DrawStructs->Pnext;
		if( Struct->m_StructType == DRAW_SEGMENT_STRUCT_TYPE)
		{
			SegmStruct = (EDA_DrawLineStruct*)Struct;
			if( (SegmStruct->m_Start.x < ox) || (SegmStruct->m_Start.x > fx) ||
				(SegmStruct->m_Start.y < oy) || (SegmStruct->m_Start.y > fy) )
				SegmStruct->m_Flags |= STARTPOINT;

			if( (SegmStruct->m_End.x < ox) || (SegmStruct->m_End.x > fx) ||
				(SegmStruct->m_End.y < oy) || (SegmStruct->m_End.y > fy) )
				SegmStruct->m_Flags |= ENDPOINT;
		}
	}

	/* Recherche des elements complementaires a "dragger", c'est a dire les
	fils et connexions hors bloc relies a des pins ou entries elles meme
	draggees */

	FirstPicked = DrawStructs = (DrawPickedStruct*)screen->BlockLocate.m_BlockDrawStruct;
	while (DrawStructs)
	{
		Struct = DrawStructs->m_PickedStruct;
		DrawStructs = (DrawPickedStruct * )DrawStructs->Pnext;
		if( Struct->m_StructType == DRAW_LIB_ITEM_STRUCT_TYPE)
		{
			LibEDA_BaseStruct *DrawItem;
			int x, y;
			DrawItem = GetNextPinPosition( (EDA_SchComponentStruct*)Struct, &x, &y);
			while ( DrawItem )
			{
				if( (x < ox) || (x > fx) || (y < oy) || (y > fy) )
					AddPickedItem(screen, x, y);

				DrawItem = GetNextPinPosition( NULL, &x, &y);
			}
		}

		if( Struct->m_StructType == DRAW_SHEET_STRUCT_TYPE)
		{
			DrawSheetLabelStruct * SLabel = ((DrawSheetStruct*)Struct)->m_Label;
			while(SLabel)
			{
				if( SLabel->m_StructType == DRAW_SHEETLABEL_STRUCT_TYPE )
					AddPickedItem(screen, SLabel->m_Pos.x, SLabel->m_Pos.y);
				SLabel = (DrawSheetLabelStruct*)SLabel->Pnext;
			}
		}

		if( Struct->m_StructType == DRAW_BUSENTRY_STRUCT_TYPE)
		{
			DrawBusEntryStruct * item = (DrawBusEntryStruct*)Struct;
			AddPickedItem(screen, item->m_Pos.x, item->m_Pos.y);
			AddPickedItem(screen, item->m_End().x, item->m_End().y);
		}
	}
}


/***********************************************************/
static void AddPickedItem( BASE_SCREEN * screen, int px, int py )
/**********************************************************/
{
DrawPickedStruct * DrawStructs;
EDA_BaseStruct * Struct;

	/* Examen de la liste des elements deja selectionnes */
	DrawStructs  = (DrawPickedStruct *) screen->BlockLocate.m_BlockDrawStruct;
	while (DrawStructs)
		{
		Struct = DrawStructs->m_PickedStruct;
		DrawStructs = (DrawPickedStruct *)DrawStructs->Pnext;
		switch( Struct->m_StructType )
			{
			case DRAW_SEGMENT_STRUCT_TYPE:
				#undef STRUCT
				#define STRUCT ((EDA_DrawLineStruct*)Struct)
				if( (STRUCT->m_Start.x == px) && (STRUCT->m_Start.y == py) )
					STRUCT->m_Flags &= ~STARTPOINT;

				if( (STRUCT->m_End.x == px) && (STRUCT->m_End.y == py) )
					STRUCT->m_Flags &= ~ENDPOINT;
				break;
			default: break;
			}
		}

	/* Examen de la liste des elements non selectionnes */

	Struct = screen->EEDrawList;
	while ( Struct )
		{
		switch (Struct->m_StructType)
			{
			case TYPE_NOT_INIT:
			    break;
			case DRAW_POLYLINE_STRUCT_TYPE:
				if( Struct->m_Flags & SELECTED ) break; /* Deja en liste */
				break;

			case DRAW_JUNCTION_STRUCT_TYPE:
				#undef STRUCT
				#define STRUCT ((DrawJunctionStruct*)Struct)
				if( Struct->m_Flags & SELECTED ) break; /* Deja en liste */
				if( STRUCT->m_Pos.x != px ) break;
				if( STRUCT->m_Pos.y != py ) break;
				DrawStructs = new DrawPickedStruct(Struct);
				DrawStructs->Pnext =
						(DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
				screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
				break;

			case DRAW_SEGMENT_STRUCT_TYPE:
				#undef STRUCT
				#define STRUCT ((EDA_DrawLineStruct*)Struct)
				if( Struct->m_Flags & SELECTED ) break; /* Deja en liste */
				if( (STRUCT->m_Start.x == px ) && ( STRUCT->m_Start.y == py ) )
					{
					DrawStructs = new DrawPickedStruct(Struct);
					DrawStructs->Pnext =
							(DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
					screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*)DrawStructs;
					Struct->m_Flags = SELECTED | ENDPOINT | STARTPOINT;
					Struct->m_Flags &= ~STARTPOINT;
					}
				else if( (STRUCT->m_End.x == px ) && ( STRUCT->m_End.y == py ) )
					{
					DrawStructs = new DrawPickedStruct(Struct);
					DrawStructs->Pnext =
						(DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
					screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
					Struct->m_Flags = SELECTED | ENDPOINT | STARTPOINT;
					Struct->m_Flags &= ~ENDPOINT;
					}
				 break;

			case DRAW_BUSENTRY_STRUCT_TYPE:
				break;

			case DRAW_TEXT_STRUCT_TYPE:
				 break;

			case DRAW_LABEL_STRUCT_TYPE:
				#undef STRUCT
				#define STRUCT ((DrawLabelStruct*)Struct)
				if( Struct->m_Flags & SELECTED ) break; /* Deja en liste */
				if( STRUCT->m_Pos.x != px ) break;
				if( STRUCT->m_Pos.y != py ) break;
				DrawStructs = new DrawPickedStruct(Struct);
				DrawStructs->Pnext =
						(DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
				screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
				Struct->m_Flags |= SELECTED;
				break;

			case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
				#undef STRUCT
				#define STRUCT ((DrawGlobalLabelStruct*)Struct)
				if( Struct->m_Flags & SELECTED ) break; /* Deja en liste */
				if( STRUCT->m_Pos.x != px ) break;
				if( STRUCT->m_Pos.y != py ) break;
				DrawStructs = new DrawPickedStruct(Struct);
				DrawStructs->Pnext =
						(DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
				screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
				Struct->m_Flags |= SELECTED;
				break;

			case DRAW_LIB_ITEM_STRUCT_TYPE:
				 break;

			case DRAW_SHEET_STRUCT_TYPE:
				 break;

			case DRAW_SHEETLABEL_STRUCT_TYPE:
				 break;

			case DRAW_PICK_ITEM_STRUCT_TYPE:
				break;

			case DRAW_MARKER_STRUCT_TYPE:
				#undef STRUCT
				#define STRUCT ((DrawMarkerStruct*)Struct)
				if( Struct->m_Flags & SELECTED ) break; /* Deja en liste */
				if( STRUCT->m_Pos.x != px ) break;
				if( STRUCT->m_Pos.y != py ) break;
				DrawStructs = new DrawPickedStruct(Struct);
				DrawStructs->Pnext =
						(DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
				screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
				Struct->m_Flags |= SELECTED;
				break;

			case DRAW_NOCONNECT_STRUCT_TYPE:
				#undef STRUCT
				#define STRUCT ((DrawNoConnectStruct*)Struct)
				if( Struct->m_Flags & SELECTED ) break; /* Deja en liste */
				if( STRUCT->m_Pos.x != px ) break;
				if( STRUCT->m_Pos.y != py ) break;
				DrawStructs = new DrawPickedStruct(Struct);
				DrawStructs->Pnext =
						(DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
				screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
				Struct->m_Flags |= SELECTED;
				break;

			default:
				break;
			}
		Struct = Struct->Pnext;
		}
}

/*********************************************************************************/
static LibEDA_BaseStruct * GetNextPinPosition( EDA_SchComponentStruct * DrawLibItem,
												int * px, int * py )
/*********************************************************************************/
{
EDA_LibComponentStruct *Entry;
static LibEDA_BaseStruct *NextItem;
static int Multi, convert, PartX, PartY, TransMat[2][2];
LibEDA_BaseStruct *DEntry;
int x2, y2, orient;
LibDrawPin * Pin;

	if( DrawLibItem )
		{
		NextItem = NULL;
		if((Entry = FindLibPart(DrawLibItem->m_ChipName.GetData(),wxEmptyString,FIND_ROOT)) == NULL)
			return(NULL);
		DEntry = Entry->m_Drawings;
		Multi = DrawLibItem->m_Multi;
		convert = DrawLibItem->m_Convert;
		PartX = DrawLibItem->m_Pos.x;
		PartY = DrawLibItem->m_Pos.y;
		memcpy(TransMat, DrawLibItem->m_Transform, sizeof(TransMat) );
		}
	else DEntry = NextItem;

	for( ; DEntry != NULL; DEntry = DEntry->Next())
	{
		/* Elimination des elements non relatifs a l'unite */
		if( Multi && DEntry->m_Unit && (DEntry->m_Unit != Multi) ) continue;
		if( convert && DEntry->m_Convert && (DEntry->m_Convert != convert) )
			 continue;
		if( DEntry->m_StructType != COMPONENT_PIN_DRAW_TYPE ) continue;

		Pin = (LibDrawPin * ) DEntry;

		/* Calcul de l'orientation reelle de la Pin */
		orient = Pin->ReturnPinDrawOrient(TransMat);

		/* Calcul de la position du point de reference */
		x2 = PartX + (TransMat[0][0] * Pin->m_Pos.x)
					 + (TransMat[0][1] * Pin->m_Pos.y);
		y2 = PartY + (TransMat[1][0] * Pin->m_Pos.x)
					+ (TransMat[1][1] * Pin->m_Pos.y);
		*px = x2; *py = y2;
		NextItem = DEntry->Next();
		return(DEntry);
	}

	NextItem = NULL;
	return(NULL);
}

