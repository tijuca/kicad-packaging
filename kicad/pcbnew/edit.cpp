/**************************************/
/* edit.cpp: edit PCB implementation. */
/**************************************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "class_drawpanel.h"
#include "confirm.h"
#include "eda_doc.h"
#include "gestfich.h"
#include "pcbnew_id.h"

#include "pcbnew.h"
#include "wxPcbStruct.h"
#include "class_board_design_settings.h"
#include "autorout.h"
#include "protos.h"

#include "kicad_device_context.h"

#include "dialog_global_edit_tracks_and_vias.h"

// Uncomment following line to enable wxBell() command (which beeps speaker)
// #include <wx/utils.h>

static void Process_Move_Item( WinEDA_PcbFrame* frame,
                               EDA_BaseStruct* DrawStruct, wxDC* DC );



/* Handles the selection of command events. */
void WinEDA_PcbFrame::Process_Special_Functions( wxCommandEvent& event )
{
    int         id = event.GetId();
    wxPoint     pos;

    int         itmp;
    INSTALL_DC( dc, DrawPanel );
    BOARD_ITEM* DrawStruct = GetCurItem();

    DrawPanel->CursorOff( &dc );

    wxGetMousePosition( &pos.x, &pos.y );

    pos.y += 20;

    switch( id )   // Some (not all ) edit commands must be finished or aborted
    {
    case wxID_CUT:
    case wxID_COPY:
    case ID_ON_GRID_SELECT:
    case ID_ON_ZOOM_SELECT:
    case ID_PCB_USER_GRID_SETUP:
    case ID_TOOLBARH_PCB_SELECT_LAYER:
    case ID_AUX_TOOLBAR_PCB_SELECT_LAYER_PAIR:
    case ID_POPUP_PCB_ROTATE_TEXTEPCB:
    case ID_POPUP_PCB_EDIT_TEXTEPCB:
    case ID_POPUP_PCB_EDIT_MIRE:
    case ID_POPUP_PCB_ROTATE_TEXTMODULE:
    case ID_POPUP_PCB_ROTATE_MODULE_CLOCKWISE:
    case ID_POPUP_PCB_ROTATE_MODULE_COUNTERCLOCKWISE:
    case ID_POPUP_PCB_CHANGE_SIDE_MODULE:
    case ID_POPUP_PCB_EDIT_MODULE:
    case ID_POPUP_PCB_EDIT_TEXTMODULE:
    case ID_POPUP_PCB_STOP_CURRENT_DRAWING:
    case ID_POPUP_PCB_END_TRACK:
    case ID_POPUP_PCB_PLACE_VIA:
    case ID_POPUP_PCB_PLACE_MICROVIA:
    case ID_POPUP_PCB_IMPORT_PAD_SETTINGS:
    case ID_POPUP_PCB_EXPORT_PAD_SETTINGS:
    case ID_POPUP_PCB_GLOBAL_IMPORT_PAD_SETTINGS:
    case ID_POPUP_PCB_STOP_CURRENT_EDGE_ZONE:
    case ID_POPUP_PCB_DELETE_ZONE_LAST_CREATED_CORNER:
    case ID_POPUP_PCB_FILL_ALL_ZONES:
    case ID_POPUP_PCB_REMOVE_FILLED_AREAS_IN_ALL_ZONES:
    case ID_POPUP_PCB_REMOVE_FILLED_AREAS_IN_CURRENT_ZONE:
    case ID_POPUP_PCB_PLACE_ZONE_CORNER:
    case ID_POPUP_PCB_PLACE_ZONE_OUTLINES:
    case ID_POPUP_PCB_EDIT_ZONE_PARAMS:
    case ID_POPUP_PCB_DELETE_ZONE:
    case ID_POPUP_PCB_MOVE_ZONE_CORNER:
    case ID_POPUP_PCB_DRAG_ZONE_OUTLINE_SEGMENT:
    case ID_POPUP_PCB_MOVE_ZONE_OUTLINES:
    case ID_POPUP_PCB_ADD_ZONE_CORNER:
    case ID_POPUP_PCB_DELETE_TRACKSEG:
    case ID_POPUP_PCB_DELETE_TRACK:
    case ID_POPUP_PCB_DELETE_TRACKNET:
    case ID_POPUP_PCB_FILL_ZONE:
    case ID_POPUP_PCB_SELECT_LAYER:
    case ID_POPUP_PCB_SELECT_CU_LAYER:
    case ID_POPUP_PCB_SELECT_LAYER_PAIR:
    case ID_POPUP_PCB_SELECT_NO_CU_LAYER:
    case ID_POPUP_PCB_MOVE_TRACK_NODE:
    case ID_POPUP_PCB_DRAG_TRACK_SEGMENT_KEEP_SLOPE:
    case ID_POPUP_PCB_DRAG_TRACK_SEGMENT:
    case ID_POPUP_PCB_MOVE_TRACK_SEGMENT:
    case ID_POPUP_PCB_PLACE_MOVED_TRACK_NODE:
    case ID_POPUP_PCB_BREAK_TRACK:
    case ID_POPUP_PCB_EDIT_NET:
    case ID_POPUP_PCB_EDIT_TRACK:
    case ID_POPUP_PCB_EDIT_TRACKSEG:
    case ID_POPUP_PCB_LOCK_ON_TRACKSEG:
    case ID_POPUP_PCB_LOCK_OFF_TRACKSEG:
    case ID_POPUP_PCB_LOCK_ON_TRACK:
    case ID_POPUP_PCB_LOCK_OFF_TRACK:
    case ID_POPUP_PCB_LOCK_ON_NET:
    case ID_POPUP_PCB_LOCK_OFF_NET:
    case ID_POPUP_DELETE_BLOCK:
    case ID_POPUP_PLACE_BLOCK:
    case ID_POPUP_ZOOM_BLOCK:
    case ID_POPUP_FLIP_BLOCK:
    case ID_POPUP_ROTATE_BLOCK:
    case ID_POPUP_COPY_BLOCK:
    case ID_POPUP_PCB_EDIT_DRAWING:
    case ID_POPUP_PCB_GETINFO_MARKER:
        break;

    case ID_POPUP_CANCEL_CURRENT_COMMAND:
        if( DrawPanel->ManageCurseur
            && DrawPanel->ForceCloseManageCurseur )
        {
            DrawPanel->ForceCloseManageCurseur( DrawPanel, &dc );
        }
        /* Should not be executed, just in case */
        if( GetScreen()->m_BlockLocate.m_Command != BLOCK_IDLE )
        {
            GetScreen()->m_BlockLocate.m_Command = BLOCK_IDLE;
            GetScreen()->m_BlockLocate.m_State   = STATE_NO_BLOCK;
            GetScreen()->m_BlockLocate.ClearItemsList();
        }
        if( m_ID_current_state == 0 )
            SetToolID( 0, wxCURSOR_ARROW, wxEmptyString );
        else
            SetCursor( DrawPanel->m_PanelCursor = DrawPanel->m_PanelDefaultCursor );
        break;

    default:        // Finish (abort) the command
        if( DrawPanel->ManageCurseur && DrawPanel->ForceCloseManageCurseur )
            DrawPanel->ForceCloseManageCurseur( DrawPanel, &dc );

        if( m_ID_current_state != id )
        {
            if( m_ID_last_state != m_ID_current_state )
                m_ID_last_state = m_ID_current_state;
            SetToolID( 0, wxCURSOR_ARROW, wxEmptyString );
        }
        break;
    }

    switch( id )   // Execute command
    {
    case 0:
        break;

    case ID_OPEN_MODULE_EDITOR:
        if( m_ModuleEditFrame == NULL )
        {
            m_ModuleEditFrame =
                new WinEDA_ModuleEditFrame( this,
                                           _( "Module Editor" ),
                                           wxPoint( -1, -1 ),
                                           wxSize( 600, 400 ) );
            m_ModuleEditFrame->Show( true );
            m_ModuleEditFrame->Zoom_Automatique( true );
        }
        else
            m_ModuleEditFrame->Iconize( false );
        break;

    case ID_PCB_GLOBAL_DELETE:
        InstallPcbGlobalDeleteFrame( pos );
        break;

    case ID_POPUP_PLACE_BLOCK:
        GetScreen()->m_BlockLocate.m_Command = BLOCK_MOVE;
        DrawPanel->m_AutoPAN_Request = false;
        HandleBlockPlace( &dc );
        break;

    case ID_POPUP_COPY_BLOCK:
        GetScreen()->m_BlockLocate.m_Command = BLOCK_COPY;
        GetScreen()->m_BlockLocate.SetMessageBlock( this );
        DrawPanel->m_AutoPAN_Request = false;
        HandleBlockPlace( &dc );
        break;

    case ID_POPUP_ZOOM_BLOCK:
        GetScreen()->m_BlockLocate.m_Command = BLOCK_ZOOM;
        GetScreen()->m_BlockLocate.SetMessageBlock( this );
        GetScreen()->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        break;

    case ID_POPUP_DELETE_BLOCK:
        GetScreen()->m_BlockLocate.m_Command = BLOCK_DELETE;
        GetScreen()->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        break;

    case ID_POPUP_ROTATE_BLOCK:
        GetScreen()->m_BlockLocate.m_Command = BLOCK_ROTATE;
        GetScreen()->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        break;

    case ID_POPUP_FLIP_BLOCK:
        GetScreen()->m_BlockLocate.m_Command = BLOCK_FLIP;
        GetScreen()->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        break;

    case ID_DRC_CONTROL:
        Install_Test_DRC_Frame( &dc );
        break;

    case ID_GET_NETLIST:
        InstallNetlistFrame( &dc, wxPoint( -1, -1 ) );
        break;

    case ID_GET_TOOLS:

        // InstalloolsFrame(this, wxPoint(-1,-1) );
        break;

    case ID_FIND_ITEMS:
        InstallFindFrame( pos, &dc );
        break;

    case ID_TRACK_BUTT:
        SetToolID( id, wxCURSOR_PENCIL, _( "Add Tracks" ) );
        if( (GetBoard()->m_Status_Pcb & LISTE_RATSNEST_ITEM_OK) == 0 )
        {
            Compile_Ratsnest( &dc, true );
        }
        break;

    case ID_PCB_ZONES_BUTT:
        SetToolID( id, wxCURSOR_PENCIL, _( "Add Zones" ) );
        if( DisplayOpt.DisplayZonesMode != 0 )
            DisplayInfoMessage( this, _( "Warning: Display Zone is OFF!!!" ) );
        if( !g_HighLight_Status && (g_HighLight_NetCode > 0 ) )
            High_Light( &dc );
        break;

    case ID_PCB_MIRE_BUTT:
        SetToolID( id, wxCURSOR_PENCIL, _( "Add Layer Alignment Target" ) );
        break;

    case ID_PCB_PLACE_OFFSET_COORD_BUTT:
        SetToolID( id, wxCURSOR_PENCIL, _( "Adjust Zero" ) );
        break;

    case ID_PCB_ADD_LINE_BUTT:
    case ID_PCB_ARC_BUTT:
    case ID_PCB_CIRCLE_BUTT:
        SetToolID( id, wxCURSOR_PENCIL, _( "Add Graphic" ) );
        break;

    case ID_PCB_ADD_TEXT_BUTT:
        SetToolID( id, wxCURSOR_PENCIL, _( "Add Text" ) );
        break;

    case ID_COMPONENT_BUTT:
        SetToolID( id, wxCURSOR_HAND, _( "Add Modules" ) );
        break;

    case ID_PCB_COTATION_BUTT:
        SetToolID( id, wxCURSOR_PENCIL, _( "Add Dimension" ) );
        break;

    case ID_NO_SELECT_BUTT:
        SetToolID( 0, wxCURSOR_ARROW, wxEmptyString );
        break;

    case ID_PCB_HIGHLIGHT_BUTT:
        SetToolID( id, wxCURSOR_HAND, _( "Net Highlight" ) );
        break;

    case ID_PCB_SHOW_1_RATSNEST_BUTT:
        SetToolID( id, wxCURSOR_HAND, _( "Local Ratsnest" ) );
        if( (GetBoard()->m_Status_Pcb & LISTE_RATSNEST_ITEM_OK) == 0 )
            Compile_Ratsnest( &dc, true );
        break;

    case ID_POPUP_CLOSE_CURRENT_TOOL:
        SetToolID( 0, wxCURSOR_ARROW, wxEmptyString );
        break;

    case ID_POPUP_CANCEL_CURRENT_COMMAND:
        break;

    case ID_POPUP_PCB_END_LINE:
        DrawPanel->MouseToCursorSchema();

        //  EndSegment(&dc);
        break;

    case ID_POPUP_PCB_EDIT_TRACK:
        if( GetCurItem() == NULL )
            break;
        Edit_Track_Width( &dc, (TRACK*) GetCurItem() );
        DrawPanel->MouseToCursorSchema();
        OnModify();
        break;

    case ID_POPUP_PCB_EDIT_TRACKSEG:
        if( GetCurItem() == NULL )
            break;
        Edit_TrackSegm_Width( &dc, (TRACK*) GetCurItem() );
        DrawPanel->MouseToCursorSchema();
        OnModify();
        break;

    case ID_POPUP_PCB_EDIT_ALL_VIAS_AND_TRACK_SIZE:
        if( GetCurItem() == NULL )
            break;
        {
        int type = GetCurItem()->Type();
        if( type == TYPE_TRACK || type == TYPE_VIA )
        {
            BOARD_CONNECTED_ITEM*item = (BOARD_CONNECTED_ITEM*) GetCurItem();
            DIALOG_GLOBAL_EDIT_TRACKS_AND_VIAS dlg( this, item->GetNet() );
            dlg.ShowModal();
        }
        }
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_END_TRACK:
        DrawPanel->MouseToCursorSchema();
        End_Route( (TRACK*) GetCurItem(), &dc );
        break;

    case ID_POPUP_PCB_PLACE_MOVED_TRACK_NODE:
        DrawPanel->MouseToCursorSchema();
        if( GetCurItem()->m_Flags & IS_DRAGGED )
        {
            PlaceDraggedOrMovedTrackSegment( (TRACK*) GetCurItem(), &dc );
        }
        break;

    case ID_POPUP_PCB_PLACE_MICROVIA:
        if( !IsMicroViaAcceptable() )
            break;
    case ID_POPUP_PCB_PLACE_VIA:
        DrawPanel->MouseToCursorSchema();
        if( GetCurItem()->m_Flags & IS_DRAGGED )
        {
            PlaceDraggedOrMovedTrackSegment( (TRACK*) GetCurItem(), &dc );
        }
        else
        {
            int v_type = GetBoard()->GetBoardDesignSettings()->m_CurrentViaType;
            if( id == ID_POPUP_PCB_PLACE_MICROVIA )
                GetBoard()->GetBoardDesignSettings()->m_CurrentViaType = VIA_MICROVIA; // place micro via and switch layer
            Other_Layer_Route( (TRACK*) GetCurItem(), &dc );
            GetBoard()->GetBoardDesignSettings()->m_CurrentViaType = v_type;
            if( DisplayOpt.ContrastModeDisplay )
                ( (PCB_SCREEN*) GetScreen() )->SetRefreshReq();
        }
        break;

    case ID_POPUP_PCB_DELETE_TRACKSEG:
        if( GetCurItem() == NULL )
            break;
        DrawPanel->MouseToCursorSchema();
        SetCurItem( Delete_Segment( &dc, (TRACK*) GetCurItem() ) );
        OnModify();
        break;

    case ID_POPUP_PCB_DELETE_TRACK:
        if( GetCurItem() == NULL )
            break;
        DrawPanel->MouseToCursorSchema();
        Delete_Track( &dc, (TRACK*) GetCurItem() );
        SetCurItem( NULL );
        OnModify();
        break;

    case ID_POPUP_PCB_DELETE_TRACKNET:
        DrawPanel->MouseToCursorSchema();
        Delete_net( &dc, (TRACK*) GetCurItem() );
        SetCurItem( NULL );
        OnModify();
        break;

    case ID_POPUP_PCB_LOCK_ON_TRACKSEG:
        Attribut_Segment( (TRACK*) GetCurItem(), &dc, true );
        break;

    case ID_POPUP_PCB_LOCK_OFF_TRACKSEG:
        Attribut_Segment( (TRACK*) GetCurItem(), &dc, false );
        break;

    case ID_POPUP_PCB_LOCK_ON_TRACK:
        Attribut_Track( (TRACK*) GetCurItem(), &dc, true );
        break;

    case ID_POPUP_PCB_LOCK_OFF_TRACK:
        Attribut_Track( (TRACK*) GetCurItem(), &dc, false );
        break;

    case ID_POPUP_PCB_LOCK_ON_NET:
        Attribut_net( &dc, ( (TRACK*) GetCurItem() )->GetNet(), true );
        break;

    case ID_POPUP_PCB_LOCK_OFF_NET:
        Attribut_net( &dc, ( (TRACK*) GetCurItem() )->GetNet(), false );
        break;

    case ID_POPUP_PCB_SETFLAGS_TRACK_MNU:
        break;

    case ID_POPUP_PCB_DELETE_ZONE:
        DrawPanel->MouseToCursorSchema();
        if( GetCurItem() == NULL )
            break;
        {
            SEGZONE* zsegm   = (SEGZONE*) GetCurItem();
            int      netcode = zsegm->GetNet();
            Delete_Zone_Fill( zsegm );
            SetCurItem( NULL );
            test_1_net_connexion( NULL, netcode );
            OnModify();
            GetBoard()->DisplayInfo( this );
        }
        break;

    case ID_POPUP_PCB_EDIT_ZONE_PARAMS:
        Edit_Zone_Params( &dc, (ZONE_CONTAINER*) GetCurItem() );
        SetCurItem( NULL ); // Outlines can have changed
        break;

    case ID_POPUP_PCB_ZONE_ADD_SIMILAR_ZONE:
        DrawPanel->MouseToCursorSchema();
        DrawPanel->m_AutoPAN_Request = true;
        Add_Similar_Zone( &dc, (ZONE_CONTAINER*) GetCurItem() );
        break;

    case ID_POPUP_PCB_ZONE_ADD_CUTOUT_ZONE:
        DrawPanel->MouseToCursorSchema();
        DrawPanel->m_AutoPAN_Request = true;
        Add_Zone_Cutout( &dc, (ZONE_CONTAINER*) GetCurItem() );
        break;

    case ID_POPUP_PCB_DELETE_ZONE_CONTAINER:
    case ID_POPUP_PCB_DELETE_ZONE_CUTOUT:
        DrawPanel->MouseToCursorSchema();
        {
            int netcode = ( (ZONE_CONTAINER*) GetCurItem() )->GetNet();
            Delete_Zone_Contour( &dc, (ZONE_CONTAINER*) GetCurItem() );
            SetCurItem( NULL );
            test_1_net_connexion( NULL, netcode );
            GetBoard()->DisplayInfo( this );
        }
        break;

    case ID_POPUP_PCB_DELETE_ZONE_CORNER:
        Remove_Zone_Corner( &dc, (ZONE_CONTAINER*) GetCurItem() );
        SetCurItem( NULL );
        break;

    case ID_POPUP_PCB_MOVE_ZONE_CORNER:
    {
        DrawPanel->MouseToCursorSchema();
        ZONE_CONTAINER* zone_cont = (ZONE_CONTAINER*) GetCurItem();
        DrawPanel->m_AutoPAN_Request = true;
        Start_Move_Zone_Corner( &dc,
                                zone_cont,
                                zone_cont->m_CornerSelection,
                                false );
        break;
    }

    case ID_POPUP_PCB_DRAG_ZONE_OUTLINE_SEGMENT:
    {
        DrawPanel->MouseToCursorSchema();
        ZONE_CONTAINER* zone_cont = (ZONE_CONTAINER*) GetCurItem();
        DrawPanel->m_AutoPAN_Request = true;
        Start_Move_Zone_Drag_Outline_Edge( &dc,
                                           zone_cont,
                                           zone_cont->m_CornerSelection );
        break;
    }

    case ID_POPUP_PCB_MOVE_ZONE_OUTLINES:
    {
        DrawPanel->MouseToCursorSchema();
        ZONE_CONTAINER* zone_cont = (ZONE_CONTAINER*) GetCurItem();
        DrawPanel->m_AutoPAN_Request = true;
        Start_Move_Zone_Outlines( &dc, zone_cont );
        break;
    }

    case ID_POPUP_PCB_ADD_ZONE_CORNER:
    {
        DrawPanel->MouseToCursorSchema();
        ZONE_CONTAINER* zone_cont = (ZONE_CONTAINER*) GetCurItem();
        wxPoint         pos = GetScreen()->m_Curseur;

        /* add corner between zone_cont->m_CornerSelection
         * and zone_cont->m_CornerSelection+1
         * and start move the new corner
         */
        zone_cont->Draw( DrawPanel, &dc, GR_XOR );
        zone_cont->m_Poly->InsertCorner( zone_cont->m_CornerSelection,
                                         pos.x, pos.y );
        zone_cont->m_CornerSelection++;
        zone_cont->Draw( DrawPanel, &dc, GR_XOR );
        DrawPanel->m_AutoPAN_Request = true;
        Start_Move_Zone_Corner( &dc,
                                zone_cont,
                                zone_cont->m_CornerSelection,
                                true );
        break;
    }

    case ID_POPUP_PCB_PLACE_ZONE_OUTLINES:
    case ID_POPUP_PCB_PLACE_ZONE_CORNER:
    {
        DrawPanel->MouseToCursorSchema();
        ZONE_CONTAINER* zone_cont = (ZONE_CONTAINER*) GetCurItem();
        End_Move_Zone_Corner_Or_Outlines( &dc, zone_cont );
        DrawPanel->m_AutoPAN_Request = false;
        break;
    }

    case ID_POPUP_PCB_FILL_ALL_ZONES:
        DrawPanel->MouseToCursorSchema();
        Fill_All_Zones();
        GetBoard()->DisplayInfo( this );
        break;

    case ID_POPUP_PCB_REMOVE_FILLED_AREAS_IN_CURRENT_ZONE:
        if( ( GetCurItem() )->Type() == TYPE_ZONE_CONTAINER )
        {
            ZONE_CONTAINER* zone_container = (ZONE_CONTAINER*) GetCurItem();
            Delete_Zone_Fill( NULL, zone_container->m_TimeStamp );
            test_1_net_connexion( NULL, zone_container->GetNet() );
            OnModify();
            GetBoard()->DisplayInfo( this );
            DrawPanel->Refresh();
        }
        SetCurItem( NULL );
        break;

    case ID_POPUP_PCB_REMOVE_FILLED_AREAS_IN_ALL_ZONES: // Remove all zones :
        GetBoard()->m_Zone.DeleteAll();                 // remove zone segments used to fill zones.
        for( int ii = 0; ii < GetBoard()->GetAreaCount(); ii++ )
        {
            // Remove filled areas in zone
            ZONE_CONTAINER* zone_container = GetBoard()->GetArea( ii );
            zone_container->m_FilledPolysList.clear();;
        }

        SetCurItem( NULL );             // CurItem might be deleted by this command, clear the pointer
        test_connexions( NULL );
        Tst_Ratsnest( NULL, 0 );        // Recalculate the active ratsnest, i.e. the unconnected links */
        OnModify();
        GetBoard()->DisplayInfo( this );
        DrawPanel->Refresh();
        break;

    case ID_POPUP_PCB_FILL_ZONE:
        DrawPanel->MouseToCursorSchema();
        Fill_Zone( (ZONE_CONTAINER*) GetCurItem() );
        test_1_net_connexion( NULL, ( (ZONE_CONTAINER*) GetCurItem() )->GetNet() );
        GetBoard()->DisplayInfo( this );
        DrawPanel->Refresh();
        break;

    case ID_PCB_DELETE_ITEM_BUTT:
        SetToolID( id, wxCURSOR_BULLSEYE, _( "Delete item" ) );
        break;

    case ID_POPUP_PCB_MOVE_TEXTEPCB_REQUEST:
        Process_Move_Item( this, GetCurItem(), &dc );
        DrawPanel->m_AutoPAN_Request = true;
        break;

    case ID_POPUP_PCB_DRAG_MODULE_REQUEST:
        g_Drag_Pistes_On = true;

    case ID_POPUP_PCB_MOVE_MODULE_REQUEST:

        // If the current Item is a pad, text module ...: Get its parent
        if( GetCurItem()->Type() != TYPE_MODULE )
            SetCurItem( GetCurItem()->GetParent() );
        if( !GetCurItem() || GetCurItem()->Type() != TYPE_MODULE )
        {
            g_Drag_Pistes_On = false;
            break;
        }
        DrawPanel->MouseToCursorSchema();
        StartMove_Module( (MODULE*) GetCurItem(), &dc );
        break;

    case ID_POPUP_PCB_GET_AND_MOVE_MODULE_REQUEST:      /* get module by name and move it */
        SetCurItem( GetModuleByName() );
        if( GetCurItem() )
        {
            DrawPanel->MouseToCursorSchema();
            StartMove_Module( (MODULE*) GetCurItem(), &dc );
        }
        break;

    case ID_POPUP_PCB_DELETE_MODULE:
        DrawPanel->MouseToCursorSchema();

        // If the current Item is a pad, text module ...: Get its parent
        if( GetCurItem()->Type() != TYPE_MODULE )
            SetCurItem( GetCurItem()->GetParent() );

        if( !GetCurItem() || GetCurItem()->Type() != TYPE_MODULE )
            break;
        if( Delete_Module( (MODULE*) GetCurItem(), &dc, true ) )
        {
            SetCurItem( NULL );
        }
        break;

    case ID_POPUP_PCB_ROTATE_MODULE_COUNTERCLOCKWISE:
        DrawPanel->MouseToCursorSchema();

        // If the current Item is a pad, text module ...: Get its parent
        if( GetCurItem()->Type() != TYPE_MODULE )
            SetCurItem( GetCurItem()->GetParent() );

        if( !GetCurItem() || GetCurItem()->Type() != TYPE_MODULE )
            break;

        if( !(GetCurItem()->m_Flags & IS_MOVED) ) /* This is a simple rotation, no other edition in progress */
            SaveCopyInUndoList(GetCurItem(), UR_ROTATED, ((MODULE*)GetCurItem())->m_Pos);
        Rotate_Module( &dc, (MODULE*) GetCurItem(), 900, true );
        break;

    case ID_POPUP_PCB_ROTATE_MODULE_CLOCKWISE:
        DrawPanel->MouseToCursorSchema();

        // If the current Item is a pad, text module ...: Get its parent
        if( GetCurItem()->Type() != TYPE_MODULE )
            SetCurItem( GetCurItem()->GetParent() );

        if( !GetCurItem() || GetCurItem()->Type() != TYPE_MODULE )
            break;
        if( !(GetCurItem()->m_Flags & IS_MOVED) ) /* This is a simple rotation, no other edition in progress */
            SaveCopyInUndoList(GetCurItem(), UR_ROTATED_CLOCKWISE, ((MODULE*)GetCurItem())->m_Pos);
        Rotate_Module( &dc, (MODULE*) GetCurItem(), -900, true );
        break;

    case ID_POPUP_PCB_CHANGE_SIDE_MODULE:
        DrawPanel->MouseToCursorSchema();

        // If the current Item is a pad, text module ...: Get its parent
        if( GetCurItem()->Type() != TYPE_MODULE )
            SetCurItem( GetCurItem()->GetParent() );
        if( !GetCurItem() || GetCurItem()->Type() != TYPE_MODULE )
            break;

        if( !(GetCurItem()->m_Flags & IS_MOVED) ) /* This is a simple flip, no other edition in progress */
            SaveCopyInUndoList(GetCurItem(), UR_FLIPPED, ((MODULE*)GetCurItem())->m_Pos);

        Change_Side_Module( (MODULE*) GetCurItem(), &dc );
        break;

    case ID_POPUP_PCB_EDIT_MODULE:

        // If the current Item is a pad, text module ...: Get its parent
        if( GetCurItem()->Type() != TYPE_MODULE )
            SetCurItem( GetCurItem()->GetParent() );
        if( !GetCurItem() || GetCurItem()->Type() != TYPE_MODULE )
            break;
        InstallModuleOptionsFrame( (MODULE*) GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_DRAG_PAD_REQUEST:
        g_Drag_Pistes_On = true;
        DrawPanel->MouseToCursorSchema();
        StartMovePad( (D_PAD*) GetCurItem(), &dc );
        break;

    case ID_POPUP_PCB_MOVE_PAD_REQUEST:
        g_Drag_Pistes_On = false;
        DrawPanel->MouseToCursorSchema();
        StartMovePad( (D_PAD*) GetCurItem(), &dc );
        break;

    case ID_POPUP_PCB_EDIT_PAD:
        InstallPadOptionsFrame( (D_PAD*) GetCurItem() );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_IMPORT_PAD_SETTINGS:
        DrawPanel->MouseToCursorSchema();
        SaveCopyInUndoList( GetCurItem()->GetParent(), UR_CHANGED );
        Import_Pad_Settings( (D_PAD*) GetCurItem(), true );
        break;

    case ID_POPUP_PCB_GLOBAL_IMPORT_PAD_SETTINGS:
        DrawPanel->MouseToCursorSchema();
        Global_Import_Pad_Settings( (D_PAD*) GetCurItem(), true );
        break;

    case ID_POPUP_PCB_EXPORT_PAD_SETTINGS:
        DrawPanel->MouseToCursorSchema();
        Export_Pad_Settings( (D_PAD*) GetCurItem() );
        break;

    case ID_POPUP_PCB_DELETE_PAD:
        SaveCopyInUndoList( GetCurItem()->GetParent(), UR_CHANGED );
        DeletePad( (D_PAD*) GetCurItem() );
        SetCurItem( NULL );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_EDIT_TEXTMODULE:
        InstallTextModOptionsFrame( (TEXTE_MODULE*) GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_MOVE_TEXTMODULE_REQUEST:
        DrawPanel->MouseToCursorSchema();
        StartMoveTexteModule( (TEXTE_MODULE*) GetCurItem(), &dc );
        break;

    case ID_POPUP_PCB_ROTATE_TEXTMODULE:
        RotateTextModule( (TEXTE_MODULE*) GetCurItem(),
                         &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_DELETE_TEXTMODULE:
        DeleteTextModule( (TEXTE_MODULE*) GetCurItem() );
        SetCurItem( NULL );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_SELECT_LAYER:
        itmp = SelectLayer( getActiveLayer(), -1, -1 );
        if( itmp >= 0 )
            setActiveLayer( itmp );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_AUX_TOOLBAR_PCB_SELECT_LAYER_PAIR:
        SelectLayerPair();
        break;

    case ID_POPUP_PCB_SELECT_NO_CU_LAYER:
        itmp = SelectLayer( getActiveLayer(), FIRST_NO_COPPER_LAYER, -1 );
        if( itmp >= 0 )
            setActiveLayer( itmp );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_SELECT_CU_LAYER:
        itmp = SelectLayer( getActiveLayer(), -1, LAST_COPPER_LAYER );
        if( itmp >= 0 )
            setActiveLayer( itmp );
        break;

    case ID_POPUP_PCB_SELECT_LAYER_PAIR:
        SelectLayerPair();
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_TOOLBARH_PCB_SELECT_LAYER:
        itmp = m_SelLayerBox->GetChoice();
        setActiveLayer( (size_t) m_SelLayerBox->wxItemContainer::GetClientData( itmp ) );
        if( DisplayOpt.ContrastModeDisplay )
            DrawPanel->Refresh( true );
        break;

    case ID_POPUP_PCB_EDIT_TEXTEPCB:
        InstallTextPCBOptionsFrame( (TEXTE_PCB*) GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_ROTATE_TEXTEPCB:
        Rotate_Texte_Pcb( (TEXTE_PCB*) GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_DELETE_TEXTEPCB:
        Delete_Texte_Pcb( (TEXTE_PCB*) GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_MOVE_MIRE_REQUEST:
        StartMove_Mire( (MIREPCB*) GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_EDIT_MIRE:
        InstallMireOptionsFrame( (MIREPCB*) GetCurItem(), &dc, pos );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_DELETE_MIRE:
        DrawPanel->MouseToCursorSchema();
        Delete_Mire( (MIREPCB*) GetCurItem(), &dc );
        SetCurItem( NULL );
        break;

    case ID_POPUP_PCB_DELETE_COTATION:
        DrawPanel->MouseToCursorSchema();
        Delete_Cotation( (COTATION*) GetCurItem(), &dc );
        SetCurItem( NULL );
        break;

    case ID_POPUP_PCB_EDIT_COTATION:
        Install_Edit_Cotation( (COTATION*) GetCurItem(), &dc, pos );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_DELETE_DRAWING:
        Delete_Segment_Edge( (DRAWSEGMENT*) GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_DELETE_MARKER:
        RemoveStruct( GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_GETINFO_MARKER:
        if( GetCurItem() && GetCurItem()->Type() == TYPE_MARKER_PCB )
            ( (MARKER_PCB*) GetCurItem() )->DisplayMarkerInfo( this );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_DELETE_DRAWING_LAYER:
        if( GetCurItem()->m_Flags != 0 )
            break;
        Delete_Drawings_All_Layer( GetCurItem()->GetLayer() );
        SetCurItem( NULL );
        DrawPanel->MouseToCursorSchema();
        DrawPanel->Refresh();
        break;

    case ID_POPUP_PCB_EDIT_DRAWING:
        InstallGraphicItemPropertiesDialog( (DRAWSEGMENT*) GetCurItem(), &dc );
        DrawPanel->MouseToCursorSchema();
        break;

    case ID_POPUP_PCB_MOVE_DRAWING_REQUEST:
        DrawPanel->MouseToCursorSchema();
        Start_Move_DrawItem( (DRAWSEGMENT*) GetCurItem(), &dc );
        break;

    case ID_POPUP_PCB_STOP_CURRENT_DRAWING:
        DrawPanel->MouseToCursorSchema();
        if( GetCurItem() && (GetCurItem()->m_Flags & IS_NEW) )
        {
            End_Edge( (DRAWSEGMENT*) GetCurItem(), &dc );
            SetCurItem( NULL );
        }
        break;

    case ID_POPUP_PCB_STOP_CURRENT_EDGE_ZONE:
        DrawPanel->MouseToCursorSchema();
        if( GetCurItem() && (GetCurItem()->m_Flags & IS_NEW) )
        {
            if( End_Zone( &dc ) )
                SetCurItem( NULL );
        }
        DrawPanel->m_AutoPAN_Request = false;
        break;

    case ID_POPUP_PCB_DELETE_ZONE_LAST_CREATED_CORNER:
        DrawPanel->MouseToCursorSchema();
        if( GetCurItem() && (GetCurItem()->m_Flags & IS_NEW) )
        {
            if( Delete_LastCreatedCorner( &dc ) == 0 )  // No more segment in outline,
                SetCurItem( NULL );
        }
        break;


    case ID_POPUP_PCB_MOVE_TRACK_SEGMENT:
        DrawPanel->MouseToCursorSchema();
        Start_MoveOneNodeOrSegment( (TRACK*) GetScreen()->GetCurItem(),
                                   &dc, id );
        break;

    case ID_POPUP_PCB_DRAG_TRACK_SEGMENT:
    case ID_POPUP_PCB_MOVE_TRACK_NODE:
        DrawPanel->MouseToCursorSchema();
        Start_MoveOneNodeOrSegment( (TRACK*) GetScreen()->GetCurItem(),
                                   &dc, id );
        break;

    case ID_POPUP_PCB_DRAG_TRACK_SEGMENT_KEEP_SLOPE:
        DrawPanel->MouseToCursorSchema();
        Start_DragTrackSegmentAndKeepSlope( (TRACK*) GetScreen()->GetCurItem(),
                                           &dc );
        break;

    case ID_POPUP_PCB_BREAK_TRACK:
        DrawPanel->MouseToCursorSchema();
        {
            TRACK*  track = (TRACK*) GetScreen()->GetCurItem();
            wxPoint pos   = GetScreen()->m_Curseur;
            track->Draw( DrawPanel, &dc, GR_XOR );
            PICKED_ITEMS_LIST itemsListPicker;
            TRACK*  newtrack = CreateLockPoint( pos, track, NULL, &itemsListPicker);
            SaveCopyInUndoList(itemsListPicker,UR_UNSPECIFIED);
            track->Draw( DrawPanel, &dc, GR_XOR );
            newtrack->Draw( DrawPanel, &dc, GR_XOR );
        }
        break;

    case ID_MENU_PCB_CLEAN:
        Clean_Pcb( &dc );
        break;

    case ID_MENU_PCB_SWAP_LAYERS:
        Swap_Layers( event );
        break;

    case ID_PCB_USER_GRID_SETUP:
        InstallGridFrame( pos );
        break;

    case ID_POPUP_PCB_DISPLAY_FOOTPRINT_DOC:
    {
        wxConfig* cfg = wxGetApp().m_EDA_CommonConfig;
        cfg->Read( wxT( "module_doc_file" ), g_DocModulesFileName );
        GetAssociatedDocument( this, g_DocModulesFileName, &wxGetApp().GetLibraryPathList() );
    }
    break;

    case ID_MENU_ARCHIVE_NEW_MODULES:
        Archive_Modules( wxEmptyString, true );
        break;

    case ID_MENU_ARCHIVE_ALL_MODULES:
        Archive_Modules( wxEmptyString, false );
        break;

    default:
        wxString msg;
        msg.Printf(
            wxT( "WinEDA_PcbFrame::Process_Special_Functions() id %d error" ),
            DrawStruct->Type() );
        DisplayError( this, msg );

        break;
    }

    SetToolbars();
    DrawPanel->CursorOn( &dc );
    DrawPanel->m_IgnoreMouseEvents = false;
}


static void Process_Move_Item( WinEDA_PcbFrame* frame,
                               EDA_BaseStruct* DrawStruct, wxDC* DC )
{
    if( DrawStruct == NULL )
        return;

    frame->DrawPanel->MouseToCursorSchema();

    switch( DrawStruct->Type() )
    {
    case TYPE_TEXTE:
        frame->StartMoveTextePcb( (TEXTE_PCB*) DrawStruct, DC );
        break;

    default:
        wxString msg;
        msg.Printf(
            wxT( "WinEDA_PcbFrame::Move_Item Error: Bad DrawType %d" ),
            DrawStruct->Type() );
        DisplayError( frame, msg );
        break;
    }
}


void WinEDA_PcbFrame::RemoveStruct( BOARD_ITEM* Item, wxDC* DC )
{
    if( Item == NULL )
        return;

    switch( Item->Type() )
    {
    case TYPE_MODULE:
        Delete_Module( (MODULE*) Item, DC, true );
        break;

    case TYPE_COTATION:
        Delete_Cotation( (COTATION*) Item, DC );
        break;

    case TYPE_MIRE:
        Delete_Mire( (MIREPCB*) Item, DC );
        break;

    case TYPE_DRAWSEGMENT:
        Delete_Segment_Edge( (DRAWSEGMENT*) Item, DC );
        break;

    case TYPE_TEXTE:
        Delete_Texte_Pcb( (TEXTE_PCB*) Item, DC );
        break;

    case TYPE_TRACK:
        Delete_Track( DC, (TRACK*) Item );
        break;

    case TYPE_VIA:
        Delete_Segment( DC, (TRACK*) Item );
        break;

    case TYPE_ZONE:
        Delete_Zone_Fill( (SEGZONE*) Item );
        break;

    case TYPE_ZONE_EDGE_CORNER:
        Remove_Zone_Corner( DC, (ZONE_CONTAINER*) Item );
        SetCurItem( NULL );
        break;

    case TYPE_ZONE_CONTAINER:
    {
        SetCurItem( NULL );
        int netcode = ( (ZONE_CONTAINER*) Item )->GetNet();
        Delete_Zone_Contour( DC, (ZONE_CONTAINER*) Item );
        test_1_net_connexion( NULL, netcode );
        GetBoard()->DisplayInfo( this );
    }
    break;

    case TYPE_MARKER_PCB:
        if( Item == GetCurItem() )
            SetCurItem( NULL );
        ( (MARKER_PCB*) Item )->Draw( DrawPanel, DC, GR_XOR );

        // delete the marker, and free memory.  Don't use undo stack.
        GetBoard()->Delete( Item );
        break;

    case TYPE_PAD:
    case TYPE_TEXTE_MODULE:
    case TYPE_EDGE_MODULE:
        break;

    case TYPE_NOT_INIT:
    case TYPE_PCB:
    default:
    {
        wxString Line;
        Line.Printf( wxT( "Remove: item type %d unknown." ),
                    Item->Type() );
        DisplayError( this, Line );
    }
    break;
    }
}


void WinEDA_PcbFrame::SwitchLayer( wxDC* DC, int layer )
{
    int curLayer = getActiveLayer();

    // Check if the specified layer matches the present layer
    if( layer == curLayer )
        return;

    // Copper layers cannot be selected unconditionally; how many
    // of those layers are currently enabled needs to be checked.
    if( IsValidCopperLayerIndex( layer ) )
    {
        // If only one copper layer is enabled, the only such layer
        // that can be selected to is the "Back" layer (so the
        // selection of any other copper layer is disregarded).
        if( GetBoard()->GetCopperLayerCount() < 2 )
        {
            if( layer != LAYER_N_BACK )
            {
                // Uncomment following command (and line 17) to beep
                // the speaker. (Doing that would provide feedback to
                // the user that the (layer-switching) command has been
                // "acknowledged", but is unable to be acted upon.)
//              wxBell();
                return;
            }
        }
        // If more than one copper layer is enabled, the "Copper"
        // and "Component" layers can be selected, but the total
        // number of copper layers determines which internal
        // layers are also capable of being selected.
        else
        {
            if( ( layer != LAYER_N_BACK ) && ( layer != LAYER_N_FRONT )
               && ( layer >= GetBoard()->GetCopperLayerCount() - 1 ) )
            {
                // Uncomment following command (and line 17) to beep
                // the speaker. (Doing that would provide feedback to
                // the user that the (layer-switching) command has been
                // "acknowledged", but is unable to be acted upon.)
//              wxBell();
                return;
            }
        }

        EDA_BaseStruct* current = GetScreen()->GetCurItem();

        // See if we are drawing a segment; if so, add a via?
        if( m_ID_current_state == ID_TRACK_BUTT && current != NULL )
        {
            if( current->Type() == TYPE_TRACK && ( current->m_Flags & IS_NEW ) )
            {
                // Want to set the routing layers so that it switches properly -
                // see the implementation of Other_Layer_Route - the working
                // layer is used to 'start' the via and set the layer masks appropriately.
                GetScreen()->m_Route_Layer_TOP    = curLayer;
                GetScreen()->m_Route_Layer_BOTTOM = layer;

                setActiveLayer( curLayer );

                if( Other_Layer_Route( (TRACK*) GetScreen()->GetCurItem(), DC ) )
                {
                    if( DisplayOpt.ContrastModeDisplay )
                        GetScreen()->SetRefreshReq();
                }
                // if the via was allowed by DRC, then the layer swap has already
                // been done by Other_Layer_Route(). if via not allowed, then
                // return now so assignment to setActiveLayer() below doesn't happen.
                return;
            }
        }
    }

    // Is yet more checking required? E.g. when the layer to be selected
    // is a non-copper layer, or when switching between a copper layer
    // and a non-copper layer, or vice-versa?
    // ...

    setActiveLayer( layer );

    if( DisplayOpt.ContrastModeDisplay )
        GetScreen()->SetRefreshReq();
}
