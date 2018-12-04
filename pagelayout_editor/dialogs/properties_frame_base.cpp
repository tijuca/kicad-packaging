///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 11 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "properties_frame_base.h"

///////////////////////////////////////////////////////////////////////////

PANEL_PROPERTIES_BASE::PANEL_PROPERTIES_BASE( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizerpanel;
	bSizerpanel = new wxBoxSizer( wxVERTICAL );
	
	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_swItemProperties = new wxScrolledWindow( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTAB_TRAVERSAL|wxVSCROLL );
	m_swItemProperties->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerButt;
	bSizerButt = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerType;
	bSizerType = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextType = new wxStaticText( m_swItemProperties, wxID_ANY, _("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextType->Wrap( -1 );
	m_staticTextType->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizerType->Add( m_staticTextType, 0, wxLEFT|wxRIGHT, 5 );
	
	m_textCtrlType = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizerType->Add( m_textCtrlType, 0, wxRIGHT|wxLEFT, 5 );
	
	
	bSizerButt->Add( bSizerType, 0, 0, 5 );
	
	wxBoxSizer* bSizerPageOpt;
	bSizerPageOpt = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextPageOpt = new wxStaticText( m_swItemProperties, wxID_ANY, _("Page 1 option:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPageOpt->Wrap( -1 );
	m_staticTextPageOpt->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	
	bSizerPageOpt->Add( m_staticTextPageOpt, 0, wxLEFT|wxRIGHT, 5 );
	
	wxString m_choicePageOptChoices[] = { _("None"), _("Page 1 only"), _("Not on page 1") };
	int m_choicePageOptNChoices = sizeof( m_choicePageOptChoices ) / sizeof( wxString );
	m_choicePageOpt = new wxChoice( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choicePageOptNChoices, m_choicePageOptChoices, 0 );
	m_choicePageOpt->SetSelection( 2 );
	bSizerPageOpt->Add( m_choicePageOpt, 0, wxRIGHT|wxLEFT, 5 );
	
	
	bSizerButt->Add( bSizerPageOpt, 0, 0, 5 );
	
	
	bSizerMain->Add( bSizerButt, 0, 0, 5 );
	
	m_staticline5 = new wxStaticLine( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerMain->Add( m_staticline5, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_SizerTextOptions = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextText = new wxStaticText( m_swItemProperties, wxID_ANY, _("Text:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextText->Wrap( -1 );
	m_SizerTextOptions->Add( m_staticTextText, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textCtrlText = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_SizerTextOptions->Add( m_textCtrlText, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	wxBoxSizer* bSizerFontOpt;
	bSizerFontOpt = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextHjust = new wxStaticText( m_swItemProperties, wxID_ANY, _("Horizontal align:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextHjust->Wrap( -1 );
	fgSizer1->Add( m_staticTextHjust, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	wxString m_choiceHjustifyChoices[] = { _("Left"), _("Center"), _("Right") };
	int m_choiceHjustifyNChoices = sizeof( m_choiceHjustifyChoices ) / sizeof( wxString );
	m_choiceHjustify = new wxChoice( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceHjustifyNChoices, m_choiceHjustifyChoices, 0 );
	m_choiceHjustify->SetSelection( 0 );
	fgSizer1->Add( m_choiceHjustify, 0, wxEXPAND|wxALL, 5 );
	
	m_checkBoxBold = new wxCheckBox( m_swItemProperties, wxID_ANY, _("Bold"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_checkBoxBold, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticTextVjust = new wxStaticText( m_swItemProperties, wxID_ANY, _("Vertical align:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextVjust->Wrap( -1 );
	fgSizer1->Add( m_staticTextVjust, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	wxString m_choiceVjustifyChoices[] = { _("Top"), _("Center"), _("Bottom") };
	int m_choiceVjustifyNChoices = sizeof( m_choiceVjustifyChoices ) / sizeof( wxString );
	m_choiceVjustify = new wxChoice( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceVjustifyNChoices, m_choiceVjustifyChoices, 0 );
	m_choiceVjustify->SetSelection( 1 );
	fgSizer1->Add( m_choiceVjustify, 0, wxEXPAND|wxALL, 5 );
	
	m_checkBoxItalic = new wxCheckBox( m_swItemProperties, wxID_ANY, _("Italic"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_checkBoxItalic, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	
	bSizerFontOpt->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	
	m_SizerTextOptions->Add( bSizerFontOpt, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableCol( 2 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTexTsizeX = new wxStaticText( m_swItemProperties, wxID_ANY, _("Text width:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTexTsizeX->Wrap( -1 );
	fgSizer2->Add( m_staticTexTsizeX, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextTsizeY = new wxStaticText( m_swItemProperties, wxID_ANY, _("Text height:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextTsizeY->Wrap( -1 );
	fgSizer2->Add( m_staticTextTsizeY, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlTextSizeX = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlTextSizeX, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	m_units1 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units1->Wrap( -1 );
	fgSizer2->Add( m_units1, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_textCtrlTextSizeY = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlTextSizeY, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	m_units11 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units11->Wrap( -1 );
	fgSizer2->Add( m_units11, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_staticTextConstraints = new wxStaticText( m_swItemProperties, wxID_ANY, _("Constraints:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextConstraints->Wrap( -1 );
	fgSizer2->Add( m_staticTextConstraints, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextConstraintX = new wxStaticText( m_swItemProperties, wxID_ANY, _("Maximum width:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextConstraintX->Wrap( -1 );
	fgSizer2->Add( m_staticTextConstraintX, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextConstraintY = new wxStaticText( m_swItemProperties, wxID_ANY, _("Maximum height:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextConstraintY->Wrap( -1 );
	fgSizer2->Add( m_staticTextConstraintY, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlConstraintX = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlConstraintX, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_units111 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units111->Wrap( -1 );
	fgSizer2->Add( m_units111, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_textCtrlConstraintY = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlConstraintY, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_units1111 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units1111->Wrap( -1 );
	fgSizer2->Add( m_units1111, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	
	m_SizerTextOptions->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_staticline6 = new wxStaticLine( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_SizerTextOptions->Add( m_staticline6, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	
	bSizerMain->Add( m_SizerTextOptions, 0, wxEXPAND, 5 );
	
	m_buttonOK = new wxButton( m_swItemProperties, wxID_ANY, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault();
	
	bSizerMain->Add( m_buttonOK, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline8 = new wxStaticLine( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerMain->Add( m_staticline8, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_staticTextComment = new wxStaticText( m_swItemProperties, wxID_ANY, _("Comment:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextComment->Wrap( -1 );
	bSizerMain->Add( m_staticTextComment, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textCtrlComment = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( m_textCtrlComment, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerMain->Add( m_staticline2, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizerPos;
	bSizerPos = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextPosX = new wxStaticText( m_swItemProperties, wxID_ANY, _("Start X:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPosX->Wrap( -1 );
	fgSizer3->Add( m_staticTextPosX, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlPosX = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlPosX, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_units12 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units12->Wrap( -1 );
	fgSizer3->Add( m_units12, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );
	
	m_staticTextPosY = new wxStaticText( m_swItemProperties, wxID_ANY, _("Start Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPosY->Wrap( -1 );
	fgSizer3->Add( m_staticTextPosY, 0, wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlPosY = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlPosY, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_units121 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units121->Wrap( -1 );
	fgSizer3->Add( m_units121, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );
	
	
	bSizerPos->Add( fgSizer3, 1, wxEXPAND|wxRIGHT, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextOrgPos = new wxStaticText( m_swItemProperties, wxID_ANY, _("Origin:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOrgPos->Wrap( -1 );
	bSizer6->Add( m_staticTextOrgPos, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_comboBoxCornerPos = new wxComboBox( m_swItemProperties, wxID_ANY, _("Lower Right"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBoxCornerPos->Append( _("Upper Right") );
	m_comboBoxCornerPos->Append( _("Upper Left") );
	m_comboBoxCornerPos->Append( _("Lower Right") );
	m_comboBoxCornerPos->Append( _("Lower Left") );
	m_comboBoxCornerPos->SetSelection( 2 );
	bSizer6->Add( m_comboBoxCornerPos, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	
	bSizerPos->Add( bSizer6, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizerMain->Add( bSizerPos, 0, wxEXPAND, 5 );
	
	m_SizerEndPosition = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextEndX = new wxStaticText( m_swItemProperties, wxID_ANY, _("End X:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextEndX->Wrap( -1 );
	fgSizer4->Add( m_staticTextEndX, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlEndX = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textCtrlEndX, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_units1211 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units1211->Wrap( -1 );
	fgSizer4->Add( m_units1211, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );
	
	m_staticTextEndY = new wxStaticText( m_swItemProperties, wxID_ANY, _("End Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextEndY->Wrap( -1 );
	fgSizer4->Add( m_staticTextEndY, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlEndY = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textCtrlEndY, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_units12111 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units12111->Wrap( -1 );
	fgSizer4->Add( m_units12111, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );
	
	
	m_SizerEndPosition->Add( fgSizer4, 1, wxEXPAND|wxRIGHT, 5 );
	
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextOrgEnd = new wxStaticText( m_swItemProperties, wxID_ANY, _("Origin:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOrgEnd->Wrap( -1 );
	bSizer61->Add( m_staticTextOrgEnd, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_comboBoxCornerEnd = new wxComboBox( m_swItemProperties, wxID_ANY, _("Lower Left"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBoxCornerEnd->Append( _("Upper Right") );
	m_comboBoxCornerEnd->Append( _("Upper Left") );
	m_comboBoxCornerEnd->Append( _("Lower Right") );
	m_comboBoxCornerEnd->Append( _("Lower Left") );
	m_comboBoxCornerEnd->SetSelection( 3 );
	bSizer61->Add( m_comboBoxCornerEnd, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	
	m_SizerEndPosition->Add( bSizer61, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizerMain->Add( m_SizerEndPosition, 0, wxEXPAND, 5 );
	
	m_SizerLineThickness = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerThickness;
	bSizerThickness = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextThickness = new wxStaticText( m_swItemProperties, wxID_ANY, _("Thickness:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextThickness->Wrap( -1 );
	bSizerThickness->Add( m_staticTextThickness, 0, wxLEFT|wxRIGHT, 5 );
	
	m_textCtrlThickness = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerThickness->Add( m_textCtrlThickness, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	m_SizerLineThickness->Add( bSizerThickness, 0, wxEXPAND, 5 );
	
	m_staticTextInfoThickness = new wxStaticText( m_swItemProperties, wxID_ANY, _("Set to 0 to use default"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInfoThickness->Wrap( -1 );
	m_SizerLineThickness->Add( m_staticTextInfoThickness, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizerMain->Add( m_SizerLineThickness, 0, 0, 5 );
	
	m_SizerRotation = new wxBoxSizer( wxVERTICAL );
	
	m_staticline1 = new wxStaticLine( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_SizerRotation->Add( m_staticline1, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizerRotation;
	bSizerRotation = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextRot = new wxStaticText( m_swItemProperties, wxID_ANY, _("Rotation:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRot->Wrap( -1 );
	bSizerRotation->Add( m_staticTextRot, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlRotation = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerRotation->Add( m_textCtrlRotation, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxLEFT|wxRIGHT, 5 );
	
	
	m_SizerRotation->Add( bSizerRotation, 0, wxEXPAND, 5 );
	
	
	bSizerMain->Add( m_SizerRotation, 0, wxEXPAND, 5 );
	
	m_SizerBitmapPPI = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextBitmapPPI = new wxStaticText( m_swItemProperties, wxID_ANY, _("Bitmap PPI:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBitmapPPI->Wrap( -1 );
	m_SizerBitmapPPI->Add( m_staticTextBitmapPPI, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlBitmapPPI = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_SizerBitmapPPI->Add( m_textCtrlBitmapPPI, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxLEFT|wxRIGHT, 5 );
	
	
	bSizerMain->Add( m_SizerBitmapPPI, 0, wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( m_swItemProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerMain->Add( m_staticline4, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_staticTextRepeatPrms = new wxStaticText( m_swItemProperties, wxID_ANY, _("Repeat parameters:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRepeatPrms->Wrap( -1 );
	bSizerMain->Add( m_staticTextRepeatPrms, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer8->AddGrowableCol( 0 );
	fgSizer8->AddGrowableCol( 2 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextRepeatCnt = new wxStaticText( m_swItemProperties, wxID_ANY, _("Repeat count:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRepeatCnt->Wrap( -1 );
	fgSizer8->Add( m_staticTextRepeatCnt, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextInclabel = new wxStaticText( m_swItemProperties, wxID_ANY, _("Text Increment:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInclabel->Wrap( -1 );
	fgSizer8->Add( m_staticTextInclabel, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlRepeatCount = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_textCtrlRepeatCount, 0, wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	
	fgSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlTextIncrement = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_textCtrlTextIncrement, 0, wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	
	fgSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextStepX = new wxStaticText( m_swItemProperties, wxID_ANY, _("Step X:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextStepX->Wrap( -1 );
	fgSizer8->Add( m_staticTextStepX, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextStepY = new wxStaticText( m_swItemProperties, wxID_ANY, _("Step Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextStepY->Wrap( -1 );
	fgSizer8->Add( m_staticTextStepY, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlStepX = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_textCtrlStepX, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_units121111 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units121111->Wrap( -1 );
	fgSizer8->Add( m_units121111, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_textCtrlStepY = new wxTextCtrl( m_swItemProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_textCtrlStepY, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_units1211111 = new wxStaticText( m_swItemProperties, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units1211111->Wrap( -1 );
	fgSizer8->Add( m_units1211111, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	
	bSizerMain->Add( fgSizer8, 1, wxEXPAND, 5 );
	
	
	m_swItemProperties->SetSizer( bSizerMain );
	m_swItemProperties->Layout();
	bSizerMain->Fit( m_swItemProperties );
	m_notebook->AddPage( m_swItemProperties, _("Item Properties"), true );
	m_swGeneralOpts = new wxScrolledWindow( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTAB_TRAVERSAL|wxVSCROLL );
	m_swGeneralOpts->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizerGeneralOpts;
	bSizerGeneralOpts = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerGeneralOpts1;
	bSizerGeneralOpts1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextDefVal = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Default Values:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDefVal->Wrap( -1 );
	bSizerGeneralOpts1->Add( m_staticTextDefVal, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer5->AddGrowableCol( 0 );
	fgSizer5->AddGrowableCol( 2 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextDefTsX = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Text width:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDefTsX->Wrap( -1 );
	fgSizer5->Add( m_staticTextDefTsX, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextDefTsY = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Text height:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDefTsY->Wrap( -1 );
	fgSizer5->Add( m_staticTextDefTsY, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlDefaultTextSizeX = new wxTextCtrl( m_swGeneralOpts, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textCtrlDefaultTextSizeX, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	m_units14 = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units14->Wrap( -1 );
	fgSizer5->Add( m_units14, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_textCtrlDefaultTextSizeY = new wxTextCtrl( m_swGeneralOpts, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textCtrlDefaultTextSizeY, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	m_units141 = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units141->Wrap( -1 );
	fgSizer5->Add( m_units141, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_staticTextDefLineW = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Line thickness:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDefLineW->Wrap( -1 );
	fgSizer5->Add( m_staticTextDefLineW, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	
	fgSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText22 = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Text thickness:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer5->Add( m_staticText22, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	
	fgSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlDefaultLineWidth = new wxTextCtrl( m_swGeneralOpts, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textCtrlDefaultLineWidth, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_staticText221 = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	fgSizer5->Add( m_staticText221, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_textCtrlDefaultTextThickness = new wxTextCtrl( m_swGeneralOpts, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textCtrlDefaultTextThickness, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	
	fgSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	bSizerGeneralOpts1->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	
	bSizerGeneralOpts->Add( bSizerGeneralOpts1, 0, wxEXPAND, 5 );
	
	m_buttonDefault = new wxButton( m_swGeneralOpts, wxID_ANY, _("Set to Default"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerGeneralOpts->Add( m_buttonDefault, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline9 = new wxStaticLine( m_swGeneralOpts, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerGeneralOpts->Add( m_staticline9, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizerGeneraMargins;
	bSizerGeneraMargins = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextMargins = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Page Margins:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMargins->Wrap( -1 );
	bSizerGeneraMargins->Add( m_staticTextMargins, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer6->AddGrowableCol( 0 );
	fgSizer6->AddGrowableCol( 2 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextLeftMargin = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Left:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextLeftMargin->Wrap( -1 );
	fgSizer6->Add( m_staticTextLeftMargin, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextDefRightMargin = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Right:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDefRightMargin->Wrap( -1 );
	fgSizer6->Add( m_staticTextDefRightMargin, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlLeftMargin = new wxTextCtrl( m_swGeneralOpts, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlLeftMargin, 0, wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_units142 = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units142->Wrap( -1 );
	fgSizer6->Add( m_units142, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_textCtrlRightMargin = new wxTextCtrl( m_swGeneralOpts, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlRightMargin, 0, wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_units143 = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units143->Wrap( -1 );
	fgSizer6->Add( m_units143, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_staticTextTopMargin = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Top:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextTopMargin->Wrap( -1 );
	fgSizer6->Add( m_staticTextTopMargin, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextBottomMargin = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("Bottom:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBottomMargin->Wrap( -1 );
	fgSizer6->Add( m_staticTextBottomMargin, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_textCtrlTopMargin = new wxTextCtrl( m_swGeneralOpts, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlTopMargin, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_units1431 = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units1431->Wrap( -1 );
	fgSizer6->Add( m_units1431, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	m_textCtrlDefaultBottomMargin = new wxTextCtrl( m_swGeneralOpts, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlDefaultBottomMargin, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_units1432 = new wxStaticText( m_swGeneralOpts, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units1432->Wrap( -1 );
	fgSizer6->Add( m_units1432, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );
	
	
	bSizerGeneraMargins->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	
	bSizerGeneralOpts->Add( bSizerGeneraMargins, 0, wxEXPAND, 5 );
	
	m_buttonGeneralOptsOK = new wxButton( m_swGeneralOpts, wxID_ANY, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonGeneralOptsOK->SetDefault();
	
	bSizerGeneralOpts->Add( m_buttonGeneralOptsOK, 0, wxALL|wxEXPAND, 5 );
	
	
	m_swGeneralOpts->SetSizer( bSizerGeneralOpts );
	m_swGeneralOpts->Layout();
	bSizerGeneralOpts->Fit( m_swGeneralOpts );
	m_notebook->AddPage( m_swGeneralOpts, _("General Options"), false );
	
	bSizerpanel->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizerpanel );
	this->Layout();
	bSizerpanel->Fit( this );
	
	// Connect Events
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_PROPERTIES_BASE::OnAcceptPrms ), NULL, this );
	m_buttonDefault->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_PROPERTIES_BASE::OnSetDefaultValues ), NULL, this );
	m_buttonGeneralOptsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_PROPERTIES_BASE::OnAcceptPrms ), NULL, this );
}

PANEL_PROPERTIES_BASE::~PANEL_PROPERTIES_BASE()
{
	// Disconnect Events
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_PROPERTIES_BASE::OnAcceptPrms ), NULL, this );
	m_buttonDefault->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_PROPERTIES_BASE::OnSetDefaultValues ), NULL, this );
	m_buttonGeneralOptsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_PROPERTIES_BASE::OnAcceptPrms ), NULL, this );
	
}
