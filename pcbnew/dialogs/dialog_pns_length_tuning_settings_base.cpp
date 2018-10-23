///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 17 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "widgets/text_ctrl_eval.h"

#include "dialog_pns_length_tuning_settings_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_PNS_LENGTH_TUNING_SETTINGS_BASE::DIALOG_PNS_LENGTH_TUNING_SETTINGS_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Length / Skew") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerLenSkew;
	fgSizerLenSkew = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizerLenSkew->AddGrowableCol( 1 );
	fgSizerLenSkew->SetFlexibleDirection( wxBOTH );
	fgSizerLenSkew->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Tune from:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizerLenSkew->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_choicePathFromChoices;
	m_choicePathFrom = new wxChoice( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choicePathFromChoices, 0 );
	m_choicePathFrom->SetSelection( 0 );
	fgSizerLenSkew->Add( m_choicePathFrom, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizerLenSkew->Add( 0, 0, 0, 0, 5 );
	
	m_staticText15 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Tune to:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizerLenSkew->Add( m_staticText15, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_choice4Choices;
	m_choice4 = new wxChoice( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice4Choices, 0 );
	m_choice4->SetSelection( 0 );
	fgSizerLenSkew->Add( m_choice4, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizerLenSkew->Add( 0, 0, 0, 0, 5 );
	
	m_staticText3 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Constraint:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizerLenSkew->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxString m_constraintSourceChoices[] = { _("From Design Rules"), _("Manual") };
	int m_constraintSourceNChoices = sizeof( m_constraintSourceChoices ) / sizeof( wxString );
	m_constraintSource = new wxChoice( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_constraintSourceNChoices, m_constraintSourceChoices, 0 );
	m_constraintSource->SetSelection( 1 );
	m_constraintSource->Enable( false );
	
	fgSizerLenSkew->Add( m_constraintSource, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizerLenSkew->Add( 0, 0, 0, 0, 5 );
	
	m_targetLengthLabel = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Target length:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_targetLengthLabel->Wrap( -1 );
	fgSizerLenSkew->Add( m_targetLengthLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_targetLengthText = new TEXT_CTRL_EVAL( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerLenSkew->Add( m_targetLengthText, 0, wxALL|wxEXPAND, 5 );
	
	m_targetLengthUnit = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_targetLengthUnit->Wrap( -1 );
	fgSizerLenSkew->Add( m_targetLengthUnit, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	
	sbSizer1->Add( fgSizerLenSkew, 1, wxEXPAND, 5 );
	
	
	bMainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 10 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Meandering") ), wxVERTICAL );
	
	m_legend = new wxStaticBitmap( sbSizer2->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_legend, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText9 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Min amplitude (Amin):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer3->Add( m_staticText9, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_minAmplText = new TEXT_CTRL_EVAL( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_minAmplText, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_minAmplUnit = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_minAmplUnit->Wrap( -1 );
	fgSizer3->Add( m_minAmplUnit, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT, 5 );
	
	m_staticText91 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Max amplitude (Amax):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText91->Wrap( -1 );
	fgSizer3->Add( m_staticText91, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_maxAmplText = new TEXT_CTRL_EVAL( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_maxAmplText, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_maxAmplUnit = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_maxAmplUnit->Wrap( -1 );
	fgSizer3->Add( m_maxAmplUnit, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT, 5 );
	
	m_staticText11 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Spacing (s):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer3->Add( m_staticText11, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_spacingText = new TEXT_CTRL_EVAL( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_spacingText, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_spacingUnit = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_spacingUnit->Wrap( -1 );
	fgSizer3->Add( m_spacingUnit, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT, 5 );
	
	m_staticText14 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Miter style:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer3->Add( m_staticText14, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxString m_miterStyleChoices[] = { _("45 degree"), _("arc") };
	int m_miterStyleNChoices = sizeof( m_miterStyleChoices ) / sizeof( wxString );
	m_miterStyle = new wxChoice( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_miterStyleNChoices, m_miterStyleChoices, 0 );
	m_miterStyle->SetSelection( 0 );
	fgSizer3->Add( m_miterStyle, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText13 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Miter radius (r):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer3->Add( m_staticText13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_radiusText = new TEXT_CTRL_EVAL( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_radiusText, 0, wxALL|wxEXPAND, 5 );
	
	m_radiusUnit = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radiusUnit->Wrap( -1 );
	fgSizer3->Add( m_radiusUnit, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	
	sbSizer2->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	
	bMainSizer->Add( sbSizer2, 0, wxEXPAND|wxRIGHT|wxLEFT, 10 );
	
	m_stdButtons = new wxStdDialogButtonSizer();
	m_stdButtonsOK = new wxButton( this, wxID_OK );
	m_stdButtons->AddButton( m_stdButtonsOK );
	m_stdButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_stdButtons->AddButton( m_stdButtonsCancel );
	m_stdButtons->Realize();
	
	bMainSizer->Add( m_stdButtons, 0, wxEXPAND|wxALL, 5 );
	
	
	this->SetSizer( bMainSizer );
	this->Layout();
}

DIALOG_PNS_LENGTH_TUNING_SETTINGS_BASE::~DIALOG_PNS_LENGTH_TUNING_SETTINGS_BASE()
{
}
