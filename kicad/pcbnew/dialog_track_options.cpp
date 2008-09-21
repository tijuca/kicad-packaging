/////////////////////////////////////////////////////////////////////////////

// Name:        dialog_track_options.cpp
// Purpose:
// Author:      jean-pierre Charras
// Modified by:
// Created:     24/02/2006 20:58:54
// RCS-ID:
// Copyright:   License GNU
// Licence:
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 24/02/2006 20:58:54

#if defined (__GNUG__) && !defined (NO_GCC_PRAGMA)
#pragma implementation "dialog_track_options.h"
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

#include "dialog_track_options.h"

////@begin XPM images
////@end XPM images

/*!
 * WinEDA_PcbTracksDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WinEDA_PcbTracksDialog, wxDialog )

/*!
 * WinEDA_PcbTracksDialog event table definition
 */

BEGIN_EVENT_TABLE( WinEDA_PcbTracksDialog, wxDialog )

////@begin WinEDA_PcbTracksDialog event table entries
EVT_INIT_DIALOG( WinEDA_PcbTracksDialog::OnInitDialog )

EVT_CHECKBOX( ID_CHECKBOX_ALLOWS_MICROVIA, WinEDA_PcbTracksDialog::OnCheckboxAllowsMicroviaClick )

EVT_BUTTON( wxID_OK, WinEDA_PcbTracksDialog::OnOkClick )

EVT_BUTTON( wxID_CANCEL, WinEDA_PcbTracksDialog::OnCancelClick )

////@end WinEDA_PcbTracksDialog event table entries

END_EVENT_TABLE()

/*!
 * WinEDA_PcbTracksDialog constructors
 */

WinEDA_PcbTracksDialog::WinEDA_PcbTracksDialog()
{
}


WinEDA_PcbTracksDialog::WinEDA_PcbTracksDialog( WinEDA_PcbFrame* parent,
                                                wxWindowID id,
                                                const wxString& caption,
                                                const wxPoint& pos,
                                                const wxSize& size,
                                                long style )
{
    m_Parent = parent;
    Create( parent, id, caption, pos, size, style );
}


/*!
 * WinEDA_PcbTracksDialog creator
 */

bool WinEDA_PcbTracksDialog::Create( wxWindow* parent,
                                     wxWindowID id,
                                     const wxString& caption,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style )
{
////@begin WinEDA_PcbTracksDialog member initialisation
    m_ViaSizeTitle = NULL;
    m_OptViaSize   = NULL;
    m_ViaDefaultDrillValueTitle = NULL;
    m_OptViaDrill = NULL;
    m_ViaAltDrillValueTitle = NULL;
    m_OptCustomViaDrill = NULL;
    m_OptViaType = NULL;
    m_MicroViaSizeTitle  = NULL;
    m_MicroViaSizeCtrl   = NULL;
    m_MicroViaDrillTitle = NULL;
    m_MicroViaDrillCtrl  = NULL;
    m_AllowMicroViaCtrl  = NULL;
    m_TrackWidthTitle    = NULL;
    m_OptTrackWidth = NULL;
    m_TrackClearanceTitle = NULL;
    m_OptTrackClearance   = NULL;
    m_MaskClearanceTitle  = NULL;
    m_OptMaskMargin = NULL;

////@end WinEDA_PcbTracksDialog member initialisation

////@begin WinEDA_PcbTracksDialog creation
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if( GetSizer() )
    {
        GetSizer()->SetSizeHints( this );
    }
    Centre();

////@end WinEDA_PcbTracksDialog creation
    return true;
}


/*!
 * Control creation for WinEDA_PcbTracksDialog
 */

void WinEDA_PcbTracksDialog::CreateControls()
{
    SetFont( *g_DialogFont );

////@begin WinEDA_PcbTracksDialog content construction
    // Generated by DialogBlocks, 11/01/2008 21:51:48 (unregistered)

    WinEDA_PcbTracksDialog* itemDialog1 = this;

    wxBoxSizer*             itemBoxSizer2 = new         wxBoxSizer( wxHORIZONTAL );

    itemDialog1->SetSizer( itemBoxSizer2 );

    wxStaticBox*      itemStaticBoxSizer3Static = new   wxStaticBox( itemDialog1, wxID_ANY, _(
                                                                        "Vias:" ) );

    wxStaticBoxSizer* itemStaticBoxSizer3 = new         wxStaticBoxSizer(
        itemStaticBoxSizer3Static,
                      wxVERTICAL );

    itemBoxSizer2->Add( itemStaticBoxSizer3, 0, wxGROW | wxALL, 5 );

    m_ViaSizeTitle = new                                wxStaticText( itemDialog1, wxID_STATIC, _(
                                                                          "Via Size" ),
                                                                      wxDefaultPosition,
                                                                      wxDefaultSize, 0 );

    itemStaticBoxSizer3->Add( m_ViaSizeTitle,
                              0,
                              wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE,
                              5 );

    m_OptViaSize = new                                  wxTextCtrl( itemDialog1, ID_TEXTCTRL, _T(
                                                                        "" ), wxDefaultPosition,
                                                                    wxDefaultSize, 0 );

    itemStaticBoxSizer3->Add( m_OptViaSize, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    m_ViaDefaultDrillValueTitle = new                   wxStaticText( itemDialog1, wxID_STATIC, _(
                                                                          "Default Via Drill" ),
                                                                      wxDefaultPosition,
                                                                      wxDefaultSize, 0 );

    itemStaticBoxSizer3->Add( m_ViaDefaultDrillValueTitle,
                              0,
                              wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE,
                              5 );

    m_OptViaDrill = new                                 wxTextCtrl( itemDialog1, ID_TEXTCTRL1, _T(
                                                                        "" ), wxDefaultPosition,
                                                                    wxDefaultSize, 0 );

    itemStaticBoxSizer3->Add( m_OptViaDrill, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    m_ViaAltDrillValueTitle = new                       wxStaticText( itemDialog1, wxID_STATIC, _(
                                                                          "Alternate Via Drill" ),
                                                                      wxDefaultPosition,
                                                                      wxDefaultSize, 0 );

    itemStaticBoxSizer3->Add( m_ViaAltDrillValueTitle,
                              0,
                              wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE,
                              5 );

    m_OptCustomViaDrill = new                           wxTextCtrl( itemDialog1, ID_TEXTCTRL2, _T(
                                                                        "" ), wxDefaultPosition,
                                                                    wxDefaultSize, 0 );

    itemStaticBoxSizer3->Add( m_OptCustomViaDrill, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    wxArrayString m_OptViaTypeStrings;
    m_OptViaTypeStrings.Add( _( "Through Via" ) );
    m_OptViaTypeStrings.Add( _( "Blind or Buried Via " ) );
    m_OptViaType = new                                  wxRadioBox( itemDialog1,
                                                                    ID_VIA_TYPE_SELECTION,
                                                                    _( "Default Via Type" ),
                                                                    wxDefaultPosition,
                                                                    wxDefaultSize,
                                                                    m_OptViaTypeStrings,
                                                                    1,
                                                                    wxRA_SPECIFY_COLS );

    m_OptViaType->SetSelection( 0 );
    itemStaticBoxSizer3->Add( m_OptViaType, 0, wxGROW | wxALL, 5 );

    wxStaticBox*      itemStaticBoxSizer11Static = new  wxStaticBox( itemDialog1, wxID_ANY, _(
                                                                        "Micro Vias:" ) );

    wxStaticBoxSizer* itemStaticBoxSizer11 = new        wxStaticBoxSizer(
        itemStaticBoxSizer11Static,
                      wxVERTICAL );

    itemBoxSizer2->Add( itemStaticBoxSizer11, 0, wxGROW | wxALL, 5 );

    m_MicroViaSizeTitle = new                           wxStaticText( itemDialog1, wxID_STATIC, _(
                                                                          "Micro Via Size" ),
                                                                      wxDefaultPosition,
                                                                      wxDefaultSize, 0 );

    itemStaticBoxSizer11->Add( m_MicroViaSizeTitle, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5 );

    m_MicroViaSizeCtrl = new                            wxTextCtrl( itemDialog1, ID_TEXTCTRL7, _T(
                                                                        "" ), wxDefaultPosition,
                                                                    wxDefaultSize, 0 );

    itemStaticBoxSizer11->Add( m_MicroViaSizeCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    m_MicroViaDrillTitle = new                          wxStaticText( itemDialog1, wxID_STATIC, _(
                                                                          "Micro Via Drill" ),
                                                                      wxDefaultPosition,
                                                                      wxDefaultSize, 0 );

    itemStaticBoxSizer11->Add( m_MicroViaDrillTitle, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5 );

    m_MicroViaDrillCtrl = new                           wxTextCtrl( itemDialog1, ID_TEXTCTRL6, _T(
                                                                        "" ), wxDefaultPosition,
                                                                    wxDefaultSize, 0 );

    itemStaticBoxSizer11->Add( m_MicroViaDrillCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    itemStaticBoxSizer11->Add( 5, 5, 0, wxGROW | wxALL, 5 );

    m_AllowMicroViaCtrl = new wxCheckBox( itemDialog1, ID_CHECKBOX_ALLOWS_MICROVIA, _(
                                              "Allows Micro Vias" ), wxDefaultPosition,
                                          wxDefaultSize, 0 );

    m_AllowMicroViaCtrl->SetValue( false );
    m_AllowMicroViaCtrl->SetHelpText( _(
                                         "Allows use of micro vias\nThey are very small vias only from an external copper layer to its near neightbour\n" ) );
    if( WinEDA_PcbTracksDialog::ShowToolTips() )
        m_AllowMicroViaCtrl->SetToolTip( _(
                                            "Allows use of micro vias\nThey are very small vias only from an external copper layer to its near neightbour\n" ) );
    itemStaticBoxSizer11->Add( m_AllowMicroViaCtrl, 0, wxGROW | wxALL, 5 );

    wxBoxSizer* itemBoxSizer18 = new    wxBoxSizer( wxVERTICAL );

    itemBoxSizer2->Add( itemBoxSizer18, 0, wxGROW | wxALL, 5 );

    m_TrackWidthTitle = new             wxStaticText( itemDialog1, wxID_STATIC, _(
                                                          "Track Width" ), wxDefaultPosition,
                                                      wxDefaultSize, 0 );

    itemBoxSizer18->Add( m_TrackWidthTitle,
                         0,
                         wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE,
                         5 );

    m_OptTrackWidth = new               wxTextCtrl( itemDialog1, ID_TEXTCTRL3, _T(
                                                        "" ), wxDefaultPosition, wxDefaultSize, 0 );

    itemBoxSizer18->Add( m_OptTrackWidth, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    m_TrackClearanceTitle = new         wxStaticText( itemDialog1, wxID_STATIC, _(
                                                          "Clearance" ), wxDefaultPosition,
                                                      wxDefaultSize, 0 );

    itemBoxSizer18->Add( m_TrackClearanceTitle,
                         0,
                         wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE,
                         5 );

    m_OptTrackClearance = new           wxTextCtrl( itemDialog1, ID_TEXTCTRL4, _T(
                                                        "" ), wxDefaultPosition, wxDefaultSize, 0 );

    itemBoxSizer18->Add( m_OptTrackClearance, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    m_MaskClearanceTitle = new          wxStaticText( itemDialog1, wxID_STATIC, _(
                                                          "Mask clearance" ), wxDefaultPosition,
                                                      wxDefaultSize, 0 );

    itemBoxSizer18->Add( m_MaskClearanceTitle,
                         0,
                         wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE,
                         5 );

    m_OptMaskMargin = new               wxTextCtrl( itemDialog1, ID_TEXTCTRL5, _T(
                                                        "" ), wxDefaultPosition, wxDefaultSize, 0 );

    itemBoxSizer18->Add( m_OptMaskMargin, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    wxBoxSizer* itemBoxSizer25 = new    wxBoxSizer( wxVERTICAL );

    itemBoxSizer2->Add( itemBoxSizer25, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    wxButton* itemButton26 = new        wxButton( itemDialog1, wxID_OK, _(
                                                      "&OK" ), wxDefaultPosition, wxDefaultSize, 0 );

    itemButton26->SetForegroundColour( wxColour( 210, 0, 0 ) );
    itemBoxSizer25->Add( itemButton26, 0, wxGROW | wxALL, 5 );

    wxButton* itemButton27 = new        wxButton( itemDialog1, wxID_CANCEL, _(
                                                      "&Cancel" ), wxDefaultPosition,
              wxDefaultSize, wxBU_LEFT );

    itemButton27->SetForegroundColour( wxColour( 0, 0, 255 ) );
    itemBoxSizer25->Add( itemButton27, 0, wxGROW | wxALL, 5 );

    // Set validators
    m_AllowMicroViaCtrl->SetValidator( wxGenericValidator( &g_DesignSettings.m_MicroViasAllowed ) );

////@end WinEDA_PcbTracksDialog content construction
    SetDisplayValue();
}


/*!
 * Should we show tooltips?
 */

bool WinEDA_PcbTracksDialog::ShowToolTips()
{
    return true;
}


/*!
 * Get bitmap resources
 */

wxBitmap WinEDA_PcbTracksDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WinEDA_PcbTracksDialog bitmap retrieval
    wxUnusedVar( name );
    return wxNullBitmap;

////@end WinEDA_PcbTracksDialog bitmap retrieval
}


/*!
 * Get icon resources
 */

wxIcon WinEDA_PcbTracksDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WinEDA_PcbTracksDialog icon retrieval
    wxUnusedVar( name );
    return wxNullIcon;

////@end WinEDA_PcbTracksDialog icon retrieval
}


/*************************************************/
void WinEDA_PcbTracksDialog::SetDisplayValue()
/*************************************************/
{
    AddUnitSymbol( *m_ViaSizeTitle );
    AddUnitSymbol( *m_MicroViaSizeTitle );
    AddUnitSymbol( *m_ViaDefaultDrillValueTitle );
    AddUnitSymbol( *m_MicroViaDrillTitle );
    AddUnitSymbol( *m_ViaAltDrillValueTitle );
    AddUnitSymbol( *m_TrackWidthTitle );
    AddUnitSymbol( *m_TrackClearanceTitle );
    AddUnitSymbol( *m_MaskClearanceTitle );

    int Internal_Unit = m_Parent->m_InternalUnits;
    PutValueInLocalUnits( *m_OptViaSize, g_DesignSettings.m_CurrentViaSize, Internal_Unit );
    PutValueInLocalUnits( *m_MicroViaSizeCtrl,
                          g_DesignSettings.m_CurrentMicroViaSize,
                          Internal_Unit );
    PutValueInLocalUnits( *m_OptViaDrill, g_DesignSettings.m_ViaDrill, Internal_Unit );
    PutValueInLocalUnits( *m_MicroViaDrillCtrl, g_DesignSettings.m_MicroViaDrill, Internal_Unit );
    PutValueInLocalUnits( *m_OptCustomViaDrill, g_DesignSettings.m_ViaDrillCustomValue, Internal_Unit );
    PutValueInLocalUnits( *m_OptTrackWidth, g_DesignSettings.m_CurrentTrackWidth, Internal_Unit );
    PutValueInLocalUnits( *m_OptTrackClearance, g_DesignSettings.m_TrackClearence, Internal_Unit );
    PutValueInLocalUnits( *m_OptMaskMargin, g_DesignSettings.m_MaskMargin, Internal_Unit );
    if( g_DesignSettings.m_CurrentViaType != VIA_THROUGH )
        m_OptViaType->SetSelection( 1 );

    m_MicroViaSizeTitle->Enable( g_DesignSettings.m_MicroViasAllowed );
    m_MicroViaSizeCtrl->Enable( g_DesignSettings.m_MicroViasAllowed );

    m_MicroViaDrillTitle->Enable( g_DesignSettings.m_MicroViasAllowed );
    m_MicroViaDrillCtrl->Enable( g_DesignSettings.m_MicroViasAllowed );
}


/*******************************************************************/
void WinEDA_PcbTracksDialog::AcceptPcbOptions( wxCommandEvent& event )
/*******************************************************************/
{
    g_DesignSettings.m_CurrentViaType = VIA_THROUGH;
    if( m_OptViaType->GetSelection() > 0 )
        g_DesignSettings.m_CurrentViaType = VIA_BLIND_BURIED;

    g_DesignSettings.m_CurrentViaSize =
        ReturnValueFromTextCtrl( *m_OptViaSize, m_Parent->m_InternalUnits );
    g_DesignSettings.m_CurrentMicroViaSize =
        ReturnValueFromTextCtrl( *m_MicroViaSizeCtrl, m_Parent->m_InternalUnits );

    g_DesignSettings.m_MicroViaDrill =
        ReturnValueFromTextCtrl( *m_MicroViaDrillCtrl, m_Parent->m_InternalUnits );
    g_DesignSettings.m_ViaDrill =
        ReturnValueFromTextCtrl( *m_OptViaDrill, m_Parent->m_InternalUnits );
    g_DesignSettings.m_ViaDrillCustomValue =
        ReturnValueFromTextCtrl( *m_OptCustomViaDrill, m_Parent->m_InternalUnits );
    g_DesignSettings.m_MicroViasAllowed = m_AllowMicroViaCtrl->IsChecked();

    g_DesignSettings.m_CurrentTrackWidth =
        ReturnValueFromTextCtrl( *m_OptTrackWidth, m_Parent->m_InternalUnits );
    g_DesignSettings.m_TrackClearence =
        ReturnValueFromTextCtrl( *m_OptTrackClearance, m_Parent->m_InternalUnits );

    g_DesignSettings.m_MaskMargin =
        ReturnValueFromTextCtrl( *m_OptMaskMargin, m_Parent->m_InternalUnits );

    m_Parent->DisplayTrackSettings();

    m_Parent->AddHistory( g_DesignSettings.m_CurrentViaSize, TYPEVIA );
    m_Parent->AddHistory( g_DesignSettings.m_CurrentTrackWidth, TYPETRACK );
    EndModal( 1 );
}


/*********************************************************************/
void WinEDA_BasePcbFrame::AddHistory( int value, KICAD_T type )
/**********************************************************************/

// Mise a jour des listes des dernieres epaisseurs de via et track utilisées
{
    bool addhistory = TRUE;
    int  ii;

    switch( type )
    {
    case TYPETRACK:
        for( ii = 0; ii < HISTORY_NUMBER; ii++ )
        {
            if( g_DesignSettings.m_TrackWidthHistory[ii] == value )
            {
                addhistory = FALSE; 
                break;
            }
        }

        if( !addhistory )
            break;
        
        for( ii = HISTORY_NUMBER-1;   ii > 0;  ii-- )
        {
            g_DesignSettings.m_TrackWidthHistory[ii] = g_DesignSettings.m_TrackWidthHistory[ii-1];
        }

        g_DesignSettings.m_TrackWidthHistory[0] = value;

        // Reclassement par valeur croissante
        for( ii = 0; ii < HISTORY_NUMBER-1; ii++ )
        {
            if( g_DesignSettings.m_TrackWidthHistory[ii+1] == 0 )
                break;                                                          // Fin de liste
            
            if( g_DesignSettings.m_TrackWidthHistory[ii] >
                g_DesignSettings.m_TrackWidthHistory[ii+1]  )
            {
                EXCHG( g_DesignSettings.m_TrackWidthHistory[ii],
                       g_DesignSettings.m_TrackWidthHistory[ii + 1] );
            }
        }

        break;

    case TYPEVIA:
        for( ii = 0; ii < HISTORY_NUMBER; ii++ )
        {
            if( g_DesignSettings.m_ViaSizeHistory[ii] == value )
            {
                addhistory = FALSE; 
                break;
            }
        }

        if( !addhistory )
            break;

        for( ii = HISTORY_NUMBER-1;  ii > 0;  ii-- )
        {
            g_DesignSettings.m_ViaSizeHistory[ii] = g_DesignSettings.m_ViaSizeHistory[ii-1];
        }

        g_DesignSettings.m_ViaSizeHistory[0] = value;

        // Reclassement par valeur croissante
        for( ii = 0;  ii < HISTORY_NUMBER-1;  ii++ )
        {
            if( g_DesignSettings.m_ViaSizeHistory[ii+1] == 0 )
                break;                                                      // Fin de liste
            
            if( g_DesignSettings.m_ViaSizeHistory[ii] > g_DesignSettings.m_ViaSizeHistory[ii+1]  )
            {
                EXCHG( g_DesignSettings.m_ViaSizeHistory[ii],
                       g_DesignSettings.m_ViaSizeHistory[ii+1] );
            }
        }
        break;

    default:
        break;
    }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void WinEDA_PcbTracksDialog::OnOkClick( wxCommandEvent& event )
{
    AcceptPcbOptions( event );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void WinEDA_PcbTracksDialog::OnCancelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in WinEDA_PcbTracksDialog.
    // Before editing this code, remove the block markers.
    event.Skip();

////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in WinEDA_PcbTracksDialog.
}


/*!
 * wxEVT_INIT_DIALOG event handler for ID_DIALOG
 */

void WinEDA_PcbTracksDialog::OnInitDialog( wxInitDialogEvent& event )
{
    m_OptViaSize->SetFocus();

    // deselect the existing text, seems SetFocus() wants to emulate Microsoft, which is not desireable here.
    m_OptViaSize->SetSelection( 0, 0 );

    event.Skip();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_ALLOWS_MICROVIA
 */

void WinEDA_PcbTracksDialog::OnCheckboxAllowsMicroviaClick( wxCommandEvent& event )
{
    bool state = m_AllowMicroViaCtrl->IsChecked();

    m_MicroViaSizeTitle->Enable( state );
    m_MicroViaSizeCtrl->Enable( state );
    m_MicroViaDrillTitle->Enable( state );
    m_MicroViaDrillCtrl->Enable( state );
}
