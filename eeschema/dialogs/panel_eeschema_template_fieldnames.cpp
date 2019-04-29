/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
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
#include <base_screen.h>
#include <widgets/wx_grid.h>
#include <template_fieldnames.h>
#include <grid_tricks.h>
#include <sch_edit_frame.h>
#include <bitmaps.h>

#include <panel_eeschema_template_fieldnames.h>

PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::PANEL_EESCHEMA_TEMPLATE_FIELDNAMES( SCH_EDIT_FRAME* aFrame,
                                                                        wxWindow* aWindow ) :
        PANEL_EESCHEMA_TEMPLATE_FIELDNAMES_BASE( aWindow ),
        m_frame( aFrame )
{
    m_addFieldButton->SetBitmap( KiBitmap( small_plus_xpm ) );
    m_deleteFieldButton->SetBitmap( KiBitmap( trash_xpm ) );

    m_checkboxColWidth = m_grid->GetColSize( 1 );

    m_grid->PushEventHandler( new GRID_TRICKS( m_grid ) );
}


PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::~PANEL_EESCHEMA_TEMPLATE_FIELDNAMES()
{
    // Delete the GRID_TRICKS.
    m_grid->PopEventHandler( true );
}


bool PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::TransferDataToWindow()
{
    m_fields = m_frame->GetTemplateFieldNames();
    return TransferDataToGrid();
}


void PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::OnAddButtonClick( wxCommandEvent& event )
{
    if( !m_grid->CommitPendingChanges() )
        return;

    int row = m_grid->GetNumberRows();
    TransferDataFromGrid();

    TEMPLATE_FIELDNAME newFieldname = TEMPLATE_FIELDNAME( "Fieldname" );
    newFieldname.m_Visible = false;
    m_fields.insert( m_fields.end(), newFieldname );
    TransferDataToGrid();

    // wx documentation is wrong, SetGridCursor does not make visible.
    m_grid->MakeCellVisible( row, 0 );
    m_grid->SetGridCursor( row, 0 );
}


void PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::OnDeleteButtonClick( wxCommandEvent& event )
{
    if( !m_grid->CommitPendingChanges() )
        return;

    int curRow = m_grid->GetGridCursorRow();

    if( curRow >= 0 && curRow < (int)m_fields.size() )
    {
        m_fields.erase( m_fields.begin() + curRow );
        m_grid->DeleteRows( curRow );
    }

    m_grid->MakeCellVisible( std::max( 0, curRow-1 ), m_grid->GetGridCursorCol() );
    m_grid->SetGridCursor( std::max( 0, curRow-1 ), m_grid->GetGridCursorCol() );
}


bool PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::TransferDataToGrid()
{
    m_grid->Freeze();

    if( m_grid->GetNumberRows() )
        m_grid->DeleteRows( 0, m_grid->GetNumberRows() );

    m_grid->AppendRows( m_fields.size() );

    for( int row = 0; row < m_grid->GetNumberRows(); ++row )
    {
        m_grid->SetCellValue( row, 0, m_fields[row].m_Name );
        m_grid->SetCellValue( row, 1, m_fields[row].m_Visible ? wxT( "1" ) : wxEmptyString );
        m_grid->SetCellValue( row, 2, m_fields[row].m_URL     ? wxT( "1" ) : wxEmptyString );

        // Set cell properties
        m_grid->SetCellAlignment( row, 0, wxALIGN_LEFT, wxALIGN_CENTRE );

        // Render the Visible and URL columns as check boxes
        m_grid->SetCellRenderer( row, 1, new wxGridCellBoolRenderer() );
        m_grid->SetReadOnly( row, 1 );  // Not really; we delegate interactivity to GRID_TRICKS
        m_grid->SetCellAlignment( row, 1, wxALIGN_CENTRE, wxALIGN_CENTRE );

        m_grid->SetCellRenderer( row, 2, new wxGridCellBoolRenderer() );
        m_grid->SetReadOnly( row, 2 );  // Not really; we delegate interactivity to GRID_TRICKS
        m_grid->SetCellAlignment( row, 2, wxALIGN_CENTRE, wxALIGN_CENTRE );
    }

    m_grid->Thaw();

    return true;
}


bool PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::TransferDataFromGrid()
{
    if( !m_grid->CommitPendingChanges() )
        return false;

    for( int row = 0; row < m_grid->GetNumberRows(); ++row )
    {
        m_fields[row].m_Name  = m_grid->GetCellValue( row, 0 );
        m_fields[row].m_Visible = ( m_grid->GetCellValue( row, 1 ) != wxEmptyString );
        m_fields[row].m_URL     = ( m_grid->GetCellValue( row, 2 ) != wxEmptyString );
    }

    return true;
}


bool PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::TransferDataFromWindow()
{
    if( !TransferDataFromGrid() )
        return false;

    m_frame->DeleteAllTemplateFieldNames();

    for( const TEMPLATE_FIELDNAME& field : m_fields )
        m_frame->AddTemplateFieldName( field );

    return true;
}


void PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::AdjustGridColumns( int aWidth )
{
    if( aWidth <= 0 )
        return;

    // Account for scroll bars
    aWidth -= ( m_grid->GetSize().x - m_grid->GetClientSize().x );

    m_grid->SetColSize( 0, aWidth - 2 * m_checkboxColWidth );
    m_grid->SetColSize( 1, m_checkboxColWidth );
    m_grid->SetColSize( 2, m_checkboxColWidth );
}


void PANEL_EESCHEMA_TEMPLATE_FIELDNAMES::OnSizeGrid( wxSizeEvent& event )
{
    AdjustGridColumns( event.GetSize().GetX() );

    event.Skip();
}
