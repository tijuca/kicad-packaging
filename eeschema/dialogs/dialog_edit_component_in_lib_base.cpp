///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 20 2019)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "widgets/wx_grid.h"

#include "dialog_edit_component_in_lib_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bUpperSizer;
	bUpperSizer = new wxBoxSizer( wxVERTICAL );

	m_NoteBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0|wxTAB_TRAVERSAL );
	m_PanelBasic = new wxPanel( m_NoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerBasicPanel;
	bSizerBasicPanel = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_PanelBasic, wxID_ANY, _("Fields") ), wxVERTICAL );

	m_grid = new WX_GRID( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	m_grid->CreateGrid( 4, 11 );
	m_grid->EnableEditing( true );
	m_grid->EnableGridLines( true );
	m_grid->EnableDragGridSize( false );
	m_grid->SetMargins( 0, 0 );

	// Columns
	m_grid->SetColSize( 0, 72 );
	m_grid->SetColSize( 1, 120 );
	m_grid->SetColSize( 2, 48 );
	m_grid->SetColSize( 3, 72 );
	m_grid->SetColSize( 4, 72 );
	m_grid->SetColSize( 5, 48 );
	m_grid->SetColSize( 6, 48 );
	m_grid->SetColSize( 7, 84 );
	m_grid->SetColSize( 8, 84 );
	m_grid->SetColSize( 9, 84 );
	m_grid->SetColSize( 10, 84 );
	m_grid->EnableDragColMove( false );
	m_grid->EnableDragColSize( true );
	m_grid->SetColLabelSize( 22 );
	m_grid->SetColLabelValue( 0, _("Name") );
	m_grid->SetColLabelValue( 1, _("Value") );
	m_grid->SetColLabelValue( 2, _("Show") );
	m_grid->SetColLabelValue( 3, _("H Align") );
	m_grid->SetColLabelValue( 4, _("V Align") );
	m_grid->SetColLabelValue( 5, _("Italic") );
	m_grid->SetColLabelValue( 6, _("Bold") );
	m_grid->SetColLabelValue( 7, _("Text Size") );
	m_grid->SetColLabelValue( 8, _("Orientation") );
	m_grid->SetColLabelValue( 9, _("X Position") );
	m_grid->SetColLabelValue( 10, _("Y Position") );
	m_grid->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	m_grid->EnableDragRowSize( true );
	m_grid->SetRowLabelSize( 0 );
	m_grid->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	m_grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	m_grid->SetMinSize( wxSize( -1,180 ) );

	sbSizer4->Add( m_grid, 0, wxTOP|wxBOTTOM|wxEXPAND, 5 );

	wxBoxSizer* bButtonSize;
	bButtonSize = new wxBoxSizer( wxHORIZONTAL );

	m_bpAdd = new wxBitmapButton( sbSizer4->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_bpAdd->SetToolTip( _("Add field") );
	m_bpAdd->SetMinSize( wxSize( 30,29 ) );

	bButtonSize->Add( m_bpAdd, 0, wxRIGHT|wxLEFT, 5 );

	m_bpMoveUp = new wxBitmapButton( sbSizer4->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_bpMoveUp->SetToolTip( _("Move up") );
	m_bpMoveUp->SetMinSize( wxSize( 30,29 ) );

	bButtonSize->Add( m_bpMoveUp, 0, wxRIGHT, 5 );

	m_bpMoveDown = new wxBitmapButton( sbSizer4->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_bpMoveDown->SetToolTip( _("Move down") );
	m_bpMoveDown->SetMinSize( wxSize( 30,29 ) );

	bButtonSize->Add( m_bpMoveDown, 0, wxRIGHT, 5 );


	bButtonSize->Add( 0, 0, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_bpDelete = new wxBitmapButton( sbSizer4->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_bpDelete->SetToolTip( _("Delete field") );
	m_bpDelete->SetMinSize( wxSize( 30,29 ) );

	bButtonSize->Add( m_bpDelete, 0, wxRIGHT|wxLEFT, 10 );


	sbSizer4->Add( bButtonSize, 1, wxEXPAND|wxBOTTOM, 5 );


	bSizerBasicPanel->Add( sbSizer4, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bSizerMidBasicPanel;
	bSizerMidBasicPanel = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizerFPID;
	fgSizerFPID = new wxFlexGridSizer( 4, 2, 3, 0 );
	fgSizerFPID->AddGrowableCol( 1 );
	fgSizerFPID->SetFlexibleDirection( wxBOTH );
	fgSizerFPID->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText* staticNameLabel;
	staticNameLabel = new wxStaticText( m_PanelBasic, wxID_ANY, _("Symbol name:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticNameLabel->Wrap( -1 );
	fgSizerFPID->Add( staticNameLabel, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );

	m_SymbolNameCtrl = new wxTextCtrl( m_PanelBasic, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerFPID->Add( m_SymbolNameCtrl, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	wxStaticText* staticDescriptionLabel;
	staticDescriptionLabel = new wxStaticText( m_PanelBasic, wxID_ANY, _("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticDescriptionLabel->Wrap( -1 );
	fgSizerFPID->Add( staticDescriptionLabel, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );

	m_DescCtrl = new wxTextCtrl( m_PanelBasic, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerFPID->Add( m_DescCtrl, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	staticKeywordsLabel = new wxStaticText( m_PanelBasic, wxID_ANY, _("Keywords:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticKeywordsLabel->Wrap( -1 );
	fgSizerFPID->Add( staticKeywordsLabel, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );

	m_KeywordCtrl = new wxTextCtrl( m_PanelBasic, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerFPID->Add( m_KeywordCtrl, 0, wxRIGHT|wxLEFT|wxEXPAND, 5 );


	bSizerMidBasicPanel->Add( fgSizerFPID, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );


	bSizerBasicPanel->Add( bSizerMidBasicPanel, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bSizerLowerBasicPanel;
	bSizerLowerBasicPanel = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizerLeftCol;
	bSizerLeftCol = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizerSymbol;
	sbSizerSymbol = new wxStaticBoxSizer( new wxStaticBox( m_PanelBasic, wxID_ANY, _("Symbol") ), wxVERTICAL );

	m_AsConvertButt = new wxCheckBox( sbSizerSymbol->GetStaticBox(), wxID_ANY, _("Has alternate symbol (DeMorgan)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_AsConvertButt->SetToolTip( _("Check this option if the symbol has an alternate body style (De Morgan)") );

	sbSizerSymbol->Add( m_AsConvertButt, 0, wxRIGHT|wxLEFT, 5 );

	m_OptionPower = new wxCheckBox( sbSizerSymbol->GetStaticBox(), wxID_ANY, _("Define as power symbol"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OptionPower->SetToolTip( _("Setting this option makes the symbol in question appear in the\n\"add power port\" dialog.  It will lock the value text to protect it\nfrom editing in Eeschema.  The symbol will not be included in\nthe BOM and cannot be assigned a footprint.") );

	sbSizerSymbol->Add( m_OptionPower, 0, wxALL, 5 );

	wxBoxSizer* bSizerUnitCount;
	bSizerUnitCount = new wxBoxSizer( wxHORIZONTAL );

	m_staticTextNbUnits = new wxStaticText( sbSizerSymbol->GetStaticBox(), wxID_ANY, _("Number of Units:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextNbUnits->Wrap( -1 );
	m_staticTextNbUnits->SetToolTip( _("Enter the number of units for a symbol that contains more than one unit") );

	bSizerUnitCount->Add( m_staticTextNbUnits, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );

	m_SelNumberOfUnits = new wxSpinCtrl( sbSizerSymbol->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 64, 1 );
	bSizerUnitCount->Add( m_SelNumberOfUnits, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5 );


	sbSizerSymbol->Add( bSizerUnitCount, 1, wxEXPAND, 5 );

	m_OptionPartsLocked = new wxCheckBox( sbSizerSymbol->GetStaticBox(), wxID_ANY, _("All units are not interchangeable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OptionPartsLocked->SetToolTip( _("Check this option when creating multiple unit symbols and all units are not interchangeable") );

	sbSizerSymbol->Add( m_OptionPartsLocked, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );


	bSizerLeftCol->Add( sbSizerSymbol, 0, wxEXPAND|wxALL, 5 );


	bSizerLowerBasicPanel->Add( bSizerLeftCol, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizerRightCol;
	bSizerRightCol = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizerAnnotations;
	sbSizerAnnotations = new wxStaticBoxSizer( new wxStaticBox( m_PanelBasic, wxID_ANY, _("Annotations") ), wxVERTICAL );

	m_ShowPinNumButt = new wxCheckBox( sbSizerAnnotations->GetStaticBox(), wxID_ANY, _("Show pin number"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShowPinNumButt->SetValue(true);
	m_ShowPinNumButt->SetToolTip( _("Show or hide pin numbers") );

	sbSizerAnnotations->Add( m_ShowPinNumButt, 0, wxRIGHT|wxLEFT, 5 );

	m_ShowPinNameButt = new wxCheckBox( sbSizerAnnotations->GetStaticBox(), wxID_ANY, _("Show pin name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShowPinNameButt->SetValue(true);
	m_ShowPinNameButt->SetToolTip( _("Show or hide pin names") );

	sbSizerAnnotations->Add( m_ShowPinNameButt, 0, wxTOP|wxRIGHT|wxLEFT, 5 );


	sbSizerAnnotations->Add( 0, 0, 1, wxEXPAND, 5 );

	m_PinsNameInsideButt = new wxCheckBox( sbSizerAnnotations->GetStaticBox(), wxID_ANY, _("Place pin names inside"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PinsNameInsideButt->SetValue(true);
	m_PinsNameInsideButt->SetToolTip( _("Check this option to have pin names inside the body and pin number outside.\nIf not checked pins names and pins numbers are outside.") );

	sbSizerAnnotations->Add( m_PinsNameInsideButt, 0, wxALL, 5 );

	wxBoxSizer* bSizerNameOffset;
	bSizerNameOffset = new wxBoxSizer( wxHORIZONTAL );

	m_nameOffsetLabel = new wxStaticText( sbSizerAnnotations->GetStaticBox(), wxID_ANY, _("Position offset:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_nameOffsetLabel->Wrap( -1 );
	m_nameOffsetLabel->SetToolTip( _("Margin (in 0.001 inches) between a pin name position and the component body.\nA value from 10 to 40 is usually good.") );

	bSizerNameOffset->Add( m_nameOffsetLabel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 30 );

	m_nameOffsetCtrl = new wxTextCtrl( sbSizerAnnotations->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerNameOffset->Add( m_nameOffsetCtrl, 1, wxLEFT|wxRIGHT, 5 );

	m_nameOffsetUnits = new wxStaticText( sbSizerAnnotations->GetStaticBox(), wxID_ANY, _("units"), wxDefaultPosition, wxDefaultSize, 0 );
	m_nameOffsetUnits->Wrap( -1 );
	bSizerNameOffset->Add( m_nameOffsetUnits, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );


	sbSizerAnnotations->Add( bSizerNameOffset, 0, wxEXPAND, 5 );


	sbSizerAnnotations->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizerRightCol->Add( sbSizerAnnotations, 1, wxEXPAND|wxALL, 5 );


	bSizerLowerBasicPanel->Add( bSizerRightCol, 1, wxEXPAND, 5 );


	bSizerBasicPanel->Add( bSizerLowerBasicPanel, 0, wxEXPAND, 5 );


	m_PanelBasic->SetSizer( bSizerBasicPanel );
	m_PanelBasic->Layout();
	bSizerBasicPanel->Fit( m_PanelBasic );
	m_NoteBook->AddPage( m_PanelBasic, _("General"), true );
	m_PanelAlias = new wxPanel( m_NoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerMainPanelAlias;
	bSizerMainPanelAlias = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bLeftBoxSizerPanelAlias;
	bLeftBoxSizerPanelAlias = new wxBoxSizer( wxVERTICAL );

	wxStaticText* staticAliasesLabel;
	staticAliasesLabel = new wxStaticText( m_PanelAlias, wxID_ANY, _("Aliases:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticAliasesLabel->Wrap( -1 );
	bLeftBoxSizerPanelAlias->Add( staticAliasesLabel, 0, wxRIGHT|wxLEFT, 5 );

	m_aliasListBox = new wxListBox( m_PanelAlias, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bLeftBoxSizerPanelAlias->Add( m_aliasListBox, 1, wxEXPAND|wxLEFT, 5 );

	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );

	m_addAliasButton = new wxBitmapButton( m_PanelAlias, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_addAliasButton->SetToolTip( _("Add alias") );
	m_addAliasButton->SetMinSize( wxSize( 30,29 ) );

	bSizerButtons->Add( m_addAliasButton, 0, wxALL, 5 );


	bSizerButtons->Add( 0, 0, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_deleteAliasButton = new wxBitmapButton( m_PanelAlias, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_deleteAliasButton->SetToolTip( _("Delete alias") );
	m_deleteAliasButton->SetMinSize( wxSize( 30,29 ) );

	bSizerButtons->Add( m_deleteAliasButton, 0, wxALL, 5 );


	bLeftBoxSizerPanelAlias->Add( bSizerButtons, 0, wxEXPAND, 5 );


	bSizerMainPanelAlias->Add( bLeftBoxSizerPanelAlias, 2, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bRightBoxSizerPanelAlias;
	bRightBoxSizerPanelAlias = new wxBoxSizer( wxVERTICAL );

	m_staticText12 = new wxStaticText( m_PanelAlias, wxID_ANY, _("Alias field substitutions:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bRightBoxSizerPanelAlias->Add( m_staticText12, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_aliasGrid = new WX_GRID( m_PanelAlias, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	m_aliasGrid->CreateGrid( 4, 2 );
	m_aliasGrid->EnableEditing( true );
	m_aliasGrid->EnableGridLines( true );
	m_aliasGrid->EnableDragGridSize( false );
	m_aliasGrid->SetMargins( 0, 0 );

	// Columns
	m_aliasGrid->SetColSize( 0, 72 );
	m_aliasGrid->SetColSize( 1, 420 );
	m_aliasGrid->EnableDragColMove( false );
	m_aliasGrid->EnableDragColSize( true );
	m_aliasGrid->SetColLabelSize( 22 );
	m_aliasGrid->SetColLabelValue( 0, _("Name") );
	m_aliasGrid->SetColLabelValue( 1, _("Value") );
	m_aliasGrid->SetColLabelValue( 2, _("Show") );
	m_aliasGrid->SetColLabelValue( 3, _("H Align") );
	m_aliasGrid->SetColLabelValue( 4, _("V Align") );
	m_aliasGrid->SetColLabelValue( 5, _("Italic") );
	m_aliasGrid->SetColLabelValue( 6, _("Bold") );
	m_aliasGrid->SetColLabelValue( 7, _("Text Size") );
	m_aliasGrid->SetColLabelValue( 8, _("Orientation") );
	m_aliasGrid->SetColLabelValue( 9, _("X Position") );
	m_aliasGrid->SetColLabelValue( 10, _("Y Position") );
	m_aliasGrid->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	m_aliasGrid->EnableDragRowSize( true );
	m_aliasGrid->SetRowLabelSize( 0 );
	m_aliasGrid->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	m_aliasGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bRightBoxSizerPanelAlias->Add( m_aliasGrid, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );


	bRightBoxSizerPanelAlias->Add( 0, 0, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );

	wxFlexGridSizer* fgSizerFPID1;
	fgSizerFPID1 = new wxFlexGridSizer( 6, 1, 0, 0 );
	fgSizerFPID1->AddGrowableCol( 0 );
	fgSizerFPID1->SetFlexibleDirection( wxBOTH );
	fgSizerFPID1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText* staticAliasNameLabel;
	staticAliasNameLabel = new wxStaticText( m_PanelAlias, wxID_ANY, _("Alias name:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticAliasNameLabel->Wrap( -1 );
	fgSizerFPID1->Add( staticAliasNameLabel, 0, wxLEFT|wxRIGHT, 5 );

	m_AliasNameCtrl = new wxTextCtrl( m_PanelAlias, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerFPID1->Add( m_AliasNameCtrl, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5 );

	wxStaticText* staticAliasDescLabel;
	staticAliasDescLabel = new wxStaticText( m_PanelAlias, wxID_ANY, _("Alias description:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticAliasDescLabel->Wrap( -1 );
	fgSizerFPID1->Add( staticAliasDescLabel, 0, wxLEFT|wxRIGHT, 5 );

	m_AliasDescCtrl = new wxTextCtrl( m_PanelAlias, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerFPID1->Add( m_AliasDescCtrl, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	staticAliasKeywordsLabel = new wxStaticText( m_PanelAlias, wxID_ANY, _("Alias keywords:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticAliasKeywordsLabel->Wrap( -1 );
	fgSizerFPID1->Add( staticAliasKeywordsLabel, 0, wxLEFT|wxRIGHT, 5 );

	m_AliasKeywordsCtrl = new wxTextCtrl( m_PanelAlias, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerFPID1->Add( m_AliasKeywordsCtrl, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );


	bRightBoxSizerPanelAlias->Add( fgSizerFPID1, 1, wxEXPAND, 5 );


	bSizerMainPanelAlias->Add( bRightBoxSizerPanelAlias, 7, wxEXPAND|wxRIGHT|wxTOP, 20 );


	m_PanelAlias->SetSizer( bSizerMainPanelAlias );
	m_PanelAlias->Layout();
	bSizerMainPanelAlias->Fit( m_PanelAlias );
	m_NoteBook->AddPage( m_PanelAlias, _("Aliases"), false );
	m_PanelFootprintFilter = new wxPanel( m_NoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bPanelFpFilterBoxSizer;
	bPanelFpFilterBoxSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bFpFilterLeftBoxSizer;
	bFpFilterLeftBoxSizer = new wxBoxSizer( wxVERTICAL );

	m_staticTextFootprints = new wxStaticText( m_PanelFootprintFilter, wxID_ANY, _("Footprint filters:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFootprints->Wrap( -1 );
	m_staticTextFootprints->SetToolTip( _("A list of footprints names that can be used for this symbol.\nFootprints names can used wildcards like sm* to allow all footprints names starting by sm.") );

	bFpFilterLeftBoxSizer->Add( m_staticTextFootprints, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_FootprintFilterListBox = new wxListBox( m_PanelFootprintFilter, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bFpFilterLeftBoxSizer->Add( m_FootprintFilterListBox, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bFpFilterRightBoxSizer;
	bFpFilterRightBoxSizer = new wxBoxSizer( wxHORIZONTAL );

	m_addFilterButton = new wxBitmapButton( m_PanelFootprintFilter, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 30,30 ), wxBU_AUTODRAW|0 );
	m_addFilterButton->SetToolTip( _("Add footprint filter") );

	bFpFilterRightBoxSizer->Add( m_addFilterButton, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_editFilterButton = new wxBitmapButton( m_PanelFootprintFilter, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 30,30 ), wxBU_AUTODRAW|0 );
	m_editFilterButton->SetToolTip( _("Edit footprint filter") );

	bFpFilterRightBoxSizer->Add( m_editFilterButton, 0, wxALL, 5 );


	bFpFilterRightBoxSizer->Add( 0, 0, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_deleteFilterButton = new wxBitmapButton( m_PanelFootprintFilter, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 30,30 ), wxBU_AUTODRAW|0 );
	m_deleteFilterButton->SetToolTip( _("Delete footprint filter") );

	bFpFilterRightBoxSizer->Add( m_deleteFilterButton, 0, wxALL, 5 );


	bFpFilterLeftBoxSizer->Add( bFpFilterRightBoxSizer, 0, 0, 5 );


	bPanelFpFilterBoxSizer->Add( bFpFilterLeftBoxSizer, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );


	m_PanelFootprintFilter->SetSizer( bPanelFpFilterBoxSizer );
	m_PanelFootprintFilter->Layout();
	bPanelFpFilterBoxSizer->Fit( m_PanelFootprintFilter );
	m_NoteBook->AddPage( m_PanelFootprintFilter, _("Footprint Filters"), false );

	bUpperSizer->Add( m_NoteBook, 1, wxEXPAND, 5 );


	bMainSizer->Add( bUpperSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxHORIZONTAL );


	bSizer101->Add( 0, 0, 1, wxEXPAND, 5 );

	m_spiceFieldsButton = new wxButton( this, wxID_ANY, _("Edit Spice Model..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_spiceFieldsButton, 0, wxEXPAND|wxALL, 5 );


	bSizer101->Add( 0, 0, 0, wxEXPAND|wxRIGHT|wxLEFT, 15 );

	m_stdSizerButton = new wxStdDialogButtonSizer();
	m_stdSizerButtonOK = new wxButton( this, wxID_OK );
	m_stdSizerButton->AddButton( m_stdSizerButtonOK );
	m_stdSizerButtonCancel = new wxButton( this, wxID_CANCEL );
	m_stdSizerButton->AddButton( m_stdSizerButtonCancel );
	m_stdSizerButton->Realize();

	bSizer101->Add( m_stdSizerButton, 0, wxEXPAND|wxALL, 5 );


	bMainSizer->Add( bSizer101, 0, wxEXPAND, 5 );


	this->SetSizer( bMainSizer );
	this->Layout();
	bMainSizer->Fit( this );

	// Connect Events
	this->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnUpdateUI ) );
	m_grid->Connect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSizeGrid ), NULL, this );
	m_bpAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAddField ), NULL, this );
	m_bpMoveUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnMoveUp ), NULL, this );
	m_bpMoveDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnMoveDown ), NULL, this );
	m_bpDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnDeleteField ), NULL, this );
	m_SymbolNameCtrl->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSymbolNameKillFocus ), NULL, this );
	m_SymbolNameCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSymbolNameText ), NULL, this );
	m_aliasListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSelectAlias ), NULL, this );
	m_addAliasButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAddAlias ), NULL, this );
	m_deleteAliasButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnDeleteAlias ), NULL, this );
	m_aliasGrid->Connect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSizeAliasGrid ), NULL, this );
	m_AliasNameCtrl->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAliasNameKillFocus ), NULL, this );
	m_AliasNameCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAliasNameText ), NULL, this );
	m_FootprintFilterListBox->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnFilterDClick ), NULL, this );
	m_FootprintFilterListBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnEditFootprintFilter ), NULL, this );
	m_addFilterButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAddFootprintFilter ), NULL, this );
	m_editFilterButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnEditFootprintFilter ), NULL, this );
	m_deleteFilterButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnDeleteFootprintFilter ), NULL, this );
	m_spiceFieldsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnEditSpiceModel ), NULL, this );
	m_stdSizerButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnCancelButtonClick ), NULL, this );
}

DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::~DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE()
{
	// Disconnect Events
	this->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnUpdateUI ) );
	m_grid->Disconnect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSizeGrid ), NULL, this );
	m_bpAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAddField ), NULL, this );
	m_bpMoveUp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnMoveUp ), NULL, this );
	m_bpMoveDown->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnMoveDown ), NULL, this );
	m_bpDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnDeleteField ), NULL, this );
	m_SymbolNameCtrl->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSymbolNameKillFocus ), NULL, this );
	m_SymbolNameCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSymbolNameText ), NULL, this );
	m_aliasListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSelectAlias ), NULL, this );
	m_addAliasButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAddAlias ), NULL, this );
	m_deleteAliasButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnDeleteAlias ), NULL, this );
	m_aliasGrid->Disconnect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnSizeAliasGrid ), NULL, this );
	m_AliasNameCtrl->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAliasNameKillFocus ), NULL, this );
	m_AliasNameCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAliasNameText ), NULL, this );
	m_FootprintFilterListBox->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnFilterDClick ), NULL, this );
	m_FootprintFilterListBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnEditFootprintFilter ), NULL, this );
	m_addFilterButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnAddFootprintFilter ), NULL, this );
	m_editFilterButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnEditFootprintFilter ), NULL, this );
	m_deleteFilterButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnDeleteFootprintFilter ), NULL, this );
	m_spiceFieldsButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnEditSpiceModel ), NULL, this );
	m_stdSizerButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_EDIT_COMPONENT_IN_LIBRARY_BASE::OnCancelButtonClick ), NULL, this );

}
