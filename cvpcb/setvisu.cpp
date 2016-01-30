/*********************************************************************/
/** setvisu.cpp: initialisations de l'ecran d'affichage du composant **/
/*********************************************************************/

#include "fctsys.h"

#include "wxstruct.h"
#include "common.h"
#include "cvpcb.h"
#include "3d_viewer.h"
#include "id.h"
#include "bitmaps.h"

#include "protos.h"

/*******************************************/
void WinEDA_CvpcbFrame::CreateScreenCmp(void)
/*******************************************/
/* Creation de la fenetre d'affichage du composant
*/
{
int ii, ModSel;
wxString msg;
STOREMOD * Module = NULL;
bool IsNew = FALSE;

	ModSel = m_ListMod->GetSelection();

	if( ModSel >= 0 )
	{
		Module = BaseListePkg;
		for( ii = 0; Module != NULL; Module = Module->Pnext, ii++ )
		{
			if (ii == ModSel) break;
		}
	}


	if ( DrawFrame == NULL)
	{
		DrawFrame = new WinEDA_DisplayFrame(this, m_Parent, _("Module"),
						wxPoint(0,0) , wxSize(600,400) );
		IsNew = TRUE;
	}
	else DrawFrame->Maximize(FALSE);

	DrawFrame->SetFocus();	/* Active entree clavier */
	DrawFrame->Show(TRUE);

	if( Module )
	{
		msg = _("Module: ") + Module->m_Module;
		DrawFrame->SetTitle(msg);
		msg = _("Lib: ") + Module->m_LibName;
		DrawFrame->SetStatusText(msg, 0);
		if ( DrawFrame->m_Pcb->m_Modules )
		{
			DeleteStructure( DrawFrame->m_Pcb->m_Modules );
			DrawFrame->m_Pcb->m_Modules = NULL;
		}
		DrawFrame->m_Pcb->m_Modules = DrawFrame->Get_Module(Module->m_Module);
		DrawFrame->Zoom_Automatique(FALSE);
	}

	else if ( !IsNew )
	{
		DrawFrame->ReDrawPanel();
		if ( DrawFrame->m_Draw3DFrame )
			DrawFrame->m_Draw3DFrame->NewDisplay();
	}
// XXX The following line appears to be redundant
	else if ( !IsNew  )DrawFrame->ReDrawPanel();

	if ( !IsNew && DrawFrame->m_Draw3DFrame )
			DrawFrame->m_Draw3DFrame->NewDisplay();
}


