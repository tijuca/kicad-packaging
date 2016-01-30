/***************************************************/
/*  tool_gerber.cpp: Build tool bars and main menu */
/***************************************************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "common.h"
#include "macros.h"
#include "gerbview.h"
#include "pcbplot.h"
#include "protos.h"
#include "bitmaps.h"
#include "gerbview_id.h"
#include "hotkeys.h"


void WinEDA_GerberFrame::ReCreateMenuBar( void )
{
    wxMenuBar  *menuBar = GetMenuBar();

    /* Destroy the existing menu bar so it can be rebuilt.  This allows
     * language changes of the menu text on the fly. */
    if( menuBar )
        SetMenuBar( NULL );

    menuBar = new wxMenuBar();

    wxMenu* filesMenu = new wxMenu;
    filesMenu->Append( wxID_FILE, _( "Load Gerber File" ),
                       _( "Load a new Gerber file on the current layer" ),
                       FALSE );

    filesMenu->Append( ID_APPEND_FILE, _( "Append Gerber File to Current Layer" ),
                       _( "Append a new Gerber file to the current layer" ),
                       FALSE );

    filesMenu->Append( ID_MENU_INC_LAYER_AND_APPEND_FILE,
                       _( "Inc Layer and load Gerber file" ),
                       _( "Increment layer number, and Load Gerber file" ),
                       FALSE );

    filesMenu->Append( ID_GERBVIEW_LOAD_DCODE_FILE, _( "Load DCodes" ),
                       _( "Load D-Codes File" ), FALSE );
#if 0
    filesMenu->Append( ID_GERBVIEW_LOAD_DRILL_FILE, _( "Load EXCELLON Drill File" ),
                       _( "Load excellon drill file" ), FALSE );
#endif

    filesMenu->Append( ID_NEW_BOARD, _( "&Clear All" ),
                       _( "Clear all layers" ), FALSE );

    filesMenu->AppendSeparator();
    filesMenu->Append( ID_GERBVIEW_EXPORT_TO_PCBNEW,  _( "&Export to Pcbnew" ),
                       _( "Export data in pcbnew format" ), FALSE );

#if 0
    filesMenu->AppendSeparator();
    filesMenu->Append( ID_SAVE_BOARD, _( "&Save Layers" ),
                       _( "Save current layers (GERBER format)" ), FALSE );

    filesMenu->Append( ID_SAVE_BOARD_AS, _( "Save Layers As..." ),
                       _( "Save current layers as.." ), FALSE );
#endif

    filesMenu->AppendSeparator();

    filesMenu->Append( ID_GEN_PRINT, _( "P&rint" ), _( "Print gerber" ) );
    filesMenu->Append( ID_GEN_PLOT, _( "Plot" ),
                       _( "Plotting in various formats" ) );

    filesMenu->AppendSeparator();
    filesMenu->Append( ID_EXIT, _( "E&xit" ), _( "Quit Gerbview" ) );

    wxGetApp().m_fileHistory.AddFilesToMenu( filesMenu );

    // Configuration and preferences:
    wxMenu* configmenu = new wxMenu;
    ADD_MENUITEM_WITH_HELP( configmenu, ID_CONFIG_REQ, _( "&File Ext" ),
                            _( "Set files extensions" ), config_xpm );
    ADD_MENUITEM_WITH_HELP( configmenu, ID_MENU_GERBVIEW_SHOW_HIDE_LAYERS_MANAGER_DIALOG,
                            _( "Hide &Layers Manager" ),
                            _( "Show/hide the layers manager toolbar" ),
                            layers_manager_xpm );
    ADD_MENUITEM_WITH_HELP( configmenu, ID_OPTIONS_SETUP, _( "&Options" ),
                            _( "Select general options" ), preference_xpm );

    ADD_MENUITEM_WITH_HELP( configmenu, ID_GERBVIEW_DISPLAY_OPTIONS_SETUP,
                            _( "Display" ),
                            _( "Select how items are displayed" ),
                            display_options_xpm );

    wxGetApp().AddMenuLanguageList( configmenu );

    configmenu->AppendSeparator();
    ADD_MENUITEM_WITH_HELP( configmenu, ID_CONFIG_SAVE, _( "&Save Setup" ),
                            _( "Save application preferences" ),
                            save_setup_xpm );

    configmenu->AppendSeparator();
    AddHotkeyConfigMenu( configmenu );

    wxMenu* miscellaneous_menu = new wxMenu;
    ADD_MENUITEM_WITH_HELP( miscellaneous_menu, ID_GERBVIEW_SHOW_LIST_DCODES,
                            _( "&List DCodes" ),
                            _( "List and edit D-codes" ), show_dcodenumber_xpm );
    ADD_MENUITEM_WITH_HELP( miscellaneous_menu, ID_GERBVIEW_SHOW_SOURCE,
                            _( "&Show Source" ),
                            _( "Show source file for the current layer" ),
                            tools_xpm );
    miscellaneous_menu->AppendSeparator();
    ADD_MENUITEM_WITH_HELP( miscellaneous_menu, ID_GERBVIEW_GLOBAL_DELETE,
                            _( "&Delete Layer" ),
                            _( "Delete current layer" ), general_deletions_xpm );

    // Menu Help:
    wxMenu* helpMenu = new wxMenu;
    ADD_MENUITEM_WITH_HELP( helpMenu, ID_GENERAL_HELP, _( "&Contents" ),
                            _( "Open the gerbview manual" ), help_xpm );
    ADD_MENUITEM_WITH_HELP( helpMenu, ID_KICAD_ABOUT, _( "&About Gerbview" ),
                            _( "About gerbview gerber and drill viewer" ),
                            online_help_xpm );

    menuBar->Append( filesMenu, _( "&File" ) );
    menuBar->Append( configmenu, _( "&Preferences" ) );
    menuBar->Append( miscellaneous_menu, _( "&Miscellaneous" ) );

//      menuBar->Append(drill_menu, _("&Drill"));
    menuBar->Append( helpMenu, _( "&Help" ) );

    // Associate the menu bar with the frame
    SetMenuBar( menuBar );
}


void WinEDA_GerberFrame::ReCreateHToolbar( void )
{
    int           layer = 0;
    GERBER*       gerber = NULL;
    int           ii;
    wxString      msg;

    // delete and recreate the toolbar
    if( m_HToolBar != NULL )
        return;

    if( GetScreen() )
    {
        layer = GetScreen()->m_Active_Layer;
        gerber = g_GERBER_List[layer];
    }

    m_HToolBar = new WinEDA_Toolbar( TOOLBAR_MAIN, this, ID_H_TOOLBAR, TRUE );

    // Set up toolbar
    m_HToolBar->AddTool( ID_NEW_BOARD, wxEmptyString,
                          wxBitmap( new_xpm ),
                         _( "New world" ) );

    m_HToolBar->AddTool( wxID_FILE, wxEmptyString,
                         wxBitmap( open_xpm ),
                         _( "Open existing Layer" ) );


    m_HToolBar->AddSeparator();


    m_HToolBar->AddTool( wxID_UNDO, wxEmptyString,
                         wxBitmap( undelete_xpm ),
                         _( "Undelete" ) );

    m_HToolBar->AddSeparator();
    m_HToolBar->AddTool( ID_GEN_PRINT, wxEmptyString,
                         wxBitmap( print_button ),
                         _( "Print world" ) );

    m_HToolBar->AddSeparator();
    msg = AddHotkeyName( _( "Zoom in" ), s_Gerbview_Hokeys_Descr, HK_ZOOM_IN );
    m_HToolBar->AddTool( ID_ZOOM_IN, wxEmptyString,
                         wxBitmap( zoom_in_xpm ),
                         msg );

    msg = AddHotkeyName( _( "Zoom out" ), s_Gerbview_Hokeys_Descr, HK_ZOOM_OUT );
    m_HToolBar->AddTool( ID_ZOOM_OUT, wxEmptyString,
                         wxBitmap( zoom_out_xpm ),
                         msg );

    msg = AddHotkeyName( _( "Redraw view" ), s_Gerbview_Hokeys_Descr,
                         HK_ZOOM_REDRAW );
    m_HToolBar->AddTool( ID_ZOOM_REDRAW, wxEmptyString,
                         wxBitmap( zoom_redraw_xpm ),
                         msg );

    msg = AddHotkeyName( _( "Zoom auto" ), s_Gerbview_Hokeys_Descr,
                         HK_ZOOM_AUTO );
    m_HToolBar->AddTool( ID_ZOOM_PAGE, wxEmptyString,
                         wxBitmap( zoom_auto_xpm ),
                         msg );

    m_HToolBar->AddSeparator();
    m_HToolBar->AddTool( ID_FIND_ITEMS, wxEmptyString,
                         wxBitmap( find_xpm ),
                         _( "Find D-codes" ) );

    wxArrayString choices;
    m_HToolBar->AddSeparator();
    for( ii = 0; ii < 32; ii++ )
    {
        wxString msg;
        msg = _( "Layer " ); msg << ii + 1;
        choices.Add( msg );
    }

    m_SelLayerBox = new WinEDAChoiceBox( m_HToolBar,
                                         ID_TOOLBARH_GERBVIEW_SELECT_LAYER,
                                         wxDefaultPosition, wxSize( 150, -1 ),
                                         choices );
    m_SelLayerBox->SetSelection( getActiveLayer() );
    m_HToolBar->AddControl( m_SelLayerBox );

    m_HToolBar->AddSeparator();
    choices.Clear();
    choices.Add( _( "No tool" ) );

    for( ii = 0; ii < MAX_TOOLS; ii++ )
    {
        wxString msg;
        msg = _( "Tool " ); msg << ii + FIRST_DCODE;
        choices.Add( msg );
    }

    m_SelLayerTool = new WinEDAChoiceBox( m_HToolBar,
                                          ID_TOOLBARH_GERBER_SELECT_TOOL,
                                          wxDefaultPosition, wxSize( 150, -1 ),
                                          choices );
    m_HToolBar->AddControl( m_SelLayerTool );


    // after adding the buttons to the toolbar, must call Realize() to reflect
    // the changes
    m_HToolBar->Realize();
    SetToolbars();
}


/**
 * Create or update the right vertical toolbar
 */
void WinEDA_GerberFrame::ReCreateVToolbar( void )
{
    if( m_VToolBar )
        return;

    m_VToolBar = new WinEDA_Toolbar( TOOLBAR_TOOL, this, ID_V_TOOLBAR, FALSE );

    // Set up toolbar
    m_VToolBar->AddTool( ID_NO_SELECT_BUTT, wxEmptyString,
                         wxBitmap( cursor_xpm ) );
    m_VToolBar->ToggleTool( ID_NO_SELECT_BUTT, TRUE );
    m_VToolBar->AddSeparator();
    m_VToolBar->AddTool( ID_GERBVIEW_DELETE_ITEM_BUTT, wxEmptyString,
                         wxBitmap( delete_body_xpm ),
                         _( "Delete items" ) );

    m_VToolBar->Realize();
    SetToolbars();
}


/**
 * Create or update the left vertical toolbar (option toolbar
 */
void WinEDA_GerberFrame::ReCreateOptToolbar( void )
{
    if( m_OptionsToolBar )
        return;

    // creation of tool bar options
    m_OptionsToolBar = new WinEDA_Toolbar( TOOLBAR_OPTION, this,
                                           ID_OPT_TOOLBAR, FALSE );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_GRID, wxEmptyString,
                                wxBitmap( grid_xpm ),
                               _( "Turn grid off" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_POLAR_COORD, wxEmptyString,
                               wxBitmap( polar_coord_xpm ),
                               _( "Turn polar coordinate on" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_UNIT_INCH, wxEmptyString,
                               wxBitmap( unit_inch_xpm ),
                               _( "Set units to inches" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_UNIT_MM, wxEmptyString,
                               wxBitmap( unit_mm_xpm ),
                               _( "Set units to millimeters" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_CURSOR, wxEmptyString,
                               wxBitmap( cursor_shape_xpm ),
                               _( "Change cursor shape" ), wxITEM_CHECK );

    m_OptionsToolBar->AddSeparator();
    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_PADS_SKETCH, wxEmptyString,
                               wxBitmap( pad_sketch_xpm ),
                               _( "Show spots in sketch mode" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_TRACKS_SKETCH, wxEmptyString,
                               wxBitmap( showtrack_xpm ),
                               _( "Show lines in sketch mode" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_POLYGONS_SKETCH, wxEmptyString,
                               wxBitmap( opt_show_polygon_xpm ),
                               _( "Show polygons in sketch mode" ),
                               wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_DCODES, wxEmptyString,
                               wxBitmap( show_dcodenumber_xpm ),
                               _( "Show dcode number" ), wxITEM_CHECK );

    // Tools to show/hide toolbars:
    m_OptionsToolBar->AddSeparator();
    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_LAYERS_MANAGER_VERTICAL_TOOLBAR,
                                    wxEmptyString,
                                    wxBitmap( layers_manager_xpm ),
                                    _(
                                    "Show/hide the layers manager toolbar" ),
                                    wxITEM_CHECK );


    m_OptionsToolBar->Realize();
    SetToolbars();
}
