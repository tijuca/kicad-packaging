/**************************************************************/
/*	librairy editor: edition of component general properties  */
/**************************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"

#include "wx/spinctrl.h"


/* Routines locales */

/* Variables locales */

extern int CurrentUnit;

/* Classe de la frame des propri�t�s d'un composant en librairie */

enum id_libedit {
	ID_LIBEDIT_NOTEBOOK = 3200,
	ID_PANEL_BASIC,
	ID_PANEL_ALIAS,
	ID_PANEL_REFERENCE,
	ID_PANEL_VALUE,
	ID_PANEL_FOOTPRINT,
	ID_PANEL_SUBSCHEMATIC,
	ID_PANEL_FIELD1,
	ID_PANEL_FIELD2,
	ID_PANEL_FIELD3,
	ID_PANEL_FIELD4,
	ID_PANEL_FIELD5,
	ID_PANEL_FIELD6,
	ID_PANEL_FIELD7,
	ID_PANEL_FIELD8,
	ID_CLOSE_PART_PROPERTIES,
	ID_ACCEPT_PART_PROPERTIES,
	ID_COPY_DOC_TO_ALIAS,
	ID_BROWSE_DOC_FILES,
	ID_ADD_ALIAS,
	ID_DELETE_ONE_ALIAS,
	ID_DELETE_ALL_ALIAS
};


/************************************************/
class WinEDA_PartPropertiesFrame: public wxDialog
/************************************************/
/* Cette classe genere une fenetre type NoteBook, pour l'edition des propri�t�s
d'un composant le librairie.
	On peut �diter:
	Texte dimensions et justification de tous les champs (Ref, Val, et autres champs)
	Documentation et mots clefs
	Nombre de part par boitier
	et autres propri�r�s g�n�rales
*/
{
private:

	WinEDA_LibeditFrame * m_Parent;
	wxNotebook* m_NoteBook;
	wxListBox * m_PartAliasList;
	wxPanel * m_PanelBasic;
	wxPanel * m_PanelAlias;
	wxPanel * m_PanelDoc;
	wxPanel * PanelField[NUMBER_OF_FIELDS];

	wxCheckBox * AsConvertButt;

	wxCheckBox * ShowFieldText[NUMBER_OF_FIELDS];
	wxCheckBox * VorientFieldText[NUMBER_OF_FIELDS];
	wxCheckBox * ShowPinNumButt;
	wxCheckBox * ShowPinNameButt;
	wxCheckBox * m_PinsNameInsideButt;
	wxSpinCtrl * SelNumberOfUnits;
	wxSpinCtrl * m_SetSkew;
	wxCheckBox * m_OptionPower;
	wxCheckBox * m_OptionPartsLocked;

	WinEDA_GraphicTextCtrl * FieldTextCtrl[NUMBER_OF_FIELDS];
	WinEDA_PositionCtrl * FieldPosition[NUMBER_OF_FIELDS];
	int FieldFlags[NUMBER_OF_FIELDS];
	int FieldOrient[NUMBER_OF_FIELDS];
	wxRadioBox * FieldHJustify[NUMBER_OF_FIELDS];
	wxRadioBox * FieldVJustify[NUMBER_OF_FIELDS];
	WinEDA_EnterText * m_Doc;
	WinEDA_EnterText * m_Docfile;
	WinEDA_EnterText * m_Keywords;
	bool m_RecreateToolbar;
	int m_AliasLocation;


public:
	// Constructor and destructor
	WinEDA_PartPropertiesFrame(WinEDA_LibeditFrame *parent, wxPoint& pos);
	~WinEDA_PartPropertiesFrame(void)
	{
	};

private:
	void BuildPanelBasic(void);
	void BuildPanelDoc(void);
	void BuildPanelAlias(void);
	void BuildPanelEditField(int fieldId);
	void PartPropertiesAccept(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void DeleteAllAliasOfPart(wxCommandEvent& event);
	void DeleteAliasOfPart(wxCommandEvent& event);
	void AddAliasOfPart(wxCommandEvent& event);
	bool ChangeNbUnitsPerPackage(int newUnit);
	bool SetUnsetConvert(void);
	void CopyDocToAlias(wxCommandEvent& event);
	void BrowseAndSelectDocFile(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(WinEDA_PartPropertiesFrame, wxDialog)
	EVT_BUTTON(ID_ACCEPT_PART_PROPERTIES, WinEDA_PartPropertiesFrame::PartPropertiesAccept)
	EVT_BUTTON(ID_CLOSE_PART_PROPERTIES, WinEDA_PartPropertiesFrame::OnQuit)
	EVT_BUTTON(ID_ADD_ALIAS, WinEDA_PartPropertiesFrame::AddAliasOfPart)
	EVT_BUTTON(ID_DELETE_ONE_ALIAS, WinEDA_PartPropertiesFrame::DeleteAliasOfPart)
	EVT_BUTTON(ID_DELETE_ALL_ALIAS, WinEDA_PartPropertiesFrame::DeleteAllAliasOfPart)
	EVT_BUTTON(ID_COPY_DOC_TO_ALIAS, WinEDA_PartPropertiesFrame::CopyDocToAlias)
	EVT_BUTTON(ID_BROWSE_DOC_FILES, WinEDA_PartPropertiesFrame::BrowseAndSelectDocFile)
END_EVENT_TABLE()

/*****************************************************************/
void WinEDA_LibeditFrame::InstallLibeditFrame(const wxPoint & pos)
/*****************************************************************/
{
wxPoint fpos = pos;

	WinEDA_PartPropertiesFrame * frame =
			new WinEDA_PartPropertiesFrame(this, fpos);
	frame->ShowModal(); frame->Destroy();
}


#define XSIZE 370
#define YSIZE 385
/**********************************************************************************/
WinEDA_PartPropertiesFrame::WinEDA_PartPropertiesFrame(WinEDA_LibeditFrame *parent,
						wxPoint& framepos):
		wxDialog(parent, -1, wxEmptyString, framepos, wxSize(XSIZE, YSIZE),
				DIALOG_STYLE)
/**********************************************************************************/
{
wxPoint pos;
wxLayoutConstraints* c;
wxString msg_text;
wxButton * Button;
int ii;
	
	m_Parent = parent;
	m_RecreateToolbar = FALSE;
	SetFont(*g_DialogFont);

	SetAutoLayout(TRUE);
   
	for ( ii = 0; ii < NUMBER_OF_FIELDS; ii++ )
		FieldFlags[ii] = 0;

	m_AliasLocation = -1;
	if ( CurrentLibEntry )
	{
		msg_text = _("Properties for ");
		if ( ! CurrentAliasName.IsEmpty() )
		{
			m_AliasLocation = LocateAlias( CurrentLibEntry->m_AliasList, CurrentAliasName);
			SetTitle( msg_text + CurrentAliasName +
						_("(alias of ") +
						wxString(CurrentLibEntry->m_Name.m_Text)
						+ wxT(")") );
		}
		else
      	{
			SetTitle(  msg_text + CurrentLibEntry->m_Name.m_Text );
			CurrentAliasName.Empty();
		}

		FieldFlags[REFERENCE] = CurrentLibEntry->m_Prefix.m_Attributs;
		FieldOrient[REFERENCE] = CurrentLibEntry->m_Prefix.m_Orient;

		FieldFlags[VALUE] = CurrentLibEntry->m_Name.m_Attributs;
		FieldOrient[VALUE] = CurrentLibEntry->m_Name.m_Orient;

		LibDrawField * Field = CurrentLibEntry->Fields;
		while ( Field )
		{
			FieldFlags[Field->m_FieldId] = Field->m_Attributs;
			FieldOrient[Field->m_FieldId] = Field->m_Orient;
			Field = (LibDrawField*)Field->Pnext;
		}
	}

	else SetTitle(_("Lib Component Properties"));

	m_NoteBook = new wxNotebook(this, ID_LIBEDIT_NOTEBOOK,
						wxDefaultPosition,wxSize(XSIZE-6, YSIZE - 70) );
	SetFont(*g_DialogFont);
	m_NoteBook->SetAutoLayout(TRUE);
	c = new wxLayoutConstraints;
	c->left.SameAs(this, wxLeft, 4);
	c->right.SameAs(this, wxRight, 4);
	c->top.SameAs(this, wxTop, 4);
	c->bottom.SameAs(this, wxBottom, 40);
	m_NoteBook->SetConstraints(c);

	/* Creation des boutons de commande */
	pos.x = 50; pos.y = YSIZE - 60;
	Button = new wxButton(this, ID_CLOSE_PART_PROPERTIES,
						_("Cancel"), pos);
	Button->SetForegroundColour(*wxBLUE);
	c = new wxLayoutConstraints;
	c->left.SameAs(this, wxLeft, 20);
	c->height.AsIs();
	c->width.AsIs();
	c->bottom.SameAs(this, wxBottom, 5);
	Button->SetConstraints(c);

	pos.x += Button->GetDefaultSize().x + 70;
	Button = new wxButton(this, ID_ACCEPT_PART_PROPERTIES,
						_("Ok"), pos);
	Button->SetForegroundColour(*wxRED);
	c = new wxLayoutConstraints;
	c->right.SameAs(this, wxRight, 20);
	c->height.AsIs();
	c->width.AsIs();
	c->bottom.SameAs(this, wxBottom, 5);
	Button->SetConstraints(c);

	// Add panel Basic
	BuildPanelBasic();
	m_NoteBook->AddPage(m_PanelBasic, _("Options"), TRUE);

	
	// Add Panel Documentation
	BuildPanelDoc();
	m_NoteBook->AddPage(m_PanelDoc, _("Doc"), FALSE);

	// Add Panel Alias List
	BuildPanelAlias();
	m_NoteBook->AddPage(m_PanelAlias, _("Alias"), FALSE);

	// Add panel Fields
	for ( ii = 0; ii < NUMBER_OF_FIELDS; ii++) BuildPanelEditField(ii);
}

/*****************************************************/
void WinEDA_PartPropertiesFrame::BuildPanelAlias(void)
/*****************************************************/
/* create the panel for component alias list editing
*/
{
wxPoint pos;
wxButton * Button;

	m_PanelAlias = new wxPanel(m_NoteBook, -1);
	m_PanelAlias->SetFont(*g_DialogFont);
	wxLayoutConstraints * c = new wxLayoutConstraints;
	c->left.SameAs(m_NoteBook, wxLeft);
	c->right.SameAs(m_NoteBook, wxRight);
	c->bottom.SameAs(m_NoteBook, wxBottom);
	m_PanelAlias->SetConstraints(c);

	pos.x = 200; pos.y = 70;
	Button = new wxButton(m_PanelAlias, ID_ADD_ALIAS, _("Add"), pos);
	Button->SetForegroundColour(*wxBLUE);

	pos.y += Button->GetSize().y + 10;
	Button = new wxButton(m_PanelAlias, ID_DELETE_ONE_ALIAS,
						_("Delete"), pos);
	Button->SetForegroundColour(*wxRED);

	pos.y += Button->GetSize().y + 10;
	Button = new wxButton(m_PanelAlias, ID_DELETE_ALL_ALIAS,
						_("Delete All"), pos);
	Button->SetForegroundColour(*wxRED);
	if ( ! CurrentAliasName.IsEmpty() ) Button->Enable(FALSE);

	pos.x = 5; pos.y = 30;
	m_PartAliasList = new wxListBox(m_PanelAlias,
							-1,
							pos, wxSize(160,170),
							0,NULL,
							wxLB_ALWAYS_SB|wxLB_SINGLE);
	wxStaticText * Msg = new wxStaticText(m_PanelAlias, -1, _("Alias"),
							wxPoint(pos.x,pos.y - 20) );
	Msg->SetForegroundColour(wxColour(200,0,0) );

	/* lecture des noms des alias */
	if ( CurrentLibEntry )
	{
		for ( unsigned ii = 0; ii < CurrentLibEntry->m_AliasList.GetCount(); ii += ALIAS_NEXT)
			m_PartAliasList->Append(CurrentLibEntry->m_AliasList[ii+ALIAS_NAME]);
	}
}


/*****************************************************/
void WinEDA_PartPropertiesFrame::BuildPanelDoc(void)
/*****************************************************/
/* create the panel for component doc editing
*/
{
wxPoint pos;
wxButton * Button;
wxString msg_text;
	
	m_PanelDoc = new wxPanel(m_NoteBook, -1);
	m_PanelDoc->SetFont(*g_DialogFont);
	wxLayoutConstraints * c = new wxLayoutConstraints;
	c->left.SameAs(m_NoteBook, wxLeft);
	c->right.SameAs(m_NoteBook, wxRight);
	c->bottom.SameAs(m_NoteBook, wxBottom);
	m_PanelDoc->SetConstraints(c);

	pos.x = 5; pos.y = 15;

	if ( CurrentLibEntry )
	{
		msg_text = _("Properties for ");
		if ( ! CurrentAliasName.IsEmpty() )
		{
			msg_text += _("alias ");
			msg_text += CurrentAliasName;
		}
		else
		{
			msg_text += CurrentLibEntry->m_Name.m_Text;
		}
		wxStaticText * text = new wxStaticText(m_PanelDoc, -1, msg_text, pos);
		text->SetForegroundColour(*wxBLUE);
	}

	pos.y += 30;
	if ( m_AliasLocation >= 0 )
		msg_text = CurrentLibEntry->m_AliasList[m_AliasLocation+ALIAS_DOC];
	else
	{
		if (CurrentLibEntry && CurrentLibEntry->m_Doc)
		msg_text = CurrentLibEntry->m_Doc;
	}
	m_Doc = new WinEDA_EnterText(m_PanelDoc,
				_("Doc:"), msg_text,
				pos, wxSize(285,-1) );

	pos.y += 40;
	msg_text.Empty();
	if ( m_AliasLocation >= 0 )
		msg_text = CurrentLibEntry->m_AliasList[m_AliasLocation+ALIAS_KEYWORD];
	else
	{
		if (CurrentLibEntry ) msg_text = CurrentLibEntry->m_KeyWord;
	}
	m_Keywords = new WinEDA_EnterText(m_PanelDoc,
				_("Keywords:"), msg_text,
				pos, wxSize(285,-1) );
	pos.y += 40;
	msg_text.Empty();
	if ( m_AliasLocation >= 0 )
		msg_text = CurrentLibEntry->m_AliasList[m_AliasLocation+ALIAS_DOC_FILENAME];
	else
	{
		if (CurrentLibEntry) msg_text = CurrentLibEntry->m_DocFile;
	}
	m_Docfile = new WinEDA_EnterText(m_PanelDoc,
				_("DocFileName:"), msg_text,
				pos, wxSize(285,-1) );

	pos.y += 40;
	Button = new wxButton(m_PanelDoc, ID_COPY_DOC_TO_ALIAS,
						_("Copy Doc"), pos);
	Button->SetForegroundColour(*wxRED);
	if ( m_AliasLocation < 0 ) Button->Enable(FALSE);

	Button = new wxButton(m_PanelDoc, ID_BROWSE_DOC_FILES,
						_("Browse DocFiles"), wxPoint(pos.x + 140, pos.y) );
	Button->SetForegroundColour(*wxBLUE);
}


/*****************************************************/
void WinEDA_PartPropertiesFrame::BuildPanelBasic(void)
/*****************************************************/
/* create the basic panel for component properties editing
*/
{
wxPoint pos;
	
	m_PanelBasic = new wxPanel(m_NoteBook, ID_PANEL_BASIC);
	m_PanelBasic->SetFont(*g_DialogFont);
	wxLayoutConstraints * c = new wxLayoutConstraints;
	c->left.SameAs(m_NoteBook, wxLeft);
	c->right.SameAs(m_NoteBook, wxRight);
	c->bottom.SameAs(m_NoteBook, wxBottom);
	m_PanelBasic->SetConstraints(c);

	pos.x = 5; pos.y = 25;
	new wxStaticBox(m_PanelBasic, -1,_("General :"), pos, wxSize(250, 120));

	pos.x = 10; pos.y += 22;
	AsConvertButt = new wxCheckBox(m_PanelBasic,-1, _("As Convert"), pos);
	if ( g_AsDeMorgan ) AsConvertButt->SetValue(TRUE);

	pos.y += 20;
	ShowPinNumButt = new  wxCheckBox(m_PanelBasic,-1, _("Show Pin Num"), pos);
	if ( CurrentLibEntry )
	{
		if ( CurrentLibEntry->m_DrawPinNum ) ShowPinNumButt->SetValue(TRUE);
	}
	else  ShowPinNumButt->SetValue(TRUE);

	pos.y += 20;
	ShowPinNameButt = new wxCheckBox(m_PanelBasic,-1, _("Show Pin Name"), pos);
	if ( CurrentLibEntry )
		{
		if( CurrentLibEntry->m_DrawPinName ) ShowPinNameButt->SetValue(TRUE);
		}
	else ShowPinNameButt->SetValue(TRUE);

	pos.y += 20;
	m_PinsNameInsideButt = new wxCheckBox(m_PanelBasic,-1, _("Pin Name Inside"), pos);
	if ( CurrentLibEntry )
	{
		if ( CurrentLibEntry->m_TextInside ) m_PinsNameInsideButt->SetValue(TRUE);
	}
	else m_PinsNameInsideButt->SetValue(TRUE);

	pos.y += 40;
	new wxStaticText(m_PanelBasic,-1,_("Number of Units:"), pos);
	pos.y += 15;
	wxString number;
	if ( CurrentLibEntry ) number.Printf( wxT("%d"), CurrentLibEntry->m_UnitCount);
	else number = wxT("1");
	SelNumberOfUnits = new wxSpinCtrl(m_PanelBasic,-1,number, pos,
				wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP,
				1, 16);

	pos.y -= 15; pos.x += 180;
	new wxStaticText(m_PanelBasic,-1,_("Skew:"), pos);
	pos.y += 15;
	if ( CurrentLibEntry && CurrentLibEntry->m_TextInside)
		number.Printf( wxT("%d"), CurrentLibEntry->m_TextInside);
	else number = wxT("40");
	m_SetSkew = new wxSpinCtrl(m_PanelBasic,-1,number, pos,
				wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP,
				1, 100);

	pos.x = 5; pos.y += 40;
	m_OptionPower = new wxCheckBox(m_PanelBasic,-1, _("Power Symbol"), pos);
	if ( CurrentLibEntry )
	{
		if( CurrentLibEntry->m_Options == ENTRY_POWER )
			m_OptionPower->SetValue(TRUE);
	}
	
	pos.y = m_OptionPower->GetRect().GetBottom() + 10;
	m_OptionPartsLocked = new wxCheckBox(m_PanelBasic,-1, _("Parts are locked"), pos);
	if ( CurrentLibEntry )
	{
		if( CurrentLibEntry->m_UnitSelectionLocked )
			m_OptionPartsLocked->SetValue(TRUE);
	}
	if ( number == wxT("1") ) m_OptionPartsLocked->Enable(FALSE);

}

/****************************************************************/
void WinEDA_PartPropertiesFrame::BuildPanelEditField(int fieldId)
/****************************************************************/
{
wxPoint pos;
int xx, yy;
LibDrawField * Field;
wxString Hjustify_list[] =
		{ _("Left justify"),_("Center"),_("Right justify")};
wxString Vjustify_list[] =
		{ _("Bottom justify"),_("Center"),_("Top justify")};
int ii = fieldId;

	PanelField[ii] = new wxPanel(m_NoteBook, ID_PANEL_REFERENCE + ii);
	PanelField[ii]->SetFont(*g_DialogFont);
	wxLayoutConstraints * c = new wxLayoutConstraints;
	c->left.SameAs(m_NoteBook, wxLeft);
	c->right.SameAs(m_NoteBook, wxRight);
	c->bottom.SameAs(m_NoteBook, wxBottom);
	PanelField[ii]->SetConstraints(c);
	m_NoteBook->AddPage(PanelField[ii], DrawPartStruct::ReturnFieldName(ii), FALSE);

	pos.x = 10; pos.y = 10;
	ShowFieldText[ii] = new wxCheckBox(PanelField[ii],-1,
			_("Show Text"), pos);
	if ( (FieldFlags[ii] & TEXT_NO_VISIBLE ) == 0 )
		ShowFieldText[ii]->SetValue(TRUE);

	ShowFieldText[ii]->GetSize(&xx, &yy);
	pos.y += yy + 5;
	VorientFieldText[ii] = new wxCheckBox(PanelField[ii],-1,
			_("Vertical"), pos);
	if ( FieldOrient[ii] ) VorientFieldText[ii]->SetValue(TRUE);

	pos.x = 150; pos.y = 5;
	FieldHJustify[ii] = new wxRadioBox(PanelField[ii],-1,
			_("Hor Justify"), pos, wxDefaultSize,
			3,Hjustify_list, 1 , wxRA_SPECIFY_COLS);
	FieldHJustify[ii]->GetSize(&xx, &yy);
	FieldHJustify[ii]->SetSelection(1);
	
	pos.y += yy + 5;
	FieldVJustify[ii] = new wxRadioBox(PanelField[ii],-1,
			_("Vert Justify"), pos, wxDefaultSize,
			3,Vjustify_list, 1 , wxRA_SPECIFY_COLS);
	FieldVJustify[ii]->SetSelection(1);

	wxPoint txtpos;
	pos.x = 10; pos.y = 70;
#define POSY_OFFSET 70
	switch ( ii )
	{
		case REFERENCE:
			if ( CurrentLibEntry )
			{
				Field = &CurrentLibEntry->m_Prefix;
				txtpos = Field->m_Pos;
				if ( Field->m_HJustify == GR_TEXT_HJUSTIFY_LEFT)
					FieldHJustify[ii]->SetSelection(0);
				else if ( Field->m_HJustify == GR_TEXT_HJUSTIFY_RIGHT)
					FieldHJustify[ii]->SetSelection(2);
				if ( Field->m_VJustify == GR_TEXT_VJUSTIFY_BOTTOM)
					FieldVJustify[ii]->SetSelection(0);
				else if ( Field->m_VJustify == GR_TEXT_VJUSTIFY_TOP)
					FieldVJustify[ii]->SetSelection(2);
			}
			else txtpos = wxPoint(0,0);	  					  
			FieldPosition[ii] = new WinEDA_PositionCtrl( PanelField[ii],
						_("Pos"), txtpos,
						g_UnitMetric , pos );
			FieldTextCtrl[ii] = new WinEDA_GraphicTextCtrl( PanelField[ii],
						DrawPartStruct::ReturnFieldName(ii),
						CurrentLibEntry ?
						CurrentLibEntry->m_Prefix.m_Text.GetData() : wxT("U"),
						CurrentLibEntry ? CurrentLibEntry->m_Prefix.m_Size.x : DEFAULT_TEXT_SIZE,
						g_UnitMetric ,
						wxPoint(pos.x, pos.y +FieldPosition[ii]->GetDimension().y + POSY_OFFSET),
						200, TRUE);
			break;

		case VALUE:
			if ( CurrentLibEntry )
			{
				Field = &CurrentLibEntry->m_Name;
				txtpos = Field->m_Pos;
				if ( Field->m_HJustify == GR_TEXT_HJUSTIFY_LEFT)
					FieldHJustify[ii]->SetSelection(0);
				else if ( Field->m_HJustify == GR_TEXT_HJUSTIFY_RIGHT)
					FieldHJustify[ii]->SetSelection(2);
				if ( Field->m_VJustify == GR_TEXT_VJUSTIFY_BOTTOM)
					FieldVJustify[ii]->SetSelection(0);
				else if ( Field->m_VJustify == GR_TEXT_VJUSTIFY_TOP)
					FieldVJustify[ii]->SetSelection(2);
			}
			else txtpos = wxPoint(0,0);	  					  
			FieldPosition[ii] = new WinEDA_PositionCtrl( PanelField[ii],
						_("Pos"), txtpos,
						g_UnitMetric , pos);
			FieldTextCtrl[ii] = new WinEDA_GraphicTextCtrl( PanelField[ii],
						DrawPartStruct::ReturnFieldName(ii),
						CurrentLibEntry ? CurrentLibEntry->m_Name.m_Text.GetData() : NULL,
						CurrentLibEntry ? CurrentLibEntry->m_Name.m_Size.x : DEFAULT_TEXT_SIZE,
						g_UnitMetric ,
						wxPoint(pos.x, pos.y +FieldPosition[ii]->GetDimension().y + POSY_OFFSET),
						200, TRUE);
			break;
			
		default:
			int fsize;
			wxString ftext;
			wxPoint fpos;
			fsize = DEFAULT_TEXT_SIZE;;
			Field = NULL;
			fpos = wxPoint(0,0);
			//recherche du Field de FieldId correspondant, s'il existe
			if ( CurrentLibEntry )
				{
				Field = CurrentLibEntry->Fields;
				while ( Field )
					{
					if( Field->m_FieldId == ii )
						{
						fsize = Field->m_Size.x;
						ftext = Field->m_Text;
						fpos = Field->m_Pos;
						if ( Field->m_HJustify == GR_TEXT_HJUSTIFY_LEFT)
							FieldHJustify[ii]->SetSelection(0);
						else if ( Field->m_HJustify == GR_TEXT_HJUSTIFY_RIGHT)
							FieldHJustify[ii]->SetSelection(2);
						if ( Field->m_VJustify == GR_TEXT_VJUSTIFY_BOTTOM)
							FieldVJustify[ii]->SetSelection(0);
						else if ( Field->m_VJustify == GR_TEXT_VJUSTIFY_TOP)
							FieldVJustify[ii]->SetSelection(2);
						break;
						}
					Field = (LibDrawField*)Field->Pnext;
					}
				}

			FieldPosition[ii] = new WinEDA_PositionCtrl( PanelField[ii],
						_("Pos"), fpos,
						g_UnitMetric , pos);
			FieldTextCtrl[ii] = new WinEDA_GraphicTextCtrl( PanelField[ii],
						DrawPartStruct::ReturnFieldName(ii),
						ftext, fsize,
						g_UnitMetric ,
						wxPoint(pos.x, pos.y +FieldPosition[ii]->GetDimension().y + POSY_OFFSET),
						200, TRUE);
			break;
	}
}


/************************************************************************/
void  WinEDA_PartPropertiesFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
/************************************************************************/
{
    // true is to force the frame to close
    Close(true);
}

/**************************************************************************/
void WinEDA_PartPropertiesFrame::PartPropertiesAccept(wxCommandEvent& event)
/**************************************************************************/
/* Met a jour les differents parametres pour le composant en cours d'�dition
*/
{
int ii, jj;
int hjustify[3] = {	GR_TEXT_HJUSTIFY_LEFT , GR_TEXT_HJUSTIFY_CENTER,
	GR_TEXT_HJUSTIFY_RIGHT };
int vjustify[3] = {	GR_TEXT_VJUSTIFY_BOTTOM , GR_TEXT_VJUSTIFY_CENTER,
	GR_TEXT_VJUSTIFY_TOP };
	
	if( CurrentLibEntry == NULL )
	{
		Close(); return;
	}

	m_Parent->m_CurrentScreen->SetModify();
	m_Parent->m_CurrentScreen->SetRefreshReq();
	m_Parent->SaveCopyInUndoList();

	/* A new name could be entered in VALUE field.
		Must not be an existing alias name in alias list box */ 
	jj = m_PartAliasList->GetCount();
	wxString newvalue = FieldTextCtrl[VALUE]->GetText();
	for ( ii = 0; ii < jj; ii++ )
	{
		if( newvalue.CmpNoCase(m_PartAliasList->GetString(ii).GetData()) == 0 )
		{
		wxString msg;
		msg.Printf( wxT("Alias %s exists!"), newvalue.GetData());
		DisplayError(this, msg);
		return;
		}
	}

	/* Update the doc, keyword and doc filename strings */
	if ( m_AliasLocation < 0 )
	{
		CurrentLibEntry->m_Doc = m_Doc->GetValue();
		CurrentLibEntry->m_KeyWord = m_Keywords->GetValue();
		CurrentLibEntry->m_DocFile = m_Docfile->GetValue();
	}

	else
	{
		CurrentLibEntry->m_AliasList[m_AliasLocation+ALIAS_DOC] = m_Doc->GetValue();
		CurrentLibEntry->m_AliasList[m_AliasLocation+ALIAS_KEYWORD] = m_Keywords->GetValue();
		CurrentLibEntry->m_AliasList[m_AliasLocation+ALIAS_DOC_FILENAME] = m_Docfile->GetValue();
	}
	
	/* Update the alias list */
	/* 1 - Add names: test for a not existing name in old alias list: */
	jj = m_PartAliasList->GetCount();
	for ( ii = 0; ii < jj; ii++ )
	{
		if ( LocateAlias( CurrentLibEntry->m_AliasList, m_PartAliasList->GetString(ii)) < 0 )
		{	// new alias must be created
			CurrentLibEntry->m_AliasList.Add(m_PartAliasList->GetString(ii));
			CurrentLibEntry->m_AliasList.Add(wxEmptyString);	// Add a void doc string 
			CurrentLibEntry->m_AliasList.Add(wxEmptyString);	// Add a void keyword list string 
			CurrentLibEntry->m_AliasList.Add(wxEmptyString);	// Add a void doc filename string 
		}
	}
	
	/* 2 - Remove delete names: test for an non existing name in new alias list: */
	int kk, kkmax = CurrentLibEntry->m_AliasList.GetCount();
	for ( kk = 0; kk < kkmax; )
	{
		jj = m_PartAliasList->GetCount();
		wxString aliasname = CurrentLibEntry->m_AliasList[kk];
		for ( ii = 0; ii < jj; ii++ )
		{
			if( aliasname.CmpNoCase(m_PartAliasList->GetString(ii).GetData()) == 0 )
			{
				kk += ALIAS_NEXT; // Alias exist in new list. keep it and test next old name
				break;
			}
		}
		
		if ( ii == jj ) // Alias not found in new list, remove it (4 strings in kk position)
		{
			for( ii = 0; ii < ALIAS_NEXT; ii++ ) CurrentLibEntry->m_AliasList.RemoveAt(kk);
			kkmax = CurrentLibEntry->m_AliasList.GetCount();
		}
		
	}
	

	if ( ! FieldTextCtrl[REFERENCE]->GetText().IsEmpty() )
	{
		CurrentLibEntry->m_Prefix.m_Text = FieldTextCtrl[REFERENCE]->GetText();
	}

	if ( ! FieldTextCtrl[VALUE]->GetText().IsEmpty() )
	{
		if ( CurrentLibEntry->m_Name.m_Text != FieldTextCtrl[VALUE]->GetText() )
		{
			m_RecreateToolbar = TRUE;
			CurrentLibEntry->m_Name.m_Text = FieldTextCtrl[VALUE]->GetText();
		}
	}

	CurrentLibEntry->m_Prefix.m_Size.x = 
		CurrentLibEntry->m_Prefix.m_Size.y = FieldTextCtrl[REFERENCE]->GetTextSize();
	CurrentLibEntry->m_Name.m_Size.x =
		CurrentLibEntry->m_Name.m_Size.y = FieldTextCtrl[VALUE]->GetTextSize();

	CurrentLibEntry->m_Prefix.m_Pos = FieldPosition[REFERENCE]->GetValue();
	CurrentLibEntry->m_Name.m_Pos = FieldPosition[VALUE]->GetValue();

	CurrentLibEntry->m_Prefix.m_Orient = VorientFieldText[REFERENCE]->GetValue() ? 1 : 0;
	CurrentLibEntry->m_Name.m_Orient = VorientFieldText[VALUE]->GetValue() ? 1 : 0;

	CurrentLibEntry->m_Prefix.m_HJustify =
			hjustify[FieldHJustify[REFERENCE]->GetSelection()];
	CurrentLibEntry->m_Prefix.m_VJustify =
			vjustify[FieldVJustify[REFERENCE]->GetSelection()];
	CurrentLibEntry->m_Name.m_HJustify =
			hjustify[FieldHJustify[VALUE]->GetSelection()];
	CurrentLibEntry->m_Name.m_VJustify =
			vjustify[FieldVJustify[VALUE]->GetSelection()];

	if ( ShowFieldText[REFERENCE]->GetValue() )
		CurrentLibEntry->m_Prefix.m_Attributs &= ~TEXT_NO_VISIBLE;
	else
		CurrentLibEntry->m_Prefix.m_Attributs |= TEXT_NO_VISIBLE;

	if ( ShowFieldText[VALUE]->GetValue() )
		CurrentLibEntry->m_Name.m_Attributs &= ~TEXT_NO_VISIBLE;
	else
		CurrentLibEntry->m_Name.m_Attributs |= TEXT_NO_VISIBLE;

	for ( ii = FOOTPRINT; ii < NUMBER_OF_FIELDS; ii++ )
	{
		LibDrawField * Field = CurrentLibEntry->Fields;
		LibDrawField * NextField, * previousField = NULL;
		while ( Field )
		{
			NextField = (LibDrawField*)Field->Pnext;
			if( Field->m_FieldId == ii )
			{
				Field->m_Text = FieldTextCtrl[ii]->GetText();
				Field->m_Size.x = Field->m_Size.y = FieldTextCtrl[ii]->GetTextSize();
				Field->m_HJustify = FieldHJustify[ii]->GetSelection();
				Field->m_VJustify = FieldVJustify[ii]->GetSelection();
				if ( ShowFieldText[ii]->GetValue() )
					Field->m_Attributs &= ~TEXT_NO_VISIBLE;
				else
					Field->m_Attributs |= TEXT_NO_VISIBLE;
				Field->m_Orient = VorientFieldText[ii]->GetValue() ? 1 : 0;
				Field->m_Pos = FieldPosition[ii]->GetValue();
				if( Field->m_Text.IsEmpty() )	// An old field exists; new is void, delete it
				{
					delete Field;
					if ( previousField ) previousField->Pnext = NextField;
					else CurrentLibEntry->Fields = NextField;
				}
				break;
			}

			previousField = Field;
			Field = NextField;
		}

		if ( (Field == NULL) &&	( ! FieldTextCtrl[ii]->GetText().IsEmpty() ) )
		{	// Do not exists: must be created
			Field = new LibDrawField(ii);
			Field->m_Text = FieldTextCtrl[ii]->GetText();
			Field->m_Size.x = Field->m_Size.y = FieldTextCtrl[ii]->GetTextSize();
			if ( ShowFieldText[Field->m_FieldId]->GetValue() )
				Field->m_Attributs &= ~TEXT_NO_VISIBLE;
			else
				Field->m_Attributs |= TEXT_NO_VISIBLE;
			Field->m_Orient = VorientFieldText[Field->m_FieldId]->GetValue() ?1 : 0;
			Field->m_Pos = FieldPosition[Field->m_FieldId]->GetValue();
			Field->m_HJustify = hjustify[FieldHJustify[Field->m_FieldId]->GetSelection()];
			Field->m_VJustify = vjustify[FieldVJustify[Field->m_FieldId]->GetSelection()];
			Field->Pnext = CurrentLibEntry->Fields;
			CurrentLibEntry->Fields = Field;
		}
	}

	ii = SelNumberOfUnits->GetValue();
	if ( ChangeNbUnitsPerPackage(ii) )
		m_RecreateToolbar = TRUE;

	if ( AsConvertButt->GetValue() )
	{
		if ( ! g_AsDeMorgan )
		{
			g_AsDeMorgan = 1;
			if ( SetUnsetConvert() ) m_RecreateToolbar = TRUE;
		}
	}
	else
	{
		if ( g_AsDeMorgan )
		{
			g_AsDeMorgan = 0;
			if ( SetUnsetConvert() ) m_RecreateToolbar = TRUE;
		}
	}

	CurrentLibEntry->m_DrawPinNum = ShowPinNumButt->GetValue() ? 1 : 0;
	CurrentLibEntry->m_DrawPinName = ShowPinNameButt->GetValue() ? 1 : 0;

	if ( m_PinsNameInsideButt->GetValue() == FALSE)
		CurrentLibEntry->m_TextInside = 0;
	else
		CurrentLibEntry->m_TextInside = m_SetSkew->GetValue();

	if ( m_OptionPower->GetValue() == TRUE)
		CurrentLibEntry->m_Options = ENTRY_POWER;
	else
		CurrentLibEntry->m_Options = ENTRY_NORMAL;

	/* Set the option "Units locked".
	Obviously, cannot be TRUE if there is only one part */
	CurrentLibEntry->m_UnitSelectionLocked = m_OptionPartsLocked->GetValue();
	if ( CurrentLibEntry->m_UnitCount <= 1 )
		CurrentLibEntry->m_UnitSelectionLocked = FALSE;
	
	if ( m_RecreateToolbar ) m_Parent->ReCreateHToolbar();

	m_Parent->DisplayLibInfos();

	Close();
}


/*******************************************************************************/
void WinEDA_PartPropertiesFrame::CopyDocToAlias(wxCommandEvent& WXUNUSED(event))
/******************************************************************************/
{
	if( CurrentLibEntry == NULL ) return;
	if ( CurrentAliasName.IsEmpty() ) return;

	m_Doc->SetValue(CurrentLibEntry->m_Doc);
	m_Docfile->SetValue(CurrentLibEntry->m_DocFile);
	m_Keywords->SetValue(CurrentLibEntry->m_KeyWord);
}

/**********************************************************/
void WinEDA_PartPropertiesFrame::DeleteAllAliasOfPart(
		wxCommandEvent& WXUNUSED(event))
/**********************************************************/
{

	CurrentAliasName.Empty();
	if( CurrentLibEntry )
	{
		if( IsOK(this, _("Ok to Delete Alias LIST") ) )
		{
			m_PartAliasList->Clear();
			m_RecreateToolbar = TRUE;
		}
	}

}

/*******************************************************************************/
void WinEDA_PartPropertiesFrame::AddAliasOfPart( wxCommandEvent& WXUNUSED(event))
/*******************************************************************************/
/* Add a new name to the alias list box
	New name cannot be the root name, and must not exists
*/
{
wxString Line;
wxString aliasname;

	if(CurrentLibEntry == NULL) return;

	if( Get_Message(_("New alias:"),Line, this) != 0 ) return;

	Line.Replace( wxT(" "), wxT("_") );
	aliasname = Line;

	if ( CurrentLibEntry->m_Name.m_Text.CmpNoCase(Line) == 0 )
	{
		DisplayError(this, _("This is the Root Part"), 10); return;
	}
		
	/* test for an existing name: */
	int ii, jj = m_PartAliasList->GetCount();
	for ( ii = 0; ii < jj; ii++ )
	{
		if( aliasname.CmpNoCase(m_PartAliasList->GetString(ii)) == 0 )
		{
			DisplayError(this, _("Already in use"), 10);
			return;
		}
	}

	m_PartAliasList->Append(aliasname);

	m_RecreateToolbar = TRUE;
}

/********************************************************/
void WinEDA_PartPropertiesFrame::DeleteAliasOfPart(
	wxCommandEvent& WXUNUSED(event))
/********************************************************/
{
wxString aliasname = m_PartAliasList->GetStringSelection();

	if ( aliasname.IsEmpty() ) return;
	if ( aliasname == CurrentAliasName )
	{
		wxString msg = CurrentAliasName + _(" is Current Selected Alias!");
		DisplayError(this, msg );
		return;
	}
	
	int ii = m_PartAliasList->GetSelection();
	m_PartAliasList->Delete(ii);

	m_RecreateToolbar = TRUE;
}




/********************************************************************/
bool WinEDA_PartPropertiesFrame::ChangeNbUnitsPerPackage(int MaxUnit)
/********************************************************************/
/* Routine de modification du nombre d'unites par package pour le
	composant courant;
*/
{
int OldNumUnits, ii, FlagDel = -1;
LibEDA_BaseStruct* DrawItem, * NextDrawItem;

	if( CurrentLibEntry == NULL ) return FALSE;

	/* Si pas de changement: termine */
	if ( CurrentLibEntry->m_UnitCount == MaxUnit ) return FALSE;

	OldNumUnits = CurrentLibEntry->m_UnitCount;
		if ( OldNumUnits < 1 ) OldNumUnits = 1;

	CurrentLibEntry->m_UnitCount = MaxUnit;


	/* Traitement des unites enlevees ou rajoutees */
	if(OldNumUnits > CurrentLibEntry->m_UnitCount )
	{
		DrawItem = CurrentLibEntry->m_Drawings;
		for ( ; DrawItem != NULL; DrawItem = NextDrawItem)
		{
			NextDrawItem = DrawItem->Next();
			if( DrawItem->m_Unit > MaxUnit )  /* Item a effacer */
			{
				if( FlagDel < 0 )
				{
					if( IsOK(this, _("Delete units") ) )
					{
						/* Si part selectee n'existe plus: selection 1ere unit */
						if( CurrentUnit > MaxUnit ) CurrentUnit = 1;
						FlagDel = 1;
					}
					else
					{
						FlagDel = 0;
						MaxUnit = OldNumUnits;
						CurrentLibEntry->m_UnitCount = MaxUnit;
						return FALSE;
					}
				}
				DeleteOneLibraryDrawStruct(m_Parent->DrawPanel, NULL, CurrentLibEntry,
								DrawItem, 0);
			}
		}
		return TRUE;
	}

	if(OldNumUnits < CurrentLibEntry->m_UnitCount )
	{
		DrawItem = CurrentLibEntry->m_Drawings;
		for ( ; DrawItem != NULL; DrawItem = DrawItem->Next() )
		{
			/* Duplication des items pour autres elements */
			if( DrawItem->m_Unit == 1 )
			{
				for ( ii = OldNumUnits +1; ii <= MaxUnit; ii ++ )
				{
					NextDrawItem = CopyDrawEntryStruct(this, DrawItem);
					NextDrawItem->Pnext = CurrentLibEntry->m_Drawings;
					CurrentLibEntry->m_Drawings = NextDrawItem;
					NextDrawItem->m_Unit = ii;
				}
			}
		}
	}
	return TRUE;
}


/*****************************************************/
bool WinEDA_PartPropertiesFrame::SetUnsetConvert(void)
/*****************************************************/
/* cr�e ou efface (selon option AsConvert) les �l�ments
	de la repr�sentation convertie d'un composant
*/
{
int FlagDel = 0;
LibEDA_BaseStruct* DrawItem = NULL, * NextDrawItem;

	if( g_AsDeMorgan )	/* Representation convertie a creer */
	{
		/* Traitement des elements a ajouter ( pins seulement ) */
		if( CurrentLibEntry ) DrawItem = CurrentLibEntry->m_Drawings;
		for ( ; DrawItem != NULL; DrawItem = DrawItem->Next() )
		{
			/* Duplication des items pour autres elements */
			if( DrawItem->m_StructType != COMPONENT_PIN_DRAW_TYPE ) continue;
			if( DrawItem->m_Convert == 1 )
			{
				if( FlagDel == 0 )
				{
					if( IsOK(this, _("Create pins for Convert items")) )
						FlagDel = 1;
					else
					{
						if( IsOK(this, _("Part as \"De Morgan\" anymore")) )
							return TRUE;

						g_AsDeMorgan = 0; return FALSE;
					}
				}
				NextDrawItem = CopyDrawEntryStruct(this, DrawItem);
				NextDrawItem->Pnext = CurrentLibEntry->m_Drawings;
				CurrentLibEntry->m_Drawings = NextDrawItem;
				NextDrawItem->m_Convert = 2;
			}
		}
	}

	else			   /* Representation convertie a supprimer */
	{
		/* Traitement des elements � supprimer */
		if( CurrentLibEntry ) DrawItem = CurrentLibEntry->m_Drawings;
		for ( ; DrawItem != NULL; DrawItem = NextDrawItem)
		{
			NextDrawItem = DrawItem->Next();
			if( DrawItem->m_Convert > 1 )  /* Item a effacer */
			{
				if( FlagDel == 0 )
				{
					if( IsOK(this, _("Delete Convert items") ) )
					{
						CurrentConvert = 1;
						FlagDel = 1;
					}
					else
					{
						g_AsDeMorgan = 1;
						return FALSE;
					}
				}
				m_Parent->GetScreen()->SetModify();
				DeleteOneLibraryDrawStruct(m_Parent->DrawPanel, NULL, CurrentLibEntry, DrawItem, 0);
			}
		}
	}
	return TRUE;
}

/****************************************************************************/
void WinEDA_PartPropertiesFrame::BrowseAndSelectDocFile(wxCommandEvent& event)
/****************************************************************************/
{
wxString FullFileName;
wxString docpath(g_RealLibDirBuffer), filename;
	
	docpath += wxT("doc"); 
	docpath += STRING_DIR_SEP; 
	FullFileName = EDA_FileSelector(_("Doc Files"),
					docpath,			/* Chemin par defaut */
					wxEmptyString,					/* nom fichier par defaut */
					wxEmptyString,					/* extension par defaut */
					wxEmptyString,					/* Masque d'affichage */
					this,
					wxFD_OPEN,
					TRUE
					);
	if ( FullFileName.IsEmpty() ) return;

	// Suppression du chemin par defaut pour le fichier de doc:
	filename = MakeReducedFileName(FullFileName,docpath, wxEmptyString);
	m_Docfile->SetValue(filename);
}	
