			/******************************************/
			/* Track editing: attribute flags edition */
			/******************************************/

#include "fctsys.h"

#include "common.h"
#include "pcbnew.h"
#include "autorout.h"

#include "protos.h"


/*****************************************************************************/
void WinEDA_PcbFrame::Attribut_Segment(TRACK * track, wxDC * DC, bool Flag_On)
/*****************************************************************************/
/* Attribute change for 1 track segment.
Attributes are
SEGM_FIXE		protection against global delete
SEGM_AR			AutoRouted segment
*/
{
	if ( track == NULL ) return;
 
	GetScreen()->SetModify();
	GetScreen()->CursorOff(DrawPanel, DC);	// Erase cursor shape
	track->SetState(SEGM_FIXE, Flag_On);
	track->Draw(DrawPanel, DC, GR_OR | GR_SURBRILL) ;
	GetScreen()->CursorOn(DrawPanel, DC);	// Display cursor shape
	Affiche_Infos_Piste(this, track);
}


/***************************************************************************/
void WinEDA_PcbFrame::Attribut_Track(TRACK * track, wxDC * DC, bool Flag_On)
/***************************************************************************/
/* Attribute change for an entire track */
{
TRACK *Track ;
int nb_segm;

	if( (track == NULL ) || (track->m_StructType == TYPEZONE) ) return;

	GetScreen()->CursorOff(DrawPanel, DC);	// Erase cursor shape
	Track = Marque_Une_Piste(this, DC, track, & nb_segm, GR_OR | GR_SURBRILL) ;

	for( ; (Track != NULL) && (nb_segm > 0) ; nb_segm-- )
	{
		Track->SetState(SEGM_FIXE, Flag_On);
		Track->SetState(BUSY,OFF);
		Track = (TRACK*)Track->Pnext;
	}
	GetScreen()->CursorOn(DrawPanel, DC);	// Display cursor shape

	GetScreen()->SetModify();
}

/***********************************************************************/
void WinEDA_PcbFrame::Attribut_net(wxDC * DC, int net_code, bool Flag_On)
/***********************************************************************/
/* Modify the flag SEGM_FIXE according to Flag_On value,
	for all the segments related to net_code.
	if net_code < 0 all the segments are modified.
*/
{
TRACK *Track = m_Pcb->m_Track;

	/* search the first segment for the selected net_code */
	if ( net_code >= 0 )
	{
		for ( ;Track != NULL; Track = (TRACK*) Track->Pnext )
		{
			if ( net_code == Track->m_NetCode ) break;
		}
	}

	GetScreen()->CursorOff(DrawPanel, DC);	// Erase cursor shape
	while ( Track )	/* Flag change */
	{		
		if ( (net_code >= 0 ) && (net_code != Track->m_NetCode) ) break;
		GetScreen()->SetModify();
		Track->SetState(SEGM_FIXE, Flag_On);
		Track->Draw(DrawPanel, DC, GR_OR | GR_SURBRILL);
		Track = Track->Next();
	}
	GetScreen()->CursorOn(DrawPanel, DC);	// Display cursor shape
	GetScreen()->SetModify();
}


