/////////////////////////////////////////////////////////////////////////////
// Name:        dialog_erc.cpp
// Purpose:     
// Author:      jean-pierre Charras
// Modified by: 
// Created:     12/02/2006 17:08:38
// RCS-ID:      
// Copyright:   License GNU
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 12/02/2006 17:08:38

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dialog_erc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes
#include "fctsys.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
#include "netlist.h"

#include "protos.h"

#include "dialog_erc.h"

////@begin XPM images
////@end XPM images

/*!
 * WinEDA_ErcFrame type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WinEDA_ErcFrame, wxDialog )

/*!
 * WinEDA_ErcFrame event table definition
 */

BEGIN_EVENT_TABLE( WinEDA_ErcFrame, wxDialog )

////@begin WinEDA_ErcFrame event table entries
    EVT_BUTTON( ID_ERC_CMP, WinEDA_ErcFrame::OnErcCmpClick )

    EVT_BUTTON( ID_ERASE_DRC_MARKERS, WinEDA_ErcFrame::OnEraseDrcMarkersClick )

    EVT_BUTTON( wxID_CANCEL, WinEDA_ErcFrame::OnCancelClick )

    EVT_BUTTON( ID_RESET_MATRIX, WinEDA_ErcFrame::OnResetMatrixClick )

////@end WinEDA_ErcFrame event table entries

	EVT_COMMAND_RANGE(ID_MATRIX_0,
					ID_MATRIX_0 + (PIN_NMAX * PIN_NMAX) - 1,
					wxEVT_COMMAND_BUTTON_CLICKED,
					WinEDA_ErcFrame::ChangeErrorLevel)

END_EVENT_TABLE()

/*!
 * WinEDA_ErcFrame constructors
 */

WinEDA_ErcFrame::WinEDA_ErcFrame( )
{
}

WinEDA_ErcFrame::WinEDA_ErcFrame( WinEDA_SchematicFrame* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
int ii, jj;
	
	m_Parent = parent;
	m_Initialized = FALSE;
	for( ii = 0; ii < PIN_NMAX; ii++ )
		for( jj = 0; jj < PIN_NMAX; jj++ )
			m_ButtonList[ii][jj] = NULL;
	Create(parent, id, caption, pos, size, style);

	GetSizer()->SetSizeHints(this);

	wxString num;
	num.Printf(wxT("%d"), g_EESchemaVar.NbErrorErc);
	m_TotalErrCount->SetLabel(num);

	num.Printf(wxT("%d"), g_EESchemaVar.NbErrorErc-g_EESchemaVar.NbWarningErc);
	m_LastErrCount->SetLabel(num);

	num.Printf(wxT("%d"), g_EESchemaVar.NbWarningErc);
	m_LastWarningCount->SetLabel(num);
	
}

/*!
 * WinEDA_ErcFrame creator
 */

bool WinEDA_ErcFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WinEDA_ErcFrame member initialisation
    m_NoteBook = NULL;
    m_PanelERC = NULL;
    m_PanelERCSizer = NULL;
    ErcTotalErrors = NULL;
    WarnErcErrors = NULL;
    ErcErrors = NULL;
    m_TotalErrCount = NULL;
    m_LastWarningCount = NULL;
    m_LastErrCount = NULL;
    m_WriteResultOpt = NULL;
    m_PanelERCOptions = NULL;
    m_PanelMatrixSizer = NULL;
    m_ResetOptButton = NULL;
    m_SeparatorLine = NULL;
    m_MatrixSizer = NULL;
////@end WinEDA_ErcFrame member initialisation

////@begin WinEDA_ErcFrame creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end WinEDA_ErcFrame creation
    return true;
}

/*!
 * Control creation for WinEDA_ErcFrame
 */

void WinEDA_ErcFrame::CreateControls()
{    
	SetFont(*g_DialogFont);

////@begin WinEDA_ErcFrame content construction
    // Generated by DialogBlocks, 07/11/2007 08:01:37 (unregistered)

    WinEDA_ErcFrame* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_NoteBook = new wxNotebook( itemDialog1, ID_ERC_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_DEFAULT|wxNB_TOP );

    m_PanelERC = new wxPanel( m_NoteBook, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_PanelERCSizer = new wxBoxSizer(wxHORIZONTAL);
    m_PanelERC->SetSizer(m_PanelERCSizer);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    m_PanelERCSizer->Add(itemBoxSizer6, 0, wxGROW|wxALL, 5);
    wxStaticBox* itemStaticBoxSizer7Static = new wxStaticBox(m_PanelERC, wxID_ANY, _("Erc File Report:"));
    wxStaticBoxSizer* itemStaticBoxSizer7 = new wxStaticBoxSizer(itemStaticBoxSizer7Static, wxHORIZONTAL);
    itemBoxSizer6->Add(itemStaticBoxSizer7, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemStaticBoxSizer7->Add(itemBoxSizer8, 0, wxGROW|wxLEFT|wxTOP|wxBOTTOM, 5);
    ErcTotalErrors = new wxStaticText( m_PanelERC, wxID_STATIC, _("-> Total Errors:  "), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(ErcTotalErrors, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    WarnErcErrors = new wxStaticText( m_PanelERC, wxID_STATIC, _("-> Last Warnings: "), wxDefaultPosition, wxDefaultSize, 0 );
    WarnErcErrors->SetForegroundColour(wxColour(0, 0, 255));
    itemBoxSizer8->Add(WarnErcErrors, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    ErcErrors = new wxStaticText( m_PanelERC, wxID_STATIC, _("-> Last Errors:   "), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    ErcErrors->SetForegroundColour(wxColour(202, 0, 0));
    itemBoxSizer8->Add(ErcErrors, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxVERTICAL);
    itemStaticBoxSizer7->Add(itemBoxSizer12, 0, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 5);
    m_TotalErrCount = new wxStaticText( m_PanelERC, wxID_STATIC, _("0000"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer12->Add(m_TotalErrCount, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    m_LastWarningCount = new wxStaticText( m_PanelERC, wxID_STATIC, _("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_LastWarningCount->SetForegroundColour(wxColour(0, 0, 255));
    itemBoxSizer12->Add(m_LastWarningCount, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    m_LastErrCount = new wxStaticText( m_PanelERC, wxID_STATIC, _("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_LastErrCount->SetForegroundColour(wxColour(202, 0, 0));
    itemBoxSizer12->Add(m_LastErrCount, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    itemBoxSizer6->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer17Static = new wxStaticBox(m_PanelERC, wxID_ANY, _("Erc File Report:"));
    wxStaticBoxSizer* itemStaticBoxSizer17 = new wxStaticBoxSizer(itemStaticBoxSizer17Static, wxHORIZONTAL);
    itemBoxSizer6->Add(itemStaticBoxSizer17, 0, wxGROW|wxALL, 5);
    m_WriteResultOpt = new wxCheckBox( m_PanelERC, ID_CHECKBOX, _("Write erc report"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_WriteResultOpt->SetValue(false);
    itemStaticBoxSizer17->Add(m_WriteResultOpt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxVERTICAL);
    m_PanelERCSizer->Add(itemBoxSizer19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* itemButton20 = new wxButton( m_PanelERC, ID_ERC_CMP, _("&Test Erc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton20->SetForegroundColour(wxColour(198, 0, 0));
    itemBoxSizer19->Add(itemButton20, 0, wxGROW|wxALL, 5);

    wxButton* itemButton21 = new wxButton( m_PanelERC, ID_ERASE_DRC_MARKERS, _("&Del Markers"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(itemButton21, 0, wxGROW|wxALL, 5);

    wxButton* itemButton22 = new wxButton( m_PanelERC, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton22->SetForegroundColour(wxColour(0, 0, 255));
    itemBoxSizer19->Add(itemButton22, 0, wxGROW|wxALL, 5);

    m_NoteBook->AddPage(m_PanelERC, _("erc"));

    m_PanelERCOptions = new wxPanel( m_NoteBook, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_PanelMatrixSizer = new wxBoxSizer(wxVERTICAL);
    m_PanelERCOptions->SetSizer(m_PanelMatrixSizer);

    m_ResetOptButton = new wxButton( m_PanelERCOptions, ID_RESET_MATRIX, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PanelMatrixSizer->Add(m_ResetOptButton, 0, wxALIGN_LEFT|wxALL, 5);

    m_SeparatorLine = new wxStaticLine( m_PanelERCOptions, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    m_PanelMatrixSizer->Add(m_SeparatorLine, 0, wxGROW|wxALL, 5);

    m_MatrixSizer = new wxBoxSizer(wxVERTICAL);
    m_PanelMatrixSizer->Add(m_MatrixSizer, 0, wxGROW|wxALL, 5);

    m_NoteBook->AddPage(m_PanelERCOptions, _("Options"));

    itemBoxSizer2->Add(m_NoteBook, 0, wxGROW|wxALL, 5);

    // Set validators
    m_WriteResultOpt->SetValidator( wxGenericValidator(& WriteFichierERC) );
////@end WinEDA_ErcFrame content construction

	// Init Panel Matrix
	ReBuildMatrixPanel();
}

/*!
 * Should we show tooltips?
 */

bool WinEDA_ErcFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap WinEDA_ErcFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WinEDA_ErcFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end WinEDA_ErcFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WinEDA_ErcFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WinEDA_ErcFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end WinEDA_ErcFrame icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ERASE_DRC_MARKERS
 */

void WinEDA_ErcFrame::OnEraseDrcMarkersClick( wxCommandEvent& event )
{
	DelERCMarkers(event); 
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void WinEDA_ErcFrame::OnCancelClick( wxCommandEvent& event )
{
	EndModal(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET_MATRIX
 */

void WinEDA_ErcFrame::OnResetMatrixClick( wxCommandEvent& event )
{
	ResetDefaultERCDiag(event);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ERC_CMP
 */

void WinEDA_ErcFrame::OnErcCmpClick( wxCommandEvent& event )
{
	TestErc(event); 
}
