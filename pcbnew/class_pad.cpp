	/************************************************/
	/* class_pad.cpp : fonctions de la classe D_PAD */
	/************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "wxstruct.h"
#include "common.h"
#include "pcbnew.h"
#include "trigo.h"

#ifdef PCBNEW
#include "drag.h"
#endif

#ifdef CVPCB
#include "cvpcb.h"

#endif

#include "protos.h"

	/*******************************/
	/* classe D_PAD : constructeur */
	/*******************************/

D_PAD::D_PAD(MODULE * parent): EDA_BaseStruct( parent, TYPEPAD)
{
	m_NumPadName = 0;
	m_Masque_Layer = CUIVRE_LAYER;
	m_NetCode = 0;				/* Numero de net pour comparaisons rapides */
	m_DrillShape = CIRCLE;		// Drill shape = circle
	m_Size.x = m_Size.y = 500;
	if (m_Parent && (m_Parent->m_StructType  == TYPEMODULE) )
		{
		m_Pos = ((MODULE*)m_Parent)->m_Pos;
		}

	m_PadShape = CIRCLE;			// forme CERCLE, RECT OVALE TRAPEZE ou libre
	m_Attribut = STANDARD;		// NORMAL, SMD, CONN, Bit 7 = STACK
	m_Orient = 0;					// en 1/10 degres

	m_logical_connexion = 0;
	m_physical_connexion = 0;		// variables utilisee lors du calcul du chevelu
	ComputeRayon();
}


D_PAD::~D_PAD(void)
{
}

/****************************/
void D_PAD::ComputeRayon(void)
/****************************/
/* met a jour m_Rayon, rayon du cercle exinscrit
*/
{
	switch (m_PadShape & 0x7F)
		{
		case CIRCLE :
			m_Rayon = m_Size.x/2;
			break;

		case OVALE :
			m_Rayon = MAX(m_Size.x, m_Size.y) / 2;
			break;

		case RECT :
		case SPECIAL_PAD:
		case TRAPEZE:
			m_Rayon = (int)(sqrt((float)m_Size.y * m_Size.y
							+ (float)m_Size.x * m_Size.x) / 2);
			break;
		}

}

/*********************************************/
const wxPoint D_PAD::ReturnShapePos(void) const
/*********************************************/
// retourne la position de la forme (pastilles excentrees)
{
    if ( (m_Offset.x == 0) && (m_Offset.y == 0) ) return m_Pos;
wxPoint shape_pos;
int dX, dY;
    dX = m_Offset.x; dY = m_Offset.y;
	RotatePoint(&dX, &dY, m_Orient );
    shape_pos.x = m_Pos.x + dX; shape_pos.y = m_Pos.y + dY;

    return shape_pos;
}


/****************************************/
wxString D_PAD::ReturnStringPadName(void)
/****************************************/
/* Return pad name as string in a wxString
*/
{
wxString name;

	ReturnStringPadName(name);
	return name;
}

/********************************************/
void D_PAD::ReturnStringPadName(wxString & text)
/********************************************/
/* Return pad name as string in a buffer
*/
{
int ii;
	
	text.Empty();
	for ( ii = 0; ii < 4; ii++ )
	{
		if ( m_Padname[ii] == 0 ) break;
		text.Append(m_Padname[ii]);
	}
}


/********************************************/
void D_PAD::SetPadName(const wxString & name)
/********************************************/
// Change pad name
{
int ii, len; 
	len = name.Length();
	if ( len > 4 ) len = 4;
	for (ii = 0; ii < len; ii ++ ) m_Padname[ii] = name.GetChar(ii);
	for (ii = len; ii < 4; ii ++ ) m_Padname[ii] = 0;
}


/********************************/
void D_PAD::Copy(D_PAD * source)
/********************************/
{
	if (source == NULL) return;

	m_Pos = source->m_Pos;
	m_Masque_Layer = source->m_Masque_Layer;

	memcpy(m_Padname,source->m_Padname, sizeof(m_Padname));/* nom de la pastille */
	m_NetCode = source->m_NetCode;	/* Numero de net pour comparaisons rapides */
	m_Drill = source->m_Drill;			// Diametre de percage
	m_DrillShape = source->m_DrillShape;
	m_Offset = source->m_Offset;        // Offset de la forme
	m_Size = source->m_Size;			// Dimension ( pour orient 0 )
	m_DeltaSize = source->m_DeltaSize;  // delta sur formes rectangle -> trapezes
	m_Pos0 = source->m_Pos0;			// Coord relatives a l'ancre du pad en
									//  orientation 0
	m_Rayon = source->m_Rayon;			// rayon du cercle exinscrit du pad
	m_PadShape = source->m_PadShape;	// forme CERCLE, RECT OVALE TRAPEZE ou libre
	m_Attribut = source->m_Attribut;	// NORMAL, SMD, CONN, Bit 7 = STACK
	m_Orient  = source->m_Orient;		// en 1/10 degres

	m_logical_connexion = 0;		// variable utilisee lors du calcul du chevelu
	m_physical_connexion = 0;		// variable utilisee lors du calcul de la connexit�

	m_Netname = source->m_Netname;
}

/**************************/
void D_PAD::UnLink( void )
/**************************/
/* supprime du chainage la structure Struct
  les structures arrieres et avant sont chainees directement
 */
{
	/* Modification du chainage arriere */
	if( Pback )
		{
		if( Pback->m_StructType != TYPEMODULE)
			{
			Pback->Pnext = Pnext;
			}

		else /* Le chainage arriere pointe sur la structure "Pere" */
			{
			((MODULE*)Pback)->m_Pads = (D_PAD *) Pnext;
			}
		}

	/* Modification du chainage avant */
	if( Pnext) Pnext->Pback = Pback;

	Pnext = Pback = NULL;
}


/*******************************************************************************************/
void D_PAD::Draw(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode)
/*******************************************************************************************/
/* Trac� a l'�cran d'un pad:
	Entree :
		ptr_pad = pointeur sur le pad du module
		offset = offset de trace
		draw_mode = mode de trace ( GR_OR, GR_XOR, GR_AND)
*/
{
int ii;
int  color = 0;
int	ux0,uy0,
	dx,dx0,dy,dy0,
	rotdx,
	delta_cx, delta_cy,
	xc, yc;
int angle;
wxPoint coord[4];
int zoom;
int fillpad = 0;
PCB_SCREEN * screen;
WinEDA_BasePcbFrame * frame;
wxPoint shape_pos;

	screen = panel ? (PCB_SCREEN *) panel->m_Parent->m_CurrentScreen : (PCB_SCREEN *) ActiveScreen;
	frame = screen->GetParentPcbFrame();

	/* Calcul de l'aspect du pad */
	if( frame->m_DisplayPadFill == FILLED) fillpad = 1;

	zoom = screen->GetZoom();

#ifdef PCBNEW
	if( m_Flags & IS_MOVED) fillpad = 0;
#endif
	if ( m_Masque_Layer & CMP_LAYER ) color = g_PadCMPColor ;
	if ( m_Masque_Layer & CUIVRE_LAYER ) color |= g_PadCUColor ;

	if( color == 0)	/* Non cuivre externe */
		{
		color = DARKGRAY;
		}

	if ( draw_mode & GR_SURBRILL )
		{
		if(draw_mode & GR_AND) color &= ~HIGHT_LIGHT_FLAG;
		else color |= HIGHT_LIGHT_FLAG;
		}
	if ( color & HIGHT_LIGHT_FLAG)
		color = ColorRefs[color & MASKCOLOR].m_LightColor;

	GRSetDrawMode(DC, draw_mode);  /* mode de trace */

	/* calcul du centre des pads en coordonnees Ecran : */
    shape_pos = ReturnShapePos();
	ux0 = shape_pos.x - offset.x;
	uy0 = shape_pos.y - offset.y;
	xc = ux0;
	yc = uy0;

	/* le trace depend de la rotation de l'empreinte */

	dx = dx0 = m_Size.x >> 1 ;
	dy = dy0 = m_Size.y >> 1 ; /* demi dim  dx et dy */

	angle = m_Orient;

	switch (m_PadShape & 0x7F)
		{
		case CIRCLE :
			if ( fillpad)
				GRFilledCircle(&panel->m_ClipBox, DC, xc, yc, dx, color, color);
			else GRCircle(&panel->m_ClipBox, DC, xc, yc, dx, color);

			if(DisplayOpt.DisplayPadIsol)
				{
				GRCircle(&panel->m_ClipBox, DC,  xc, yc, dx + g_DesignSettings.m_TrackClearence, color );
				}
			break;

		case OVALE :
			/* calcul de l'entraxe de l'ellipse */
			if( dx > dy )	/* ellipse horizontale */
				{
				delta_cx = dx - dy; delta_cy = 0;
				rotdx = m_Size.y;
				}
			else			/* ellipse verticale */
				{
				delta_cx = 0; delta_cy = dy - dx;
				rotdx = m_Size.x;
				}
			RotatePoint(&delta_cx, &delta_cy, angle);

			if (fillpad)
				{
				GRFillCSegm(&panel->m_ClipBox, DC, ux0 + delta_cx, uy0 + delta_cy,
							ux0 - delta_cx, uy0 - delta_cy,
							rotdx, color);
				}

			else
				{
				GRCSegm(&panel->m_ClipBox, DC, ux0 + delta_cx, uy0 + delta_cy,
							ux0 - delta_cx, uy0 - delta_cy,
							rotdx, color);
				}

			/* Trace de la marge d'isolement */
			if(DisplayOpt.DisplayPadIsol)
				{
				rotdx = rotdx + g_DesignSettings.m_TrackClearence + g_DesignSettings.m_TrackClearence;
				GRCSegm(&panel->m_ClipBox, DC, ux0 + delta_cx, uy0 + delta_cy,
							ux0 - delta_cx, uy0 - delta_cy,
							rotdx, color);
				}
			break;

		case RECT :
		case SPECIAL_PAD:
		case TRAPEZE:
			{
			int ddx, ddy ;
			ddx = m_DeltaSize.x >> 1 ;
			ddy = m_DeltaSize.y >> 1 ; /* demi dim  dx et dy */

			 coord[0].x = - dx - ddy;
			 coord[0].y = + dy + ddx;

			 coord[1].x = - dx + ddy;
			 coord[1].y = - dy - ddx;

			 coord[2].x = + dx - ddy;
			 coord[2].y = - dy + ddx;

			 coord[3].x = + dx + ddy;
			 coord[3].y = + dy - ddx;

			for (ii = 0; ii < 4; ii++)
				{
				RotatePoint(&coord[ii].x,&coord[ii].y, angle);
				coord[ii].x = coord[ii].x + ux0;
				coord[ii].y = coord[ii].y + uy0;
				}

			GRClosedPoly(&panel->m_ClipBox, DC,  4, (int*) coord, fillpad, color, color);

			if(DisplayOpt.DisplayPadIsol)
				{
				dx += g_DesignSettings.m_TrackClearence; dy += g_DesignSettings.m_TrackClearence;
				 coord[0].x = -dx - ddy;
				 coord[0].y =  dy + ddx;

				 coord[1].x = -dx + ddy;
				 coord[1].y = -dy - ddx;

				 coord[2].x =  dx - ddy;
				 coord[2].y = -dy + ddx;

				 coord[3].x =  dx + ddy;
				 coord[3].y =  dy - ddx;

				 for (ii = 0; ii < 4; ii++)
					 {
					 RotatePoint(&coord[ii].x,&coord[ii].y, angle);
					 coord[ii].x = coord[ii].x + ux0;
					 coord[ii].y = coord[ii].y + uy0;
					 }

				GRClosedPoly(&panel->m_ClipBox, DC,  4, (int*)coord, 0, color, color);
				}
			break;

			default:
				break;
			}
		}

	/* Draw the pad hole */
    int cx0 = m_Pos.x - offset.x;
    int cy0 = m_Pos.y - offset.y;
	int hole = m_Drill.x >> 1 ;

	if( fillpad && hole )
	{
		color = g_IsPrinting ? WHITE : BLACK; // ou DARKGRAY;
		if(draw_mode != GR_XOR) GRSetDrawMode(DC, GR_COPY);
		else GRSetDrawMode(DC, GR_XOR);
		switch ( m_DrillShape )
		{
			case CIRCLE:
				if( (hole/zoom) > 1 )	/* C.a.d si le diametre est suffisant */
					GRFilledCircle(&panel->m_ClipBox, DC, cx0, cy0, hole, color, color);
				break;
				
			case OVALE:
				dx = m_Drill.x >> 1 ;
				dy = m_Drill.y >> 1 ; /* demi dim  dx et dy */
				/* calcul de l'entraxe de l'ellipse */
				if( m_Drill.x > m_Drill.y )	/* ellipse horizontale */
				{
					delta_cx = dx - dy; delta_cy = 0;
					rotdx = m_Drill.y;
				}
				else			/* ellipse verticale */
				{
					delta_cx = 0; delta_cy = dy - dx;
					rotdx = m_Drill.x;
				}
				RotatePoint(&delta_cx, &delta_cy, angle);
	
				GRFillCSegm(&panel->m_ClipBox, DC, ux0 + delta_cx, uy0 + delta_cy,
							ux0 - delta_cx, uy0 - delta_cy,
							rotdx, color);
				break;
			
			default:
				break;
		}
	}

	GRSetDrawMode(DC, draw_mode);
	/* Trace du symbole "No connect" ( / ou \ ou croix en X) si necessaire : */
	if( m_Netname.IsEmpty() && DisplayOpt.DisplayPadNoConn )
		{
		dx0 = min(dx0,dy0);
		int nc_color = BLUE;
		if(m_Masque_Layer & CMP_LAYER)	/* Trace forme \ */
			GRLine(&panel->m_ClipBox, DC, cx0 - dx0, cy0 - dx0,
				cx0 + dx0, cy0 + dx0, nc_color );

		if(m_Masque_Layer & CUIVRE_LAYER)	 /* Trace forme / */
			GRLine(&panel->m_ClipBox, DC, cx0 + dx0, cy0 - dx0,
				cx0 - dx0, cy0 + dx0, nc_color );
		}
	/* Trace de la reference */
	if( ! frame->m_DisplayPadNum) return;
	dx = min(m_Size.x, m_Size.y);	/* dx = taille du texte */
	if( (dx / zoom) > 12 )	/* Si taille suffisante pour 2 lettres */
	{
		wxString buffer;
		ReturnStringPadName(buffer);
		dy = buffer.Len();
		if ( dy < 2 ) dy = 2;	/* alignement sur textes a 2 lettres */
		dx = (dx * 9 ) / (dy * 13 );	/* le texte est ajuste pour
									tenir entierement dans la pastille */
		DrawGraphicText(panel, DC, wxPoint(ux0, uy0),
				WHITE, buffer, angle, wxSize(dx, dx),
				GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_CENTER);
	}
}

/*************************************************/
int D_PAD::ReadDescr( FILE * File, int * LineNum)
/*************************************************/
/* Routine de lecture de descr de pads
	la 1ere ligne de descr ($PAD) est supposee etre deja lue
syntaxe:
$PAD
Sh "N1" C 550 550 0 0 1800
Dr 310 0 0
At STD N 00C0FFFF
Ne 3 "netname"
Po 6000 -6000
$EndPAD
*/
{
char Line[1024], BufLine[1024], BufCar[256];
char * PtLine;
int nn, ll, dx, dy, drill_shape;

	while( GetLine(File, Line, LineNum ) != NULL )
		{
		if( Line[0] == '$' ) return( 0 );

		PtLine = Line + 3;  /* Pointe 1er code utile de la ligne */
		switch( Line[0] )
			{
			case 'S':		/* Ligne de description de forme et dims*/
				/* Lecture du nom pad */
				nn = 0;
				while( (*PtLine != '"') && *PtLine ) PtLine++;
				if ( *PtLine ) PtLine++;
				memset(m_Padname,0 ,sizeof(m_Padname) );
				while( (*PtLine != '"') && *PtLine )
					{
					if(nn < (int) sizeof(m_Padname))
						{
						if( * PtLine > ' ' )
							{
							m_Padname[nn] = *PtLine; nn++;
							}
						}
					PtLine++;
					}

				if ( *PtLine == '"' ) PtLine++;

				nn = sscanf(PtLine," %s %d %d %d %d %d",
						BufCar, &m_Size.x, &m_Size.y,
						&m_DeltaSize.x, &m_DeltaSize.y,
						&m_Orient);

				ll = 0xFF & BufCar[0];

				/* Mise a jour de la forme */
				m_PadShape = CIRCLE;
				switch(ll)
					{
					case 'C':	m_PadShape = CIRCLE; break;
					case 'R':	m_PadShape = RECT; break;
					case 'O':	m_PadShape = OVALE; break;
					case 'T':	m_PadShape = TRAPEZE; break;
					}
				ComputeRayon();
				break;

			case 'D':
				drill_shape = 0;
				nn = sscanf(PtLine,"%d %d %d %c %d %d", &m_Drill.x,
					&m_Offset.x, &m_Offset.y, &drill_shape, &dx, &dy );
				m_Drill.y = m_Drill.x;
				m_DrillShape = CIRCLE;
				if (nn >= 6 )	// Drill shape = OVAL ?
				{
					if (drill_shape == 'O' )
					{
						m_Drill.x = dx; m_Drill.y = dy;
						m_DrillShape = OVALE;
					}
				}
				break;

			case 'A':
				nn = sscanf(PtLine,"%s %s %X", BufLine, BufCar,
											&m_Masque_Layer);
				/* Contenu de BufCar non encore utilise ( reserve pour evolutions
					ulterieures */
				/* Mise a jour de l'attribut */
				m_Attribut = STANDARD;
				if( strncmp(BufLine,"SMD",3) == 0 ) m_Attribut = SMD;
				if( strncmp(BufLine,"CONN",4) == 0 ) m_Attribut = CONN;
				if( strncmp(BufLine,"HOLE",4) == 0 ) m_Attribut = P_HOLE;
				if( strncmp(BufLine,"MECA",4) == 0 ) m_Attribut = MECA;
				break;

			case 'N':	/* Lecture du netname */
				nn = sscanf(PtLine,"%d", &m_NetCode);
				/* Lecture du netname */
				ReadDelimitedText(BufLine, PtLine, sizeof(BufLine));
				m_Netname = CONV_FROM_UTF8(StrPurge(BufLine));
				break;

			case 'P':
				nn = sscanf(PtLine,"%d %d", &m_Pos0.x, &m_Pos0.y );
				m_Pos = m_Pos0;
				break;

			default:
				DisplayError(NULL, wxT("Err Pad: Id inconnu"));
				return(1);
			}
		}
	return(2);	/* error : EOF */
}

/***********************************/
int D_PAD::WriteDescr( FILE * File )
/***********************************/
/* Sauvegarde de la description d'un PAD
*/
{
int cshape, NbLigne = 0;;
char * texttype;

	if( GetState(DELETED) ) return(NbLigne);

	/* Generation du fichier pad: */
	fprintf( File,"$PAD\n"); NbLigne++;
	switch(m_PadShape)
	{
		case CIRCLE: cshape = 'C'; break;
		case RECT: cshape = 'R'; break;
		case OVALE: cshape = 'O'; break;
		case TRAPEZE: cshape = 'T'; break;
		default: cshape = 'C';
			DisplayError(NULL, _("Unknown Pad shape"));
			break;
	}
	fprintf(File,"Sh \"%.4s\" %c %d %d %d %d %d\n",
						m_Padname, cshape, m_Size.x, m_Size.y,
						m_DeltaSize.x, m_DeltaSize.y,m_Orient);
	NbLigne++;
	fprintf(File,"Dr %d %d %d", m_Drill.x, m_Offset.x, m_Offset.y );
	if ( m_DrillShape == OVALE )
	{
		fprintf(File," %c %d %d", 'O', m_Drill.x, m_Drill.y );
	}
	fprintf(File,"\n");
	
	NbLigne++;
	switch(m_Attribut)
		{
		case STANDARD: texttype = "STD"; break;
		case SMD: texttype = "SMD"; break;
		case CONN: texttype = "CONN"; break;
		case P_HOLE: texttype = "HOLE"; break;
		case MECA: texttype = "MECA"; break;
		default:
			texttype = "STD";
			DisplayError(NULL, wxT("attribut Pad inconnu"));
			break;
		}
	fprintf(File,"At %s N %8.8X\n", texttype, m_Masque_Layer);
	NbLigne++;

	fprintf(File,"Ne %d \"%s\"\n", m_NetCode,CONV_TO_UTF8(m_Netname));
	NbLigne++;

	fprintf(File,"Po %d %d\n", m_Pos0.x, m_Pos0.y );
	NbLigne++;

	fprintf( File,"$EndPAD\n");
	NbLigne++;
	return(NbLigne);
}

/******************************************************/
void D_PAD::Display_Infos(WinEDA_BasePcbFrame * frame)
/******************************************************/
/* Affiche en bas d'ecran les caract de la pastille demandee */
{
int ii;
MODULE* Module;
wxString Line;
int pos = 1;
/* Pad messages */
wxString Msg_Pad_Shape[6] =
	{ wxT("??? "), wxT("Circ"), wxT("Rect"), wxT("Oval"), wxT("trap"), wxT("spec") } ;

wxString Msg_Pad_Layer[8] =
	{ wxT("??? "), wxT("cmp   "), wxT("cu    "), wxT("cmp+cu "), wxT("int    "),
		 wxT("cmp+int "), wxT("cu+int "), wxT("all    ") } ;
wxString Msg_Pad_Attribut[5] =
		{ wxT("norm"), wxT("smd "), wxT("conn"), wxT("hole"), wxT("????")} ;


	frame->MsgPanel->EraseMsgBox();

	/* Recherche du module correspondant */
	Module = (MODULE *) m_Parent;
	if(Module)
		{
		wxString msg = Module->m_Reference->m_Text;
		Affiche_1_Parametre(frame, pos,_("Module"), msg, DARKCYAN) ;
		ReturnStringPadName(Line);
		pos += 8;
		Affiche_1_Parametre(frame, pos,_("RefP"),Line,BROWN) ;
		}
	pos += 4;
	Affiche_1_Parametre(frame, pos,_("Net"),m_Netname, DARKCYAN);

	/* pour mise au point (peut etre supprim�) : Affichage du numero de Net et sous net */
	pos += 10;
#if 0
	Line.Printf( wxT("%d.%d "),m_logical_connexion, m_physical_connexion);
	Affiche_1_Parametre(frame, pos,"L.P",Line,WHITE);
#endif

	ii = 0;
	if(m_Masque_Layer & CUIVRE_LAYER) ii = 2;
	if(m_Masque_Layer & CMP_LAYER) ii += 1;
	if((m_Masque_Layer & ALL_CU_LAYERS) == ALL_CU_LAYERS) ii = 7;
	pos += 3;
	Affiche_1_Parametre(frame, pos,_("Layer"),Msg_Pad_Layer[ii], DARKGREEN) ;

	pos += 6;
	Affiche_1_Parametre(frame, pos,Msg_Pad_Shape[m_PadShape],wxEmptyString, DARKGREEN);
	/* Affichage en couleur diff si pad stack ou non */

	if (m_Attribut & PAD_STACK)
		Affiche_1_Parametre(frame, -1,wxEmptyString,Msg_Pad_Attribut[m_Attribut&15],RED);
	else Affiche_1_Parametre(frame, -1,wxEmptyString,Msg_Pad_Attribut[m_Attribut&15], DARKGREEN);

	valeur_param(m_Size.x,Line) ;
	pos += 6;
	Affiche_1_Parametre(frame, pos,_("H Size"),Line,RED) ;

	valeur_param(m_Size.y,Line) ;
	pos += 7;
	Affiche_1_Parametre(frame, pos,_("V Size"),Line,RED) ;

	pos += 7;
		valeur_param((unsigned)m_Drill.x,Line) ;
	if ( m_DrillShape == CIRCLE )
	{
		Affiche_1_Parametre(frame, pos,_("Drill"),Line,RED);
	}
	else
	{
		valeur_param((unsigned)m_Drill.x,Line);
		wxString msg;
		valeur_param((unsigned)m_Drill.x,msg);
		Line += wxT(" ") + msg;
		Affiche_1_Parametre(frame, pos,_("Drill X / Y"),Line,RED);
	}
	
	
	int module_orient = Module ? Module->m_Orient : 0;
	if( module_orient )
		Line.Printf( wxT("%3.1f(+%3.1f)"),
			(float)(m_Orient - module_orient) /10, (float)module_orient /10);
	else
		Line.Printf( wxT("%3.1f"), (float) m_Orient /10);
	pos += 8;
	Affiche_1_Parametre(frame, pos,_("Orient"),Line,BLUE);

	valeur_param(m_Pos.x,Line);
	pos += 8;
	Affiche_1_Parametre(frame, pos,_("X Pos"),Line,BLUE) ;

	valeur_param(m_Pos.y,Line);
	pos += 6;
	Affiche_1_Parametre(frame, pos,_("Y pos"),Line,BLUE) ;
}
