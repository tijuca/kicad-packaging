	/*********************************************/
	/*	tool_pcb.cpp: construction des tool bars */
	/*********************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"

#include "protos.h"

#include "bitmaps.h"

#include "id.h"

#define BITMAP wxBitmap

#ifdef __UNIX__
#define LISTBOX_WIDTH 140
#else
#define LISTBOX_WIDTH 120
#endif

#include  "wx/ownerdrw.h"
#include  "wx/menuitem.h"

#define MUWAVE_ENBL

#include "mw_Add_Stub.xpm"
#include "mw_Add_stub_arc.xpm"
#include "mw_Add_Shape.xpm"
#include "mw_Add_Line.xpm"
#include "mw_Add_Gap.xpm"
#include "mw_toolbar.xpm"
#include "Add_Tracks.xpm"
#include "Add_Zone.xpm"
#include "Show_Zone.xpm"
#include "net_hightlight.xpm"
#include "PcbOffset.xpm"
#include "Add_Mires.xpm"
#include "Mode_Module.xpm"
#include "Mode_Track.xpm"
#include "tool_ratsnet.xpm"
#include "local_ratsnet.xpm"
#include "general_ratsnet.xpm"
#include "add_cotation.xpm"

/******************************************/
void WinEDA_PcbFrame::ReCreateHToolbar(void)
/******************************************/
// Create the main horizontal toolbar
{
int ii;

	if ( m_HToolBar != NULL )
		{		// simple mise a jour de la liste des fichiers anciens
		wxMenuItem * item;
		for ( ii = 9; ii >=0 ; ii-- )
			{
			if( m_FilesMenu->FindItem(ID_LOAD_FILE_1 + ii) )
				{
				item = m_FilesMenu->Remove(ID_LOAD_FILE_1 + ii);
				if ( item ) delete item;
				}
			}
		for ( ii = 0; ii < 10; ii++ )
			{
			if ( GetLastProject(ii).IsEmpty() ) break;
			m_FilesMenu->Append(ID_LOAD_FILE_1 + ii, GetLastProject(ii) );
			}

		SetToolbars();
		return;
		}


	m_HToolBar = new WinEDA_Toolbar(TOOLBAR_MAIN, this, ID_H_TOOLBAR, TRUE);
	m_HToolBar->SetRows(1);
	SetToolBar(m_HToolBar);

	// Set up toolbar
	m_HToolBar->AddTool(ID_NEW_BOARD, wxEmptyString, BITMAP(new_xpm), _("New Board"));
	m_HToolBar->AddTool(ID_LOAD_FILE, wxEmptyString, BITMAP(open_xpm), _("Open existing Board"));
	m_HToolBar->AddTool(ID_SAVE_BOARD, wxEmptyString, BITMAP(save_xpm), _("Save Board"));

	m_HToolBar->AddSeparator();
	m_HToolBar->AddTool(ID_SHEET_SET, wxEmptyString, BITMAP(sheetset_xpm), _("page settings (size, texts)") );

	m_HToolBar->AddSeparator();
	m_HToolBar->AddTool(ID_OPEN_MODULE_EDITOR, wxEmptyString, BITMAP(modedit_xpm),
		_("Open Module Editor"));

	m_HToolBar->AddSeparator();
	m_HToolBar->AddTool(wxID_CUT, wxEmptyString, BITMAP(cut_button), _("Cut selected item"));

#if 0
	m_HToolBar->AddTool(wxID_COPY, wxEmptyString, BITMAP(copy_button), _("Copy selected item"));

	m_HToolBar->AddTool(wxID_PASTE, wxEmptyString, BITMAP(paste_xpm), _("Paste"));
#endif

	m_HToolBar->AddTool(ID_UNDO_BUTT, wxEmptyString, BITMAP(undelete_xpm), _("Undelete"));

	m_HToolBar->AddSeparator();
	m_HToolBar->AddTool(ID_GEN_PRINT, wxEmptyString, BITMAP(print_button), _("Print Board"));
	m_HToolBar->AddTool(ID_GEN_PLOT, wxEmptyString, BITMAP(plot_xpm), _("Plot (Hplg, Postscript, or Gerber format)"));

	m_HToolBar->AddSeparator();
	m_HToolBar->AddTool(ID_ZOOM_PLUS_BUTT, wxEmptyString, BITMAP(zoom_in_xpm), _("zoom + (F1)"));
	m_HToolBar->AddTool(ID_ZOOM_MOINS_BUTT, wxEmptyString, BITMAP(zoom_out_xpm), _("zoom - (F2)"));
	m_HToolBar->AddTool(ID_ZOOM_REDRAW_BUTT, wxEmptyString, BITMAP(repaint_xpm), _("redraw (F3)"));

	m_HToolBar->AddTool(ID_ZOOM_PAGE_BUTT, wxEmptyString, BITMAP(zoom_optimal_xpm), _("auto zoom"));

	m_HToolBar->AddSeparator();
	m_HToolBar->AddTool(ID_FIND_ITEMS, wxEmptyString, BITMAP(find_xpm), _("Find components and texts"));

	m_HToolBar->AddSeparator();
	m_HToolBar->AddTool(ID_GET_NETLIST, wxEmptyString, BITMAP(netlist_xpm), _("Read Netlist"));
	m_HToolBar->AddTool(ID_DRC_CONTROL, wxEmptyString, BITMAP(erc_xpm), _("Pcb Design Rules Check"));

	m_HToolBar->AddSeparator();
    ReCreateLayerBox(m_HToolBar);

	m_HToolBar->AddSeparator();
	m_HToolBar->AddTool(ID_TOOLBARH_PCB_AUTOPLACE, wxEmptyString, BITMAP(mode_module_xpm),
			_("Mode Module: Manual and Automatic Move or Place for modules"), wxITEM_CHECK );
	m_HToolBar->AddTool(ID_TOOLBARH_PCB_AUTOROUTE, wxEmptyString, BITMAP(mode_track_xpm),
			_("Mode Track and Autorouting"), wxITEM_CHECK);


	// after adding the buttons to the toolbar, must call Realize() to reflect
	// the changes

	m_HToolBar->Realize();

	SetToolbars();
}

/*********************************************/
void WinEDA_PcbFrame::ReCreateOptToolbar(void)
/*********************************************/
// Create the left vertical toolbar (option selections)
{
	if ( m_OptionsToolBar ) return;

	// creation du tool bar options
	m_OptionsToolBar = new WinEDA_Toolbar(TOOLBAR_OPTION, this, ID_OPT_TOOLBAR, FALSE);

	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_DRC_OFF, wxEmptyString, BITMAP(drc_off_xpm),
			_("Drc OFF"), wxITEM_CHECK);
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_GRID, wxEmptyString, BITMAP(grid_xpm),
			_("Display Grid OFF"), wxITEM_CHECK);
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_POLAR_COORD, wxEmptyString,
			BITMAP(polar_coord_xpm), _("Display Polar Coord ON"), wxITEM_CHECK);
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SELECT_UNIT_INCH, wxEmptyString,
					BITMAP(unit_inch_xpm), _("Units = Inch"), wxITEM_CHECK );
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SELECT_UNIT_MM, wxEmptyString,
					BITMAP(unit_mm_xpm), _("Units = mm"), wxITEM_CHECK );
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SELECT_CURSOR, wxEmptyString, BITMAP(cursor_shape_xpm),
					_("Change Cursor Shape"), wxITEM_CHECK);

	m_OptionsToolBar->AddSeparator();
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_RATSNEST, wxEmptyString,
					BITMAP(general_ratsnet_xpm),
					_("Show General Ratsnest"), wxITEM_CHECK);
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_MODULE_RATSNEST,wxEmptyString,
					BITMAP(local_ratsnet_xpm),
					_("Show Module Ratsnest when moving"), wxITEM_CHECK);

	m_OptionsToolBar->AddSeparator();
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_AUTO_DEL_TRACK, wxEmptyString, BITMAP(auto_delete_track_xpm),
					_("Enable Auto Del Track"), wxITEM_CHECK);

	m_OptionsToolBar->AddSeparator();
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_ZONES,wxEmptyString, BITMAP(show_zone_xpm),
					_("Show Zones"), wxITEM_CHECK);

	m_OptionsToolBar->AddSeparator();
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_PADS_SKETCH,wxEmptyString,
					BITMAP(pad_sketch_xpm),
					_("Show Pads Sketch"), wxITEM_CHECK);

	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_TRACKS_SKETCH,wxEmptyString,
					BITMAP(showtrack_xpm),
					_("Show Tracks Sketch"), wxITEM_CHECK);

	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_HIGHT_CONTRAST_MODE, wxEmptyString,
					BITMAP(palette_xpm),
					_("Hight Contrast Mode Display"), wxITEM_CHECK);
	m_OptionsToolBar->ToggleTool(ID_TB_OPTIONS_SHOW_HIGHT_CONTRAST_MODE,
					DisplayOpt.ContrastModeDisplay);

#ifdef MUWAVE_ENBL
	m_OptionsToolBar->AddSeparator();
	m_OptionsToolBar->AddTool(ID_TB_OPTIONS_SHOW_EXTRA_VERTICAL_TOOLBAR1, wxEmptyString,
					BITMAP(mw_toolbar_xpm),
					_("Display auxiliary vertical toolbar (tools for micro wave applications)\n This is a very experimental feature (under development)"), wxITEM_CHECK);
#endif

	m_OptionsToolBar->Realize();

	SetToolbars();
}


/********************************************/
void WinEDA_PcbFrame::ReCreateVToolbar(void)
/********************************************/
/* Create the main vertical right toolbar, showing usual tools
*/
{
	if( m_VToolBar ) return;

	m_VToolBar = new WinEDA_Toolbar(TOOLBAR_TOOL, this, ID_V_TOOLBAR, FALSE);

	// Set up toolbar
	m_VToolBar->AddTool(ID_NO_SELECT_BUTT, wxEmptyString,
				BITMAP(cursor_xpm), wxEmptyString, wxITEM_CHECK );
	m_VToolBar->ToggleTool(ID_NO_SELECT_BUTT, TRUE);
	m_VToolBar->AddSeparator();

	m_VToolBar->AddTool(ID_PCB_HIGHLIGHT_BUTT, wxEmptyString,
				BITMAP(net_hightlight_xpm), _("Net highlight"), wxITEM_CHECK );

	m_VToolBar->AddTool(ID_PCB_SHOW_1_RATSNEST_BUTT, wxEmptyString,
				BITMAP(tool_ratsnet_xpm),
				_("Display local ratsnest (pad or module)"), wxITEM_CHECK );

	m_VToolBar->AddSeparator();
	m_VToolBar->AddTool(ID_COMPONENT_BUTT, wxEmptyString,
				BITMAP(module_xpm),
				_("Add modules"), wxITEM_CHECK);

	m_VToolBar->AddTool(ID_TRACK_BUTT, wxEmptyString,
				BITMAP(add_tracks_xpm),
				_("Add Tracks an vias"), wxITEM_CHECK);

	m_VToolBar->AddTool(ID_PCB_ZONES_BUTT, wxEmptyString,
				BITMAP(add_zone_xpm),
				_("Add Zones"), wxITEM_CHECK);

	m_VToolBar->AddSeparator();
	m_VToolBar->AddTool(ID_LINE_COMMENT_BUTT, wxEmptyString,
				BITMAP(add_dashed_line_xpm),
				_("Add graphic line or polygon"), wxITEM_CHECK);

	m_VToolBar->AddTool(ID_PCB_CIRCLE_BUTT, wxEmptyString,
				BITMAP(add_circle_xpm),
				_("Add graphic circle"), wxITEM_CHECK);

	m_VToolBar->AddTool(ID_PCB_ARC_BUTT, wxEmptyString,
				BITMAP(add_arc_xpm),
				_("Add graphic arc"), wxITEM_CHECK);

	m_VToolBar->AddTool(ID_TEXT_COMMENT_BUTT, wxEmptyString,
				BITMAP(add_text_xpm),
				_("Add Text"), wxITEM_CHECK);

	m_VToolBar->AddSeparator();
	m_VToolBar->AddTool(ID_PCB_COTATION_BUTT, wxEmptyString,
				BITMAP(add_cotation_xpm),
				_("Add Cotation"), wxITEM_CHECK);

	m_VToolBar->AddTool(ID_PCB_MIRE_BUTT, wxEmptyString,
				BITMAP(add_mires_xpm),
				_("Add Mires"), wxITEM_CHECK);

	m_VToolBar->AddSeparator();
	m_VToolBar->AddTool(ID_PCB_DELETE_ITEM_BUTT, wxEmptyString,
				BITMAP(delete_body_xpm),
				_("Delete items"), wxITEM_CHECK);

	m_VToolBar->AddSeparator();
	m_VToolBar->AddTool(ID_PCB_PLACE_OFFSET_COORD_BUTT, wxEmptyString,
				BITMAP(pcb_offset_xpm),
				_("Offset adjust for drill and place files"), wxITEM_CHECK);

	m_VToolBar->Realize();

	SetToolbars();
}


/*********************************************/
void WinEDA_PcbFrame::ReCreateAuxVToolbar(void)
/*********************************************/
/* Create the auxiliary vertical right toolbar, showing tools fo microwave applications
*/
{
	if( m_AuxVToolBar ) return;

	m_AuxVToolBar = new WinEDA_Toolbar(TOOLBAR_TOOL, this, ID_AUX_V_TOOLBAR, FALSE);

	// Set up toolbar
	m_AuxVToolBar->AddTool(ID_PCB_MUWAVE_TOOL_SELF_CMD,
				BITMAP(mw_Add_Line_xpm),
				wxNullBitmap, TRUE,
				-1,-1, (wxObject *) NULL,
				_("Create line of specified length for microwave applications") );

	m_AuxVToolBar->AddTool(ID_PCB_MUWAVE_TOOL_GAP_CMD,
				BITMAP(mw_Add_Gap_xpm),
				wxNullBitmap, TRUE,
				-1,-1, (wxObject *) NULL,
				_("Create gap of specified length for microwave applications") );

	m_AuxVToolBar->AddSeparator();

	m_AuxVToolBar->AddTool(ID_PCB_MUWAVE_TOOL_STUB_CMD,
				BITMAP(mw_Add_Stub_xpm),
				wxNullBitmap, TRUE,
				-1,-1, (wxObject *) NULL,
				_("Create stub of specified length for microwave applications") );

	m_AuxVToolBar->AddTool(ID_PCB_MUWAVE_TOOL_STUB_ARC_CMD,
				BITMAP(mw_Add_stub_arc_xpm),
				wxNullBitmap, TRUE,
				-1,-1, (wxObject *) NULL,
				_("Create stub (arc) of specified length for microwave applications") );

	m_AuxVToolBar->AddTool(ID_PCB_MUWAVE_TOOL_FUNCTION_SHAPE_CMD,
				BITMAP(mw_Add_Shape_xpm),
				wxNullBitmap, TRUE,
				-1,-1, (wxObject *) NULL,
				_("Create a polynomial shape for microwave applications") );

	m_AuxVToolBar->Realize();

	SetToolbars();
}

/****************************************************/
void WinEDA_PcbFrame::ReCreateAuxiliaryToolbar(void)
/****************************************************/
/* Create auxiliary horizontal toolbar
*/
{
int ii;
wxString msg;

	if ( m_AuxiliaryToolBar == NULL )
	{
		m_AuxiliaryToolBar = new WinEDA_Toolbar(TOOLBAR_AUX, this, ID_AUX_TOOLBAR, TRUE);

		// Set up toolbar
		m_AuxiliaryToolBar->AddSeparator();
		m_SelTrackWidthBox = new WinEDAChoiceBox(m_AuxiliaryToolBar,
					ID_AUX_TOOLBAR_PCB_TRACK_WIDTH,
					wxPoint(-1,-1), wxSize(LISTBOX_WIDTH+20, -1));
		m_AuxiliaryToolBar->AddControl(m_SelTrackWidthBox);
		m_SelTrackWidthBox_Changed = TRUE;

		m_AuxiliaryToolBar->AddSeparator();
		m_SelViaSizeBox = new WinEDAChoiceBox(m_AuxiliaryToolBar,
					ID_AUX_TOOLBAR_PCB_VIA_SIZE,
					wxPoint(-1,-1), wxSize(LISTBOX_WIDTH+10, -1));
		m_AuxiliaryToolBar->AddControl(m_SelViaSizeBox);

		m_AuxiliaryToolBar->AddSeparator();
		// Boite de selection du pas de grille
		m_SelGridBox = new WinEDAChoiceBox(m_AuxiliaryToolBar,
					ID_ON_GRID_SELECT,
					wxPoint(-1,-1), wxSize(LISTBOX_WIDTH, -1));
		m_AuxiliaryToolBar->AddControl( m_SelGridBox);

		// Boite de selection du Zoom
		m_AuxiliaryToolBar->AddSeparator();
		m_SelZoomBox = new WinEDAChoiceBox(m_AuxiliaryToolBar,
					ID_ON_ZOOM_SELECT,
					wxPoint(-1,-1), wxSize(LISTBOX_WIDTH, -1));
		msg = _("Auto");
		m_SelZoomBox->Append(msg);
		for ( int jj = 0, ii = 1; ii <= m_ZoomMaxValue; ii <<= 1, jj++ )
		{
			msg = _("Zoom "); msg << ii;
			m_SelZoomBox->Append(msg);
		}
		m_SelZoomBox->Append(wxT(""));

		m_AuxiliaryToolBar->AddControl( m_SelZoomBox);

		// after adding the buttons to the toolbar, must call Realize()
		m_AuxiliaryToolBar->Realize();
	}

	// mise a jour des affichages
	m_SelGridBox->Clear();
	wxString format  = _("Grid");
	if ( g_UnitMetric == INCHES ) format += wxT(" %.1f");
	else format += wxT(" %.3f");

	for ( ii = 0; g_GridList[ii].x > 0; ii++ )
	{
	double value = To_User_Unit(g_UnitMetric, g_GridList[ii].x, PCB_INTERNAL_UNIT);
		if ( g_UnitMetric == INCHES )
			msg.Printf( format.GetData(), value * 1000);
		else
			msg.Printf( format.GetData(), value);
		m_SelGridBox->Append(msg);
	}
	m_SelGridBox->Append( _("User Grid") );

	m_SelViaSizeBox_Changed = TRUE;
	m_SelTrackWidthBox_Changed = TRUE;

	SetToolbars();
}


/**********************************************************************/
WinEDAChoiceBox * WinEDA_PcbFrame::ReCreateLayerBox(WinEDA_Toolbar * parent)
/**********************************************************************/
{
int ii, jj, ll;
bool rebuild = FALSE;
long current_mask_layer;

    if ( m_SelLayerBox == NULL )
	{
		if ( parent == NULL ) return NULL;
    	m_SelLayerBox = new WinEDAChoiceBox(parent, ID_TOOLBARH_PCB_SELECT_LAYER,
					wxPoint(-1,-1), wxSize(LISTBOX_WIDTH, -1));
		parent->AddControl(m_SelLayerBox);
	}

    // Test si reconstruction de la liste n�cessaire
    current_mask_layer = 0;
	int Masque_Layer = g_TabAllCopperLayerMask[g_DesignSettings.m_CopperLayerCount-1];
	Masque_Layer |= ALL_NO_CU_LAYERS;
	for ( ii = 0; ii < (int)m_SelLayerBox->GetCount() ; ii ++ )
	{
        jj = (int) ((size_t) m_SelLayerBox->GetClientData(ii));
        current_mask_layer |= g_TabOneLayerMask[jj];
	}
	if ( current_mask_layer != Masque_Layer) rebuild = TRUE;

    // Construction de la liste
    if ( rebuild )
	{
        m_SelLayerBox->Clear();
    	for ( ii = 0, jj = 0; ii <= EDGE_N ; ii ++ )
		{
		    if ( (g_TabOneLayerMask[ii] & Masque_Layer) )
			{
           	    m_SelLayerBox->Append(ReturnPcbLayerName(ii));
           	    m_SelLayerBox->SetClientData(jj, (void*)ii);
      		    jj++;
			}
		}
	}

    // Activation de l'affichage sur la bonne couche
	// Pour eviter la reentrance (Bug wxGTK version Linux?), la selection n'est faite que si
	// elle est mauvaise (Pb corrige sur wxGTK 2.6.0)
    jj = m_SelLayerBox->GetCount();
    ll = m_SelLayerBox->GetChoice();
   	for ( ii = 0; ii < jj ; ii ++ )
	{
  		if ( (int)((size_t)m_SelLayerBox->GetClientData(ii)) == GetScreen()->m_Active_Layer )
		{
			if ( ii != ll ) m_SelLayerBox->SetSelection( ii );
            break;
		}
	}

    return m_SelLayerBox;
}


