/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2013 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2013 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright (C) 2013-2018 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file pcb_edit_frame.cpp
 * @brief PCB editor main frame implementation.
 */

#include <fctsys.h>
#include <kiface_i.h>
#include <pgm_base.h>
#include <class_drawpanel.h>
#include <confirm.h>
#include <pcb_edit_frame.h>
#include <collectors.h>
#include <build_version.h>
#include <macros.h>
#include <3d_viewer/eda_3d_viewer.h>
#include <msgpanel.h>
#include <fp_lib_table.h>
#include <bitmaps.h>
#include <trace_helpers.h>

#include <pcbnew.h>
#include <pcbnew_id.h>
#include <drc.h>
#include <layer_widget.h>
#include <dialog_design_rules.h>
#include <pcb_layer_widget.h>
#include <hotkeys.h>
#include <config_params.h>
#include <footprint_edit_frame.h>
#include <dialog_helpers.h>
#include <dialog_plot.h>
#include <dialog_exchange_footprints.h>
#include <dialog_edit_footprint_for_BoardEditor.h>
#include <convert_to_biu.h>
#include <view/view.h>
#include <view/view_controls.h>
#include <pcb_painter.h>
#include <invoke_pcb_dialog.h>

#include <class_track.h>
#include <class_board.h>
#include <class_module.h>
#include <worksheet_viewitem.h>
#include <connectivity_data.h>
#include <ratsnest_viewitem.h>

#include <tool/tool_manager.h>
#include <tool/tool_dispatcher.h>
#include <tools/pcb_actions.h>

#include <wildcards_and_files_ext.h>
#include <kicad_string.h>

#if defined(KICAD_SCRIPTING) || defined(KICAD_SCRIPTING_WXPYTHON)
#include <python_scripting.h>
#endif

#include <pcb_draw_panel_gal.h>
#include <gal/graphics_abstraction_layer.h>
#include <functional>

using namespace std::placeholders;

///@{
/// \ingroup config

static const wxString PlotLineWidthEntry =      "PlotLineWidth_mm";
static const wxString MagneticPadsEntry =       "PcbMagPadOpt";
static const wxString MagneticTracksEntry =     "PcbMagTrackOpt";
static const wxString ShowMicrowaveEntry =      "ShowMicrowaveTools";
static const wxString ShowLayerManagerEntry =   "ShowLayerManagerTools";
static const wxString ShowPageLimitsEntry =     "ShowPageLimits";
static const wxString IconScaleEntry =          "PcbIconScale";

///@}


BEGIN_EVENT_TABLE( PCB_EDIT_FRAME, PCB_BASE_FRAME )
    EVT_SOCKET( ID_EDA_SOCKET_EVENT_SERV, PCB_EDIT_FRAME::OnSockRequestServer )
    EVT_SOCKET( ID_EDA_SOCKET_EVENT, PCB_EDIT_FRAME::OnSockRequest )

    EVT_CHOICE( ID_ON_ZOOM_SELECT, PCB_EDIT_FRAME::OnSelectZoom )
    EVT_CHOICE( ID_ON_GRID_SELECT, PCB_EDIT_FRAME::OnSelectGrid )

    EVT_CLOSE( PCB_EDIT_FRAME::OnCloseWindow )
    EVT_SIZE( PCB_EDIT_FRAME::OnSize )

    EVT_TOOL( ID_LOAD_FILE, PCB_EDIT_FRAME::Files_io )
    EVT_TOOL( ID_MENU_READ_BOARD_BACKUP_FILE, PCB_EDIT_FRAME::Files_io )
    EVT_TOOL( ID_MENU_RECOVER_BOARD_AUTOSAVE, PCB_EDIT_FRAME::Files_io )
    EVT_TOOL( ID_NEW_BOARD, PCB_EDIT_FRAME::Files_io )
    EVT_TOOL( ID_SAVE_BOARD, PCB_EDIT_FRAME::Files_io )
    EVT_TOOL( ID_OPEN_MODULE_EDITOR, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( ID_OPEN_MODULE_VIEWER, PCB_EDIT_FRAME::Process_Special_Functions )

    // Menu Files:
    EVT_MENU( ID_MAIN_MENUBAR, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_MENU_PCB_FLIP_VIEW, PCB_EDIT_FRAME::OnFlipPcbView )

    EVT_MENU( ID_APPEND_FILE, PCB_EDIT_FRAME::Files_io )
    EVT_MENU( ID_SAVE_BOARD_AS, PCB_EDIT_FRAME::Files_io )
    EVT_MENU( ID_COPY_BOARD_AS, PCB_EDIT_FRAME::Files_io )
    EVT_MENU( ID_IMPORT_NON_KICAD_BOARD, PCB_EDIT_FRAME::Files_io )
    EVT_MENU_RANGE( wxID_FILE1, wxID_FILE9, PCB_EDIT_FRAME::OnFileHistory )

    EVT_MENU( ID_GEN_PLOT, PCB_EDIT_FRAME::ToPlotter )

    EVT_MENU( ID_GEN_EXPORT_SPECCTRA, PCB_EDIT_FRAME::ExportToSpecctra )
    EVT_MENU( ID_GEN_EXPORT_FILE_GENCADFORMAT, PCB_EDIT_FRAME::ExportToGenCAD )
    EVT_MENU( ID_GEN_EXPORT_FILE_MODULE_REPORT, PCB_EDIT_FRAME::GenFootprintsReport )
    EVT_MENU( ID_GEN_EXPORT_FILE_VRML, PCB_EDIT_FRAME::OnExportVRML )
    EVT_MENU( ID_GEN_EXPORT_FILE_IDF3, PCB_EDIT_FRAME::OnExportIDF3 )
    EVT_MENU( ID_GEN_EXPORT_FILE_STEP, PCB_EDIT_FRAME::OnExportSTEP )

    EVT_MENU( ID_GEN_IMPORT_SPECCTRA_SESSION,PCB_EDIT_FRAME::ImportSpecctraSession )
    EVT_MENU( ID_GEN_IMPORT_SPECCTRA_DESIGN, PCB_EDIT_FRAME::ImportSpecctraDesign )
    EVT_MENU( ID_GEN_IMPORT_DXF_FILE, PCB_EDIT_FRAME::Process_Special_Functions )

    EVT_MENU( ID_MENU_ARCHIVE_MODULES_IN_LIBRARY, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_MENU_CREATE_LIBRARY_AND_ARCHIVE_MODULES, PCB_EDIT_FRAME::Process_Special_Functions )

    EVT_MENU( wxID_EXIT, PCB_EDIT_FRAME::OnQuit )

    // menu Config
    EVT_MENU( ID_PCB_DRAWINGS_WIDTHS_SETUP, PCB_EDIT_FRAME::OnConfigurePcbOptions )
    EVT_MENU( ID_PCB_LIB_TABLE_EDIT, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_PCB_3DSHAPELIB_WIZARD, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_PREFERENCES_CONFIGURE_PATHS, PCB_EDIT_FRAME::OnConfigurePaths )
    EVT_MENU( ID_CONFIG_SAVE, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_CONFIG_READ, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU_RANGE( ID_PREFERENCES_HOTKEY_START, ID_PREFERENCES_HOTKEY_END,
                    PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( wxID_PREFERENCES, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_PCB_LAYERS_SETUP, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_PCB_MASK_CLEARANCE, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_PCB_PAD_SETUP, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_CONFIG_SAVE, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_CONFIG_READ, PCB_EDIT_FRAME::Process_Config )
    EVT_MENU( ID_PCB_DISPLAY_OPTIONS_SETUP, PCB_EDIT_FRAME::InstallDisplayOptionsDialog )
    EVT_MENU( ID_PCB_USER_GRID_SETUP, PCB_EDIT_FRAME::Process_Special_Functions )

    // menu Postprocess
    EVT_MENU( ID_PCB_GEN_POS_MODULES_FILE, PCB_EDIT_FRAME::GenFootprintsPositionFile )
    EVT_MENU( ID_PCB_GEN_DRILL_FILE, PCB_EDIT_FRAME::InstallDrillFrame )
    EVT_MENU( ID_PCB_GEN_D356_FILE, PCB_EDIT_FRAME::GenD356File )
    EVT_MENU( ID_PCB_GEN_CMP_FILE, PCB_EDIT_FRAME::RecreateCmpFileFromBoard )
    EVT_MENU( ID_PCB_GEN_BOM_FILE_FROM_BOARD, PCB_EDIT_FRAME::RecreateBOMFileFromBoard )

    // menu Miscellaneous
    EVT_MENU( ID_MENU_LIST_NETS, PCB_EDIT_FRAME::ListNetsAndSelect )
    EVT_MENU( ID_PCB_EDIT_ALL_VIAS_AND_TRACK_SIZE, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_PCB_GLOBAL_DELETE, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_MENU_PCB_CLEAN, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_MENU_PCB_UPDATE_FOOTPRINTS, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_MENU_PCB_EXCHANGE_FOOTPRINTS, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_MENU_PCB_SWAP_LAYERS, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_MENU_PCB_RESET_TEXTMODULE_FIELDS_SIZES,
              PCB_EDIT_FRAME::OnResetModuleTextSizes )

    // Menu Help
    EVT_MENU( wxID_HELP, EDA_DRAW_FRAME::GetKicadHelp )
    EVT_MENU( wxID_INDEX, EDA_DRAW_FRAME::GetKicadHelp )
    EVT_MENU( ID_HELP_GET_INVOLVED, EDA_DRAW_FRAME::GetKicadContribute )
    EVT_MENU( wxID_ABOUT, EDA_BASE_FRAME::GetKicadAbout )

    // Menu 3D Frame
    EVT_MENU( ID_MENU_PCB_SHOW_3D_FRAME, PCB_EDIT_FRAME::Show3D_Frame )

    // Switching canvases
    EVT_MENU( ID_MENU_CANVAS_LEGACY, PCB_EDIT_FRAME::OnSwitchCanvas )
    EVT_MENU( ID_MENU_CANVAS_CAIRO, PCB_EDIT_FRAME::OnSwitchCanvas )
    EVT_MENU( ID_MENU_CANVAS_OPENGL, PCB_EDIT_FRAME::OnSwitchCanvas )

    // Menu Get Design Rules Editor
    EVT_MENU( ID_MENU_PCB_SHOW_DESIGN_RULES_DIALOG, PCB_EDIT_FRAME::ShowDesignRulesEditor )

    // Horizontal toolbar
    EVT_TOOL( ID_RUN_LIBRARY, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( ID_SHEET_SET, EDA_DRAW_FRAME::Process_PageSettings )
    EVT_TOOL( wxID_CUT, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( wxID_COPY, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( wxID_PASTE, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( wxID_UNDO, PCB_BASE_EDIT_FRAME::RestoreCopyFromUndoList )
    EVT_TOOL( wxID_REDO, PCB_BASE_EDIT_FRAME::RestoreCopyFromRedoList )
    EVT_TOOL( wxID_PRINT, PCB_EDIT_FRAME::ToPrinter )
    EVT_TOOL( ID_GEN_PLOT_SVG, PCB_EDIT_FRAME::SVG_Print )
    EVT_TOOL( ID_GEN_PLOT, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( ID_FIND_ITEMS, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( ID_GET_NETLIST, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( ID_DRC_CONTROL, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( ID_AUX_TOOLBAR_PCB_SELECT_LAYER_PAIR, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_TOOL( ID_AUX_TOOLBAR_PCB_SELECT_AUTO_WIDTH, PCB_EDIT_FRAME::Tracks_and_Vias_Size_Event )
    EVT_COMBOBOX( ID_TOOLBARH_PCB_SELECT_LAYER, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_CHOICE( ID_AUX_TOOLBAR_PCB_TRACK_WIDTH, PCB_EDIT_FRAME::Tracks_and_Vias_Size_Event )
    EVT_CHOICE( ID_AUX_TOOLBAR_PCB_VIA_SIZE, PCB_EDIT_FRAME::Tracks_and_Vias_Size_Event )


#if defined(KICAD_SCRIPTING) && defined(KICAD_SCRIPTING_ACTION_MENU)
    EVT_TOOL( ID_TOOLBARH_PCB_ACTION_PLUGIN_REFRESH, PCB_EDIT_FRAME::OnActionPluginRefresh )
#endif

#if defined( KICAD_SCRIPTING_WXPYTHON )
    // has meaning only with KICAD_SCRIPTING_WXPYTHON enabled
    EVT_TOOL( ID_TOOLBARH_PCB_SCRIPTING_CONSOLE, PCB_EDIT_FRAME::ScriptingConsoleEnableDisable )
    EVT_UPDATE_UI( ID_TOOLBARH_PCB_SCRIPTING_CONSOLE,
                   PCB_EDIT_FRAME::OnUpdateScriptingConsoleState )
#endif

    // Option toolbar
    EVT_TOOL( ID_TB_OPTIONS_DRC_OFF,
                    PCB_EDIT_FRAME::OnSelectOptionToolbar )
    EVT_TOOL( ID_TB_OPTIONS_SHOW_RATSNEST,
                    PCB_EDIT_FRAME::OnSelectOptionToolbar )
    EVT_TOOL( ID_TB_OPTIONS_SHOW_VIAS_SKETCH,
                    PCB_EDIT_FRAME::OnSelectOptionToolbar )
    EVT_TOOL( ID_TB_OPTIONS_SHOW_TRACKS_SKETCH,
                    PCB_EDIT_FRAME::OnSelectOptionToolbar )
    EVT_TOOL( ID_TB_OPTIONS_SHOW_HIGH_CONTRAST_MODE,
                    PCB_EDIT_FRAME::OnSelectOptionToolbar )
    EVT_TOOL( ID_TB_OPTIONS_SHOW_EXTRA_VERTICAL_TOOLBAR_MICROWAVE,
                    PCB_EDIT_FRAME::OnSelectOptionToolbar )

    EVT_TOOL( ID_UPDATE_PCB_FROM_SCH, PCB_EDIT_FRAME::OnUpdatePCBFromSch )

    EVT_TOOL_RANGE( ID_TB_OPTIONS_SHOW_ZONES, ID_TB_OPTIONS_SHOW_ZONES_OUTLINES_ONLY,
                    PCB_EDIT_FRAME::OnSelectOptionToolbar )

    EVT_TOOL( ID_TB_OPTIONS_SHOW_MANAGE_LAYERS_VERTICAL_TOOLBAR,
              PCB_EDIT_FRAME::OnSelectOptionToolbar )

    // Vertical main toolbar:
    EVT_TOOL( ID_NO_TOOL_SELECTED, PCB_EDIT_FRAME::OnSelectTool )
    EVT_TOOL( ID_ZOOM_SELECTION, PCB_EDIT_FRAME::OnSelectTool )
    EVT_TOOL_RANGE( ID_PCB_HIGHLIGHT_BUTT, ID_PCB_MEASUREMENT_TOOL,
                    PCB_EDIT_FRAME::OnSelectTool )

    EVT_TOOL_RANGE( ID_PCB_MUWAVE_START_CMD, ID_PCB_MUWAVE_END_CMD,
                    PCB_EDIT_FRAME::ProcessMuWaveFunctions )

    EVT_MENU_RANGE( ID_POPUP_PCB_START_RANGE, ID_POPUP_PCB_END_RANGE,
                    PCB_EDIT_FRAME::Process_Special_Functions )

    // Tracks and vias sizes general options
    EVT_MENU_RANGE( ID_POPUP_PCB_SELECT_WIDTH_START_RANGE,
                    ID_POPUP_PCB_SELECT_WIDTH_END_RANGE,
                    PCB_EDIT_FRAME::Tracks_and_Vias_Size_Event )

    // popup menus
    EVT_MENU( ID_POPUP_PCB_DELETE_TRACKSEG, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU_RANGE( ID_POPUP_GENERAL_START_RANGE, ID_POPUP_GENERAL_END_RANGE,
                    PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_POPUP_PCB_SPREAD_ALL_MODULES, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_POPUP_PCB_SPREAD_NEW_MODULES, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_POPUP_PCB_AUTOPLACE_FIXE_MODULE, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_POPUP_PCB_AUTOPLACE_FIXE_ALL_MODULES, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_POPUP_PCB_AUTOPLACE_FREE_ALL_MODULES, PCB_EDIT_FRAME::Process_Special_Functions )
    EVT_MENU( ID_POPUP_PCB_AUTOPLACE_FREE_MODULE, PCB_EDIT_FRAME::Process_Special_Functions )

    // User interface update event handlers.
    EVT_UPDATE_UI( ID_SAVE_BOARD, PCB_EDIT_FRAME::OnUpdateSave )
    EVT_UPDATE_UI( ID_AUX_TOOLBAR_PCB_SELECT_LAYER_PAIR, PCB_EDIT_FRAME::OnUpdateLayerPair )
    EVT_UPDATE_UI( ID_TOOLBARH_PCB_SELECT_LAYER, PCB_EDIT_FRAME::OnUpdateLayerSelectBox )
    EVT_UPDATE_UI( ID_TB_OPTIONS_DRC_OFF, PCB_EDIT_FRAME::OnUpdateDrcEnable )
    EVT_UPDATE_UI( ID_TB_OPTIONS_SHOW_RATSNEST, PCB_EDIT_FRAME::OnUpdateShowBoardRatsnest )
    EVT_UPDATE_UI( ID_TB_OPTIONS_SHOW_VIAS_SKETCH, PCB_EDIT_FRAME::OnUpdateViaDrawMode )
    EVT_UPDATE_UI( ID_TB_OPTIONS_SHOW_TRACKS_SKETCH, PCB_EDIT_FRAME::OnUpdateTraceDrawMode )
    EVT_UPDATE_UI( ID_TB_OPTIONS_SHOW_HIGH_CONTRAST_MODE,
                   PCB_EDIT_FRAME::OnUpdateHighContrastDisplayMode )
    EVT_UPDATE_UI( ID_TB_OPTIONS_SHOW_MANAGE_LAYERS_VERTICAL_TOOLBAR,
                   PCB_EDIT_FRAME::OnUpdateShowLayerManager )
    EVT_UPDATE_UI( ID_TB_OPTIONS_SHOW_EXTRA_VERTICAL_TOOLBAR_MICROWAVE,
                   PCB_EDIT_FRAME::OnUpdateShowMicrowaveToolbar )
    EVT_UPDATE_UI( ID_NO_TOOL_SELECTED, PCB_EDIT_FRAME::OnUpdateVerticalToolbar )
    EVT_UPDATE_UI( ID_ZOOM_SELECTION, PCB_EDIT_FRAME::OnUpdateVerticalToolbar )
    EVT_UPDATE_UI( ID_AUX_TOOLBAR_PCB_TRACK_WIDTH, PCB_EDIT_FRAME::OnUpdateSelectTrackWidth )
    EVT_UPDATE_UI( ID_AUX_TOOLBAR_PCB_SELECT_AUTO_WIDTH,
                   PCB_EDIT_FRAME::OnUpdateSelectAutoTrackWidth )
    EVT_UPDATE_UI( ID_POPUP_PCB_SELECT_AUTO_WIDTH, PCB_EDIT_FRAME::OnUpdateSelectAutoTrackWidth )
    EVT_UPDATE_UI( ID_POPUP_PCB_SELECT_CUSTOM_WIDTH,
                   PCB_EDIT_FRAME::OnUpdateSelectCustomTrackWidth )
    EVT_UPDATE_UI( ID_AUX_TOOLBAR_PCB_VIA_SIZE, PCB_EDIT_FRAME::OnUpdateSelectViaSize )
    EVT_UPDATE_UI_RANGE( ID_POPUP_PCB_SELECT_WIDTH1, ID_POPUP_PCB_SELECT_WIDTH8,
                         PCB_EDIT_FRAME::OnUpdateSelectTrackWidth )
    EVT_UPDATE_UI_RANGE( ID_POPUP_PCB_SELECT_VIASIZE1, ID_POPUP_PCB_SELECT_VIASIZE8,
                         PCB_EDIT_FRAME::OnUpdateSelectViaSize )
    EVT_UPDATE_UI_RANGE( ID_PCB_HIGHLIGHT_BUTT, ID_PCB_MEASUREMENT_TOOL,
                         PCB_EDIT_FRAME::OnUpdateVerticalToolbar )
    EVT_UPDATE_UI_RANGE( ID_PCB_MUWAVE_START_CMD, ID_PCB_MUWAVE_END_CMD,
                         PCB_EDIT_FRAME::OnUpdateMuWaveToolbar )

    EVT_COMMAND( wxID_ANY, LAYER_WIDGET::EVT_LAYER_COLOR_CHANGE, PCB_EDIT_FRAME::OnLayerColorChange )
END_EVENT_TABLE()


PCB_EDIT_FRAME::PCB_EDIT_FRAME( KIWAY* aKiway, wxWindow* aParent ) :
    PCB_BASE_EDIT_FRAME( aKiway, aParent, FRAME_PCB, wxT( "Pcbnew" ), wxDefaultPosition,
        wxDefaultSize, KICAD_DEFAULT_DRAWFRAME_STYLE, PCB_EDIT_FRAME_NAME )
{
    m_showBorderAndTitleBlock = true;   // true to display sheet references
    m_showAxis = false;                 // true to display X and Y axis
    m_showOriginAxis = true;
    m_showGridAxis = true;
    m_SelTrackWidthBox = NULL;
    m_SelViaSizeBox = NULL;
    m_SelLayerBox = NULL;
    m_show_microwave_tools = false;
    m_show_layer_manager_tools = true;
    m_hotkeysDescrList = g_Board_Editor_Hotkeys_Descr;
    m_hasAutoSave = true;
    m_microWaveToolBar = NULL;
    m_Layers = nullptr;

    // We don't know what state board was in when it was lasat saved, so we have to
    // assume dirty
    m_ZoneFillsDirty = true;

    m_rotationAngle = 900;

    // Create GAL canvas
    EDA_DRAW_PANEL_GAL* galCanvas = new PCB_DRAW_PANEL_GAL( this, -1, wxPoint( 0, 0 ),
                                                m_FrameSize,
                                                GetGalDisplayOptions(),
                                                EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE );

    SetGalCanvas( galCanvas );

    SetBoard( new BOARD() );

    // Create the PCB_LAYER_WIDGET *after* SetBoard():
    m_Layers = new PCB_LAYER_WIDGET( this, GetCanvas() );

    m_drc = new DRC( this );        // these 2 objects point to each other

    wxIcon  icon;
    icon.CopyFromBitmap( KiBitmap( icon_pcbnew_xpm ) );
    SetIcon( icon );

    // LoadSettings() *after* creating m_LayersManager, because LoadSettings()
    // initialize parameters in m_LayersManager
    LoadSettings( config() );

    SetScreen( new PCB_SCREEN( GetPageSettings().GetSizeIU() ) );
    GetScreen()->SetMaxUndoItems( m_UndoRedoCountMax );

    // PCB drawings start in the upper left corner.
    GetScreen()->m_Center = false;

    SetSize( m_FramePos.x, m_FramePos.y, m_FrameSize.x, m_FrameSize.y );

    GetScreen()->AddGrid( m_UserGridSize, EDA_UNITS_T::UNSCALED_UNITS, ID_POPUP_GRID_USER );
    GetScreen()->SetGrid( ID_POPUP_GRID_LEVEL_1000 + m_LastGridSizeId  );

    if( m_canvas )
        m_canvas->SetEnableBlockCommands( true );

    ReCreateMenuBar();
    ReCreateHToolbar();
    ReCreateAuxiliaryToolbar();
    ReCreateVToolbar();
    ReCreateOptToolbar();

    ReCreateMicrowaveVToolbar();

    m_auimgr.SetManagedWindow( this );

    EDA_PANEINFO horiz;
    horiz.HorizontalToolbarPane();

    EDA_PANEINFO vert;
    vert.VerticalToolbarPane();

    EDA_PANEINFO mesg;
    mesg.MessageToolbarPane();

    // Create a wxAuiPaneInfo for the Layers Manager, not derived from the template.
    // LAYER_WIDGET is floatable, but initially docked at far right
    EDA_PANEINFO   lyrs;
    lyrs.LayersToolbarPane();
    lyrs.MinSize( m_Layers->GetBestSize() );    // updated in ReFillLayerWidget
    lyrs.BestSize( m_Layers->GetBestSize() );
    lyrs.Caption( _( "Layers Manager" ) );
    lyrs.TopDockable( false ).BottomDockable( false );

    if( m_mainToolBar )    // The main horizontal toolbar
    {
        m_auimgr.AddPane( m_mainToolBar,
                          wxAuiPaneInfo( horiz ).Name( wxT( "m_mainToolBar" ) ).Top().Row( 0 ) );
    }

    if( m_auxiliaryToolBar )    // the auxiliary horizontal toolbar, that shows track and via sizes, zoom ...)
    {
        m_auimgr.AddPane( m_auxiliaryToolBar,
                          wxAuiPaneInfo( horiz ).Name( wxT( "m_auxiliaryToolBar" ) ).Top().Row( 1 ) );
    }

    if( m_microWaveToolBar )    // The auxiliary vertical right toolbar (currently microwave tools)
        m_auimgr.AddPane( m_microWaveToolBar,
                          wxAuiPaneInfo( vert ).Name( wxT( "m_microWaveToolBar" ) ).
                          Right().Layer( 1 ).Position(1).Hide() );

    if( m_drawToolBar )    // The main right vertical toolbar
        m_auimgr.AddPane( m_drawToolBar,
                          wxAuiPaneInfo( vert ).Name( wxT( "m_VToolBar" ) ).Right().Layer( 2 ) );

    // Add the layer manager ( most right side of pcbframe )
    m_auimgr.AddPane( m_Layers, lyrs.Name( wxT( "m_LayersManagerToolBar" ) ).Right().Layer( 3 ) );

    if( m_optionsToolBar )    // The left vertical toolbar (fast acces display options of Pcbnew)
    {
        m_auimgr.AddPane( m_optionsToolBar,
                          wxAuiPaneInfo( vert ).Name( wxT( "m_optionsToolBar" ) ).Left().Layer(1) );

        m_auimgr.GetPane( wxT( "m_LayersManagerToolBar" ) ).Show( m_show_layer_manager_tools );
        m_auimgr.GetPane( wxT( "m_microWaveToolBar" ) ).Show( m_show_microwave_tools );
    }

    if( m_canvas )
        m_auimgr.AddPane( m_canvas,
                          wxAuiPaneInfo().Name( wxT( "DrawFrame" ) ).CentrePane() );

    if( GetGalCanvas() )
        m_auimgr.AddPane( (wxWindow*) GetGalCanvas(),
                          wxAuiPaneInfo().Name( wxT( "DrawFrameGal" ) ).CentrePane().Hide() );

    if( m_messagePanel )
        m_auimgr.AddPane( m_messagePanel,
                          wxAuiPaneInfo( mesg ).Name( wxT( "MsgPanel" ) ).Bottom().Layer(10) );

    ReFillLayerWidget();        // this is near end because contents establish size
    m_Layers->ReFillRender();   // Update colors in Render after the config is read
    syncLayerWidgetLayer();

    m_auimgr.Update();

    setupTools();

    Zoom_Automatique( false );

    EDA_DRAW_PANEL_GAL::GAL_TYPE canvasType = loadCanvasTypeSetting();

    // Nudge user to switch to OpenGL if they are on legacy or Cairo
    if( m_firstRunDialogSetting < 1 )
    {
        if( canvasType != EDA_DRAW_PANEL_GAL::GAL_TYPE_OPENGL )
        {
            wxString msg = _( "KiCad can use your graphics card to give you a smoother "
                              "and faster experience. This option is turned off by "
                              "default since it is not compatible with all computers.\n\n"
                              "Would you like to try enabling graphics acceleration?\n\n"
                              "If you'd like to choose later, select Modern Toolset "
                              "(Accelerated) in the Preferences menu." );

            wxMessageDialog dlg( this, msg, _( "Enable Graphics Acceleration" ),
                                 wxYES_NO );

            dlg.SetYesNoLabels( _( "&Enable Acceleration" ), _( "&No Thanks" ) );

            if( dlg.ShowModal() == wxID_YES )
            {
                // Save Cairo as default in case OpenGL crashes
                saveCanvasTypeSetting( EDA_DRAW_PANEL_GAL::GAL_TYPE_CAIRO );

                // Switch to OpenGL, which will save the new setting if successful
                wxCommandEvent evt( wxEVT_MENU, ID_MENU_CANVAS_OPENGL );
                GetEventHandler()->ProcessEvent( evt );

                // Switch back to Cairo if OpenGL is not supported
                if( GetGalCanvas()->GetBackend() == EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE )
                {
                    wxCommandEvent cairoEvt( wxEVT_MENU, ID_MENU_CANVAS_CAIRO );
                    GetEventHandler()->ProcessEvent( cairoEvt );
                }
            }
            else if( canvasType == EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE )
            {
                // If they were on legacy, switch them to Cairo
                wxCommandEvent evt( wxEVT_MENU, ID_MENU_CANVAS_CAIRO );
                GetEventHandler()->ProcessEvent( evt );
            }
        }

        m_firstRunDialogSetting = 1;
        SaveSettings( config() );
    }
    else
    {
        if( canvasType != EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE )
        {
            if( GetGalCanvas()->SwitchBackend( canvasType ) )
                UseGalCanvas( true );
        }
    }

    enableGALSpecificMenus();

    // disable Export STEP item if kicad2step does not exist
    wxString strK2S = Pgm().GetExecutablePath();

#ifdef __WXMAC__
    if (strK2S.find( "pcbnew.app" ) != wxNOT_FOUND )
    {
        // On macOS, we have standalone applications inside the main bundle, so we handle that here:
        strK2S += "../../";
    }

    strK2S += "Contents/MacOS/";
#endif

    wxFileName appK2S( strK2S, "kicad2step" );

    #ifdef _WIN32
    appK2S.SetExt( "exe" );
    #endif

    if( !appK2S.FileExists() )
        GetMenuBar()->FindItem( ID_GEN_EXPORT_FILE_STEP )->Enable( false );
}


PCB_EDIT_FRAME::~PCB_EDIT_FRAME()
{
    delete m_drc;
}


void PCB_EDIT_FRAME::SetBoard( BOARD* aBoard )
{
    PCB_BASE_EDIT_FRAME::SetBoard( aBoard );

    if( IsGalCanvasActive() )
    {
        aBoard->GetConnectivity()->Build( aBoard );

        // reload the worksheet
        SetPageSettings( aBoard->GetPageSettings() );
    }
}


BOARD_ITEM_CONTAINER* PCB_EDIT_FRAME::GetModel() const
{
    return m_Pcb;
}


void PCB_EDIT_FRAME::SetPageSettings( const PAGE_INFO& aPageSettings )
{
    PCB_BASE_FRAME::SetPageSettings( aPageSettings );

    if( IsGalCanvasActive() )
    {
        PCB_DRAW_PANEL_GAL* drawPanel = static_cast<PCB_DRAW_PANEL_GAL*>( GetGalCanvas() );

        // Prepare worksheet template
        KIGFX::WORKSHEET_VIEWITEM* worksheet;
        worksheet = new KIGFX::WORKSHEET_VIEWITEM( IU_PER_MILS ,&m_Pcb->GetPageSettings(),
                                                   &m_Pcb->GetTitleBlock() );
        worksheet->SetSheetName( std::string( GetScreenDesc().mb_str() ) );

        BASE_SCREEN* screen = GetScreen();

        if( screen != NULL )
        {
            worksheet->SetSheetNumber( screen->m_ScreenNumber );
            worksheet->SetSheetCount( screen->m_NumberOfScreens );
        }

        // PCB_DRAW_PANEL_GAL takes ownership of the worksheet
        drawPanel->SetWorksheet( worksheet );
    }
}


bool PCB_EDIT_FRAME::isAutoSaveRequired() const
{
    if( GetScreen() )
        return GetScreen()->IsSave();

    return false;
}


void PCB_EDIT_FRAME::setupTools()
{
    // Create the manager and dispatcher & route draw panel events to the dispatcher
    m_toolManager = new TOOL_MANAGER;
    m_toolManager->SetEnvironment( m_Pcb, GetGalCanvas()->GetView(),
                                   GetGalCanvas()->GetViewControls(), this );
    m_actions = new PCB_ACTIONS();
    m_toolDispatcher = new TOOL_DISPATCHER( m_toolManager, m_actions );

    // Register tools
    m_actions->RegisterAllTools( m_toolManager );
    m_toolManager->InitTools();

    // Run the selection tool, it is supposed to be always active
    m_toolManager->InvokeTool( "pcbnew.InteractiveSelection" );
}


void PCB_EDIT_FRAME::ReFillLayerWidget()
{
    m_Layers->ReFill();

    wxAuiPaneInfo& lyrs = m_auimgr.GetPane( m_Layers );

    wxSize bestz = m_Layers->GetBestSize();

    lyrs.MinSize( bestz );
    lyrs.BestSize( bestz );
    lyrs.FloatingSize( bestz );

    if( lyrs.IsDocked() )
        m_auimgr.Update();
    else
        m_Layers->SetSize( bestz );
}


void PCB_EDIT_FRAME::OnQuit( wxCommandEvent& event )
{
    Close( false );
}


void PCB_EDIT_FRAME::OnCloseWindow( wxCloseEvent& Event )
{
    m_canvas->SetAbortRequest( true );

    if( GetScreen()->IsModify() && !GetBoard()->IsEmpty() )
    {
        wxString msg = wxString::Format( _(
                "Save the changes in\n"
                "\"%s\"\n"
                "before closing?" ),
                GetChars( GetBoard()->GetFileName() )
                );

        int ii = DisplayExitDialog( this, msg );
        switch( ii )
        {
        case wxID_CANCEL:
            Event.Veto();
            return;

        case wxID_NO:
            break;

        case wxID_YES:
            // save the board. if the board has no name,
            // the ID_SAVE_BOARD_AS will actually made
            Files_io_from_id( ID_SAVE_BOARD );
            break;
        }
    }

    if( IsGalCanvasActive() )
    {
        // On Windows 7 / 32 bits, on OpenGL mode only, Pcbnew crashes
        // when closing this frame if a footprint was selected, and the footprint editor called
        // to edit this footprint, and when closing pcbnew if this footprint is still selected
        // See https://bugs.launchpad.net/kicad/+bug/1655858
        // I think this is certainly a OpenGL event fired after frame deletion, so this workaround
        // avoid the crash (JPC)
        GetGalCanvas()->SetEvtHandlerEnabled( false );
    }

    GetGalCanvas()->StopDrawing();

    // Delete the auto save file if it exists.
    wxFileName fn = GetBoard()->GetFileName();

    // Auto save file name is the normal file name prefixed with '_autosave'.
    fn.SetName( GetAutoSaveFilePrefix() + fn.GetName() );

    // When the auto save feature does not have write access to the board file path, it falls
    // back to a platform specific user temporary file path.
    if( !fn.IsOk() || !fn.IsDirWritable() )
        fn.SetPath( wxFileName::GetTempDir() );

    wxLogTrace( traceAutoSave, "Deleting auto save file <" + fn.GetFullPath() + ">" );

    // Remove the auto save file on a normal close of Pcbnew.
    if( fn.FileExists() && !wxRemoveFile( fn.GetFullPath() ) )
    {
        wxString msg = wxString::Format( _(
                "The auto save file \"%s\" could not be removed!" ),
                GetChars( fn.GetFullPath() )
                );

        wxMessageBox( msg, Pgm().App().GetAppName(), wxOK | wxICON_ERROR, this );
    }

    // Delete board structs and undo/redo lists, to avoid crash on exit
    // when deleting some structs (mainly in undo/redo lists) too late
    Clear_Pcb( false );

    // do not show the window because ScreenPcb will be deleted and we do not
    // want any paint event
    Show( false );

    Destroy();
}


void PCB_EDIT_FRAME::Show3D_Frame( wxCommandEvent& event )
{
    bool forceRecreateIfNotOwner = true;
    CreateAndShow3D_Frame( forceRecreateIfNotOwner );
}


void PCB_EDIT_FRAME::UseGalCanvas( bool aEnable )
{
    if( !aEnable )
        Compile_Ratsnest( NULL, true );

    PCB_BASE_EDIT_FRAME::UseGalCanvas( aEnable );
    COLORS_DESIGN_SETTINGS& cds = Settings().Colors();

    if( aEnable )
    {
        cds.SetLegacyMode( false );
        GetGalCanvas()->GetGAL()->SetGridColor( cds.GetLayerColor( LAYER_GRID ) );
        auto view = GetGalCanvas()->GetView();
        view->GetPainter()->GetSettings()->ImportLegacyColors( &cds );
        GetGalCanvas()->Refresh();
    }

    enableGALSpecificMenus();

    // Force colors to be legacy-compatible in case they were changed in GAL
    if( !aEnable )
    {
        cds.SetLegacyMode( true );
        Refresh();
    }

    // Re-create the layer manager to allow arbitrary colors when GAL is enabled
    UpdateUserInterface();
}


void PCB_EDIT_FRAME::enableGALSpecificMenus()
{
    // some menus are active only in GAL mode and do nothing in legacy mode.
    // So enable or disable them, depending on the display mode

    ReCreateMenuBar();

    if( GetMenuBar() )
    {
        // Enable / disable some menus which are usable only on GAL
        pcbnew_ids id_list[] =
        {
            ID_MENU_INTERACTIVE_ROUTER_SETTINGS,
            ID_DIFF_PAIR_BUTT,
            ID_TUNE_SINGLE_TRACK_LEN_BUTT,
            ID_TUNE_DIFF_PAIR_LEN_BUTT,
            ID_TUNE_DIFF_PAIR_SKEW_BUTT,
            ID_MENU_DIFF_PAIR_DIMENSIONS,
            ID_MENU_PCB_FLIP_VIEW
        };

        bool enbl = IsGalCanvasActive();

        for( unsigned ii = 0; ii < DIM( id_list ); ii++ )
        {
            if( GetMenuBar()->FindItem( id_list[ii] ) )
                GetMenuBar()->FindItem( id_list[ii] )->Enable( enbl );
        }

        // Update settings for GAL menus
        auto view = GetGalCanvas()->GetView();
        GetMenuBar()->FindItem( ID_MENU_PCB_FLIP_VIEW )->Check( view->IsMirroredX() );
    }
}


void PCB_EDIT_FRAME::ShowDesignRulesEditor( wxCommandEvent& event )
{
    DIALOG_DESIGN_RULES dR_editor( this );
    int returncode = dR_editor.ShowModal();

    if( returncode == wxID_OK )     // New rules, or others changes.
    {
        ReCreateLayerBox();
        ReCreateAuxiliaryToolbar();
        m_Layers->ReFillRender();
        OnModify();

        if( GetGalCanvas() )
        {
            GetGalCanvas()->GetView()->RecacheAllItems();
            GetGalCanvas()->GetView()->MarkTargetDirty( KIGFX::TARGET_NONCACHED );
        }
        GetCanvas()->Refresh();
    }
}


void PCB_EDIT_FRAME::LoadSettings( wxConfigBase* aCfg )
{
    PCB_BASE_FRAME::LoadSettings( aCfg );

    wxConfigLoadSetups( aCfg, GetConfigurationSettings() );

    m_configSettings.Load( aCfg );

    double dtmp;
    aCfg->Read( PlotLineWidthEntry, &dtmp, 0.1 ); // stored in mm

    if( dtmp < 0.01 )
        dtmp = 0.01;

    if( dtmp > 5.0 )
        dtmp = 5.0;

    g_DrawDefaultLineThickness = Millimeter2iu( dtmp );

    aCfg->Read( ShowMicrowaveEntry, &m_show_microwave_tools );
    aCfg->Read( ShowLayerManagerEntry, &m_show_layer_manager_tools );
    aCfg->Read( ShowPageLimitsEntry, &m_showPageLimits );
}


void PCB_EDIT_FRAME::SaveSettings( wxConfigBase* aCfg )
{
    m_configSettings.Save( aCfg );

    PCB_BASE_FRAME::SaveSettings( aCfg );

    wxConfigSaveSetups( aCfg, GetConfigurationSettings() );

    // This value is stored in mm )
    aCfg->Write( PlotLineWidthEntry, MM_PER_IU * g_DrawDefaultLineThickness );
    aCfg->Write( ShowMicrowaveEntry, (long) m_show_microwave_tools );
    aCfg->Write( ShowLayerManagerEntry, (long)m_show_layer_manager_tools );
    aCfg->Write( ShowPageLimitsEntry, m_showPageLimits );
}


bool PCB_EDIT_FRAME::IsGridVisible() const
{
    return IsElementVisible( LAYER_GRID );
}


void PCB_EDIT_FRAME::SetGridVisibility(bool aVisible)
{
    SetElementVisibility( LAYER_GRID, aVisible );
}


COLOR4D PCB_EDIT_FRAME::GetGridColor()
{
    return Settings().Colors().GetItemColor( LAYER_GRID );
}


void PCB_EDIT_FRAME::SetGridColor( COLOR4D aColor )
{

    Settings().Colors().SetItemColor( LAYER_GRID, aColor );

    if( IsGalCanvasActive() )
    {
        GetGalCanvas()->GetGAL()->SetGridColor( aColor );
    }
}


bool PCB_EDIT_FRAME::IsMicroViaAcceptable()
{
    int copperlayercnt = GetBoard()->GetCopperLayerCount( );
    PCB_LAYER_ID currLayer = GetActiveLayer();

    if( !GetDesignSettings().m_MicroViasAllowed )
        return false;   // Obvious..

    if( copperlayercnt < 4 )
        return false;   // Only on multilayer boards..

    if( ( currLayer == B_Cu )
       || ( currLayer == F_Cu )
       || ( currLayer == copperlayercnt - 2 )
       || ( currLayer == In1_Cu ) )
        return true;

    return false;
}


void PCB_EDIT_FRAME::SetActiveLayer( PCB_LAYER_ID aLayer )
{
    PCB_BASE_FRAME::SetActiveLayer( aLayer );

    syncLayerWidgetLayer();

    if( IsGalCanvasActive() )
    {
        m_toolManager->RunAction( PCB_ACTIONS::layerChanged );       // notify other tools
        GetGalCanvas()->SetFocus();                 // otherwise hotkeys are stuck somewhere

        GetGalCanvas()->SetHighContrastLayer( aLayer );
        GetGalCanvas()->Refresh();
    }
}


void PCB_EDIT_FRAME::onBoardLoaded()
{
    UpdateTitle();

    // Re-create layers manager based on layer info in board
    ReFillLayerWidget();
    ReCreateLayerBox();

    // Sync layer and item visibility
    syncLayerVisibilities();
    syncLayerWidgetLayer();
    syncRenderStates();

    // Update the tracks / vias available sizes list:
    ReCreateAuxiliaryToolbar();

    // Update the RATSNEST items, which were not loaded at the time
    // BOARD::SetVisibleElements() was called from within any PLUGIN.
    // See case LAYER_RATSNEST: in BOARD::SetElementVisibility()
    GetBoard()->SetVisibleElements( GetBoard()->GetVisibleElements() );

    // Display the loaded board:
    Zoom_Automatique( false );

    Refresh();

    SetMsgPanel( GetBoard() );
    SetStatusText( wxEmptyString );
}


void PCB_EDIT_FRAME::syncLayerWidgetLayer()
{
    m_Layers->SelectLayer( GetActiveLayer() );
    m_Layers->OnLayerSelected();
}


void PCB_EDIT_FRAME::syncRenderStates()
{
    m_Layers->ReFillRender();
}


void PCB_EDIT_FRAME::syncLayerVisibilities()
{
    m_Layers->SyncLayerVisibilities();
    static_cast<PCB_DRAW_PANEL_GAL*>( GetGalCanvas() )->SyncLayersVisibility( m_Pcb );
}


void PCB_EDIT_FRAME::OnUpdateLayerAlpha( wxUpdateUIEvent & )
{
    m_Layers->SyncLayerAlphaIndicators();
}


void PCB_EDIT_FRAME::unitsChangeRefresh()
{
    PCB_BASE_FRAME::unitsChangeRefresh();    // Update the grid size select box.

    ReCreateAuxiliaryToolbar();
}


bool PCB_EDIT_FRAME::IsElementVisible( GAL_LAYER_ID aElement ) const
{
    return GetBoard()->IsElementVisible( aElement );
}


void PCB_EDIT_FRAME::SetElementVisibility( GAL_LAYER_ID aElement, bool aNewState )
{
    GetGalCanvas()->GetView()->SetLayerVisible( aElement , aNewState );
    GetBoard()->SetElementVisibility( aElement, aNewState );
    m_Layers->SetRenderState( aElement, aNewState );
}


void PCB_EDIT_FRAME::SetVisibleAlls()
{
    GetBoard()->SetVisibleAlls();

    for( GAL_LAYER_ID ii = GAL_LAYER_ID_START; ii < GAL_LAYER_ID_BITMASK_END; ++ii )
        m_Layers->SetRenderState( ii, true );
}


void PCB_EDIT_FRAME::ShowChangedLanguage()
{
    // call my base class
    PCB_BASE_FRAME::ShowChangedLanguage();

    // update the layer manager
    m_Layers->Freeze();

    wxAuiPaneInfo& pane_info = m_auimgr.GetPane( m_Layers );
    pane_info.Caption( _( "Visibles" ) );
    m_auimgr.Update();

    m_Layers->SetLayersManagerTabsText();
    ReFillLayerWidget();
    m_Layers->ReFillRender();

    // upate the layer widget to match board visibility states, both layers and render columns.
    syncLayerVisibilities();
    syncLayerWidgetLayer();
    syncRenderStates();

    m_Layers->Thaw();

    // pcbnew-specific toolbars
    ReCreateMicrowaveVToolbar();
}


wxString PCB_EDIT_FRAME::GetLastNetListRead()
{
    wxFileName absoluteFileName = m_lastNetListRead;
    wxFileName pcbFileName = GetBoard()->GetFileName();

    if( !absoluteFileName.MakeAbsolute( pcbFileName.GetPath() ) || !absoluteFileName.FileExists() )
    {
        absoluteFileName.Clear();
        m_lastNetListRead = wxEmptyString;
    }

    return absoluteFileName.GetFullPath();
}


void PCB_EDIT_FRAME::SetLastNetListRead( const wxString& aLastNetListRead )
{
    wxFileName relativeFileName = aLastNetListRead;
    wxFileName pcbFileName = GetBoard()->GetFileName();

    if( relativeFileName.MakeRelativeTo( pcbFileName.GetPath() )
        && relativeFileName.GetFullPath() != aLastNetListRead )
    {
        m_lastNetListRead = relativeFileName.GetFullPath();
    }
}


void PCB_EDIT_FRAME::OnModify( )
{
    PCB_BASE_FRAME::OnModify();

    Update3DView();

    m_ZoneFillsDirty = true;
}


void PCB_EDIT_FRAME::SVG_Print( wxCommandEvent& event )
{
    PCB_PLOT_PARAMS  plot_prms = GetPlotSettings();

    // we don't want dialogs knowing about complex wxFrame functions so
    // pass everything the dialog needs without reference to *this frame's class.
    if( InvokeSVGPrint( this, GetBoard(), &plot_prms ) )
    {
        if( !plot_prms.IsSameAs( GetPlotSettings(), false ) )
        {
            // First, mark board as modified only for parameters saved in file
            if( !plot_prms.IsSameAs( GetPlotSettings(), true ) )
                OnModify();

            // Now, save any change, for the session
            SetPlotSettings( plot_prms );
        }
    }
}


void PCB_EDIT_FRAME::UpdateTitle()
{
    wxFileName fileName = GetBoard()->GetFileName();
    wxString fileinfo;

    if( fileName.IsOk() && fileName.FileExists() )
    {
        fileinfo = fileName.IsFileWritable() ? wxString( wxEmptyString ) : _( " [Read Only]" );
    }
    else
    {
        fileinfo = _( " [new file]" );
    }

    wxString title;
    title.Printf( _( "Pcbnew" ) + wxT( " \u2014 %s%s" ),
                  fileName.GetFullPath(),
                  fileinfo );

    SetTitle( title );
}


void PCB_EDIT_FRAME::UpdateUserInterface()
{
    // Update the layer manager and other widgets from the board setup
    // (layer and items visibility, colors ...)

    // Rebuild list of nets (full ratsnest rebuild)
    Compile_Ratsnest( NULL, true );
    GetBoard()->BuildConnectivity();

    // Update info shown by the horizontal toolbars
    ReCreateLayerBox();

    // Update the layer manager
    m_Layers->Freeze();
    ReFillLayerWidget();
    m_Layers->ReFillRender();

    // upate the layer widget to match board visibility states, both layers and render columns.
    syncLayerVisibilities();
    syncLayerWidgetLayer();
    syncRenderStates();

    m_Layers->Thaw();
}


#if defined( KICAD_SCRIPTING_WXPYTHON )

void PCB_EDIT_FRAME::ScriptingConsoleEnableDisable( wxCommandEvent& aEvent )
{

    wxWindow * pythonPanelFrame = findPythonConsole();
    bool pythonPanelShown = true;

    if( pythonPanelFrame == NULL )
        pythonPanelFrame = CreatePythonShellWindow( this, pythonConsoleNameId() );
    else
        pythonPanelShown = ! pythonPanelFrame->IsShown();

    if( pythonPanelFrame )
        pythonPanelFrame->Show( pythonPanelShown );
    else
        wxMessageBox( wxT( "Error: unable to create the Python Console" ) );
}
#endif


void PCB_EDIT_FRAME::OnLayerColorChange( wxCommandEvent& aEvent )
{
    ReCreateLayerBox();
}


void PCB_EDIT_FRAME::OnSwitchCanvas( wxCommandEvent& aEvent )
{
    // switches currently used canvas (default / Cairo / OpenGL).
    PCB_BASE_FRAME::OnSwitchCanvas( aEvent );

    // The base class method reinit the layers manager.
    // We must upate the layer widget to match board visibility states,
    // both layers and render columns.
    syncLayerVisibilities();
    syncLayerWidgetLayer();
    syncRenderStates();
}

void PCB_EDIT_FRAME::ToPlotter( wxCommandEvent& event )
{
    // Force rebuild the dialog if currently open because the old dialog can be not up to date
    // if the board (or units) has changed
    wxWindow* dlg =  wxWindow::FindWindowByName( DLG_WINDOW_NAME );

    if( dlg )
        dlg->Destroy();

    dlg = new DIALOG_PLOT( this );
    dlg->Show( true );
}


bool PCB_EDIT_FRAME::SetCurrentNetClass( const wxString& aNetClassName )
{
    bool change = GetDesignSettings().SetCurrentNetClass( aNetClassName );

    if( change )
    {
        ReCreateAuxiliaryToolbar();
    }

    return change;
}


void PCB_EDIT_FRAME::OnConfigurePaths( wxCommandEvent& aEvent )
{
    Pgm().ConfigurePaths( this );
}


void PCB_EDIT_FRAME::OnUpdatePCBFromSch( wxCommandEvent& event )
{
    if( Kiface().IsSingle() )
    {
        DisplayError( this,  _( "Cannot update the PCB, because Pcbnew is "
                                "opened in stand-alone mode. In order to create or update "
                                "PCBs from schematics, you need to launch the KiCad project manager "
                                "and create a PCB project." ) );
        return;
    }
    else
    {
        // Update PCB requires a netlist. Therefore the schematic editor must be running
        // If this is not the case, open the schematic editor
        KIWAY_PLAYER* frame = Kiway().Player( FRAME_SCH, true );

        if( !frame->IsShown() )
        {
            wxFileName schfn( Prj().GetProjectPath(), Prj().GetProjectName(), SchematicFileExtension );

            frame->OpenProjectFiles( std::vector<wxString>( 1, schfn.GetFullPath() ) );
            // Because the schematic editor frame is not on screen, iconize it:
            // However, another valid option is to do not iconize the schematic editor frame
            // and show it
            frame->Iconize( true );
            // we show the schematic editor frame, because do not show is seen as
            // a not yet opened schematic by Kicad manager, which is not the case
            frame->Show( true );
        }

        Kiway().ExpressMail( FRAME_SCH, MAIL_SCH_PCB_UPDATE_REQUEST, "", this );
    }
}


void PCB_EDIT_FRAME::OnFlipPcbView( wxCommandEvent& evt )
{
    auto view = GetGalCanvas()->GetView();
    view->SetMirror( evt.IsChecked(), false );
    view->RecacheAllItems();
    Refresh();
}


void PCB_EDIT_FRAME::PythonPluginsReload()
{
    // Reload Python plugins if they are newer than
    // the already loaded, and load new plugins
#if defined(KICAD_SCRIPTING)
    //Reload plugin list: reload Python plugins if they are newer than
    // the already loaded, and load new plugins
    PythonPluginsReloadBase();

    #if defined(KICAD_SCRIPTING_ACTION_MENU)
        // Action plugins can be modified, therefore the plugins menu
        // must be updated:
        RebuildActionPluginMenus();
    #endif
#endif
}


void PCB_EDIT_FRAME::InstallFootprintPropertiesDialog( MODULE* Module, wxDC* DC )
{
    if( Module == NULL )
        return;

#ifdef __WXMAC__
    // avoid Avoid "writes" in the dialog, creates errors with WxOverlay and NSView & Modal
    // Raising an Exception - Fixes #764678
    DC = NULL;
#endif

    DIALOG_FOOTPRINT_BOARD_EDITOR* dlg = new DIALOG_FOOTPRINT_BOARD_EDITOR( this, Module, DC );

    int retvalue = dlg->ShowModal();
    /* retvalue =
     *  FP_PRM_EDITOR_RETVALUE::PRM_EDITOR_ABORT if abort,
     *  FP_PRM_EDITOR_RETVALUE::PRM_EDITOR_WANT_EXCHANGE_FP if exchange module,
     *  FP_PRM_EDITOR_RETVALUE::PRM_EDITOR_EDIT_OK for normal edition
     *  FP_PRM_EDITOR_RETVALUE::PRM_EDITOR_WANT_MODEDIT for a goto editor command
     */

    dlg->Close();
    dlg->Destroy();

#ifdef __WXMAC__
    // If something edited, push a refresh request
    if( retvalue == DIALOG_FOOTPRINT_BOARD_EDITOR::PRM_EDITOR_EDIT_OK )
        m_canvas->Refresh();
#endif

    if( retvalue == DIALOG_FOOTPRINT_BOARD_EDITOR::PRM_EDITOR_WANT_MODEDIT )
    {
        FOOTPRINT_EDIT_FRAME* editor = (FOOTPRINT_EDIT_FRAME*) Kiway().Player( FRAME_PCB_MODULE_EDITOR, true );

        editor->Load_Module_From_BOARD( Module );
        SetCurItem( NULL );

        editor->Show( true );
        editor->Raise();        // Iconize( false );
    }

    if( retvalue == DIALOG_FOOTPRINT_BOARD_EDITOR::PRM_EDITOR_WANT_UPDATE_FP )
    {
        InstallExchangeModuleFrame( Module, true );
    }

    if( retvalue == DIALOG_FOOTPRINT_BOARD_EDITOR::PRM_EDITOR_WANT_EXCHANGE_FP )
    {
        InstallExchangeModuleFrame( Module, false );
    }
}


int PCB_EDIT_FRAME::InstallExchangeModuleFrame( MODULE* Module, bool updateMode )
{
    DIALOG_EXCHANGE_FOOTPRINTS dialog( this, Module, updateMode );

    return dialog.ShowQuasiModal();
}


int PCB_EDIT_FRAME::GetIconScale()
{
    int scale = 0;
    Kiface().KifaceSettings()->Read( IconScaleEntry, &scale, 0 );
    return scale;
}


void PCB_EDIT_FRAME::SetIconScale( int aScale )
{
    Kiface().KifaceSettings()->Write( IconScaleEntry, aScale );
    ReCreateMenuBar();
    ReCreateHToolbar();
    ReCreateAuxiliaryToolbar();
    ReCreateVToolbar();
    ReCreateOptToolbar();
    ReCreateMicrowaveVToolbar();
    Layout();
    SendSizeEvent();
}


void PCB_EDIT_FRAME::LockModule( MODULE* aModule, bool aLocked )
{
    const wxString ModulesMaskSelection = wxT( "*" );
    if( aModule )
    {
        aModule->SetLocked( aLocked );
        SetMsgPanel( aModule );
        OnModify();
    }
    else
    {
        aModule = GetBoard()->m_Modules;

        for( ; aModule != NULL; aModule = aModule->Next() )
        {
            if( WildCompareString( ModulesMaskSelection, aModule->GetReference() ) )
            {
                aModule->SetLocked( aLocked );
                OnModify();
            }
        }
    }
}
