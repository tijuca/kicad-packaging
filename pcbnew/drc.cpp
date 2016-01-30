		/*******************************/
		/* Edition des pistes			*/
		/* Routines DRC					*/
		/*******************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"
#include "autorout.h"
#include "trigo.h"

#include "protos.h"

/* variables locales */
class WinEDA_DrcFrame;
WinEDA_DrcFrame * DrcFrame;

int NumberOfErrors;
static MARQUEUR * current_marqueur;	/* Pour gestion des marqueurs sur pcb */

bool AbortDrc, DrcInProgress = FALSE;
static int spot_cX, spot_cY; /* position d'elements a tester */
static int finx, finy;		// coord relatives de l'extremite du segm de reference
static int segm_angle;		// angle d'inclinaison du segment de reference en 0,1 degre
static int segm_long;		// longueur du segment de reference
static int xcliplo,ycliplo,xcliphi,ycliphi ;	/* coord de la surface de securite du segment a comparer */

/* Routines Locales */
static int Pad_to_Pad_Isol(const D_PAD * pad_ref, const D_PAD * pad, const int dist_min);
static bool TestPadDrc(WinEDA_BasePcbFrame *frame, wxDC * DC, D_PAD * pad_ref,
		LISTE_PAD * start_buffer, LISTE_PAD * end_buffer, int max_size, bool show_err);
static int distance_a_pad(const D_PAD* pad_to_test, int seg_width, int isol);
static int distance_a_rond(int cx, int cy, int rayon, int longueur);
static int Tst_Ligne(int x1,int y1,int x2,int y2);
static void Affiche_Erreur_DRC(WinEDA_DrawPanel * panel, wxDC * DC, BOARD * Pcb,
					TRACK * pt_ref, void * pt_item, int errnumber);
static void Affiche_Erreur_DRC(WinEDA_DrawPanel * panel, wxDC * DC,
			BOARD * Pcb, const D_PAD * pad1, const D_PAD * pad2);



	/*******************************************/
	/* Frame d'option et execution DRC general */
	/*******************************************/
#include "dialog_drc.cpp"

/***************************************************************/
void WinEDA_DrcFrame::ListUnconnectedPads(wxCommandEvent & event)
/***************************************************************/
{
	if( (m_Parent->m_Pcb->m_Status_Pcb & LISTE_CHEVELU_OK) == 0 )
		{
		m_Parent->Compile_Ratsnest( m_DC, TRUE);
		}
	if( m_Parent->m_Pcb->m_Ratsnest == NULL ) return;

CHEVELU* Ratsnest = m_Parent->m_Pcb->m_Ratsnest;
int draw_mode = GR_SURBRILL | GR_OR;
WinEDA_DrawPanel * panel = m_Parent->DrawPanel;
int ii;
wxString msg;
float convert = 0.0001;
int unconnect = 0;
	for( ii = m_Parent->m_Pcb->GetNumRatsnests() ;ii > 0; Ratsnest++, ii--)
		{
		if( (Ratsnest->status & CH_ACTIF) == 0) continue;
		unconnect++;
		Ratsnest->pad_start->Draw(panel, m_DC, wxPoint(0,0),draw_mode);
		Ratsnest->pad_end->Draw(panel, m_DC, wxPoint(0,0),draw_mode);
		msg.Printf(_("Unconnected:\nPad @ %.4f,%.4f and\nPad @ %.4f,%.4f\n"),
			Ratsnest->pad_start->m_Pos.x * convert, Ratsnest->pad_start->m_Pos.y * convert,
			Ratsnest->pad_end->m_Pos.x * convert, Ratsnest->pad_end->m_Pos.y * convert);
		m_logWindow->AppendText(msg);
		}

	if ( unconnect ) msg.Printf(_("Active routes: %d\n"), unconnect);
	else msg = _("OK! (No unconnect)\n");
	m_logWindow->AppendText(msg);
	m_logWindow->AppendText(_("End tst"));
}


/****************************************************/
void WinEDA_DrcFrame::TestDrc(wxCommandEvent & event)
/****************************************************/
{
int errors;
wxString msg;
	if ( ! DrcInProgress )
	{
		AbortDrc = FALSE;
		m_logWindow->Clear();
		g_DesignSettings.m_TrackClearence =
			ReturnValueFromTextCtrl(*m_SetClearance, m_Parent->m_InternalUnits);
		errors = m_Parent->Test_DRC(m_DC);
		if ( errors )
			msg.Printf(_("** End Drc: %d errors **\n"),errors);
		else msg = _("** End Drc: No Error **\n");
		m_logWindow->AppendText(msg);
	}
	else wxBell();
}

/*********************************************************/
void WinEDA_DrcFrame::DelDRCMarkers(wxCommandEvent & event)
/*********************************************************/
{
	if ( ! DrcInProgress )
		{
		m_Parent->Erase_Marqueurs(m_DC, FALSE);
		m_Parent->DrawPanel->ReDraw(m_DC,TRUE);
		}
	else wxBell();
}

/******************************************************/
void WinEDA_PcbFrame::Install_Test_DRC_Frame(wxDC * DC)
/******************************************************/
/* Test des isolements : teste les isolements des pistes et place un
	marqueur sur les divers segments en defaut
	Principe:
		Appelle la routine drc() pour chaque segment de piste existant
*/
{
	AbortDrc = FALSE;
	DrcFrame = new WinEDA_DrcFrame(this, DC);
	DrcFrame->ShowModal(); DrcFrame->Destroy();
	DrcFrame = NULL;
}


/***************************************/
int WinEDA_PcbFrame::Test_DRC(wxDC * DC)
/***************************************/
/* Test des isolements : teste les isolements des pistes et place un
	marqueur sur les divers segments en defaut
	Principe:
		Appelle la routine drc() pour chaque segment de piste existant
*/
{
int ii, old_net;
int flag_err_Drc;
TRACK * pt_segm;
D_PAD * pad;
MARQUEUR * Marqueur;
EDA_BaseStruct * PtStruct, *PtNext;
wxString Line;
#define PRINT_NB_PAD_POS 42
#define PRINT_PAD_ERR_POS 48
#define PRINT_TST_POS 20
#define PRINT_NB_SEGM_POS 26
#define PRINT_TRACK_ERR_POS 32

	DrcInProgress = TRUE;
    NumberOfErrors = 0;
	Compile_Ratsnest(DC, TRUE);

	MsgPanel->EraseMsgBox();

	m_CurrentScreen->SetRefreshReq();

	/* Effacement des anciens marqueurs */
	PtStruct = (EDA_BaseStruct*) m_Pcb->m_Drawings;
	for( ; PtStruct != NULL; PtStruct = PtNext)
	{
		PtNext = PtStruct->Pnext;
		if(PtStruct->m_StructType == TYPEMARQUEUR ) DeleteStructure(PtStruct);
	}

	/* Test DRC des pads entre eux */
	Line.Printf( wxT("%d"),m_Pcb->m_NbPads) ;
	Affiche_1_Parametre(this, PRINT_NB_PAD_POS, wxT("NbPad"),Line,RED) ;
	Affiche_1_Parametre(this, PRINT_PAD_ERR_POS, wxT("Pad Err"), wxT("0"), LIGHTRED);
	Line = wxT("Tst Pad to Pad\n");
	if ( DrcFrame ) DrcFrame->m_logWindow->AppendText(Line);
	LISTE_PAD * pad_list_start = CreateSortedPadListByXCoord(m_Pcb);
	LISTE_PAD * pad_list_limit = &pad_list_start[m_Pcb->m_NbPads];
	int max_size = 0;
	LISTE_PAD * pad_list;
	/* Compute the max size of the pads ( used to stop the test) */
	for ( pad_list = pad_list_start; pad_list < pad_list_limit; pad_list++)
	{
		pad = * pad_list;
		if ( pad->m_Rayon > max_size ) max_size = pad->m_Rayon;
	}
	/* Test the pads */
	for ( pad_list = pad_list_start; pad_list < pad_list_limit; pad_list++)
	{
		pad = * pad_list;
		if ( TestPadDrc(this, DC, pad, pad_list, pad_list_limit, max_size, TRUE) == BAD_DRC )
		{
			Marqueur = current_marqueur;
			current_marqueur = NULL;
			if( Marqueur == NULL )
			{
				DisplayError(this, wxT("Test_Drc(): internal err"));
				return NumberOfErrors;
			}
			Line.Printf( wxT("%d"),NumberOfErrors) ;
			Affiche_1_Parametre(this, PRINT_PAD_ERR_POS,wxEmptyString,Line, LIGHTRED);
			Marqueur->Pnext = m_Pcb->m_Drawings;
			Marqueur->Pback = m_Pcb;

			PtStruct = m_Pcb->m_Drawings;
			if(PtStruct) PtStruct->Pback = Marqueur;
			m_Pcb->m_Drawings = Marqueur;
		}
	}
	free(pad_list_start);
	
	/* Test des segments de piste */
	Line.Printf( wxT("%d"),m_Pcb->m_NbSegmTrack) ;
	Affiche_1_Parametre(this, PRINT_NB_SEGM_POS,_("SegmNb"),Line,RED) ;
	Affiche_1_Parametre(this, PRINT_TRACK_ERR_POS,_("Track Err"), wxT("0"), LIGHTRED);
	pt_segm = (TRACK*)m_Pcb->m_Track;

	if ( DrcFrame ) DrcFrame->m_logWindow->AppendText( _("Tst Tracks\n") );
	for( ii = 0, old_net = -1;  pt_segm != NULL; pt_segm = (TRACK*)pt_segm->Pnext, ii++)
	{
		wxYield();
		if(AbortDrc)
		{
			AbortDrc = FALSE; break;
		}
		if( pt_segm->Pnext == NULL) break;
		g_HightLigth_NetCode = pt_segm->m_NetCode;
		flag_err_Drc = Drc(this, DC, pt_segm,(TRACK*)pt_segm->Pnext, 1);
		Line.Printf( wxT("%d"),ii);
		Affiche_1_Parametre(this, PRINT_TST_POS, wxT("Test"),Line,CYAN) ;
		if ( old_net != pt_segm->m_NetCode)
		{
			wxString msg;
			EQUIPOT * equipot = GetEquipot(m_Pcb, pt_segm->m_NetCode);
			if ( equipot ) msg =  equipot->m_Netname + wxT("        ");
			else msg = wxT("<noname>");
			Affiche_1_Parametre(this, 0,_("Netname"),msg, YELLOW);
			old_net = pt_segm->m_NetCode;
		}
		if(flag_err_Drc == BAD_DRC)
		{
			Marqueur = current_marqueur;
			current_marqueur = NULL;
			if( Marqueur == NULL )
				{
				DisplayError(this, wxT("Test_Drc(): internal err"));
				return NumberOfErrors;
				}
			Marqueur->Pnext = m_Pcb->m_Drawings;
			Marqueur->Pback = m_Pcb;

			PtStruct = m_Pcb->m_Drawings;
			if(PtStruct) PtStruct->Pback = Marqueur;
			m_Pcb->m_Drawings = Marqueur;

			GRSetDrawMode(DC, GR_OR);
			pt_segm->Draw(DrawPanel, DC, RED^LIGHTRED );
			Line.Printf( wxT("%d"),NumberOfErrors);
			Affiche_1_Parametre(this, PRINT_TRACK_ERR_POS,wxEmptyString,Line, LIGHTRED);
			Line.Printf( wxT("%d"),m_Pcb->m_NbSegmTrack);
			Affiche_1_Parametre(this, PRINT_NB_SEGM_POS,wxEmptyString,Line,RED) ;
		}
	}

	AbortDrc = FALSE;
	DrcInProgress = FALSE;
	return NumberOfErrors;
}


/***********************************************************************/
int Drc(WinEDA_BasePcbFrame *frame, wxDC * DC,
				TRACK * pt_segment, TRACK * StartBuffer, int show_err)
/***********************************************************************/
/*
Teste le segment en cours de trace:
	pt_segment = pointeur sur segment a controler
	StartBuffer = adresse de la zone des pistes a controler
	(typiquement m_Pcb->m_Track)
	show_err (flag) si 0 pas d'affichage d'erreur sur ecran
	retourne :
		BAD_DRC (1) si Violation DRC
		OK_DRC  (0) si OK
*/
{
int ii ;
TRACK * pttrack;
int x0,y0,xf,yf ;	// coord des extremites du segment teste dans le repere modifie
int dx, dy;			// utilise pour calcul des dim x et dim y des segments
int w_dist;
int MaskLayer ;
int net_code_ref;
int org_X, org_Y;	// Origine sur le PCB des axes du repere centre sur
					//	l'origine du segment de reference
wxPoint shape_pos;

	org_X = pt_segment->m_Start.x; org_Y = pt_segment->m_Start.y ;
	finx = dx = pt_segment->m_End.x - org_X ;
	finy = dy = pt_segment->m_End.y - org_Y ;
	MaskLayer = pt_segment->ReturnMaskLayer();
	net_code_ref = pt_segment->m_NetCode;

	segm_angle = 0;
	if( dx || dy)
	{
		/* calcul de l'angle d'inclinaison en 0,1 degre */
		segm_angle = ArcTangente(dy,dx);
		/* Calcul de la longueur du segment en segm_long : dx = longueur */
		RotatePoint(&dx, &dy, segm_angle); /* segm_long = longueur, yf = 0 */
	}

	/* Ici le segment a ete tourne de segm_angle, et est horizontal, dx > 0 */
	segm_long = dx;

	/******************************************/
	/* Phase 1 : test DRC avec les pastilles :*/
	/******************************************/

	/* calcul de la distance min aux pads : */
	w_dist = (unsigned)(pt_segment->m_Width >> 1 ) ;
	for ( ii = 0 ; ii < frame->m_Pcb->m_NbPads ; ii++)
	{
		D_PAD * pt_pad = frame->m_Pcb->m_Pads[ii];

		/* Pas de probleme si les pads sont en surface autre que la couche,
		sauf si le trou de percage gene	(cas des pastilles perc�es simple
		face sur CI double face */
		if( (pt_pad->m_Masque_Layer & MaskLayer ) == 0 )
		{
			/* We must test the pad hole. In order to use the function "distance_a_pad",
			a pseudo pad is used, with a shape and a size like the hole */
			if ( pt_pad->m_Drill.x == 0 ) continue;
			D_PAD pseudo_pad((MODULE*)NULL);
			pseudo_pad.m_Size = pt_pad->m_Drill;
			pseudo_pad.m_Pos = pt_pad->m_Pos;
			pseudo_pad.m_PadShape = pt_pad->m_DrillShape;
			pseudo_pad.m_Orient = pt_pad->m_Orient;
			pseudo_pad.ComputeRayon();
			spot_cX = pseudo_pad.m_Pos.x - org_X;
			spot_cY = pseudo_pad.m_Pos.y - org_Y;
			if( distance_a_pad(&pseudo_pad, w_dist, g_DesignSettings.m_TrackClearence) != OK_DRC )
			{
				NumberOfErrors++;
				if( show_err )
					Affiche_Erreur_DRC(frame->DrawPanel, DC,
							frame->m_Pcb, pt_segment,pt_pad,0);
				return(BAD_DRC);
			}
			continue;
		}

		/* Le pad doit faire partie d'un net mais pas de probleme
			si le pad est du meme net */
		if( pt_pad->m_NetCode && (net_code_ref == pt_pad->m_NetCode) )
			continue ;

		/* Test DRC pour les pads */
        shape_pos = pt_pad->ReturnShapePos();
		spot_cX = shape_pos.x - org_X;
		spot_cY = shape_pos.y - org_Y;
		if( distance_a_pad(pt_pad, w_dist, g_DesignSettings.m_TrackClearence) == OK_DRC ) continue ;

		/* extremite sur pad ou defaut d'isolation trouve */
		else
		{
			NumberOfErrors++;
 			if( show_err )
				Affiche_Erreur_DRC(frame->DrawPanel, DC,
						frame->m_Pcb, pt_segment,pt_pad,1);
			return(BAD_DRC);
		}
	}

	/**********************************************/
	/* Phase 2 : test DRC avec les autres pistes :*/
	/**********************************************/

	/* Ici le segment de reference est sur l'axe X */

	/* Comparaison du segment de reference aux autres segments de piste */
	pttrack = StartBuffer ;
	for ( ;pttrack != NULL ; pttrack = (TRACK*) pttrack->Pnext )
		{
		//pas de probleme si le segment a tester est du meme net:
		if( net_code_ref == pttrack->m_NetCode ) continue ;
		//pas de probleme si le segment a tester est sur une autre couche :
		if( (MaskLayer & pttrack->ReturnMaskLayer() ) == 0 ) continue ;

		/* calcul de la Distance mini = Isol+ rayon ou demi largeur seg ref
		+ rayon ou demi largeur seg a comparer */
		w_dist = pt_segment->m_Width >> 1;
		w_dist += pttrack->m_Width >> 1;
		w_dist += g_DesignSettings.m_TrackClearence;

		/* si le segment de reference est une via, le traitement est ici */
		if ( pt_segment->m_StructType == TYPEVIA )
			{
			int orgx, orgy;	// origine du repere d'axe X = segment a comparer
			int angle = 0;	// angle du segment a tester;
			orgx = pttrack->m_Start.x; orgy = pttrack->m_Start.y;
			dx = pttrack->m_End.x - orgx ; dy = pttrack->m_End.y - orgy;
			x0 = pt_segment->m_Start.x - orgx ; y0 = pt_segment->m_Start.y - orgy ;

			if( pttrack->m_StructType == TYPEVIA)	/* Tst distance entre 2 vias */
				{
				if( (int)hypot((float)x0,(float)y0) < w_dist )
					{
					NumberOfErrors++;
					if( show_err)
						Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,21);
					return(BAD_DRC) ;
					}
				}
			else	/* Tst distance de via a segment */
				{
				/* calcul de l'angle */
				angle = ArcTangente(dy,dx);

				/* Calcul des coord dans le nouveau repere */
				RotatePoint(&dx, &dy, angle);
				RotatePoint(&x0, &y0, angle);

				if( distance_a_rond(x0, y0, w_dist, dx) == BAD_DRC )
					{
					NumberOfErrors++;
					if(show_err)
						Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,20);
					return(BAD_DRC) ;
					}
				}
			continue;
			}

		/* calcule x0,y0, xf,yf = coord de debut et fin du segment de piste
		 a tester, dans le repere axe X = segment de reference */
		x0 = pttrack->m_Start.x - org_X ; y0 = pttrack->m_Start.y - org_Y ;
		xf = pttrack->m_End.x - org_X ; yf = pttrack->m_End.y - org_Y ;

		RotatePoint(&x0, &y0, segm_angle); RotatePoint(&xf, &yf, segm_angle);

		if ( pttrack->m_StructType == TYPEVIA )
			{
			if( distance_a_rond(x0, y0,w_dist,segm_long) == OK_DRC) continue;
			NumberOfErrors++;
			if(show_err)
				Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,21);
			return(BAD_DRC) ;
			}


		/*
		le segment de reference est Horizontal, par suite des modifs  d'axe.
		3 cas : segment a comparer parallele, perp ou incline
		*/
		if ( y0 == yf )	// segments paralleles
			{
			if ( abs(y0) >= w_dist ) continue ;
			if ( x0 > xf) EXCHG (x0,xf) ; /* pour que x0 <= xf */

			if ( x0 > (-w_dist) && x0 < (segm_long + w_dist)) /* Risque de defaut */
				{
				/* test fin tenant compte des formes arrondies des extremites */
				if ( x0 >= 0 && x0 <= segm_long )
					{
					NumberOfErrors++;
					if ( show_err )
						Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,2);
					return(BAD_DRC) ;
					}
				if( distance_a_rond(x0, y0, w_dist,segm_long) == BAD_DRC)
					{
					NumberOfErrors++;
					if(show_err)
						Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,2);
					return(BAD_DRC) ;
					}
				}
			if ( xf > (-w_dist) && xf < (segm_long + w_dist))
				{
				/* test fin tenant compte des formes arrondies des extremites */
				if ( xf >= 0 && xf <= segm_long )
					{
					NumberOfErrors++;
					if(show_err)
						Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,3);
					return(BAD_DRC) ;
					}
				if( distance_a_rond(xf, yf, w_dist,segm_long) == BAD_DRC)
					{
					NumberOfErrors++;
					if(show_err)
						Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,3);
					return(BAD_DRC) ;
					}
				}

			if ( x0 <=0 && xf >= 0 )
				{
				NumberOfErrors++;
				if(show_err)
					Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,4);
				return(BAD_DRC) ;
				}
			}

		else if ( x0 == xf ) // segments perpendiculaires
			{
			if ( (x0 <= (-w_dist)) || (x0 >= (segm_long + w_dist))) continue ;

			/* test si les segments se croisent */
			if( y0 > yf ) EXCHG(y0, yf);
			if( (y0 < 0) && (yf > 0) )
				{
				NumberOfErrors++;
				if( show_err)
					Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,6);
				return(BAD_DRC);
				}

			/* ici l'erreur est due a une extremite pres d'une extremite du segm
				de reference */
			if(distance_a_rond(x0,y0,w_dist,segm_long) == BAD_DRC)
				{
				NumberOfErrors++;
				if(show_err)
					Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,7);
				return(BAD_DRC) ;
				}
			if(distance_a_rond(xf,yf,w_dist,segm_long) == BAD_DRC)
				{
				NumberOfErrors++;
				if(show_err)
					Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,8);
				return(BAD_DRC) ;
				}
			}
		else // segments quelconques entre eux */
			{
			int bflag = OK_DRC;
			/* calcul de la "surface de securite du segment de reference */
			/* premiere passe : la piste est assimilee a un rectangle */

			xcliplo = ycliplo = -w_dist;
			xcliphi = segm_long + w_dist; ycliphi = w_dist;

			bflag = Tst_Ligne(x0,y0,xf,yf);
			if (bflag == BAD_DRC)
				{
				/* 2eme passe : la piste a des extremites arrondies.
				Si le defaut de drc est du a une extremite : le calcul
				est affine pour tenir compte de cet arrondi */

				xcliplo = 0 ; xcliphi = segm_long ;
				bflag = Tst_Ligne(x0,y0,xf,yf) ;

				if(bflag == BAD_DRC)
					{
					NumberOfErrors++;
					if(show_err)
						Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,9);
					return(BAD_DRC) ;
					}

				else	// L'erreur est due a une extremite du segment de reference:
					{	// il faut tester les extremites de ce segment
					int angle, rx0,ry0,rxf,ryf;
					x0 = pttrack->m_Start.x; y0 = pttrack->m_Start.y;
					xf = pttrack->m_End.x; yf = pttrack->m_End.y;
					dx = xf - x0; dy = yf - y0;
					/* calcul de l'angle d'inclinaison en 0,1 degre */
					angle = ArcTangente(dy,dx);
					/* Calcul de la longueur du segment: dx = longueur */
					RotatePoint(&dx, &dy, angle);

					/* calcul des coord du segment de reference ds le repere
						d'axe X = segment courant en tst */
					rx0 = pt_segment->m_Start.x - x0;
					ry0 = pt_segment->m_Start.y - y0;
					rxf = pt_segment->m_End.x - x0;
					ryf = pt_segment->m_End.y - y0;

					RotatePoint(&rx0,&ry0, angle);
					RotatePoint(&rxf,&ryf, angle);
					if(distance_a_rond(rx0,ry0,w_dist,dx) == BAD_DRC)
						{
						NumberOfErrors++;
						if(show_err)
							Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,10);
						return(BAD_DRC) ;
						}
					if(distance_a_rond(rxf,ryf,w_dist,dx) == BAD_DRC)
						{
						NumberOfErrors++;
						if(show_err)
							Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pt_segment,pttrack,11);
						return(BAD_DRC) ;
						}
					}
				}
			}
		}
	return(OK_DRC) ;
}

/*****************************************************************************/
static bool TestPadDrc(WinEDA_BasePcbFrame *frame, wxDC * DC, D_PAD * pad_ref,
		LISTE_PAD * start_buffer, LISTE_PAD * end_buffer, int max_size, bool show_err)
/*****************************************************************************/
/* Teste l'isolation de pad_ref avec les autres pads.
	end_buffer = upper limit of the pad list.
	max_size = size of the biggest pad (used to stop the test when the X distance is > max_size)
*/
{
int	MaskLayer;
D_PAD * pad;
LISTE_PAD * pad_list = start_buffer;
	
	MaskLayer = pad_ref->m_Masque_Layer & ALL_CU_LAYERS;
	int x_limite = max_size + g_DesignSettings.m_TrackClearence +
		pad_ref->m_Rayon + pad_ref->m_Pos.x;
	for ( ; pad_list < end_buffer ; pad_list++)
	{
		pad = * pad_list;
		if ( pad == pad_ref ) continue;
		
		/* We can stop the test when pad->m_Pos.x > x_limite
		because the list is sorted by X values */
		if ( pad->m_Pos.x > x_limite ) break;

		/* Pas de probleme si les pads ne sont pas sur les memes couches cuivre*/
		if( (pad->m_Masque_Layer & MaskLayer ) == 0 ) continue;

		/* Le pad doit faire partie d'un net,
		mais pas de probleme si les pads sont du meme net */
		if( pad->m_NetCode && (pad_ref->m_NetCode == pad->m_NetCode) )
			continue ;

		/* pas de pb si les pads sont du meme module et
		 de la meme reference ( pads multiples )  */
        if ( (pad->m_Parent == pad_ref->m_Parent) &&
			 (strncmp(pad->m_Padname, pad_ref->m_Padname, 4 ) == 0) )
			 continue;

		if( Pad_to_Pad_Isol(pad_ref, pad, g_DesignSettings.m_TrackClearence) == OK_DRC ) continue ;
		else	/* defaut d'isolation trouve */
		{
			NumberOfErrors++;
 			if( show_err )
				Affiche_Erreur_DRC(frame->DrawPanel, DC, frame->m_Pcb, pad_ref, pad);
			return(BAD_DRC);
		}
	}
	return OK_DRC;
}

/**************************************************************************************/
static int Pad_to_Pad_Isol(const D_PAD * pad_ref, const D_PAD * pad, const int dist_min)
/***************************************************************************************/
/* Return OK_DRC si distance entre pad_ref et pas >= dist_min
et BAD_DRC sinon */
{
wxPoint rel_pos;
int dist, diag;
wxPoint shape_pos;

    rel_pos = pad->ReturnShapePos();
    shape_pos = pad_ref->ReturnShapePos();
 	rel_pos.x -= shape_pos.x;
 	rel_pos.y -= shape_pos.y;
 	dist = (int) hypot( (double) rel_pos.x, (double) rel_pos.y);

   	/* tst rapide: si les cercles exinscrits sont distants de dist_min au moins,
    il n'y a pas de risque: */
	if ( (dist - pad_ref->m_Rayon - pad->m_Rayon) >= dist_min )
		return OK_DRC;

	/* Ici les pads sont proches et les cercles exinxcrits sont trop proches
	Selon les formes relatives il peut y avoir ou non erreur */
	if ( (pad_ref->m_PadShape != CIRCLE) && (pad->m_PadShape == CIRCLE) )
	{
		EXCHG (pad_ref, pad);
		rel_pos.x = - rel_pos.x;
		rel_pos.y = - rel_pos.y;
	}
	
	switch (pad_ref->m_PadShape)
	{
		case CIRCLE:	// pad_ref est assimile a un segment de longeur nulle
			segm_long = 0;
			segm_angle = 0;
			finx = finy = 0;
			spot_cX = rel_pos.x;
			spot_cY = rel_pos.y;
			diag = distance_a_pad(pad, pad_ref->m_Rayon, dist_min );
			break;

		case RECT:
		case OVALE :
		default:
			/* TODO...*/
			diag = OK_DRC;
			break;
	}
	return diag;
}

/***************************************************************************/
static int distance_a_pad(const D_PAD* pad_to_test, int w_segm, int dist_min)
/****************************************************************************/
/*
 Routine adaptee de la "distance()" (LOCATE.CC)
 teste la distance du pad au segment de droite en cours

	retourne:
		0 si distance >= dist_min
		1 si distance < dist_min
	Parametres d'appel:
		pad_to_test	= pointeur sur le pad a tester
		w_segm = demi largeur du segment a tester
		dist_min = marge a respecter

	en variables globales
		segm_long = longueur du segment en test
		segm_angle = angle d'inclinaison du segment;
		finx, finy = coord fin du segment / origine
		spot_cX, spot_cY = position du pad / origine du segment
*/
{
int p_dimx, p_dimy ;	/* demi - dimensions X et Y du pad a controler */
int bflag;
int orient;
int x0, y0, xf, yf ;
int seuil;
int deltay;

	seuil = w_segm + dist_min;
	p_dimx = pad_to_test->m_Size.x >> 1 ;
	p_dimy = pad_to_test->m_Size.y >> 1 ;

	if (pad_to_test->m_PadShape == CIRCLE )
		{
		/* calcul des coord centre du pad dans le repere axe X confondu
			avec le segment en tst */
		RotatePoint(&spot_cX, &spot_cY, segm_angle);
		return (distance_a_rond(spot_cX, spot_cY, seuil+p_dimx, segm_long));
		}
	else
		{
		/* calcul de la "surface de securite" du pad de reference */
		xcliplo = spot_cX - seuil - p_dimx ;
		ycliplo = spot_cY - seuil - p_dimy;
		xcliphi = spot_cX + seuil + p_dimx;
		ycliphi = spot_cY + seuil + p_dimy;
		x0 = y0 = 0 ; xf = finx; yf = finy ;
		orient = pad_to_test->m_Orient;
		RotatePoint(&x0,&y0,spot_cX, spot_cY, -orient);
		RotatePoint(&xf,&yf,spot_cX, spot_cY, -orient);

		bflag = Tst_Ligne(x0,y0,xf,yf) ;

		if (bflag == OK_DRC) return (OK_DRC);
		/* Erreur DRC : analyse fine de la forme de la pastille */

		switch (pad_to_test->m_PadShape )
			{
			default: return(BAD_DRC);

			case OVALE :
				/* test de la pastille ovale ramenee au type ovale vertical */
				if (p_dimx > p_dimy)
					{
					EXCHG(p_dimx,p_dimy); orient += 900;
					if(orient >= 3600) orient -=3600;
					}
				deltay = p_dimy - p_dimx;
				/* ici: p_dimx = rayon,
						delta = dist centre cercles a centre pad */

				/* Test du rectangle separant les 2 demi cercles */
				xcliplo = spot_cX - seuil - p_dimx;
				ycliplo = spot_cY - w_segm - deltay;
				xcliphi = spot_cX + seuil + p_dimx;
				ycliphi = spot_cY + w_segm + deltay;

				bflag = Tst_Ligne(x0,y0,xf,yf);
				if (bflag == BAD_DRC) return(BAD_DRC);

				/* test des 2 cercles */
				x0 = spot_cX; /* x0,y0 = centre du cercle superieur du pad ovale */
				y0 = spot_cY + deltay;
				RotatePoint(&x0,&y0, spot_cX, spot_cY, orient);
				RotatePoint(&x0,&y0, segm_angle);
				bflag = distance_a_rond(x0,y0,p_dimx + seuil, segm_long);
				if( bflag == BAD_DRC) return(BAD_DRC);

				x0 = spot_cX; /* x0,y0 = centre du cercle inferieur du pad ovale */
				y0 = spot_cY - deltay;
				RotatePoint(&x0,&y0, spot_cX, spot_cY, orient);
				RotatePoint(&x0,&y0, segm_angle);
				bflag = distance_a_rond(x0,y0,p_dimx + seuil, segm_long);
				if( bflag == BAD_DRC) return(BAD_DRC);
				break;

			case RECT:	/* 2 rectangle + 4 1/4 cercles a tester */
				/* Test du rectangle dimx + seuil, dimy */
				xcliplo = spot_cX - p_dimx - seuil;
				ycliplo = spot_cY - p_dimy;
				xcliphi = spot_cX + p_dimx + seuil;
				ycliphi = spot_cY + p_dimy;

				bflag = Tst_Ligne(x0,y0,xf,yf);
				if (bflag == BAD_DRC)
					{
					return(BAD_DRC);
					}

				/* Test du rectangle dimx , dimy + seuil */
				xcliplo = spot_cX - p_dimx;
				ycliplo = spot_cY - p_dimy - seuil;
				xcliphi = spot_cX + p_dimx;
				ycliphi = spot_cY + p_dimy + seuil;

				bflag = Tst_Ligne(x0,y0,xf,yf);
				if (bflag == BAD_DRC)
					{
					return(BAD_DRC);
					}

				/* test des 4 cercles ( surface d'solation autour des sommets */
				/* test du coin sup. gauche du pad */
				x0 = spot_cX - p_dimx;
				y0 = spot_cY - p_dimy;
				RotatePoint(&x0,&y0, spot_cX, spot_cY, orient);
				RotatePoint(&x0,&y0, segm_angle);
				bflag = distance_a_rond(x0, y0, seuil, segm_long);
				if( bflag == BAD_DRC)
					{
					return(BAD_DRC);
					}

				/* test du coin sup. droit du pad */
				x0 = spot_cX + p_dimx;
				y0 = spot_cY - p_dimy;
				RotatePoint(&x0,&y0, spot_cX, spot_cY, orient);
				RotatePoint(&x0,&y0, segm_angle);
				bflag = distance_a_rond(x0, y0, seuil, segm_long);
				if( bflag == BAD_DRC)
					{
					return(BAD_DRC);
					}

				/* test du coin inf. gauche du pad */
				x0 = spot_cX - p_dimx;
				y0 = spot_cY + p_dimy;
				RotatePoint(&x0,&y0, spot_cX, spot_cY, orient);
				RotatePoint(&x0,&y0, segm_angle);
				bflag = distance_a_rond(x0, y0, seuil, segm_long);
				if( bflag == BAD_DRC)
					{
					return(BAD_DRC);
					}

				/* test du coin inf. droit du pad */
				x0 = spot_cX + p_dimx;
				y0 = spot_cY + p_dimy;
				RotatePoint(&x0,&y0, spot_cX, spot_cY, orient);
				RotatePoint(&x0,&y0, segm_angle);
				bflag = distance_a_rond(x0, y0, seuil, segm_long);
				if( bflag == BAD_DRC)
					{
					return(BAD_DRC);
					}

				break;

			}
		}
	return(OK_DRC) ;
}

/*******************************************************************/
static int distance_a_rond(int cx, int cy, int rayon, int longueur )
/*******************************************************************/
/*
 Routine analogue a distance_a_pad.
 Calcul de la distance d'un cercle (via ronde, extremite de piste)
	 au segment de droite en cours de controle (segment de reference dans
	 son repere )
	parametres:
		cx, cy: centre du cercle (surface ronde) a tester, dans le repere
							segment de reference
		rayon = rayon du cercle
		longueur = longueur du segment dans son repere (i.e. coord de fin)
	retourne:
		OK_DRC si distance >= rayon
		BAD_DRC si distance < rayon
*/
{
	if ( abs(cy) > rayon) return(OK_DRC);

	if ( (cx >= -rayon ) && (cx <= (longueur+rayon) ) )
		{
		if( (cx >= 0) && (cx <= longueur) ) return(BAD_DRC) ;
		if( cx > longueur) cx -= longueur ;
		if( hypot((double)cx, (double)cy) < rayon ) return(BAD_DRC);
		}

	return(OK_DRC) ;
}


/******************************************************************************/
static void Affiche_Erreur_DRC(WinEDA_DrawPanel * panel, wxDC * DC, BOARD * Pcb,
					TRACK * pt_ref, void * pt_item, int errnumber)
/******************************************************************************/

/* affiche les erreurs de DRC :
	Message d'erreur
	+
	Marqueur
	number = numero d'identification
*/
{
int ercx, ercy;
D_PAD * pt_pad;
TRACK * pt_segm;
wxString msg;

	if( ((EDA_BaseStruct*)pt_item)->m_StructType == TYPEPAD )
		{
		pt_pad = (D_PAD*) pt_item;
		ercx = pt_pad->m_Pos.x; ercy = pt_pad->m_Pos.y;
		msg.Printf(_("%d Err type %d sur PAD @ %d,%d\n"),
				NumberOfErrors, errnumber, ercx,ercy);
		}
	else	/* erreur sur segment de piste */
		{
		pt_segm = (TRACK *) pt_item;
		ercx = pt_segm->m_Start.x; ercy = pt_segm->m_Start.y;
		if(pt_segm->m_StructType == TYPEVIA)
			{
			msg.Printf(_("%d Err type %d: sur VIA @ %d,%d\n"),
					NumberOfErrors, errnumber,ercx,ercy);
			}
		else
			{
			int ercfx = pt_segm->m_End.x, ercfy = pt_segm->m_End.y;
			if(hypot( (double)(ercfx - pt_ref->m_End.x),(double)(ercfy - pt_ref->m_End.y) )
			  < hypot( (double)(ercx - pt_ref->m_End.x),(double)(ercy - pt_ref->m_End.y) ) )
				{
				EXCHG(ercfx, ercx); EXCHG(ercfy, ercy);
				}
			msg.Printf(_("%d Err type %d: sur SEGMENT @ %d,%d\n"),
					NumberOfErrors, errnumber,ercx,ercy);
			}
		}

	if ( DrcFrame ) DrcFrame->m_logWindow->AppendText(msg);
	else panel->m_Parent->Affiche_Message(msg);

	if(current_marqueur == NULL) current_marqueur = new MARQUEUR(Pcb);
	current_marqueur->m_Pos = wxPoint(ercx, ercy);
	current_marqueur->m_Color = WHITE;
	current_marqueur->m_Diag = msg;
	current_marqueur->Draw(panel, DC, GR_OR);
}


/******************************************************************************/
static void Affiche_Erreur_DRC(WinEDA_DrawPanel * panel, wxDC * DC, BOARD * Pcb,
					const D_PAD * pad1, const D_PAD * pad2)
/******************************************************************************/

/* affiche les erreurs de DRC :
	Message d'erreur
	+
	Marqueur
	number = numero d'identification
*/
{
int ercx, ercy;
wxString msg;

	ercx = pad1->m_Pos.x; ercy = pad1->m_Pos.y;
	msg.Printf( _("%d Err pad to pad (PAD @ %d,%d and PAD @ %d,%d\n"),
			NumberOfErrors, ercx,ercy,
			pad2->m_Pos.x, pad2->m_Pos.y);
	if ( DrcFrame ) DrcFrame->m_logWindow->AppendText(msg);
	else panel->m_Parent->Affiche_Message(msg);

	if(current_marqueur == NULL) current_marqueur = new MARQUEUR(Pcb);
	current_marqueur->m_Pos = wxPoint(ercx, ercy);
	current_marqueur->m_Color = WHITE;
	current_marqueur->m_Diag = msg;
	current_marqueur->Draw(panel, DC, GR_OR);
}


	/**********************************************/
	/* int Tst_Ligne(int x1,int y1,int x2,int y2) */
	/**********************************************/

/* Routine utilisee pour tester si une piste est en contact avec une autre piste.

	Cette routine controle si la ligne (x1,y1 x2,y2) a une partie s'inscrivant
	dans le cadre (xcliplo,ycliplo xcliphi,ycliphi) (variables globales,
	locales a ce fichier)

	Retourne OK_DRC si aucune partie commune
	Retourne BAD_DRC si partie commune
*/
#define us unsigned int
static inline int USCALE(us arg, us num, us den)
	{
	int ii;

	ii = (int)( ((float) arg * num) / den);
	return( ii );
	}

#define WHEN_OUTSIDE return(OK_DRC)
#define WHEN_INSIDE

static int Tst_Ligne(int x1,int y1,int x2,int y2)
{
int temp;

do {
	if(x1 > x2) { EXCHG(x1,x2); EXCHG(y1,y2); }
	if((x2 < xcliplo) || (x1 > xcliphi)) { WHEN_OUTSIDE; }
	if(y1 < y2)
		{
		if((y2 < ycliplo) || (y1 > ycliphi)) { WHEN_OUTSIDE;}
		if(y1 < ycliplo)
			{
			temp = USCALE((x2 - x1),(ycliplo - y1),(y2 - y1));
			if((x1 += temp) > xcliphi) { WHEN_OUTSIDE; }
			y1 = ycliplo;
			WHEN_INSIDE;
			}
		if(y2 > ycliphi)
			{
			temp = USCALE((x2 - x1),(y2 - ycliphi),(y2 - y1));
			if((x2 -= temp) < xcliplo) { WHEN_OUTSIDE; }
			y2 = ycliphi;
			WHEN_INSIDE;
			}
		if(x1 < xcliplo)
			{
			temp = USCALE((y2 - y1),(xcliplo - x1),(x2 - x1));
			y1 += temp; x1 = xcliplo;
			WHEN_INSIDE;
			}
		if(x2 > xcliphi)
			{
			temp = USCALE((y2 - y1),(x2 - xcliphi),(x2 - x1));
			y2 -= temp; x2 = xcliphi;
			WHEN_INSIDE;
			}
		}
	else
		{
		if((y1 < ycliplo) || (y2 > ycliphi)) { WHEN_OUTSIDE; }
		if(y1 > ycliphi)
			{
			temp = USCALE((x2 - x1),(y1 - ycliphi),(y1 - y2));
			if((x1 += temp) > xcliphi) { WHEN_OUTSIDE; }
			y1 = ycliphi;
			WHEN_INSIDE;
			}
		if(y2 < ycliplo)
			{
			temp = USCALE((x2 - x1),(ycliplo - y2),(y1 - y2));
			if((x2 -= temp) < xcliplo) { WHEN_OUTSIDE; }
			y2 = ycliplo;
			WHEN_INSIDE;
			}
		if(x1 < xcliplo)
			{
			temp = USCALE((y1 - y2),(xcliplo - x1),(x2 - x1));
			y1 -= temp; x1 = xcliplo;
			WHEN_INSIDE;
			}
		if(x2 > xcliphi)
			{
			temp = USCALE((y1 - y2),(x2 - xcliphi),(x2 - x1));
			y2 += temp; x2 = xcliphi;
			WHEN_INSIDE;
			}
		}
	} while(0);

	if ( ((x2 + x1)/2 <= xcliphi ) && ((x2+x1)/2 >= xcliplo) \
		&& ((y2 + y1)/2 <= ycliphi ) && ((y2+y1)/2 >= ycliplo) )
		{
		return(BAD_DRC) ;
		}

	else return(OK_DRC);
}


