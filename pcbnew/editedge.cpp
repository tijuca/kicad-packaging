		/******************************************************/
		/*	 Edition des contours du pcb: Routines		  	  */
		/* d'effacement et d'edition  de segments et contours */
		/*   du type PCB, draw, edgePCB						  */
		/******************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"

#include "protos.h"

/* Routines Locales */
static void Exit_EditEdge(WinEDA_DrawFrame * frame, wxDC * DC);
static void Montre_Position_NewSegment(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);
static void Move_Segment(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);

/* Variables locales : */
static wxPoint cursor_pos;	// position originelle du curseur souris (fct deplacement)
static wxPoint cursor_pos0;	// position courante du curseur souris

/****************************************************************************/
void WinEDA_PcbFrame::Start_Move_DrawItem(DRAWSEGMENT * drawitem, wxDC * DC)
/****************************************************************************/
 /* Routine de preparation du deplacement d'un element graphique type DRAWSEGMENT
*/
{
	if( drawitem == NULL ) return;
	Trace_DrawSegmentPcb(DrawPanel, DC, drawitem,GR_XOR) ;
	drawitem->m_Flags |= IS_MOVED;
	cursor_pos = cursor_pos0 = GetScreen()->m_Curseur;
	Affiche_Infos_DrawSegment(this, drawitem);
	GetScreen()->ManageCurseur = Move_Segment;
	GetScreen()->ForceCloseManageCurseur = Exit_EditEdge;
	GetScreen()->m_CurrentItem = drawitem;
	GetScreen()->ManageCurseur( DrawPanel, DC, FALSE);
}

/*********************************************************************/
void WinEDA_PcbFrame::Place_DrawItem(DRAWSEGMENT * drawitem, wxDC * DC)
/*********************************************************************/
/*
 Routine de placement de l'element graphique type DRAWSEGMENT en cours de deplacement
*/
{
	if( drawitem == NULL ) return;

	Trace_DrawSegmentPcb(DrawPanel, DC, drawitem,GR_OR) ;
	GetScreen()->ManageCurseur = NULL;
	GetScreen()->ForceCloseManageCurseur = NULL;
	GetScreen()->m_CurrentItem = NULL;
	GetScreen()->SetModify();
	drawitem->m_Flags = 0;
}


/************************************************************************/
static void Move_Segment(WinEDA_DrawPanel * panel, wxDC * DC, bool erase)
/************************************************************************/
/* redessin du contour du Segment Edge lors des deplacements de la souris */
{
DRAWSEGMENT * Segment = ( DRAWSEGMENT * )
	panel->GetScreen()->m_CurrentItem ;
int t_fill = DisplayOpt.DisplayDrawItems;

	if(Segment == NULL) return ;

	DisplayOpt.DisplayDrawItems = SKETCH ;

	/* efface ancienne position */
	if( erase )
		Trace_DrawSegmentPcb(panel, DC, Segment, GR_XOR);

wxPoint delta;
	delta.x = panel->GetScreen()->m_Curseur.x - cursor_pos.x;
	delta.y = panel->GetScreen()->m_Curseur.y - cursor_pos.y;
	Segment->m_Start.x += delta.x;
	Segment->m_Start.y += delta.y;
	Segment->m_End.x += delta.x;
	Segment->m_End.y += delta.y;
	cursor_pos = panel->GetScreen()->m_Curseur;

	Trace_DrawSegmentPcb(panel, DC, Segment,GR_XOR);
	DisplayOpt.DisplayDrawItems = t_fill ;
}



/**************************************************************************/
void WinEDA_PcbFrame::Delete_Segment_Edge( DRAWSEGMENT* Segment,  wxDC * DC)
/**************************************************************************/
{
EDA_BaseStruct * PtStruct;
int track_fill_copy  = DisplayOpt.DisplayDrawItems;

	if (Segment == NULL ) return;

	if(Segment->m_Flags & IS_NEW)  // Trace en cours, on peut effacer le dernier segment
		{
		/* effacement du segment en cours de trace */
		DisplayOpt.DisplayDrawItems = SKETCH ;
		Trace_DrawSegmentPcb(DrawPanel, DC, Segment,GR_XOR) ;
		PtStruct = Segment->Pback;
		DeleteStructure(Segment);
		if( PtStruct && (PtStruct->m_StructType == TYPEDRAWSEGMENT ) )
			Segment = (DRAWSEGMENT*) PtStruct;
		DisplayOpt.DisplayDrawItems = track_fill_copy ;
		GetScreen()->m_CurrentItem = NULL;
		}

	else
		{
		Trace_DrawSegmentPcb(DrawPanel, DC, (DRAWSEGMENT *)Segment,GR_XOR) ;
		Segment->m_Flags = 0;
		DeleteStructure(Segment);
		GetScreen()->m_CurrentItem = NULL;
		GetScreen()->SetModify();
		}
}


/*************************************************************************/
void WinEDA_PcbFrame::Drawing_SetNewWidth(DRAWSEGMENT *DrawSegm, wxDC * DC)
/*************************************************************************/
/* Met a la largeur courante le segment pointe part la souris
*/
{

	if( DrawSegm == NULL ) return;

	Trace_DrawSegmentPcb(DrawPanel, DC, DrawSegm, GR_XOR);

	if(DrawSegm->m_Layer == EDGE_N) DrawSegm->m_Width = g_DesignSettings.m_EdgeSegmentWidth;
	else DrawSegm->m_Width = g_DesignSettings.m_DrawSegmentWidth;

	Trace_DrawSegmentPcb(DrawPanel, DC, DrawSegm, GR_OR);

	Affiche_Infos_DrawSegment(this, DrawSegm);

	GetScreen()->SetModify();
}


/******************************************************************************/
void WinEDA_PcbFrame::Delete_Drawings_All_Layer(DRAWSEGMENT *Segment, wxDC * DC)
/******************************************************************************/
{
DRAWSEGMENT * pt_segm;
TEXTE_PCB* pt_txt;
EDA_BaseStruct * PtStruct, *PtNext;
COTATION * Cotation;
int layer = Segment->m_Layer;

	if(layer <= CMP_N)
		{
		DisplayError(this, _("Copper layer global delete not allowed!"), 20);
		return;
		}

	if ( Segment->m_Flags )
		{
		DisplayError(this, _("Segment is being edited"), 10);
		return;
		}

	wxString msg = _("Delete Layer ") + ReturnPcbLayerName(layer);
	if( !IsOK(this, msg) ) return ;

	PtStruct = m_Pcb->m_Drawings;
	for( ; PtStruct != NULL; PtStruct = PtNext )
		{
		GetScreen()->SetModify();
		PtNext = PtStruct->Pnext;
		switch( PtStruct->m_StructType)
			{
			case TYPEDRAWSEGMENT:
				pt_segm = (DRAWSEGMENT*) PtStruct;
				if(pt_segm->m_Layer == layer)
					{
					Trace_DrawSegmentPcb(DrawPanel, DC, pt_segm,GR_XOR);
					DeleteStructure(PtStruct);
					}
				break;

			case TYPETEXTE:
				pt_txt = (TEXTE_PCB*)PtStruct;
				if(pt_txt->m_Layer == layer)
					{
					pt_txt->Draw(DrawPanel, DC, wxPoint(0, 0), GR_XOR);
					DeleteStructure( PtStruct );
					}
				break;


			case TYPECOTATION:
				Cotation = (COTATION*)PtStruct;
				if(Cotation->m_Layer == layer)
					{
					Cotation->Draw(DrawPanel, DC, wxPoint(0, 0), GR_XOR);
					DeleteStructure(PtStruct);
					}
				break;

			default: DisplayError(this, wxT("Type Drawing Inconnu") );
				break;
			}
		}
}


/*************************************************************/
static void Exit_EditEdge(WinEDA_DrawFrame * frame, wxDC * DC)
/*************************************************************/
{
DRAWSEGMENT *Segment = (DRAWSEGMENT *) frame->GetScreen()->m_CurrentItem ;

	if (Segment == NULL) return;

	if( Segment->m_Flags & IS_NEW )
		{
		frame->GetScreen()->ManageCurseur(frame->DrawPanel, DC, FALSE);
		DeleteStructure(Segment);
		Segment = NULL;
		}

	else
		{
		wxPoint pos = frame->GetScreen()->m_Curseur;
		frame->GetScreen()->m_Curseur = cursor_pos0;
		frame->GetScreen()->ManageCurseur(frame->DrawPanel, DC, TRUE);
		frame->GetScreen()->m_Curseur = pos;
		Segment->m_Flags = 0;
		Trace_DrawSegmentPcb(frame->DrawPanel, DC, Segment,GR_OR) ;
		}
	frame->GetScreen()->ManageCurseur = NULL;
	frame->GetScreen()->ForceCloseManageCurseur = NULL;
	frame->GetScreen()->m_CurrentItem = NULL;
}

/**********************************************************************/
DRAWSEGMENT * WinEDA_PcbFrame::Begin_DrawSegment(DRAWSEGMENT * Segment,
			int shape, wxDC * DC)
/**********************************************************************/
/* Routine d'initialisation du trace d'un segment de type autre que piste
*/
{
int s_large;
int angle = 0;
DRAWSEGMENT * DrawItem;

	s_large = g_DesignSettings.m_DrawSegmentWidth;
	if(GetScreen()->m_Active_Layer == EDGE_N)
		{
		s_large = g_DesignSettings.m_EdgeSegmentWidth;
		}

	if ( shape == S_ARC ) angle = 900;

	if(Segment == NULL )		/* debut reel du trace */
		{
		GetScreen()->m_CurrentItem = Segment = new DRAWSEGMENT( m_Pcb);
		Segment->m_Flags = IS_NEW;
		Segment->m_Layer = GetScreen()->m_Active_Layer;
		Segment->m_Width = s_large;
		Segment->m_Shape = shape;
		Segment->m_Angle = 900;
		Segment->m_Start = Segment->m_End = GetScreen()->m_Curseur ;
		GetScreen()->ManageCurseur = Montre_Position_NewSegment;
		GetScreen()->ForceCloseManageCurseur = Exit_EditEdge;
		}

	else	/* trace en cours : les coord du point d'arrivee ont ete mises
			a jour par la routine Montre_Position_NewSegment*/
		{
		if( (Segment->m_Start.x != Segment->m_End.x )
			|| (Segment->m_Start.y != Segment->m_End.y ) )
			{
			if ( Segment->m_Shape == S_SEGMENT )
				{
				Segment->Pnext = m_Pcb->m_Drawings;
				Segment->Pback = m_Pcb;
				if( m_Pcb->m_Drawings )
				 m_Pcb->m_Drawings->Pback = Segment;
				m_Pcb->m_Drawings = Segment;
				GetScreen()->SetModify();
				Segment->m_Flags = 0;

				Trace_DrawSegmentPcb(DrawPanel, DC, Segment,GR_OR);

				DrawItem = Segment;

				GetScreen()->m_CurrentItem = Segment = new DRAWSEGMENT( m_Pcb );

				Segment->m_Flags = IS_NEW;
				Segment->m_Layer = DrawItem->m_Layer;
				Segment->m_Width = s_large;
				Segment->m_Shape = DrawItem->m_Shape;
				Segment->m_Type = DrawItem->m_Type;
				Segment->m_Angle = DrawItem->m_Angle;
				Segment->m_Start = Segment->m_End = DrawItem->m_End;
				Montre_Position_NewSegment(DrawPanel, DC, FALSE);
				}
			else
				{
				End_Edge(Segment, DC);
				Segment = NULL;
				}
			}
		}
	return Segment;
}


/***************************************************************/
void WinEDA_PcbFrame::End_Edge(DRAWSEGMENT * Segment, wxDC * DC)
/***************************************************************/
{

	if( Segment == NULL ) return ;
	Trace_DrawSegmentPcb(DrawPanel, DC, (DRAWSEGMENT *)Segment,GR_OR);

	/* Effacement si Longueur nulle */
	if( (Segment->m_Start.x == Segment->m_End.x) &&
   		(Segment->m_Start.y == Segment->m_End.y) )
		DeleteStructure(Segment);

	else
		{
		Segment->m_Flags = 0;
		Segment->Pnext = m_Pcb->m_Drawings;
		Segment->Pback = m_Pcb;
		if( m_Pcb->m_Drawings ) m_Pcb->m_Drawings->Pback = Segment;
		m_Pcb->m_Drawings = Segment;
		GetScreen()->SetModify();
		}

	GetScreen()->ManageCurseur = NULL;
	GetScreen()->ForceCloseManageCurseur = NULL;
	GetScreen()->m_CurrentItem = NULL;
}


/************************************************************/
static void Montre_Position_NewSegment(WinEDA_DrawPanel * panel,
        wxDC * DC, bool erase)
/************************************************************/
/* redessin du contour du Segment Edge lors des deplacements de la souris */
{
DRAWSEGMENT * Segment = ( DRAWSEGMENT * )
	panel->GetScreen()->m_CurrentItem ;
int t_fill = DisplayOpt.DisplayDrawItems;

	if(Segment == NULL) return ;

	DisplayOpt.DisplayDrawItems = SKETCH ;

	/* efface ancienne position */
	if( erase )
		Trace_DrawSegmentPcb(panel, DC, Segment, GR_XOR);

	if( Segments_45_Only && (Segment->m_Shape == S_SEGMENT ) )
		{
		Calcule_Coord_Extremite_45(Segment->m_Start.x,Segment->m_Start.y,
							&Segment->m_End.x, &Segment->m_End.y);
		}

	else	/* ici l'angle d'inclinaison est quelconque */
		{
		Segment->m_End = panel->GetScreen()->m_Curseur;
		}

	Trace_DrawSegmentPcb(panel, DC, Segment,GR_XOR);
	DisplayOpt.DisplayDrawItems = t_fill ;
}


