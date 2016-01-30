	/***********************************************************/
	/*						wxstruct.h:							 */
	/* descriptions des principales classes derivees utilisees */
	/***********************************************************/

#ifndef CVSTRUCT_H
#define CVSTRUCT_H

#ifndef eda_global
#define eda_global extern
#endif


#include "wx/listctrl.h"

/*  Forward declarations of all top-level window classes. */
class ListBoxMod;
class ListBoxCmp;
class WinEDA_DisplayFrame;

#define LIST_BOX_TYPE wxListView

	/******************************************************/
	/* classe derivee pour la Fenetre principale de cvpcb */
	/******************************************************/

class WinEDA_CvpcbFrame: public WinEDA_BasicFrame
{
public:

	ListBoxMod * m_ListMod;
	ListBoxCmp * m_ListCmp;
	WinEDA_DisplayFrame * DrawFrame;
	WinEDA_Toolbar * m_HToolBar;	// Toolbar horizontal haut d'ecran

private:
	wxMenu * m_FilesMenu;

	// Constructor and destructor
public:
	WinEDA_CvpcbFrame(WinEDA_App * parent, const wxString & title);
	~WinEDA_CvpcbFrame(void)
	{
	}

	void Update_Config(wxCommandEvent& event);/* enregistrement de la config */
	void OnQuit(wxCommandEvent& event);
	void OnCloseWindow(wxCloseEvent & Event);
	void OnSize(wxSizeEvent& SizeEvent);
	void OnChar(wxKeyEvent& event);
	void ReCreateHToolbar(void);
	virtual void ReCreateMenuBar(void);
	void SetLanguage(wxCommandEvent& event);
	void AddFontSelectionMenu(wxMenu * main_menu);
	void ProcessFontPreferences(wxCommandEvent& event);

	void ToFirstNA(wxCommandEvent& event);
	void ToPreviousNA(wxCommandEvent& event);
	void DelAssociations(wxCommandEvent& event);
	void SaveQuitCvpcb(wxCommandEvent& event);
	void LoadNetList(wxCommandEvent& event);
	void ConfigCvpcb(wxCommandEvent& event);
	void DisplayModule(wxCommandEvent& event);
	void AssocieModule(wxCommandEvent& event);
	void WriteStuffList(wxCommandEvent & event);
	void DisplayDocFile(wxCommandEvent & event);
	void SetNewPkg(void);
	void BuildCmpListBox(void);
	void BuildModListBox(void);
	void CreateScreenCmp(void);
	void CreateConfigWindow(void);
	int SaveNetList(const wxString & FullFileName);
	int SaveComponentList(const wxString & FullFileName);
	bool ReadInputNetList(const wxString & FullFileName);
	void ReadNetListe(void);
	int rdpcad(void);
	int rdorcad(void);
	int ReadViewlogicWirList(void);
	int ReadViewlogicNetList(void);

	DECLARE_EVENT_TABLE()
};


/***********************************************/
/* ListBox derivee pour l'affichage des listes */
/***********************************************/
class ListBoxBase: public LIST_BOX_TYPE
{
public:
	WinEDA_CvpcbFrame * m_Parent;
	wxArrayString m_StringList;

public:

	ListBoxBase(WinEDA_CvpcbFrame * parent, wxWindowID id,
				const wxPoint& loc, const wxSize& size);

	~ListBoxBase(void);

	wxString OnGetItemText(long item, long column) const;
	void OnSize(wxSizeEvent& event);

	void Clear(void);
	int GetCount(void);
	int GetSelection(void);
	void SetSelection(unsigned index, bool State = TRUE);
	void SetString(unsigned linecount, const wxString & text);
	void AppendLine(const wxString & text);
};

/************************************************************/
/* ListBox derivee pour l'affichage de la liste des Modules */
/************************************************************/

class ListBoxMod: public ListBoxBase
{
public:
	ListBoxMod(WinEDA_CvpcbFrame * parent,
				wxWindowID id, const wxPoint& loc, const wxSize& size,
				int nbitems, wxString choice[]);
	~ListBoxMod(void);

	void OnLeftClick(wxListEvent & event);
	void OnLeftDClick(wxListEvent & event);
	DECLARE_EVENT_TABLE()
};

/***************************************************************/
/* ListBox derivee pour l'affichage de la liste des Composants */
/***************************************************************/

class ListBoxCmp: public ListBoxBase
{
public:
	WinEDA_CvpcbFrame * m_Parent;

public:

	ListBoxCmp(WinEDA_CvpcbFrame * parent, wxWindowID id,
				const wxPoint& loc, const wxSize& size,
				int nbitems, wxString choice[]);

	~ListBoxCmp(void);

	DECLARE_EVENT_TABLE()
};


	/*******************************************************/
	/* class WWinEDA_DisplayFrame: public WinEDA_DrawFrame */
	/*******************************************************/

class WinEDA_DisplayFrame: public WinEDA_BasePcbFrame
{
public:

public:
	WinEDA_DisplayFrame( wxWindow * father, WinEDA_App *parent,
					const wxString & title,
					const wxPoint& pos, const wxSize& size);

	~WinEDA_DisplayFrame(void);

	void OnCloseWindow(wxCloseEvent & Event);
	void Process_Special_Functions(wxCommandEvent& event);
	void RedrawActiveWindow(wxDC * DC, bool EraseBg);
	void ReCreateHToolbar(void);
	void ReCreateVToolbar(void);
	void RecreateMenuBar(void);
	void OnLeftClick(wxDC * DC, const wxPoint& MousePos);
	void OnLeftDClick(wxDC * DC, const wxPoint& MousePos);
	void OnRightClick(const wxPoint& MousePos, wxMenu * PopMenu);
	void SetToolbars(void);
	void InstallOptionsDisplay(wxCommandEvent& event);
	MODULE * Get_Module(const wxString & CmpName);

	void Process_Settings(wxCommandEvent& event);
	void Show3D_Frame(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};


#endif	//#ifndef CVSTRUCT_H

