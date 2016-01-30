///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  8 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_display_options_base.h"

///////////////////////////////////////////////////////////////////////////

DialogDisplayOptions_base::DialogDisplayOptions_base( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sLeftBoxSizer;
	sLeftBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Tracks and vias:") ), wxVERTICAL );
	
	wxString m_OptDisplayTracksChoices[] = { _("Sketch"), _("Filled") };
	int m_OptDisplayTracksNChoices = sizeof( m_OptDisplayTracksChoices ) / sizeof( wxString );
	m_OptDisplayTracks = new wxRadioBox( this, wxID_DISPLAY_TRACK, _("Tracks:"), wxDefaultPosition, wxDefaultSize, m_OptDisplayTracksNChoices, m_OptDisplayTracksChoices, 1, wxRA_SPECIFY_COLS );
	m_OptDisplayTracks->SetSelection( 0 );
	sLeftBoxSizer->Add( m_OptDisplayTracks, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_OptDisplayTracksClearanceChoices[] = { _("Always"), _("New track"), _("Never") };
	int m_OptDisplayTracksClearanceNChoices = sizeof( m_OptDisplayTracksClearanceChoices ) / sizeof( wxString );
	m_OptDisplayTracksClearance = new wxRadioBox( this, ID_SHOW_CLEARANCE, _("Show Tracks Clearance:"), wxDefaultPosition, wxDefaultSize, m_OptDisplayTracksClearanceNChoices, m_OptDisplayTracksClearanceChoices, 1, wxRA_SPECIFY_COLS );
	m_OptDisplayTracksClearance->SetSelection( 1 );
	sLeftBoxSizer->Add( m_OptDisplayTracksClearance, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_OptDisplayViaHoleChoices[] = { _("Never"), _("defined holes"), _("Always") };
	int m_OptDisplayViaHoleNChoices = sizeof( m_OptDisplayViaHoleChoices ) / sizeof( wxString );
	m_OptDisplayViaHole = new wxRadioBox( this, ID_VIAS_HOLES, _("Show Via Holes:"), wxDefaultPosition, wxDefaultSize, m_OptDisplayViaHoleNChoices, m_OptDisplayViaHoleChoices, 1, wxRA_SPECIFY_COLS );
	m_OptDisplayViaHole->SetSelection( 1 );
	sLeftBoxSizer->Add( m_OptDisplayViaHole, 0, wxALL|wxEXPAND, 5 );
	
	bMainSizer->Add( sLeftBoxSizer, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sMiddleSizer;
	sMiddleSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Footprints:") ), wxHORIZONTAL );
	
	wxBoxSizer* bLModuleSizer;
	bLModuleSizer = new wxBoxSizer( wxVERTICAL );
	
	wxString m_OptDisplayModEdgesChoices[] = { _("Line"), _("Filled"), _("Sketch") };
	int m_OptDisplayModEdgesNChoices = sizeof( m_OptDisplayModEdgesChoices ) / sizeof( wxString );
	m_OptDisplayModEdges = new wxRadioBox( this, ID_EDGES_MODULES, _("Module Edges:"), wxDefaultPosition, wxDefaultSize, m_OptDisplayModEdgesNChoices, m_OptDisplayModEdgesChoices, 1, wxRA_SPECIFY_COLS );
	m_OptDisplayModEdges->SetSelection( 1 );
	bLModuleSizer->Add( m_OptDisplayModEdges, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_OptDisplayModTextsChoices[] = { _("Line"), _("Filled"), _("Sketch") };
	int m_OptDisplayModTextsNChoices = sizeof( m_OptDisplayModTextsChoices ) / sizeof( wxString );
	m_OptDisplayModTexts = new wxRadioBox( this, ID_TEXT_MODULES, _("Texts:"), wxDefaultPosition, wxDefaultSize, m_OptDisplayModTextsNChoices, m_OptDisplayModTextsChoices, 1, wxRA_SPECIFY_COLS );
	m_OptDisplayModTexts->SetSelection( 1 );
	bLModuleSizer->Add( m_OptDisplayModTexts, 0, wxALL|wxEXPAND, 5 );
	
	sMiddleSizer->Add( bLModuleSizer, 0, 0, 5 );
	
	wxStaticBoxSizer* bRModuleSizer;
	bRModuleSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Pad Options:") ), wxVERTICAL );
	
	wxString m_OptDisplayPadsChoices[] = { _("Sketch"), _("Filled") };
	int m_OptDisplayPadsNChoices = sizeof( m_OptDisplayPadsChoices ) / sizeof( wxString );
	m_OptDisplayPads = new wxRadioBox( this, ID_PADS_SHAPES, _("Pad Shapes:"), wxDefaultPosition, wxDefaultSize, m_OptDisplayPadsNChoices, m_OptDisplayPadsChoices, 1, wxRA_SPECIFY_COLS );
	m_OptDisplayPads->SetSelection( 1 );
	bRModuleSizer->Add( m_OptDisplayPads, 0, wxALL|wxEXPAND, 5 );
	
	m_OptDisplayPadClearence = new wxCheckBox( this, wxID_ANY, _("Show Pad Clearance"), wxDefaultPosition, wxDefaultSize, 0 );
	bRModuleSizer->Add( m_OptDisplayPadClearence, 0, wxALL, 5 );
	
	m_OptDisplayPadNumber = new wxCheckBox( this, wxID_ANY, _("Show Pad Number"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OptDisplayPadNumber->SetValue(true); 
	bRModuleSizer->Add( m_OptDisplayPadNumber, 0, wxALL, 5 );
	
	m_OptDisplayPadNoConn = new wxCheckBox( this, wxID_ANY, _("Show Pad NoConnect"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OptDisplayPadNoConn->SetValue(true); 
	bRModuleSizer->Add( m_OptDisplayPadNoConn, 0, wxALL, 5 );
	
	sMiddleSizer->Add( bRModuleSizer, 0, 0, 5 );
	
	bMainSizer->Add( sMiddleSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bRightSizer;
	bRightSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sRightUpperSizer;
	sRightUpperSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Others:") ), wxVERTICAL );
	
	wxString m_OptDisplayDrawingsChoices[] = { _("Line"), _("Filled"), _("Sketch") };
	int m_OptDisplayDrawingsNChoices = sizeof( m_OptDisplayDrawingsChoices ) / sizeof( wxString );
	m_OptDisplayDrawings = new wxRadioBox( this, wxID_ANY, _("Display other items:"), wxDefaultPosition, wxDefaultSize, m_OptDisplayDrawingsNChoices, m_OptDisplayDrawingsChoices, 1, wxRA_SPECIFY_COLS );
	m_OptDisplayDrawings->SetSelection( 1 );
	sRightUpperSizer->Add( m_OptDisplayDrawings, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_Show_Page_LimitsChoices[] = { _("Yes"), _("No") };
	int m_Show_Page_LimitsNChoices = sizeof( m_Show_Page_LimitsChoices ) / sizeof( wxString );
	m_Show_Page_Limits = new wxRadioBox( this, wxID_ANY, _("Show page limits"), wxDefaultPosition, wxDefaultSize, m_Show_Page_LimitsNChoices, m_Show_Page_LimitsChoices, 1, wxRA_SPECIFY_COLS );
	m_Show_Page_Limits->SetSelection( 0 );
	sRightUpperSizer->Add( m_Show_Page_Limits, 0, wxALL|wxEXPAND, 5 );
	
	bRightSizer->Add( sRightUpperSizer, 1, wxEXPAND, 5 );
	
	
	bRightSizer->Add( 10, 10, 0, 0, 5 );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetForegroundColour( wxColour( 204, 0, 0 ) );
	
	bRightSizer->Add( m_buttonOK, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_buttonCANCEL = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCANCEL->SetForegroundColour( wxColour( 0, 0, 200 ) );
	
	bRightSizer->Add( m_buttonCANCEL, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	bMainSizer->Add( bRightSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();
	
	// Connect Events
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogDisplayOptions_base::OnOkClick ), NULL, this );
	m_buttonCANCEL->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogDisplayOptions_base::OnCancelClick ), NULL, this );
}

DialogDisplayOptions_base::~DialogDisplayOptions_base()
{
	// Disconnect Events
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogDisplayOptions_base::OnOkClick ), NULL, this );
	m_buttonCANCEL->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogDisplayOptions_base::OnCancelClick ), NULL, this );
}
