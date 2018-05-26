///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 19 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_block_options_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_BLOCK_OPTIONS_BASE::DIALOG_BLOCK_OPTIONS_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Options:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 4, 2, 3, 3 );
	
	m_Include_Modules = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include &footprints"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_Include_Modules, 0, 0, 5 );
	
	m_Include_PcbTextes = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include t&ext items"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_Include_PcbTextes, 0, 0, 5 );
	
	m_IncludeLockedModules = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include &locked footprints"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_IncludeLockedModules, 0, 0, 5 );
	
	m_Include_Draw_Items = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include &drawings"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_Include_Draw_Items, 0, 0, 5 );
	
	m_Include_Tracks = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include &tracks"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_Include_Tracks, 0, 0, 5 );
	
	m_Include_Edges_Items = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include &board outline layer"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_Include_Edges_Items, 0, 0, 5 );
	
	m_Include_Vias = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include &vias"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_Include_Vias, 0, 0, 5 );
	
	m_Include_Zones = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include &zones"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_Include_Zones, 0, 0, 5 );
	
	
	fgSizer1->Add( gSizer1, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 1, 2, 3, 3 );
	
	m_checkBoxIncludeInvisible = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Include &items on invisible layers"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_checkBoxIncludeInvisible, 0, 0, 5 );
	
	m_DrawBlockItems = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Draw &selected items while moving"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_DrawBlockItems, 0, 0, 5 );
	
	
	fgSizer1->Add( gSizer2, 1, wxALL|wxEXPAND, 5 );
	
	
	sbSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	
	bSizerMain->Add( sbSizer1, 1, wxALL|wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bSizerMain->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_Include_Modules->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_PcbTextes->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_IncludeLockedModules->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Draw_Items->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Tracks->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Edges_Items->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Vias->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Zones->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_checkBoxIncludeInvisible->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_DrawBlockItems->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::OnCancel ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::ExecuteCommand ), NULL, this );
}

DIALOG_BLOCK_OPTIONS_BASE::~DIALOG_BLOCK_OPTIONS_BASE()
{
	// Disconnect Events
	m_Include_Modules->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_PcbTextes->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_IncludeLockedModules->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Draw_Items->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Tracks->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Edges_Items->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Vias->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_Include_Zones->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_checkBoxIncludeInvisible->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_DrawBlockItems->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::checkBoxClicked ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::OnCancel ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_BLOCK_OPTIONS_BASE::ExecuteCommand ), NULL, this );
	
}
