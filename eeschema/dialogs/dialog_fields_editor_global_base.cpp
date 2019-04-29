///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "widgets/wx_grid.h"

#include "dialog_fields_editor_global_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_FIELDS_EDITOR_GLOBAL_BASE::DIALOG_FIELDS_EDITOR_GLOBAL_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
	m_splitter1->SetMinimumPaneSize( 200 );
	
	m_leftPanel = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bLeftSizer;
	bLeftSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bGroupSizer;
	bGroupSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_groupComponentsBox = new wxCheckBox( m_leftPanel, OPT_GROUP_COMPONENTS, _("Group symbols"), wxDefaultPosition, wxDefaultSize, 0 );
	m_groupComponentsBox->SetValue(true); 
	m_groupComponentsBox->SetToolTip( _("Group components together based on common properties") );
	
	bGroupSizer->Add( m_groupComponentsBox, 0, wxALL|wxEXPAND, 5 );
	
	
	bGroupSizer->Add( 0, 0, 1, wxEXPAND|wxRIGHT|wxLEFT, 10 );
	
	m_bRefresh = new wxBitmapButton( m_leftPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_bRefresh->SetMinSize( wxSize( 30,30 ) );
	
	bGroupSizer->Add( m_bRefresh, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bLeftSizer->Add( bGroupSizer, 0, wxALL|wxBOTTOM|wxEXPAND|wxTOP, 2 );
	
	m_fieldsCtrl = new wxDataViewListCtrl( m_leftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_fieldsCtrl->SetMinSize( wxSize( -1,220 ) );
	
	bLeftSizer->Add( m_fieldsCtrl, 1, wxALL|wxEXPAND, 5 );
	
	m_addFieldButton = new wxButton( m_leftPanel, wxID_ANY, _("Add Field..."), wxDefaultPosition, wxDefaultSize, 0 );
	bLeftSizer->Add( m_addFieldButton, 0, wxALL|wxEXPAND, 5 );
	
	
	m_leftPanel->SetSizer( bLeftSizer );
	m_leftPanel->Layout();
	bLeftSizer->Fit( m_leftPanel );
	m_panel4 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bRightSizer;
	bRightSizer = new wxBoxSizer( wxVERTICAL );
	
	m_grid = new WX_GRID( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_grid->CreateGrid( 5, 6 );
	m_grid->EnableEditing( true );
	m_grid->EnableGridLines( true );
	m_grid->EnableDragGridSize( false );
	m_grid->SetMargins( 0, 0 );
	
	// Columns
	m_grid->EnableDragColMove( true );
	m_grid->EnableDragColSize( true );
	m_grid->SetColLabelSize( 20 );
	m_grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_grid->EnableDragRowSize( false );
	m_grid->SetRowLabelSize( 0 );
	m_grid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	m_grid->SetMinSize( wxSize( 400,240 ) );
	
	bRightSizer->Add( m_grid, 1, wxALL|wxEXPAND, 5 );
	
	
	m_panel4->SetSizer( bRightSizer );
	m_panel4->Layout();
	bRightSizer->Fit( m_panel4 );
	m_splitter1->SplitVertically( m_leftPanel, m_panel4, -1 );
	bMainSizer->Add( m_splitter1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bButtonsSizer;
	bButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	bButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, _("Apply, Save Schematic && Continue"), wxDefaultPosition, wxDefaultSize, 0 );
	bButtonsSizer->Add( m_button1, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bButtonsSizer->Add( m_sdbSizer1, 0, wxBOTTOM|wxEXPAND|wxLEFT, 5 );
	
	
	bMainSizer->Add( bButtonsSizer, 0, wxEXPAND, 5 );
	
	
	this->SetSizer( bMainSizer );
	this->Layout();
	bMainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnClose ) );
	m_groupComponentsBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnGroupComponentsToggled ), NULL, this );
	m_bRefresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnRegroupComponents ), NULL, this );
	m_fieldsCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED, wxDataViewEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnColumnItemToggled ), NULL, this );
	m_fieldsCtrl->Connect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnSizeFieldList ), NULL, this );
	m_addFieldButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnAddField ), NULL, this );
	m_grid->Connect( wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnTableValueChanged ), NULL, this );
	m_grid->Connect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnTableCellClick ), NULL, this );
	m_grid->Connect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnTableCellClick ), NULL, this );
	m_grid->Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnTableItemContextMenu ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnSaveAndContinue ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnCancel ), NULL, this );
}

DIALOG_FIELDS_EDITOR_GLOBAL_BASE::~DIALOG_FIELDS_EDITOR_GLOBAL_BASE()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnClose ) );
	m_groupComponentsBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnGroupComponentsToggled ), NULL, this );
	m_bRefresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnRegroupComponents ), NULL, this );
	m_fieldsCtrl->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED, wxDataViewEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnColumnItemToggled ), NULL, this );
	m_fieldsCtrl->Disconnect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnSizeFieldList ), NULL, this );
	m_addFieldButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnAddField ), NULL, this );
	m_grid->Disconnect( wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnTableValueChanged ), NULL, this );
	m_grid->Disconnect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnTableCellClick ), NULL, this );
	m_grid->Disconnect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnTableCellClick ), NULL, this );
	m_grid->Disconnect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnTableItemContextMenu ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnSaveAndContinue ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIELDS_EDITOR_GLOBAL_BASE::OnCancel ), NULL, this );
	
}
