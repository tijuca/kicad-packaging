/*************************************************************************/
/* listboxes.cpp: class for displaying footprint list and component list */
/*************************************************************************/

#include "fctsys.h"

#include "wxstruct.h"
#include "common.h"
#include "cvpcb.h"

#include "id.h"

/************************************************************************/
/* Class (from wxListView) for displaying component and footprint lists */
/************************************************************************/

ListBoxBase::ListBoxBase(WinEDA_CvpcbFrame * parent,
				wxWindowID id, const wxPoint& loc, const wxSize& size):
			LIST_BOX_TYPE(parent, id, loc, size,
				wxSUNKEN_BORDER | wxLC_NO_HEADER |
				wxLC_SINGLE_SEL|wxLC_REPORT | wxLC_VIRTUAL)
{
	m_Parent = parent;
    InsertColumn(0, wxEmptyString);
	SetColumnWidth(0, wxLIST_AUTOSIZE );
}



ListBoxBase::~ListBoxBase(void)
{
}

/************************************************/
void ListBoxBase::OnSize(wxSizeEvent& event)
/************************************************/
// Ajust the column width to the entire available window width
{
wxSize size = GetClientSize();
int width = 0;
	
//	SetColumnWidth(0, wxLIST_AUTOSIZE );
//	width = GetColumnWidth(0);
	SetColumnWidth(0, MAX(width, size.x) );

	event.Skip();
}

/****************************/
void ListBoxBase::Clear(void)
/****************************/
// Reset ALL datas
{
	m_StringList.Clear();
	SetItemCount(m_StringList.Count() );
}

/******************************/
int ListBoxBase::GetCount(void)
/******************************/
// Return number of items
{
	return m_StringList.Count();
}

/*********************************/
int ListBoxBase::GetSelection(void)
/*********************************/
// Return an index for the selected item
{
	return GetFirstSelected();
}

/********************************************************/
void ListBoxBase::SetSelection(unsigned index, bool State)
/*********************************************************/
// Enable or disable an item
{
	Select(index, State);
	EnsureVisible(index);
}

/********************************************************************/
void ListBoxBase::SetString(unsigned linecount, const wxString & text)
/********************************************************************/
// Change an item text
{
	if (linecount >= m_StringList.Count())
		linecount = m_StringList.Count()-1;
	if ( linecount >= 0 ) m_StringList[linecount] = text;
}

/****************************************************/
void ListBoxBase::AppendLine(const wxString & text)
/****************************************************/
// Add an item at end of list
{
	m_StringList.Add(text);
	SetItemCount(m_StringList.Count() );
}


/****************************************************************/
wxString ListBoxBase::OnGetItemText(long item, long column) const
/****************************************************************/
/* Overlayed function: MUST be provided in wxLC_VIRTUAL mode
	because real datas are not handled by ListBoxBase
*/
{
	return m_StringList.Item(item);
}


/************************************************************/
/* ListBox derivee pour l'affichage de la liste des Modules */
/************************************************************/

ListBoxMod::ListBoxMod(WinEDA_CvpcbFrame * parent,
				wxWindowID id, const wxPoint& loc, const wxSize& size,
				int nbitems, wxString choice[]):
			ListBoxBase(parent, id, loc, size)
{
}



ListBoxMod::~ListBoxMod(void)
{
}

/***************************************************************/
/* ListBox derivee pour l'affichage de la liste des Composants */
/***************************************************************/

ListBoxCmp::ListBoxCmp(WinEDA_CvpcbFrame * parent, wxWindowID id,
				const wxPoint& loc, const wxSize& size,
				int nbitems, wxString choice[]):
			ListBoxBase(parent, id, loc, size)
{
}



ListBoxCmp::~ListBoxCmp(void)
{
}



/***********************************************************************/
/* Construction de la table des evenements pour la fenetre des composants */
/***********************************************************************/

BEGIN_EVENT_TABLE(ListBoxCmp, LIST_BOX_TYPE)
	EVT_SIZE( ListBoxBase::OnSize)

END_EVENT_TABLE()


/********************************************/
void WinEDA_CvpcbFrame::BuildCmpListBox(void)
/********************************************/
/* Construit la fenetre d'affichage de la liste des composant.
	dimx et dimy sont les dimensions de la surface totale d'affichage
	et non les dims de la fenetre des Cmp
	Si la fenetre d'affichage a deja ete creee, il y a seulement
	mise a jour de la liste
*/
{
int ii;
STORECMP * Composant;
wxString msg;
wxSize size(10,10);

	if( m_ListCmp == NULL )
	{
		m_ListCmp = new ListBoxCmp(this, ID_CVPCB_LIST_COMPONANTS,
							wxDefaultPosition, size,
							0, NULL);
		m_ListCmp->SetBackgroundColour(wxColour(225,255,255));
		m_ListCmp->SetForegroundColour(wxColour(0,0,0));
		m_ListCmp->SetFont(*g_FixedFont);
	}
	
	m_ListCmp->m_StringList.Clear();
	Composant = BaseListeCmp;
	for (ii = 1 ; Composant != NULL; Composant = Composant->Pnext, ii++ )
	{
		msg.Printf(CMP_FORMAT ,ii,
			Composant->m_Reference.GetData(), Composant->m_Valeur.GetData(),
			Composant->m_Module.GetData());
		m_ListCmp->m_StringList.Add(msg);
	}
	m_ListCmp->SetItemCount(m_ListCmp->m_StringList.Count() );

	if ( BaseListeCmp ) m_ListCmp->SetSelection(0, TRUE);
}


/*************************************************************/
void WinEDA_CvpcbFrame::BuildModListBox(void)
/*************************************************************/
/* Construit la fenetre d'affichage de la liste des Modules.
	dimx et dimy sont les dimensions de la surface totale d'affichage
	et non les dims de la fenetre des Modules
	Si la fenetre d'affichage a deja ete creee, il y a seulement
	mise a jour de la liste
*/
{
STOREMOD * Module;
int ii;
wxString msg;
wxSize size(10,10);

	if( m_ListMod == NULL )
	{
		m_ListMod = new ListBoxMod(this, ID_CVPCB_LIST_MODULES,
							wxDefaultPosition, size,
							0, NULL);
		m_ListMod->SetBackgroundColour(wxColour(225,255,225));
		m_ListMod->SetForegroundColour(wxColour(0,0,0));
		m_ListMod->SetFont(*g_FixedFont);
	}
	m_ListMod->m_StringList.Clear();
	Module = BaseListePkg;

	for ( ii = 0; Module != NULL; Module = Module->Pnext, ii++ )
	{
		msg.Printf( wxT("%3d %s"), ii+1, Module->m_Module.GetData());
		m_ListMod->m_StringList.Add(msg);
	}
	m_ListMod->SetItemCount(m_ListMod->m_StringList.Count() );

	if ( BaseListePkg ) m_ListMod->SetSelection(0, TRUE);
	msg.Printf(_("Modules: %d"), nblib);
	SetStatusText(msg,2);
}



/***********************************************************************/
/* Construction de la table des evenements pour la fenetre des modules */
/***********************************************************************/

BEGIN_EVENT_TABLE(ListBoxMod, LIST_BOX_TYPE)
	EVT_LIST_ITEM_SELECTED (ID_CVPCB_LIST_MODULES, ListBoxMod::OnLeftClick)
	EVT_LIST_ITEM_ACTIVATED (ID_CVPCB_LIST_MODULES,  ListBoxMod::OnLeftDClick)
	EVT_SIZE( ListBoxBase::OnSize)

END_EVENT_TABLE()


/********************************************************/
void ListBoxMod::OnLeftClick(wxListEvent & event)
/********************************************************/
{
int ii, selection;
STOREMOD * Module;
wxString msg;

	selection = event.GetIndex();
	Module = BaseListePkg;
	for ( ii = selection; Module != NULL; Module = Module->Pnext, ii-- )
	{
		if ( ii > 0 ) continue;
		g_CurrentPkg = Module->m_Module;
		break;
	}

	if( m_Parent->DrawFrame && (selection >= 0) )
	{
		m_Parent->CreateScreenCmp(); /* refresh general */
	}

	if ( Module ) msg = Module->m_Doc;
	m_Parent->SetStatusText(msg,0);

	msg = wxT("KeyW: ");
	if( Module ) msg += Module->m_KeyWord;
	m_Parent->SetStatusText(msg, 1);
}


/****************************************************/
void ListBoxMod::OnLeftDClick(wxListEvent & event)
/****************************************************/
{
int selection;
STOREMOD * Module;

	selection = event.GetIndex();
	if (selection <0) return;

	Module = BaseListePkg;
	for ( ; Module != NULL; Module = Module->Pnext, selection-- )
	{
		if ( selection > 0 ) continue;
		g_CurrentPkg = Module->m_Module;
		break;
	}

	if( Module ) m_Parent->SetNewPkg();
}

