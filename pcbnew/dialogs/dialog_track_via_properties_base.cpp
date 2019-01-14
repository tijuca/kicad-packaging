///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 11 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "pcb_layer_box_selector.h"

#include "dialog_track_via_properties_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_TRACK_VIA_PROPERTIES_BASE::DIALOG_TRACK_VIA_PROPERTIES_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	m_MainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_sbCommonSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Common") ), wxVERTICAL );
	
	wxBoxSizer* bSizerNetWidgets;
	bSizerNetWidgets = new wxBoxSizer( wxHORIZONTAL );
	
	m_netSelectorLabel = new wxStaticText( m_sbCommonSizer->GetStaticBox(), wxID_ANY, _("Net:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_netSelectorLabel->Wrap( -1 );
	bSizerNetWidgets->Add( m_netSelectorLabel, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_netSelector = new NET_SELECTOR( m_sbCommonSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 ); 
	bSizerNetWidgets->Add( m_netSelector, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5 );
	
	
	m_sbCommonSizer->Add( bSizerNetWidgets, 5, wxEXPAND|wxRIGHT, 10 );
	
	m_lockedCbox = new wxCheckBox( m_sbCommonSizer->GetStaticBox(), wxID_ANY, _("Locked"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE );
	m_sbCommonSizer->Add( m_lockedCbox, 0, wxALL, 5 );
	
	
	m_MainSizer->Add( m_sbCommonSizer, 0, wxEXPAND|wxALL, 5 );
	
	m_sbTrackSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Tracks") ), wxHORIZONTAL );
	
	wxFlexGridSizer* fgTrackLeftGridSizer;
	fgTrackLeftGridSizer = new wxFlexGridSizer( 7, 3, 3, 5 );
	fgTrackLeftGridSizer->AddGrowableCol( 1 );
	fgTrackLeftGridSizer->SetFlexibleDirection( wxBOTH );
	fgTrackLeftGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_TrackStartXLabel = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Start point X:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackStartXLabel->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackStartXLabel, 0, wxALIGN_CENTER_VERTICAL, 3 );
	
	m_TrackStartXCtrl = new wxTextCtrl( m_sbTrackSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgTrackLeftGridSizer->Add( m_TrackStartXCtrl, 0, wxEXPAND, 3 );
	
	m_TrackStartXUnit = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackStartXUnit->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackStartXUnit, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 3 );
	
	m_TrackStartYLabel = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Start point Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackStartYLabel->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackStartYLabel, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );
	
	m_TrackStartYCtrl = new wxTextCtrl( m_sbTrackSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgTrackLeftGridSizer->Add( m_TrackStartYCtrl, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxEXPAND, 5 );
	
	m_TrackStartYUnit = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackStartYUnit->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackStartYUnit, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT, 5 );
	
	m_TrackEndXLabel = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("End point X:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackEndXLabel->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackEndXLabel, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_TrackEndXCtrl = new wxTextCtrl( m_sbTrackSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgTrackLeftGridSizer->Add( m_TrackEndXCtrl, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_TrackEndXUnit = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackEndXUnit->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackEndXUnit, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
	
	m_TrackEndYLabel = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("End point Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackEndYLabel->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackEndYLabel, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, 8 );
	
	m_TrackEndYCtrl = new wxTextCtrl( m_sbTrackSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgTrackLeftGridSizer->Add( m_TrackEndYCtrl, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxEXPAND, 8 );
	
	m_TrackEndYUnit = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackEndYUnit->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackEndYUnit, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, 8 );
	
	m_DesignRuleWidths = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Pre-defined widths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DesignRuleWidths->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_DesignRuleWidths, 0, wxALIGN_CENTER_VERTICAL|wxTOP, 5 );
	
	wxArrayString m_DesignRuleWidthsCtrlChoices;
	m_DesignRuleWidthsCtrl = new wxChoice( m_sbTrackSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DesignRuleWidthsCtrlChoices, 0 );
	m_DesignRuleWidthsCtrl->SetSelection( 0 );
	fgTrackLeftGridSizer->Add( m_DesignRuleWidthsCtrl, 0, wxEXPAND|wxTOP|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_DesignRuleWidthsUnits = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DesignRuleWidthsUnits->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_DesignRuleWidthsUnits, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT, 5 );
	
	m_TrackWidthLabel = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Track width:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackWidthLabel->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackWidthLabel, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_TrackWidthCtrl = new wxTextCtrl( m_sbTrackSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgTrackLeftGridSizer->Add( m_TrackWidthCtrl, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_TrackWidthUnit = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackWidthUnit->Wrap( -1 );
	fgTrackLeftGridSizer->Add( m_TrackWidthUnit, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT, 5 );
	
	
	fgTrackLeftGridSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_trackNetclass = new wxCheckBox( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Use net class widths"), wxDefaultPosition, wxDefaultSize, 0 );
	fgTrackLeftGridSizer->Add( m_trackNetclass, 0, wxTOP|wxBOTTOM, 5 );
	
	
	fgTrackLeftGridSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_sbTrackSizer->Add( fgTrackLeftGridSizer, 5, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	
	m_sbTrackSizer->Add( 0, 0, 0, wxEXPAND|wxRIGHT|wxLEFT, 15 );
	
	wxFlexGridSizer* fgTrackRightSizer;
	fgTrackRightSizer = new wxFlexGridSizer( 1, 3, 3, 5 );
	fgTrackRightSizer->AddGrowableCol( 1 );
	fgTrackRightSizer->SetFlexibleDirection( wxBOTH );
	fgTrackRightSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_TrackLayerLabel = new wxStaticText( m_sbTrackSizer->GetStaticBox(), wxID_ANY, _("Layer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TrackLayerLabel->Wrap( -1 );
	fgTrackRightSizer->Add( m_TrackLayerLabel, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 25 );
	
	m_TrackLayerCtrl = new PCB_LAYER_BOX_SELECTOR( m_sbTrackSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgTrackRightSizer->Add( m_TrackLayerCtrl, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	
	fgTrackRightSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_sbTrackSizer->Add( fgTrackRightSizer, 4, wxLEFT, 10 );
	
	
	m_MainSizer->Add( m_sbTrackSizer, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_sbViaSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Vias") ), wxHORIZONTAL );
	
	wxFlexGridSizer* fgViaLeftSizer;
	fgViaLeftSizer = new wxFlexGridSizer( 6, 3, 3, 5 );
	fgViaLeftSizer->AddGrowableCol( 1 );
	fgViaLeftSizer->SetFlexibleDirection( wxBOTH );
	fgViaLeftSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_ViaXLabel = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Position X:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaXLabel->Wrap( -1 );
	fgViaLeftSizer->Add( m_ViaXLabel, 0, wxALIGN_CENTER_VERTICAL, 3 );
	
	m_ViaXCtrl = new wxTextCtrl( m_sbViaSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgViaLeftSizer->Add( m_ViaXCtrl, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 3 );
	
	m_ViaXUnit = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaXUnit->Wrap( -1 );
	fgViaLeftSizer->Add( m_ViaXUnit, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 3 );
	
	m_ViaYLabel = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Position Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaYLabel->Wrap( -1 );
	fgViaLeftSizer->Add( m_ViaYLabel, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );
	
	m_ViaYCtrl = new wxTextCtrl( m_sbViaSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgViaLeftSizer->Add( m_ViaYCtrl, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxEXPAND, 5 );
	
	m_ViaYUnit = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaYUnit->Wrap( -1 );
	fgViaLeftSizer->Add( m_ViaYUnit, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT, 5 );
	
	m_DesignRuleVias = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Pre-defined sizes:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DesignRuleVias->Wrap( -1 );
	fgViaLeftSizer->Add( m_DesignRuleVias, 0, wxALIGN_CENTER_VERTICAL|wxTOP, 5 );
	
	wxArrayString m_DesignRuleViasCtrlChoices;
	m_DesignRuleViasCtrl = new wxChoice( m_sbViaSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DesignRuleViasCtrlChoices, 0 );
	m_DesignRuleViasCtrl->SetSelection( 0 );
	fgViaLeftSizer->Add( m_DesignRuleViasCtrl, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP, 5 );
	
	m_DesignRuleViasUnit = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DesignRuleViasUnit->Wrap( -1 );
	fgViaLeftSizer->Add( m_DesignRuleViasUnit, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5 );
	
	m_ViaDiameterLabel = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Via diameter:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaDiameterLabel->Wrap( -1 );
	fgViaLeftSizer->Add( m_ViaDiameterLabel, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ViaDiameterCtrl = new wxTextCtrl( m_sbViaSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgViaLeftSizer->Add( m_ViaDiameterCtrl, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_ViaDiameterUnit = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaDiameterUnit->Wrap( -1 );
	fgViaLeftSizer->Add( m_ViaDiameterUnit, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
	
	m_ViaDrillLabel = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Via drill:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaDrillLabel->Wrap( -1 );
	fgViaLeftSizer->Add( m_ViaDrillLabel, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ViaDrillCtrl = new wxTextCtrl( m_sbViaSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgViaLeftSizer->Add( m_ViaDrillCtrl, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_ViaDrillUnit = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaDrillUnit->Wrap( -1 );
	fgViaLeftSizer->Add( m_ViaDrillUnit, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
	
	
	fgViaLeftSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_viaNetclass = new wxCheckBox( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Use net class sizes"), wxDefaultPosition, wxDefaultSize, 0 );
	fgViaLeftSizer->Add( m_viaNetclass, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );
	
	
	fgViaLeftSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_sbViaSizer->Add( fgViaLeftSizer, 5, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	
	m_sbViaSizer->Add( 0, 0, 0, wxEXPAND|wxRIGHT|wxLEFT, 10 );
	
	wxFlexGridSizer* fgViaRightSizer;
	fgViaRightSizer = new wxFlexGridSizer( 3, 2, 3, 5 );
	fgViaRightSizer->AddGrowableCol( 1 );
	fgViaRightSizer->SetFlexibleDirection( wxBOTH );
	fgViaRightSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_ViaTypeLabel = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Via type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaTypeLabel->Wrap( -1 );
	fgViaRightSizer->Add( m_ViaTypeLabel, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );
	
	wxString m_ViaTypeChoiceChoices[] = { _("Through"), _("Micro"), _("Blind/buried"), wxEmptyString };
	int m_ViaTypeChoiceNChoices = sizeof( m_ViaTypeChoiceChoices ) / sizeof( wxString );
	m_ViaTypeChoice = new wxChoice( m_sbViaSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ViaTypeChoiceNChoices, m_ViaTypeChoiceChoices, 0 );
	m_ViaTypeChoice->SetSelection( 0 );
	m_ViaTypeChoice->Enable( false );
	
	fgViaRightSizer->Add( m_ViaTypeChoice, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 4 );
	
	m_ViaStartLayerLabel = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("Start layer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaStartLayerLabel->Wrap( -1 );
	fgViaRightSizer->Add( m_ViaStartLayerLabel, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ViaStartLayer = new PCB_LAYER_BOX_SELECTOR( m_sbViaSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgViaRightSizer->Add( m_ViaStartLayer, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );
	
	m_ViaEndLayerLabel1 = new wxStaticText( m_sbViaSizer->GetStaticBox(), wxID_ANY, _("End layer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ViaEndLayerLabel1->Wrap( -1 );
	fgViaRightSizer->Add( m_ViaEndLayerLabel1, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ViaEndLayer = new PCB_LAYER_BOX_SELECTOR( m_sbViaSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgViaRightSizer->Add( m_ViaEndLayer, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	m_sbViaSizer->Add( fgViaRightSizer, 4, wxLEFT, 10 );
	
	
	m_MainSizer->Add( m_sbViaSizer, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	m_MainSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_StdButtons = new wxStdDialogButtonSizer();
	m_StdButtonsOK = new wxButton( this, wxID_OK );
	m_StdButtons->AddButton( m_StdButtonsOK );
	m_StdButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_StdButtons->AddButton( m_StdButtonsCancel );
	m_StdButtons->Realize();
	
	m_MainSizer->Add( m_StdButtons, 0, wxEXPAND|wxALL, 5 );
	
	
	this->SetSizer( m_MainSizer );
	this->Layout();
	m_MainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_trackNetclass->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES_BASE::onTrackNetclassCheck ), NULL, this );
	m_viaNetclass->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES_BASE::onViaNetclassCheck ), NULL, this );
}

DIALOG_TRACK_VIA_PROPERTIES_BASE::~DIALOG_TRACK_VIA_PROPERTIES_BASE()
{
	// Disconnect Events
	m_trackNetclass->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES_BASE::onTrackNetclassCheck ), NULL, this );
	m_viaNetclass->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES_BASE::onViaNetclassCheck ), NULL, this );
	
}
