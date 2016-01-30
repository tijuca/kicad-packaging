	/****************/
	/* controle.cpp */
	/****************/

#include "fctsys.h"

#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "eda_dde.h"

#include "id.h"

#include "protos.h"

/* Routines locales */

/* variables externes */

#define MSG_TO_PCB KICAD_PCB_PORT_SERVICE_NUMBER


/**************************************************************/
EDA_BaseStruct * WinEDA_SchematicFrame::
		SchematicGeneralLocateAndDisplay(bool IncludePin)
/**************************************************************/

/* Routine de localisation et d'affichage des caract (si utile )
	de l'element pointe par la souris ou par le curseur pcb
	- marqueur
	- noconnect
	- jonction
	- wire/bus/entry
	- label
	- composant
	- pin
	retourne
		un pointeur sur le composant
		Null sinon
*/
{
EDA_BaseStruct *DrawStruct;
wxString msg;
wxPoint mouse_position = GetScreen()->m_MousePosition;
LibDrawPin * Pin = NULL;
EDA_SchComponentStruct * LibItem = NULL;
char Line[1024];

	DrawStruct = SchematicGeneralLocateAndDisplay(mouse_position, IncludePin);
	if(! DrawStruct && ( mouse_position != GetScreen()->m_Curseur) )
	{
		DrawStruct = SchematicGeneralLocateAndDisplay(GetScreen()->m_Curseur, IncludePin);
	}
	if ( ! DrawStruct ) return NULL;

	/* Cross probing to pcbnew if a pin or a component is found */
	switch (DrawStruct->m_StructType )
	{
		case COMPONENT_FIELD_DRAW_TYPE:
		{
			PartTextStruct * Field = (PartTextStruct *) DrawStruct;
			LibItem = (EDA_SchComponentStruct * )Field->m_Parent;
			sprintf(Line,"$PART: %s", CONV_TO_UTF8(LibItem->m_Field[REFERENCE].m_Text));
			SendCommand(MSG_TO_PCB, Line);
		}
			break;

		case DRAW_LIB_ITEM_STRUCT_TYPE:
			Pin = LocateAnyPin(m_CurrentScreen->EEDrawList, GetScreen()->m_Curseur, &LibItem);
			if ( Pin ) break;	// Priority is probing a pin first
			LibItem = (EDA_SchComponentStruct *) DrawStruct;
			sprintf(Line,"$PART: %s", CONV_TO_UTF8(LibItem->m_Field[REFERENCE].m_Text) );
			SendCommand(MSG_TO_PCB, Line);
			break;

		default:
			Pin = LocateAnyPin(m_CurrentScreen->EEDrawList, GetScreen()->m_Curseur, &LibItem);
			break;
		
		case COMPONENT_PIN_DRAW_TYPE:
			Pin = (LibDrawPin*) DrawStruct;
			break;
	}
	
	if ( Pin )
	{
		/* Force display pin infos (the previous display could be a component info) */
		Pin->Display_Infos(this);
		if ( LibItem )
			Affiche_1_Parametre( this, 1,
					LibItem->m_Field[REFERENCE].m_Text,
					LibItem->m_Field[VALUE].m_Text,
					CYAN);
	
		// Cross probing:2 - pin found, and send a locate pin command to pcbnew (hightlight net)
		if(Pin->m_PinNum)
		{
			wxString pinnum;
			Pin->ReturnPinStringNum(pinnum);
			sprintf(Line,"$PIN: %s $PART: %s", CONV_TO_UTF8(pinnum),
						CONV_TO_UTF8(LibItem->m_Field[REFERENCE].m_Text));
			SendCommand(MSG_TO_PCB, Line);
		}
	}
	return DrawStruct;
}


/************************************************************************************/
EDA_BaseStruct * WinEDA_SchematicFrame::
		SchematicGeneralLocateAndDisplay(const wxPoint & refpoint, bool IncludePin)
/************************************************************************************/

/* Find the schematic item at position "refpoint"
	the priority order is:
	- marker
	- noconnect
	- junction
	- wire/bus/entry
	- label
	- pin
	- component
	return:
		an EDA_BaseStruct pointer on the item
		a Null pointer if no item found

	For some items, caracteristics are displayed on the screen.
*/
{
EDA_BaseStruct *DrawStruct;
LibDrawPin * Pin;
EDA_SchComponentStruct * LibItem;
wxString Text;
wxString msg;
int ii;

	DrawStruct = PickStruct(refpoint, GetScreen()->EEDrawList, MARKERITEM);
	if( DrawStruct )
	{
		DrawMarkerStruct * Marker = (DrawMarkerStruct *) DrawStruct;
		ii = Marker->m_Type;
		Text = Marker->GetComment();
		if(Text.IsEmpty() ) Text = wxT("NoComment");
		msg = NameMarqueurType[ii]; msg << wxT(" << ") << Text;
		Affiche_Message(msg);
		return(DrawStruct);
	}

	DrawStruct = PickStruct(refpoint, GetScreen()->EEDrawList,
			NOCONNECTITEM);
	if( DrawStruct )
	{
		MsgPanel->EraseMsgBox();
		return(DrawStruct);
	}

	DrawStruct = PickStruct(refpoint, GetScreen()->EEDrawList,
			JUNCTIONITEM);
	if( DrawStruct )
	{
		MsgPanel->EraseMsgBox();
		return(DrawStruct);
	}

	DrawStruct = PickStruct(refpoint, GetScreen()->EEDrawList,
			WIREITEM|BUSITEM|RACCORDITEM);
	if( DrawStruct )	// Search for a pin
	{
		Pin = LocateAnyPin(m_CurrentScreen->EEDrawList,refpoint, &LibItem);
		if( Pin )
		{
			Pin->Display_Infos(this);
			if ( LibItem )
				Affiche_1_Parametre( this, 1,
						LibItem->m_Field[REFERENCE].m_Text,
						LibItem->m_Field[VALUE].m_Text,
						CYAN);
	
		}
		else  MsgPanel->EraseMsgBox();
		return(DrawStruct);
	}

	DrawStruct = PickStruct(refpoint, GetScreen()->EEDrawList, FIELDCMPITEM);
	if( DrawStruct )
	{
		PartTextStruct * Field = (PartTextStruct *) DrawStruct;
		LibItem = (EDA_SchComponentStruct * )Field->m_Parent;
		LibItem->Display_Infos(this);

		return(DrawStruct);
	}

	/* search for a pin */
	Pin = LocateAnyPin(m_CurrentScreen->EEDrawList, refpoint, &LibItem);
	if( Pin )
	{
		Pin->Display_Infos(this);
		if ( LibItem )
			Affiche_1_Parametre( this, 1,
					LibItem->m_Field[REFERENCE].m_Text,
					LibItem->m_Field[VALUE].m_Text,
					CYAN);
		if ( IncludePin == TRUE ) return(LibItem);
	}

	DrawStruct = PickStruct(refpoint, GetScreen()->EEDrawList, LIBITEM);
	if( DrawStruct )
	{
		DrawStruct = LocateSmallestComponent( GetScreen() );
		LibItem = (EDA_SchComponentStruct *) DrawStruct;
		LibItem->Display_Infos(this);
		return(DrawStruct);
	}

	DrawStruct = PickStruct(refpoint, GetScreen()->EEDrawList,
			SHEETITEM);
	if( DrawStruct )
	{
		((DrawSheetStruct*) DrawStruct)->Display_Infos(this);
		return(DrawStruct);
	}

	// Recherche des autres elements
	DrawStruct = PickStruct(refpoint, GetScreen()->EEDrawList,
			SEARCHALL);
	if( DrawStruct )
	{
		return(DrawStruct);
	}

	MsgPanel->EraseMsgBox();
	return(NULL);
}



/***********************************************************************/
void WinEDA_DrawFrame::GeneralControle(wxDC *DC, wxPoint MousePositionInPixels)
/***********************************************************************/
{
wxSize delta;
int zoom = m_CurrentScreen->GetZoom();
wxPoint curpos, oldpos;
int hotkey = 0;
	
	ActiveScreen = (SCH_SCREEN *) m_CurrentScreen;
	
	curpos = m_CurrentScreen->m_MousePosition;
	oldpos = m_CurrentScreen->m_Curseur;

	delta.x = m_CurrentScreen->GetGrid().x / zoom;
	delta.y = m_CurrentScreen->GetGrid().y / zoom;

	if( delta.x <= 0 ) delta.x = 1;
	if( delta.y <= 0 ) delta.y = 1;
	
	switch( g_KeyPressed )
		{
		case EDA_PANNING_UP_KEY :
			OnZoom(ID_ZOOM_PANNING_UP);
			curpos = m_CurrentScreen->m_Curseur;
			break;
		case EDA_PANNING_DOWN_KEY :
			OnZoom(ID_ZOOM_PANNING_DOWN);
			curpos = m_CurrentScreen->m_Curseur;
			break;
		case EDA_PANNING_LEFT_KEY :
			OnZoom(ID_ZOOM_PANNING_LEFT);
			curpos = m_CurrentScreen->m_Curseur;
			break;
		case EDA_PANNING_RIGHT_KEY :
			OnZoom(ID_ZOOM_PANNING_RIGHT);
			curpos = m_CurrentScreen->m_Curseur;
			break;
		case WXK_F1 :
			OnZoom(ID_ZOOM_PLUS_KEY);
			curpos = m_CurrentScreen->m_Curseur;
			break;
		case WXK_F2 :
			OnZoom(ID_ZOOM_MOINS_KEY);
			curpos = m_CurrentScreen->m_Curseur;
			break;
		case WXK_F3 :
			OnZoom(ID_ZOOM_REDRAW_KEY);
			break;
		case WXK_F4 :
			OnZoom(ID_ZOOM_CENTER_KEY);
			curpos = m_CurrentScreen->m_Curseur;
			break;

		case ' ':	// Remise a zero coord relatives
			m_CurrentScreen->m_O_Curseur = m_CurrentScreen->m_Curseur;
			break;

		case '\t':    // Switch to drag mode, when block moving
			((WinEDA_SchematicFrame*)this)->HandleBlockEndByPopUp(BLOCK_DRAG, DC);
			break;

		case WXK_NUMPAD8  :	/* Deplacement curseur vers le haut */
		case WXK_UP	:
			MousePositionInPixels.y -= delta.y;
			DrawPanel->MouseTo(MousePositionInPixels);
			break ;

		case WXK_NUMPAD2:	/* Deplacement curseur vers le bas */
		case WXK_DOWN:
			MousePositionInPixels.y += delta.y;
			DrawPanel->MouseTo(MousePositionInPixels);
			break ;

		case WXK_NUMPAD4:	/* Deplacement curseur vers la gauche */
		case WXK_LEFT :
			MousePositionInPixels.x -= delta.x;
			DrawPanel->MouseTo(MousePositionInPixels);
			break ;

		case WXK_NUMPAD6:  /* Deplacement curseur vers la droite */
		case WXK_RIGHT:
			MousePositionInPixels.x += delta.x;
			DrawPanel->MouseTo(MousePositionInPixels);
			break;

		case WXK_INSERT:
		case WXK_NUMPAD0:
			if ( m_Ident == SCHEMATIC_FRAME )
			{
				if ( g_ItemToRepeat && (g_ItemToRepeat->m_Flags == 0)  )
				{
					((WinEDA_SchematicFrame*)this)->RepeatDrawItem(DC);
				}
				else wxBell();
				break;
			}
			if ( m_Ident == LIBEDITOR_FRAME )
			{
				if ( LibItemToRepeat && (LibItemToRepeat->m_Flags == 0) &&
					 (LibItemToRepeat->m_StructType == COMPONENT_PIN_DRAW_TYPE) )
				{
					((WinEDA_LibeditFrame*)this)->RepeatPinItem(DC,
								(LibDrawPin*) LibItemToRepeat);
				}
				else wxBell();
				break;
			}

		case 0:
		case WXK_DECIMAL:
			break;
		
		default: hotkey = g_KeyPressed;
			break;
			
		}

	/* Recalcul de la position du curseur schema */
	m_CurrentScreen->m_Curseur = curpos;
	/* Placement sur la grille generale */
	PutOnGrid( & m_CurrentScreen->m_Curseur);

	if( m_CurrentScreen->IsRefreshReq() )
	{
		RedrawActiveWindow(DC, TRUE);
	}

	if ( (oldpos.x != m_CurrentScreen->m_Curseur.x) ||
		 (oldpos.y != m_CurrentScreen->m_Curseur.y) )
	{
		curpos = m_CurrentScreen->m_Curseur;
		m_CurrentScreen->m_Curseur = oldpos;
		DrawPanel->CursorOff(DC);
		m_CurrentScreen->m_Curseur = curpos;
		DrawPanel->CursorOn(DC);

		if(DrawPanel->ManageCurseur)
		{
			DrawPanel->ManageCurseur(DrawPanel, DC, TRUE);
		}
	}

	Affiche_Status_Box();	 /* Affichage des coord curseur */
	
	if ( hotkey )
	{
		if( m_CurrentScreen->m_CurrentItem  &&
			m_CurrentScreen->m_CurrentItem->m_Flags )
			OnHotKey(DC, hotkey, m_CurrentScreen->m_CurrentItem);
		else OnHotKey(DC, hotkey, NULL);
	}

}


