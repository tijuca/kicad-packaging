/************************************************************************/
/* hierarch.cpp: Gestion  de la hierarchie: navigation dans les feuilles */
/************************************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"

#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"


#include "../bitmaps/treesel.xpm"
#include "../bitmaps/treensel.xpm"


static void InstallScreenFromSheet(WinEDA_SchematicFrame * frame,
	DrawSheetStruct * Sheet );

enum {
	ID_TREECTRL_HIERARCHY = 1600
};


/* nombre maxi de menus (donc de sheets directement accessibles) */
#define NB_MAX_SHEET 200

class WinEDA_HierFrame;

/* Cette classe permet de memoriser la feuille (sheet) associée a l'item
 pour l'arbre de hierarchie */
class TreeItemData: public wxTreeItemData
{
public:
	DrawSheetStruct * Sheet;
	TreeItemData(DrawSheetStruct * sheet) :wxTreeItemData()
		{
		Sheet = sheet;
		}
};

/* Classe de l'arbre de hierarchie */
class WinEDA_Tree : public wxTreeCtrl
{
private:
	WinEDA_HierFrame * m_Parent;
	wxImageList *imageList;

public:
	WinEDA_Tree(void) { }
	WinEDA_Tree(WinEDA_HierFrame *parent);

	DECLARE_DYNAMIC_CLASS(WinEDA_Tree)
};
IMPLEMENT_DYNAMIC_CLASS(WinEDA_Tree, wxTreeCtrl)



WinEDA_Tree::WinEDA_Tree(WinEDA_HierFrame *parent) :
	wxTreeCtrl( (wxWindow*)parent, ID_TREECTRL_HIERARCHY,
				wxDefaultPosition, wxDefaultSize,
                wxTR_HAS_BUTTONS, wxDefaultValidator, wxT("HierachyTreeCtrl"))
{
	m_Parent = parent;
	// Make an image list containing small icons
	imageList = new wxImageList(16, 15, TRUE, 2);

	imageList->Add(wxBitmap(tree_nosel_xpm));
	imageList->Add(wxBitmap(tree_sel_xpm));

	AssignImageList(imageList);

}

/* Classe definissant la fenetre d'affichage de la hierarchie */
class WinEDA_HierFrame : public wxDialog
{
public:
	WinEDA_SchematicFrame * m_Parent;
	WinEDA_Tree * m_Tree;
	int nbsheets;
	wxDC * m_DC;

private:
	wxSize m_TreeSize;	// Taille de l'arbre de hierarchie
	int maxposx;

public:
	WinEDA_HierFrame(WinEDA_SchematicFrame *parent, wxDC * DC, const wxPoint& pos);
	void BuildListeSheets(EDA_BaseStruct * DrawStruct,
						wxTreeItemId * previousmenu);
	~WinEDA_HierFrame(void);

	void OnSelect(wxTreeEvent& event);

private:
	void OnQuit(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(WinEDA_HierFrame, wxDialog)
	EVT_TREE_ITEM_ACTIVATED(ID_TREECTRL_HIERARCHY,
			WinEDA_HierFrame::OnSelect)
END_EVENT_TABLE()


void WinEDA_SchematicFrame::InstallHierarchyFrame(wxDC * DC, wxPoint &pos)
{
	WinEDA_HierFrame * treeframe = new WinEDA_HierFrame(this, DC, pos);
	treeframe->ShowModal(); treeframe->Destroy();
}


WinEDA_HierFrame::WinEDA_HierFrame(WinEDA_SchematicFrame *parent, wxDC * DC,
					const wxPoint& pos):
			wxDialog(parent, -1, _("Navigator"), pos, wxSize(110,50),
				 DIALOG_STYLE)
{
wxTreeItemId cellule;

	m_Parent = parent;
	m_DC = DC;
	m_Tree = new WinEDA_Tree(this);

	nbsheets = 1;

	cellule = m_Tree->AddRoot(_("Root"), 0, 1);
	m_Tree->SetItemBold(cellule, TRUE);
	m_Tree->SetItemData( cellule, new TreeItemData(NULL) );

wxRect itemrect;
#ifdef __UNIX__
  itemrect.SetWidth(100);
  itemrect.SetHeight(20);
#else
	m_Tree->GetBoundingRect(cellule,itemrect);
#endif
	m_TreeSize.x = itemrect.GetWidth() + 10;
	m_TreeSize.y = 20;

	if (ScreenSch == m_Parent->m_CurrentScreen )
	{
		m_Tree->SelectItem(cellule);
	}

	maxposx = 15;
	BuildListeSheets(ScreenSch->EEDrawList, &cellule);

	if ( nbsheets > 1)
	{
		m_Tree->Expand(cellule);

		// Reajustage de la taille de la frame a une valeur optimale
		m_TreeSize.y += nbsheets * itemrect.GetHeight();
		m_TreeSize.x = MIN(m_TreeSize.x, 250);
		m_TreeSize.y = MIN( m_TreeSize.y, 350);
		SetClientSize(m_TreeSize);
	}
}

WinEDA_HierFrame::~WinEDA_HierFrame(void)
{
}


/************************************************************************/
void  WinEDA_HierFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
/************************************************************************/
{
    // true is to force the frame to close
    Close(true);
}

/********************************************************************/
void WinEDA_HierFrame::BuildListeSheets(EDA_BaseStruct * DrawStruct,
						wxTreeItemId * previousmenu)
/********************************************************************/
/* Routine de creation de l'aebre de navigation dans la hierarchy
schematique
	Cette routine est Reentrante !
*/
{
wxTreeItemId menu;
SCH_SCREEN * Screen;

	if( nbsheets > NB_MAX_SHEET )
	{
		if( nbsheets == (NB_MAX_SHEET + 1) )
		{
			DisplayError(this, wxT("BuildListeSheets: Error: nbsheets > 200"));
			nbsheets++;
		}
		return;
	}

	maxposx += m_Tree->GetIndent();

	while( DrawStruct )
	{
		if(DrawStruct->m_StructType == DRAW_SHEET_STRUCT_TYPE )
		{
			#define STRUCT ((DrawSheetStruct*)DrawStruct)
			nbsheets++;
			menu = m_Tree->AppendItem(*previousmenu,
					STRUCT->m_Field[VALUE].m_Text, 0 , 1 );
			m_Tree->SetItemData( menu, new TreeItemData(STRUCT) );
//			m_Tree->SetItemFont( menu, *StdFont);

			int ll = m_Tree->GetItemText(menu).Len();
#ifdef __WINDOWS__
			ll *= 9;	//  * char width
#else
			ll *= 12;	//  * char width
#endif
			ll += maxposx + 20;
			m_TreeSize.x = MAX(m_TreeSize.x, ll);
			m_TreeSize.y += 1;

			Screen = (SCH_SCREEN*) DrawStruct->m_Son;
			if (Screen == m_Parent->m_CurrentScreen )
			{
				m_Tree->EnsureVisible(menu);
				m_Tree->SelectItem(menu);
			}

			/* Examen des sous - hierarchies */
			if(Screen)
			{
				int oldnbsheets = nbsheets;
				BuildListeSheets(Screen->EEDrawList,&menu);
				if( oldnbsheets != nbsheets )
				{
					m_Tree->SetItemBold(menu, TRUE);
					m_Tree->Expand(menu);
				}
			}
		}
		DrawStruct = DrawStruct->Pnext;
	}
	maxposx -= m_Tree->GetIndent();
}


/***************************************************/
void WinEDA_HierFrame::OnSelect(wxTreeEvent& event)
/***************************************************/
/* appelee sur un double-click de la souris pour la selection d'un item:
	Selectionne et affiche l'ecran demandé
*/
{
DrawSheetStruct * Sheet;

	wxTreeItemId ItemSel = m_Tree->GetSelection();
	Sheet = ((TreeItemData*)(m_Tree->GetItemData(ItemSel)))->Sheet;
	InstallScreenFromSheet(m_Parent, Sheet);
	Close(TRUE);
}


/******************************************************/
void WinEDA_SchematicFrame::InstallPreviousScreen(void)
/******************************************************/
/* Routine d'installation de l'ecran Parent de l'ecran courant
*/
{
SCH_SCREEN * Window;

	if( m_CurrentScreen == ScreenSch ) return;

	g_ItemToRepeat = NULL;
	MsgPanel->EraseMsgBox();

	Window = ScreenSch;
	if( ((SCH_SCREEN*)m_CurrentScreen)->m_RootSheet )
		if( ((SCH_SCREEN*)m_CurrentScreen)->m_RootSheet->m_Parent )
			Window = (SCH_SCREEN*) ((SCH_SCREEN*)m_CurrentScreen)->m_RootSheet->m_Parent;

	m_CurrentScreen = ActiveScreen = Window;
	DrawPanel->m_CanStartBlock = -1;
	DrawPanel->SetScrollbars( DrawPanel->m_Scroll_unit,
							DrawPanel->m_Scroll_unit,
							m_CurrentScreen->m_ScrollbarNumber.x,
							m_CurrentScreen->m_ScrollbarNumber.y,
							m_CurrentScreen->m_ScrollbarPos.x,
							m_CurrentScreen->m_ScrollbarPos.y,TRUE);

	ReDrawPanel();
	DrawPanel->MouseToCursorSchema();
}

/*********************************************************************/
void WinEDA_SchematicFrame::InstallNextScreen(DrawSheetStruct * Sheet)
/*********************************************************************/
/* Routine d'installation de l'ecran correspondant au symbole Sheet pointe
	par la souris
*/
{
	if( Sheet == NULL)
		{
		wxBell(); return;
		}
	g_ItemToRepeat = NULL;
	MsgPanel->EraseMsgBox();
	InstallScreenFromSheet(this, Sheet );
}



/**************************************************************/
static void InstallScreenFromSheet(WinEDA_SchematicFrame * frame,
	DrawSheetStruct * Sheet )
/**************************************************************/

/* Recherche et installe de l'ecran relatif au sheet symbole Sheet.
	Si Sheet == NULL installation de l'ecran de base ( Root ).
	Retourne un pointeur sur l' ecran installe.
	Si l'ecran n'existe pas il sera cree ( cas d'un Sheet Symbol
	venant d'etre cree ).
*/

{
SCH_SCREEN * NewScreen = NULL;
SCH_SCREEN * oldscreen = (SCH_SCREEN*) frame->m_CurrentScreen;

	if( Sheet == NULL )
		{
		NewScreen = ScreenSch;
		}

	else
		{
		NewScreen = (SCH_SCREEN*) Sheet->m_Son;
		if(NewScreen)
			{
			if( NewScreen->m_Type != SCHEMATIC_FRAME)
				{
				DisplayError(frame, wxT("Bad new screen type"));
				return;
				}
			NewScreen->m_RootSheet = Sheet;
			}
		}

	if ( NewScreen )
		{
        frame->m_CurrentScreen = ActiveScreen = NewScreen;
		if ( oldscreen != frame->m_CurrentScreen )
        	{  // Reinit des parametres d'affichage du nouvel ecran
			frame->MsgPanel->EraseMsgBox();
			frame->DrawPanel->SetScrollbars( frame->DrawPanel->m_Scroll_unit,
							frame->DrawPanel->m_Scroll_unit,
							frame->m_CurrentScreen->m_ScrollbarNumber.x,
							frame->m_CurrentScreen->m_ScrollbarNumber.y,
							frame->m_CurrentScreen->m_ScrollbarPos.x,
							frame->m_CurrentScreen->m_ScrollbarPos.y,TRUE);

			frame->DrawPanel->m_CanStartBlock = -1;
			frame->ReDrawPanel();
			frame->DrawPanel->MouseToCursorSchema();
			}
		return;
		}

	/* Ici l'ecran n'a pas ete trouve: il faudra le creer */
	NewScreen = ScreenSch ;
	while(NewScreen->Pnext) NewScreen = (SCH_SCREEN*) NewScreen->Pnext;
	NewScreen->Pnext = CreateNewScreen(frame, NewScreen, Sheet->m_TimeStamp);

	NewScreen = (SCH_SCREEN*)NewScreen->Pnext;
	Sheet->m_Son = NewScreen;
	NewScreen->m_Parent = Sheet;
	NewScreen->m_FileName = Sheet->m_Field[SHEET_FILENAME].m_Text;
	NewScreen->m_RootSheet = Sheet;
	frame->m_CurrentScreen = ActiveScreen = NewScreen;
	frame->Zoom_Automatique(FALSE);
	return;
}

