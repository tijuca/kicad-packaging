/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Oliver Walters
 * Copyright (C) 2017-2018 KiCad Developers, see AUTHORS.txt for contributors.
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


#include <wx/colour.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/file.h>
#include <wx/filename.h>

#include <confirm.h>

#include <build_version.h>
#include <general.h>
#include <class_library.h>

#include "dialog_fields_editor_global.h"
#include <fields_editor_table_model.h>


// Create and show fields editor
void InvokeDialogCreateBOMEditor( SCH_EDIT_FRAME* aCaller )
{
    DIALOG_FIELDS_EDITOR_GLOBAL dlg( aCaller );
    dlg.ShowQuasiModal();
}


DIALOG_FIELDS_EDITOR_GLOBAL::DIALOG_FIELDS_EDITOR_GLOBAL( SCH_EDIT_FRAME* parent ) :
        DIALOG_FIELDS_EDITOR_GLOBAL_BASE( parent ),
        m_parent( parent )
{
    m_bom = FIELDS_EDITOR_TABLE_MODEL::Create();

    auto nameColumn = m_columnListCtrl->AppendTextColumn( _( "Field" ) );

    auto showColumn = m_columnListCtrl->AppendToggleColumn(
                                        _( "Show" ),
                                        wxDATAVIEW_CELL_ACTIVATABLE,
                                        100 );

    auto sortColumn = m_columnListCtrl->AppendToggleColumn(
                                        _( "Sort" ),
                                        wxDATAVIEW_CELL_ACTIVATABLE,
                                        100 );


    // Read all components
    LoadComponents();

    LoadColumnNames();
    ReloadColumns();

    m_bom->ReloadTable();

    // Set default column widths for fields table
    showColumn->SetWidth( wxCOL_WIDTH_AUTOSIZE );
    showColumn->SetResizeable( false );

    nameColumn->SetWidth( wxCOL_WIDTH_AUTOSIZE );
    nameColumn->SetResizeable( true );

    sortColumn->SetWidth( wxCOL_WIDTH_AUTOSIZE );
    sortColumn->SetResizeable( false );


    // Reference column is either single reference or a list of references.
    // Autosize can fill the window in the case of a list so use a fixed width.
    // wxCOL_WIDTH_DEFAULT is wxDVC_DEFAULT_WIDTH on all platforms and too small.
    auto refcol = m_bomView->GetColumn( 0 );
    refcol->SetWidth( wxDVC_DEFAULT_WIDTH * 2 );
    refcol->SetResizeable( true );

    // Set default column widths for BOM table
    for( unsigned int ii = 1; ii < m_bomView->GetColumnCount(); ii++ )
    {
        auto col = m_bomView->GetColumn( ii );

        if( !col )
            continue;

        col->SetWidth( wxCOL_WIDTH_AUTOSIZE );
        col->SetResizeable( true );
    }

    Layout();
    SetSizeInDU( 400, 240 );
    Center();
}


DIALOG_FIELDS_EDITOR_GLOBAL::~DIALOG_FIELDS_EDITOR_GLOBAL()
{
    // Nothing to do.
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnCloseButton( wxCommandEvent& event )
{
    // DIALOG_FIELDS_EDITOR_GLOBAL::OnDialogClosed() will be called,
    // when closing this dialog.
    // The default wxID_CANCEL handler is not suitable for us,
    // because it calls DIALOG_SHIM::EndQuasiModal() without calling
    // DIALOG_FIELDS_EDITOR_GLOBAL::OnDialogClosed()
    Close();
}


bool DIALOG_FIELDS_EDITOR_GLOBAL::CanCloseDialog()
{
    if( !m_bom->HaveFieldsChanged() )
        return true;

    int result = DisplayExitDialog( this, _( "Changes exist in component table" ) );

    switch( result )
    {
    case wxID_CANCEL:
       return false;

    case wxID_NO:
       break;

    case wxID_YES:
       ApplyAllChanges();
       break;
    }

    return true;
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnDialogClosed( wxCloseEvent& event )
{
    if( !CanCloseDialog() )
    {
        event.Veto();
    }
    else
        // Mandatory to call DIALOG_SHIM::OnCloseWindow( wxCloseEvent& aEvent )
        // and actually close the dialog
        event.Skip();
}


/* Struct for keeping track of schematic sheet changes
 * Stores:
 * SHEET_PATH - Schematic to apply changes to
 * PICKED_ITEMS_LIST - List of changes to apply
 */

typedef struct
{
    SCH_SHEET_PATH path;
    PICKED_ITEMS_LIST items;
} SheetUndoList;


void DIALOG_FIELDS_EDITOR_GLOBAL::ApplyAllChanges()
{
    if( !m_bom->HaveFieldsChanged() )
        return;

     /**
      * As we may be saving changes across multiple sheets,
      * we need to first determine which changes need to be made to which sheet.
      * To this end, we perform the following:
      * 1. Save the "path" of the currently displayed sheet
      * 2. Create a MAP of <SheetPath:ChangeList> changes that need to be made
      * 3. Push UNDO actions to appropriate sheets
      * 4. Perform all the update actions
      * 5. Reset the view to the current sheet
      */

    auto currentSheet = m_parent->GetCurrentSheet();

    //! Create a map of changes required for each sheet
    std::map<wxString, SheetUndoList> undoSheetMap;

    // List of components that have changed
    auto changed = m_bom->GetChangedComponents();

    ITEM_PICKER picker;

    // Iterate through each of the components that were changed
    for( const auto& ref : changed )
    {
        // Extract the SCH_COMPONENT* object
        auto cmp = ref.GetComp();

        wxString path = ref.GetSheetPath().Path();

        // Push the component into the picker list
        picker = ITEM_PICKER( cmp, UR_CHANGED );
        picker.SetFlags( cmp->GetFlags() );

        /*
         * If there is not currently an undo list for the given sheet,
         * create an empty one
         */

        if( undoSheetMap.count( path ) == 0 )
        {
            SheetUndoList newList;

            newList.path = ref.GetSheetPath();

            undoSheetMap[path] = newList;
        }

        auto& pickerList = undoSheetMap[path];

        pickerList.items.PushItem( picker );
    }

    // Iterate through each sheet that needs updating
    for( auto it = undoSheetMap.begin(); it != undoSheetMap.end(); ++it )
    {
        auto undo = it->second;

        m_parent->SetCurrentSheet( undo.path );
        m_parent->SaveCopyInUndoList( undo.items, UR_CHANGED );
        m_parent->OnModify();
    }

    // Make all component changes
    m_bom->ApplyFieldChanges();

    // Redraw the current sheet and mark as dirty
    m_parent->Refresh();
    m_parent->OnModify();

    // Reset the view to where we left the user
    m_parent->SetCurrentSheet(currentSheet);

    // Instruct the table to set the current values as the new backup values
    m_bom->SetBackupPoint();
}


void DIALOG_FIELDS_EDITOR_GLOBAL::UpdateTitle()
{
    wxString title;


    if( m_bom->GetColumnGrouping() )
    {
        title.Printf ( _( "Symbol Table - %u symbols in %u groups" ),
                       m_bom->ComponentCount(),
                       (unsigned int) m_bom->Groups.size() );
    }
    else
        title.Printf ( _( "Symbol Table - %u components" ),
                       m_bom->ComponentCount() );

    unsigned int count = m_bom->CountChangedComponents();

    if( count > 0 )
        title += wxString::Format( _( " - %u changed" ), count );

    // Update title only if it has changed, to avoid flicker created by
    // useless update, for instance when moving the mouse, because UpdateTitle()
    // is called by a wxUpdateUIEvent:
    if( GetTitle() != title )
        SetTitle( title );
}


void DIALOG_FIELDS_EDITOR_GLOBAL::LoadComponents()
{
    if( !m_parent ) return;

    // List of component objects
    SCH_REFERENCE_LIST refs;

    // Generate a list of schematic sheets
    SCH_SHEET_LIST sheets( g_RootSheet );
    sheets.GetComponents( refs, false );

    // Pass the references through to the model
    m_bom->SetComponents( refs, m_parent->GetTemplateFieldNames() );
}


void DIALOG_FIELDS_EDITOR_GLOBAL::LoadColumnNames()
{
    m_columnListCtrl->DeleteAllItems();

    wxVector< wxVariant > data;

    for( auto* col : m_bom->ColumnList.Columns )
    {
        if( nullptr == col )
            continue;

        data.clear();

        data.push_back( wxVariant( col->Title() ) );        // Column title      (string)
        data.push_back( wxVariant( col->IsVisible() ) );    // Column visibility (bool)
        data.push_back( wxVariant( col->IsUsedToSort() ) ); // Column is used to sort

        m_columnListCtrl->AppendItem( data );
    }
}


void DIALOG_FIELDS_EDITOR_GLOBAL::ReloadColumns()
{
    m_bom->AttachTo( m_bomView );
    UpdateTitle();
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnColumnItemToggled( wxDataViewEvent& event )
{
    wxDataViewItem item = event.GetItem();

    int row = m_columnListCtrl->ItemToRow( item );
    int col = event.GetColumn();

    if( row == wxNOT_FOUND || row < 0 || row >= (int) m_bom->ColumnCount() )
        return;

    FIELDS_EDITOR_COLUMN* bomColumn = m_bom->ColumnList.GetColumnByIndex( row );

    if( nullptr == bomColumn )
        return;

    bool bValue = m_columnListCtrl->GetToggleValue( row, col );

    switch ( col )
    {
    default:
        break;

    case 1: // Column visibility
        bomColumn->SetVisible( bValue );

        // Insert a new column
        if( bValue )
        {
            m_bom->AddColumn( bomColumn );
        }
        else
        {
            m_bom->RemoveColumn( bomColumn );
        }
        break;

    case 2: // Column used to sort
        bomColumn->SetUsedToSort( bValue );
        m_bom->ReloadTable();
        break;
    }
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnGroupComponentsToggled( wxCommandEvent& event )
{
    bool group = m_groupComponentsBox->GetValue();

    m_bom->SetColumnGrouping( group );
    m_bom->ReloadTable();

    Update();
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnUpdateUI( wxUpdateUIEvent& event )
{
    m_regroupComponentsButton->Enable( m_bom->GetColumnGrouping() );

    bool changes = m_bom->HaveFieldsChanged();

    m_applyChangesButton->Enable( changes );
    m_revertChangesButton->Enable( changes );

    UpdateTitle();
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnTableValueChanged( wxDataViewEvent& event )
{
    Update();
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnRegroupComponents( wxCommandEvent& event )
{
    m_bom->ReloadTable();
    Update();
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnApplyFieldChanges( wxCommandEvent& event )
{
    ApplyAllChanges();
    Update();
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnRevertFieldChanges( wxCommandEvent& event )
{
    if( m_bom->HaveFieldsChanged() )
    {
        if( IsOK( this, _( "Revert all symbol table changes?" ) ) )
        {
            m_bom->RevertFieldChanges();
            Update();
        }
    }
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnTableItemActivated( wxDataViewEvent& event )
{
    /* TODO
     * - Focus on component selected in SCH_FRAME
     */

    event.Skip();
}


void DIALOG_FIELDS_EDITOR_GLOBAL::OnTableItemContextMenu( wxDataViewEvent& event )
{
    /* TODO
     * - Display contect menu
     * - Option to revert local changes if changes have been made
     * - Option to select footprint if FOOTPRINT column selected
     */

    event.Skip();
}
