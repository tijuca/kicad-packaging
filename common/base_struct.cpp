	/********************************************************/
	/* methodes pour les structures de base:				*/
	/*		EDA_BaseStruct (classe non utilisable seule)	*/
	/*		EDA_TextStruct (classe non utilisable seule)	*/
	/********************************************************/

		/* Fichier base_struct.cpp */

#include "fctsys.h"
#include "gr_basic.h"
#include "trigo.h"
#include "common.h"
#include "wxstruct.h"
#include "base_struct.h"
#include "grfonte.h"


// DrawStructureType names for error messages only:
static wxChar DrawStructureTypeName[MAX_STRUCT_TYPE_ID+1]
#ifdef MAIN
 = {
"Not init",

"Pcb",
"Equipot",
"Module",
"Pad",
"DrawSegment",
"Text (pcb)",
"Text module",
"edge module",
"track",
"zone",
"via",
"marker",
"cotation",
"mire",
"screen",
"block",
"edge zone",

"Polyline",
"Junction",
"Text",
"Label",
"Glob label",
"Lib item",
"Pick struct",
"Segment",
"Raccord",
"Sheet",
"Sheet label",
"Marker",
"No connect",
"Text (lib item)",
"Screen",
"Block locate",
"Library component",
"lib cmp draw circle",
"lib cmp draw graphic text",
"lib cmp draw rect",
"lib cmp draw poly line",
"lib cmp draw line",
"lib cmp pin",
"lib cmp field",
"unknown"
}
#endif
;




enum textbox {
	ID_TEXTBOX_LIST = 8010
};


/******************************************************************************/
EDA_BaseStruct::EDA_BaseStruct(EDA_BaseStruct * parent, int idType)
/******************************************************************************/
{
	InitVars();
	m_StructType = idType;
	m_Parent = parent;	/* Chainage hierarchique sur struct racine */
}


/********************************************/
EDA_BaseStruct::EDA_BaseStruct(int idType)
/********************************************/
{
	InitVars();
	m_StructType = idType;
}

/********************************************/
void EDA_BaseStruct::InitVars(void)
/********************************************/
{
	m_StructType = TYPE_NOT_INIT;
	Pnext = NULL;		/* Chainage avant */
	Pback = NULL;		/* Chainage arriere */
	m_Parent = NULL;	/* Chainage hierarchique sur struct racine */
	m_Son = NULL;		/* Chainage hierarchique sur struct fille */
	m_Flags = 0;		/* Indicateur utilise temporairement dans certaines routines */
	m_TimeStamp = 0;	// signature temporelle (lien avec une entite..)
	m_Status = 0;
	m_Selected = 0;		/* Used by block commands, and selective editing */
}


/* Gestion de l'etat (status) de la structure (active, deleted..) */
int EDA_BaseStruct::GetState(int type)
{
	return(m_Status & type);
}

void EDA_BaseStruct::SetState(int type, int state )
{
	if( state ) m_Status |= type;	/* state = ON ou OFF */
	else m_Status &= ~type;
}


/*********************************************************/
void EDA_BaseStruct::AddToChain(EDA_BaseStruct * laststruct)
/*********************************************************/
/*
addition d'une nouvelle struct a la liste chain�e, apres la structure laststruct
*/
{
	Pnext = laststruct->Pnext;
	Pback = laststruct;
	laststruct->Pnext = this;
}


#if 0
/**************************************************************/
void EDA_BaseStruct::Place(WinEDA_DrawFrame * frame, wxDC * DC)
/**************************************************************/
/* fonction virtuelle de placement: non utilisee en pcbnew (utilisee eeschema)
---- A mieux utiliser (TODO...)
*/
{
}
#endif

/*********************************************/
wxString EDA_BaseStruct::ReturnClassName(void)
/*********************************************/
/* Used at run time for diags: return the class name of the item,
	from its .m_StructType value.
*/
{
int ii = m_StructType;
wxString classname;

	if ( (ii < 0) || (ii > MAX_STRUCT_TYPE_ID) )
		ii = MAX_STRUCT_TYPE_ID;
	classname = DrawStructureTypeName[ii];

	return classname;
}



/**********************************************************************************************/
EDA_BaseLineStruct::EDA_BaseLineStruct(EDA_BaseStruct * StructFather, DrawStructureType idtype):
				EDA_BaseStruct(StructFather, idtype)
/**********************************************************************************************/
{
	m_Layer = 0;
	m_Width = 0;                // 0 = line, > 0 = tracks, bus ...
};


/*********************************************************/
/* EDA_TextStruct (classe de base, non utilis�e seule */
/*********************************************************/
EDA_TextStruct::EDA_TextStruct(const wxString & text)
{
	m_Layer = 0;
	m_Size.x = m_Size.y = DEFAULT_SIZE_TEXT;	/* XY size of font */
	m_Orient = 0;                               /* Orient in 0.1 degrees */
	m_Attributs = 0;
	m_Miroir = 0;								// vue normale / miroir
	m_HJustify = GR_TEXT_HJUSTIFY_CENTER;
	m_VJustify = GR_TEXT_VJUSTIFY_CENTER;	/* Justifications Horiz et Vert du texte */
	m_Width = 0;							/* epaisseur du trait */
	m_CharType = 0;							/* normal, bold, italic ... */
	m_Text = text;
	m_ZoomLevelDrawable = 0;				/* Niveau de zoom acceptable pour affichage normal */
	m_TextDrawings = NULL;					/* Pointeur sur le dessin du caractere */
	m_TextDrawingsSize = 0;                 /* taille du tableau point� */
}


EDA_TextStruct::~EDA_TextStruct(void)
{
	if( m_TextDrawings )	/* pointeur sur la liste des segments de dessin */
		{
		free(m_TextDrawings); m_TextDrawings = NULL;
		}
	m_TextDrawingsSize = 0;			/* nombre de sommets a dessiner */
}

/*************************************************/
int EDA_TextStruct::Locate(const wxPoint & posref)
/*************************************************/
/* locate function
	return:
		1 if posref is inside the text area.
		0 else.
*/
{
int dx, dy;
int spot_cX, spot_cY;

	dx = (Pitch() * GetLength()) / 2;
	dy = m_Size.y /2 ;

	/* Is the ref point inside the text area ?  */
	spot_cX = posref.x - m_Pos.x ; spot_cY = posref.y - m_Pos.y ;
	RotatePoint(&spot_cX, &spot_cY, - m_Orient);
	if( (abs(spot_cX) <= abs(dx) ) && ( abs(spot_cY) <= abs(dy) ) ) return 1;
	return 0;
}

/*******************************/
int EDA_TextStruct::Pitch(void)
/*******************************/
/* retourne le pas entre 2 caracteres
*/
{
	return ((10 * m_Size.x ) / 9) + m_Width;
}

/***************************************************************/
void EDA_TextStruct::Draw(WinEDA_DrawPanel * panel, wxDC * DC,
				const wxPoint & offset, int color, int draw_mode,
				int display_mode, int anchor_color)
/***************************************************************/
/*
 Trace de 1 texte type EDA_TextStruct.
	offset = Offset de trace (usuellement (0,0)
	color = couleur du texte
	draw_mode = GR_OR, GR_XOR.., -1 si mode courant.
	display_mose = FILAIRE, FILLED ou SKETCH
    anchor_color = couleur de l'ancre ( -1 si pas d'ancre ).
*/
{
int zoom;
int coord[100];
int ii, jj, kk, ll, nbpoints;
int width;

	if( m_TextDrawings == NULL)	/* pointeur sur la liste des segments de dessin */
		CreateDrawData();
	if( m_TextDrawings == NULL)	return;

	zoom = panel->GetZoom();
	width = m_Width / zoom;
	if ( display_mode == FILAIRE ) width = 0;
	/* choix de la couleur du texte : */
	if ( draw_mode != -1 ) GRSetDrawMode(DC, draw_mode);

	/* trace du texte */
	if ( zoom > m_ZoomLevelDrawable)
		{
		GRLine(&panel->m_ClipBox, DC,
			m_TextDrawings[1] + offset.x + m_Pos.x,
			m_TextDrawings[2] + offset.y + m_Pos.y,
			m_TextDrawings[3] + offset.x + m_Pos.x,
			m_TextDrawings[4] + offset.y + m_Pos.y,
			color);
		}

	else
		{
		/* trace ancre du texte ? */
		if( anchor_color != -1)
			{
			int anchor_size = 2 * zoom;
			anchor_color &= MASKCOLOR;
			/* calcul de la position du texte */
			int cX = m_Pos.x - offset.x;
			int cY = m_Pos.y - offset.y;
			/* trace ancre du texte */
			GRLine(&panel->m_ClipBox, DC, cX - anchor_size, cY,
				cX + anchor_size, cY, anchor_color);
			GRLine(&panel->m_ClipBox, DC, cX, cY - anchor_size ,
				cX, cY + anchor_size , anchor_color);
			}
		jj = 5; ii = jj+1;
		while (ii < m_TextDrawingsSize)
			{
			nbpoints = m_TextDrawings[jj];
			if (nbpoints > 50 ) nbpoints = 50;
			for ( kk = 0, ll = 0; (kk < nbpoints) && (ii < m_TextDrawingsSize) ; kk++)
				{
				coord[ll] = m_TextDrawings[ii] + offset.x + m_Pos.x;
				ll++; ii++;
				coord[ll] = m_TextDrawings[ii] + offset.y + m_Pos.y;
				ll++; ii++;
				}
			jj = ii; ii++;

			if( width > 2)
				{
				for ( kk = 0, ll = 0; kk < (nbpoints-1); kk ++, ll+= 2 )
					{
					if( display_mode == SKETCH)
						GRCSegm(&panel->m_ClipBox, DC,
						coord[ll],coord[ll+1],
						coord[ll+2],coord[ll+3],
						m_Width, color) ;

					else GRFillCSegm(&panel->m_ClipBox, DC,
						coord[ll],coord[ll+1],
						coord[ll+2],coord[ll+3],
						m_Width, color) ;
					}
				}
			else
				GRPoly(&panel->m_ClipBox, DC, nbpoints, coord, 0, color, color);

			}
		 }
}

/****************************************/
void EDA_TextStruct::CreateDrawData(void)
/****************************************/
/* Cree le tableau de donn�es n�cessaire au trace d'un texte (pcb, module..)
	Met a jour:
	m_ZoomLevelDrawable 	Niveau de zoom acceptable pour affichage normal
	m_TextDrawings 			Pointeur sur le tableau de donn�es
	m_TextDrawingsSize      taille (en int) du tableau
 Codage dans le tableau:
 	Suite des coord des sommets des polygones a tracer pr�c�d� du nombre de sommets
	nn xx1 yy1 xx2 yy2 .. xxn yyn mm  xx1 yy1 xx2 yy2 .. xxm yym
	les 2 premiers sommets sont le segment symbolisant le texte pour les
	affichages a trop petite echelle
*/
{
int ii, jj, kk,nbchar, nbpoints, AsciiCode, endcar;
int k1 , k2 , x0 , y0 ;
int size_h , size_v , espacement ;
char f_cod , plume = 'U';
const wxChar * ptr;
const SH_CODE * ptcar;
int ux0, uy0, dx, dy;	// Coord de trace des segments de texte & variables de calcul */
int cX, cY;				// Centre du texte
int ox, oy;				// coord de trace du caractere courant
int coord[1000];			// liste des coord des segments a tracer

	if( m_TextDrawings )	/* pointeur sur la liste des segments de dessin */
	{
		free(m_TextDrawings); m_TextDrawings = 0;
	}
	m_TextDrawingsSize = 0;			/* nombre de segments a dessiner */

	nbchar = m_Text.Length();
	if ( nbchar == 0 ) return;

	size_h = m_Size.x;
	size_v = m_Size.y;
	espacement = Pitch();
	if ( m_Miroir == 0 )
		{
		size_h = -size_h; espacement = - espacement;
		}

	kk = 0; ptr = m_Text.GetData() ; /* ptr pointe 1er caractere du texte */

	/* calcul de la position du debut des textes: ox et oy */
	ox = cX = 0; oy = cY = 0;

	/* Calcul du cadrage du texte */
	dx = (espacement * nbchar) / 2;
	dy = size_v / 2;	/* Decalage du debut du texte / centre */

	ux0 = uy0 = 0;  /* Decalage du centre du texte / ccord de ref */

	if( (m_Orient == 0) || (m_Orient == 1800) ) /* Texte Horizontal */
		{
		switch(m_HJustify)
			{
			case GR_TEXT_HJUSTIFY_CENTER:
				break;

			case GR_TEXT_HJUSTIFY_RIGHT:
				ux0 = - dx;
				break;

			case GR_TEXT_HJUSTIFY_LEFT:
				ux0 = dx;
				break;
			}

		switch(m_VJustify)
			{
			case GR_TEXT_VJUSTIFY_CENTER:
				break;

			case GR_TEXT_VJUSTIFY_TOP:
				uy0 = dy;
				break;

			case GR_TEXT_VJUSTIFY_BOTTOM:
				uy0 = -dy;
				break;
			}
		}

	else	/* Texte Vertical */
		{
		switch(m_HJustify)
			{
			case GR_TEXT_HJUSTIFY_CENTER:
				break;

			case GR_TEXT_HJUSTIFY_RIGHT:
				ux0 = - dy;
				break;

			case GR_TEXT_HJUSTIFY_LEFT:
				ux0 = dy;
				break;
			}

		switch(m_VJustify)
			{
			case GR_TEXT_VJUSTIFY_CENTER:
				break;

			case GR_TEXT_VJUSTIFY_TOP:
				uy0 = dx;
				break;

			case GR_TEXT_VJUSTIFY_BOTTOM:
				uy0 = -dx;
				break;
			}
		}
	cX += ux0; cY += uy0;
	ox = cX - dx; ; oy = cY + dy;

	/* lorsque les chars sont trop petits pour etre dessines,
	le texte est symbolise par une barre */
	m_ZoomLevelDrawable = m_Size.x / 3;
	dx = (espacement * nbchar) / 2;
	dy = size_v / 2;	 /* Decalage du debut du texte / centre */
	ux0 = cX - dx; uy0 = cY;
	dx += cX; dy = cY;
	RotatePoint(&ux0, &uy0, cX, cY, m_Orient);
	RotatePoint(&dx, &dy, cX, cY, m_Orient);
	coord[0] = 2;
	coord[1] = ux0; coord[2] = uy0;
	coord[3] = dx; coord[4] = dy;

	jj = 5; ii = jj+1; nbpoints = 0;
	while(kk++ < nbchar)
		{
		x0 = 0 ; y0 = 0 ;
		AsciiCode = (*ptr) & 255;
		ptcar = graphic_fonte_shape[AsciiCode] ; /* ptcar pointe la description
						du caractere a dessiner */

		for(endcar = FALSE; ! endcar; ptcar++)
			{
			 f_cod = *ptcar ; /* get code n de la forme selectionnee */
			 switch(f_cod)
				 {
				 case 'X' :
					endcar = TRUE;/* fin du caractere */
					break;

				 case 'U' :
						if(nbpoints && (plume == 'D' ) )
							{
							coord[jj] = nbpoints;
							jj = ii; ii++;
							}
						plume = f_cod; nbpoints = 0;
						break;

				 case 'D' :
				 	plume = f_cod;
					nbpoints = 1;	/* 1 point va suivre (origine du trac�) */
					break ;

				 default  :
					{
					k1 = f_cod; /* trace sur axe V */
					k1 = - ((k1 * size_v) / 9) ;
					ptcar++ ;
					f_cod = *ptcar  ;
					k2 = f_cod; /* trace sur axe H */
					k2 = (k2 * size_h) / 9 ;
					dx = k2 + ox; dy = k1 + oy;
					RotatePoint(&dx, &dy, cX, cY, m_Orient);
					coord[ii] = dx;  ii++; coord[ii] = dy; ii++;
					nbpoints++;
					break ;
					}
				 } /* end switch */
			} /* end boucle for = end trace de 1 caractere */

		ptr++; ox += espacement;
		} /* end trace du texte */

	m_TextDrawings = (int*) malloc( ii * sizeof(int) );
	memcpy ( m_TextDrawings, coord,  ii * sizeof(int));
	m_TextDrawingsSize = ii;	//taille (en int) du tableau
}



/******************************/
void EDA_Rect::Normalize(void)
/******************************/
// Ensure the height ant width are >= 0
{
	if ( m_Size.y < 0 )
	{
		m_Size.y = - m_Size.y;
		m_Pos.y -= m_Size.y;
	}
	if ( m_Size.x < 0 )
	{
		m_Size.x = - m_Size.x;
		m_Pos.x -= m_Size.x;
	}
}


/*******************************************/
bool EDA_Rect::Inside(const wxPoint & point)
/*******************************************/
/* Return TRUE if point is in Rect
	Accept rect size < 0
*/
{
int rel_posx = point.x - m_Pos.x;
int rel_posy = point.y - m_Pos.y;
wxSize size = m_Size;

	if ( size.x < 0 )
	{
		size.x = -size.x; rel_posx += size.x;
	}

 	if ( size.y < 0 )
	{
		size.y = -size.y; rel_posy += size.y;
	}
	return ( (rel_posx >= 0) && (rel_posy >= 0)
          && ( rel_posy <= size.y)
          && ( rel_posx <= size.x)
          );
}

/**************************************************/
EDA_Rect& EDA_Rect::Inflate(wxCoord dx, wxCoord dy)
/**************************************************/
{
     if (-2*dx > m_Size.x)
     {
		// Don't allow deflate to eat more width than we have,
		// a well-defined rectangle cannot have negative width.
		m_Pos.x += m_Size.x/2;
		m_Size.x = 0;
     }
     else
     {
		// The inflate is valid.
		m_Pos.x-=dx;
		m_Size.x += 2*dx;
     }

     if (-2*dy > m_Size.y)
     {
		// Don't allow deflate to eat more height than we have,
		// a well-defined rectangle cannot have negative height.
		m_Pos.y += m_Size.y / 2;
        m_Size.y = 0;
     }
     else
     {
		// The inflate is valid.
		m_Pos.y -= dy;
		m_Size.y += 2*dy;
     }

    return *this;
}



	/**************************/
	/* class DrawPickedStruct */
	/**************************/

/* This class has only one useful member: .m_PickedStruct, used as a link.
	It does not describe really an item.
	It is used to create a linked list of selected items (in block selection).
	Each DrawPickedStruct item has is member: .m_PickedStruct pointing the
	real selected item
*/

/*******************************************************************/
DrawPickedStruct::DrawPickedStruct(EDA_BaseStruct *pickedstruct ) :
			EDA_BaseStruct(DRAW_PICK_ITEM_STRUCT_TYPE)
/*******************************************************************/
{
	m_PickedStruct = pickedstruct;
}

DrawPickedStruct::~DrawPickedStruct(void)
{
}


/*********************************************/
void DrawPickedStruct::DeleteWrapperList(void)
/*********************************************/
/* Delete this item all the items of the linked list
	DO NOT delete the picked items linked by .m_PickedStruct
*/
{
DrawPickedStruct * wrapp_struct, * next_struct;

	for ( wrapp_struct = Next(); wrapp_struct != NULL; wrapp_struct = next_struct)
	{
		next_struct = wrapp_struct->Next();
		delete wrapp_struct;
	}
}
