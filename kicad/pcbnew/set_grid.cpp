/************************************/
/* set_grid.cpp - manage user grid. */
/************************************/

#include "fctsys.h"
#include "common.h"
#include "class_drawpanel.h"

#include "pcbnew.h"

#include "protos.h"
#include <wx/spinctrl.h>

#include "pcbnew_id.h"

#include "set_grid.h"


void WinEDA_BasePcbFrame::InstallGridFrame( const wxPoint& pos )
{
    WinEDA_PcbGridFrame dlg( this, pos );

    dlg.SetGridSize( m_UserGridSize );
    dlg.SetGridUnits( m_UserGridUnits );

    if( dlg.ShowModal() == wxID_CANCEL )
        return;

    m_UserGridSize  = dlg.GetGridSize();
    m_UserGridUnits = dlg.GetGridUnits();

    GetScreen()->AddGrid( m_UserGridSize, m_UserGridUnits, ID_POPUP_GRID_USER );
    
    // If the user grid is the current option, recall SetGrid()
    // to force new values put in list as current grid value
    if( GetScreen()->GetGridId() == ID_POPUP_GRID_USER )
        GetScreen()->SetGrid( ID_POPUP_GRID_USER  );
    DrawPanel->Refresh();
}


/*!
 * WinEDA_PcbGridFrame type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WinEDA_PcbGridFrame, wxDialog )

/*!
 * WinEDA_PcbGridFrame event table definition
 */

BEGIN_EVENT_TABLE( WinEDA_PcbGridFrame, wxDialog )

////@begin WinEDA_PcbGridFrame event table entries
////@end WinEDA_PcbGridFrame event table entries

END_EVENT_TABLE()
/*!
 * WinEDA_PcbGridFrame constructors
 */

WinEDA_PcbGridFrame::WinEDA_PcbGridFrame()
{
}


WinEDA_PcbGridFrame::WinEDA_PcbGridFrame( WinEDA_BasePcbFrame* parent,
                                          const wxPoint& pos,
                                          wxWindowID id,
                                          const wxString& caption,
                                          const wxSize& size, long style )
{
    Create( parent, id, caption, pos, size, style );
}


/*!
 * WinEDA_PcbGridFrame creator
 */

bool WinEDA_PcbGridFrame::Create( wxWindow*       parent,
                                  wxWindowID      id,
                                  const wxString& caption,
                                  const wxPoint&  pos,
                                  const wxSize&   size,
                                  long            style )
{
////@begin WinEDA_PcbGridFrame member initialisation
    m_UnitGrid     = NULL;
    m_OptGridSizeX = NULL;
    m_OptGridSizeY = NULL;

////@end WinEDA_PcbGridFrame member initialisation

////@begin WinEDA_PcbGridFrame creation
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if( GetSizer() )
    {
        GetSizer()->SetSizeHints( this );
    }

////@end WinEDA_PcbGridFrame creation
    return true;
}


/*!
 * Control creation for WinEDA_PcbGridFrame
 */

void WinEDA_PcbGridFrame::CreateControls()
{
////@begin WinEDA_PcbGridFrame content construction
    // Generated by DialogBlocks, 06/05/2009 13:48:21 (unregistered)

    WinEDA_PcbGridFrame* itemDialog1 = this;

    wxBoxSizer*          itemBoxSizer2 = new wxBoxSizer( wxHORIZONTAL );

    itemDialog1->SetSizer( itemBoxSizer2 );

    wxBoxSizer*   itemBoxSizer3 = new wxBoxSizer( wxVERTICAL );
    itemBoxSizer2->Add( itemBoxSizer3, 0, wxGROW | wxALL, 5 );

    wxArrayString m_UnitGridStrings;
    m_UnitGridStrings.Add( _( "Inches" ) );
    m_UnitGridStrings.Add( _( "mm" ) );
    m_UnitGrid = new wxRadioBox( itemDialog1, ID_RADIOBOX,
                                 _( "Grid Size Units" ), wxDefaultPosition,
                                 wxDefaultSize, m_UnitGridStrings, 1,
                                 wxRA_SPECIFY_COLS );
    m_UnitGrid->SetSelection( 0 );
    itemBoxSizer3->Add( m_UnitGrid, 0, wxGROW | wxALL, 5 );

    itemBoxSizer3->Add( 5, 5, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1,
                                                      wxID_STATIC,
                                                      _( "User Grid Size X" ),
                                                      wxDefaultPosition,
                                                      wxDefaultSize,
                                                      0 );
    itemBoxSizer3->Add( itemStaticText6,
                        0,
                        wxGROW | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE,
                        5 );

    m_OptGridSizeX = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, _T( "" ),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add( m_OptGridSizeX,
                        0,
                        wxGROW | wxLEFT | wxRIGHT | wxBOTTOM,
                        5 );

    wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1,
                                                      wxID_STATIC,
                                                      _( "User Grid Size Y" ),
                                                      wxDefaultPosition,
                                                      wxDefaultSize,
                                                      0 );
    itemBoxSizer3->Add( itemStaticText8,
                        0,
                        wxGROW | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE,
                        5 );

    m_OptGridSizeY = new wxTextCtrl( itemDialog1, ID_TEXTCTRL1, _T( "" ),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add( m_OptGridSizeY,
                        0,
                        wxGROW | wxLEFT | wxRIGHT | wxBOTTOM,
                        5 );

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer( wxVERTICAL );
    itemBoxSizer2->Add( itemBoxSizer10, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    wxButton*   itemButton11 = new wxButton( itemDialog1, wxID_OK, _( "&OK" ),
                                             wxDefaultPosition,
                                             wxDefaultSize, 0 );
    itemButton11->SetDefault();
    itemBoxSizer10->Add( itemButton11, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

    wxButton* itemButton12 = new wxButton( itemDialog1, wxID_CANCEL,
                                           _( "&Cancel" ), wxDefaultPosition,
                                           wxDefaultSize, 0 );
    itemBoxSizer10->Add( itemButton12, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

////@end WinEDA_PcbGridFrame content construction
}


/*!
 * Should we show tooltips?
 */

bool WinEDA_PcbGridFrame::ShowToolTips()
{
    return true;
}


/*!
 * Get bitmap resources
 */

wxBitmap WinEDA_PcbGridFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WinEDA_PcbGridFrame bitmap retrieval
    wxUnusedVar( name );
    return wxNullBitmap;

////@end WinEDA_PcbGridFrame bitmap retrieval
}


/*!
 * Get icon resources
 */

wxIcon WinEDA_PcbGridFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WinEDA_PcbGridFrame icon retrieval
    wxUnusedVar( name );
    return wxNullIcon;

////@end WinEDA_PcbGridFrame icon retrieval
}


void WinEDA_PcbGridFrame::SetGridSize( const wxRealPoint& grid )
{
    wxString msg;

    msg.Printf( wxT( "%.4f" ), grid.x );
    m_OptGridSizeX->SetValue( msg );
    msg.Printf( wxT( "%.4f" ), grid.y );
    m_OptGridSizeY->SetValue( msg );
}


wxRealPoint WinEDA_PcbGridFrame::GetGridSize()
{
    wxRealPoint grid;


    /* TODO: Some error checking here would be a good thing. */
    m_OptGridSizeX->GetValue().ToDouble( &grid.x );
    m_OptGridSizeY->GetValue().ToDouble( &grid.y );

    return grid;
}


void WinEDA_PcbGridFrame::SetGridUnits( int units )
{
    if( units != INCHES )
        m_UnitGrid->SetSelection( 1 );
}


int WinEDA_PcbGridFrame::GetGridUnits()
{
    return m_UnitGrid->GetSelection();
}