/**
 * @file dialog_gfx_import.cpp
 * @brief Dialog to import a vector graphics file on a given board layer.
 */

/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 1992-2019 KiCad Developers, see AUTHORS.txt for contributors.
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

#include "dialog_import_gfx.h"

#include <advanced_config.h>
#include <convert_to_biu.h>
#include <kiface_i.h>
#include <pcb_layer_box_selector.h>
#include <wildcards_and_files_ext.h>

#include <class_board.h>
#include <class_module.h>
#include <class_edge_mod.h>
#include <class_text_mod.h>
#include <class_pcb_text.h>

// Configuration path (group) to store entry keys below.
#define IMPORT_GFX_GROUP                        "ImportGraphics"

// Entry keys to store setup in config
#define IMPORT_GFX_LAYER_OPTION_KEY             "BoardLayer"
#define IMPORT_GFX_PLACEMENT_INTERACTIVE_KEY    "InteractivePlacement"
#define IMPORT_GFX_LAST_FILE_KEY                "LastFile"
#define IMPORT_GFX_POSITION_UNITS_KEY           "PositionUnits"
#define IMPORT_GFX_POSITION_X_KEY               "PositionX"
#define IMPORT_GFX_POSITION_Y_KEY               "PositionY"
#define IMPORT_GFX_LINEWIDTH_UNITS_KEY          "LineWidthUnits"
#define IMPORT_GFX_LINEWIDTH_KEY                "LineWidth"


// Static members of DIALOG_IMPORT_GFX, to remember the user's choices during the session
wxString DIALOG_IMPORT_GFX::m_filename;
bool DIALOG_IMPORT_GFX::m_placementInteractive = true;
LAYER_NUM DIALOG_IMPORT_GFX::m_layer = Dwgs_User;
double DIALOG_IMPORT_GFX::m_scaleImport = 1.0;     // Do not change the imported items size
int DIALOG_IMPORT_GFX::m_originUnits = 0;          // millimeter
int DIALOG_IMPORT_GFX::m_lineWidthUnits = 0;       // millimeter


DIALOG_IMPORT_GFX::DIALOG_IMPORT_GFX( PCB_BASE_FRAME* aParent, bool aImportAsFootprintGraphic )
    : DIALOG_IMPORT_GFX_BASE( aParent )
{
    m_parent = aParent;

    if( aImportAsFootprintGraphic )
        m_importer.reset( new GRAPHICS_IMPORTER_MODULE( m_parent->GetBoard()->m_Modules ) );
    else
        m_importer.reset( new GRAPHICS_IMPORTER_BOARD( m_parent->GetBoard() ) );

    // construct an import manager with options from config
    {
        GRAPHICS_IMPORT_MGR::TYPE_LIST blacklist;

        if( !ADVANCED_CFG::GetCfg().m_enableSvgImport )
            blacklist.push_back( GRAPHICS_IMPORT_MGR::SVG );

        m_gfxImportMgr = std::make_unique<GRAPHICS_IMPORT_MGR>( blacklist );
    }

    m_config = Kiface().KifaceSettings();
    m_originUnits = 0;
    m_origin.x = 0.0;              // always in mm
    m_origin.y = 0.0;              // always in mm
    m_lineWidth = 0.2;             // always in mm
    m_lineWidthUnits = 0;

    if( m_config )
    {
        wxString tmp = m_config->GetPath();
        m_config->SetPath( IMPORT_GFX_GROUP );
        m_layer = m_config->Read( IMPORT_GFX_LAYER_OPTION_KEY, (long)Dwgs_User );
        m_placementInteractive = m_config->Read( IMPORT_GFX_PLACEMENT_INTERACTIVE_KEY, true );
        m_filename =  m_config->Read( IMPORT_GFX_LAST_FILE_KEY, wxEmptyString );
        m_config->Read( IMPORT_GFX_LINEWIDTH_KEY, &m_lineWidth, 0.2 );
        m_config->Read( IMPORT_GFX_LINEWIDTH_UNITS_KEY, &m_lineWidthUnits, 0 );
        m_config->Read( IMPORT_GFX_POSITION_UNITS_KEY, &m_originUnits, 0 );
        m_config->Read( IMPORT_GFX_POSITION_X_KEY, &m_origin.x, 0.0 );
        m_config->Read( IMPORT_GFX_POSITION_Y_KEY, &m_origin.y, 0.0 );
        m_config->SetPath( tmp );
    }

    m_choiceUnitLineWidth->SetSelection( m_lineWidthUnits );
    showPCBdefaultLineWidth();

    m_DxfPcbPositionUnits->SetSelection( m_originUnits );
    showPcbImportOffsets();

    m_textCtrlFileName->SetValue( m_filename );
    m_rbInteractivePlacement->SetValue( m_placementInteractive );
    m_rbAbsolutePlacement->SetValue( not m_placementInteractive );

    m_textCtrlImportScale->SetValue( wxString::Format( "%f", m_scaleImport ) );

    // Configure the layers list selector
    m_SelLayerBox->SetLayersHotkeys( false );                    // Do not display hotkeys
    m_SelLayerBox->SetNotAllowedLayerSet( LSET::AllCuMask() );   // Do not use copper layers
    m_SelLayerBox->SetBoardFrame( m_parent );
    m_SelLayerBox->Resync();

    if( m_SelLayerBox->SetLayerSelection( m_layer ) < 0 )
    {
        m_layer = Dwgs_User;
        m_SelLayerBox->SetLayerSelection( m_layer );
    }

    SetInitialFocus( m_textCtrlFileName );
    m_sdbSizerOK->SetDefault();
    GetSizer()->Fit( this );
    GetSizer()->SetSizeHints( this );
    Centre();
}


DIALOG_IMPORT_GFX::~DIALOG_IMPORT_GFX()
{
    if( m_config )
    {
        wxString tmp = m_config->GetPath();
        m_config->SetPath( IMPORT_GFX_GROUP );
        m_config->Write( IMPORT_GFX_LAYER_OPTION_KEY, (long)m_layer );
        m_config->Write( IMPORT_GFX_PLACEMENT_INTERACTIVE_KEY, m_placementInteractive );
        m_config->Write( IMPORT_GFX_LAST_FILE_KEY, m_filename );

        m_config->Write( IMPORT_GFX_POSITION_UNITS_KEY, m_originUnits );
        m_config->Write( IMPORT_GFX_POSITION_X_KEY, m_origin.x );
        m_config->Write( IMPORT_GFX_POSITION_Y_KEY, m_origin.y );

        m_config->Write( IMPORT_GFX_LINEWIDTH_KEY, m_lineWidth );
        m_config->Write( IMPORT_GFX_LINEWIDTH_UNITS_KEY, m_lineWidthUnits );
        m_config->SetPath( tmp );
    }
}


void DIALOG_IMPORT_GFX::DIALOG_IMPORT_GFX::onUnitPositionSelection( wxCommandEvent& event )
{
    // Collect last entered values:
    updatePcbImportOffsets_mm();

    m_originUnits = m_DxfPcbPositionUnits->GetSelection();;
    showPcbImportOffsets();
}


double DIALOG_IMPORT_GFX::getPCBdefaultLineWidthMM()
{
    double value = DoubleValueFromString( UNSCALED_UNITS, m_textCtrlLineWidth->GetValue() );

    switch( m_lineWidthUnits )
    {
    default:
    case 0:     // display units = mm
        break;

    case 1:     // display units = mil
        value *= 25.4 / 1000;
        break;

    case 2:     // display units = inch
        value *= 25.4;
        break;
    }

    return value;   // value is in mm
}


void DIALOG_IMPORT_GFX::onUnitWidthSelection( wxCommandEvent& event )
{
    m_lineWidth = getPCBdefaultLineWidthMM();

    // Switch to new units
    m_lineWidthUnits = m_choiceUnitLineWidth->GetSelection();
    showPCBdefaultLineWidth();
}


void DIALOG_IMPORT_GFX::showPcbImportOffsets()
{
    // Display m_origin value according to the unit selection:
    VECTOR2D offset = m_origin;

    if( m_originUnits )   // Units are inches
        offset = m_origin / 25.4;

    m_DxfPcbXCoord->SetValue( wxString::Format( "%f", offset.x ) );
    m_DxfPcbYCoord->SetValue( wxString::Format( "%f", offset.y ) );

}


void DIALOG_IMPORT_GFX::showPCBdefaultLineWidth()
{
    double value;

    switch( m_lineWidthUnits )
    {
    default:
    case 0:     // display units = mm
        value = m_lineWidth;
        break;

    case 1:     // display units = mil
        value = m_lineWidth / 25.4 * 1000;
        break;

    case 2:     // display units = inch
        value = m_lineWidth / 25.4;
        break;
    }

    m_textCtrlLineWidth->SetValue( wxString::Format( "%f", value ) );
}


void DIALOG_IMPORT_GFX::onBrowseFiles( wxCommandEvent& event )
{
    wxString path;
    wxString filename;

    if( !m_filename.IsEmpty() )
    {
        wxFileName fn( m_filename );
        path = fn.GetPath();
        filename = fn.GetFullName();
    }

    // Generate the list of handled file formats
    wxString wildcardsDesc;
    wxString allWildcards;

    for( auto pluginType : m_gfxImportMgr->GetImportableFileTypes() )
    {
        auto       plugin = m_gfxImportMgr->GetPlugin( pluginType );
        const auto wildcards = plugin->GetWildcards();

        wildcardsDesc += "|" + plugin->GetName() + " (" + wildcards + ")|" + wildcards;
        allWildcards += wildcards + ";";
    }

    wildcardsDesc = _( "All supported formats|" ) + allWildcards + wildcardsDesc;

    wxFileDialog dlg( m_parent, _( "Open File" ), path, filename,
                      wildcardsDesc, wxFD_OPEN|wxFD_FILE_MUST_EXIST );

    if( dlg.ShowModal() != wxID_OK )
        return;

    wxString fileName = dlg.GetPath();

    if( fileName.IsEmpty() )
        return;

    m_filename = fileName;
    m_textCtrlFileName->SetValue( fileName );
}


bool DIALOG_IMPORT_GFX::TransferDataFromWindow()
{
    if( !wxDialog::TransferDataFromWindow() )
        return false;

    m_filename = m_textCtrlFileName->GetValue();

    if( m_filename.IsEmpty() )
    {
        wxMessageBox( _( "No file selected!" ) );
        return false;
    }

    m_originUnits = m_DxfPcbPositionUnits->GetSelection();
    updatePcbImportOffsets_mm();      // Update m_originX and m_originY;

    m_layer = m_SelLayerBox->GetLayerSelection();

    if( m_layer < 0 )
    {
        wxMessageBox( _( "Please select a valid layer." ) );
        return false;
    }

    m_lineWidthUnits = m_choiceUnitLineWidth->GetSelection();
    m_lineWidth = getPCBdefaultLineWidthMM();

    m_importer->SetLayer( PCB_LAYER_ID( m_layer ) );

    auto plugin = m_gfxImportMgr->GetPluginByExt( wxFileName( m_filename ).GetExt() );

    if( plugin )
    {
        // Set coordinates offset for import (offset is given in mm)
        m_importer->SetImportOffsetMM( m_origin );
        m_scaleImport = DoubleValueFromString( UNSCALED_UNITS, m_textCtrlImportScale->GetValue() );

        m_importer->SetLineWidthMM( m_lineWidth );
        m_importer->SetPlugin( std::move( plugin ) );

        LOCALE_IO dummy;    // Ensure floats can be read.

        if( m_importer->Load( m_filename ) )
            m_importer->Import( m_scaleImport );

        // Get warning messages:
        const std::string& warnings = m_importer->GetMessages();

        // This isn't a fatal error so allow the dialog to close with wxID_OK.
        if( !warnings.empty() )
            wxMessageBox( warnings.c_str(), _( "Items Not Handled" ) );
    }
    else
    {
        wxMessageBox( _( "There is no plugin to handle this file type." ) );
        return false;
    }

    return true;
}


void DIALOG_IMPORT_GFX::originOptionOnUpdateUI( wxUpdateUIEvent& event )
{
    if( m_rbInteractivePlacement->GetValue() != m_placementInteractive )
        m_rbInteractivePlacement->SetValue( m_placementInteractive );

    if( m_rbAbsolutePlacement->GetValue() == m_placementInteractive )
        m_rbAbsolutePlacement->SetValue( not m_placementInteractive );

    m_DxfPcbPositionUnits->Enable( not m_placementInteractive );
    m_DxfPcbXCoord->Enable( not m_placementInteractive );
    m_DxfPcbYCoord->Enable( not m_placementInteractive );
}


void DIALOG_IMPORT_GFX::updatePcbImportOffsets_mm()
{
    m_origin.x = DoubleValueFromString( UNSCALED_UNITS, m_DxfPcbXCoord->GetValue() );
    m_origin.y = DoubleValueFromString( UNSCALED_UNITS, m_DxfPcbYCoord->GetValue() );

    if( m_originUnits )   // Units are inches
    {
        m_origin = m_origin * 25.4;
    }

    return;
}


// Used only in legacy canvas by the board editor.
bool InvokeDialogImportGfxBoard( PCB_BASE_FRAME* aCaller )
{
    DIALOG_IMPORT_GFX dlg( aCaller );

    if( dlg.ShowModal() != wxID_OK )
        return false;

    auto& list = dlg.GetImportedItems();

    // Ensure the list is not empty:
    if( list.empty() )
    {
        wxMessageBox( _( "No graphic items found in file to import." ) );
        return false;
    }

    PICKED_ITEMS_LIST picklist;         // the pick list for undo command
    ITEM_PICKER item_picker( nullptr, UR_NEW );
    BOARD* board = aCaller->GetBoard();

    // Now prepare a block move command to place the new items, if interactive placement,
    // and prepare the undo command.
    EDA_RECT bbox;          // the new items bounding box, for block move if interactive placement.
    bool bboxInit = true;   // true until the bounding box is initialized
    BLOCK_SELECTOR& blockmove = aCaller->GetScreen()->m_BlockLocate;

    if( dlg.IsPlacementInteractive() )
        aCaller->HandleBlockBegin( NULL, BLOCK_PRESELECT_MOVE, wxPoint( 0, 0 ) );

    PICKED_ITEMS_LIST& blockitemsList = blockmove.GetItems();

    for( auto it = list.begin(); it != list.end(); ++it )
    {
        BOARD_ITEM* item = static_cast<BOARD_ITEM*>( it->release() );

        if( dlg.IsPlacementInteractive() )
            item->SetFlags( IS_MOVED );

        board->Add( item );

        item_picker.SetItem( item );
        picklist.PushItem( item_picker );

        if( dlg.IsPlacementInteractive() )
        {
            blockitemsList.PushItem( item_picker );

            if( bboxInit )
                bbox = item->GetBoundingBox();
            else
                bbox.Merge( item->GetBoundingBox() );

            bboxInit = false;
       }
    }

    aCaller->SaveCopyInUndoList( picklist, UR_NEW, wxPoint( 0, 0 ) );
    aCaller->OnModify();

    if( dlg.IsPlacementInteractive() )
    {
        // Finish block move command:
        wxPoint cpos = aCaller->GetNearestGridPosition( bbox.Centre() );
        blockmove.SetOrigin( bbox.GetOrigin() );
        blockmove.SetSize( bbox.GetSize() );
        blockmove.SetLastCursorPosition( cpos );
        aCaller->HandleBlockEnd( NULL );
    }

    return true;
}


// Used only in legacy canvas by the footprint editor.
bool InvokeDialogImportGfxModule( PCB_BASE_FRAME* aCaller, MODULE* aModule )
{
    if( !aModule )
        return false;

    DIALOG_IMPORT_GFX dlg( aCaller, true );

    if( dlg.ShowModal() != wxID_OK )
        return false;

    auto& list = dlg.GetImportedItems();

    // Ensure the list is not empty:
    if( list.empty() )
    {
        wxMessageBox( _( "No graphic items found in file to import" ) );
        return false;
    }

    aCaller->SaveCopyInUndoList( aModule, UR_CHANGED );

    PICKED_ITEMS_LIST picklist;         // the pick list for undo command
    ITEM_PICKER item_picker( nullptr, UR_NEW );

    // Now prepare a block move command to place the new items, if interactive placement,
    // and prepare the undo command.
    EDA_RECT bbox;          // the new items bounding box, for block move if interactive placement.
    bool bboxInit = true;   // true until the bounding box is initialized
    BLOCK_SELECTOR& blockmove = aCaller->GetScreen()->m_BlockLocate;

    if( dlg.IsPlacementInteractive() )
        aCaller->HandleBlockBegin( nullptr, BLOCK_PRESELECT_MOVE, wxPoint( 0, 0 ) );

    PICKED_ITEMS_LIST& blockitemsList = blockmove.GetItems();

    for( auto it = list.begin(); it != list.end(); ++it )
    {
        BOARD_ITEM* item = static_cast<BOARD_ITEM*>( it->release() );
        aModule->Add( item );

        if( dlg.IsPlacementInteractive() )
        {
            item->SetFlags( IS_MOVED );
            item_picker.SetItem( item );
            blockitemsList.PushItem( item_picker );

            if( bboxInit )
                bbox = item->GetBoundingBox();
            else
                bbox.Merge( item->GetBoundingBox() );

            bboxInit = false;
       }
    }

    aCaller->OnModify();

    if( dlg.IsPlacementInteractive() )
    {
        // Finish block move command:
        wxPoint cpos = aCaller->GetNearestGridPosition( bbox.Centre() );
        blockmove.SetOrigin( bbox.GetOrigin() );
        blockmove.SetSize( bbox.GetSize() );
        blockmove.SetLastCursorPosition( cpos );
        aCaller->HandleBlockEnd( NULL );
    }

    return true;
}
