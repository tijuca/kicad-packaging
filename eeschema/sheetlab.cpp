	/****************************************************************/
	/*	sheetlab.cpp  module pour creation /editin des Sheet labels */
	/****************************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"

/* Routines Locales */
static void ExitPinSheet(WinEDA_DrawFrame * frame, wxDC * DC );
static void Move_PinSheet(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);

/* Variables locales */
static int CurrentTypeLabel = NET_INPUT;
static wxSize NetSheetTextSize(DEFAULT_SIZE_TEXT, DEFAULT_SIZE_TEXT);

	/****************************************/
	/* class WinEDA_PinSheetPropertiesFrame */
	/****************************************/

#define NBSHAPES 5
static wxString shape_list[NBSHAPES] =
{
 wxT("Input"), wxT("Output"), wxT("Bidi"), wxT("TriState"), wxT("Passive")
};

enum id_Textdit
{
	ID_ACCEPT_PINSHEET_PROPERTIES = 1970,
	ID_CANCEL_PINSHEET_PROPERTIES
};


/*****************************************************/
class WinEDA_PinSheetPropertiesFrame: public wxDialog
/*****************************************************/
{
private:

	WinEDA_SchematicFrame * m_Parent;
	DrawSheetLabelStruct * m_CurrentPinSheet;
	wxRadioBox * m_PinSheetType;
	wxRadioBox * m_PinSheetShape;
	WinEDA_GraphicTextCtrl * m_TextWin;

public:
	// Constructor and destructor
	WinEDA_PinSheetPropertiesFrame(WinEDA_SchematicFrame *parent,
					DrawSheetLabelStruct * curr_pinsheet,
					const wxPoint & framepos = wxPoint(-1,-1) );
	~WinEDA_PinSheetPropertiesFrame(void){};

private:
	void PinSheetPropertiesAccept(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(WinEDA_PinSheetPropertiesFrame, wxDialog)
	EVT_BUTTON(ID_ACCEPT_PINSHEET_PROPERTIES, WinEDA_PinSheetPropertiesFrame::PinSheetPropertiesAccept)
	EVT_BUTTON(ID_CANCEL_PINSHEET_PROPERTIES, WinEDA_PinSheetPropertiesFrame::OnQuit)
END_EVENT_TABLE()


/**********************************************************************************/
WinEDA_PinSheetPropertiesFrame::WinEDA_PinSheetPropertiesFrame(
				WinEDA_SchematicFrame *parent,
				DrawSheetLabelStruct * curr_pinsheet,
				const wxPoint & framepos):
		wxDialog(parent, -1, _("PinSheet Properties:"), framepos, wxSize(340, 220),
				 DIALOG_STYLE)
/**********************************************************************************/
{
wxPoint pos;
wxString number;
wxButton * Button;

	m_Parent = parent;
	Centre();

	wxBoxSizer * MainBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(MainBoxSizer);
	wxBoxSizer * LeftBoxSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer * RightBoxSizer = new wxBoxSizer(wxVERTICAL);
	MainBoxSizer->Add(LeftBoxSizer, 0, wxGROW|wxALL, 5);
	MainBoxSizer->Add(RightBoxSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_CurrentPinSheet = curr_pinsheet;

	/* Creation des boutons de commande */
	Button = new wxButton(this, ID_ACCEPT_PINSHEET_PROPERTIES,
						_("Ok"));
	Button->SetForegroundColour(*wxRED);
	RightBoxSizer->Add(Button, 0, wxGROW|wxALL, 5);

	Button = new wxButton(this, ID_CANCEL_PINSHEET_PROPERTIES,
						_("Cancel"));
	Button->SetForegroundColour(*wxBLUE);
	RightBoxSizer->Add(Button, 0, wxGROW|wxALL, 5);

	m_TextWin = new WinEDA_GraphicTextCtrl(this, _("Text:"),
				m_CurrentPinSheet->m_Text, m_CurrentPinSheet->m_Size.x,
				g_UnitMetric , LeftBoxSizer, 200);

	// Selection de la forme :
	m_PinSheetShape = new wxRadioBox(this, -1, _("PinSheet Shape:"),
				wxDefaultPosition, wxSize(-1,-1),
				NBSHAPES, shape_list, 1);
	m_PinSheetShape->SetSelection( m_CurrentPinSheet->m_Shape );
	LeftBoxSizer->Add(m_PinSheetShape, 0, wxGROW|wxALL, 5);

	GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
}


/************************************************************************/
void  WinEDA_PinSheetPropertiesFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
/************************************************************************/
{
    // true is to force the frame to close
    Close(true);
}


/***********************************************************************************/
void WinEDA_PinSheetPropertiesFrame::PinSheetPropertiesAccept(wxCommandEvent& event)
/***********************************************************************************/
{
	m_CurrentPinSheet->m_Text = m_TextWin->GetText();
	m_CurrentPinSheet->m_Size.x = m_CurrentPinSheet->m_Size.y = m_TextWin->GetTextSize();

	m_CurrentPinSheet->m_Shape = m_PinSheetShape->GetSelection();
	Close(TRUE);
}

/*****************************************************************/
/*  Routine de sortie du Menu d'Edition Des NETS (Labels) SHEET  */
/*****************************************************************/
static void ExitPinSheet(WinEDA_DrawFrame * frame, wxDC * DC)
{
DrawSheetLabelStruct * SheetLabel = (DrawSheetLabelStruct *)
		frame->GetScreen()->m_CurrentItem;

	if ( SheetLabel == NULL ) return;

	if ( SheetLabel->m_Flags & IS_NEW )
		{ /* Nouveau Placement en cours, on l'efface */
		RedrawOneStruct(frame->DrawPanel, DC, SheetLabel, g_XorMode);
		delete SheetLabel;
		}
	else
		{
		RedrawOneStruct(frame->DrawPanel, DC, SheetLabel, GR_DEFAULT_DRAWMODE);
		SheetLabel->m_Flags = 0;
		}

	frame->GetScreen()->m_CurrentItem = NULL;
	frame->GetScreen()->ManageCurseur = NULL;
	frame->GetScreen()->ForceCloseManageCurseur = NULL;
}

/* Cette routine place un nouveau NetSheet ou place un ancien en cours
de deplacement
	Si le NetSheet est nouveau, il est pointe par NewSheetLabel
*/
void DrawSheetLabelStruct::Place( WinEDA_DrawFrame * frame, wxDC * DC)
{
DrawSheetStruct * Sheet = (DrawSheetStruct *) m_Parent;

	if( m_Flags & IS_NEW )	/* ajout a la liste des structures */
		{
		if(Sheet->m_Label == NULL ) Sheet->m_Label = this;
		else
			{
			DrawSheetLabelStruct * pinsheet = Sheet->m_Label;
			while(pinsheet)
				{
				if( pinsheet->Pnext == NULL )
					{
					pinsheet->Pnext = this;
					break;
					}
				pinsheet = (DrawSheetLabelStruct*)pinsheet->Pnext;
				}
			}
		}

	m_Flags = 0;
	m_Pos.x = Sheet->m_Pos.x;
	m_Edge = 0;
	if( frame->GetScreen()->m_Curseur.x > (Sheet->m_Pos.x + (Sheet->m_End.x/2)) )
		{
		m_Edge = 1;
		m_Pos.x = Sheet->m_Pos.x + Sheet->m_End.x;
		}

	m_Pos.y = frame->GetScreen()->m_Curseur.y;
	if( m_Pos.y < Sheet->m_Pos.y ) m_Pos.y = Sheet->m_Pos.y;
	if( m_Pos.y > (Sheet->m_Pos.y+Sheet->m_End.y) )
			m_Pos.y = Sheet->m_Pos.y + Sheet->m_End.y;

	RedrawOneStruct(frame->DrawPanel, DC, Sheet, GR_DEFAULT_DRAWMODE);

	frame->GetScreen()->ManageCurseur = NULL;
	frame->GetScreen()->ForceCloseManageCurseur = NULL;
}


/*******************************************************************************/
void WinEDA_SchematicFrame::StartMove_PinSheet(DrawSheetLabelStruct * SheetLabel,
			wxDC * DC)
/*******************************************************************************/
/* Initialise un deplacement de NetSheet */
{
	NetSheetTextSize = SheetLabel->m_Size;
	CurrentTypeLabel = SheetLabel->m_Shape;
	SheetLabel->m_Flags |= IS_MOVED;

	GetScreen()->ManageCurseur = Move_PinSheet;
	GetScreen()->ForceCloseManageCurseur = ExitPinSheet;
	GetScreen()->ManageCurseur(DrawPanel, DC, TRUE);
}

/**********************************************************************/
/* Routine de deplacement du  NetSheet actif selon la position souris */
/**********************************************************************/

static void Move_PinSheet(WinEDA_DrawPanel * panel, wxDC * DC, bool erase)
{
DrawSheetLabelStruct * SheetLabel = (DrawSheetLabelStruct *)
		panel->m_Parent->GetScreen()->m_CurrentItem;

	if ( SheetLabel == NULL ) return;

DrawSheetStruct * Sheet = (DrawSheetStruct *) SheetLabel->m_Parent;

	if ( Sheet == NULL ) return;
	if( erase )
		RedrawOneStruct(panel, DC, SheetLabel, g_XorMode);

	SheetLabel->m_Edge = 0;
	SheetLabel->m_Pos.x = Sheet->m_Pos.x;
	if( panel->m_Parent->GetScreen()->m_Curseur.x > (Sheet->m_Pos.x + (Sheet->m_End.x/2)) )
		{
		SheetLabel->m_Edge = 1;
		SheetLabel->m_Pos.x = Sheet->m_Pos.x + Sheet->m_End.x;
		}

	SheetLabel->m_Pos.y = panel->m_Parent->GetScreen()->m_Curseur.y;
	if( SheetLabel->m_Pos.y < Sheet->m_Pos.y )
			SheetLabel->m_Pos.y = Sheet->m_Pos.y;
	if( SheetLabel->m_Pos.y > (Sheet->m_Pos.y+Sheet->m_End.y) )
			SheetLabel->m_Pos.y = Sheet->m_Pos.y + Sheet->m_End.y;

	RedrawOneStruct(panel, DC, SheetLabel, g_XorMode);
}


/***************************************************************************/
void WinEDA_SchematicFrame::Edit_PinSheet(DrawSheetLabelStruct * SheetLabel,
					wxDC * DC)
/***************************************************************************/
/* Modification du texte d'un net sheet */
{
	if( SheetLabel == NULL) return;

	RedrawOneStruct(DrawPanel, DC, SheetLabel, g_XorMode);

WinEDA_PinSheetPropertiesFrame * frame =
		new WinEDA_PinSheetPropertiesFrame(this, SheetLabel);

	frame->ShowModal(); frame->Destroy();

	RedrawOneStruct(DrawPanel, DC, SheetLabel, GR_DEFAULT_DRAWMODE);
}


/***************************************************************/
DrawSheetLabelStruct * WinEDA_SchematicFrame::Create_PinSheet(
		DrawSheetStruct * Sheet, wxDC * DC)
/**************************************************************/
/* Addition d'un nouveau PinSheet sur la feuille selectionnee, a l'endroit
	pointe par la souris
*/
{
wxString Line, Text;
DrawSheetLabelStruct * NewSheetLabel;

	switch(CurrentTypeLabel)
		{
		default: CurrentTypeLabel = NET_INPUT;
		case NET_INPUT: Text = wxT("Pin Input: ");
			break;

		case NET_OUTPUT: Text = wxT("Pin Output: ");
			break;

		case NET_BIDI: Text = wxT("Pin BiDi: ");
			break;

		case NET_TRISTATE: Text = wxT("Pin TriStat: ");
			break;

		case NET_UNSPECIFIED: Text = wxT("Pin Unspec.: ");
			break;

		}

	Get_Message(Text,Line, this);
	if( Line.IsEmpty() ) return NULL;

	SetFlagModify(GetScreen());

	/* Creation en memoire */
	NewSheetLabel = new DrawSheetLabelStruct(Sheet, wxPoint(0,0), Line);
	NewSheetLabel->m_Flags = IS_NEW;
	NewSheetLabel->m_Size = NetSheetTextSize;
	NewSheetLabel->m_Shape = CurrentTypeLabel;

	GetScreen()->m_CurrentItem = NewSheetLabel;

	GetScreen()->ManageCurseur = Move_PinSheet;
	GetScreen()->ForceCloseManageCurseur = ExitPinSheet;
	GetScreen()->ManageCurseur(DrawPanel, DC, TRUE);

	return NewSheetLabel;
}


/*****************************************************************************/
DrawSheetLabelStruct *
	WinEDA_SchematicFrame::Import_PinSheet(DrawSheetStruct * Sheet, wxDC * DC)
/*****************************************************************************/
/* Permet de creer automatiquement les Sheet Labels a partir des Labels Globaux
de la feuille de sous hierarchie correspondante
*/
{
SCH_SCREEN * Window;
EDA_BaseStruct *DrawStruct;
DrawSheetLabelStruct * NewSheetLabel, * SheetLabel = NULL;
DrawGlobalLabelStruct * GLabel = NULL;

	Window = (SCH_SCREEN *) Sheet->m_Son;
	if ( Window == NULL )
		{
		DisplayError(this, wxT("No Subhierarchy"), 10); return NULL;
		}

	DrawStruct = Window->EEDrawList;
	GLabel = NULL;
	for (; DrawStruct != NULL; DrawStruct = DrawStruct->Pnext )
	{
		if( DrawStruct->m_StructType != DRAW_GLOBAL_LABEL_STRUCT_TYPE )
			continue;
		GLabel = (DrawGlobalLabelStruct *) DrawStruct;

		/* Ici un G-Label a ete trouve: y a t-il un SheetLabel correspondant */
		SheetLabel = Sheet->m_Label;
		for( ;SheetLabel != NULL; SheetLabel = (DrawSheetLabelStruct*) SheetLabel->Pnext )
		{
			if( SheetLabel->m_Text.CmpNoCase(GLabel->m_Text) == 0 )
			{
				break;
			}
		}
		/* Ici si SheetLabel == NULL le G-Label n'a pas de SheetLabel corresp */
		if( SheetLabel == NULL ) break;
	}

	if( (GLabel == NULL ) || SheetLabel )
	{
		DisplayError(this, _("No New Global Label found"), 10);
		return NULL;
	}

	 /* Ici G-Label n'a pas de SheetLabel corresp, on va le creer */

	SetFlagModify(GetScreen());
	/* Creation en memoire */
	NewSheetLabel = new DrawSheetLabelStruct(Sheet, wxPoint(0,0), GLabel->m_Text);
	NewSheetLabel->m_Flags = IS_NEW;
	NewSheetLabel->m_Size = NetSheetTextSize;
	CurrentTypeLabel = NewSheetLabel->m_Shape = GLabel->m_Shape;

	GetScreen()->m_CurrentItem = NewSheetLabel;
	GetScreen()->ManageCurseur = Move_PinSheet;
	GetScreen()->ForceCloseManageCurseur = ExitPinSheet;
	Move_PinSheet(DrawPanel, DC, FALSE);

	return NewSheetLabel;
}


/**************************************************************/
void WinEDA_SchematicFrame::DeleteSheetLabel(wxDC * DC,
		DrawSheetLabelStruct * SheetLabelToDel)
/**************************************************************/
/*
 Routine de suppression de 1 Structure type (DrawSheetLabelStruct.
Cette Structure ne peut etre mise en pile "undelete" car il ne serait pas
possible de la ratacher a la 'DrawSheetStruct' d'origine
si DC != NULL, effacement a l'ecran du dessin
*/
{
EDA_BaseStruct * DrawStruct;
DrawSheetLabelStruct* SheetLabel, *NextLabel;

	if ( DC ) RedrawOneStruct(DrawPanel, DC, SheetLabelToDel, g_XorMode);

	/* Recherche de la DrawSheetStruct d'origine */
	DrawStruct = SheetLabelToDel->m_Parent;

	if (DrawStruct)	// Modification du chainage
	{
		if (DrawStruct->m_StructType != DRAW_SHEET_STRUCT_TYPE)
		{
			DisplayError(this, wxT("DeleteSheetLabel error: m_Parent != DRAW_SHEET_STRUCT_TYPE"));
			return;
		}

		/* suppression chainage */
		SheetLabel = ((DrawSheetStruct *) DrawStruct)->m_Label;
		if( SheetLabel == SheetLabelToDel )
			((DrawSheetStruct *) DrawStruct)->m_Label =
					(DrawSheetLabelStruct *)SheetLabel->Pnext;

		else while( SheetLabel )/* Examen de la liste dependante et suppression chainage */
		{
			NextLabel = (DrawSheetLabelStruct *)SheetLabel->Pnext;
			if( NextLabel == SheetLabelToDel )
			{
				SheetLabel->Pnext = NextLabel->Pnext;
				break;;
			}
			SheetLabel = NextLabel;
		}
	}

	delete SheetLabelToDel;
}



