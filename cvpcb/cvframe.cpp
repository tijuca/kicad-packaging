		/*********************/
		/* File: cvframe.cpp */
		/*********************/
#include "fctsys.h"
#include "common.h"
#include <wx/fontdlg.h>

#include "cvpcb.h"
#include "gr_basic.h"
#include "pcbnew.h"

#include "bitmaps.h"

#include "protos.h"

#include "id.h"

#define FRAME_MIN_SIZE_X 450
#define FRAME_MIN_SIZE_Y 300

	/*******************************************************/
	/* Constructeur de WinEDA_CvpcbFrame: la fenetre generale */
	/*******************************************************/
WinEDA_CvpcbFrame::WinEDA_CvpcbFrame(WinEDA_App *parent, const wxString & title ):
		WinEDA_BasicFrame(NULL, CVPCB_FRAME, parent, title, wxDefaultPosition, wxDefaultSize )
{
	m_FrameName = wxT("CvpcbFrame");
	m_ListCmp = NULL;
	m_ListMod = NULL;
	DrawFrame = NULL;
	m_FilesMenu = NULL;
	m_HToolBar = NULL;

	// Give an icon
	#ifdef __WINDOWS__
	SetIcon( wxICON(a_icon_cvpcb));
	#else
	SetIcon( wxICON(icon_cvpcb));
	#endif

	SetFont(*g_StdFont);

	SetAutoLayout(TRUE);

	GetSettings();
	if ( m_FrameSize.x < FRAME_MIN_SIZE_X ) m_FrameSize.x = FRAME_MIN_SIZE_X;
	if ( m_FrameSize.y < FRAME_MIN_SIZE_Y ) m_FrameSize.y = FRAME_MIN_SIZE_Y;

	// create the status bar
int dims[3] = { -1, -1, 150};
	CreateStatusBar(3);
	SetStatusWidths(3,dims);
	ReCreateMenuBar();
    ReCreateHToolbar();

	// Creation des listes de modules disponibles et des composants du schema
	// Create child subwindows.
	BuildCmpListBox();
	BuildModListBox();

	/* Creation des contraintes de dimension de la fenetre d'affichage des composants
		du schema */
	wxLayoutConstraints * linkpos = new wxLayoutConstraints;
	linkpos->top.SameAs(this,wxTop);
	linkpos->bottom.SameAs(this,wxBottom);
	linkpos->left.SameAs(this,wxLeft);
	linkpos->width.PercentOf(this,wxWidth,66);
	m_ListCmp->SetConstraints(linkpos);

	/* Creation des contraintes de dimension de la fenetre d'affichage des modules
		de la librairie */
	linkpos = new wxLayoutConstraints;
	linkpos->top.SameAs(m_ListCmp,wxTop);
	linkpos->bottom.SameAs(m_ListCmp,wxBottom);
	linkpos->right.SameAs(this,wxRight);
	linkpos->left.SameAs(m_ListCmp,wxRight);
	m_ListMod->SetConstraints(linkpos);

	SetSizeHints(FRAME_MIN_SIZE_X,FRAME_MIN_SIZE_Y, -1,-1, -1,-1);	// Set minimal size to w,h
	SetSize(m_FramePos.x, m_FramePos.y, m_FrameSize.x, m_FrameSize.y);
}


/************************************************/
void WinEDA_CvpcbFrame::OnSize(wxSizeEvent& event)
/************************************************/
{
	event.Skip();
}

/***************************************************************/
/* Construction de la table des evenements pour WinEDA_CvpcbFrame */
/***************************************************************/

BEGIN_EVENT_TABLE(WinEDA_CvpcbFrame, wxFrame)

	EVT_MENU_RANGE(ID_LOAD_PROJECT,ID_LOAD_FILE_10,
		WinEDA_CvpcbFrame::LoadNetList)
	EVT_MENU(ID_SAVE_PROJECT, WinEDA_CvpcbFrame::SaveQuitCvpcb)
	EVT_MENU(ID_CVPCB_QUIT, WinEDA_CvpcbFrame::OnQuit)
	EVT_MENU(ID_CVPCB_DISPLAY_HELP, WinEDA_CvpcbFrame::GetKicadHelp)
	EVT_MENU(ID_CVPCB_DISPLAY_LICENCE, WinEDA_CvpcbFrame::GetKicadAbout)
	EVT_MENU(ID_CONFIG_REQ,WinEDA_CvpcbFrame::ConfigCvpcb)
	EVT_MENU(ID_CONFIG_SAVE,WinEDA_CvpcbFrame::Update_Config)

	EVT_MENU_RANGE(ID_PREFERENCES_FONT_INFOSCREEN, ID_PREFERENCES_FONT_END,
		WinEDA_CvpcbFrame::ProcessFontPreferences)

	EVT_MENU_RANGE(ID_LANGUAGE_CHOICE, ID_LANGUAGE_CHOICE_END,
		WinEDA_CvpcbFrame::SetLanguage)

	EVT_TOOL(ID_CVPCB_QUIT, WinEDA_CvpcbFrame::OnQuit)
	EVT_TOOL(ID_CVPCB_READ_INPUT_NETLIST, WinEDA_CvpcbFrame::LoadNetList)
	EVT_TOOL(ID_CVPCB_SAVEQUITCVPCB, WinEDA_CvpcbFrame::SaveQuitCvpcb)
	EVT_TOOL(ID_CVPCB_CREATE_CONFIGWINDOW,WinEDA_CvpcbFrame::ConfigCvpcb)
	EVT_TOOL(ID_CVPCB_CREATE_SCREENCMP, WinEDA_CvpcbFrame::DisplayModule)
	EVT_TOOL(ID_CVPCB_GOTO_FIRSTNA, WinEDA_CvpcbFrame::ToFirstNA)
	EVT_TOOL(ID_CVPCB_GOTO_PREVIOUSNA, WinEDA_CvpcbFrame::ToPreviousNA)
	EVT_TOOL(ID_CVPCB_DEL_ASSOCIATIONS, WinEDA_CvpcbFrame::DelAssociations)
	EVT_TOOL(ID_CVPCB_AUTO_ASSOCIE, WinEDA_CvpcbFrame::AssocieModule)
	EVT_TOOL(ID_CVPCB_CREATE_STUFF_FILE, WinEDA_CvpcbFrame::WriteStuffList)
	EVT_TOOL(ID_PCB_DISPLAY_FOOTPRINT_DOC, WinEDA_CvpcbFrame::DisplayDocFile)
	EVT_CHAR_HOOK(WinEDA_CvpcbFrame::OnChar)
	EVT_CLOSE(WinEDA_CvpcbFrame::OnCloseWindow)
	EVT_SIZE( WinEDA_CvpcbFrame::OnSize)

END_EVENT_TABLE()

	/************************************************************/
	/* Fonctions de base de WinEDA_CvpcbFrame: la fenetre generale */
	/************************************************************/

/* Sortie de CVPCB */

void WinEDA_CvpcbFrame::OnQuit(wxCommandEvent& event)
{
	Close(TRUE);
}

/**********************************************************/
void WinEDA_CvpcbFrame::OnCloseWindow(wxCloseEvent & Event)
/**********************************************************/
{
int diag;

	if( modified )
	{
	unsigned ii;
		wxMessageDialog dialog(this, _("Netlist and Cmp list modified, Save before exit ?"),
			_("Confirmation"), wxYES_NO | wxCANCEL | wxICON_EXCLAMATION | wxYES_DEFAULT);
		ii = dialog.ShowModal();
		switch ( ii )
		{
			case wxID_CANCEL:
			Event.Veto();
			return;

			case wxID_NO:
				break;

			case wxID_OK:
			case wxID_YES:
				diag = SaveNetList(wxEmptyString);
				if( diag > 0 ) modified = 0;
				else if ( diag == 0 )
				{
					if( ! IsOK(this, _("Problem when saving files, Exit anyway") ) )
					{
						Event.Veto(); return;
					}
				}
				break;
		}
	}

	// Close the help frame
	if ( m_Parent->m_HtmlCtrl )
	{
		if ( m_Parent->m_HtmlCtrl->GetFrame() ) // returns NULL if no help frame active
			m_Parent->m_HtmlCtrl->GetFrame()->Close(TRUE);
	}

	if ( ! NetInNameBuffer.IsEmpty() )
	{
		SetLastProject(NetInNameBuffer);
	}

	FreeMemoryModules();
	FreeMemoryComponants();
	modified = 0;

	SaveSettings();

	Destroy();
	return;
}


/************************************************/
void WinEDA_CvpcbFrame::OnChar(wxKeyEvent& event)
/************************************************/
{
	event.Skip();
}



/*******************************************************/
void WinEDA_CvpcbFrame::ToFirstNA(wxCommandEvent& event)
/*******************************************************/
{
STORECMP * Composant;
int ii, selection;;

	Composant = BaseListeCmp;
	selection= m_ListCmp->GetSelection();
	if(selection < 0) selection = 0;
	for (ii = 0 ; Composant != NULL; Composant = Composant->Pnext)
	{
		if( Composant->m_Module.IsEmpty() && (ii > selection) )
			break;
		ii++;
	}

	if ( Composant == NULL )
	{
		wxBell(); ii = selection;
	}

	if ( BaseListeCmp ) m_ListCmp->SetSelection(ii);
}

/**********************************************************/
void WinEDA_CvpcbFrame::ToPreviousNA(wxCommandEvent& event)
/**********************************************************/
{
STORECMP * Composant;
int ii, selection;

	Composant = BaseListeCmp;
	selection = m_ListCmp->GetSelection();
	if(selection < 0) selection = 0;

	for (ii = 0 ; Composant != NULL; Composant = Composant->Pnext)
		{
		if( ii == selection ) break;
		ii ++;
		}

	for ( ; Composant != NULL ; Composant = Composant->Pback)
	{
		if( Composant->m_Module.IsEmpty() && (ii != selection) )
			break;
		ii--;
	}

	if ( Composant == NULL )
	{
		wxBell(); ii = selection;
	}

	if ( BaseListeCmp ) m_ListCmp->SetSelection(ii);
}


/**********************************************************/
void WinEDA_CvpcbFrame::SaveQuitCvpcb(wxCommandEvent& event)
/**********************************************************/
{
	if( SaveNetList(wxEmptyString)  > 0)
	{
		modified = 0;
		Close(TRUE);
	}
}

/*************************************************************/
void WinEDA_CvpcbFrame::DelAssociations(wxCommandEvent& event)
/*************************************************************/
/* Supprime toutes les associations deja faites
*/
{
int ii;
STORECMP * Composant;
wxString Line;

	if( IsOK(this, _("Delete selections")) )
		{
		Composant = BaseListeCmp;
		g_CurrentPkg.Empty();
		for ( ii = 0; Composant != NULL; Composant = Composant->Pnext, ii++)
			{
			Composant->m_Module.Empty();
			m_ListCmp->SetSelection(ii);
			SetNewPkg();
			}
		m_ListCmp->SetSelection(0);
		composants_non_affectes	= nbcomp;
		}

	Line.Printf( _("Componants: %d (free: %d)"), nbcomp, composants_non_affectes);
	SetStatusText(Line,1);
}


/***********************************************************/
void WinEDA_CvpcbFrame::LoadNetList(wxCommandEvent& event)
/***********************************************************/
/* Fonction liee au boutton "Load"
	Lit la netliste
*/
{
int id = event.GetId();
wxString fullfilename;
wxString oldfilename;
bool newfile;

	if ( ! NetInNameBuffer.IsEmpty() )
		{
		oldfilename = NetInNameBuffer;
		}

	switch ( id )
		{
		case ID_LOAD_FILE_1:
		case ID_LOAD_FILE_2:
		case ID_LOAD_FILE_3:
		case ID_LOAD_FILE_4:
		case ID_LOAD_FILE_5:
		case ID_LOAD_FILE_6:
		case ID_LOAD_FILE_7:
		case ID_LOAD_FILE_8:
		case ID_LOAD_FILE_9:
		case ID_LOAD_FILE_10:
			id -= ID_LOAD_FILE_1;
			fullfilename = GetLastProject(id);
			break;
		}
	newfile = ReadInputNetList(fullfilename);
	if (newfile &&  ! oldfilename.IsEmpty() )
	{
		SetLastProject(NetInNameBuffer);
		ReCreateMenuBar();
	}
}


/***********************************************************/
void WinEDA_CvpcbFrame::ConfigCvpcb(wxCommandEvent& event)
/***********************************************************/
/* Fonction liee au boutton "Config"
	Affiche le panneau de configuration
*/
{
	CreateConfigWindow();
}

/************************************************************/
void WinEDA_CvpcbFrame::DisplayModule(wxCommandEvent& event)
/************************************************************/
/* Fonction liee au boutton "Visu"
	Affiche l'ecran de visualisation des modules
*/
{
	CreateScreenCmp();
	DrawFrame->AdjustScrollBars();
	DrawFrame->Recadre_Trace(FALSE);
}


/****************************************************************/
void WinEDA_CvpcbFrame::AddFontSelectionMenu(wxMenu * main_menu)
/*****************************************************************/
/* create the submenu for fonte selection and setup fonte size
*/
{
wxMenu * fontmenu = new wxMenu();
	ADD_MENUITEM(fontmenu, ID_PREFERENCES_FONT_DIALOG, _("font for dialog boxes"),
		fonts_xpm );
	ADD_MENUITEM(fontmenu, ID_PREFERENCES_FONT_INFOSCREEN, _("font for Lists"),
		fonts_xpm );
	ADD_MENUITEM(fontmenu, ID_PREFERENCES_FONT_STATUS, _("font for Status Line"),
		fonts_xpm );
	ADD_MENUITEM_WITH_HELP_AND_SUBMENU(main_menu, fontmenu,
		ID_PREFERENCES_FONT, _("&Font selection"),
		_("Choose font type and size for dialogs, infos and status box"),
		fonts_xpm);
}


/********************************************************/
void WinEDA_CvpcbFrame::SetLanguage(wxCommandEvent& event)
/********************************************************/
{
int id = event.GetId();

	m_Parent->SetLanguageIdentifier(id );
	m_Parent->SetLanguage();
}

/*************************************************************/
void WinEDA_CvpcbFrame::DisplayDocFile(wxCommandEvent & event)
/*************************************************************/
{
	wxString msg = FindKicadHelpPath();
	msg += wxT("pcbnew/footprints.pdf");
	GetAssociatedDocument(this, wxEmptyString, msg);
}

/********************************************************************/
void WinEDA_CvpcbFrame::ProcessFontPreferences(wxCommandEvent& event)
/********************************************************************/
{
int id = event.GetId();
wxFont font;

	switch (id)
	{

		case ID_PREFERENCES_FONT:
		case ID_PREFERENCES_FONT_DIALOG:
		case ID_PREFERENCES_FONT_STATUS:
			WinEDA_BasicFrame::ProcessFontPreferences(id);
			break;


		case ID_PREFERENCES_FONT_INFOSCREEN:
		{
			font = wxGetFontFromUser(this, *g_FixedFont);
			if ( font.Ok() )
			{
				int pointsize = font.GetPointSize();
				*g_FixedFont = font;
				g_FixedFontPointSize = pointsize;
				m_ListMod->SetFont(*g_FixedFont);
				m_ListCmp->SetFont(*g_FixedFont);
			}
			break;
		}
		default: DisplayError(this, wxT("WinEDA_DrawFrame::ProcessFontPreferences Internal Error") );
			break;
	}
}

