/////////////////////////////////////////////////////////////////////////////
// Name:        dialog_build_BOM.cpp
// Purpose:     
// Author:      jean-pierre Charras
// Modified by: 
// Created:     01/15/06 18:18:44
// RCS-ID:      
// Copyright:   GNU license
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 01/15/06 18:18:44

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "dialog_build_BOM.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "fctsys.h"

////@begin includes
////@end includes

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
#include "netlist.h"

#include "dialog_build_BOM.h"


#include "protos.h"

////@begin XPM images
////@end XPM images


/* Structures pour memo et liste des elements */
typedef struct ListLabel
{
	int m_StructType;
	void * m_Label;
	int m_SheetNumber;
} ListLabel;


/* fonctions locales */
static int GenListeGLabels( ListLabel * List );
static int ListTriComposantByRef(EDA_SchComponentStruct **Objet1,
									EDA_SchComponentStruct **Objet2);
static int ListTriComposantByVal(EDA_SchComponentStruct **Objet1,
									EDA_SchComponentStruct **Objet2);
static int ListTriGLabelBySheet(ListLabel *Objet1, ListLabel *Objet2);
static int ListTriGLabelByVal(ListLabel *Objet1, ListLabel *Objet2);
static void DeleteSubCmp( EDA_BaseStruct ** List , int NbItems);

static int PrintListeGLabel( FILE *f, ListLabel *List, int NbItems);

/* Local variables */
static bool s_ListByRef = TRUE;
static bool s_ListByValue = TRUE;
static bool s_ListWithSubCmponents;
static bool s_ListHierarchicalPinByName;
static bool s_ListBySheet;
static bool s_BrowsList;
static int s_OutputFormOpt;
static int s_OutputSeparatorOpt;
static bool s_Add_F1_state;
static bool s_Add_F2_state;
static bool s_Add_F3_state;
static bool s_Add_F4_state;
static bool s_Add_F5_state;
static bool s_Add_F6_state;
static bool s_Add_F7_state;
static bool s_Add_F8_state;
static bool * s_AddFieldList[] = {
	& s_Add_F1_state,
	& s_Add_F2_state,
	& s_Add_F3_state,
	& s_Add_F4_state,
	& s_Add_F5_state,
	& s_Add_F6_state,
	& s_Add_F7_state,
	& s_Add_F8_state,
	NULL
};


#define OPTION_BOM_FORMAT wxT("BomFormat")
#define OPTION_BOM_SEPARATOR wxT("BomExportSeparator")
#define OPTION_BOM_ADD_FIELD wxT("BomAddField")

/* list of separators used in bom export to spreadsheet
(selected by s_OutputSeparatorOpt, and s_OutputSeparatorOpt radiobox) */
static char s_ExportSeparator[] = ("\t;,.");
static char s_ExportSeparatorSymbol;
/*!
 * WinEDA_Build_BOM_Frame type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WinEDA_Build_BOM_Frame, wxDialog )

/*!
 * WinEDA_Build_BOM_Frame event table definition
 */

BEGIN_EVENT_TABLE( WinEDA_Build_BOM_Frame, wxDialog )

////@begin WinEDA_Build_BOM_Frame event table entries
    EVT_CLOSE( WinEDA_Build_BOM_Frame::OnCloseWindow )

    EVT_RADIOBOX( ID_RADIOBOX_SELECT_FORMAT, WinEDA_Build_BOM_Frame::OnRadioboxSelectFormatSelected )

    EVT_BUTTON( wxID_OK, WinEDA_Build_BOM_Frame::OnOkClick )

    EVT_BUTTON( wxID_EXIT, WinEDA_Build_BOM_Frame::OnExitClick )

////@end WinEDA_Build_BOM_Frame event table entries

END_EVENT_TABLE()

/***************************************************************/
void InstallToolsFrame(WinEDA_DrawFrame *parent, wxPoint & pos)
/***************************************************************/
{
	WinEDA_Build_BOM_Frame * frame = new WinEDA_Build_BOM_Frame(parent);
	frame->ShowModal(); frame->Destroy();
}

/*!
 * WinEDA_Build_BOM_Frame constructors
 */

WinEDA_Build_BOM_Frame::WinEDA_Build_BOM_Frame( )
{
}

WinEDA_Build_BOM_Frame::WinEDA_Build_BOM_Frame( WinEDA_DrawFrame* parent,
			wxWindowID id, const wxString& caption, const wxPoint& pos,
			const wxSize& size, long style )
{
	m_Parent = parent;

	/* Get options */
	s_OutputFormOpt = m_Parent->m_Parent->m_EDA_Config->Read(OPTION_BOM_FORMAT, (long) 0);
	s_OutputSeparatorOpt = m_Parent->m_Parent->m_EDA_Config->Read(OPTION_BOM_SEPARATOR, (long) 0);
	long addfields = m_Parent->m_Parent->m_EDA_Config->Read(OPTION_BOM_ADD_FIELD, (long) 0);
	for ( int ii = 0, bitmask = 1; s_AddFieldList[ii] != NULL; ii++ )
	{
		if ( (addfields & bitmask) )  * s_AddFieldList[ii] = true;
		else * s_AddFieldList[ii] = false;
		
		bitmask <<= 1;
	}
	
    Create(parent, id, caption, pos, size, style);
	
	m_OutputFormCtrl->SetSelection(s_OutputFormOpt);
	m_OutputSeparatorCtrl->SetSelection(s_OutputSeparatorOpt);
	if ( s_OutputFormOpt == 1 )
		m_OutputSeparatorCtrl->Enable(true);
	else 
		m_OutputSeparatorCtrl->Enable(false);

}

/*!
 * WinEDA_Build_BOM_Frame creator
 */

bool WinEDA_Build_BOM_Frame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WinEDA_Build_BOM_Frame member initialisation
    m_ListCmpbyRefItems = NULL;
    m_ListSubCmpItems = NULL;
    m_ListCmpbyValItems = NULL;
    m_GenListLabelsbyVal = NULL;
    m_GenListLabelsbySheet = NULL;
    m_OutputFormCtrl = NULL;
    m_OutputSeparatorCtrl = NULL;
    m_GetListBrowser = NULL;
    m_FieldsToAppendListSizer = NULL;
    m_AddField1 = NULL;
    m_AddField2 = NULL;
    m_AddField3 = NULL;
    m_AddField4 = NULL;
    m_AddField5 = NULL;
    m_AddField6 = NULL;
    m_AddField7 = NULL;
    m_AddField8 = NULL;
////@end WinEDA_Build_BOM_Frame member initialisation

////@begin WinEDA_Build_BOM_Frame creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end WinEDA_Build_BOM_Frame creation
    return TRUE;
}

/*!
 * Control creation for WinEDA_Build_BOM_Frame
 */

void WinEDA_Build_BOM_Frame::CreateControls()
{    
	SetFont(*g_DialogFont);
	
////@begin WinEDA_Build_BOM_Frame content construction
    // Generated by DialogBlocks, 01/07/2007 21:25:39 (unregistered)

    WinEDA_Build_BOM_Frame* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxStaticBox* itemStaticBoxSizer5Static = new wxStaticBox(itemDialog1, wxID_ANY, _("List items : "));
    wxStaticBoxSizer* itemStaticBoxSizer5 = new wxStaticBoxSizer(itemStaticBoxSizer5Static, wxVERTICAL);
    itemBoxSizer4->Add(itemStaticBoxSizer5, 0, wxGROW|wxALL, 5);

    m_ListCmpbyRefItems = new wxCheckBox( itemDialog1, ID_CHECKBOX, _("Components by Reference"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_ListCmpbyRefItems->SetValue(true);
    itemStaticBoxSizer5->Add(m_ListCmpbyRefItems, 0, wxALIGN_LEFT|wxALL, 5);

    m_ListSubCmpItems = new wxCheckBox( itemDialog1, ID_CHECKBOX2, _("Sub Components (i.e U2A, U2B..)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_ListSubCmpItems->SetValue(false);
    itemStaticBoxSizer5->Add(m_ListSubCmpItems, 0, wxALIGN_LEFT|wxALL, 5);

    m_ListCmpbyValItems = new wxCheckBox( itemDialog1, ID_CHECKBOX1, _("Components by Value"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_ListCmpbyValItems->SetValue(true);
    itemStaticBoxSizer5->Add(m_ListCmpbyValItems, 0, wxALIGN_LEFT|wxALL, 5);

    m_GenListLabelsbyVal = new wxCheckBox( itemDialog1, ID_CHECKBOX3, _("Hierachy Pins by name"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_GenListLabelsbyVal->SetValue(false);
    itemStaticBoxSizer5->Add(m_GenListLabelsbyVal, 0, wxALIGN_LEFT|wxALL, 5);

    m_GenListLabelsbySheet = new wxCheckBox( itemDialog1, ID_CHECKBOX4, _("Hierachy Pins by Sheets"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_GenListLabelsbySheet->SetValue(false);
    itemStaticBoxSizer5->Add(m_GenListLabelsbySheet, 0, wxALIGN_LEFT|wxALL, 5);

    wxArrayString m_OutputFormCtrlStrings;
    m_OutputFormCtrlStrings.Add(_("List"));
    m_OutputFormCtrlStrings.Add(_("Text for spreadsheet import"));
    m_OutputFormCtrl = new wxRadioBox( itemDialog1, ID_RADIOBOX_SELECT_FORMAT, _("Output format:"), wxDefaultPosition, wxDefaultSize, m_OutputFormCtrlStrings, 1, wxRA_SPECIFY_COLS );
    m_OutputFormCtrl->SetSelection(0);
    itemBoxSizer4->Add(m_OutputFormCtrl, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_OutputSeparatorCtrlStrings;
    m_OutputSeparatorCtrlStrings.Add(_("Tab"));
    m_OutputSeparatorCtrlStrings.Add(_(";"));
    m_OutputSeparatorCtrlStrings.Add(_(","));
    m_OutputSeparatorCtrl = new wxRadioBox( itemDialog1, ID_RADIOBOX_SEPARATOR, _("Field separator for spreadsheet import:"), wxDefaultPosition, wxDefaultSize, m_OutputSeparatorCtrlStrings, 1, wxRA_SPECIFY_ROWS );
    m_OutputSeparatorCtrl->SetSelection(0);
    itemBoxSizer4->Add(m_OutputSeparatorCtrl, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticBox* itemStaticBoxSizer13Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer13 = new wxStaticBoxSizer(itemStaticBoxSizer13Static, wxHORIZONTAL);
    itemBoxSizer4->Add(itemStaticBoxSizer13, 0, wxGROW|wxALL, 5);

    m_GetListBrowser = new wxCheckBox( itemDialog1, ID_CHECKBOX6, _("Launch list browser"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_GetListBrowser->SetValue(false);
    itemStaticBoxSizer13->Add(m_GetListBrowser, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer15, 0, 0, 0);

    wxStaticBox* itemStaticBoxSizer16Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Fields to Add"));
    m_FieldsToAppendListSizer = new wxStaticBoxSizer(itemStaticBoxSizer16Static, wxVERTICAL);
    itemBoxSizer15->Add(m_FieldsToAppendListSizer, 0, wxGROW|wxALL, 5);

    m_AddField1 = new wxCheckBox( itemDialog1, ID_CHECKBOX_FIELD1, _("Add Field 1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddField1->SetValue(false);
    m_FieldsToAppendListSizer->Add(m_AddField1, 0, wxGROW|wxALL, 5);

    m_AddField2 = new wxCheckBox( itemDialog1, ID_CHECKBOX_FIELD2, _("Add Field 2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddField2->SetValue(false);
    m_FieldsToAppendListSizer->Add(m_AddField2, 0, wxGROW|wxALL, 5);

    m_AddField3 = new wxCheckBox( itemDialog1, ID_CHECKBOX_FIELD1, _("Add Field 3"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddField3->SetValue(false);
    m_FieldsToAppendListSizer->Add(m_AddField3, 0, wxGROW|wxALL, 5);

    m_AddField4 = new wxCheckBox( itemDialog1, ID_CHECKBOX_FIELD4, _("Add Field 4"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddField4->SetValue(false);
    m_FieldsToAppendListSizer->Add(m_AddField4, 0, wxGROW|wxALL, 5);

    m_AddField5 = new wxCheckBox( itemDialog1, ID_CHECKBOX_FIELD5, _("Add Field 5"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddField5->SetValue(false);
    m_FieldsToAppendListSizer->Add(m_AddField5, 0, wxGROW|wxALL, 5);

    m_AddField6 = new wxCheckBox( itemDialog1, ID_CHECKBOX_FIELD6, _("Add Field 6"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddField6->SetValue(false);
    m_FieldsToAppendListSizer->Add(m_AddField6, 0, wxGROW|wxALL, 5);

    m_AddField7 = new wxCheckBox( itemDialog1, ID_CHECKBOX_FIELD7, _("Add Field 7"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddField7->SetValue(false);
    m_FieldsToAppendListSizer->Add(m_AddField7, 0, wxGROW|wxALL, 5);

    m_AddField8 = new wxCheckBox( itemDialog1, ID_CHECKBOX_FIELD8, _("Add Field 8"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddField8->SetValue(false);
    m_FieldsToAppendListSizer->Add(m_AddField8, 0, wxGROW|wxALL, 5);

    itemBoxSizer15->Add(5, 5, 0, wxGROW|wxALL, 15);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer15->Add(itemBoxSizer26, 0, wxGROW|wxALL, 5);

    wxButton* itemButton27 = new wxButton( itemDialog1, wxID_OK, _("&Create List"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton27->SetDefault();
    itemButton27->SetForegroundColour(wxColour(166, 0, 0));
    itemBoxSizer26->Add(itemButton27, 0, wxGROW|wxALL, 5);

    wxButton* itemButton28 = new wxButton( itemDialog1, wxID_EXIT, _("&Quit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton28->SetForegroundColour(wxColour(0, 0, 210));
    itemBoxSizer26->Add(itemButton28, 0, wxGROW|wxALL, 5);

    // Set validators
    m_ListCmpbyRefItems->SetValidator( wxGenericValidator(& s_ListByRef) );
    m_ListSubCmpItems->SetValidator( wxGenericValidator(& s_ListWithSubCmponents) );
    m_ListCmpbyValItems->SetValidator( wxGenericValidator(& s_ListByValue) );
    m_GenListLabelsbyVal->SetValidator( wxGenericValidator(& s_ListHierarchicalPinByName) );
    m_GenListLabelsbySheet->SetValidator( wxGenericValidator(& s_ListBySheet) );
    m_OutputFormCtrl->SetValidator( wxGenericValidator(& s_OutputFormOpt) );
    m_OutputSeparatorCtrl->SetValidator( wxGenericValidator(& s_OutputSeparatorOpt) );
    m_GetListBrowser->SetValidator( wxGenericValidator(& s_BrowsList) );
    m_AddField1->SetValidator( wxGenericValidator(& s_Add_F1_state) );
    m_AddField2->SetValidator( wxGenericValidator(& s_Add_F2_state) );
    m_AddField3->SetValidator( wxGenericValidator(& s_Add_F3_state) );
    m_AddField4->SetValidator( wxGenericValidator(& s_Add_F4_state) );
    m_AddField5->SetValidator( wxGenericValidator(& s_Add_F5_state) );
    m_AddField6->SetValidator( wxGenericValidator(& s_Add_F6_state) );
    m_AddField7->SetValidator( wxGenericValidator(& s_Add_F7_state) );
    m_AddField8->SetValidator( wxGenericValidator(& s_Add_F8_state) );
////@end WinEDA_Build_BOM_Frame content construction
}

/*!
 * Should we show tooltips?
 */

bool WinEDA_Build_BOM_Frame::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WinEDA_Build_BOM_Frame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WinEDA_Build_BOM_Frame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end WinEDA_Build_BOM_Frame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WinEDA_Build_BOM_Frame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WinEDA_Build_BOM_Frame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end WinEDA_Build_BOM_Frame icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void WinEDA_Build_BOM_Frame::OnOkClick( wxCommandEvent& event )
{
	GenList();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_EXIT
 */

void WinEDA_Build_BOM_Frame::OnExitClick( wxCommandEvent& event )
{
	SavePreferences();
	EndModal(0);
}

/**********************************************************/
void WinEDA_Build_BOM_Frame::GenList(void)
/**********************************************************/
{
#define EXT_LIST wxT(".lst")
wxString mask, filename;

	s_ListByRef = m_ListCmpbyRefItems->GetValue();
	s_ListByValue = m_ListCmpbyValItems->GetValue();
	s_ListWithSubCmponents = m_ListSubCmpItems->GetValue();
	s_ListHierarchicalPinByName = m_GenListLabelsbyVal->GetValue();
	s_ListBySheet = m_GenListLabelsbySheet->GetValue();
    s_BrowsList = m_GetListBrowser->GetValue();
	s_OutputFormOpt = m_OutputFormCtrl->GetSelection();
	s_OutputSeparatorOpt = m_OutputSeparatorCtrl->GetSelection();
	if ( s_OutputSeparatorOpt < 0 ) s_OutputSeparatorOpt = 0;
	s_ExportSeparatorSymbol = s_ExportSeparator[s_OutputSeparatorOpt];

	m_ListFileName = ScreenSch->m_FileName;
	ChangeFileNameExt(m_ListFileName, EXT_LIST);
	mask = wxT("*"); mask += EXT_LIST;

	filename = EDA_FileSelector(_("Bill of material:"),
					wxEmptyString,				/* Chemin par defaut (ici dir courante) */
					m_ListFileName,	/* nom fichier par defaut, et resultat */
					EXT_LIST,		/* extension par defaut */
					mask,			/* Masque d'affichage */
					this,
					wxFD_SAVE,
					TRUE
					);
	if ( filename.IsEmpty() ) return;
	else m_ListFileName = filename;

	/* Close dialog and show the list, if wanted */
	if ( s_OutputFormOpt == 0)	GenereListeOfItems(m_ListFileName);
	else CreateExportList(m_ListFileName);

	Close();
	
	if ( s_BrowsList )
	{
		wxString editorname = GetEditorName();
		AddDelimiterString(filename);
		ExecuteFile(this, editorname, filename);
	}
}


/****************************************************************************/
void WinEDA_Build_BOM_Frame::CreateExportList(const wxString & FullFileName)
/****************************************************************************/
/*
	Print a list of components, in a form which can be imported by a spreadsheet
	form is;
	cmp name;cmp val; fields;
*/
{
FILE *f;
EDA_BaseStruct ** List;
int NbItems;
wxString msg;
	
	/* Creation de la liste des elements */
	if ((f = wxFopen(FullFileName, wxT("wt"))) == NULL)
	{
		msg = _("Failed to open file "); msg << FullFileName;
		DisplayError(this, msg);
		return;
	}

	NbItems = GenListeCmp(NULL );
	if ( NbItems )
	{
		List = (EDA_BaseStruct **)
				MyZMalloc( NbItems * sizeof(EDA_BaseStruct **) );
		if (List == NULL )
		{
			fclose(f);  return;
		}

		GenListeCmp(List);

		/*  sort component list */
		qsort( List, NbItems, sizeof( EDA_BaseStruct * ),
				(int(*)(const void*, const void*))ListTriComposantByRef);

		if( ! s_ListWithSubCmponents ) DeleteSubCmp(List, NbItems);

		/* create the file */
		PrintListeCmpByRef(f, List, NbItems, TRUE);

		MyFree( List );
	}

	fclose(f);
}



/****************************************************************************/
void WinEDA_Build_BOM_Frame::GenereListeOfItems(const wxString & FullFileName)
/****************************************************************************/
/*
	Routine principale pour la creation des listings ( composants et/ou labels
	globaux et "sheet labels" )
*/
{
FILE *f;
EDA_BaseStruct ** List;
ListLabel * ListOfLabels;
int NbItems;
char Line[1024];
wxString msg;
	
	/* Creation de la liste des elements */
	if ((f = wxFopen(FullFileName, wxT("wt"))) == NULL)
	{
		msg = _("Failed to open file "); msg << FullFileName;
		DisplayError(this, msg);
		return;
	}

	NbItems = GenListeCmp(NULL );
	if ( NbItems )
	{
		List = (EDA_BaseStruct **)
				MyZMalloc( NbItems * sizeof(EDA_BaseStruct **) );
		if (List == NULL )
		{
			fclose(f);  return;
		}

		GenListeCmp(List);

		/* generation du fichier listing */
		DateAndTime(Line);
		wxString Title = g_Main_Title + wxT(" ") + GetBuildVersion();
		fprintf( f, "%s  >> Creation date: %s\n", CONV_TO_UTF8(Title), Line );

		/* Tri et impression de la liste des composants */

		qsort( List, NbItems, sizeof( EDA_BaseStruct * ),
				(int(*)(const void*, const void*))ListTriComposantByRef);

		if( ! s_ListWithSubCmponents ) DeleteSubCmp(List, NbItems);

		if( s_ListByRef )
		{
			PrintListeCmpByRef(f, List, NbItems);
		}

		if( s_ListByValue )
		{
			qsort( List, NbItems, sizeof( EDA_BaseStruct * ),
					(int(*)(const void*, const void*))ListTriComposantByVal);
			PrintListeCmpByVal(f, List, NbItems);
		}
		MyFree( List );
	}

	/***************************************/
	/* Generation liste des Labels globaux */
	/***************************************/

	NbItems = GenListeGLabels( NULL );
	if ( NbItems )
	{
		ListOfLabels = (ListLabel *) MyZMalloc( NbItems * sizeof(ListLabel) );
		if (ListOfLabels == NULL )
		{
			  fclose(f); return;
		}
		
		GenListeGLabels(ListOfLabels);

		/* Tri de la liste */
		if( s_ListBySheet )
		{
			qsort( ListOfLabels, NbItems, sizeof( ListLabel ),
				(int(*)(const void*, const void*))ListTriGLabelBySheet);

			msg.Printf( _("\n#Glob labels ( order = Sheet Number ) count = %d\n"), NbItems);
			fprintf( f, "%s", CONV_TO_UTF8(msg));
			PrintListeGLabel(f, ListOfLabels, NbItems);
		}

		if( s_ListHierarchicalPinByName )
		{
			qsort( ListOfLabels, NbItems, sizeof( ListLabel ),
				(int(*)(const void*, const void*))ListTriGLabelByVal);

			msg.Printf(_("\n#Glob labels ( order = Alphab. ) count = %d\n\n"), NbItems);
			fprintf( f, "%s", CONV_TO_UTF8(msg));
			PrintListeGLabel(f, ListOfLabels, NbItems);
		}
		MyFree( ListOfLabels );
	}

	msg = _("\n#End List\n");
	fprintf( f, "%s", CONV_TO_UTF8(msg));
	fclose(f);
}



/****************************************/
int GenListeCmp( EDA_BaseStruct ** List )
/****************************************/
/* Routine de generation de la liste des elements utiles du dessin
	Si List == NULL: comptage des elements
	Sinon remplissage de la liste
	Initialise "FlagControlMulti" a SheetNumber pour la sortie des listes
	et m_Father comme pointeur sur la sheet d'appartenance
*/
{
int ItemCount = 0;
EDA_BaseStruct *DrawList;
EDA_SchComponentStruct *DrawLibItem;
BASE_SCREEN * screen;

	/* Build the screen list */
	EDA_ScreenList ScreenList(NULL);

	for ( screen = ScreenList.GetFirst(); screen != NULL; screen = ScreenList.GetNext() )
	{
		DrawList = screen->EEDrawList;
		while ( DrawList )
		{
			switch( DrawList->m_StructType )
			{

				case DRAW_LIB_ITEM_STRUCT_TYPE :
					ItemCount++;
					DrawLibItem = (EDA_SchComponentStruct *) DrawList;
					DrawLibItem->m_FlagControlMulti = screen->m_SheetNumber;
					DrawLibItem->m_Parent = screen;
					if( List )
						{
						*List = DrawList; List++;
						}
					break;


				default: break;
			}
			DrawList = DrawList->Pnext;
		}
	}
	return ( ItemCount );
}

/*********************************************/
static int GenListeGLabels( ListLabel * List )
/*********************************************/
/* Count the Glabels, or fill the list Listwith Glabel pointers 
	If List == NULL: Item count only
	Else fill list of Glabels
*/
{
int ItemCount = 0;
EDA_BaseStruct *DrawList;
DrawSheetLabelStruct *SheetLabel;
BASE_SCREEN * screen;

	/* Build the screen list */
	EDA_ScreenList ScreenList(NULL);

	for ( screen = ScreenList.GetFirst(); screen != NULL; screen = ScreenList.GetNext() )
	{
		DrawList = screen->EEDrawList;
		while ( DrawList )
		{
			switch( DrawList->m_StructType )
			{
				case DRAW_GLOBAL_LABEL_STRUCT_TYPE :
					ItemCount++;
					if( List )
					{
						List->m_StructType = DRAW_GLOBAL_LABEL_STRUCT_TYPE;
						List->m_SheetNumber = screen->m_SheetNumber;
						List->m_Label = DrawList;
						List++;
					}
					break;
				
				case DRAW_SHEET_STRUCT_TYPE :
				{
					#define Sheet ((DrawSheetStruct * ) DrawList)
					SheetLabel= Sheet->m_Label;
					while( SheetLabel != NULL )
					{
						if ( List )
						{
							List->m_StructType = DRAW_SHEETLABEL_STRUCT_TYPE;
							List->m_SheetNumber = screen->m_SheetNumber;
							List->m_Label = SheetLabel;
							List++;
						}
						ItemCount++;
						SheetLabel = (DrawSheetLabelStruct*)(SheetLabel->Pnext);
					}
					break;
				}

				default: break;
			}
			DrawList = DrawList->Pnext;
		}
	}
	return ( ItemCount );
}

/**********************************************************/
static int ListTriComposantByVal(EDA_SchComponentStruct **Objet1,
							EDA_SchComponentStruct **Objet2)
/**********************************************************/
 /* Routine de comparaison pour le tri du Tableau par qsort()
	Les composants sont tries
		par valeur
		si meme valeur: par reference
			si meme valeur: par numero d'unite

*/
{
int ii;
const wxString * Text1, *Text2;

	if( (*Objet1 == NULL) && (*Objet2 == NULL ) ) return(0);
	if( *Objet1 == NULL) return(-1);
	if( *Objet2 == NULL) return(1);

	Text1 = &(*Objet1)->m_Field[VALUE].m_Text;
	Text2 = &(*Objet2)->m_Field[VALUE].m_Text;
	ii = Text1->CmpNoCase(*Text2);

	if( ii == 0 )
	{
		Text1 = &(*Objet1)->m_Field[REFERENCE].m_Text;
		Text2 = &(*Objet2)->m_Field[REFERENCE].m_Text;
		ii = Text1->CmpNoCase(*Text2);
	}

	if ( ii == 0 )
	{
		ii = (*Objet1)->m_Multi - (*Objet2)->m_Multi;
	}

	return(ii);
}

/**********************************************************/
static int ListTriComposantByRef(EDA_SchComponentStruct **Objet1,
							EDA_SchComponentStruct **Objet2)
/**********************************************************/
 /* Routine de comparaison pour le tri du Tableau par qsort()
	Les composants sont tries
		par reference
		si meme referenece: par valeur
			si meme valeur: par numero d'unite

*/
{
int ii;
const wxString * Text1, *Text2;

	if( (*Objet1 == NULL) && (*Objet2 == NULL ) ) return(0);
	if( *Objet1 == NULL) return(-1);
	if( *Objet2 == NULL) return(1);

	Text1 = &(*Objet1)->m_Field[REFERENCE].m_Text;
	Text2 = &(*Objet2)->m_Field[REFERENCE].m_Text;
	ii = Text1->CmpNoCase(*Text2);

	if( ii == 0 )
	{
		Text1 = &(*Objet1)->m_Field[VALUE].m_Text;
		Text2 = &(*Objet2)->m_Field[VALUE].m_Text;
		ii = Text1->CmpNoCase(*Text2);
	}

	if ( ii == 0 )
	{
		ii = (*Objet1)->m_Multi - (*Objet2)->m_Multi;
	}

	return(ii);
}

/******************************************************************/
static int ListTriGLabelByVal(ListLabel *Objet1, ListLabel *Objet2)
/*******************************************************************/
/* Routine de comparaison pour le tri du Tableau par qsort()
	Les labels sont tries
		par comparaison ascii
		si meme valeur: par numero de sheet

*/
{
int ii;
const wxString * Text1, *Text2;

	if( Objet1->m_StructType == DRAW_SHEETLABEL_STRUCT_TYPE )
		Text1 = &((DrawSheetLabelStruct *)Objet1->m_Label)->m_Text;
	else
		Text1 = &((DrawTextStruct *)Objet1->m_Label)->m_Text;

	if( Objet2->m_StructType == DRAW_SHEETLABEL_STRUCT_TYPE )
		Text2 = &((DrawSheetLabelStruct *)Objet2->m_Label)->m_Text;
	else
		Text2 = &((DrawTextStruct *)Objet2->m_Label)->m_Text;
	ii = Text1->CmpNoCase(*Text2);

	if ( ii == 0 )
		{
		ii = Objet1->m_SheetNumber - Objet2->m_SheetNumber;
		}

	return(ii);
}

/*******************************************************************/
static int ListTriGLabelBySheet(ListLabel *Objet1, ListLabel *Objet2)
/*******************************************************************/
/* Routine de comparaison pour le tri du Tableau par qsort()
	Les labels sont tries
		par sheet number
		si meme valeur, par ordre alphabetique

*/
{
int ii;
const wxString * Text1, *Text2;

	ii = Objet1->m_SheetNumber - Objet2->m_SheetNumber;

	if ( ii == 0 )
	{
		if( Objet1->m_StructType == DRAW_SHEETLABEL_STRUCT_TYPE )
			Text1 = &((DrawSheetLabelStruct *)Objet1->m_Label)->m_Text;
		else
			Text1 = &((DrawTextStruct *)Objet1->m_Label)->m_Text;

		if( Objet2->m_StructType == DRAW_SHEETLABEL_STRUCT_TYPE )
			Text2 = &((DrawSheetLabelStruct *)Objet2->m_Label)->m_Text;
		else
			Text2 = &((DrawTextStruct *)Objet2->m_Label)->m_Text;
		ii = Text1->CmpNoCase(*Text2);
	}

	return(ii);
}



/**************************************************************/
static void DeleteSubCmp( EDA_BaseStruct ** List, int NbItems )
/**************************************************************/
/* Supprime les sous-composants, c'est a dire les descriptions redonnantes des
boitiers multiples
	La liste des composant doit etre triee par reference et par num d'unite
*/
{
int ii;
EDA_SchComponentStruct * LibItem;
const wxString * OldName = NULL;

	for( ii = 0; ii < NbItems ; ii++ )
	{
		LibItem = (EDA_SchComponentStruct *) List[ii];
		if ( LibItem == NULL ) continue;
		if( OldName )
		{
			if ( OldName->CmpNoCase( LibItem->m_Field[REFERENCE].m_Text ) == 0 )
			{
				List[ii] = NULL;
			}
		}
		OldName = &LibItem->m_Field[REFERENCE].m_Text;
	}
}


/*******************************************************************************************/
void WinEDA_Build_BOM_Frame::PrintFieldData(FILE * f, EDA_SchComponentStruct * DrawLibItem,
		bool CompactForm)
/*******************************************************************************************/
{
wxCheckBox * FieldListCtrl[FIELD8-FIELD1+1] = {
    m_AddField1,
    m_AddField2,
    m_AddField3,
    m_AddField4,
    m_AddField5,
    m_AddField6,
    m_AddField7,
    m_AddField8 
	};
int ii;
wxCheckBox * FieldCtrl = FieldListCtrl[0];

	if ( CompactForm ) 
	{
		fprintf(f, "%c%s", s_ExportSeparatorSymbol, 
			CONV_TO_UTF8(DrawLibItem->m_Field[FOOTPRINT].m_Text));
	}

	for ( ii = FIELD1; ii <= FIELD8; ii ++ )
	{
		FieldCtrl = FieldListCtrl[ii-FIELD1];
		if ( FieldCtrl == NULL ) continue;
		if ( ! FieldCtrl->IsChecked() ) continue;
		if ( CompactForm ) fprintf(f, "%c%s", s_ExportSeparatorSymbol,
			CONV_TO_UTF8(DrawLibItem->m_Field[ii].m_Text));
		else fprintf(f, "; %-12s", CONV_TO_UTF8(DrawLibItem->m_Field[ii].m_Text));
	}
}

/*********************************************************************************************/
int WinEDA_Build_BOM_Frame::PrintListeCmpByRef( FILE * f, EDA_BaseStruct ** List, int NbItems,
	bool CompactForm )
/*********************************************************************************************/
/* Print the B.O.M sorted by reference
*/
{
int ii, Multi, Unit;
EDA_BaseStruct *DrawList;
EDA_SchComponentStruct *DrawLibItem;
EDA_LibComponentStruct *Entry;
char NameCmp[80];
wxString msg;
	
	if ( CompactForm )
	{
		fprintf(f, "ref%cvalue%csheet number%csheet name%cfootprint",
			s_ExportSeparatorSymbol, s_ExportSeparatorSymbol,
			s_ExportSeparatorSymbol, s_ExportSeparatorSymbol );
		wxCheckBox * FieldListCtrl[FIELD8-FIELD1+1] = {
			m_AddField1,
			m_AddField2,
			m_AddField3,
			m_AddField4,
			m_AddField5,
			m_AddField6,
			m_AddField7,
			m_AddField8 
			};
			for ( ii = FIELD1; ii <= FIELD8; ii ++ )
			{
			wxCheckBox * FieldCtrl = FieldListCtrl[ii-FIELD1];
				if ( FieldCtrl == NULL ) continue;
				if ( ! FieldCtrl->IsChecked() ) continue;
				msg = _("Field");
				fprintf(f, "%c%s%d", s_ExportSeparatorSymbol, CONV_TO_UTF8(msg), ii - FIELD1 + 1);
			}
		fprintf( f, "\n");
	}

	else {
		msg = _("\n#Cmp ( order = Reference )");
		if ( s_ListWithSubCmponents ) msg << _(" (with SubCmp)");
		fprintf( f, "%s\n", CONV_TO_UTF8(msg));
	}

	for ( ii = 0; ii < NbItems; ii++ )
	{
		DrawList = List[ii];

		if( DrawList == NULL ) continue;
		if( DrawList->m_StructType != DRAW_LIB_ITEM_STRUCT_TYPE ) continue;

		DrawLibItem = (EDA_SchComponentStruct *) DrawList;
		if( DrawLibItem->m_Field[REFERENCE].m_Text[0] == '#' ) continue;

		Multi = 0; Unit = ' ';
		Entry = FindLibPart(DrawLibItem->m_ChipName.GetData(), wxEmptyString, FIND_ROOT);
		if( Entry ) Multi = Entry->m_UnitCount;
		if( (Multi > 1 ) && s_ListWithSubCmponents )
			 Unit = DrawLibItem->m_Multi + 'A' - 1;

		sprintf( NameCmp,"%s", CONV_TO_UTF8(DrawLibItem->m_Field[REFERENCE].m_Text) );
		if ( ! CompactForm || Unit != ' ' ) sprintf( NameCmp+strlen(NameCmp),"%c", Unit);

		if ( CompactForm ) fprintf(f, "%s%c%s", NameCmp, s_ExportSeparatorSymbol,
					CONV_TO_UTF8(DrawLibItem->m_Field[VALUE].m_Text));
		else fprintf(f, "| %-10s %-12s", NameCmp,
					CONV_TO_UTF8(DrawLibItem->m_Field[VALUE].m_Text));

		if ( s_ListWithSubCmponents )
		{
			DrawSheetStruct * sheet = (DrawSheetStruct *)(DrawLibItem->m_Parent);
			wxString sheetname;
			if( sheet && sheet->m_StructType == DRAW_SHEET_STRUCT_TYPE )
				sheetname = sheet->m_SheetName;
			else sheetname = _("Root");
			if ( CompactForm ) 
				fprintf(f, "%c%d;%s", s_ExportSeparatorSymbol, DrawLibItem->m_FlagControlMulti,
					CONV_TO_UTF8(sheetname));
			else fprintf(f, "   (Sheet %.2d: \"%s\")", DrawLibItem->m_FlagControlMulti,
					CONV_TO_UTF8(sheetname));
		}

		PrintFieldData(f, DrawLibItem, CompactForm);

		fprintf(f,"\n");
	}
	if ( ! CompactForm )
	{
		msg = _("#End Cmp\n");
		fprintf(f, CONV_TO_UTF8(msg));
	}
	return(0);
}

/*********************************************************************************************/
int WinEDA_Build_BOM_Frame::PrintListeCmpByVal( FILE * f, EDA_BaseStruct ** List, int NbItems )
/**********************************************************************************************/
{
int ii, Multi;
wxChar Unit;
EDA_BaseStruct *DrawList;
EDA_SchComponentStruct *DrawLibItem;
EDA_LibComponentStruct *Entry;
wxString msg;
	
	msg = _("\n#Cmp ( order = Value )");
	if ( s_ListWithSubCmponents ) msg <<  _(" (with SubCmp)");
	msg << wxT("\n");
	fprintf(f, CONV_TO_UTF8(msg));

	for ( ii = 0; ii < NbItems; ii++ )
		{
		DrawList = List[ii];

		if( DrawList == NULL ) continue;
		if( DrawList->m_StructType != DRAW_LIB_ITEM_STRUCT_TYPE ) continue;

		DrawLibItem = (EDA_SchComponentStruct *) DrawList;
		if( DrawLibItem->m_Field[REFERENCE].m_Text[0] == '#' ) continue;

		Multi = 0; Unit = ' ';
		Entry = FindLibPart(DrawLibItem->m_ChipName.GetData(), wxEmptyString, FIND_ROOT);
		if( Entry ) Multi = Entry->m_UnitCount;
		if( (Multi > 1 ) && s_ListWithSubCmponents )
			 Unit = DrawLibItem->m_Multi + 'A' - 1;
		msg = DrawLibItem->m_Field[REFERENCE].m_Text;
		msg.Append(Unit);

		fprintf(f, "| %-12s %-10s",
					CONV_TO_UTF8(DrawLibItem->m_Field[VALUE].m_Text),
					CONV_TO_UTF8(msg) );
		if ( s_ListWithSubCmponents )
		{
			fprintf(f, "   (Sheet %.2d)", DrawLibItem->m_FlagControlMulti);
		}

		PrintFieldData(f, DrawLibItem);

		fprintf(f,"\n");
	}
	msg = _("#End Cmp\n");
	fprintf(f, CONV_TO_UTF8(msg));
	return(0);
}


/******************************************************************/
static int PrintListeGLabel( FILE *f, ListLabel *List, int NbItems)
/******************************************************************/
{
int ii, jj;
DrawGlobalLabelStruct *DrawTextItem;
DrawSheetLabelStruct * DrawSheetLabel;
ListLabel * LabelItem;
wxString msg;
	
	for ( ii = 0; ii < NbItems; ii++ )
	{
		LabelItem = & List[ii];

		switch( LabelItem->m_StructType )
		{
			case DRAW_GLOBAL_LABEL_STRUCT_TYPE :
				DrawTextItem = (DrawGlobalLabelStruct *)(LabelItem->m_Label);
				msg.Printf(
                        _("> %-28.28s Global        (Sheet %.2d) pos: %3.3f, %3.3f\n"),
							DrawTextItem->m_Text.GetData(),
							LabelItem->m_SheetNumber,
							(float)DrawTextItem->m_Pos.x / 1000,
							(float)DrawTextItem->m_Pos.y / 1000);
				
				fprintf(f, CONV_TO_UTF8(msg));
 				break;

			case DRAW_SHEETLABEL_STRUCT_TYPE :
			{
				DrawSheetLabel = (DrawSheetLabelStruct *) LabelItem->m_Label;
				jj = DrawSheetLabel->m_Shape;
				if ( jj < 0 ) jj = NET_TMAX; if ( jj > NET_TMAX ) jj = 4;
				wxString labtype = CONV_FROM_UTF8(SheetLabelType[jj]);
				msg.Printf(
                        _("> %-28.28s Sheet %-7.7s (Sheet %.2d) pos: %3.3f, %3.3f\n"),
							DrawSheetLabel->m_Text.GetData(),
							labtype.GetData(),
							LabelItem->m_SheetNumber,
							(float)DrawSheetLabel->m_Pos.x / 1000,
							(float)DrawSheetLabel->m_Pos.y / 1000);
				fprintf(f, CONV_TO_UTF8(msg));
			}
				break;

			default: break;
		}
	}
	msg = _("#End labels\n");
	fprintf(f, CONV_TO_UTF8(msg));
 	return(0);
}

/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG
 */

void WinEDA_Build_BOM_Frame::OnCloseWindow( wxCloseEvent& event )
{
	SavePreferences();
	EndModal(0);
}

/**************************************************/
void WinEDA_Build_BOM_Frame::SavePreferences(void)
/**************************************************/
{
	s_Add_F1_state = m_AddField1->GetValue();
	s_Add_F2_state = m_AddField2->GetValue();
	s_Add_F3_state = m_AddField3->GetValue();
	s_Add_F4_state = m_AddField4->GetValue();
	s_Add_F5_state = m_AddField5->GetValue();
	s_Add_F6_state = m_AddField6->GetValue();
	s_Add_F7_state = m_AddField7->GetValue();
	s_Add_F8_state = m_AddField8->GetValue();
	s_OutputFormOpt = m_OutputFormCtrl->GetSelection();
	s_OutputSeparatorOpt = m_OutputSeparatorCtrl->GetSelection();

	m_Parent->m_Parent->m_EDA_Config->Write(OPTION_BOM_FORMAT, (long)s_OutputFormOpt);
	m_Parent->m_Parent->m_EDA_Config->Write(OPTION_BOM_SEPARATOR, (long) s_OutputSeparatorOpt);

	long addfields = 0; 
	for ( int ii = 0, bitmask = 1; s_AddFieldList[ii] != NULL; ii++ )
	{
		if (  * s_AddFieldList[ii] ) addfields |= bitmask;
		bitmask <<= 1;
	}
	m_Parent->m_Parent->m_EDA_Config->Write(OPTION_BOM_ADD_FIELD, addfields);
}



/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIOBOX1
 */

void WinEDA_Build_BOM_Frame::OnRadioboxSelectFormatSelected( wxCommandEvent& event )
{
	if ( m_OutputFormCtrl->GetSelection() == 1 )
		m_OutputSeparatorCtrl->Enable(true);
	else 
		m_OutputSeparatorCtrl->Enable(false);
}

