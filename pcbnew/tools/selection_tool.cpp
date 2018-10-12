/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2013-2017 CERN
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 * Copyright (C) 2018 KiCad Developers, see AUTHORS.txt for contributors.
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
#include <limits>

#include <functional>
using namespace std::placeholders;

#include <class_board.h>
#include <class_board_item.h>
#include <class_track.h>
#include <class_module.h>
#include <class_pcb_text.h>
#include <class_drawsegment.h>
#include <class_zone.h>

#include <pcb_edit_frame.h>
#include <collectors.h>
#include <confirm.h>
#include <dialog_find.h>
#include <dialog_block_options.h>

#include <class_draw_panel_gal.h>
#include <view/view_controls.h>
#include <view/view_group.h>
#include <preview_items/selection_area.h>
#include <painter.h>
#include <bitmaps.h>
#include <hotkeys.h>

#include <tool/tool_event.h>
#include <tool/tool_manager.h>
#include <router/router_tool.h>
#include <connectivity_data.h>
#include <footprint_viewer_frame.h>
#include "tool_event_utils.h"

#include "selection_tool.h"
#include "pcb_bright_box.h"
#include "pcb_actions.h"

#include "kicad_plugin.h"

// Selection tool actions
TOOL_ACTION PCB_ACTIONS::selectionActivate( "pcbnew.InteractiveSelection",
        AS_GLOBAL, 0,
        "", "", NULL, AF_ACTIVATE ); // No description, it is not supposed to be shown anywhere

TOOL_ACTION PCB_ACTIONS::selectionCursor( "pcbnew.InteractiveSelection.Cursor",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION PCB_ACTIONS::selectItem( "pcbnew.InteractiveSelection.SelectItem",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION PCB_ACTIONS::selectItems( "pcbnew.InteractiveSelection.SelectItems",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION PCB_ACTIONS::unselectItem( "pcbnew.InteractiveSelection.UnselectItem",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION PCB_ACTIONS::unselectItems( "pcbnew.InteractiveSelection.UnselectItems",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION PCB_ACTIONS::selectionClear( "pcbnew.InteractiveSelection.Clear",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION PCB_ACTIONS::selectConnection( "pcbnew.InteractiveSelection.SelectConnection",
        AS_GLOBAL, TOOL_ACTION::LegacyHotKey( HK_SEL_TRIVIAL_CONNECTION ),
        _( "Trivial Connection" ), _( "Selects a connection between two junctions." ), add_tracks_xpm );

TOOL_ACTION PCB_ACTIONS::selectCopper( "pcbnew.InteractiveSelection.SelectCopper",
        AS_GLOBAL, TOOL_ACTION::LegacyHotKey( HK_SEL_COPPER_CONNECTION ),
        _( "Copper Connection" ), _( "Selects whole copper connection." ), net_highlight_xpm );

TOOL_ACTION PCB_ACTIONS::expandSelectedConnection( "pcbnew.InteractiveSelection.ExpandConnection",
        AS_GLOBAL, 0,
        _( "Expand Selected Connection" ),
        _( "Expands the current selection to select a connection between two junctions." ) );

TOOL_ACTION PCB_ACTIONS::selectNet( "pcbnew.InteractiveSelection.SelectNet",
        AS_GLOBAL, 0,
        _( "Whole Net" ), _( "Selects all tracks & vias belonging to the same net." ), mode_track_xpm );

TOOL_ACTION PCB_ACTIONS::selectOnSheetFromEeschema( "pcbnew.InteractiveSelection.SelectOnSheet",
        AS_GLOBAL,  0,
        _( "Sheet" ), _( "Selects all modules and tracks in the schematic sheet" ), select_same_sheet_xpm );

TOOL_ACTION PCB_ACTIONS::selectSameSheet( "pcbnew.InteractiveSelection.SelectSameSheet",
        AS_GLOBAL,  0,
        _( "Items in Same Hierarchical Sheet" ),
        _( "Selects all modules and tracks in the same schematic sheet" ), select_same_sheet_xpm );

TOOL_ACTION PCB_ACTIONS::find( "pcbnew.InteractiveSelection.Find",
        AS_GLOBAL, 0, //TOOL_ACTION::LegacyHotKey( HK_FIND_ITEM ), // handled by wxWidgets
        _( "Find Item..." ),_( "Searches the document for an item" ), find_xpm );

TOOL_ACTION PCB_ACTIONS::findMove( "pcbnew.InteractiveSelection.FindMove",
        AS_GLOBAL, TOOL_ACTION::LegacyHotKey( HK_GET_AND_MOVE_FOOTPRINT ),
        _( "Get and Move Footprint" ),
        _( "Selects a footprint by reference and places it under the cursor for moving"),
        move_xpm );

TOOL_ACTION PCB_ACTIONS::filterSelection( "pcbnew.InteractiveSelection.FilterSelection",
        AS_GLOBAL, 0,
        _( "Filter Selection..." ), _( "Filter the types of items in the selection" ),
        options_generic_xpm );

class SELECT_MENU: public CONTEXT_MENU
{
public:
    SELECT_MENU()
    {
        SetTitle( _( "Select" ) );
        SetIcon( options_generic_xpm );

        Add( PCB_ACTIONS::filterSelection );

        AppendSeparator();

        Add( PCB_ACTIONS::selectConnection );
        Add( PCB_ACTIONS::selectCopper );
        Add( PCB_ACTIONS::selectNet );
        Add( PCB_ACTIONS::selectSameSheet );
    }

private:

    void update() override
    {
        using S_C = SELECTION_CONDITIONS;

        const auto& selection = getToolManager()->GetTool<SELECTION_TOOL>()->GetSelection();

        bool connItem = S_C::OnlyTypes( GENERAL_COLLECTOR::Tracks )( selection );
        bool sheetSelEnabled = ( S_C::OnlyType( PCB_MODULE_T ) )( selection );

        Enable( getMenuId( PCB_ACTIONS::selectNet ), connItem );
        Enable( getMenuId( PCB_ACTIONS::selectCopper ), connItem );
        Enable( getMenuId( PCB_ACTIONS::selectConnection ), connItem );
        Enable( getMenuId( PCB_ACTIONS::selectSameSheet ), sheetSelEnabled );
    }

    CONTEXT_MENU* create() const override
    {
        return new SELECT_MENU();
    }
};


/**
 * Private implementation of firewalled private data
 */
class SELECTION_TOOL::PRIV
{
public:
    DIALOG_BLOCK_OPTIONS::OPTIONS m_filterOpts;
};


SELECTION_TOOL::SELECTION_TOOL() :
        PCB_TOOL( "pcbnew.InteractiveSelection" ),
        m_frame( NULL ),
        m_additive( false ),
        m_subtractive( false ),
        m_multiple( false ),
        m_skip_heuristics( false ),
        m_locked( true ),
        m_menu( *this ),
        m_priv( std::make_unique<PRIV>() )
{

}


SELECTION_TOOL::~SELECTION_TOOL()
{
    getView()->Remove( &m_selection );
}


bool SELECTION_TOOL::Init()
{
    auto frame = getEditFrame<PCB_BASE_FRAME>();

    if( frame && ( frame->IsType( FRAME_PCB_MODULE_VIEWER )
                   || frame->IsType( FRAME_PCB_MODULE_VIEWER_MODAL ) ) )
    {
        m_menu.AddStandardSubMenus( *frame );
        return true;
    }

    auto selectMenu = std::make_shared<SELECT_MENU>();
    selectMenu->SetTool( this );
    m_menu.AddSubMenu( selectMenu );

    auto& menu = m_menu.GetMenu();

    menu.AddMenu( selectMenu.get(), false, SELECTION_CONDITIONS::NotEmpty );
    menu.AddSeparator( SELECTION_CONDITIONS::NotEmpty, 1000 );

    if( frame )
    {
        m_menu.AddStandardSubMenus( *frame );
    }

    return true;
}


void SELECTION_TOOL::Reset( RESET_REASON aReason )
{
    m_frame = getEditFrame<PCB_BASE_FRAME>();
    m_locked = true;

    if( aReason == TOOL_BASE::MODEL_RELOAD )
    {
        // Remove pointers to the selected items from containers
        // without changing their properties (as they are already deleted
        // while a new board is loaded)
        m_selection.Clear();
        getView()->GetPainter()->GetSettings()->SetHighlight( false );
    }
    else
        // Restore previous properties of selected items and remove them from containers
        clearSelection();

    // Reinsert the VIEW_GROUP, in case it was removed from the VIEW
    view()->Remove( &m_selection );
    view()->Add( &m_selection );
}


int SELECTION_TOOL::Main( const TOOL_EVENT& aEvent )
{
    // Main loop: keep receiving events
    while( OPT_TOOL_EVENT evt = Wait() )
    {
        // Should selected items be added to the current selection or
        // become the new selection (discarding previously selected items)
        m_additive = evt->Modifier( MD_SHIFT );

        // Should selected items be REMOVED from the current selection?
        // This will be ignored if the SHIFT modifier is pressed
        m_subtractive = !m_additive && evt->Modifier( MD_CTRL );

        // Is the user requesting that the selection list include all possible
        // items without removing less likely selection candidates
        m_skip_heuristics = !!evt->Modifier( MD_ALT );

        // Single click? Select single object
        if( evt->IsClick( BUT_LEFT ) )
        {
            if( evt->Modifier( MD_CTRL ) && !m_editModules )
            {
                m_toolMgr->RunAction( PCB_ACTIONS::highlightNet, true );
            }
            else
            {
                CLIENT_SELECTION_FILTER filter = nullptr;

                // If no modifier keys are pressed, clear the selection
                if( !m_additive )
                {
                    if( m_selection.Size() != 0 )
                        filter = []( const VECTOR2I&, GENERAL_COLLECTOR& aCollector )
                        {
                            for( int i = aCollector.GetCount() - 1; i >= 0; i-- )
                                if( aCollector[i]->Type() == PCB_ZONE_AREA_T )
                                    aCollector.Remove( i );
                        };

                    clearSelection();
                }

                selectPoint( evt->Position(), false, nullptr, filter );

            }
        }

        // right click? if there is any object - show the context menu
        else if( evt->IsClick( BUT_RIGHT ) )
        {
            bool selectionCancelled = false;

            if( m_selection.Empty() )
            {
                selectPoint( evt->Position(), false, &selectionCancelled );
                m_selection.SetIsHover( true );
            }

            if( !selectionCancelled )
                m_menu.ShowContextMenu( m_selection );
        }

        // double click? Display the properties window
        else if( evt->IsDblClick( BUT_LEFT ) )
        {
            if( m_selection.Empty() )
                selectPoint( evt->Position() );

            m_toolMgr->RunAction( PCB_ACTIONS::properties );
        }

        // drag with LMB? Select multiple objects (or at least draw a selection box) or drag them
        else if( evt->IsDrag( BUT_LEFT ) )
        {
            if( m_additive || m_subtractive )
            {
                selectMultiple();
            }
            else if( m_selection.Empty() )
            {
                // There is nothing selected, so try to select something
                if( getEditFrame<PCB_BASE_FRAME>()->Settings().m_dragSelects || !selectCursor() )
                {
                    // If nothings has been selected, user wants to select more or selection
                    // box is preferred to dragging - draw selection box
                    selectMultiple();
                }
                else
                {
                    m_selection.SetIsHover( true );
                    m_toolMgr->InvokeTool( "pcbnew.InteractiveEdit" );
                }
            }

            else
            {
                // Check if dragging has started within any of selected items bounding box
                if( selectionContains( evt->Position() ) )
                {
                    // Yes -> run the move tool and wait till it finishes
                    m_toolMgr->InvokeTool( "pcbnew.InteractiveEdit" );
                }
                else
                {
                    // No -> clear the selection list
                    clearSelection();
                }
            }
        }

        else if( evt->IsCancel() || evt->Action() == TA_UNDO_REDO_PRE )
        {
            clearSelection();

            if( evt->IsCancel() && !m_editModules )
                m_toolMgr->RunAction( PCB_ACTIONS::clearHighlight, true );
        }

        else if( evt->Action() == TA_CONTEXT_MENU_CLOSED )
        {
            m_menu.CloseContextMenu( evt );
        }
    }

    // This tool is supposed to be active forever
    assert( false );

    return 0;
}


SELECTION& SELECTION_TOOL::GetSelection()
{
    return m_selection;
}


SELECTION& SELECTION_TOOL::RequestSelection( int aFlags, CLIENT_SELECTION_FILTER aClientFilter )
{
    std::vector<EDA_ITEM*> removed_items;
    bool selectionEmpty = m_selection.Empty();
    m_selection.SetIsHover( selectionEmpty );

    if( selectionEmpty )
    {
        if( aFlags & SELECTION_FORCE_UNLOCK )
            m_locked = false;

        m_toolMgr->RunAction( PCB_ACTIONS::selectionCursor, true, aClientFilter );
        m_selection.ClearReferencePoint();
    }

    // Be careful with iterators: items can be removed from list that invalidate iterators.
    for( auto item : m_selection )
    {
        if( ( aFlags & SELECTION_EDITABLE ) && item->Type() == PCB_MARKER_T )
            removed_items.push_back( item );
    }

    // Now safely remove the items from the selection
    for( auto item : removed_items )
        unselect( static_cast<BOARD_ITEM *>( item ) );

    if( aFlags & SELECTION_SANITIZE_PADS )
        SanitizeSelection();

    return m_selection;
}


void SELECTION_TOOL::toggleSelection( BOARD_ITEM* aItem )
{
    if( aItem->IsSelected() )
    {
        unselect( aItem );

        // Inform other potentially interested tools
        m_toolMgr->ProcessEvent( UnselectedEvent );
    }
    else
    {
        if( !m_additive )
            clearSelection();

        // Prevent selection of invisible or inactive items
        if( selectable( aItem ) )
        {
            select( aItem );

            // Inform other potentially interested tools
            m_toolMgr->ProcessEvent( SelectedEvent );
        }
    }

    if( m_frame )
    {
        m_frame->GetGalCanvas()->ForceRefresh();
    }
}

const GENERAL_COLLECTORS_GUIDE SELECTION_TOOL::getCollectorsGuide() const
{
    GENERAL_COLLECTORS_GUIDE guide( board()->GetVisibleLayers(),
                                    (PCB_LAYER_ID) view()->GetTopLayer() );

    // account for the globals
    guide.SetIgnoreMTextsMarkedNoShow( ! board()->IsElementVisible( LAYER_MOD_TEXT_INVISIBLE ) );
    guide.SetIgnoreMTextsOnBack( ! board()->IsElementVisible( LAYER_MOD_TEXT_BK ) );
    guide.SetIgnoreMTextsOnFront( ! board()->IsElementVisible( LAYER_MOD_TEXT_FR ) );
    guide.SetIgnoreModulesOnBack( ! board()->IsElementVisible( LAYER_MOD_BK ) );
    guide.SetIgnoreModulesOnFront( ! board()->IsElementVisible( LAYER_MOD_FR ) );
    guide.SetIgnorePadsOnBack( ! board()->IsElementVisible( LAYER_PAD_BK ) );
    guide.SetIgnorePadsOnFront( ! board()->IsElementVisible( LAYER_PAD_FR ) );
    guide.SetIgnoreModulesVals( ! board()->IsElementVisible( LAYER_MOD_VALUES ) );
    guide.SetIgnoreModulesRefs( ! board()->IsElementVisible( LAYER_MOD_REFERENCES ) );
    guide.SetIgnoreThroughVias( ! board()->IsElementVisible( LAYER_VIA_THROUGH ) );
    guide.SetIgnoreBlindBuriedVias( ! board()->IsElementVisible( LAYER_VIA_BBLIND ) );
    guide.SetIgnoreMicroVias( ! board()->IsElementVisible( LAYER_VIA_MICROVIA ) );
    guide.SetIgnoreTracks( ! board()->IsElementVisible( LAYER_TRACKS ) );

    return guide;
}


bool SELECTION_TOOL::selectPoint( const VECTOR2I& aWhere, bool aOnDrag,
                                  bool* aSelectionCancelledFlag,
                                  CLIENT_SELECTION_FILTER aClientFilter )
{
    auto guide = getCollectorsGuide();
    GENERAL_COLLECTOR collector;

    collector.Collect( board(),
        m_editModules ? GENERAL_COLLECTOR::ModuleItems : GENERAL_COLLECTOR::AllBoardItems,
        wxPoint( aWhere.x, aWhere.y ), guide );

    bool anyCollected = collector.GetCount() != 0;

    // Remove unselectable items
    for( int i = collector.GetCount() - 1; i >= 0; --i )
    {
        if( !selectable( collector[i] ) || ( aOnDrag && collector[i]->IsLocked() ) )
            collector.Remove( i );
    }

    m_selection.ClearReferencePoint();

    // Allow the client to do tool- or action-specific filtering to see if we
    // can get down to a single item
    if( aClientFilter )
        aClientFilter( aWhere, collector );

    if( collector.GetCount() == 0 )
    {
        if( !m_additive && anyCollected )
        {
            clearSelection();
        }
        return false;
    }

    if( collector.GetCount() == 1 )
    {
        toggleSelection( collector[0] );
        return true;
    }

    // Apply some ugly heuristics to avoid disambiguation menus whenever possible
    if( !m_skip_heuristics )
    {
        guessSelectionCandidates( collector, aWhere );

        if( collector.GetCount() == 1 )
        {
            toggleSelection( collector[0] );
            return true;
        }
    }

    // Still more than one item.  We're going to have to ask the user.
    if( aOnDrag )
    {
        Wait( TOOL_EVENT( TC_ANY, TA_MOUSE_UP, BUT_LEFT ) );
    }

    BOARD_ITEM* item = disambiguationMenu( &collector );

    if( item )
    {
        toggleSelection( item );
        return true;
    }
    else
    {
        if( aSelectionCancelledFlag )
            *aSelectionCancelledFlag = true;

        return false;
    }

    return false;
}


bool SELECTION_TOOL::selectCursor( bool aSelectAlways, CLIENT_SELECTION_FILTER aClientFilter )
{
    if( aSelectAlways || m_selection.Empty() )
    {
        clearSelection();
        selectPoint( getViewControls()->GetCursorPosition( false ), false, NULL, aClientFilter );
    }

    return !m_selection.Empty();
}


bool SELECTION_TOOL::selectMultiple()
{
    bool cancelled = false;     // Was the tool cancelled while it was running?
    m_multiple = true;          // Multiple selection mode is active
    KIGFX::VIEW* view = getView();

    KIGFX::PREVIEW::SELECTION_AREA area;
    view->Add( &area );

    while( OPT_TOOL_EVENT evt = Wait() )
    {
        if( TOOL_EVT_UTILS::IsCancelInteractive( *evt ) )
        {
            cancelled = true;
            break;
        }

        if( evt->IsDrag( BUT_LEFT ) )
        {
            // Start drawing a selection box
            area.SetOrigin( evt->DragOrigin() );
            area.SetEnd( evt->Position() );
            area.SetAdditive( m_additive );
            area.SetSubtractive( m_subtractive );

            view->SetVisible( &area, true );
            view->Update( &area );
            getViewControls()->SetAutoPan( true );
        }

        if( evt->IsMouseUp( BUT_LEFT ) )
        {
            getViewControls()->SetAutoPan( false );

            // End drawing the selection box
            view->SetVisible( &area, false );

            // Mark items within the selection box as selected
            std::vector<KIGFX::VIEW::LAYER_ITEM_PAIR> selectedItems;

            // Filter the view items based on the selection box
            BOX2I selectionBox = area.ViewBBox();
            view->Query( selectionBox, selectedItems );         // Get the list of selected items

            std::vector<KIGFX::VIEW::LAYER_ITEM_PAIR>::iterator it, it_end;

            int width = area.GetEnd().x - area.GetOrigin().x;
            int height = area.GetEnd().y - area.GetOrigin().y;

            /* Selection mode depends on direction of drag-selection:
             * Left > Right : Select objects that are fully enclosed by selection
             * Right > Left : Select objects that are crossed by selection
             */
            bool windowSelection = width >= 0 ? true : false;

            if( view->IsMirroredX() )
                windowSelection = !windowSelection;

            // Construct an EDA_RECT to determine BOARD_ITEM selection
            EDA_RECT selectionRect( wxPoint( area.GetOrigin().x, area.GetOrigin().y ),
                                    wxSize( width, height ) );

            selectionRect.Normalize();

            for( it = selectedItems.begin(), it_end = selectedItems.end(); it != it_end; ++it )
            {
                BOARD_ITEM* item = static_cast<BOARD_ITEM*>( it->first );

                if( !item || !selectable( item ) )
                    continue;

                if( windowSelection )
                {
                    BOX2I bbox( item->GetBoundingBox() );

                    if( selectionBox.Contains( bbox ) )
                    {
                        if( m_subtractive )
                            unselect( item );
                        else
                            select( item );
                    }
                }
                else
                {
                    if( item->HitTest( selectionRect, false ) )
                    {
                        if( m_subtractive )
                            unselect( item );
                        else
                            select( item );
                    }
                }
            }

            if( m_frame )
            {
                if( m_selection.Size() == 1 )
                    m_frame->SetCurItem( static_cast<BOARD_ITEM*>( m_selection.Front() ) );
                else
                    m_frame->SetCurItem( NULL );
            }

            // Inform other potentially interested tools
            if( !m_selection.Empty() )
                m_toolMgr->ProcessEvent( SelectedEvent );

            break;  // Stop waiting for events
        }
    }

    getViewControls()->SetAutoPan( false );

    // Stop drawing the selection box
    view->Remove( &area );
    m_multiple = false;         // Multiple selection mode is inactive

    if( !cancelled )
        m_selection.ClearReferencePoint();

    return cancelled;
}


void SELECTION_TOOL::setTransitions()
{
    Go( &SELECTION_TOOL::Main, PCB_ACTIONS::selectionActivate.MakeEvent() );
    Go( &SELECTION_TOOL::CursorSelection, PCB_ACTIONS::selectionCursor.MakeEvent() );
    Go( &SELECTION_TOOL::ClearSelection, PCB_ACTIONS::selectionClear.MakeEvent() );
    Go( &SELECTION_TOOL::SelectItem, PCB_ACTIONS::selectItem.MakeEvent() );
    Go( &SELECTION_TOOL::SelectItems, PCB_ACTIONS::selectItems.MakeEvent() );
    Go( &SELECTION_TOOL::UnselectItem, PCB_ACTIONS::unselectItem.MakeEvent() );
    Go( &SELECTION_TOOL::UnselectItems, PCB_ACTIONS::unselectItems.MakeEvent() );
    Go( &SELECTION_TOOL::find, PCB_ACTIONS::find.MakeEvent() );
    Go( &SELECTION_TOOL::findMove, PCB_ACTIONS::findMove.MakeEvent() );
    Go( &SELECTION_TOOL::filterSelection, PCB_ACTIONS::filterSelection.MakeEvent() );
    Go( &SELECTION_TOOL::selectConnection, PCB_ACTIONS::selectConnection.MakeEvent() );
    Go( &SELECTION_TOOL::expandSelectedConnection, PCB_ACTIONS::expandSelectedConnection.MakeEvent() );
    Go( &SELECTION_TOOL::selectCopper, PCB_ACTIONS::selectCopper.MakeEvent() );
    Go( &SELECTION_TOOL::selectNet, PCB_ACTIONS::selectNet.MakeEvent() );
    Go( &SELECTION_TOOL::selectSameSheet, PCB_ACTIONS::selectSameSheet.MakeEvent() );
    Go( &SELECTION_TOOL::selectOnSheetFromEeschema, PCB_ACTIONS::selectOnSheetFromEeschema.MakeEvent() );
    Go( &SELECTION_TOOL::updateSelection, PCB_ACTIONS::selectionModified.MakeEvent() );
}


SELECTION_LOCK_FLAGS SELECTION_TOOL::CheckLock()
{
    if( !m_locked || m_editModules )
        return SELECTION_UNLOCKED;

    bool containsLocked = false;

    // Check if the selection contains locked items
    for( const auto& item : m_selection )
    {
        switch( item->Type() )
        {
        case PCB_MODULE_T:
            if( static_cast<MODULE*>( item )->IsLocked() )
                containsLocked = true;
            break;

        case PCB_MODULE_EDGE_T:
        case PCB_MODULE_TEXT_T:
            if( static_cast<MODULE*>( item->GetParent() )->IsLocked() )
                containsLocked = true;
            break;

        default:    // suppress warnings
            break;
        }
    }

    if( containsLocked )
    {
        if( IsOK( m_frame, _( "Selection contains locked items. Do you want to continue?" ) ) )
        {
            m_locked = false;
            return SELECTION_LOCK_OVERRIDE;
        }
        else
            return SELECTION_LOCKED;
    }

    return SELECTION_UNLOCKED;
}


int SELECTION_TOOL::CursorSelection( const TOOL_EVENT& aEvent )
{
    CLIENT_SELECTION_FILTER aClientFilter = aEvent.Parameter<CLIENT_SELECTION_FILTER>();

    if( m_selection.Empty() )                        // Try to find an item that could be modified
    {
        selectCursor( true, aClientFilter );

        if( CheckLock() == SELECTION_LOCKED )
        {
            clearSelection();
            return 0;
        }
    }

    return 0;
}


int SELECTION_TOOL::ClearSelection( const TOOL_EVENT& aEvent )
{
    clearSelection();

    return 0;
}


int SELECTION_TOOL::SelectItems( const TOOL_EVENT& aEvent )
{
    std::vector<BOARD_ITEM*>* items = aEvent.Parameter<std::vector<BOARD_ITEM*>*>();

    if( items )
    {
        // Perform individual selection of each item
        // before processing the event.
        for( auto item : *items )
        {
            select( item );
        }

        m_toolMgr->ProcessEvent( SelectedEvent );
    }

    return 0;
}


int SELECTION_TOOL::SelectItem( const TOOL_EVENT& aEvent )
{
    // Check if there is an item to be selected
    BOARD_ITEM* item = aEvent.Parameter<BOARD_ITEM*>();

    if( item )
    {
        select( item );

        // Inform other potentially interested tools
        m_toolMgr->ProcessEvent( SelectedEvent );
    }

    return 0;
}


int SELECTION_TOOL::UnselectItems( const TOOL_EVENT& aEvent )
{
    std::vector<BOARD_ITEM*>* items = aEvent.Parameter<std::vector<BOARD_ITEM*>*>();

    if( items )
    {
        // Perform individual unselection of each item
        // before processing the event
        for( auto item : *items )
        {
            unselect( item );
        }

        m_toolMgr->ProcessEvent( UnselectedEvent );
    }

    return 0;
}


int SELECTION_TOOL::UnselectItem( const TOOL_EVENT& aEvent )
{
    // Check if there is an item to be selected
    BOARD_ITEM* item = aEvent.Parameter<BOARD_ITEM*>();

    if( item )
    {
        unselect( item );

        // Inform other potentially interested tools
        m_toolMgr->ProcessEvent( UnselectedEvent );
    }

    return 0;
}


void connectedTrackFilter( const VECTOR2I& aPt, GENERAL_COLLECTOR& aCollector )
{
    /* Narrow the collection down to a single TRACK item for a trivial
     * connection, or multiple TRACK items for non-trivial connections.
     */
    for( int i = aCollector.GetCount() - 1; i >= 0; i-- )
    {
        if( !dynamic_cast<TRACK*>( aCollector[i] ) )
            aCollector.Remove( i );
    }

    ROUTER_TOOL::NeighboringSegmentFilter( aPt, aCollector );
}


int SELECTION_TOOL::selectConnection( const TOOL_EVENT& aEvent )
{
    if( !m_selection.HasType( PCB_TRACE_T ) && !m_selection.HasType( PCB_VIA_T ) )
        selectCursor( true, connectedTrackFilter );

    if( !m_selection.HasType( PCB_TRACE_T ) && !m_selection.HasType( PCB_VIA_T ) )
        return 0;

    return expandSelectedConnection( aEvent );
}


int SELECTION_TOOL::expandSelectedConnection( const TOOL_EVENT& aEvent )
{
    // copy the selection, since we're going to iterate and modify
    auto selection = m_selection.GetItems();

    // We use the BUSY flag to mark connections
    for( auto item : selection )
        item->SetState( BUSY, false );

    for( auto item : selection )
    {
        TRACK* trackItem = dynamic_cast<TRACK*>( item );

        // Track items marked BUSY have already been visited
        //  therefore their connections have already been marked
        if( trackItem && !trackItem->GetState( BUSY ) )
            selectAllItemsConnectedToTrack( *trackItem );
    }

    // Inform other potentially interested tools
    if( m_selection.Size() > 0 )
        m_toolMgr->ProcessEvent( SelectedEvent );

    return 0;
}


void connectedItemFilter( const VECTOR2I&, GENERAL_COLLECTOR& aCollector )
{
    /* Narrow the collection down to a single BOARD_CONNECTED_ITEM for each
     * represented net.  All other items types are removed.
     */
    std::set<int> representedNets;

    for( int i = aCollector.GetCount() - 1; i >= 0; i-- )
    {
        BOARD_CONNECTED_ITEM* item = dynamic_cast<BOARD_CONNECTED_ITEM*>( aCollector[i] );
        if( !item )
            aCollector.Remove( i );
        else if ( representedNets.count( item->GetNetCode() ) )
            aCollector.Remove( i );
        else
            representedNets.insert( item->GetNetCode() );
    }
}


int SELECTION_TOOL::selectCopper( const TOOL_EVENT& aEvent )
{
    bool haveCopper = false;

    for( auto item : m_selection.GetItems() )
    {
        if( dynamic_cast<BOARD_CONNECTED_ITEM*>( item ) )
            haveCopper = true;;
    }

    if( !haveCopper )
        selectCursor( true, connectedItemFilter );

    // copy the selection, since we're going to iterate and modify
    auto selection  = m_selection.GetItems();

    for( auto item : selection )
    {
        BOARD_CONNECTED_ITEM* connItem = dynamic_cast<BOARD_CONNECTED_ITEM*>( item );

        if( connItem )
            selectAllItemsConnectedToItem( *connItem );
    }

    // Inform other potentially interested tools
    if( m_selection.Size() > 0 )
        m_toolMgr->ProcessEvent( SelectedEvent );

    return 0;
}


void SELECTION_TOOL::selectAllItemsConnectedToTrack( TRACK& aSourceTrack )
{
    int segmentCount;
    TRACK* trackList = board()->MarkTrace( board()->m_Track, &aSourceTrack, &segmentCount,
                                           nullptr, nullptr, true );

    for( int i = 0; i < segmentCount; ++i )
    {
        select( trackList );
        trackList = trackList->Next();
    }
}


void SELECTION_TOOL::selectAllItemsConnectedToItem( BOARD_CONNECTED_ITEM& aSourceItem )
{
    constexpr KICAD_T types[] = { PCB_TRACE_T, PCB_VIA_T, PCB_PAD_T, EOT };
    auto connectivity = board()->GetConnectivity();

    for( auto item : connectivity->GetConnectedItems( &aSourceItem, types ) )
    {
        // We want to select items connected through pads but not pads
        // otherwise, the common use case of "Select Copper"->Delete will
        // remove footprints in addition to traces and vias
        if( item->Type() != PCB_PAD_T )
            select( item );
    }
}


void SELECTION_TOOL::selectAllItemsOnNet( int aNetCode )
{
    constexpr KICAD_T types[] = { PCB_TRACE_T, PCB_VIA_T, EOT };
    auto connectivity = board()->GetConnectivity();

    for( auto item : connectivity->GetNetItems( aNetCode, types ) )
        select( item );
}


int SELECTION_TOOL::selectNet( const TOOL_EVENT& aEvent )
{
    if( !selectCursor() )
        return 0;

    // copy the selection, since we're going to iterate and modify
    auto selection = m_selection.GetItems();

    for( auto i : selection )
    {
        auto item = static_cast<BOARD_ITEM*>( i );

        // only connected items get a net code
        if( item->IsConnected() )
        {
            auto& connItem = static_cast<BOARD_CONNECTED_ITEM&>( *item );

            selectAllItemsOnNet( connItem.GetNetCode() );
        }
    }

    // Inform other potentially interested tools
    if( m_selection.Size() > 0 )
        m_toolMgr->ProcessEvent( SelectedEvent );

    return 0;
}


void SELECTION_TOOL::selectAllItemsOnSheet( wxString& aSheetpath )
{
    auto modules = board()->m_Modules.GetFirst();
    std::list<MODULE*> modList;

    // store all modules that are on that sheet
    for( MODULE* mitem = modules; mitem; mitem = mitem->Next() )
    {
        if( mitem != NULL && mitem->GetPath().Contains( aSheetpath ) )
        {
            modList.push_back( mitem );
        }
    }

    //Generate a list of all pads, and of all nets they belong to.
    std::list<int> netcodeList;
    std::list<BOARD_CONNECTED_ITEM*> padList;
    for( MODULE* mmod : modList )
    {
        for( auto pad : mmod->Pads() )
        {
            if( pad->IsConnected() )
            {
                netcodeList.push_back( pad->GetNetCode() );
                padList.push_back( pad );
            }
        }
    }
    // remove all duplicates
    netcodeList.sort();
    netcodeList.unique();

    // auto select trivial connections segments which are launched from the pads
    std::list<TRACK*> launchTracks;

    for( auto pad : padList )
    {
        launchTracks = board()->GetTracksByPosition( pad->GetPosition() );

        for( auto track : launchTracks )
        {
            selectAllItemsConnectedToTrack( *track );
        }
    }

    // now we need to find all modules that are connected to each of these nets
    // then we need to determine if these modules are in the list of modules
    // belonging to this sheet ( modList )
    std::list<int> removeCodeList;
    constexpr KICAD_T padType[] = { PCB_PAD_T, EOT };

    for( int netCode : netcodeList )
    {
        for( BOARD_CONNECTED_ITEM* mitem : board()->GetConnectivity()->GetNetItems( netCode, padType ) )
        {
            if( mitem->Type() == PCB_PAD_T)
            {
                bool found = ( std::find( modList.begin(), modList.end(),
                                    mitem->GetParent() ) != modList.end() );

                if( !found )
                {
                    // if we cannot find the module of the pad in the modList
                    // then we can assume that that module is not located in the same
                    // schematic, therefore invalidate this netcode.
                    removeCodeList.push_back( netCode );
                    break;
                }
            }
        }
    }

    // remove all duplicates
    removeCodeList.sort();
    removeCodeList.unique();

    for( int removeCode : removeCodeList )
    {
        netcodeList.remove( removeCode );
    }

    std::list<BOARD_CONNECTED_ITEM*> localConnectionList;
    constexpr KICAD_T trackViaType[] = { PCB_TRACE_T, PCB_VIA_T, EOT };

    for( int netCode : netcodeList )
    {
        for( BOARD_CONNECTED_ITEM* item : board()->GetConnectivity()->GetNetItems( netCode, trackViaType ) )
        {
            localConnectionList.push_back( item );
        }
    }

    for( BOARD_ITEM* i : modList )
    {
        if( i != NULL )
            select( i );
    }

    for( BOARD_CONNECTED_ITEM* i : localConnectionList )
    {
        if( i != NULL )
            select( i );
    }
}


void SELECTION_TOOL::zoomFitSelection( void )
{
    //Should recalculate the view to zoom in on the selection
    auto selectionBox = m_selection.ViewBBox();
    auto canvas = m_frame->GetGalCanvas();
    auto view = getView();

    VECTOR2D screenSize = view->ToWorld( canvas->GetClientSize(), false );

    if( !( selectionBox.GetWidth() == 0 ) || !( selectionBox.GetHeight() == 0 ) )
    {
        VECTOR2D vsize = selectionBox.GetSize();
        double scale = view->GetScale() / std::max( fabs( vsize.x / screenSize.x ),
                fabs( vsize.y / screenSize.y ) );
        view->SetScale( scale );
        view->SetCenter( selectionBox.Centre() );
        view->Add( &m_selection );
    }

    m_frame->GetGalCanvas()->ForceRefresh();
}


int SELECTION_TOOL::selectOnSheetFromEeschema( const TOOL_EVENT& aEvent )
{
    clearSelection();
    wxString* sheetpath = aEvent.Parameter<wxString*>();

    selectAllItemsOnSheet( *sheetpath );

    zoomFitSelection();

    if( m_selection.Size() > 0 )
        m_toolMgr->ProcessEvent( SelectedEvent );

    return 0;
}


int SELECTION_TOOL::selectSameSheet( const TOOL_EVENT& aEvent )
{
    if( !selectCursor( true ) )
        return 0;

    // this function currently only supports modules since they are only
    // on one sheet.
    auto item = m_selection.Front();

    if( !item )
        return 0;

    if( item->Type() != PCB_MODULE_T )
        return 0;

    auto mod = dynamic_cast<MODULE*>( item );

    clearSelection();

    // get the lowest subsheet name for this.
    wxString sheetPath = mod->GetPath();
    sheetPath = sheetPath.BeforeLast( '/' );
    sheetPath = sheetPath.AfterLast( '/' );

    selectAllItemsOnSheet( sheetPath );

    // Inform other potentially interested tools
    if( m_selection.Size() > 0 )
        m_toolMgr->ProcessEvent( SelectedEvent );

    return 0;
}


void SELECTION_TOOL::findCallback( BOARD_ITEM* aItem )
{
    clearSelection();

    if( aItem )
    {
        select( aItem );
        getView()->SetCenter( aItem->GetPosition() );

        // Inform other potentially interested tools
        m_toolMgr->ProcessEvent( SelectedEvent );
    }

    m_frame->GetGalCanvas()->ForceRefresh();
}


int SELECTION_TOOL::find( const TOOL_EVENT& aEvent )
{
    DIALOG_FIND dlg( m_frame );
    dlg.SetCallback( std::bind( &SELECTION_TOOL::findCallback, this, _1 ) );
    dlg.ShowModal();

    return 0;
}


int SELECTION_TOOL::findMove( const TOOL_EVENT& aEvent )
{
    MODULE* module = m_frame->GetFootprintFromBoardByReference();

    if( module )
    {
        KIGFX::VIEW_CONTROLS* viewCtrls = getViewControls();
        clearSelection();
        toggleSelection( module );

        auto cursorPosition = viewCtrls->GetCursorPosition( false );

        // Set a reference point so InteractiveEdit will move it to the
        // cursor before waiting for mouse move events
        m_selection.SetReferencePoint( module->GetPosition() );

        // Place event on module origin first, so the generic anchor snap
        // doesn't just choose the closest pin for us
        viewCtrls->ForceCursorPosition( true, module->GetPosition() );

        // pick the component up and start moving
        m_toolMgr->InvokeTool( "pcbnew.InteractiveEdit" );

        // restore the previous cursor position
        viewCtrls->SetCursorPosition( cursorPosition, false );
    }

    return 0;
}


/**
 * Function itemIsIncludedByFilter()
 *
 * Determine if an item is included by the filter specified
 *
 * @return true if the parameter indicate the items should be selected
 * by this filter (i..e not filtered out)
 */
static bool itemIsIncludedByFilter( const BOARD_ITEM& aItem,
                                    const BOARD& aBoard,
                                    const DIALOG_BLOCK_OPTIONS::OPTIONS& aBlockOpts )
{
    bool include = true;
    const PCB_LAYER_ID layer = aItem.GetLayer();

    // can skip without even checking item type
    // fixme: selecting items on invisible layers does not work in GAL
    if( !aBlockOpts.includeItemsOnInvisibleLayers
        && !aBoard.IsLayerVisible( layer ) )
    {
        include = false;
    }

    // if the item needs to be checked against the options
    if( include )
    {
        switch( aItem.Type() )
        {
        case PCB_MODULE_T:
        {
            const auto& module = static_cast<const MODULE&>( aItem );

            include = aBlockOpts.includeModules;

            if( include && !aBlockOpts.includeLockedModules )
            {
                include = !module.IsLocked();
            }

            break;
        }
        case PCB_TRACE_T:
        {
            include = aBlockOpts.includeTracks;
            break;
        }
        case PCB_VIA_T:
        {
            include = aBlockOpts.includeVias;
            break;
        }
        case PCB_ZONE_AREA_T:
        {
            include = aBlockOpts.includeZones;
            break;
        }
        case PCB_LINE_T:
        case PCB_TARGET_T:
        case PCB_DIMENSION_T:
        {
            if( layer == Edge_Cuts )
                include = aBlockOpts.includeBoardOutlineLayer;
            else
                include = aBlockOpts.includeItemsOnTechLayers;
            break;
        }
        case PCB_TEXT_T:
        {
            include = aBlockOpts.includePcbTexts;
            break;
        }
        default:
        {
            // no filtering, just select it
            break;
        }
        }
    }

    return include;
}


int SELECTION_TOOL::filterSelection( const TOOL_EVENT& aEvent )
{
    auto& opts = m_priv->m_filterOpts;
    DIALOG_BLOCK_OPTIONS dlg( m_frame, opts, false, _( "Filter selection" ) );

    const int cmd = dlg.ShowModal();

    if( cmd != wxID_OK )
        return 0;

    const auto& board = *getModel<BOARD>();

    // copy current selection
    auto selection = m_selection.GetItems();

    // clear current selection
    clearSelection();

    // copy selection items from the saved selection
    // according to the dialog options
    for( auto i : selection )
    {
        auto item = static_cast<BOARD_ITEM*>( i );
        bool include = itemIsIncludedByFilter( *item, board, opts );

        if( include )
        {
            select( item );
        }
    }
    return 0;
}


void SELECTION_TOOL::clearSelection()
{
    if( m_selection.Empty() )
        return;

    for( auto item : m_selection )
        unselectVisually( static_cast<BOARD_ITEM*>( item ) );

    m_selection.Clear();
    m_selection.SetIsHover( false );
    m_selection.ClearReferencePoint();

    if( m_frame )
    {
        m_frame->SetCurItem( NULL );
    }

    m_locked = true;

    // Inform other potentially interested tools
    m_toolMgr->ProcessEvent( ClearedEvent );
}


BOARD_ITEM* SELECTION_TOOL::disambiguationMenu( GENERAL_COLLECTOR* aCollector )
{
    BOARD_ITEM* current = NULL;
    KIGFX::VIEW_GROUP highlightGroup;
    CONTEXT_MENU menu;

    highlightGroup.SetLayer( LAYER_SELECT_OVERLAY );
    getView()->Add( &highlightGroup );

    int limit = std::min( 9, aCollector->GetCount() );

    for( int i = 0; i < limit; ++i )
    {
        wxString text;
        BOARD_ITEM* item = ( *aCollector )[i];
        text = item->GetSelectMenuText();

        wxString menuText = wxString::Format("&%d. %s", i + 1, text );
        menu.Add( menuText, i + 1, item->GetMenuImage() );
    }

    menu.SetTitle( _( "Clarify Selection" ) );
    menu.SetIcon( info_xpm );
    menu.DisplayTitle( true );
    SetContextMenu( &menu, CMENU_NOW );

    while( OPT_TOOL_EVENT evt = Wait() )
    {
        if( evt->Action() == TA_CONTEXT_MENU_UPDATE )
        {
            if( current )
            {
                current->ClearBrightened();
                getView()->Hide( current, false );
                highlightGroup.Remove( current );
                getView()->MarkTargetDirty( KIGFX::TARGET_OVERLAY );
            }

            int id = *evt->GetCommandId();

            // User has pointed an item, so show it in a different way
            if( id > 0 && id <= limit )
            {
                current = ( *aCollector )[id - 1];
                current->SetBrightened();
                getView()->Hide( current, true );
                highlightGroup.Add( current );
                getView()->MarkTargetDirty( KIGFX::TARGET_OVERLAY );
            }
            else
            {
                current = NULL;
            }
        }
        else if( evt->Action() == TA_CONTEXT_MENU_CHOICE )
        {
            if( current )
            {
                current->ClearBrightened();
                getView()->Hide( current, false );
                highlightGroup.Remove( current );
                getView()->MarkTargetDirty( KIGFX::TARGET_OVERLAY );
            }

            OPT<int> id = evt->GetCommandId();

            // User has selected an item, so this one will be returned
            if( id && ( *id > 0 ) )
                current = ( *aCollector )[*id - 1];
            else
                current = NULL;

            break;
        }
    }
    getView()->Remove( &highlightGroup );


    return current;
}


BOARD_ITEM* SELECTION_TOOL::pickSmallestComponent( GENERAL_COLLECTOR* aCollector )
{
    int count = aCollector->GetPrimaryCount();     // try to use preferred layer

    if( 0 == count )
        count = aCollector->GetCount();

    for( int i = 0; i < count; ++i )
    {
        if( ( *aCollector )[i]->Type() != PCB_MODULE_T )
            return NULL;
    }

    // All are modules, now find smallest MODULE
    int minDim = 0x7FFFFFFF;
    int minNdx = 0;

    for( int i = 0; i < count; ++i )
    {
        MODULE* module = (MODULE*) ( *aCollector )[i];

        int lx = module->GetBoundingBox().GetWidth();
        int ly = module->GetBoundingBox().GetHeight();

        int lmin = std::min( lx, ly );

        if( lmin < minDim )
        {
            minDim = lmin;
            minNdx = i;
        }
    }

    return (*aCollector)[minNdx];
}


bool SELECTION_TOOL::selectable( const BOARD_ITEM* aItem ) const
{
    // Is high contrast mode enabled?
    bool highContrast = getView()->GetPainter()->GetSettings()->GetHighContrast();

    int layers[KIGFX::VIEW::VIEW_MAX_LAYERS], layers_count;

    // Filter out items that do not belong to active layers
    const std::set<unsigned int>& activeLayers = getView()->GetPainter()->
                                                 GetSettings()->GetActiveLayers();

    aItem->ViewGetLayers( layers, layers_count );

    if( highContrast )
    {
        bool onActive = false;          // Is the item on any of active layers?

        for( int i = 0; i < layers_count; ++i )
        {
            if( activeLayers.count( layers[i] ) > 0 ) // Item is on at least one of the active layers
            {
                onActive = true;
                break;
            }
        }

        if( !onActive ) // We do not want to select items that are in the background
        {
            return false;
        }
    }

    switch( aItem->Type() )
    {
    case PCB_ZONE_AREA_T:
        // Keepout zones can exist on multiple layers!
        {
            auto* zone = static_cast<const ZONE_CONTAINER*>( aItem );

            if( zone && zone->GetIsKeepout() )
            {
                auto zoneLayers = zone->GetLayerSet().Seq();

                for( unsigned int i = 0; i < zoneLayers.size(); i++ )
                {
                    if( board()->IsLayerVisible( zoneLayers[i] ) )
                    {
                        return true;
                    }
                }

                // No active layers selected!
                return false;
            }
        }
        break;

    case PCB_TRACE_T:
        {
            if( !board()->IsElementVisible( LAYER_TRACKS ) )
                return false;
        }
        break;

    case PCB_VIA_T:
        {
            const VIA* via = static_cast<const VIA*>( aItem );

            // Check if appropriate element layer is visible
            switch( via->GetViaType() )
            {
                case VIA_THROUGH:
                    if( !board()->IsElementVisible( LAYER_VIA_THROUGH ) )
                        return false;
                    break;

                case VIA_BLIND_BURIED:
                    if( !board()->IsElementVisible( LAYER_VIA_BBLIND ) )
                        return false;
                    break;

                case VIA_MICROVIA:
                    if( !board()->IsElementVisible( LAYER_VIA_MICROVIA ) )
                        return false;
                    break;

                default:
                    wxFAIL;
                    return false;
            }

            // For vias it is enough if only one of its layers is visible
            return ( board()->GetVisibleLayers() & via->GetLayerSet() ).any();
        }
        break;

    case PCB_MODULE_T:
    {
        // In the module editor, we do not want to select the module itself
        // rather, the module sub-components should be selected individually
        if( m_editModules )
            return false;

        // Allow selection of footprints if at least one draw layer is on and
        // the appropriate LAYER_MOD is on

        bool layer_mod = ( ( aItem->IsOnLayer( F_Cu ) && board()->IsElementVisible( LAYER_MOD_FR ) ) ||
                           ( aItem->IsOnLayer( B_Cu ) && board()->IsElementVisible( LAYER_MOD_BK ) ) );

        bool draw_layer_visible = false;
        int draw_layers[KIGFX::VIEW::VIEW_MAX_LAYERS], draw_layers_count;

        static_cast<const MODULE*>( aItem )->GetAllDrawingLayers( draw_layers,
                                                                  draw_layers_count,
                                                                  true );

        for( int i = 0; i < draw_layers_count; ++i )
        {
            // NOTE: Pads return LAYER_PADS_PLATEDHOLES but the visibility
            // control only directly switches LAYER_PADS_TH, so we overwrite it
            // here so that the visibility check is accurate
            if( draw_layers[i] == LAYER_PADS_PLATEDHOLES )
                draw_layers[i] = LAYER_PADS_TH;

            if( ( ( draw_layers[i] < PCB_LAYER_ID_COUNT ) &&
                  board()->IsLayerVisible( static_cast<PCB_LAYER_ID>( draw_layers[i] ) ) ) ||
                ( ( draw_layers[i] >= GAL_LAYER_ID_START ) &&
                  board()->IsElementVisible( static_cast<GAL_LAYER_ID>( draw_layers[i] ) ) ) )
            {
                draw_layer_visible = true;
            }
        }

        return ( draw_layer_visible && layer_mod );

        break;
    }

    case PCB_MODULE_TEXT_T:
        if( m_multiple && !m_editModules )
            return false;

        return view()->IsVisible( aItem ) && board()->IsLayerVisible( aItem->GetLayer() );

    case PCB_MODULE_EDGE_T:
    case PCB_PAD_T:
    {
        // Multiple selection is only allowed in modedit mode
        // In pcbnew, you have to select subparts of modules
        // one-by-one, rather than with a drag selection.
        // This is so you can pick up items under an (unlocked)
        // module without also moving the module's sub-parts.
        if( m_multiple && !m_editModules )
            return false;

        // In pcbnew, locked modules prevent individual pad selection
        // in modedit, we don't enforce this as the module is assumed to
        // be edited by design
        if( !m_editModules )
        {
            MODULE* mod = static_cast<const D_PAD*>( aItem )->GetParent();
            if( mod && mod->IsLocked() )
                return false;
        }

        if( aItem->Type() == PCB_PAD_T )
        {
            // In editor, pads are selectable if any draw layer is visible

            auto pad = static_cast<const D_PAD*>( aItem );

            // Shortcut: check copper layer visibility
            if( board()->IsLayerVisible( F_Cu ) && pad->IsOnLayer( F_Cu ) )
                return true;

            if( board()->IsLayerVisible( B_Cu ) && pad->IsOnLayer( B_Cu ) )
                return true;

            // Now check the non-copper layers

            bool draw_layer_visible = false;

            int pad_layers[KIGFX::VIEW::VIEW_MAX_LAYERS], pad_layers_count;
            pad->ViewGetLayers( pad_layers, pad_layers_count );

            for( int i = 0; i < pad_layers_count; ++i )
            {
                // NOTE: Only checking the regular layers (not GAL meta-layers)
                if( ( ( pad_layers[i] < PCB_LAYER_ID_COUNT ) &&
                      board()->IsLayerVisible( static_cast<PCB_LAYER_ID>( pad_layers[i] ) ) ) )
                {
                    draw_layer_visible = true;
                }
            }

            return draw_layer_visible;
        }

        break;
    }

    // These are not selectable
    case NOT_USED:
    case TYPE_NOT_INIT:
        return false;

    default:    // Suppress warnings
        break;
    }

    // All other items are selected only if the layer on which they exist is visible
    return board()->IsLayerVisible( aItem->GetLayer() );
}


void SELECTION_TOOL::select( BOARD_ITEM* aItem )
{
    if( aItem->IsSelected() )
    {
        return;
    }

    if( aItem->Type() == PCB_PAD_T )
    {
        MODULE* module = static_cast<MODULE*>( aItem->GetParent() );

        if( m_selection.Contains( module ) )
            return;
    }

    m_selection.Add( aItem );
    selectVisually( aItem );

    if( m_frame )
    {
        if( m_selection.Size() == 1 )
        {
            // Set as the current item, so the information about selection is displayed
            m_frame->SetCurItem( aItem, true );
        }
        else if( m_selection.Size() == 2 )  // Check only for 2, so it will not be
        {                                   // called for every next selected item
            // If multiple items are selected, do not show the information about the selected item
            m_frame->SetCurItem( NULL, true );
        }
    }
}


void SELECTION_TOOL::unselect( BOARD_ITEM* aItem )
{
    m_selection.Remove( aItem );
    unselectVisually( aItem );

    if( m_frame && m_frame->GetCurItem() == aItem )
        m_frame->SetCurItem( NULL );

    if( m_selection.Empty() )
    {
        if( m_frame )
        {
            m_frame->SetCurItem( NULL );
        }
        m_locked = true;
    }
}


void SELECTION_TOOL::selectVisually( BOARD_ITEM* aItem )
{
    // Hide the original item, so it is shown only on overlay
    aItem->SetSelected();
    view()->Hide( aItem, true );

    // Modules are treated in a special way - when they are selected, we have to
    // unselect all the parts that make the module, not the module itself

    if( aItem->Type() == PCB_MODULE_T )
    {
        static_cast<MODULE*>( aItem )->RunOnChildren( [&] ( BOARD_ITEM* item )
        {
            item->SetSelected();
            view()->Hide( item, true );
        });
    }

    view()->Update( &m_selection );
}


void SELECTION_TOOL::unselectVisually( BOARD_ITEM* aItem )
{
    // Restore original item visibility
    aItem->ClearSelected();
    view()->Hide( aItem, false );
    view()->Update( aItem );

    // Modules are treated in a special way - when they are selected, we have to
    // unselect all the parts that make the module, not the module itself

    if( aItem->Type() == PCB_MODULE_T )
    {
        static_cast<MODULE*>( aItem )->RunOnChildren( [&] ( BOARD_ITEM* item )
        {
            item->ClearSelected();
            view()->Hide( item, false );
            view()->Update( item );
        });
    }

    view()->Update( &m_selection );
}


bool SELECTION_TOOL::selectionContains( const VECTOR2I& aPoint ) const
{
    const unsigned GRIP_MARGIN = 20;
    VECTOR2D margin = getView()->ToWorld( VECTOR2D( GRIP_MARGIN, GRIP_MARGIN ), false );

    // Check if the point is located within any of the currently selected items bounding boxes
    for( auto item : m_selection )
    {
        BOX2I itemBox = item->ViewBBox();
        itemBox.Inflate( margin.x, margin.y );    // Give some margin for gripping an item

        if( itemBox.Contains( aPoint ) )
            return true;
    }

    return false;
}


static EDA_RECT getRect( const BOARD_ITEM* aItem )
{
    if( aItem->Type() == PCB_MODULE_T )
        return static_cast<const MODULE*>( aItem )->GetFootprintRect();

    return aItem->GetBoundingBox();
}


static double calcArea( const BOARD_ITEM* aItem )
{
    if( aItem->Type() == PCB_TRACE_T )
    {
        const TRACK* t = static_cast<const TRACK*>( aItem );
        return ( t->GetWidth() + t->GetLength() ) * t->GetWidth();
    }

    return getRect( aItem ).GetArea();
}


/*static double calcMinArea( GENERAL_COLLECTOR& aCollector, KICAD_T aType )
{
    double best = std::numeric_limits<double>::max();

    if( !aCollector.GetCount() )
        return 0.0;

    for( int i = 0; i < aCollector.GetCount(); i++ )
    {
        BOARD_ITEM* item = aCollector[i];
        if( item->Type() == aType )
            best = std::min( best, calcArea( item ) );
    }

    return best;
}*/


static double calcMaxArea( GENERAL_COLLECTOR& aCollector, KICAD_T aType )
{
    double best = 0.0;

    for( int i = 0; i < aCollector.GetCount(); i++ )
    {
        BOARD_ITEM* item = aCollector[i];
        if( item->Type() == aType )
            best = std::max( best, calcArea( item ) );
    }

    return best;
}


static inline double calcCommonArea( const BOARD_ITEM* aItem, const BOARD_ITEM* aOther )
{
    if( !aItem || !aOther )
        return 0;

    return getRect( aItem ).Common( getRect( aOther ) ).GetArea();
}


double calcRatio( double a, double b )
{
    if( a == 0.0 && b == 0.0 )
        return 1.0;

    if( b == 0.0 )
        return std::numeric_limits<double>::max();

    return a / b;
}


// The general idea here is that if the user clicks directly on a small item inside a larger
// one, then they want the small item.  The quintessential case of this is clicking on a pad
// within a footprint, but we also apply it for text within a footprint, footprints within
// larger footprints, and vias within either larger pads or longer tracks.
//
// These "guesses" presume there is area within the larger item to click in to select it.  If
// an item is mostly covered by smaller items within it, then the guesses are inappropriate as
// there might not be any area left to click to select the larger item.  In this case we must
// leave the items in the collector and bring up a Selection Clarification menu.
//
// We currently check for pads and text mostly covering a footprint, but we don’t check for
// smaller footprints mostly covering a larger footprint.
//
void SELECTION_TOOL::guessSelectionCandidates( GENERAL_COLLECTOR& aCollector,
        const VECTOR2I& aWhere ) const
{
    std::set<BOARD_ITEM*> rejected;
    std::set<BOARD_ITEM*> forced;

    // footprints which are below this percentage of the largest footprint will be considered
    // for selection; all others will not
    constexpr double footprintToFootprintMinRatio = 0.20;
    // pads which are below this percentage of their parent's area will exclude their parent
    constexpr double padToFootprintMinRatio = 0.45;
    // footprints containing items with items-to-footprint area ratio higher than this will be
    // forced to stay on the list
    constexpr double footprintMaxCoverRatio = 0.80;
    constexpr double viaToPadMinRatio = 0.50;
    constexpr double trackViaLengthRatio = 2.0;
    constexpr double trackTrackLengthRatio = 0.3;
    constexpr double textToFeatureMinRatio = 0.2;
    constexpr double textToFootprintMinRatio = 0.4;
    // If the common area of two compared items is above the following threshold, they cannot
    // be rejected (it means they overlap and it might be hard to pick one by selecting
    // its unique area).
    constexpr double commonAreaRatio = 0.6;

    PCB_LAYER_ID actLayer = (PCB_LAYER_ID) view()->GetTopLayer();

    LSET silkLayers( 2, B_SilkS, F_SilkS );

    if( silkLayers[actLayer] )
    {
        std::set<BOARD_ITEM*> preferred;

        for( int i = 0; i < aCollector.GetCount(); ++i )
        {
            BOARD_ITEM* item = aCollector[i];
            KICAD_T type = item->Type();

            if( ( type == PCB_MODULE_TEXT_T || type == PCB_TEXT_T || type == PCB_LINE_T )
                    && silkLayers[item->GetLayer()] )
            {
                preferred.insert( item );
            }
        }

        if( preferred.size() != 0 )
        {
            aCollector.Empty();

            for( BOARD_ITEM* item : preferred )
                aCollector.Append( item );
            return;
        }
    }

    int numZones = aCollector.CountType( PCB_ZONE_AREA_T );

    if( numZones > 0 && aCollector.GetCount() > numZones )
    {
        for( int i = aCollector.GetCount() - 1; i >= 0; i-- )
        {
            if( aCollector[i]->Type() == PCB_ZONE_AREA_T && !static_cast<ZONE_CONTAINER*>
                    ( aCollector[i] )->HitTestForEdge( wxPoint( aWhere.x, aWhere.y ) ) )
            {
                aCollector.Remove( i );
            }
        }
    }

    int numDrawitems = aCollector.CountType( PCB_LINE_T ) +
            aCollector.CountType( PCB_MODULE_EDGE_T );

    if( numDrawitems > 0 && aCollector.GetCount() > numDrawitems )
    {
        for( int i = aCollector.GetCount() - 1; i >= 0; i-- )
        {
            auto ds = static_cast<DRAWSEGMENT*>( aCollector[i] );
            if( ds->GetShape() == S_POLYGON )
                aCollector.Remove( i );
        }
    }

    if( aCollector.CountType( PCB_MODULE_TEXT_T ) > 0 )
    {
        for( int i = 0; i < aCollector.GetCount(); ++i )
        {
            if( TEXTE_MODULE* txt = dyn_cast<TEXTE_MODULE*>( aCollector[i] ) )
            {
                double textArea = calcArea( txt );

                for( int j = 0; j < aCollector.GetCount(); ++j )
                {
                    if( i == j )
                        continue;

                    BOARD_ITEM* item = aCollector[j];
                    double itemArea = calcArea( item );
                    double areaRatio = calcRatio( textArea, itemArea );
                    double commonArea = calcCommonArea( txt, item );
                    double itemCommonRatio = calcRatio( commonArea, itemArea );
                    double txtCommonRatio = calcRatio( commonArea, textArea );

                    if( item->Type() == PCB_MODULE_T )
                    {
                        // when text area is small compared to an overlapping footprint,
                        // then it's a clear sign the text is the selection target
                        if( areaRatio < textToFootprintMinRatio && itemCommonRatio < commonAreaRatio )
                            rejected.insert( item );
                    }

                    switch( item->Type() )
                    {
                        case PCB_TRACE_T:
                        case PCB_PAD_T:
                        case PCB_LINE_T:
                        case PCB_VIA_T:
                        case PCB_MODULE_T:
                            if( areaRatio > textToFeatureMinRatio && txtCommonRatio < commonAreaRatio )
                                rejected.insert( txt );
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    if( aCollector.CountType( PCB_PAD_T ) > 0 )
    {
        for( int i = 0; i < aCollector.GetCount(); ++i )
        {
            if( D_PAD* pad = dyn_cast<D_PAD*>( aCollector[i] ) )
            {
                MODULE* parent = pad->GetParent();
                double ratio = calcRatio( calcArea( pad ), calcArea( parent ) );

                // when pad area is small compared to the parent footprint,
                // then it is a clear sign the pad is the selection target
                if( ratio < padToFootprintMinRatio )
                    rejected.insert( pad->GetParent() );
            }
        }
    }

    int moduleCount = aCollector.CountType( PCB_MODULE_T );

    if( moduleCount > 0 )
    {
        double maxArea = calcMaxArea( aCollector, PCB_MODULE_T );
        BOX2D viewportD = getView()->GetViewport();
        BOX2I viewport( VECTOR2I( viewportD.GetPosition() ), VECTOR2I( viewportD.GetSize() ) );
        double maxCoverRatio = footprintMaxCoverRatio;

        // MODULE::CoverageRatio() doesn't take zone handles & borders into account so just
        // use a more aggressive cutoff point if zones are involved.
        if(  aCollector.CountType( PCB_ZONE_AREA_T ) )
            maxCoverRatio /= 2;

        for( int i = 0; i < aCollector.GetCount(); ++i )
        {
            if( MODULE* mod = dyn_cast<MODULE*>( aCollector[i] ) )
            {
                // filter out components larger than the viewport
                if( mod->ViewBBox().Contains( viewport ) )
                    rejected.insert( mod );
                // footprints completely covered with other features have no other
                // means of selection, so must be kept
                else if( mod->CoverageRatio( aCollector ) > maxCoverRatio )
                    rejected.erase( mod );
                // if a footprint is much smaller than the largest overlapping
                // footprint then it should be considered for selection
                else if( calcRatio( calcArea( mod ), maxArea ) <= footprintToFootprintMinRatio )
                    continue;
                // reject ALL OTHER footprints (whether there are one or more of
                // them); the other items in the list should have precedence
                else
                    rejected.insert( mod );
            }
        }
    }

    if( aCollector.CountType( PCB_VIA_T ) > 0 )
    {
        for( int i = 0; i < aCollector.GetCount(); ++i )
        {
            if( VIA* via = dyn_cast<VIA*>( aCollector[i] ) )
            {
                double viaArea = calcArea( via );

                for( int j = 0; j < aCollector.GetCount(); ++j )
                {
                    if( i == j )
                        continue;

                    BOARD_ITEM* item = aCollector[j];
                    double areaRatio = calcRatio( viaArea, calcArea( item ) );

                    if( item->Type() == PCB_MODULE_T && areaRatio < padToFootprintMinRatio )
                        rejected.insert( item );

                    if( item->Type() == PCB_PAD_T && areaRatio < viaToPadMinRatio )
                        rejected.insert( item );

                    if( TRACK* track = dyn_cast<TRACK*>( item ) )
                    {
                        if( track->GetNetCode() != via->GetNetCode() )
                            continue;

                        double lenRatio = (double) ( track->GetLength() + track->GetWidth() ) /
                                          (double) via->GetWidth();

                        if( lenRatio > trackViaLengthRatio )
                            rejected.insert( track );
                    }
                }
            }
        }
    }

    int nTracks = aCollector.CountType( PCB_TRACE_T );

    if( nTracks > 0 )
    {
        double maxLength = 0.0;
        double minLength = std::numeric_limits<double>::max();
        double maxArea = 0.0;
        const TRACK* maxTrack = nullptr;

        for( int i = 0; i < aCollector.GetCount(); ++i )
        {
            if( TRACK* track = dyn_cast<TRACK*>( aCollector[i] ) )
            {
                maxLength = std::max( track->GetLength(), maxLength );
                maxLength = std::max( (double) track->GetWidth(), maxLength );

                minLength = std::min( std::max( track->GetLength(), (double) track->GetWidth() ), minLength );

                double area = track->GetLength() * track->GetWidth();

                if( area > maxArea )
                {
                    maxArea = area;
                    maxTrack = track;
                }
            }
        }

        if( maxLength > 0.0 && minLength / maxLength < trackTrackLengthRatio && nTracks > 1 )
        {
            for( int i = 0; i < aCollector.GetCount(); ++i )
             {
                if( TRACK* track = dyn_cast<TRACK*>( aCollector[i] ) )
                {
                    double ratio = std::max( (double) track->GetWidth(), track->GetLength() ) / maxLength;

                    if( ratio > trackTrackLengthRatio )
                        rejected.insert( track );
                }
            }
        }

        for( int j = 0; j < aCollector.GetCount(); ++j )
        {
            if( MODULE* mod = dyn_cast<MODULE*>( aCollector[j] ) )
            {
                double ratio = calcRatio( maxArea, mod->GetFootprintRect().GetArea() );

                if( ratio < padToFootprintMinRatio && calcCommonArea( maxTrack, mod ) < commonAreaRatio )
                    rejected.insert( mod );
            }
        }
    }

    if( (unsigned) aCollector.GetCount() > rejected.size() )  // do not remove everything
    {
        for( BOARD_ITEM* item : rejected )
        {
            aCollector.Remove( item );
        }
    }
}


int SELECTION_TOOL::updateSelection( const TOOL_EVENT& aEvent )
{
    getView()->Update( &m_selection );

    return 0;
}


bool SELECTION_TOOL::SanitizeSelection()
{
    std::set<BOARD_ITEM*> rejected;
    std::set<BOARD_ITEM*> added;

    if( !m_editModules )
    {
        for( auto i : m_selection )
        {
            auto item = static_cast<BOARD_ITEM*>( i );
            if( item->Type() == PCB_PAD_T )
            {
                MODULE* mod = static_cast<MODULE*>( item->GetParent() );

                // case 1: module (or its pads) are locked
                if( mod && ( mod->PadsLocked() || mod->IsLocked() ) )
                {
                    rejected.insert( item );

                    if( !mod->IsLocked() && !mod->IsSelected() )
                        added.insert( mod );
                }

                // case 2: multi-item selection contains both the module and its pads - remove the pads
                if( mod && m_selection.Contains( mod ) )
                    rejected.insert( item );
            }
        }
    }

    if( !rejected.empty() )
    {
        for( BOARD_ITEM* item : rejected )
            unselect( item );

        // Inform other potentially interested tools
        m_toolMgr->ProcessEvent( UnselectedEvent );
    }

    if( !added.empty() )
    {
        for( BOARD_ITEM* item : added )
            select( item );

        // Inform other potentially interested tools
        m_toolMgr->ProcessEvent( SelectedEvent );
    }

    return true;
}


const TOOL_EVENT SELECTION_TOOL::SelectedEvent( TC_MESSAGE, TA_ACTION, "pcbnew.InteractiveSelection.selected" );
const TOOL_EVENT SELECTION_TOOL::UnselectedEvent( TC_MESSAGE, TA_ACTION, "pcbnew.InteractiveSelection.unselected" );
const TOOL_EVENT SELECTION_TOOL::ClearedEvent( TC_MESSAGE, TA_ACTION, "pcbnew.InteractiveSelection.cleared" );
