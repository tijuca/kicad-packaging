	/*******************************************************/
	/* PCBNEW - Gestion des Options et Reglages de modeedit*/
	/*******************************************************/

	/*	 Fichier modeditoptions.cpp 	*/

/*
 Affichage et modifications des parametres de travail de Modedit
*/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"
#include "pcbplot.h"
#include "autorout.h"

#include "id.h"

#include "protos.h"

/* Fonctions locales */

/* variables locales */


/***********************************************************************/
void WinEDA_ModuleEditFrame::OnSelectOptionToolbar(wxCommandEvent& event)
/***********************************************************************/
{
int id = event.GetId();
wxClientDC dc(DrawPanel);

	GetScreen()->CursorOff(DrawPanel, &dc);
	DrawPanel->PrepareGraphicContext(&dc);
	switch ( id )
		{
		case ID_TB_OPTIONS_SHOW_GRID:
			m_Draw_Grid = m_OptionsToolBar->GetToolState(id);
			DrawPanel->ReDraw(&dc, TRUE);
			break;

		case ID_TB_OPTIONS_SELECT_UNIT_MM:
			g_UnitMetric = MILLIMETRE;
		case ID_TB_OPTIONS_SELECT_UNIT_INCH:
			if ( id == ID_TB_OPTIONS_SELECT_UNIT_INCH )
				g_UnitMetric = INCHES;
			Affiche_Status_Box();	 /* Reaffichage des coord curseur */
			ReCreateAuxiliaryToolbar();
			break;

		case ID_TB_OPTIONS_SHOW_POLAR_COORD:
			Affiche_Message(wxEmptyString);
			DisplayOpt.DisplayPolarCood = m_OptionsToolBar->GetToolState(id);
			Affiche_Status_Box();	 /* Reaffichage des coord curseur */
			break;

		case ID_TB_OPTIONS_SELECT_CURSOR:
			g_CursorShape = m_OptionsToolBar->GetToolState(id);
			break;

		case ID_TB_OPTIONS_SHOW_PADS_SKETCH:
			m_DisplayPadFill = ! m_OptionsToolBar->GetToolState(id);
			DrawPanel->ReDraw(&dc, TRUE);
			break;

		case ID_TB_OPTIONS_SHOW_MODULE_TEXT_SKETCH:
			m_DisplayModText =
				m_OptionsToolBar->GetToolState(id) ? SKETCH : FILLED;
			DrawPanel->ReDraw(&dc, TRUE);
			break;

		case ID_TB_OPTIONS_SHOW_MODULE_EDGE_SKETCH:
			m_DisplayModEdge =
				m_OptionsToolBar->GetToolState(id) ? SKETCH : FILLED;
			DrawPanel->ReDraw(&dc, TRUE);
			break;

		default:
			DisplayError(this, wxT("WinEDA_ModuleEditFrame::OnSelectOptionToolbar error") );
			break;
		}

	SetToolbars();
	GetScreen()->CursorOn(DrawPanel, &dc);
}

