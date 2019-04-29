///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 17 2019)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "panel_pcbnew_settings_base.h"

///////////////////////////////////////////////////////////////////////////

PANEL_PCBNEW_SETTINGS_BASE::PANEL_PCBNEW_SETTINGS_BASE( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bPanelSizer;
	bPanelSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bMargins;
	bMargins = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bMiddleLeftSizer;
	bMiddleLeftSizer = new wxBoxSizer( wxVERTICAL );

	wxString m_PolarDisplayChoices[] = { _("Cartesian coordinates"), _("Polar coordinates") };
	int m_PolarDisplayNChoices = sizeof( m_PolarDisplayChoices ) / sizeof( wxString );
	m_PolarDisplay = new wxRadioBox( this, wxID_POLAR_CTRL, _("Coordinates"), wxDefaultPosition, wxDefaultSize, m_PolarDisplayNChoices, m_PolarDisplayChoices, 1, wxRA_SPECIFY_COLS );
	m_PolarDisplay->SetSelection( 0 );
	m_PolarDisplay->SetToolTip( _("Set display of relative (dx/dy) coordinates to Cartesian (rectangular) or polar (angle/distance).") );

	bMiddleLeftSizer->Add( m_PolarDisplay, 0, wxALL|wxEXPAND, 5 );

	wxString m_UnitsSelectionChoices[] = { _("Inches"), _("Millimeters") };
	int m_UnitsSelectionNChoices = sizeof( m_UnitsSelectionChoices ) / sizeof( wxString );
	m_UnitsSelection = new wxRadioBox( this, wxID_UNITS, _("Units"), wxDefaultPosition, wxDefaultSize, m_UnitsSelectionNChoices, m_UnitsSelectionChoices, 1, wxRA_SPECIFY_COLS );
	m_UnitsSelection->SetSelection( 0 );
	m_UnitsSelection->SetToolTip( _("Set units used to display dimensions and positions.") );

	bMiddleLeftSizer->Add( m_UnitsSelection, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* bOptionsSizer;
	bOptionsSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Editing Options") ), wxVERTICAL );

	m_Show_Page_Limits = new wxCheckBox( bOptionsSizer->GetStaticBox(), wxID_ANY, _("Show page limits"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Show_Page_Limits->SetValue(true);
	bOptionsSizer->Add( m_Show_Page_Limits, 0, wxALL, 5 );

	m_Segments_45_Only_Ctrl = new wxCheckBox( bOptionsSizer->GetStaticBox(), wxID_SEGMENTS45, _("L&imit graphic lines to H, V and 45 degrees"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Segments_45_Only_Ctrl->SetToolTip( _("Force line segment directions to H, V or 45 degrees when drawing on technical layers.") );

	bOptionsSizer->Add( m_Segments_45_Only_Ctrl, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	m_UseEditKeyForWidth = new wxCheckBox( bOptionsSizer->GetStaticBox(), wxID_ANY, _("Edit action changes track width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UseEditKeyForWidth->SetToolTip( _("When active, hitting Edit hotkey or double-clicking on a track or via changes its width/diameter to the one selected in the main toolbar. ") );

	bOptionsSizer->Add( m_UseEditKeyForWidth, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	m_dragSelects = new wxCheckBox( bOptionsSizer->GetStaticBox(), wxID_ANY, _("Prefer selection to dragging"), wxDefaultPosition, wxDefaultSize, 0 );
	m_dragSelects->SetToolTip( _("When enabled and nothing is selected, drag gesture will draw a selection box, even if there are items under the cursor that could be immediately dragged.") );

	bOptionsSizer->Add( m_dragSelects, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer12->AddGrowableCol( 1 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticTextRotationAngle = new wxStaticText( bOptionsSizer->GetStaticBox(), wxID_ANY, _("&Rotation angle:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRotationAngle->Wrap( -1 );
	fgSizer12->Add( m_staticTextRotationAngle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_RotationAngle = new wxTextCtrl( bOptionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_RotationAngle->SetToolTip( _("Set increment (in degrees) for context menu and hotkey rotation.") );

	fgSizer12->Add( m_RotationAngle, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );


	bOptionsSizer->Add( fgSizer12, 1, wxEXPAND, 5 );


	bMiddleLeftSizer->Add( bOptionsSizer, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );


	bMargins->Add( bMiddleLeftSizer, 1, wxEXPAND|wxRIGHT, 5 );

	wxBoxSizer* bRightSizer;
	bRightSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbMagnets;
	sbMagnets = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Magnetic Points") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 15 );
	fgSizer2->SetFlexibleDirection( wxVERTICAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText2 = new wxStaticText( sbMagnets->GetStaticBox(), wxID_ANY, _("Snap to Pads"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetToolTip( _("Capture cursor when the mouse enters a pad area") );

	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	wxString m_magneticPadChoiceChoices[] = { _("Never"), _("When creating tracks"), _("Always") };
	int m_magneticPadChoiceNChoices = sizeof( m_magneticPadChoiceChoices ) / sizeof( wxString );
	m_magneticPadChoice = new wxChoice( sbMagnets->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_magneticPadChoiceNChoices, m_magneticPadChoiceChoices, 0 );
	m_magneticPadChoice->SetSelection( 1 );
	m_magneticPadChoice->SetToolTip( _("Capture cursor when the mouse enters a pad area") );

	fgSizer2->Add( m_magneticPadChoice, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	m_staticText21 = new wxStaticText( sbMagnets->GetStaticBox(), wxID_ANY, _("Snap to Tracks"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	m_staticText21->SetToolTip( _("Capture cursor when the mouse approaches a track") );

	fgSizer2->Add( m_staticText21, 0, wxALL, 5 );

	wxString m_magneticTrackChoiceChoices[] = { _("Never"), _("When creating tracks"), _("Always") };
	int m_magneticTrackChoiceNChoices = sizeof( m_magneticTrackChoiceChoices ) / sizeof( wxString );
	m_magneticTrackChoice = new wxChoice( sbMagnets->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_magneticTrackChoiceNChoices, m_magneticTrackChoiceChoices, 0 );
	m_magneticTrackChoice->SetSelection( 1 );
	m_magneticTrackChoice->SetToolTip( _("Capture cursor when the mouse approaches a track") );

	fgSizer2->Add( m_magneticTrackChoice, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	m_staticText211 = new wxStaticText( sbMagnets->GetStaticBox(), wxID_ANY, _("Snap to Graphical"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText211->Wrap( -1 );
	m_staticText211->SetToolTip( _("Capture cursor when the mouse approaches graphical control points") );

	fgSizer2->Add( m_staticText211, 0, wxALL, 5 );

	wxString m_magneticGraphicsChoiceChoices[] = { _("Yes"), _("No") };
	int m_magneticGraphicsChoiceNChoices = sizeof( m_magneticGraphicsChoiceChoices ) / sizeof( wxString );
	m_magneticGraphicsChoice = new wxChoice( sbMagnets->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_magneticGraphicsChoiceNChoices, m_magneticGraphicsChoiceChoices, 0 );
	m_magneticGraphicsChoice->SetSelection( 0 );
	m_magneticGraphicsChoice->SetToolTip( _("Capture cursor when the mouse approaches graphical control points") );

	fgSizer2->Add( m_magneticGraphicsChoice, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT, 5 );


	sbMagnets->Add( fgSizer2, 1, wxEXPAND, 5 );


	bRightSizer->Add( sbMagnets, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* bLegacyOptionsSizer;
	bLegacyOptionsSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Legacy Toolset Routing Options") ), wxVERTICAL );

	m_DrcOn = new wxCheckBox( bLegacyOptionsSizer->GetStaticBox(), wxID_DRC_ONOFF, _("&Enforce design rules when routing"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DrcOn->SetValue(true);
	m_DrcOn->SetToolTip( _("Enable DRC control. When DRC control is disabled, all connections are allowed.") );

	bLegacyOptionsSizer->Add( m_DrcOn, 0, wxEXPAND|wxALL, 5 );

	m_TrackAutodel = new wxCheckBox( bLegacyOptionsSizer->GetStaticBox(), wxID_TRACK_AUTODEL, _("Auto-delete old tracks"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackAutodel->SetValue(true);
	m_TrackAutodel->SetToolTip( _("Enable automatic track deletion when redrawing a track.") );

	bLegacyOptionsSizer->Add( m_TrackAutodel, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	m_Track_45_Only_Ctrl = new wxCheckBox( bLegacyOptionsSizer->GetStaticBox(), wxID_TRACKS45, _("&Limit tracks to H, V and 45 degrees"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Track_45_Only_Ctrl->SetValue(true);
	m_Track_45_Only_Ctrl->SetToolTip( _("Force track directions to H, V or 45 degrees when drawing a track.") );

	bLegacyOptionsSizer->Add( m_Track_45_Only_Ctrl, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	m_Track_DoubleSegm_Ctrl = new wxCheckBox( bLegacyOptionsSizer->GetStaticBox(), wxID_ANY, _("&Use double segmented tracks"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Track_DoubleSegm_Ctrl->SetValue(true);
	m_Track_DoubleSegm_Ctrl->SetToolTip( _("Use two track segments, with 45 degrees angle between them, when drawing a new track") );

	bLegacyOptionsSizer->Add( m_Track_DoubleSegm_Ctrl, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );


	bRightSizer->Add( bLegacyOptionsSizer, 1, wxEXPAND|wxTOP|wxLEFT, 5 );


	bMargins->Add( bRightSizer, 1, wxEXPAND|wxRIGHT, 5 );


	bPanelSizer->Add( bMargins, 1, wxRIGHT, 5 );


	this->SetSizer( bPanelSizer );
	this->Layout();
	bPanelSizer->Fit( this );
}

PANEL_PCBNEW_SETTINGS_BASE::~PANEL_PCBNEW_SETTINGS_BASE()
{
}
