/////////////////////////////////////////////////////////////////////////////
// Name:        dialog_edit_component_in_lib.cpp
// Purpose:     
// Author:      jean-pierre Charras
// Modified by: 
// Created:     02/03/2006 08:51:09
// RCS-ID:      
// Copyright:   License GNU
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 02/03/2006 08:51:09


////@begin includes
////@end includes

#include "dialog_edit_component_in_lib.h"

////@begin XPM images
////@end XPM images

/*!
 * WinEDA_PartPropertiesFrame type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WinEDA_PartPropertiesFrame, wxDialog )

/*!
 * WinEDA_PartPropertiesFrame event table definition
 */

BEGIN_EVENT_TABLE( WinEDA_PartPropertiesFrame, wxDialog )

////@begin WinEDA_PartPropertiesFrame event table entries
    EVT_BUTTON( ID_COPY_DOC_TO_ALIAS, WinEDA_PartPropertiesFrame::OnCopyDocToAliasClick )

    EVT_BUTTON( ID_BROWSE_DOC_FILES, WinEDA_PartPropertiesFrame::OnBrowseDocFilesClick )

    EVT_BUTTON( wxID_CANCEL, WinEDA_PartPropertiesFrame::OnCancelClick )

    EVT_BUTTON( wxID_OK, WinEDA_PartPropertiesFrame::OnOkClick )

////@end WinEDA_PartPropertiesFrame event table entries
	EVT_RADIOBOX(ID_ON_SELECT_FIELD, WinEDA_PartPropertiesFrame::SelectNewField)
	EVT_BUTTON(ID_ADD_ALIAS, WinEDA_PartPropertiesFrame::AddAliasOfPart)
	EVT_BUTTON(ID_DELETE_ONE_ALIAS, WinEDA_PartPropertiesFrame::DeleteAliasOfPart)
	EVT_BUTTON(ID_DELETE_ALL_ALIAS, WinEDA_PartPropertiesFrame::DeleteAllAliasOfPart)
	EVT_BUTTON(ID_ADD_FOOTPRINT_FILTER, WinEDA_PartPropertiesFrame::AddFootprintFilter)
	EVT_BUTTON(ID_DELETE_ONE_FOOTPRINT_FILTER, WinEDA_PartPropertiesFrame::DeleteOneFootprintFilter)
	EVT_BUTTON(ID_DELETE_ALL_FOOTPRINT_FILTER, WinEDA_PartPropertiesFrame::DeleteAllFootprintFilter)
END_EVENT_TABLE()

/*!
 * WinEDA_PartPropertiesFrame constructors
 */

WinEDA_PartPropertiesFrame::WinEDA_PartPropertiesFrame( )
{
}

WinEDA_PartPropertiesFrame::WinEDA_PartPropertiesFrame( WinEDA_LibeditFrame* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	m_Parent = parent;
	m_RecreateToolbar = FALSE;
	m_AliasLocation = -1;
	m_CurrentFieldId = 0;
   
	InitBuffers();
	
    Create(parent, id, caption, pos, size, style);
	
	SetTitle(m_Title);
}

/*!
 * WinEDA_PartPropertiesFrame creator
 */

bool WinEDA_PartPropertiesFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	m_FieldNameCtrl = NULL;
////@begin WinEDA_PartPropertiesFrame member initialisation
    m_GeneralBoxSizer = NULL;
    m_NoteBook = NULL;
    m_PanelBasic = NULL;
    m_PanelBasicBoxSizer = NULL;
    m_OptionsBoxSizer = NULL;
    SelNumberOfUnits = NULL;
    m_SetSkew = NULL;
    m_OptionPower = NULL;
    m_OptionPartsLocked = NULL;
    m_PanelDoc = NULL;
    m_PanelDocBoxSizer = NULL;
    m_Doc = NULL;
    m_Keywords = NULL;
    m_Docfile = NULL;
    m_ButtonCopyDoc = NULL;
    m_PanelAlias = NULL;
////@end WinEDA_PartPropertiesFrame member initialisation

////@begin WinEDA_PartPropertiesFrame creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end WinEDA_PartPropertiesFrame creation
    return true;
}

/*!
 * Control creation for WinEDA_PartPropertiesFrame
 */

void WinEDA_PartPropertiesFrame::CreateControls()
{    
	SetFont(*g_DialogFont);
	
////@begin WinEDA_PartPropertiesFrame content construction
    // Generated by DialogBlocks, 02/03/2006 16:15:16 (unregistered)

    WinEDA_PartPropertiesFrame* itemDialog1 = this;

    m_GeneralBoxSizer = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(m_GeneralBoxSizer);

    m_NoteBook = new wxNotebook( itemDialog1, ID_LIBEDIT_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

    m_PanelBasic = new wxPanel( m_NoteBook, ID_PANEL_BASIC, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_PanelBasicBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_PanelBasic->SetSizer(m_PanelBasicBoxSizer);

    wxStaticBox* itemStaticBoxSizer6Static = new wxStaticBox(m_PanelBasic, wxID_ANY, _("General :"));
    m_OptionsBoxSizer = new wxStaticBoxSizer(itemStaticBoxSizer6Static, wxVERTICAL);
    m_PanelBasicBoxSizer->Add(m_OptionsBoxSizer, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    m_PanelBasicBoxSizer->Add(itemBoxSizer7, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer7->Add(itemBoxSizer8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText9 = new wxStaticText( m_PanelBasic, wxID_STATIC, _("Number of Parts:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(itemStaticText9, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    SelNumberOfUnits = new wxSpinCtrl( m_PanelBasic, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 26, 0 );
    itemBoxSizer8->Add(SelNumberOfUnits, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer7->Add(itemBoxSizer11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText12 = new wxStaticText( m_PanelBasic, wxID_STATIC, _("Skew:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_SetSkew = new wxSpinCtrl( m_PanelBasic, ID_SPINCTRL, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0 );
    itemBoxSizer11->Add(m_SetSkew, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_OptionPower = new wxCheckBox( m_PanelBasic, ID_CHECKBOX, _("Power Symbol"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_OptionPower->SetValue(false);
    m_PanelBasicBoxSizer->Add(m_OptionPower, 0, wxGROW|wxALL, 5);

    m_OptionPartsLocked = new wxCheckBox( m_PanelBasic, ID_CHECKBOX1, _("Parts are locked"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_OptionPartsLocked->SetValue(false);
    m_PanelBasicBoxSizer->Add(m_OptionPartsLocked, 0, wxGROW|wxALL, 5);

    m_NoteBook->AddPage(m_PanelBasic, _("Options"));

    m_PanelDoc = new wxPanel( m_NoteBook, ID_PANEL_DOC, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_PanelDocBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_PanelDoc->SetSizer(m_PanelDocBoxSizer);

    wxStaticText* itemStaticText18 = new wxStaticText( m_PanelDoc, wxID_STATIC, _("Doc:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText18->SetForegroundColour(wxColour(196, 0, 0));
    m_PanelDocBoxSizer->Add(itemStaticText18, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Doc = new wxTextCtrl( m_PanelDoc, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_PanelDocBoxSizer->Add(m_Doc, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText20 = new wxStaticText( m_PanelDoc, wxID_STATIC, _("Keywords:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText20->SetForegroundColour(wxColour(196, 0, 0));
    m_PanelDocBoxSizer->Add(itemStaticText20, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Keywords = new wxTextCtrl( m_PanelDoc, ID_TEXTCTRL1, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_PanelDocBoxSizer->Add(m_Keywords, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText22 = new wxStaticText( m_PanelDoc, wxID_STATIC, _("DocFileName:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText22->SetForegroundColour(wxColour(196, 0, 0));
    m_PanelDocBoxSizer->Add(itemStaticText22, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Docfile = new wxTextCtrl( m_PanelDoc, ID_TEXTCTRL2, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_PanelDocBoxSizer->Add(m_Docfile, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    m_PanelDocBoxSizer->Add(itemBoxSizer24, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    m_ButtonCopyDoc = new wxButton( m_PanelDoc, ID_COPY_DOC_TO_ALIAS, _("Copy Doc"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ButtonCopyDoc->SetForegroundColour(wxColour(0, 0, 255));
    itemBoxSizer24->Add(m_ButtonCopyDoc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton26 = new wxButton( m_PanelDoc, ID_BROWSE_DOC_FILES, _("Browse DocFiles"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton26->SetForegroundColour(wxColour(202, 0, 0));
    itemBoxSizer24->Add(itemButton26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_NoteBook->AddPage(m_PanelDoc, _("Doc"));

    m_PanelAlias = new wxPanel( m_NoteBook, ID_PANEL_ALIAS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

    m_NoteBook->AddPage(m_PanelAlias, _("Alias"));

    m_GeneralBoxSizer->Add(m_NoteBook, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    m_GeneralBoxSizer->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton29 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton29->SetForegroundColour(wxColour(0, 0, 255));
    itemBoxSizer28->Add(itemButton29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton30 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton30->SetDefault();
    itemButton30->SetForegroundColour(wxColour(202, 0, 0));
    itemBoxSizer28->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end WinEDA_PartPropertiesFrame content construction
	BuildPanelBasic();
	BuildPanelDoc();
	BuildPanelAlias();
	BuildPanelEditField();	// Add panel Field edition
	BuildPanelFootprintFilter();
}

/*!
 * Should we show tooltips?
 */

bool WinEDA_PartPropertiesFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap WinEDA_PartPropertiesFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WinEDA_PartPropertiesFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end WinEDA_PartPropertiesFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WinEDA_PartPropertiesFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WinEDA_PartPropertiesFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end WinEDA_PartPropertiesFrame icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COPY_DOC_TO_ALIAS
 */

void WinEDA_PartPropertiesFrame::OnCopyDocToAliasClick( wxCommandEvent& event )
{
	CopyDocToAlias(event);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BROWSE_DOC_FILES
 */

void WinEDA_PartPropertiesFrame::OnBrowseDocFilesClick( wxCommandEvent& event )
{
	BrowseAndSelectDocFile(event);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void WinEDA_PartPropertiesFrame::OnCancelClick( wxCommandEvent& event )
{
	EndModal(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void WinEDA_PartPropertiesFrame::OnOkClick( wxCommandEvent& event )
{
	PartPropertiesAccept(event);
}


