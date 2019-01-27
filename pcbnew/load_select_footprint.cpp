/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras, jean-pierre.charras@ujf-grenoble.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
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

/**
 * @file load_select_footprint.cpp
 * @brief Footprints selection and loading functions.
 */

#include <functional>
using namespace std::placeholders;

#include <fctsys.h>
#include <class_drawpanel.h>
#include <pcb_draw_panel_gal.h>
#include <confirm.h>
#include <eda_doc.h>
#include <kicad_string.h>
#include <pgm_base.h>
#include <kiway.h>
#include <pcb_edit_frame.h>
#include <dialog_helpers.h>
#include <filter_reader.h>
#include <gr_basic.h>
#include <macros.h>
#include <fp_lib_table.h>
#include <lib_id.h>
#include <footprint_tree_pane.h>
#include <class_board.h>
#include <class_module.h>
#include <io_mgr.h>

#include <pcbnew.h>
#include <footprint_edit_frame.h>
#include <footprint_info.h>
#include <footprint_info_impl.h>
#include <dialog_choose_footprint.h>
#include <footprint_viewer_frame.h>
#include <wildcards_and_files_ext.h>
#include <widgets/progress_reporter.h>
#include <widgets/lib_tree.h>
#include "fp_tree_model_adapter.h"


static wxArrayString s_ModuleHistoryList;
static unsigned      s_ModuleHistoryMaxCount = 8;

static void AddModuleToHistory( const wxString& aName )
{
    // Remove duplicates
    for( int ii = s_ModuleHistoryList.GetCount() - 1; ii >= 0; --ii )
    {
        if( s_ModuleHistoryList[ ii ] == aName )
            s_ModuleHistoryList.RemoveAt( (size_t) ii );
    }

    // Add the new name at the beginning of the history list
    s_ModuleHistoryList.Insert( aName, 0 );

    // Remove extra names
    while( s_ModuleHistoryList.GetCount() >= s_ModuleHistoryMaxCount )
        s_ModuleHistoryList.RemoveAt( s_ModuleHistoryList.GetCount() - 1 );
}


static void clearModuleItemFlags( BOARD_ITEM* aItem )
{
    aItem->ClearFlags();
}

#include "pcbnew_id.h"
#include <bitmaps.h>
bool FOOTPRINT_EDIT_FRAME::Load_Module_From_BOARD( MODULE* aModule )
{
    bool is_last_fp_from_brd = IsCurrentFPFromBoard();

    MODULE* newModule;
    PCB_EDIT_FRAME* frame = (PCB_EDIT_FRAME*) Kiway().Player( FRAME_PCB, false );

    if( frame == NULL )     // happens if no board editor opened
        return false;

    if( aModule == NULL )
    {
        if( ! frame->GetBoard() || ! frame->GetBoard()->m_Modules )
            return false;

        aModule = SelectFootprintFromBoard( frame->GetBoard() );
    }

    if( aModule == NULL )
        return false;

    SetCurItem( NULL );

    if( !Clear_Pcb( true ) )
        return false;

    GetBoard()->m_Status_Pcb = 0;
    newModule = new MODULE( *aModule );
    newModule->SetParent( GetBoard() );
    newModule->SetLink( aModule->GetTimeStamp() );

    newModule->ClearFlags();
    newModule->RunOnChildren( std::bind( &clearModuleItemFlags, _1 ) );

    AddModuleToBoard( newModule );

    // Clear references to any net info, because the footprint editor
    // does know any thing about nets handled by the current edited board.
    // Morever we do not want to save any reference to an unknown net when
    // saving the footprint in lib cache
    // so we force the ORPHANED dummy net info for all pads
    newModule->ClearAllNets();

    SetCrossHairPosition( wxPoint( 0, 0 ) );
    PlaceModule( newModule, NULL );
    newModule->SetPosition( wxPoint( 0, 0 ) ); // cursor in GAL may not be initialized at the moment

    // Put it on FRONT layer,
    // because this is the default in ModEdit, and in libs
    if( newModule->GetLayer() != F_Cu )
        newModule->Flip( newModule->GetPosition() );

    // Put it in orientation 0,
    // because this is the default orientation in ModEdit, and in libs
    Rotate_Module( NULL, newModule, 0, false );
    Zoom_Automatique( false );

    m_adapter->SetPreselectNode( newModule->GetFPID(), 0 );

    GetScreen()->ClearUndoRedoList();
    GetScreen()->ClrModify();

    // Update the bitmap of the ID_MODEDIT_SAVE tool if needed.
    if( !is_last_fp_from_brd )
        ReCreateHToolbar();

    Update3DView();

    if( IsGalCanvasActive() )
        updateView();

    m_canvas->Refresh();

    m_treePane->GetLibTree()->Refresh();    // update any previously-highlighted items

    return true;
}


wxString PCB_BASE_FRAME::SelectFootprintFromLibBrowser()
{
    // Close the current non-modal Lib browser if opened, and open a new one, in "modal" mode:
    FOOTPRINT_VIEWER_FRAME* viewer;
    viewer = (FOOTPRINT_VIEWER_FRAME*) Kiway().Player( FRAME_PCB_MODULE_VIEWER, false );

    if( viewer )
    {
        viewer->Destroy();
        // Destroy() does not immediately delete the viewer, if some events are pending.
        // (for this reason delete operator cannot be used blindly with "top level" windows)
        // so gives a slice of time to delete the viewer frame.
        // This is especially important in OpenGL mode to avoid recreating context before
        // the old one is deleted
        wxSafeYield();
    }

    SetFocus();

    // Creates the modal Lib browser:
    viewer = (FOOTPRINT_VIEWER_FRAME*) Kiway().Player( FRAME_PCB_MODULE_VIEWER_MODAL, true, this );

    wxString    fpid;
    int ret = viewer->ShowModal( &fpid, this );
    (void) ret;     // make static analyser quiet

    viewer->Destroy();

    return fpid;
}


MODULE* PCB_BASE_FRAME::SelectFootprintFromLibTree( bool aAllowBrowser )
{
    FP_LIB_TABLE*   fpTable = Prj().PcbFootprintLibs();
    wxString        moduleName;
    LIB_ID          fpid;
    MODULE*         module = NULL;

    static wxString lastComponentName;

    WX_PROGRESS_REPORTER progressReporter( this, _( "Loading Footprint Libraries" ), 3 );
    GFootprintList.ReadFootprintFiles( fpTable, nullptr, &progressReporter );
    progressReporter.Show( false );

    if( GFootprintList.GetErrorCount() )
        GFootprintList.DisplayErrors( this );

    auto adapterPtr( FP_TREE_MODEL_ADAPTER::Create( fpTable ) );
    auto adapter = static_cast<FP_TREE_MODEL_ADAPTER*>( adapterPtr.get() );

    std::vector<LIB_TREE_ITEM*> historyInfos;

    for( auto const& item : s_ModuleHistoryList )
        historyInfos.push_back( GFootprintList.GetModuleInfo( item ) );

    adapter->DoAddLibrary( "-- " + _( "Recently Used" ) + " --", wxEmptyString, historyInfos, true );

    if( !historyInfos.empty() )
        adapter->SetPreselectNode( historyInfos[0]->GetLibId(), 0 );

    adapter->AddLibraries();

    wxString title;
    title.Printf( _( "Choose Footprint (%d items loaded)" ), adapter->GetItemCount() );

    DIALOG_CHOOSE_FOOTPRINT dialog( this, title, adapterPtr, aAllowBrowser );

    if( dialog.ShowQuasiModal() == wxID_CANCEL )
        return NULL;

    if( dialog.IsExternalBrowserSelected() )
    {
        // SelectFootprintFromLibBrowser() returns the "full" footprint name, i.e.
        // <lib_name>/<footprint name> or LIB_ID format "lib_name:fp_name:rev#"
        moduleName = SelectFootprintFromLibBrowser();

        if( moduleName.IsEmpty() )  // Cancel command
            return NULL;
        else
            fpid.Parse( moduleName, LIB_ID::ID_PCB );
    }
    else
    {
        fpid = dialog.GetSelectedLibId();

        if( !fpid.IsValid() )
            return NULL;
        else
            moduleName = fpid.Format();
    }

    try
    {
        module = loadFootprint( fpid );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogDebug( wxT( "Error loading footprint '%s'.\n\nError: %s" ),
                    fpid.Format().c_str(),
                    ioe.What() );
    }

    if( module )
    {
        lastComponentName = moduleName;
        AddModuleToHistory( moduleName );
    }

    return module;
}


MODULE* PCB_BASE_FRAME::LoadFootprint( const LIB_ID& aFootprintId )
{
    MODULE* module = NULL;

    try
    {
        module = loadFootprint( aFootprintId );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogDebug( wxT( "An error occurred attemping to load footprint '%s'.\n\nError: %s" ),
                    aFootprintId.Format().c_str(), GetChars( ioe.What() ) );
    }

    return module;
}


MODULE* PCB_BASE_FRAME::loadFootprint( const LIB_ID& aFootprintId )
{
    FP_LIB_TABLE*   fptbl = Prj().PcbFootprintLibs();

    wxCHECK_MSG( fptbl, NULL, wxT( "Cannot look up LIB_ID in NULL FP_LIB_TABLE." ) );

    MODULE *module = nullptr;
    try
    {
        module = fptbl->FootprintLoadWithOptionalNickname( aFootprintId );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogDebug( wxT( "An error occurred attemping to load footprint '%s'.\n\nError: %s" ),
                    aFootprintId.Format().c_str(), GetChars( ioe.What() ) );
    }

    // If the module is found, clear all net info,
    // to be sure there is no broken links
    // to any netinfo list (should be not needed, but it can be edited from
    // the footprint editor )
    if( module )
        module->ClearAllNets();

    return module;
}


MODULE* FOOTPRINT_EDIT_FRAME::SelectFootprintFromBoard( BOARD* aPcb )
{
    static wxString oldName;       // Save name of last module selected.

    wxString        fpname;
    wxString        msg;
    wxArrayString   listnames;
    MODULE*         module = aPcb->m_Modules;

    for(  ; module;  module = module->Next() )
        listnames.Add( module->GetReference() );

    msg.Printf( _( "Footprints [%u items]" ), (unsigned) listnames.GetCount() );

    wxArrayString headers;

    headers.Add( _( "Footprint" ) );

    std::vector<wxArrayString> itemsToDisplay;

    // Conversion from wxArrayString to vector of ArrayString
    for( unsigned i = 0; i < listnames.GetCount(); i++ )
    {
        wxArrayString item;

        item.Add( listnames[i] );
        itemsToDisplay.push_back( item );
    }

    EDA_LIST_DIALOG dlg( this, msg, headers, itemsToDisplay, wxEmptyString, NULL, NULL, SORT_LIST );

    if( dlg.ShowModal() == wxID_OK )
        fpname = dlg.GetTextSelection();
    else
        return NULL;

    oldName = fpname;

    module = aPcb->m_Modules;

    for(  ;  module;  module = module->Next() )
    {
        if( fpname == module->GetReference() )
            break;
    }

    return module;
}


bool FOOTPRINT_EDIT_FRAME::SaveLibraryAs( const wxString& aLibraryPath )
{
    wxString    curLibPath = aLibraryPath;
    wxString    dstLibPath = CreateNewLibrary();

    if( !dstLibPath )
        return false;             // user aborted in CreateNewLibrary()

    wxBusyCursor dummy;
    wxString msg;

    IO_MGR::PCB_FILE_T  dstType = IO_MGR::GuessPluginTypeFromLibPath( dstLibPath );
    IO_MGR::PCB_FILE_T  curType = IO_MGR::GuessPluginTypeFromLibPath( curLibPath );

    try
    {
        PLUGIN::RELEASER cur( IO_MGR::PluginFind( curType ) );
        PLUGIN::RELEASER dst( IO_MGR::PluginFind( dstType ) );

        wxArrayString footprints;

        cur->FootprintEnumerate( footprints, curLibPath );

        for( unsigned i = 0;  i < footprints.size();  ++i )
        {
            const MODULE* footprint = cur->GetEnumeratedFootprint( curLibPath, footprints[i] );
            dst->FootprintSave( dstLibPath, footprint );

            msg = wxString::Format( _( "Footprint \"%s\" saved" ), footprints[i] );
            SetStatusText( msg );
        }
    }
    catch( const IO_ERROR& ioe )
    {
        DisplayError( this, ioe.What() );
        return false;
    }

    msg = wxString::Format(
                    _( "Footprint library \"%s\" saved as \"%s\"." ),
                    GetChars( curLibPath ), GetChars( dstLibPath ) );

    DisplayInfoMessage( this, msg );

    SetStatusText( wxEmptyString );
    return true;
}
