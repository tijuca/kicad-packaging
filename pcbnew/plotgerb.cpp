		/****************************************/
		/**** Routine de trace GERBER RS274X ****/
		/****************************************/

#include "fctsys.h"

#include "common.h"
#include "plot_common.h"
#include "pcbnew.h"
#include "pcbplot.h"
#include "trigo.h"

#include "protos.h"


/* Format Gerber : NOTES :
Fonctions preparatoires:
	Gn =
	G01			interpolation lineaire ( trace de droites )
	G02,G20,G21	Interpolation circulaire , sens trigo < 0
	G03,G30,G31	Interpolation circulaire , sens trigo > 0
	G04			commentaire
	G06			Interpolation parabolique
	G07			Interpolation cubique
	G10			interpolation lineaire ( echelle 10x )
	G11			interpolation lineaire ( echelle 0.1x )
	G12			interpolation lineaire ( echelle 0.01x )
	G36			Start polygon description
	G37			End polygon description
	G52			plot symbole reference par Dnn code
	G53			plot symbole reference par Dnn ; symbole tourne de -90 degres
	G54			Selection d'outil
	G55			Mode exposition photo
	G56			plot symbole reference par Dnn A code
	G57			affiche le symbole reference sur la console
	G58			plot et affiche le symbole reference sur la console
	G60			interpolation lineaire ( echelle 100x )
	G70			Unites = Inches
	G71			Unites = Millimetres
	G74			supprime interpolation circulaire sur 360 degre, revient a G01
	G75			Active interpolation circulaire sur 360 degre
	G90			Mode Coordonnees absolues
	G91			Mode Coordonnees Relatives

Coordonnees X,Y
	X,Y sont suivies de + ou - et de m+n chiffres (non separes)
			m = partie entiere
			n = partie apres la virgule
			 formats classiques : 	m = 2, n = 3 (format 2.3)
			 						m = 3, n = 4 (format 3.4)
 	ex:
	G__ X00345Y-06123 D__*

Outils et D_CODES
	numero d'outil ( identification des formes )
	1 a 99 	(classique)
	1 a 999
	D_CODES:

	D01 ... D9 = codes d'action:
	D01			= activation de lumiere ( baisser de plume)
	D02			= extinction de lumiere ( lever de plume)
	D03			= Flash
	D09			= VAPE Flash
	D51			= precede par G54 -> Select VAPE

	D10 ... D255 = Indentification d'outils ( d'ouvertures )
				Ne sont pas tj dans l'ordre ( voir tableau ci dessous)
*/

#define DIM_TRAIT 120 	/* largeur des traits (serigraphie,contours) en 0.1 mils */

/* Variables locales : */
static int s_Last_D_code ;
static float Gerb_scale_plot;		/*Coeff de conversion d'unites des traces */
static int scale_spot_mini;		/* Ouverture mini (pour remplissages) */
static D_CODE * ListeDCode;		/* Pointeur sur la zone de stockage des D_CODES
							(typiquement adr_himem - MAX_D_CODE - 10 ) */
wxString GerberFullFileName;
static double scale_x , scale_y ; /* echelles de convertion en X et Y (compte tenu
									des unites relatives du PCB et des traceurs*/

/* Routines Locales */

static void Init_Trace_GERBER(WinEDA_BasePcbFrame * frame, FILE * gerbfile);
static void Init_ApertureList(void);
static void Fin_Trace_GERBER(WinEDA_BasePcbFrame * frame, FILE * gerbfile);
static void Plot_1_CIRCLE_pad_GERBER(wxPoint pos,int diametre) ;
static void trace_1_pastille_OVALE_GERBER(wxPoint pos, wxSize size,int orient);
static void PlotRectangularPad_GERBER(wxPoint pos, wxSize size, int orient);

static int get_D_code(int dx,int dy, int type, int drill ) ;
static void trace_1_pad_TRAPEZE_GERBER(wxPoint pos, wxSize size,wxSize delta,
							int orient,int modetrace);


/********************************************************************************/
void WinEDA_BasePcbFrame::Genere_GERBER(const wxString & FullFileName, int Layer)
/********************************************************************************/
/* Genere les divers fichiers de trace:
	Pour chaque couche  1 fichier xxxc.PHO au format RS274X
*/
{
int tracevia = 1;

	EraseMsgBox();
	GerberFullFileName = FullFileName;

	g_PlotOrient = 0;
	if (Plot_Set_MIROIR) g_PlotOrient |= PLOT_MIROIR;

	/* Calcul des echelles de conversion */
	Gerb_scale_plot = 1.0;  /* pour unites gerber en 0,1 Mils, format 3.4 */
	scale_spot_mini = (int)(spot_mini * 10 * Gerb_scale_plot);
	scale_x = Scale_X * Gerb_scale_plot;
	scale_y = Scale_Y * Gerb_scale_plot;
	g_PlotOffset.x = 0;
	g_PlotOffset.y = 0;

	InitPlotParametresGERBER(g_PlotOffset, scale_x, scale_y);

	/*	Clear the memory used for handle the D_CODE (aperture) list	 */
	Init_ApertureList();
	
	dest = wxFopen(FullFileName, wxT("wt"));
	if (dest == NULL)
	{
		wxString msg = _("unable to create file ") + FullFileName;
		DisplayError(this, msg); return ;
	}

	Affiche_1_Parametre(this, 0, _("File"),FullFileName,CYAN) ;

	Init_Trace_GERBER(this, dest) ;

	nb_plot_erreur = 0 ;

	int layer_mask = g_TabOneLayerMask[Layer];
	switch(Layer)
		{
		case CUIVRE_N :
		case LAYER_N_2 :
		case LAYER_N_3 :
		case LAYER_N_4 :
		case LAYER_N_5 :
		case LAYER_N_6 :
		case LAYER_N_7 :
		case LAYER_N_8 :
		case LAYER_N_9 :
		case LAYER_N_10 :
		case LAYER_N_11:
		case LAYER_N_12:
		case LAYER_N_13 :
		case LAYER_N_14 :
		case LAYER_N_15 :
		case CMP_N  :
			Plot_Layer_GERBER(dest,layer_mask, 0, 1);
			break;

		case SOLDERMASK_N_CU :
		case SOLDERMASK_N_CMP :  /* Trace du vernis epargne */
			if ( g_DrawViaOnMaskLayer ) tracevia = 1;
			else tracevia = 0;
			Plot_Layer_GERBER(dest, layer_mask, g_DesignSettings.m_MaskMargin, tracevia);
			break;

		case SOLDERPASTE_N_CU :
		case SOLDERPASTE_N_CMP :  /* Trace du masque de pate de soudure */
			Plot_Layer_GERBER(dest, layer_mask, 0, 0);
			break;

		default:
			Plot_Serigraphie(PLOT_FORMAT_GERBER,dest, layer_mask);
			break;
		}

	Fin_Trace_GERBER(this, dest) ;
}


/***********************************************************************/
void WinEDA_BasePcbFrame::Plot_Layer_GERBER(FILE * File,int masque_layer,
						int garde, int tracevia)
/***********************************************************************/
/* Trace en format GERBER. d'une couche cuivre ou masque
*/
{
wxPoint pos;
wxSize size;
MODULE * Module;
D_PAD * PtPad;
TRACK * track ;
EDA_BaseStruct * PtStruct;
wxString msg;

	masque_layer |= EDGE_LAYER;	/* Les elements de la couche EDGE sont tj traces */

	/* trace des elements type Drawings Pcb : */
	PtStruct = m_Pcb->m_Drawings;
	for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext )
		{
		switch( PtStruct->m_StructType )
			{
			case TYPEDRAWSEGMENT:
				PlotDrawSegment( (DRAWSEGMENT*) PtStruct, PLOT_FORMAT_GERBER,
												masque_layer);
				break;

			case TYPETEXTE:
				PlotTextePcb((TEXTE_PCB*) PtStruct,PLOT_FORMAT_GERBER,
												masque_layer);
				break;

			case TYPECOTATION:
				PlotCotation((COTATION*) PtStruct, PLOT_FORMAT_GERBER,
												masque_layer);
				break;

			case TYPEMIRE:
				PlotMirePcb((MIREPCB*) PtStruct, PLOT_FORMAT_GERBER,
												masque_layer);
				break;

			case TYPEMARQUEUR:
				break;

			default:
				DisplayError(this, wxT("Type Draw non gere"));
				break;
			}
		}

	/* Trace des Elements des modules autres que pads */
	nb_items = 0 ;
	Affiche_1_Parametre(this, 38, wxT("DrawMod"), wxEmptyString,GREEN) ;
	Module = m_Pcb->m_Modules;
	for( ; Module != NULL ;Module = (MODULE *)Module->Pnext )
		{
		 PtStruct = Module->m_Drawings;
		 for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext )
			 {
			switch( PtStruct->m_StructType )
				 {
				 case TYPEEDGEMODULE:
					if( masque_layer & g_TabOneLayerMask[((EDGE_MODULE*)PtStruct)->m_Layer] )
						Plot_1_EdgeModule(PLOT_FORMAT_GERBER, (EDGE_MODULE*) PtStruct);
					break;

				default: break;
				}
			}
		}

	/* Trace des Elements des modules : Pastilles */
	nb_items = 0 ;
	Affiche_1_Parametre(this, 48, wxT("Pads"),wxEmptyString,GREEN) ;
	Module = m_Pcb->m_Modules;
	for( ; Module != NULL ;Module = (MODULE *)Module->Pnext )
	{
		PtPad = (D_PAD*) Module->m_Pads;
		for ( ; PtPad != NULL ; PtPad = (D_PAD*)PtPad->Pnext )
		{
			wxPoint shape_pos;
			if( (PtPad->m_Masque_Layer & masque_layer) == 0)
							continue ;
			shape_pos = PtPad->ReturnShapePos();
			pos = shape_pos;

			size.x = PtPad->m_Size.x + (garde * 2) ;
			size.y = PtPad->m_Size.y + (garde * 2) ;
			
			/* Don't draw a null size item : */
			if ( (size.x == 0) || (size.y == 0) ) continue;

			nb_items++ ;

			switch (PtPad->m_PadShape)
			{
				case CIRCLE :
					Plot_1_CIRCLE_pad_GERBER(pos,size.x) ;
					break ;

				case OVALE :
					{
					trace_1_pastille_OVALE_GERBER(pos, size,PtPad->m_Orient);
					break ;
					}

				case TRAPEZE :
					{
					wxSize delta = PtPad->m_DeltaSize;
					trace_1_pad_TRAPEZE_GERBER(pos,size,
								delta, PtPad->m_Orient, FILLED) ;
					break ;
					}

				case RECT:
				default:
					PlotRectangularPad_GERBER(pos,size, PtPad->m_Orient) ;
					break ;
			}
			msg.Printf( wxT("%d"),nb_items) ;
			Affiche_1_Parametre(this, 48,wxEmptyString, msg,GREEN) ;
		}
	}
	/* trace des VIAS : */
	if(tracevia)
	{
		nb_items = 0 ;
		Affiche_1_Parametre(this, 56, wxT("Vias"), wxEmptyString,RED) ;
		for( track = m_Pcb->m_Track; track != NULL; track = (TRACK*) track->Pnext)
		{
			if( track->m_StructType != TYPEVIA ) continue;
			SEGVIA * Via = (SEGVIA *) track;
			/* vias not plotted if not on selected layer, but if layer
			== SOLDERMASK_LAYER_CU or SOLDERMASK_LAYER_CMP, vias are drawn ,
			if they are on a external copper layer
			*/
			int via_mask_layer = Via->ReturnMaskLayer();
			if ( (via_mask_layer & CUIVRE_LAYER ) ) via_mask_layer |= SOLDERMASK_LAYER_CU;
			if ( (via_mask_layer & CMP_LAYER ) ) via_mask_layer |= SOLDERMASK_LAYER_CMP;
			if( (via_mask_layer & masque_layer) == 0 ) continue;

			pos = Via->m_Start;
			size.x = size.y = Via->m_Width  + (garde * 2);
			Plot_1_CIRCLE_pad_GERBER(pos,size.x) ;
			nb_items++ ; msg.Printf( wxT("%d"),nb_items) ;
			Affiche_1_Parametre(this, 56,wxEmptyString, msg,RED) ;
		}
	}
	/* trace des pistes : */
	nb_items = 0 ;
	Affiche_1_Parametre(this, 64, wxT("Tracks"),wxEmptyString,YELLOW) ;

	for( track = m_Pcb->m_Track; track != NULL; track = (TRACK*) track->Pnext)
	{
		wxPoint end;

		if ( track->m_StructType == TYPEVIA ) continue ;
		if( (g_TabOneLayerMask[track->m_Layer] & masque_layer) == 0 ) continue;

		size.x = size.y = track->m_Width;
		pos = track->m_Start; end = track->m_End;

		PlotGERBERLine(pos,end, size.x) ;

		nb_items++ ; msg.Printf( wxT("%d"),nb_items) ;
		Affiche_1_Parametre(this, 64, wxEmptyString, msg,YELLOW) ;
	}

	/* trace des zones: */
	nb_items = 0 ;
	if ( m_Pcb->m_Zone ) Affiche_1_Parametre(this, 72, wxT("Zones  "),wxEmptyString,YELLOW) ;

	for( track = m_Pcb->m_Zone; track != NULL; track = (TRACK*) track->Pnext)
	{
		wxPoint end;

		if( (g_TabOneLayerMask[track->m_Layer] & masque_layer) == 0 ) continue;

		size.x = size.y = track->m_Width;
		pos = track->m_Start; end = track->m_End;

		PlotGERBERLine(pos,end, size.x) ;

		nb_items++ ; msg.Printf( wxT("%d"),nb_items) ;
		Affiche_1_Parametre(this, 72, wxEmptyString,msg,YELLOW) ;
	}
}


/**********************************************************************/
void trace_1_pastille_OVALE_GERBER(wxPoint pos, wxSize size, int orient)
/**********************************************************************/
/* Trace 1 pastille OVALE en position pos_X,Y:
		dimensions dx,dy,
		orientation orient
	Pour une orientation verticale ou horizontale, la forme est flashee
	Pour une orientation quelconque la forme est tracee comme un segment
*/
{
int ii ;
int x0, y0, x1, y1, delta;

	if( (orient == 900) || (orient == 2700)) /* orient tournee de 90 deg */
		{
		EXCHG(size.x,size.y);
		}

	/* Trace de la forme flashee */
	if( (orient == 0) || (orient == 900) ||
		(orient == 1800) || (orient == 2700) )
		{
		UserToDeviceCoordinate(pos) ;
		UserToDeviceSize(size);

		ii = get_D_code(size.x,size.y,GERB_OVALE,0) ;
		if (ii != s_Last_D_code )
			{
			sprintf(cbuf,"G54D%d*\n",ref_D_CODE[ii]) ;
			fputs(cbuf,dest) ;
			s_Last_D_code = ii ;
			}
		sprintf(cbuf,"X%5.5dY%5.5dD03*\n", pos.x, pos.y);
		fputs(cbuf,dest) ;
		}

	else	/* Forme tracee comme un segment */
		{
		if(size.x > size.y )
			{
			EXCHG(size.x,size.y); orient += 900;
			}
		/* la pastille est ramenee a une pastille ovale avec dy > dx */
		delta = size.y - size.x;
		x0 = 0; y0 = -delta / 2;
		x1 = 0; y1 = delta / 2;
		RotatePoint(&x0,&y0, orient);
		RotatePoint(&x1,&y1, orient);
		PlotGERBERLine( wxPoint(pos.x + x0, pos.y + y0),
						wxPoint(pos.x + x1, pos.y + y1), size.x);
		}
}


/******************************************************************/
void Plot_1_CIRCLE_pad_GERBER(wxPoint pos,int diametre)
/******************************************************************/
/* Plot a circulat pad or via at the user position pos
*/
{
int ii ;
wxSize size(diametre, diametre);

	UserToDeviceCoordinate(pos);
	UserToDeviceSize(size);

	ii = get_D_code(size.x,size.x,GERB_CIRCLE,0) ;
	if (ii != s_Last_D_code )
		{
		sprintf(cbuf,"G54D%d*\n",ref_D_CODE[ii]) ;
		fputs(cbuf,dest) ;
		s_Last_D_code = ii ;
		}

	sprintf(cbuf,"X%5.5dY%5.5dD03*\n", pos.x, pos.y);
	fputs(cbuf,dest) ;
}

/**************************************************************************/
void PlotRectangularPad_GERBER(wxPoint pos, wxSize size, int orient)
/**************************************************************************/
/*
 Trace 1 pad rectangulaire d'orientation quelconque
	donne par son centre, ses dimensions, et son orientation
	Pour une orientation verticale ou horizontale, la forme est flashee
	Pour une orientation quelconque la forme est tracee par 4 segments
	de largeur 1/2 largeur pad
*/
{
int ii ;

	/* Trace de la forme flashee */
	switch (orient)
	{
		case 900 :
		case 2700 :  /* la rotation de 90 ou 270 degres revient a permutter des dimensions */
				 EXCHG(size.x,size.y);
		case 1800 :
		case 0 :
			UserToDeviceCoordinate(pos) ;
			UserToDeviceSize(size);

			ii = get_D_code(size.x,size.y,GERB_RECT,0) ;
			if (ii != s_Last_D_code )
			{
				sprintf(cbuf,"G54D%d*\n",ref_D_CODE[ii]) ;
				fputs(cbuf,dest) ;
				s_Last_D_code = ii ;
			}
			sprintf(cbuf,"X%5.5dY%5.5dD03*\n", pos.x, pos.y);
			fputs(cbuf,dest) ;
			break;

		default:	  /* Forme tracee par remplissage */
				trace_1_pad_TRAPEZE_GERBER(pos, size, wxSize(0, 0), orient,FILLED);
			break;
	}
}


/*****************************************************************/
void trace_1_contour_GERBER(wxPoint pos, wxSize size, wxSize delta,
										int penwidth, int orient)
/*****************************************************************/
/*
 Trace 1 contour rectangulaire ou trapezoidal d'orientation quelconque
	donne par son centre,
	ses dimensions ,
	ses variations ,
	l'epaisseur du trait,
	et son orientation orient
*/
{
int ii;
wxPoint coord[4];

	size.x /= 2;  size.y /= 2 ;
	delta.x /= 2; delta.y /= 2 ; /* demi dim  dx et dy */

	coord[0].x = pos.x - size.x - delta.y;
	coord[0].y = pos.y + size.y + delta.x;

	coord[1].x = pos.x - size.x + delta.y;
	coord[1].y = pos.y - size.y - delta.x;

	coord[2].x = pos.x + size.x - delta.y;
	coord[2].y = pos.y - size.y + delta.x;

	coord[3].x = pos.x + size.x + delta.y;
	coord[3].y = pos.y + size.y - delta.x;

	for (ii = 0; ii < 4; ii++)
		{
		RotatePoint(&coord[ii].x, &coord[ii].y, pos.x, pos.y, orient);
		}


	PlotGERBERLine( coord[0], coord[1], penwidth);
	PlotGERBERLine( coord[1], coord[2], penwidth);
	PlotGERBERLine( coord[2], coord[3], penwidth);
	PlotGERBERLine( coord[3], coord[0], penwidth);
}


/*******************************************************************/
void trace_1_pad_TRAPEZE_GERBER(wxPoint pos, wxSize size,wxSize delta,
							int orient,int modetrace)
/*******************************************************************/

/*
 Trace 1 pad trapezoidal donne par :
	son centre pos.x,pos.y
	ses dimensions size.x et size.y
	les variations delta.x et delta.y ( 1 des deux au moins doit etre nulle)
	son orientation orient en 0.1 degres
	le mode de trace (FILLED, SKETCH, FILAIRE)

 Le trace n'est fait que pour un trapeze, c.a.d que delta.x ou delta.y
	= 0.

	les notation des sommets sont ( vis a vis de la table tracante )

"       0 ------------- 3   "
"        .             .    "
"         .     O     .     "
"          .         .      "
"           1 ---- 2        "


	exemple de Disposition pour delta.y > 0, delta.x = 0
"           1 ---- 2        "
"          .         .      "
"         .     O     .     "
"        .             .    "
"       0 ------------- 3   "


	exemple de Disposition pour delta.y = 0, delta.x > 0
"       0                  "
"       . .                "
"       .     .            "
"       .           3      "
"       .           .      "
"       .     O     .      "
"       .           .      "
"       .           2      "
"       .     .            "
"       . .                "
"       1                  "
*/
{
int ii , jj;
int dx,dy;
wxPoint polygone[4];	/* coord sommets */
int coord[8];
int ddx, ddy ;

	/* calcul des dimensions optimales du spot choisi = 1/4 plus petite dim */
	dx = size.x - abs(delta.y);
	dy = size.y - abs(delta.x);

	dx = size.x / 2;  dy = size.y / 2 ;
	ddx = delta.x / 2; ddy = delta.y / 2 ;

	polygone[0].x = - dx - ddy; polygone[0].y = + dy + ddx;
	polygone[1].x = - dx + ddy; polygone[1].y = - dy - ddx;
	polygone[2].x = + dx - ddy; polygone[2].y = - dy + ddx;
	polygone[3].x = + dx + ddy; polygone[3].y = + dy - ddx;

	/* Dessin du polygone et Remplissage eventuel de l'interieur */

	for (ii = 0, jj = 0; ii < 4; ii++)
	{
		RotatePoint(&polygone[ii].x, &polygone[ii].y, orient);
		coord[jj] = polygone[ii].x += pos.x;
		jj++;
		coord[jj] = polygone[ii].y += pos.y;
		jj++;
	}

	if(modetrace != FILLED )
	{
		PlotGERBERLine( polygone[0], polygone[1], scale_spot_mini);
		PlotGERBERLine( polygone[1], polygone[2], scale_spot_mini);
		PlotGERBERLine( polygone[2], polygone[3], scale_spot_mini);
		PlotGERBERLine( polygone[3], polygone[0], scale_spot_mini);
	}
	
	else
		PlotPolygon_GERBER(4, coord, TRUE);
}


/**********************************************************/
void PlotGERBERLine(wxPoint start, wxPoint end, int large)
/**********************************************************/
/* Trace 1 segment de piste :
*/
{
int ii ;

	UserToDeviceCoordinate(start);
	UserToDeviceCoordinate(end);

	ii = get_D_code(large,large,GERB_LINE,0) ;
	if (ii != s_Last_D_code )
	{
		sprintf(cbuf,"G54D%d*\n",ref_D_CODE[ii]) ;
		fputs(cbuf,dest) ;
		s_Last_D_code = ii ;
	}
	sprintf(cbuf,"X%5.5dY%5.5dD02*\n",start.x,start.y) ; fputs(cbuf,dest) ;
	sprintf(cbuf,"X%5.5dY%5.5dD01*\n",end.x,end.y) ; fputs(cbuf,dest) ;
}

/********************************************************************/
void PlotCircle_GERBER( wxPoint centre, int rayon, int epaisseur)
/********************************************************************/
/* routine de trace de 1 cercle de centre centre par approximation de segments
*/
{
int ii ;
int ox, oy, fx, fy;
int delta;  /* increment (en 0.1 degres) angulaire pour trace de cercles */

	delta = 120;	/* un cercle sera trace en 3600/delta segments */
	/* Correction pour petits cercles par rapport a l'epaisseur du trait */
	if( rayon < (epaisseur * 10) ) delta = 225; /* 16 segm pour 360 deg */
	if( rayon < (epaisseur * 5) ) delta = 300;  /* 12 segm pour 360 deg */
	if( rayon < (epaisseur * 2) ) delta = 600;  /* 6 segm pour 360 deg */

	ox = centre.x + rayon; oy = centre.y;
	for (ii = delta ; ii < 3600 ; ii += delta )
		{
		fx = centre.x + (int)(rayon * fcosinus[ii]);
		fy = centre.y + (int)(rayon * fsinus[ii]);
		PlotGERBERLine(wxPoint(ox,oy), wxPoint(fx,fy), epaisseur) ;
		ox = fx; oy = fy;
		}

	fx = centre.x + rayon; fy = centre.y;
	PlotGERBERLine(wxPoint(ox,oy), wxPoint(fx,fy), epaisseur) ;
}


/***************************************************************/
void PlotPolygon_GERBER(int nb_segm, int * coord, bool fill)
/***************************************************************/
{
int ii;
wxPoint pos;

	fputs("G36*\n", dest);
	pos.x = *coord; coord++;
	pos.y = *coord; coord++;
	UserToDeviceCoordinate(pos);
	fprintf(dest, "X%5.5dY%5.5dD02*\n", pos.x, pos.y );
	for ( ii = 1; ii < nb_segm; ii++ )
	{
		pos.x = *coord; coord++;
		pos.y = *coord; coord++;
		UserToDeviceCoordinate(pos);
		fprintf(dest, "X%5.5dY%5.5dD01*\n", pos.x, pos.y );
	}

	fputs("G37*\n", dest);
}


/*****************************************************/
int get_D_code(int dx,int dy, int type, int drill )
/*****************************************************/
/*
 Fonction Recherchant et Creant eventuellement la description
	du D_CODE du type et dimensions demandees
*/
{
D_CODE * ptr_tool;
int num_new_D_code = 1 ;


	ptr_tool = ListeDCode;

	while(ptr_tool->m_Type >= 0 )
	{
		if( ( ptr_tool->m_Dx == dx ) &&
			( ptr_tool->m_Dy == dy ) &&
			( ptr_tool->m_Type == type ) )	/* D_code deja existant */
					return(ptr_tool->m_NumDcode) ;

			ptr_tool++ ; num_new_D_code++ ;
	}

	/* At this point, the requested D_CODE does not exist: It will be created */
	if( ref_D_CODE[num_new_D_code] < 0 )
	{	/* Tous les DCODES prevus sont epuises */
		nb_plot_erreur++ ;Affiche_erreur(nb_plot_erreur) ;
		return (-1) ;
	}
	ptr_tool->m_Dx = dx ;
	ptr_tool->m_Dy = dy ;
	ptr_tool->m_Type = type ;
	ptr_tool->m_Drill = drill ;
	ptr_tool->m_NumDcode = num_new_D_code ;
	(ptr_tool+1)->m_Type = -1;
	return(ptr_tool->m_NumDcode) ;
}


/******************************************************************/
void Init_Trace_GERBER(WinEDA_BasePcbFrame * frame, FILE * gerbfile)
/******************************************************************/
{
char Line[1024];

	s_Last_D_code = 0 ;

	DateAndTime(Line);
	wxString Title = g_Main_Title + wxT(" ") + GetBuildVersion();
	fprintf(gerbfile,"G04 (Genere par %s) le %s*\n",Title.GetData(), Line);

	// Specify linear interpol (G01), unit = INCH (G70), abs format (G90):
	fputs("G01*\nG70*\nG90*\n", gerbfile);
	fputs("%MOIN*%\n", gerbfile);		// set unites = INCHES

	/* Set gerber format to 3.4 */
	strcpy(Line,"G04 Gerber Fmt 3.4, Leading zero omitted, Abs format*\n%FSLAX34Y34*%\n") ;

	fputs(Line,gerbfile);

	fputs("G04 APERTURE LIST*\n", gerbfile);
}


/***********************************/
static void Init_ApertureList(void)
/***********************************/
/* Init the memory to handle the aperture list: Create the first aperture descr
	the member .m_Type is used by get_D_code() to handle the end of list:
	.m_Type < 0 is the first free aperture descr */
{
	ListeDCode = (D_CODE *) adr_himem - (MAX_D_CODE + 10 );
	memset(ListeDCode, 0, sizeof(D_CODE)); /* code indiquant a la routine get_D_code la fin de
					la zone de description des D_CODES */
	ListeDCode->m_Type = -1;	// The first aperture is free (.dx = 0)
}

/*****************************************************************/
void Fin_Trace_GERBER(WinEDA_BasePcbFrame * frame, FILE * gerbfile)
/*****************************************************************/
{
char line[1024];
wxString TmpFileName, msg;
FILE * outfile;

	fputs("M02*\n",gerbfile);
	fclose(gerbfile);

	// Reouverture gerbfile pour ajout des Apertures
	gerbfile = wxFopen(GerberFullFileName, wxT("rt") );
	if (gerbfile == NULL)
	{
		msg.Printf( _("unable to reopen file <%s>"),GerberFullFileName.GetData() ) ;
		DisplayError(frame, msg); return ;
	}


	// Ouverture tmpfile
	TmpFileName = GerberFullFileName + wxT(".$$$");
	outfile = wxFopen(TmpFileName, wxT("wt") );
	if ( outfile == NULL )
	{
		fclose(gerbfile);
		DisplayError(frame, wxT("Fin_Trace_GERBER(): Can't Open tmp file"));
		return;
	}

	// Placement des Apertures en RS274X
	rewind(gerbfile);
	while ( fgets(line, 1024, gerbfile) )
	{
		fputs(line, outfile);
		if ( strcmp(strtok(line, "\n\r"),"G04 APERTURE LIST*") == 0 )
		{
			frame->Gen_D_CODE_File(outfile);
			fputs("G04 APERTURE END LIST*\n", outfile);
		}
	}

	fclose(outfile);
	fclose(gerbfile);
	wxRemoveFile(GerberFullFileName);
	wxRenameFile(TmpFileName, GerberFullFileName);

}


/******************************************************/
int WinEDA_BasePcbFrame::Gen_D_CODE_File(FILE * penfile)
/******************************************************/
/* Genere la liste courante des D_CODES
	Retourne le nombre de D_Codes utilises
	Genere une sequence RS274X
 */
{
D_CODE * ptr_tool, * pt_lim ;
int nb_dcodes = 0 ;

	/* Init : */
	ptr_tool = ListeDCode;
	pt_lim = ptr_tool + MAX_D_CODE;

	while((ptr_tool->m_Type >= 0 ) && (ptr_tool <= pt_lim) )
	{
		float fscale = 0.0001;	// For 3.4 format
		char * text;
		sprintf(cbuf,"%%ADD%d", ref_D_CODE[ptr_tool->m_NumDcode]);
		text = cbuf + strlen(cbuf);
		switch ( ptr_tool->m_Type )
		{
			case 1:	// Circle (flash )
				sprintf(text,"C,%f*%%\n", ptr_tool->m_Dx * fscale);
				break;

			case 2:	// RECT
				sprintf(text,"R,%fX%f*%%\n", ptr_tool->m_Dx * fscale,
								ptr_tool->m_Dy * fscale);
				break;

			case 3:	// Circle ( lines )
				sprintf(text,"C,%f*%%\n", ptr_tool->m_Dx * fscale);
				break;

			case 4:	// OVALE
				sprintf(text,"O,%fX%f*%%\n", ptr_tool->m_Dx * fscale,
								ptr_tool->m_Dy * fscale);
				break;

			default:
				DisplayError(this, wxT("Gen_D_CODE_File(): Dcode Type err") );
				break;
		}
		// compensation localisation printf (float x.y généré x,y)
		to_point ( text + 2 );

		fputs(cbuf,penfile) ;
		ptr_tool++ ; nb_dcodes++ ;
	}

	return(nb_dcodes);
}


