	/***************************************/
	/* PCBNEW: Autorouting command control */
	/***************************************/

#include "fctsys.h"
#include "gr_basic.h"
#include "common.h"
#include "pcbnew.h"
#include "autorout.h"
#include "cell.h"

#include "protos.h"

/* routines internes */

/* Variables locales */

/********************************************************/
void WinEDA_PcbFrame::Autoroute(wxDC * DC, int mode)
/********************************************************/
/* init board, route traces*/
{
int ii, start, stop;
CHEVELU* ptmp;
MODULE * Module = NULL;
D_PAD * Pad = NULL;
int autoroute_net_code = -1;
wxString msg;


	Route_Layer_TOP = GetScreen()->m_Route_Layer_TOP;
	Route_Layer_BOTTOM = GetScreen()->m_Route_Layer_BOTTOM;

	switch ( mode )
		{
		case ROUTE_NET:
			if ( GetScreen()->m_CurrentItem )
				{
				switch ( GetScreen()->m_CurrentItem->m_StructType )
					{
					case TYPEPAD:
						Pad = (D_PAD*) GetScreen()->m_CurrentItem;
						autoroute_net_code = Pad->m_NetCode;
						break;

					default:
						break;
					}
				}
			if( autoroute_net_code <= 0)
				{
				DisplayError(this, _("Net not selected")); return;
				}
			break; 

		case ROUTE_MODULE:
			Module = (MODULE*) GetScreen()->m_CurrentItem;
			if( (Module == NULL) || (Module->m_StructType != TYPEMODULE) )
			{
				DisplayError(this, _("Module not selected") ); return;
			}
			break;
 
		case ROUTE_PAD:
			Pad = (D_PAD*) GetScreen()->m_CurrentItem;
			if( (Pad == NULL)  || (Pad->m_StructType != TYPEPAD) )
			{
				DisplayError(this, _("Pad not selected") ); return;
			}
			break;
 
		}
	if( (m_Pcb->m_Status_Pcb & LISTE_CHEVELU_OK ) == 0 )
		Compile_Ratsnest(DC, TRUE);

	/* Placement du flag CH_ROUTE_REQ sur les chevelus demandes */
	ptmp = (CHEVELU*) m_Pcb->m_Ratsnest;
	for ( ii = m_Pcb->GetNumRatsnests(); ii > 0; ii--, ptmp ++ )
	{
		ptmp->status &= ~CH_ROUTE_REQ;
		switch ( mode )
		{
			case ROUTE_ALL:
				ptmp->status |= CH_ROUTE_REQ; break;

			case ROUTE_NET:
				if( autoroute_net_code == ptmp->m_NetCode)
					ptmp->status |= CH_ROUTE_REQ;
				break;
 
			case ROUTE_MODULE:
			{
				D_PAD * pt_pad = (D_PAD *) Module->m_Pads;
				for( ; pt_pad != NULL; pt_pad = (D_PAD *)pt_pad->Pnext)
				{
					if( ptmp->pad_start == pt_pad )
						ptmp->status |= CH_ROUTE_REQ;
					if( ptmp->pad_end == pt_pad )
						ptmp->status |= CH_ROUTE_REQ;
				}
				break;
			}
			case ROUTE_PAD:
				if( (ptmp->pad_start == Pad) || (ptmp->pad_end == Pad))
					ptmp->status |= CH_ROUTE_REQ;
				break;
	}
	}
	ptmp = (CHEVELU*)m_Pcb->m_Ratsnest;

	start = time( NULL );

	/* Calcul du pas de routage fixe a 5 mils et plus */
	g_GridRoutingSize = GetScreen()->GetGrid().x;
	if (g_GridRoutingSize < 50 ) g_GridRoutingSize = 50 ;
	E_scale = g_GridRoutingSize / 50 ; if (E_scale < 1 ) E_scale = 1 ;

	/* calcule de Ncols et Nrow, taille de la matrice de routage */
	ComputeMatriceSize(this, g_GridRoutingSize);

	MsgPanel->EraseMsgBox();

	/* Creation du mapping du board */
	Nb_Sides = ONE_SIDE;
	if(Route_Layer_TOP != Route_Layer_BOTTOM ) Nb_Sides = TWO_SIDES;

	if ( Board.InitBoard() < 0 )
	{
		DisplayError(this, _("No memory for autorouting") );
		Board.UnInitBoard();  /* Libere la memoire BitMap */
		return;
	}

	Affiche_Message( _("Place Cells") );
	PlaceCells(m_Pcb, -1, FORCE_PADS);

	/* Construction de la liste des pistes a router */
	Build_Work(m_Pcb, ptmp);

	// DisplayBoard(DrawPanel, DC);

	if( Nb_Sides == TWO_SIDES )
		Solve(DC, TWO_SIDES);	/* double face */
	else Solve(DC, ONE_SIDE);	/* simple face */
	
	/* Liberation de la memoire */
	FreeQueue();	/* Libere la memoire de routage */
	InitWork();	/* Libere la memoire de la liste des connexions a router */
	Board.UnInitBoard();  /* Libere la memoire BitMap */
	stop = time( NULL ) - start;
	msg.Printf( wxT("time = %d second%s"), stop, (stop == 1) ? wxT("") : wxT("s") );
	Affiche_Message(msg);
}

/************************************************/
void WinEDA_PcbFrame::Reset_Noroutable(wxDC * DC)
/*************************************************/
 /* Remet a 0 le flag CH_NOROUTABLE qui est positionne a 1 par Solve()
	lorsque un chevelu n'a pas ete route.
	Si ce flag est a 1 il n'est pas reroute
*/
{
int ii;
CHEVELU* pt_rats;

	if( (m_Pcb->m_Status_Pcb & LISTE_CHEVELU_OK )== 0 )
		Compile_Ratsnest(DC, TRUE);

	pt_rats = (CHEVELU*)m_Pcb->m_Ratsnest;
	if( pt_rats == NULL ) return;

	for ( ii = m_Pcb->GetNumRatsnests(); ii > 0 ; ii-- , pt_rats++)
	{
		pt_rats->status &= ~CH_UNROUTABLE ;
	}
}

/*****************************************************/
void DisplayBoard(WinEDA_DrawPanel * panel, wxDC * DC)
/****************************************************/
/* Fonction de DEBUG : affiche le remplissage des cellules TOP et BOTTOM */
{
int row, col, i , j;
int dcell0, dcell1 = 0, color;
int maxi;

	maxi = (/*ActiveScreen->Lim_XD - 20*/ 500) / Ncols;
	maxi = (maxi * 3 ) / 4;
	if (!maxi) maxi = 1;

	for ( col = 0; col < Ncols; col++)
		{
		for(row = 0; row < Nrows; row++)
			{
			color = 0;
			dcell0 = GetCell(row,col,BOTTOM); if (dcell0 & HOLE) color = GREEN;
			if(Nb_Sides) dcell1 = GetCell(row,col,TOP);
			if ( dcell1 & HOLE ) color |= RED;
			dcell0 |= dcell1;			
			if( !color && (dcell0 & VIA_IMPOSSIBLE)) color = BLUE ;
			if(color)
				{
				for (i = 0; i < maxi; i++)
					for (j = 0; j < maxi; j++)
						GRSPutPixel(&panel->m_ClipBox, DC,
							(col*maxi)+i + 10,(row*maxi) + 60 + j, color);
				}
	  		}
  		}
}


