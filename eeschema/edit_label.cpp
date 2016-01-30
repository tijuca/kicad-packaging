/**************************************************************************/
/* EESchema											  					  */
/* editexte.cpp: creation/ editions des textes (labels, textes sur schema) */
/**************************************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"

/* Fonctions locales */
static void ShowWhileMoving(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);
static void ExitMoveTexte(WinEDA_DrawFrame * frame, wxDC *DC);

/* Variables locales */
static wxPoint ItemInitialPosition;
static int OldOrient;
static wxSize OldSize;
static int ShapeGLabel = (int) NET_INPUT;
static int TextLabelSize = DEFAULT_SIZE_TEXT;


	/************************************/
	/* class WinEDA_LabelPropertiesFrame */
	/************************************/

#include "dialog_edit_label.cpp"



void WinEDA_LabelPropertiesFrame::TextPropertiesAccept(wxCommandEvent& event)
{
wxString text;
int value;
	
	text = m_TextLabel->GetValue();

	if ( ! text.IsEmpty() ) m_CurrentText->m_Text = text;
	else if ( (m_CurrentText->m_Flags & IS_NEW) == 0 )
		DisplayError(this, wxT("Empty Text!") );

	m_CurrentText->m_Orient = m_TextOrient->GetSelection();
	text = m_TextSize->GetValue();
	value = ReturnValueFromString(g_UnitMetric, text, m_Parent->m_InternalUnits);
	m_CurrentText->m_Size.x = m_CurrentText->m_Size.y = value;
	if ( m_TextShape ) m_CurrentText->m_Shape = m_TextShape->GetSelection();

	SetFlagModify(m_Parent->GetScreen());

	Close(TRUE);
}



/********************************************************************************/
void WinEDA_SchematicFrame::StartMoveTexte(DrawTextStruct * TextStruct, wxDC *DC)
/********************************************************************************/
{
	if(TextStruct == NULL) return;

	g_ItemToRepeat = NULL;
	TextStruct->m_Flags |= IS_MOVED;

	switch( TextStruct->m_StructType )
		{
		case DRAW_LABEL_STRUCT_TYPE:
		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
		case DRAW_TEXT_STRUCT_TYPE:
			ItemInitialPosition = TextStruct->m_Pos;
			OldSize = TextStruct->m_Size; OldOrient = TextStruct->m_Orient;
			break;

		default: break;
		}

 	m_CurrentScreen->CursorOff(DrawPanel, DC);
 	m_CurrentScreen->m_Curseur = ItemInitialPosition;
 	DrawPanel->MouseToCursorSchema();
 
	SetFlagModify(GetScreen());
	m_CurrentScreen->ManageCurseur = ShowWhileMoving;
	m_CurrentScreen->ForceCloseManageCurseur = ExitMoveTexte;
	m_CurrentScreen->ManageCurseur(DrawPanel, DC, TRUE);
 
 	m_CurrentScreen->CursorOn(DrawPanel, DC);
}



/*************************************************************************/
void WinEDA_SchematicFrame::EditSchematicText(DrawTextStruct * TextStruct,
			wxDC * DC)
/*************************************************************************/
/* Changement du texte (Label.. ) pointe par la souris
*/
{
	if(TextStruct == NULL)  return;

	GetScreen()->CursorOff(DrawPanel, DC);
	RedrawOneStruct(DrawPanel, DC, TextStruct, g_XorMode);

WinEDA_LabelPropertiesFrame * frame = new WinEDA_LabelPropertiesFrame(this,
	TextStruct, wxPoint(30,30));
	frame->ShowModal(); frame->Destroy();

	RedrawOneStruct(DrawPanel, DC, TextStruct, GR_DEFAULT_DRAWMODE);
	GetScreen()->CursorOn(DrawPanel, DC);
}


/***********************************************************************************/
void WinEDA_SchematicFrame::ChangeTextOrient(DrawTextStruct * TextStruct, wxDC * DC)
/***********************************************************************************/
{
	if( TextStruct == NULL )
		TextStruct = (DrawTextStruct *) PickStruct(GetScreen()->m_Curseur,
				GetScreen()->EEDrawList, TEXTITEM|LABELITEM);
	if( TextStruct == NULL ) return;

	/* Effacement du texte en cours */
	RedrawOneStruct(DrawPanel, DC, TextStruct, g_XorMode);

	/* Rotation du texte */
	switch( TextStruct->m_StructType )
		{
		case DRAW_LABEL_STRUCT_TYPE:
		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
		case DRAW_TEXT_STRUCT_TYPE:
			((DrawTextStruct*)TextStruct)->m_Orient++;
			((DrawTextStruct*)TextStruct)->m_Orient &= 3;
			break;

		default: break;
		}

	SetFlagModify(GetScreen());

	/* Reaffichage */
	RedrawOneStruct(DrawPanel, DC, TextStruct, g_XorMode);
}

/*************************************************************************/
EDA_BaseStruct * WinEDA_SchematicFrame::CreateNewText(wxDC * DC, int type)
/*************************************************************************/
/* Routine to create new text struct (GraphicText, label or Glabel).
*/
{
DrawTextStruct * NewText =  NULL;

	g_ItemToRepeat = NULL;

	switch( type )
		{
		case LAYER_NOTES:
			NewText = new DrawTextStruct(m_CurrentScreen->m_Curseur);
			NewText->m_Size.x = NewText->m_Size.y = TextLabelSize;
			break;

		case LAYER_LOCLABEL:
			{
			NewText = new DrawLabelStruct(m_CurrentScreen->m_Curseur);
			NewText->m_Size.x = NewText->m_Size.y = TextLabelSize;
			}
			break;

		case LAYER_GLOBLABEL:
			NewText = new DrawGlobalLabelStruct(m_CurrentScreen->m_Curseur);
			NewText->m_Size.x = NewText->m_Size.y = TextLabelSize;
			((DrawGlobalLabelStruct*)NewText)->m_Shape = ShapeGLabel;
			break;

		default:
			DisplayError(this, wxT("Editexte: Internal error") );
			break;
		}

	NewText->m_Flags = IS_NEW | IS_MOVED;

	EditSchematicText(NewText, DC);

	if ( NewText->m_Text.IsEmpty() )
	{
		delete NewText;
		return NULL;
	}

	RedrawOneStruct(DrawPanel, DC, NewText, GR_DEFAULT_DRAWMODE);
	m_CurrentScreen->ManageCurseur = ShowWhileMoving;
	m_CurrentScreen->ForceCloseManageCurseur = ExitMoveTexte;

	m_CurrentScreen->m_CurrentItem = NewText;

	return NewText;
}



	/****************************************/
	/*		Dessin du Texte en deplacement	*/
	/****************************************/
static void ShowWhileMoving(WinEDA_DrawPanel * panel, wxDC * DC, bool erase)
{
EDA_BaseStruct * TextStruct = panel->GetScreen()->m_CurrentItem;

	/* effacement ancienne position */
	if( erase )
		RedrawOneStruct(panel, DC, TextStruct, g_XorMode);

	/* Redessin du texte */
	switch( TextStruct->m_StructType )
		{
		case DRAW_LABEL_STRUCT_TYPE:
		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
		case DRAW_TEXT_STRUCT_TYPE:
			((DrawTextStruct*)TextStruct)->m_Pos = panel->GetScreen()->m_Curseur;
			break;

		default: break;
		}

	RedrawOneStruct(panel, DC, TextStruct, g_XorMode);
}


/*************************************************************/
static void ExitMoveTexte(WinEDA_DrawFrame * frame, wxDC *DC)
/*************************************************************/
/* Routine de sortie des menus de Texte */
{
EDA_BaseStruct * Struct = frame->m_CurrentScreen->m_CurrentItem;

	g_ItemToRepeat = NULL;
	frame->m_CurrentScreen->ManageCurseur = NULL;
	frame->m_CurrentScreen->ForceCloseManageCurseur = NULL;

	if( Struct == NULL)  /* Pas de trace en cours  */
		{
		return;
		}

	/* ici : trace en cours */

	/* Effacement du trace en cours et suppression eventuelle de la structure */
	RedrawOneStruct(frame->DrawPanel, DC, Struct, g_XorMode);

	if( Struct->m_Flags & IS_NEW )	/* Suppression du nouveau texte en cours de placement */
		{
		delete Struct;
		frame->m_CurrentScreen->m_CurrentItem = NULL;
		}

	else	/* Remise a jour des anciens parametres du texte */
		{
		switch( Struct->m_StructType )
			{
			case DRAW_LABEL_STRUCT_TYPE:
			case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
			case DRAW_TEXT_STRUCT_TYPE:
         	{
				DrawTextStruct * Text = (DrawTextStruct *) Struct;
				Text->m_Pos = ItemInitialPosition;
				Text->m_Size = OldSize;
				Text->m_Orient = OldOrient;
            }
				break;

			default: break;
			}

		RedrawOneStruct(frame->DrawPanel, DC, Struct, GR_DEFAULT_DRAWMODE);
		Struct->m_Flags = 0;
		}

}

/*****************************************************************************/
void WinEDA_SchematicFrame::ChangeTypeText(DrawTextStruct * Text,
				wxDC * DC, int newtype)
/*****************************************************************************/

/* Routine to create new text struct (GraphicText, label or Glabel).
*/
{
	if ( Text == NULL ) return;

	m_CurrentScreen->CursorOff(DrawPanel, DC);	// Erase schematic cursor
	RedrawOneStruct(DrawPanel, DC, Text, g_XorMode);	// erase drawing
	switch( newtype )
		{
		case DRAW_LABEL_STRUCT_TYPE:
			Text->m_StructType = DRAW_LABEL_STRUCT_TYPE;
			Text->m_Layer = LAYER_LOCLABEL;
			break;

		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
			Text->m_StructType = DRAW_GLOBAL_LABEL_STRUCT_TYPE;
			Text->m_Layer = LAYER_GLOBLABEL;
			break;

		case DRAW_TEXT_STRUCT_TYPE:
			Text->m_StructType = DRAW_TEXT_STRUCT_TYPE;
			Text->m_Layer = LAYER_NOTES;
			break;

		default:
			DisplayError(this, wxT("ChangeTypeText: Internal error") );
			break;
		}

	RedrawOneStruct(DrawPanel, DC, Text, GR_DEFAULT_DRAWMODE);
	m_CurrentScreen->CursorOn(DrawPanel, DC);	// redraw schematic cursor
}


