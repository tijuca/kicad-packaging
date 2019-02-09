///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "widgets/wx_grid.h"

#include "dialog_edit_components_libid_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_EDIT_COMPONENTS_LIBID_BASE::DIALOG_EDIT_COMPONENTS_LIBID_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	m_grid = new WX_GRID( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_grid->CreateGrid( 5, 3 );
	m_grid->EnableEditing( true );
	m_grid->EnableGridLines( true );
	m_grid->EnableDragGridSize( false );
	m_grid->SetMargins( 0, 0 );
	
	// Columns
	m_grid->SetColSize( 0, 280 );
	m_grid->SetColSize( 1, 280 );
	m_grid->SetColSize( 2, 280 );
	m_grid->EnableDragColMove( false );
	m_grid->EnableDragColSize( true );
	m_grid->SetColLabelSize( 22 );
	m_grid->SetColLabelValue( 0, _("Symbols") );
	m_grid->SetColLabelValue( 1, _("Current Library Reference") );
	m_grid->SetColLabelValue( 2, _("New Library Reference") );
	m_grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_grid->EnableDragRowSize( false );
	m_grid->SetRowLabelSize( 0 );
	m_grid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	m_grid->SetMinSize( wxSize( -1,300 ) );
	
	bSizerMain->Add( m_grid, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 10 );
	
	wxBoxSizer* bSizerMsgWarning;
	bSizerMsgWarning = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextWarning = new wxStaticText( this, wxID_ANY, _("Warning: changes made from this dialog cannot be undone after closing it."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextWarning->Wrap( -1 );
	m_staticTextWarning->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizerMsgWarning->Add( m_staticTextWarning, 1, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxTOP, 5 );
	
	m_buttonUndo = new wxButton( this, wxID_ANY, _("Undo Changes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMsgWarning->Add( m_buttonUndo, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	bSizerMain->Add( bSizerMsgWarning, 0, wxALL|wxEXPAND, 10 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerMain->Add( m_staticline2, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOrphanItems = new wxButton( this, wxID_ANY, _("Map Orphans"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOrphanItems->SetToolTip( _("If  some components are orphan (the linked symbol is found nowhere),\ntry to find a candidate having the same name in one of loaded symbol libraries") );
	
	bSizerButtons->Add( m_buttonOrphanItems, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizerButtons->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerApply = new wxButton( this, wxID_APPLY );
	m_sdbSizer->AddButton( m_sdbSizerApply );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	
	bSizerButtons->Add( m_sdbSizer, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizerMain->Add( bSizerButtons, 0, wxEXPAND|wxLEFT, 5 );
	
	
	this->SetSizer( bSizerMain );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_grid->Connect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onCellBrowseLib ), NULL, this );
	m_grid->Connect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::OnSizeGrid ), NULL, this );
	m_buttonUndo->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onUndoChangesButton ), NULL, this );
	m_buttonUndo->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::updateUIChangesButton ), NULL, this );
	m_buttonOrphanItems->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onClickOrphansButton ), NULL, this );
	m_sdbSizerApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onApplyButton ), NULL, this );
	m_sdbSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onCancel ), NULL, this );
}

DIALOG_EDIT_COMPONENTS_LIBID_BASE::~DIALOG_EDIT_COMPONENTS_LIBID_BASE()
{
	// Disconnect Events
	m_grid->Disconnect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onCellBrowseLib ), NULL, this );
	m_grid->Disconnect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::OnSizeGrid ), NULL, this );
	m_buttonUndo->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onUndoChangesButton ), NULL, this );
	m_buttonUndo->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::updateUIChangesButton ), NULL, this );
	m_buttonOrphanItems->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onClickOrphansButton ), NULL, this );
	m_sdbSizerApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onApplyButton ), NULL, this );
	m_sdbSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENTS_LIBID_BASE::onCancel ), NULL, this );
	
}
