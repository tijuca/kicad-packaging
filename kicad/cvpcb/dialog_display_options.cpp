/////////////////////////////////////////////////////////////////////////////

// Name:        dialog_display_options.cpp
// Purpose:
// Author:      jean-pierre Charras
// Modified by:
// Created:     17/02/2006 17:47:55
// RCS-ID:
// Copyright:   License GNU
// Licence:
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 17/02/2006 17:47:55

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dialog_display_options.h"
#endif

////@begin includes
////@end includes
#include "fctsys.h"

#include "wxstruct.h"
#include "common.h"
#include "cvpcb.h"
#include "protos.h"
#include "class_drawpanel.h"
#include "cvstruct.h"
#include "class_DisplayFootprintsFrame.h"

#include "dialog_display_options.h"

////@begin XPM images
////@end XPM images


void DISPLAY_FOOTPRINTS_FRAME::InstallOptionsDisplay( wxCommandEvent& event )
{
    WinEDA_FootprintDisplayOptionsFrame* OptionWindow =
        new WinEDA_FootprintDisplayOptionsFrame( this );

    OptionWindow->ShowModal();
    OptionWindow->Destroy();
}


/*!
 * WinEDA_FootprintDisplayOptionsFrame type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WinEDA_FootprintDisplayOptionsFrame, wxDialog )

/*!
 * WinEDA_FootprintDisplayOptionsFrame event table definition
 */

BEGIN_EVENT_TABLE( WinEDA_FootprintDisplayOptionsFrame, wxDialog )

////@begin WinEDA_FootprintDisplayOptionsFrame event table entries
    EVT_BUTTON( wxID_OK, WinEDA_FootprintDisplayOptionsFrame::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, WinEDA_FootprintDisplayOptionsFrame::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, WinEDA_FootprintDisplayOptionsFrame::OnApplyClick )

////@end WinEDA_FootprintDisplayOptionsFrame event table entries

END_EVENT_TABLE()

/*!
 * WinEDA_FootprintDisplayOptionsFrame constructors
 */

WinEDA_FootprintDisplayOptionsFrame::WinEDA_FootprintDisplayOptionsFrame()
{
}


WinEDA_FootprintDisplayOptionsFrame::WinEDA_FootprintDisplayOptionsFrame(
    WinEDA_BasePcbFrame* parent,
    wxWindowID           id,
    const wxString&      caption,
    const wxPoint&       pos,
    const wxSize&        size,
    long                 style )
{
    m_Parent = parent;

    Create( parent, id, caption, pos, size, style );
}


/*!
 * WinEDA_FootprintDisplayOptionsFrame creator
 */

bool WinEDA_FootprintDisplayOptionsFrame::Create( wxWindow*       parent,
                                                  wxWindowID      id,
                                                  const wxString& caption,
                                                  const wxPoint&  pos,
                                                  const wxSize&   size,
                                                  long            style )
{
////@begin WinEDA_FootprintDisplayOptionsFrame member initialisation
    OuterBoxSizer = NULL;
    MainBoxSizer  = NULL;
    m_EdgesDisplayOption = NULL;
    m_TextDisplayOption  = NULL;
    ColumnBoxSizer  = NULL;
    m_IsShowPadFill = NULL;
    m_IsShowViaFill = NULL;
    m_IsShowPadNum  = NULL;
    m_CancelButton  = NULL;

////@end WinEDA_FootprintDisplayOptionsFrame member initialisation

////@begin WinEDA_FootprintDisplayOptionsFrame creation
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if( GetSizer() )
    {
        GetSizer()->SetSizeHints( this );
    }
    Centre();

////@end WinEDA_FootprintDisplayOptionsFrame creation
    return true;
}


/*!
 * Control creation for WinEDA_FootprintDisplayOptionsFrame
 */

void WinEDA_FootprintDisplayOptionsFrame::CreateControls()
{
////@begin WinEDA_FootprintDisplayOptionsFrame content construction
    // Generated by DialogBlocks, 16/04/2009 14:23:49 (unregistered)

    WinEDA_FootprintDisplayOptionsFrame* itemDialog1 = this;

    OuterBoxSizer = new wxBoxSizer( wxVERTICAL );
    itemDialog1->SetSizer( OuterBoxSizer );

    MainBoxSizer = new wxBoxSizer( wxHORIZONTAL );
    OuterBoxSizer->Add( MainBoxSizer, 1, wxGROW | wxALL, 5 );

    wxArrayString m_EdgesDisplayOptionStrings;
    m_EdgesDisplayOptionStrings.Add( _( "&Line" ) );
    m_EdgesDisplayOptionStrings.Add( _( "&Filled" ) );
    m_EdgesDisplayOptionStrings.Add( _( "&Sketch" ) );
    m_EdgesDisplayOption =
        new wxRadioBox( itemDialog1, EDGE_SELECT, _( "Edges:" ),
                        wxDefaultPosition, wxDefaultSize,
                        m_EdgesDisplayOptionStrings,
                        1, wxRA_SPECIFY_COLS );
    m_EdgesDisplayOption->SetSelection( 0 );
    MainBoxSizer->Add( m_EdgesDisplayOption, 0, wxALIGN_TOP | wxALL, 5 );

    wxArrayString m_TextDisplayOptionStrings;
    m_TextDisplayOptionStrings.Add( _( "&Line" ) );
    m_TextDisplayOptionStrings.Add( _( "&Filled" ) );
    m_TextDisplayOptionStrings.Add( _( "&Sketch" ) );
    m_TextDisplayOption =
        new wxRadioBox( itemDialog1, TEXT_SELECT, _( "Texts:" ),
                        wxDefaultPosition, wxDefaultSize,
                        m_TextDisplayOptionStrings, 1,
                        wxRA_SPECIFY_COLS );
    m_TextDisplayOption->SetSelection( 0 );
    MainBoxSizer->Add( m_TextDisplayOption, 0, wxALIGN_TOP | wxALL, 5 );

    ColumnBoxSizer = new wxBoxSizer( wxVERTICAL );
    MainBoxSizer->Add( ColumnBoxSizer, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    m_IsShowPadFill = new wxCheckBox( itemDialog1, PADFILL_OPT,
                                      _( "Fill &pad" ), wxDefaultPosition,
                                      wxDefaultSize, wxCHK_2STATE );
    m_IsShowPadFill->SetValue( false );
    ColumnBoxSizer->Add( m_IsShowPadFill, 1, wxGROW | wxALL, 5 );

    m_IsShowViaFill = new wxCheckBox( itemDialog1, VIAFILL_OPT,
                                      _( "Fill &via" ), wxDefaultPosition,
                                      wxDefaultSize, wxCHK_2STATE );
    m_IsShowViaFill->SetValue( false );
    ColumnBoxSizer->Add( m_IsShowViaFill, 1, wxGROW | wxALL, 5 );

    m_IsShowPadNum =
        new wxCheckBox( itemDialog1, PADNUM_OPT, _( "Show pad &number" ),
                        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_IsShowPadNum->SetValue( false );
    if( WinEDA_FootprintDisplayOptionsFrame::ShowToolTips() )
        m_IsShowPadNum->SetToolTip( _( "Display pad number" ) );
    ColumnBoxSizer->Add( m_IsShowPadNum, 1, wxGROW | wxALL, 5 );

    wxStaticLine* itemStaticLine9 = new wxStaticLine( itemDialog1,
                                                      ID_STATICLINE1,
                                                      wxDefaultPosition,
                                                      wxDefaultSize,
                                                      wxLI_HORIZONTAL );
    OuterBoxSizer->Add( itemStaticLine9, 0, wxGROW | wxALL, 5 );

    wxStdDialogButtonSizer* itemStdDialogButtonSizer10 =
        new wxStdDialogButtonSizer;

    OuterBoxSizer->Add( itemStdDialogButtonSizer10,
                        0,
                        wxALIGN_CENTER_HORIZONTAL | wxALL,
                        5 );
    wxButton* itemButton11 = new wxButton( itemDialog1, wxID_OK, _( "&OK" ),
                                           wxDefaultPosition,
                                           wxDefaultSize, 0 );
    itemStdDialogButtonSizer10->AddButton( itemButton11 );

    m_CancelButton = new wxButton( itemDialog1, wxID_CANCEL, _( "&Cancel" ),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer10->AddButton( m_CancelButton );

    wxButton* itemButton13 =
        new wxButton( itemDialog1, wxID_APPLY, _( "&Apply" ),
                      wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer10->AddButton( itemButton13 );

    itemStdDialogButtonSizer10->Realize();

    // Set validators
    m_EdgesDisplayOption->SetValidator( wxGenericValidator( &DisplayOpt.
                                                            DisplayModEdge ) );
    m_TextDisplayOption->SetValidator( wxGenericValidator( &DisplayOpt.
                                                           DisplayModText ) );
    m_IsShowPadFill->SetValidator( wxGenericValidator( &DisplayOpt.
                                                       DisplayPadFill ) );
    m_IsShowViaFill->SetValidator( wxGenericValidator( &DisplayOpt.
                                                       DisplayViaFill ) );
    m_IsShowPadNum->SetValidator( wxGenericValidator( &DisplayOpt.
                                                      DisplayPadNum ) );

////@end WinEDA_FootprintDisplayOptionsFrame content construction

    /* Necessary to use escape key as cancel under wxGTK. */
    m_CancelButton->SetFocus();
}


/*!
 * Should we show tooltips?
 */

bool WinEDA_FootprintDisplayOptionsFrame::ShowToolTips()
{
    return true;
}


/*!
 * Get bitmap resources
 */

wxBitmap WinEDA_FootprintDisplayOptionsFrame::GetBitmapResource(
    const wxString& name )
{
    // Bitmap retrieval
////@begin WinEDA_FootprintDisplayOptionsFrame bitmap retrieval
    wxUnusedVar( name );
    return wxNullBitmap;

////@end WinEDA_FootprintDisplayOptionsFrame bitmap retrieval
}


/*!
 * Get icon resources
 */

wxIcon WinEDA_FootprintDisplayOptionsFrame::GetIconResource(
    const wxString& name )
{
    // Icon retrieval
////@begin WinEDA_FootprintDisplayOptionsFrame icon retrieval
    wxUnusedVar( name );
    return wxNullIcon;

////@end WinEDA_FootprintDisplayOptionsFrame icon retrieval
}


/*!
 * Update settings related to edges, text strings, and pads
 */

void WinEDA_FootprintDisplayOptionsFrame::UpdateObjectSettings( void )
{
    // Update settings
////@begin WinEDA_FootprintDisplayOptionsFrame update settings
    m_Parent->m_DisplayModEdge = m_EdgesDisplayOption->GetSelection();
    m_Parent->m_DisplayModText = m_TextDisplayOption->GetSelection();
    m_Parent->m_DisplayPadNum  = m_IsShowPadNum->GetValue();
    m_Parent->m_DisplayPadFill = m_IsShowPadFill->GetValue();
    m_Parent->m_DisplayViaFill = m_IsShowViaFill->GetValue();
    m_Parent->DrawPanel->Refresh();

////@end WinEDA_FootprintDisplayOptionsFrame update settings
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void WinEDA_FootprintDisplayOptionsFrame::OnOkClick( wxCommandEvent& event )
{
    UpdateObjectSettings();
    EndModal( 1 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void WinEDA_FootprintDisplayOptionsFrame::OnCancelClick( wxCommandEvent& event )
{
    EndModal( -1 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void WinEDA_FootprintDisplayOptionsFrame::OnApplyClick( wxCommandEvent& event )
{
    UpdateObjectSettings();
}