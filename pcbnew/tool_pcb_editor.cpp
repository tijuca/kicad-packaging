/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2012 Jean-Pierre Charras, jean-pierre.charras@ujf-grenoble.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2012=2015 Wayne Stambaugh <stambaughw@verizon.net>
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
 * @file tool_pcb_editor.cpp
 * @brief PCB editor toolbars build/rebuild functions
 */

#include <fctsys.h>
#include <kiface_i.h>
#include <help_common_strings.h>
#include <dialog_helpers.h>
#include <pcb_edit_frame.h>
#include <class_drawpanel.h>
#include <confirm.h>
#include <bitmaps.h>

#include <class_board.h>

#include <pcbnew.h>
#include <pcbnew_id.h>
#include <hotkeys.h>
#include <pcb_layer_box_selector.h>

#include <wx/wupdlock.h>
#include <memory>

extern bool IsWxPythonLoaded();

#define SEL_LAYER_HELP _( \
        "Show active layer selections\nand select layer pair for route and place via" )


/* Data to build the layer pair indicator button */
static std::unique_ptr<wxBitmap> LayerPairBitmap;

#define BM_LAYERICON_SIZE 24
static const char s_BitmapLayerIcon[BM_LAYERICON_SIZE][BM_LAYERICON_SIZE] =
{
    // 0 = draw pixel with active layer color
    // 1 = draw pixel with top layer color (top/bottom layer used inautoroute and place via)
    // 2 = draw pixel with bottom layer color
    // 3 = draw pixel with via color
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0 },
    { 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 0, 1, 1, 1, 1, 3, 3, 2, 2, 2, 2, 2, 2, 2 },
    { 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 1, 1, 1, 1, 0, 3, 3, 2, 2, 2, 2, 2, 2, 2 },
    { 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 1, 1, 1, 1, 0, 3, 3, 2, 2, 2, 2, 2, 2, 2 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 1, 1, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};


void PCB_EDIT_FRAME::PrepareLayerIndicator()
{
    int        ii, jj;
    COLOR4D    active_layer_color, Route_Layer_TOP_color,
               Route_Layer_BOTTOM_color, via_color, background_color;
    bool       change = false;

    static int previous_requested_scale;
    static COLOR4D previous_active_layer_color, previous_Route_Layer_TOP_color,
                   previous_Route_Layer_BOTTOM_color, previous_via_color,
                   previous_background_color;

    const int  requested_scale = GetIconScale();

    if( requested_scale != previous_requested_scale )
    {
        previous_requested_scale = requested_scale;
        change = true;
    }

    active_layer_color = Settings().Colors().GetLayerColor(GetActiveLayer());

    if( previous_active_layer_color != active_layer_color )
    {
        previous_active_layer_color = active_layer_color;
        change = true;
    }

    Route_Layer_TOP_color =
        Settings().Colors().GetLayerColor( GetScreen()->m_Route_Layer_TOP );

    if( previous_Route_Layer_TOP_color != Route_Layer_TOP_color )
    {
        previous_Route_Layer_TOP_color = Route_Layer_TOP_color;
        change = true;
    }

    Route_Layer_BOTTOM_color =
        Settings().Colors().GetLayerColor( GetScreen()->m_Route_Layer_BOTTOM );

    if( previous_Route_Layer_BOTTOM_color != Route_Layer_BOTTOM_color )
    {
        previous_Route_Layer_BOTTOM_color = Route_Layer_BOTTOM_color;
        change = true;
    }

    int via_type = GetDesignSettings().m_CurrentViaType;
    via_color = Settings().Colors().GetItemColor( LAYER_VIAS + via_type );

    if( previous_via_color != via_color )
    {
        previous_via_color = via_color;
        change = true;
    }

    background_color = Settings().Colors().GetItemColor( LAYER_PCB_BACKGROUND );

    if( previous_background_color != background_color )
    {
        previous_background_color = background_color;
        change = true;
    }

    if( !change && LayerPairBitmap )
        return;

    LayerPairBitmap = std::make_unique<wxBitmap>( 24, 24 );

    /* Draw the icon, with colors according to the active layer and layer
     * pairs for via command (change layer)
     */
    wxMemoryDC iconDC;
    iconDC.SelectObject( *LayerPairBitmap );
    wxBrush    brush;
    wxPen      pen;
    int buttonColor = -1;

    brush.SetStyle( wxBRUSHSTYLE_SOLID );
    brush.SetColour( background_color.WithAlpha(1.0).ToColour() );
    iconDC.SetBrush( brush );
    iconDC.DrawRectangle( 0, 0, BM_LAYERICON_SIZE, BM_LAYERICON_SIZE );

    for( ii = 0; ii < BM_LAYERICON_SIZE; ii++ )
    {
        for( jj = 0; jj < BM_LAYERICON_SIZE; jj++ )
        {
            if( s_BitmapLayerIcon[ii][jj] != buttonColor )
            {
                switch( s_BitmapLayerIcon[ii][jj] )
                {
                default:
                case 0:
                    pen.SetColour( active_layer_color.ToColour() );
                    break;

                case 1:
                    pen.SetColour( Route_Layer_TOP_color.ToColour() );
                    break;

                case 2:
                    pen.SetColour( Route_Layer_BOTTOM_color.ToColour() );
                    break;

                case 3:
                    pen.SetColour( via_color.ToColour() );
                    break;
                }

                buttonColor = s_BitmapLayerIcon[ii][jj];
                iconDC.SetPen( pen );
            }

            iconDC.DrawPoint( jj, ii );
        }
    }

    /* Deselect the Tool Bitmap from DC,
     *  in order to delete the MemoryDC safely without deleting the bitmap */
    iconDC.SelectObject( wxNullBitmap );

    // Scale the bitmap
    const int scale = ( requested_scale <= 0 ) ? KiIconScale( this ) : requested_scale;
    wxImage image = LayerPairBitmap->ConvertToImage();

    // "NEAREST" causes less mixing of colors
    image.Rescale( scale * image.GetWidth() / 4, scale * image.GetHeight() / 4,
                   wxIMAGE_QUALITY_NEAREST );

    LayerPairBitmap = std::make_unique<wxBitmap>( image );

    if( m_mainToolBar )
    {
        m_mainToolBar->SetToolBitmap( ID_AUX_TOOLBAR_PCB_SELECT_LAYER_PAIR, *LayerPairBitmap );
        m_mainToolBar->Refresh();
    }
}


void PCB_EDIT_FRAME::ReCreateHToolbar()
{
    wxString msg;

    wxWindowUpdateLocker dummy( this );

    if( m_mainToolBar )
        m_mainToolBar->Clear();
    else
        m_mainToolBar = new wxAuiToolBar( this, ID_H_TOOLBAR, wxDefaultPosition, wxDefaultSize,
                                          KICAD_AUI_TB_STYLE | wxAUI_TB_HORZ_LAYOUT );

    // Set up toolbar
    if( Kiface().IsSingle() )
    {
        m_mainToolBar->AddTool( ID_NEW_BOARD, wxEmptyString, KiScaledBitmap( new_board_xpm, this ),
                                _( "New board" ) );
        m_mainToolBar->AddTool( ID_LOAD_FILE, wxEmptyString, KiScaledBitmap( open_brd_file_xpm, this ),
                                _( "Open existing board" ) );
    }

    m_mainToolBar->AddTool( ID_SAVE_BOARD, wxEmptyString, KiScaledBitmap( save_xpm, this ),
                            _( "Save board" ) );

    KiScaledSeparator( m_mainToolBar, this );
    m_mainToolBar->AddTool( ID_SHEET_SET, wxEmptyString, KiScaledBitmap( sheetset_xpm, this ),
                            _( "Page settings for paper size and texts" ) );

    KiScaledSeparator( m_mainToolBar, this );
    m_mainToolBar->AddTool( ID_OPEN_MODULE_EDITOR, wxEmptyString,
                            KiScaledBitmap( module_editor_xpm, this ),
                            _( "Open footprint editor" ) );

    m_mainToolBar->AddTool( ID_OPEN_MODULE_VIEWER, wxEmptyString,
                            KiScaledBitmap( modview_icon_xpm, this ),
                            _( "Open footprint viewer" ) );

    KiScaledSeparator( m_mainToolBar, this );
    msg = AddHotkeyName( HELP_UNDO, g_Board_Editor_Hotkeys_Descr, HK_UNDO, IS_COMMENT );
    m_mainToolBar->AddTool( wxID_UNDO, wxEmptyString, KiScaledBitmap( undo_xpm, this ), msg );
    msg = AddHotkeyName( HELP_REDO, g_Board_Editor_Hotkeys_Descr, HK_REDO, IS_COMMENT );
    m_mainToolBar->AddTool( wxID_REDO, wxEmptyString, KiScaledBitmap( redo_xpm, this ), msg );

    KiScaledSeparator( m_mainToolBar, this );
    m_mainToolBar->AddTool( wxID_PRINT, wxEmptyString, KiScaledBitmap( print_button_xpm, this ),
                            _( "Print board" ) );
    m_mainToolBar->AddTool( ID_GEN_PLOT, wxEmptyString, KiScaledBitmap( plot_xpm, this ),
                            _( "Plot (HPGL, PostScript, or GERBER format)" ) );

    KiScaledSeparator( m_mainToolBar, this );
    msg = AddHotkeyName( HELP_ZOOM_REDRAW, g_Board_Editor_Hotkeys_Descr, HK_ZOOM_REDRAW,
                         IS_COMMENT );
    m_mainToolBar->AddTool( ID_ZOOM_REDRAW, wxEmptyString, KiScaledBitmap( zoom_redraw_xpm, this ), msg );

    msg = AddHotkeyName( HELP_ZOOM_IN, g_Board_Editor_Hotkeys_Descr, HK_ZOOM_IN, IS_COMMENT );
    m_mainToolBar->AddTool( ID_ZOOM_IN, wxEmptyString, KiScaledBitmap( zoom_in_xpm, this ), msg );

    msg = AddHotkeyName( HELP_ZOOM_OUT, g_Board_Editor_Hotkeys_Descr, HK_ZOOM_OUT, IS_COMMENT );
    m_mainToolBar->AddTool( ID_ZOOM_OUT, wxEmptyString, KiScaledBitmap( zoom_out_xpm, this ), msg );

    msg = AddHotkeyName( HELP_ZOOM_FIT, g_Board_Editor_Hotkeys_Descr, HK_ZOOM_AUTO, IS_COMMENT );
    m_mainToolBar->AddTool( ID_ZOOM_PAGE, wxEmptyString, KiScaledBitmap( zoom_fit_in_page_xpm, this ), msg );

    m_mainToolBar->AddTool( ID_ZOOM_SELECTION, wxEmptyString, KiScaledBitmap( zoom_area_xpm, this ),
                            _( "Zoom to selection" ), wxITEM_CHECK );

    KiScaledSeparator( m_mainToolBar, this );
    msg = AddHotkeyName( HELP_FIND, g_Board_Editor_Hotkeys_Descr, HK_FIND_ITEM, IS_COMMENT );
    m_mainToolBar->AddTool( ID_FIND_ITEMS, wxEmptyString, KiScaledBitmap( find_xpm, this ), msg );

    KiScaledSeparator( m_mainToolBar, this );
    m_mainToolBar->AddTool( ID_GET_NETLIST, wxEmptyString, KiScaledBitmap( netlist_xpm, this ),
                            _( "Read netlist" ) );
    m_mainToolBar->AddTool( ID_DRC_CONTROL, wxEmptyString, KiScaledBitmap( erc_xpm, this ),
                            _( "Perform design rules check" ) );

    KiScaledSeparator( m_mainToolBar, this );

    if( m_SelLayerBox == NULL )
    {
        m_SelLayerBox = new PCB_LAYER_BOX_SELECTOR( m_mainToolBar, ID_TOOLBARH_PCB_SELECT_LAYER );
        m_SelLayerBox->SetBoardFrame( this );
    }

    ReCreateLayerBox( false );
    m_mainToolBar->AddControl( m_SelLayerBox );

    PrepareLayerIndicator();    // Initialize the bitmap with current
                                // active layer colors for the next tool
    m_mainToolBar->AddTool( ID_AUX_TOOLBAR_PCB_SELECT_LAYER_PAIR, wxEmptyString,
                            *LayerPairBitmap, SEL_LAYER_HELP );

    // Fast call to FreeROUTE Web Bases router
    KiScaledSeparator( m_mainToolBar, this );
    m_mainToolBar->AddTool( ID_TOOLBARH_PCB_FREEROUTE_ACCESS, wxEmptyString,
                            KiScaledBitmap( web_support_xpm, this ),
                            _( "Fast access to the FreeROUTE external advanced router" ) );

    // Access to the scripting console
#if defined(KICAD_SCRIPTING_WXPYTHON)
    if( IsWxPythonLoaded() )
    {
        KiScaledSeparator( m_mainToolBar, this );

        m_mainToolBar->AddTool( ID_TOOLBARH_PCB_SCRIPTING_CONSOLE, wxEmptyString,
                                KiScaledBitmap( py_script_xpm, this ),
                                _( "Show/Hide the Python Scripting console" ),
                                wxITEM_CHECK );
    }
#endif

    // after adding the buttons to the toolbar, must call Realize() to reflect the changes
    m_mainToolBar->Realize();
}


void PCB_EDIT_FRAME::ReCreateOptToolbar()
{
    wxWindowUpdateLocker dummy( this );

    if( m_optionsToolBar )
        m_optionsToolBar->Clear();
    else
        m_optionsToolBar = new wxAuiToolBar( this, ID_OPT_TOOLBAR, wxDefaultPosition, wxDefaultSize,
                                             KICAD_AUI_TB_STYLE | wxAUI_TB_VERTICAL );

    m_optionsToolBar->AddTool( ID_TB_OPTIONS_DRC_OFF, wxEmptyString, KiScaledBitmap( drc_off_xpm, this ),
                               _( "Enable design rule checking" ), wxITEM_CHECK );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_GRID, wxEmptyString, KiScaledBitmap( grid_xpm, this ),
                               _( "Hide grid" ), wxITEM_CHECK );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_POLAR_COORD, wxEmptyString,
                               KiScaledBitmap( polar_coord_xpm, this ),
                               _( "Display polar coordinates" ), wxITEM_CHECK );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_UNIT_INCH, wxEmptyString,
                               KiScaledBitmap( unit_inch_xpm, this ),
                               _( "Set units to inches" ), wxITEM_CHECK );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_UNIT_MM, wxEmptyString,
                               KiScaledBitmap( unit_mm_xpm, this ),
                               _( "Set units to millimeters" ), wxITEM_CHECK );

#ifndef __APPLE__
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_CURSOR, wxEmptyString,
                               KiScaledBitmap( cursor_shape_xpm, this ),
                               _( "Change cursor shape" ), wxITEM_CHECK );
#else
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SELECT_CURSOR, wxEmptyString,
                               KiScaledBitmap( cursor_shape_xpm, this ),
                               _( "Change cursor shape (not supported in Legacy Toolset)" ),
                               wxITEM_CHECK  );
#endif

    KiScaledSeparator( m_optionsToolBar, this );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_RATSNEST, wxEmptyString,
                               KiScaledBitmap( general_ratsnest_xpm, this ),
                               _( "Show board ratsnest" ), wxITEM_CHECK );

    KiScaledSeparator( m_optionsToolBar, this );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_ZONES, wxEmptyString, KiScaledBitmap( show_zone_xpm, this ),
                               _( "Show filled areas in zones" ), wxITEM_CHECK );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_ZONES_DISABLE, wxEmptyString,
                               KiScaledBitmap( show_zone_disable_xpm, this ),
                               _( "Do not show filled areas in zones" ) , wxITEM_CHECK );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_ZONES_OUTLINES_ONLY, wxEmptyString,
                               KiScaledBitmap( show_zone_outline_only_xpm, this ),
                               _( "Show outlines of filled areas only in zones" ), wxITEM_CHECK );

    KiScaledSeparator( m_optionsToolBar, this );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_PADS_SKETCH, wxEmptyString,
                               KiScaledBitmap( pad_sketch_xpm, this ),
                               _( "Show pads in outline mode" ), wxITEM_CHECK );

    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_VIAS_SKETCH, wxEmptyString,
                               KiScaledBitmap( via_sketch_xpm, this ),
                               _( "Show vias in outline mode" ), wxITEM_CHECK );

    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_TRACKS_SKETCH, wxEmptyString,
                               KiScaledBitmap( showtrack_xpm, this ),
                               _( "Show tracks in outline mode" ),
                               wxITEM_CHECK );

    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_HIGH_CONTRAST_MODE, wxEmptyString,
                               KiScaledBitmap( contrast_mode_xpm, this ),
                               _( "Enable high contrast display mode" ),
                               wxITEM_CHECK );

    // Tools to show/hide toolbars:
    KiScaledSeparator( m_optionsToolBar, this );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_MANAGE_LAYERS_VERTICAL_TOOLBAR,
                               wxEmptyString,
                               KiScaledBitmap( layers_manager_xpm, this ),
                               HELP_SHOW_HIDE_LAYERMANAGER,
                               wxITEM_CHECK );
    m_optionsToolBar->AddTool( ID_TB_OPTIONS_SHOW_EXTRA_VERTICAL_TOOLBAR_MICROWAVE,
                               wxEmptyString,
                               KiScaledBitmap( mw_toolbar_xpm, this ),
                               HELP_SHOW_HIDE_MICROWAVE_TOOLS,
                               wxITEM_CHECK );


    KiScaledSeparator( m_optionsToolBar, this );
    m_optionsToolBar->Realize();
}


void PCB_EDIT_FRAME::ReCreateVToolbar()
{
    wxWindowUpdateLocker dummy( this );

    if( m_drawToolBar )
        m_drawToolBar->Clear();
    else
        m_drawToolBar = new wxAuiToolBar( this, ID_V_TOOLBAR, wxDefaultPosition, wxDefaultSize,
                                          KICAD_AUI_TB_STYLE | wxAUI_TB_VERTICAL );

    // Set up toolbar
    m_drawToolBar->AddTool( ID_NO_TOOL_SELECTED, wxEmptyString, KiScaledBitmap( cursor_xpm, this ),
                            wxEmptyString, wxITEM_CHECK );

    KiScaledSeparator( m_drawToolBar, this );

    m_drawToolBar->AddTool( ID_PCB_HIGHLIGHT_BUTT, wxEmptyString, KiScaledBitmap( net_highlight_xpm, this ),
                            _( "Highlight net" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_SHOW_1_RATSNEST_BUTT, wxEmptyString,
                            KiScaledBitmap( tool_ratsnest_xpm, this ),
                            _( "Display local ratsnest" ), wxITEM_CHECK );

    KiScaledSeparator( m_drawToolBar, this );
    m_drawToolBar->AddTool( ID_PCB_MODULE_BUTT, wxEmptyString, KiScaledBitmap( module_xpm, this ),
                            _( "Add footprints" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_TRACK_BUTT, wxEmptyString, KiScaledBitmap( add_tracks_xpm, this ),
                            _( "Route tracks" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_DRAW_VIA_BUTT, wxEmptyString, KiScaledBitmap( add_via_xpm, this ),
                            _( "Add vias" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_ZONES_BUTT, wxEmptyString, KiScaledBitmap( add_zone_xpm, this ),
                            _( "Add filled zones" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_KEEPOUT_AREA_BUTT, wxEmptyString,
                            KiScaledBitmap( add_keepout_area_xpm, this ),
                            _( "Add keepout areas" ), wxITEM_CHECK );

    KiScaledSeparator( m_drawToolBar, this );

    m_drawToolBar->AddTool( ID_PCB_ADD_LINE_BUTT, wxEmptyString, KiScaledBitmap( add_graphical_segments_xpm, this ),
                            _( "Add graphic lines" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_CIRCLE_BUTT, wxEmptyString, KiScaledBitmap( add_circle_xpm, this ),
                            _( "Add graphic circle" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_ARC_BUTT, wxEmptyString, KiScaledBitmap( add_arc_xpm, this ),
                            _( "Add graphic arc" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_ADD_POLYGON_BUTT, wxEmptyString, KiScaledBitmap( add_graphical_polygon_xpm, this ),
                            _( "Add graphic polygon" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_ADD_TEXT_BUTT, wxEmptyString, KiScaledBitmap( text_xpm, this ),
                            _( "Add text on copper layers or graphic text" ), wxITEM_CHECK );

    KiScaledSeparator( m_drawToolBar, this );
    m_drawToolBar->AddTool( ID_PCB_DIMENSION_BUTT, wxEmptyString, KiScaledBitmap( add_dimension_xpm, this ),
                            _( "Add dimension" ), wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_TARGET_BUTT, wxEmptyString, KiScaledBitmap( add_pcb_target_xpm, this ),
                            _( "Add layer alignment target" ), wxITEM_CHECK );

    KiScaledSeparator( m_drawToolBar, this );
    m_drawToolBar->AddTool( ID_PCB_DELETE_ITEM_BUTT, wxEmptyString, KiScaledBitmap( delete_xpm, this ),
                            _( "Delete items" ), wxITEM_CHECK );

    KiScaledSeparator( m_drawToolBar, this );
    m_drawToolBar->AddTool( ID_PCB_PLACE_OFFSET_COORD_BUTT, wxEmptyString,
                            KiScaledBitmap( pcb_offset_xpm, this ),
                            _( "Place the auxiliary axis origin for some plot file formats,\n"
                               "and for drill and place files" ),
                            wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_PLACE_GRID_COORD_BUTT, wxEmptyString,
                            KiScaledBitmap( grid_select_axis_xpm, this ),
                            _( "Set the origin point for the grid" ),
                            wxITEM_CHECK );

    m_drawToolBar->AddTool( ID_PCB_MEASUREMENT_TOOL, wxEmptyString,
                            KiScaledBitmap( measurement_xpm, this ),
                            _( "Measure distance" ),
                            wxITEM_CHECK );

    m_drawToolBar->Realize();
}


/* Create the auxiliary vertical right toolbar, showing tools for microwave applications
 */
void PCB_EDIT_FRAME::ReCreateMicrowaveVToolbar()
{
    wxWindowUpdateLocker dummy(this);

    if( m_microWaveToolBar )
        m_microWaveToolBar->Clear();
    else
        m_microWaveToolBar = new wxAuiToolBar( this, ID_MICROWAVE_V_TOOLBAR, wxDefaultPosition,
                                               wxDefaultSize,
                                               KICAD_AUI_TB_STYLE | wxAUI_TB_VERTICAL );

    // Set up toolbar
    m_microWaveToolBar->AddTool( ID_PCB_MUWAVE_TOOL_SELF_CMD, wxEmptyString,
                                 KiScaledBitmap( mw_add_line_xpm, this ),
                                 _( "Create line of specified length for microwave applications" ),
                                 wxITEM_CHECK );

    m_microWaveToolBar->AddTool( ID_PCB_MUWAVE_TOOL_GAP_CMD, wxEmptyString,
                                 KiScaledBitmap( mw_add_gap_xpm, this ),
                                 _( "Create gap of specified length for microwave applications" ),
                                 wxITEM_CHECK );

    KiScaledSeparator( m_microWaveToolBar, this );

    m_microWaveToolBar->AddTool( ID_PCB_MUWAVE_TOOL_STUB_CMD, wxEmptyString,
                                 KiScaledBitmap( mw_add_stub_xpm, this ),
                                 _( "Create stub of specified length for microwave applications" ),
                                 wxITEM_CHECK );

    m_microWaveToolBar->AddTool( ID_PCB_MUWAVE_TOOL_STUB_ARC_CMD, wxEmptyString,
                                 KiScaledBitmap( mw_add_stub_arc_xpm, this ),
                                 _( "Create stub (arc) of specified length for microwave applications" ),
                                 wxITEM_CHECK );

    m_microWaveToolBar->AddTool( ID_PCB_MUWAVE_TOOL_FUNCTION_SHAPE_CMD, wxEmptyString,
                                 KiScaledBitmap( mw_add_shape_xpm, this ),
                                 _( "Create a polynomial shape for microwave applications" ),
                                 wxITEM_CHECK );

    m_microWaveToolBar->Realize();
}


void PCB_EDIT_FRAME::ReCreateAuxiliaryToolbar()
{
    wxWindowUpdateLocker dummy( this );

    if( m_auxiliaryToolBar )
    {
        updateTraceWidthSelectBox();
        updateViaSizeSelectBox();

        // combobox sizes can have changed: apply new best sizes
        wxAuiToolBarItem* item = m_auxiliaryToolBar->FindTool( ID_AUX_TOOLBAR_PCB_TRACK_WIDTH );
        item->SetMinSize( m_SelTrackWidthBox->GetBestSize() );
        item = m_auxiliaryToolBar->FindTool( ID_AUX_TOOLBAR_PCB_VIA_SIZE );
        item->SetMinSize( m_SelViaSizeBox->GetBestSize() );

        m_auxiliaryToolBar->Realize();
        m_auimgr.Update();
        return;
    }

    m_auxiliaryToolBar = new wxAuiToolBar( this, ID_AUX_TOOLBAR, wxDefaultPosition, wxDefaultSize,
                                           KICAD_AUI_TB_STYLE | wxAUI_TB_HORZ_LAYOUT );

    /* Set up toolbar items */

    // Creates box to display and choose tracks widths:
    m_SelTrackWidthBox = new wxChoice( m_auxiliaryToolBar,
                                         ID_AUX_TOOLBAR_PCB_TRACK_WIDTH,
                                         wxDefaultPosition, wxDefaultSize,
                                         0, NULL );
    updateTraceWidthSelectBox();
    m_auxiliaryToolBar->AddControl( m_SelTrackWidthBox );

    // Creates box to display and choose vias diameters:
    m_SelViaSizeBox = new wxChoice( m_auxiliaryToolBar,
                                      ID_AUX_TOOLBAR_PCB_VIA_SIZE,
                                      wxDefaultPosition, wxDefaultSize,
                                      0, NULL );
    updateViaSizeSelectBox();
    m_auxiliaryToolBar->AddControl( m_SelViaSizeBox );
    KiScaledSeparator( m_auxiliaryToolBar, this );

    // Creates box to display and choose strategy to handle tracks an vias sizes:
    m_auxiliaryToolBar->AddTool( ID_AUX_TOOLBAR_PCB_SELECT_AUTO_WIDTH,
                                 wxEmptyString,
                                 KiScaledBitmap( auto_track_width_xpm, this ),
                                 _( "Auto track width: when starting on an existing track "
                                    "use its width\notherwise, use current width setting" ),
                                 wxITEM_CHECK );

    // Add the box to display and select the current grid size:
    KiScaledSeparator( m_auxiliaryToolBar, this );
    m_gridSelectBox = new wxChoice( m_auxiliaryToolBar,
                                      ID_ON_GRID_SELECT,
                                      wxDefaultPosition, wxDefaultSize,
                                      0, NULL );
    updateGridSelectBox();
    m_auxiliaryToolBar->AddControl( m_gridSelectBox );

    //  Add the box to display and select the current Zoom
    KiScaledSeparator( m_auxiliaryToolBar, this );
    m_zoomSelectBox = new wxChoice( m_auxiliaryToolBar,
                                      ID_ON_ZOOM_SELECT,
                                      wxDefaultPosition, wxDefaultSize,
                                      0, NULL );
    updateZoomSelectBox();
    m_auxiliaryToolBar->AddControl( m_zoomSelectBox );

    // after adding the buttons to the toolbar, must call Realize()
    m_auxiliaryToolBar->Realize();
}


void PCB_EDIT_FRAME::updateTraceWidthSelectBox()
{
    if( m_SelTrackWidthBox == NULL )
        return;

    wxString msg;
    bool mmFirst = g_UserUnit != INCHES;

    m_SelTrackWidthBox->Clear();

    for( unsigned ii = 0; ii < GetDesignSettings().m_TrackWidthList.size(); ii++ )
    {
        int size = GetDesignSettings().m_TrackWidthList[ii];

        double valueMils = To_User_Unit( INCHES, size ) * 1000;
        double value_mm = To_User_Unit( MILLIMETRES, size );

        if( mmFirst )
            msg.Printf( _( "Track: %.3f mm (%.2f mils)" ),
                        value_mm, valueMils );
        else
            msg.Printf( _( "Track: %.2f mils (%.3f mm)" ),
                        valueMils, value_mm );

        // Mark the netclass track width value (the first in list)
        if( ii == 0 )
            msg << wxT( " *" );

        m_SelTrackWidthBox->Append( msg );
    }

    if( GetDesignSettings().GetTrackWidthIndex() >= GetDesignSettings().m_TrackWidthList.size() )
        GetDesignSettings().SetTrackWidthIndex( 0 );

    m_SelTrackWidthBox->SetSelection( GetDesignSettings().GetTrackWidthIndex() );
}


void PCB_EDIT_FRAME::updateViaSizeSelectBox()
{
    if( m_SelViaSizeBox == NULL )
        return;

    wxString msg;

    m_SelViaSizeBox->Clear();
    bool mmFirst = g_UserUnit != INCHES;

    for( unsigned ii = 0; ii < GetDesignSettings().m_ViasDimensionsList.size(); ii++ )
    {
        int diam = GetDesignSettings().m_ViasDimensionsList[ii].m_Diameter;

        double valueMils = To_User_Unit( INCHES, diam ) * 1000;
        double value_mm = To_User_Unit( MILLIMETRES, diam );

        if( mmFirst )
            msg.Printf( _( "Via: %.2f mm (%.1f mils)" ),
                        value_mm, valueMils );
        else
            msg.Printf( _( "Via: %.1f mils (%.2f mm)" ),
                        valueMils, value_mm );

        int hole = GetDesignSettings().m_ViasDimensionsList[ii].m_Drill;

        if( hole )
        {
            msg  << wxT("/ ");
            wxString hole_str;
            valueMils = To_User_Unit( INCHES, hole ) * 1000;
            value_mm = To_User_Unit( MILLIMETRES, hole );

            if( mmFirst )
                hole_str.Printf( _( "%.2f mm (%.1f mils)" ),
                            value_mm, valueMils );
            else
                hole_str.Printf( _( "%.1f mils (%.2f mm)" ),
                            valueMils, value_mm );

            msg += hole_str;
        }

        // Mark the netclass via size value (the first in list)
        if( ii == 0 )
            msg << wxT( " *" );

        m_SelViaSizeBox->Append( msg );
    }

    if( GetDesignSettings().GetViaSizeIndex() >= GetDesignSettings().m_ViasDimensionsList.size() )
        GetDesignSettings().SetViaSizeIndex( 0 );

    m_SelViaSizeBox->SetSelection( GetDesignSettings().GetViaSizeIndex() );
}


void PCB_EDIT_FRAME::ReCreateLayerBox( bool aForceResizeToolbar )
{
    if( m_SelLayerBox == NULL || m_mainToolBar == NULL )
        return;

    m_SelLayerBox->SetToolTip( _( "+/- to switch" ) );
    m_SelLayerBox->m_hotkeys = g_Board_Editor_Hotkeys_Descr;
    m_SelLayerBox->Resync();

    if( aForceResizeToolbar )
    {
        // the layer box can have its size changed
        // Update the aui manager, to take in account the new size
        m_auimgr.Update();
    }
}


void PCB_EDIT_FRAME::OnSelectOptionToolbar( wxCommandEvent& event )
{
    int id = event.GetId();
    bool state = event.IsChecked();
    auto displ_opts = (PCB_DISPLAY_OPTIONS*)GetDisplayOptions();

    switch( id )
    {
    case ID_TB_OPTIONS_DRC_OFF:
        Settings().m_legacyDrcOn = !state;

        if( GetToolId() == ID_TRACK_BUTT )
        {
            if( Settings().m_legacyDrcOn )
                m_canvas->SetCursor( wxCURSOR_PENCIL );
            else
                m_canvas->SetCursor( wxCURSOR_QUESTION_ARROW );
        }
        break;

    case ID_TB_OPTIONS_SHOW_RATSNEST:
        SetElementVisibility( LAYER_RATSNEST, state );
        OnModify();
        Compile_Ratsnest( NULL, true );

        m_canvas->Refresh();
        break;

    case ID_TB_OPTIONS_SHOW_ZONES:
        displ_opts->m_DisplayZonesMode = 0;
        m_canvas->Refresh();
        break;

    case ID_TB_OPTIONS_SHOW_ZONES_DISABLE:
        displ_opts->m_DisplayZonesMode = 1;
        m_canvas->Refresh();
        break;

    case ID_TB_OPTIONS_SHOW_ZONES_OUTLINES_ONLY:
        displ_opts->m_DisplayZonesMode = 2;
        m_canvas->Refresh();
        break;

    case ID_TB_OPTIONS_SHOW_VIAS_SKETCH:
        displ_opts->m_DisplayViaFill = !state;
        m_canvas->Refresh();
        break;

    case ID_TB_OPTIONS_SHOW_TRACKS_SKETCH:
        displ_opts->m_DisplayPcbTrackFill = !state;
        m_canvas->Refresh();
        break;

    case ID_TB_OPTIONS_SHOW_HIGH_CONTRAST_MODE:
    {
        displ_opts->m_ContrastModeDisplay = state;
        m_canvas->Refresh();
        break;
    }

    case ID_TB_OPTIONS_SHOW_EXTRA_VERTICAL_TOOLBAR_MICROWAVE:
        m_show_microwave_tools = state;
        m_auimgr.GetPane( wxT( "m_microWaveToolBar" ) ).Show( m_show_microwave_tools );
        m_auimgr.Update();
        break;

    case ID_TB_OPTIONS_SHOW_MANAGE_LAYERS_VERTICAL_TOOLBAR:
        // show auxiliary Vertical layers and visibility manager toolbar
        m_show_layer_manager_tools = state;
        m_auimgr.GetPane( wxT( "m_LayersManagerToolBar" ) ).Show( m_show_layer_manager_tools );
        m_auimgr.Update();
        break;

    default:
        DisplayErrorMessage( this, "Invalid toolbar option",
                       "PCB_EDIT_FRAME::OnSelectOptionToolbar error \n (event not handled!)" );
        break;
    }
}
