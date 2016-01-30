/******************/
/*  tool_sch.cpp  */
/******************/

#include "fctsys.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"
#include "bitmaps.h"
#include "wxEeschemaStruct.h"

#include "general.h"
#include "protos.h"
#include "hotkeys.h"
#include "eeschema_id.h"

#include "help_common_strings.h"


/* Create  the main Horizontal Toolbar for the schematic editor
 */
void SCH_EDIT_FRAME::ReCreateHToolbar()
{
    if( m_HToolBar != NULL )
        return;

    wxString msg;
    m_HToolBar = new EDA_TOOLBAR( TOOLBAR_MAIN, this, ID_H_TOOLBAR, TRUE );

    // Set up toolbar
    m_HToolBar->AddTool( ID_NEW_PROJECT, wxEmptyString, wxBitmap( new_xpm ),
                         _( "New schematic project" ) );

    m_HToolBar->AddTool( ID_LOAD_PROJECT, wxEmptyString, wxBitmap( open_document_xpm ),
                         _( "Open schematic project" ) );

    m_HToolBar->AddTool( ID_SAVE_PROJECT, wxEmptyString, wxBitmap( save_project_xpm ),
                         _( "Save schematic project" ) );

    m_HToolBar->AddSeparator();
    m_HToolBar->AddTool( ID_SHEET_SET, wxEmptyString, wxBitmap( sheetset_xpm ),
                         _( "Page settings" ) );

    m_HToolBar->AddSeparator();
    m_HToolBar->AddTool( ID_TO_LIBRARY, wxEmptyString, wxBitmap( libedit_xpm ),
                         _( "Library editor" ) );

    m_HToolBar->AddTool( ID_TO_LIBVIEW, wxEmptyString, wxBitmap( library_browse_xpm ),
                         _( "Library browser" ) );

    m_HToolBar->AddSeparator();
    m_HToolBar->AddTool( ID_HIERARCHY, wxEmptyString, wxBitmap( hierarchy_nav_xpm ),
                         _( "Navigate schematic hierarchy" ) );

    m_HToolBar->AddSeparator();

    m_HToolBar->AddTool( wxID_CUT, wxEmptyString, wxBitmap( cut_button ),
                         _( "Cut selected item" ) );

    m_HToolBar->AddTool( wxID_COPY, wxEmptyString, wxBitmap( copy_button ),
                         _( "Copy selected item" ) );

    m_HToolBar->AddTool( wxID_PASTE, wxEmptyString, wxBitmap( paste_xpm ),
                         _( "Paste" ) );

    m_HToolBar->AddSeparator();
    msg = AddHotkeyName( HELP_UNDO, s_Schematic_Hokeys_Descr, HK_UNDO, false );
    m_HToolBar->AddTool( wxID_UNDO, wxEmptyString, wxBitmap( undo_xpm ), msg );

    msg = AddHotkeyName( HELP_REDO, s_Schematic_Hokeys_Descr, HK_REDO, false );
    m_HToolBar->AddTool( wxID_REDO, wxEmptyString, wxBitmap( redo_xpm ), msg );

    m_HToolBar->AddSeparator();
    m_HToolBar->AddTool( wxID_PRINT, wxEmptyString, wxBitmap( print_button ),
                         _( "Print schematic" ) );

    m_HToolBar->AddSeparator();
    m_HToolBar->AddTool( ID_TO_CVPCB, wxEmptyString, wxBitmap( cvpcb_xpm ),
                         _( "Run Cvpcb" ) );

    m_HToolBar->AddTool( ID_TO_PCB, wxEmptyString, wxBitmap( pcbnew_xpm ),
                         _( "Run pcbnew" ) );

    m_HToolBar->AddSeparator();
    msg = AddHotkeyName( HELP_ZOOM_IN, s_Schematic_Hokeys_Descr, HK_ZOOM_IN, false );
    m_HToolBar->AddTool( ID_ZOOM_IN, wxEmptyString, wxBitmap( zoom_in_xpm ), msg );

    msg = AddHotkeyName( HELP_ZOOM_OUT, s_Schematic_Hokeys_Descr, HK_ZOOM_OUT, false );
    m_HToolBar->AddTool( ID_ZOOM_OUT, wxEmptyString, wxBitmap( zoom_out_xpm ), msg );

    msg = AddHotkeyName( HELP_ZOOM_REDRAW, s_Schematic_Hokeys_Descr, HK_ZOOM_REDRAW, false );
    m_HToolBar->AddTool( ID_ZOOM_REDRAW, wxEmptyString, wxBitmap( zoom_redraw_xpm ), msg );

    msg = AddHotkeyName( HELP_ZOOM_FIT, s_Schematic_Hokeys_Descr, HK_ZOOM_AUTO, false );
    m_HToolBar->AddTool( ID_ZOOM_PAGE, wxEmptyString, wxBitmap( zoom_fit_in_page_xpm ), msg );

    m_HToolBar->AddSeparator();
    msg = AddHotkeyName( HELP_FIND, s_Schematic_Hokeys_Descr, HK_FIND_ITEM, false );
    m_HToolBar->AddTool( ID_FIND_ITEMS, wxEmptyString, wxBitmap( find_xpm ), msg );

    m_HToolBar->AddSeparator();
    m_HToolBar->AddTool( ID_GET_NETLIST, wxEmptyString, wxBitmap( netlist_xpm ),
                         _( "Generate netlist" ) );

    m_HToolBar->AddTool( ID_GET_ANNOTATE, wxEmptyString, wxBitmap( annotate_xpm ),
                         _( "Annotate schematic" ) );

    m_HToolBar->AddTool( ID_GET_ERC, wxEmptyString, wxBitmap( erc_xpm ),
                         _( "Perform electric rules check" ) );

    m_HToolBar->AddTool( ID_GET_TOOLS, wxEmptyString, wxBitmap( tools_xpm ),
                         _( "Generate bill of materials and/or cross references" ) );

    m_HToolBar->AddTool( ID_BACKANNO_ITEMS, wxEmptyString, wxBitmap( import_footprint_names_xpm ),
                         _( "Back annotate component foot prints" ) );

    // after adding the tools to the toolbar, must call Realize() to reflect the changes
    m_HToolBar->Realize();
}


/* Create Vertical Right Toolbar
 */
void SCH_EDIT_FRAME::ReCreateVToolbar()
{
    if( m_VToolBar )
        return;

    m_VToolBar = new EDA_TOOLBAR( TOOLBAR_TOOL, this, ID_V_TOOLBAR, false );

    // Set up toolbar
    m_VToolBar->AddTool( ID_NO_TOOL_SELECTED, wxEmptyString, wxBitmap( cursor_xpm ),
                         wxEmptyString, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_HIERARCHY_PUSH_POP_BUTT, wxEmptyString,
                         wxBitmap( hierarchy_cursor_xpm ),
                         _( "Ascend or descend hierarchy" ), wxITEM_CHECK );

    m_VToolBar->AddTool( ID_SCH_PLACE_COMPONENT, wxEmptyString, wxBitmap( add_component_xpm ),
                         HELP_PLACE_COMPONENTS, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_PLACE_POWER_BUTT, wxEmptyString, wxBitmap( add_power_xpm ),
                         HELP_PLACE_POWERPORT, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_WIRE_BUTT, wxEmptyString, wxBitmap( add_line_xpm ),
                         HELP_PLACE_WIRE, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_BUS_BUTT, wxEmptyString, wxBitmap( add_bus_xpm ),
                         HELP_PLACE_BUS, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_WIRETOBUS_ENTRY_BUTT, wxEmptyString, wxBitmap( add_line2bus_xpm ),
                         HELP_PLACE_WIRE2BUS_ENTRY, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_BUSTOBUS_ENTRY_BUTT, wxEmptyString, wxBitmap( add_bus2bus_xpm ),
                         HELP_PLACE_BUS2BUS_ENTRY, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_NOCONN_BUTT, wxEmptyString, wxBitmap( noconn_button ),
                         HELP_PLACE_NC_FLAG, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_LABEL_BUTT, wxEmptyString, wxBitmap( add_line_label_xpm ),
                         HELP_PLACE_NETLABEL, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_GLABEL_BUTT, wxEmptyString, wxBitmap( add_glabel_xpm ),
                         HELP_PLACE_GLOBALLABEL, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_JUNCTION_BUTT, wxEmptyString, wxBitmap( add_junction_xpm ),
                         HELP_PLACE_JUNCTION, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_HIERLABEL_BUTT, wxEmptyString, wxBitmap( add_hierarchical_label_xpm ),
                         HELP_PLACE_HIER_LABEL, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_SHEET_SYMBOL_BUTT, wxEmptyString,
                         wxBitmap( add_hierarchical_subsheet_xpm ),
                         HELP_PLACE_SHEET, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_IMPORT_HLABEL_BUTT, wxEmptyString,
                         wxBitmap( import_hierarchical_label_xpm ),
                         HELP_IMPORT_SHEETPIN, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_SHEET_PIN_BUTT, wxEmptyString,
                         wxBitmap( add_hierar_pin_xpm ),
                         HELP_PLACE_SHEETPIN, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_LINE_COMMENT_BUTT, wxEmptyString,
                         wxBitmap( add_dashed_line_xpm ),
                         HELP_PLACE_GRAPHICLINES, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_TEXT_COMMENT_BUTT, wxEmptyString, wxBitmap( add_text_xpm ),
                         HELP_PLACE_GRAPHICTEXTS, wxITEM_CHECK );

    m_VToolBar->AddTool( ID_SCHEMATIC_DELETE_ITEM_BUTT, wxEmptyString, wxBitmap( delete_body_xpm ),
                         HELP_DELETE_ITEMS, wxITEM_CHECK );

    m_VToolBar->Realize();
}


/* Create Vertical Left Toolbar (Option Toolbar)
 */
void SCH_EDIT_FRAME::ReCreateOptToolbar()
{
    if( m_OptionsToolBar )
        return;

    m_OptionsToolBar = new EDA_TOOLBAR( TOOLBAR_OPTION, this, ID_OPT_TOOLBAR, false );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_GRID, wxEmptyString,
                               wxBitmap( grid_xpm ),
                               _( "Turn grid off" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_UNIT_INCH, wxEmptyString,
                               wxBitmap( unit_inch_xpm ),
                               _( "Units in inches" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_UNIT_MM, wxEmptyString,
                               wxBitmap( unit_mm_xpm ),
                               _( "Units in millimeters" ), wxITEM_CHECK );

    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_CURSOR, wxEmptyString,
                               wxBitmap( cursor_shape_xpm ),
                               _( "Change cursor shape" ), wxITEM_CHECK );

    m_OptionsToolBar->AddSeparator();
    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_HIDDEN_PINS, wxEmptyString,
                               wxBitmap( hidden_pin_xpm ),
                               _( "Show hidden pins" ), wxITEM_CHECK );

    m_OptionsToolBar->AddSeparator();
    m_OptionsToolBar->AddTool( ID_TB_OPTIONS_BUS_WIRES_ORIENT, wxEmptyString,
                               wxBitmap( lines90_xpm ),
                               _( "HV orientation for wires and bus" ),
                               wxITEM_CHECK );

    m_OptionsToolBar->Realize();
}


void SCH_EDIT_FRAME::OnSelectOptionToolbar( wxCommandEvent& event )
{
    if( DrawPanel == NULL )
        return;

    int id = event.GetId();

    switch( id )
    {
    case ID_TB_OPTIONS_HIDDEN_PINS:
        m_ShowAllPins = m_OptionsToolBar->GetToolState( id );
        DrawPanel->Refresh( );
        break;

    case ID_TB_OPTIONS_BUS_WIRES_ORIENT:
        g_HVLines = m_OptionsToolBar->GetToolState( id );
        break;

    default:
        wxFAIL_MSG( wxT( "Unexpected select option tool bar ID." ) );
        break;
    }
}
