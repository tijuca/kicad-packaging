/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see change_log.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#include <fctsys.h>
#include <class_drawpanel.h>
#include <base_units.h>
#include <pcb_edit_frame.h>
#include <board_design_settings.h>
#include <bitmaps.h>
#include <widgets/wx_grid.h>
#include <grid_tricks.h>

#include <panel_setup_tracks_and_vias.h>


enum TRACK_VAR_GRID_COLUMNS
{
    TR_WIDTH_COL = 0
};

enum VIA_VAR_GRID_COLUMNS
{
    VIA_SIZE_COL = 0,
    VIA_DRILL_COL
};

enum DIFF_VAR_GRID_COLUMNS
{
    DP_WIDTH_COL = 0,
    DP_GAP_COL,
    DP_VIA_GAP_COL
};


PANEL_SETUP_TRACKS_AND_VIAS::PANEL_SETUP_TRACKS_AND_VIAS(
                                          PAGED_DIALOG* aParent, PCB_EDIT_FRAME* aFrame,
                                          PANEL_SETUP_FEATURE_CONSTRAINTS* aConstraintsPanel ) :
    PANEL_SETUP_TRACKS_AND_VIAS_BASE( aParent->GetTreebook() )
{
    m_Parent = aParent;
    m_Frame = aFrame;
    m_Pcb = m_Frame->GetBoard();
    m_BrdSettings = &m_Pcb->GetDesignSettings();
    m_ConstraintsPanel = aConstraintsPanel;

    m_trackWidthsAddButton->SetBitmap( KiBitmap( small_plus_xpm ) );
    m_trackWidthsRemoveButton->SetBitmap( KiBitmap( trash_xpm ) );
    m_viaSizesAddButton->SetBitmap( KiBitmap( small_plus_xpm ) );
    m_viaSizesRemoveButton->SetBitmap( KiBitmap( trash_xpm ) );
    m_diffPairsAddButton->SetBitmap( KiBitmap( small_plus_xpm ) );
    m_diffPairsRemoveButton->SetBitmap( KiBitmap( trash_xpm ) );

    // Membership combobox editors require a bit more room, so increase the row size of
    // all our grids for consistency
    m_trackWidthsGrid->SetDefaultRowSize( m_trackWidthsGrid->GetDefaultRowSize() + 4 );
    m_viaSizesGrid->SetDefaultRowSize(    m_viaSizesGrid->GetDefaultRowSize()    + 4 );
    m_diffPairsGrid->SetDefaultRowSize(   m_diffPairsGrid->GetDefaultRowSize()   + 4 );

    m_trackWidthsGrid->PushEventHandler( new GRID_TRICKS( m_trackWidthsGrid ) );
    m_viaSizesGrid->PushEventHandler( new GRID_TRICKS( m_viaSizesGrid ) );
    m_diffPairsGrid->PushEventHandler( new GRID_TRICKS( m_diffPairsGrid ) );

    m_trackWidthsGrid->SetSelectionMode( wxGrid::wxGridSelectionModes::wxGridSelectRows );
    m_viaSizesGrid->SetSelectionMode( wxGrid::wxGridSelectionModes::wxGridSelectRows );
    m_diffPairsGrid->SetSelectionMode( wxGrid::wxGridSelectionModes::wxGridSelectRows );
}

PANEL_SETUP_TRACKS_AND_VIAS::~PANEL_SETUP_TRACKS_AND_VIAS()
{
    // Delete the GRID_TRICKS.
    m_trackWidthsGrid->PopEventHandler( true );
    m_viaSizesGrid->PopEventHandler( true );
    m_diffPairsGrid->PopEventHandler( true );
}

bool PANEL_SETUP_TRACKS_AND_VIAS::TransferDataToWindow()
{
    if( m_trackWidthsGrid->GetNumberRows() > 0 )
        m_trackWidthsGrid->DeleteRows( 0, m_trackWidthsGrid->GetNumberRows() );

    if( m_viaSizesGrid->GetNumberRows() > 0 )
        m_viaSizesGrid->DeleteRows( 0, m_viaSizesGrid->GetNumberRows() );

    if( m_diffPairsGrid->GetNumberRows() > 0 )
        m_diffPairsGrid->DeleteRows( 0, m_diffPairsGrid->GetNumberRows() );

    // Skip the first item, which is the current netclass value
    for( unsigned ii = 1; ii < m_BrdSettings->m_TrackWidthList.size(); ii++ )
    {
        AppendTrackWidth( m_BrdSettings->m_TrackWidthList[ii] );
    }

    // Skip the first item, which is the current netclass value
    for( unsigned ii = 1; ii < m_BrdSettings->m_ViasDimensionsList.size(); ii++ )
    {
        AppendViaSize( m_BrdSettings->m_ViasDimensionsList[ii].m_Diameter,
                       m_BrdSettings->m_ViasDimensionsList[ii].m_Drill );
    }

    // Skip the first item, which is the current netclass value
    for( unsigned ii = 1; ii < m_BrdSettings->m_DiffPairDimensionsList.size(); ii++ )
    {
        AppendDiffPairs( m_BrdSettings->m_DiffPairDimensionsList[ii].m_Width,
                         m_BrdSettings->m_DiffPairDimensionsList[ii].m_Gap,
                         m_BrdSettings->m_DiffPairDimensionsList[ii].m_ViaGap );
    }

    return true;
}


bool PANEL_SETUP_TRACKS_AND_VIAS::TransferDataFromWindow()
{
    if( !validateData() )
        return false;

    wxString                         msg;
    std::vector<int>                 trackWidths;
    std::vector<VIA_DIMENSION>       vias;
    std::vector<DIFF_PAIR_DIMENSION> diffPairs;

    if( !m_trackWidthsGrid->CommitPendingChanges()
            || !m_viaSizesGrid->CommitPendingChanges()
            || !m_diffPairsGrid->CommitPendingChanges() )
    {
        return false;
    }

    for( int row = 0; row < m_trackWidthsGrid->GetNumberRows();  ++row )
    {
        msg = m_trackWidthsGrid->GetCellValue( row, TR_WIDTH_COL );

        if( !msg.IsEmpty() )
            trackWidths.push_back( ValueFromString( m_Frame->GetUserUnits(), msg, true ) );
    }

    for( int row = 0; row < m_viaSizesGrid->GetNumberRows();  ++row )
    {
        msg = m_viaSizesGrid->GetCellValue( row, VIA_SIZE_COL );

        if( !msg.IsEmpty() )
        {
            VIA_DIMENSION via_dim;
            via_dim.m_Diameter = ValueFromString( m_Frame->GetUserUnits(), msg, true );

            msg = m_viaSizesGrid->GetCellValue( row, VIA_DRILL_COL );

            if( !msg.IsEmpty() )
                via_dim.m_Drill = ValueFromString( m_Frame->GetUserUnits(), msg, true );

            vias.push_back( via_dim );
        }
    }

    for( int row = 0; row < m_diffPairsGrid->GetNumberRows();  ++row )
    {
        msg = m_diffPairsGrid->GetCellValue( row, DP_WIDTH_COL );

        if( !msg.IsEmpty() )
        {
            DIFF_PAIR_DIMENSION diffPair_dim;
            diffPair_dim.m_Width = ValueFromString( m_Frame->GetUserUnits(), msg, true );

            msg = m_diffPairsGrid->GetCellValue( row, DP_GAP_COL );
            diffPair_dim.m_Gap = ValueFromString( m_Frame->GetUserUnits(), msg, true );

            msg = m_diffPairsGrid->GetCellValue( row, DP_VIA_GAP_COL );

            if( !msg.IsEmpty() )
                diffPair_dim.m_ViaGap = ValueFromString( m_Frame->GetUserUnits(), msg, true );

            diffPairs.push_back( diffPair_dim );
        }
    }

    // Sort lists by increasing value
    sort( trackWidths.begin(), trackWidths.end() );
    sort( vias.begin(), vias.end() );
    sort( diffPairs.begin(), diffPairs.end() );

    trackWidths.insert( trackWidths.begin(), m_BrdSettings->m_TrackWidthList[ 0 ] );
    m_BrdSettings->m_TrackWidthList = trackWidths;

    vias.insert( vias.begin(), m_BrdSettings->m_ViasDimensionsList[ 0 ] );
    m_BrdSettings->m_ViasDimensionsList = vias;

    diffPairs.insert( diffPairs.begin(), m_BrdSettings->m_DiffPairDimensionsList[ 0 ] );
    m_BrdSettings->m_DiffPairDimensionsList = diffPairs;

    return true;
}


bool PANEL_SETUP_TRACKS_AND_VIAS::validateData()
{
    if( !m_trackWidthsGrid->CommitPendingChanges()
            || !m_viaSizesGrid->CommitPendingChanges()
            || !m_diffPairsGrid->CommitPendingChanges() )
    {
        return false;
    }

    wxString msg;
    int minViaDia = m_ConstraintsPanel->m_viaMinSize.GetValue();
    int minViaDrill = m_ConstraintsPanel->m_viaMinDrill.GetValue();
    int minTrackWidth = m_ConstraintsPanel->m_trackMinWidth.GetValue();

    // Test tracks
    for( int row = 0; row < m_trackWidthsGrid->GetNumberRows();  ++row )
    {
        wxString tvalue = m_trackWidthsGrid->GetCellValue( row, TR_WIDTH_COL );

        if( tvalue.IsEmpty() )
            continue;

        if( ValueFromString( m_Frame->GetUserUnits(), tvalue ) < minTrackWidth )
        {
            msg.Printf( _( "Track width less than minimum track width (%s)." ),
                        StringFromValue( m_Frame->GetUserUnits(), minTrackWidth, true, true ) );
            m_Parent->SetError( msg, this, m_trackWidthsGrid, row, TR_WIDTH_COL );
            return false;
        }
    }

    // Test vias
    for( int row = 0; row < m_viaSizesGrid->GetNumberRows();  ++row )
    {
        wxString viaDia = m_viaSizesGrid->GetCellValue( row, VIA_SIZE_COL );

        if( viaDia.IsEmpty() )
            continue;

        if( ValueFromString( m_Frame->GetUserUnits(), viaDia ) < minViaDia )
        {
            msg.Printf( _( "Via diameter less than minimum via diameter (%s)." ),
                        StringFromValue( m_Frame->GetUserUnits(), minViaDia, true, true ) );
            m_Parent->SetError( msg, this, m_viaSizesGrid, row, VIA_SIZE_COL );
            return false;
        }

        wxString viaDrill = m_viaSizesGrid->GetCellValue( row, VIA_DRILL_COL );

        if( viaDrill.IsEmpty() )
        {
            msg = _( "No via drill defined." );
            m_Parent->SetError( msg, this, m_viaSizesGrid, row, VIA_DRILL_COL );
            return false;
        }

        if( ValueFromString( m_Frame->GetUserUnits(), viaDrill ) < minViaDrill )
        {
            msg.Printf( _( "Via drill less than minimum via drill (%s)." ),
                        StringFromValue( m_Frame->GetUserUnits(), minViaDrill, true, true ) );
            m_Parent->SetError( msg, this, m_viaSizesGrid, row, VIA_DRILL_COL );
            return false;
        }

        if( ValueFromString( m_Frame->GetUserUnits(), viaDrill )
                >= ValueFromString( m_Frame->GetUserUnits(), viaDia ) )
        {
            msg = _( "Via drill larger than via diameter." );
            m_Parent->SetError( msg, this, m_viaSizesGrid, row, VIA_DRILL_COL );
            return false;
        }
    }

    // Test diff pairs
    for( int row = 0; row < m_diffPairsGrid->GetNumberRows();  ++row )
    {
        wxString tvalue = m_diffPairsGrid->GetCellValue( row, 0 );

        if( tvalue.IsEmpty() )
            continue;

        if( ValueFromString( m_Frame->GetUserUnits(), tvalue ) < minTrackWidth )
        {
            msg.Printf( _( "Differential pair track width less than minimum track width (%s)." ),
                        StringFromValue( m_Frame->GetUserUnits(), minTrackWidth, true, true ) );
            m_Parent->SetError( msg, this, m_diffPairsGrid, row, 0 );
            return false;
        }

        wxString gap = m_diffPairsGrid->GetCellValue( row, 1 );

        if( gap.IsEmpty() )
        {
            msg = _( "No differential pair gap defined." );
            m_Parent->SetError( msg, this, m_diffPairsGrid, row, 1 );
            return false;
        }

        if( ValueFromString( m_Frame->GetUserUnits(), gap ) < 0 )
        {
            msg.Printf( _( "Differential pair gap cannot be negative." ) );
            m_Parent->SetError( msg, this, m_diffPairsGrid, row, 1 );
            return false;
        }

        wxString viaGap = m_diffPairsGrid->GetCellValue( row, 2 );

        if( viaGap.IsEmpty() )
            continue;

        if( ValueFromString( m_Frame->GetUserUnits(), viaGap ) < 0 )
        {
            msg.Printf( _( "Differential pair via gap cannot be negative." ) );
            m_Parent->SetError( msg, this, m_diffPairsGrid, row, 2 );
            return false;
        }
    }

    return true;
}


void PANEL_SETUP_TRACKS_AND_VIAS::ImportSettingsFrom( BOARD* aBoard )
{
    m_trackWidthsGrid->CommitPendingChanges( true );
    m_viaSizesGrid->CommitPendingChanges( true );
    m_diffPairsGrid->CommitPendingChanges( true );

    // Note: do not change the board, as we need to get the current nets from it for
    // netclass memberships.  All the netclass definitions and dimension lists are in
    // the BOARD_DESIGN_SETTINGS.

    BOARD_DESIGN_SETTINGS* savedSettings = m_BrdSettings;

    m_BrdSettings = &aBoard->GetDesignSettings();
    TransferDataToWindow();

    m_BrdSettings = savedSettings;
}

void PANEL_SETUP_TRACKS_AND_VIAS::AppendTrackWidth( const int aWidth )
{
    int i = m_trackWidthsGrid->GetNumberRows();

    m_trackWidthsGrid->AppendRows( 1 );

    wxString val = StringFromValue( m_Frame->GetUserUnits(), aWidth, true, true );
    m_trackWidthsGrid->SetCellValue( i, TR_WIDTH_COL, val );
}

void PANEL_SETUP_TRACKS_AND_VIAS::AppendViaSize( const int aSize, const int aDrill )
{
    int i = m_viaSizesGrid->GetNumberRows();

    m_viaSizesGrid->AppendRows( 1 );

    wxString val = StringFromValue( m_Frame->GetUserUnits(), aSize, true, true );
    m_viaSizesGrid->SetCellValue( i, VIA_SIZE_COL, val );

    if( aDrill > 0 )
    {
        val = StringFromValue( m_Frame->GetUserUnits(), aDrill, true, true );
        m_viaSizesGrid->SetCellValue( i, VIA_DRILL_COL, val );
    }
}

void PANEL_SETUP_TRACKS_AND_VIAS::AppendDiffPairs( const int aWidth, const int aGap,
                                                   const int aViaGap )
{
    int i = m_diffPairsGrid->GetNumberRows();

    m_diffPairsGrid->AppendRows( 1 );

    wxString val = StringFromValue( m_Frame->GetUserUnits(), aWidth, true, true );
    m_diffPairsGrid->SetCellValue( i, DP_WIDTH_COL, val );

    if( aGap > 0 )
    {
        val = StringFromValue( m_Frame->GetUserUnits(), aGap, true, true );
        m_diffPairsGrid->SetCellValue( i, DP_GAP_COL, val );
    }

    if( aViaGap > 0 )
    {
        val = StringFromValue( m_Frame->GetUserUnits(), aViaGap, true, true );
        m_diffPairsGrid->SetCellValue( i, DP_VIA_GAP_COL, val );
    }
}

void PANEL_SETUP_TRACKS_AND_VIAS::OnAddTrackWidthsClick( wxCommandEvent& aEvent )
{
    AppendTrackWidth( 0 );

    m_trackWidthsGrid->MakeCellVisible( m_trackWidthsGrid->GetNumberRows() - 1, TR_WIDTH_COL );
    m_trackWidthsGrid->SetGridCursor( m_trackWidthsGrid->GetNumberRows() - 1, TR_WIDTH_COL );

    m_trackWidthsGrid->EnableCellEditControl( true );
    m_trackWidthsGrid->ShowCellEditControl();
}

void PANEL_SETUP_TRACKS_AND_VIAS::OnRemoveTrackWidthsClick( wxCommandEvent& event )
{
    int curRow = m_trackWidthsGrid->GetGridCursorRow();

    if( curRow < 0 || m_trackWidthsGrid->GetNumberRows() <= curRow )
        return;

    m_trackWidthsGrid->DeleteRows( curRow, 1 );

    curRow = std::max( 0, curRow - 1 );
    m_trackWidthsGrid->MakeCellVisible( curRow, m_trackWidthsGrid->GetGridCursorCol() );
    m_trackWidthsGrid->SetGridCursor( curRow, m_trackWidthsGrid->GetGridCursorCol() );
}

void PANEL_SETUP_TRACKS_AND_VIAS::OnAddViaSizesClick( wxCommandEvent& event )
{
    AppendViaSize( 0, 0 );

    m_viaSizesGrid->MakeCellVisible( m_viaSizesGrid->GetNumberRows() - 1, VIA_SIZE_COL );
    m_viaSizesGrid->SetGridCursor( m_viaSizesGrid->GetNumberRows() - 1, VIA_SIZE_COL );

    m_viaSizesGrid->EnableCellEditControl( true );
    m_viaSizesGrid->ShowCellEditControl();
}

void PANEL_SETUP_TRACKS_AND_VIAS::OnRemoveViaSizesClick( wxCommandEvent& event )
{
    int curRow = m_viaSizesGrid->GetGridCursorRow();

    if( curRow < 0 || m_viaSizesGrid->GetNumberRows() <= curRow )
        return;

    m_viaSizesGrid->DeleteRows( curRow, 1 );

    curRow = std::max( 0, curRow - 1 );
    m_viaSizesGrid->MakeCellVisible( curRow, m_viaSizesGrid->GetGridCursorCol() );
    m_viaSizesGrid->SetGridCursor( curRow, m_viaSizesGrid->GetGridCursorCol() );
}

void PANEL_SETUP_TRACKS_AND_VIAS::OnAddDiffPairsClick( wxCommandEvent& event )
{
    AppendDiffPairs( 0, 0, 0 );

    m_diffPairsGrid->MakeCellVisible( m_diffPairsGrid->GetNumberRows() - 1, DP_WIDTH_COL );
    m_diffPairsGrid->SetGridCursor( m_diffPairsGrid->GetNumberRows() - 1, DP_WIDTH_COL );

    m_diffPairsGrid->EnableCellEditControl( true );
    m_diffPairsGrid->ShowCellEditControl();
}

void PANEL_SETUP_TRACKS_AND_VIAS::OnRemoveDiffPairsClick( wxCommandEvent& event )
{
    int curRow = m_diffPairsGrid->GetGridCursorRow();

    if( curRow < 0 || m_diffPairsGrid->GetNumberRows() <= curRow )
        return;

    m_diffPairsGrid->DeleteRows( curRow, 1 );

    curRow = std::max( 0, curRow - 1 );
    m_diffPairsGrid->MakeCellVisible( curRow, m_diffPairsGrid->GetGridCursorCol() );
    m_diffPairsGrid->SetGridCursor( curRow, m_diffPairsGrid->GetGridCursorCol() );
}
