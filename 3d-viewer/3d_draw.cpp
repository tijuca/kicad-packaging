//////////////////////////////////////
// Name:        3d_draw.cpp
//////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "fctsys.h"
#include "trigo.h"


#if !wxUSE_GLCANVAS
#error Please set wxUSE_GLCANVAS to 1 in setup.h.
#endif

#include "common.h"
#include "pcbstruct.h"
#include "macros.h"

//#include "pcbnew.h"

#include "3d_viewer.h"
#include "trackball.h"

#include "3d_struct.h"

static void Draw3D_FilledCircle(double posx, double posy,
				double rayon, double hole_rayon, double zpos);
static void Draw3D_FilledSegment(double startx, double starty,
				double endx, double endy,double width, double zpos);
static void Draw3D_FilledCylinder(double posx, double posy,
				double rayon, double height, double zpos);
static void Draw3D_FilledSegmentWithHole(double startx, double starty,
				double endx, double endy,double width,
				double holex, double holey, double holeradius, double zpos);


/**********************************/
void Pcb3D_GLCanvas::Redraw( void )
/**********************************/
{
    SetCurrent();

	InitGL();

    glMatrixMode(GL_MODELVIEW);    /* position viewer */
    /* transformations */
    GLfloat mat[4][4];

    build_rotmatrix( mat, g_Parm_3D_Visu.m_Quat );
    glMultMatrixf( &mat[0][0] );

	glTranslatef(g_Draw3d_dx, g_Draw3d_dy, 0.0F);

    glRotatef(g_Parm_3D_Visu.m_Rot[0], 1.0, 0.0, 0.0);
	glRotatef(g_Parm_3D_Visu.m_Rot[1], 0.0, 1.0, 0.0);
    glRotatef(g_Parm_3D_Visu.m_Rot[2], 0.0, 0.0, 1.0);

	if( m_gllist ) glCallList( m_gllist );
	else
	{
		m_gllist = CreateDrawGL_List();
//		m_gllist = DisplayCubeforTest();	// Only for test
	}

	glFlush();
	SwapBuffers();
 }

/**********************************************/
GLuint Pcb3D_GLCanvas::CreateDrawGL_List(void)
/**********************************************/
/* Creation de la liste des elements a afficher
*/
{
GLuint gllist = glGenLists( 1 );
WinEDA_BasePcbFrame * pcbframe = m_Parent->m_Parent;
BOARD * pcb = pcbframe->m_Pcb;
TRACK * pt_piste;
int ii;

	wxBusyCursor dummy;

	pcb->ComputeBoundaryBox();
	g_Parm_3D_Visu.m_BoardSettings = pcb->m_BoardSettings;
	g_Parm_3D_Visu.m_BoardSize = pcb->m_BoundaryBox.GetSize();
	g_Parm_3D_Visu.m_BoardPos = pcb->m_BoundaryBox.Centre();
	g_Parm_3D_Visu.m_BoardPos.y = - g_Parm_3D_Visu.m_BoardPos.y;
	g_Parm_3D_Visu.m_Layers = pcb->m_BoardSettings->m_CopperLayerCount;
	g_Parm_3D_Visu.m_BoardScale = 2.0 / MAX(g_Parm_3D_Visu.m_BoardSize.x, g_Parm_3D_Visu.m_BoardSize.y);
	float epoxy_width = 1.6;	// epoxy width in mm
	g_Parm_3D_Visu.m_Epoxy_Width = epoxy_width/2.54 * 1000
				* g_Parm_3D_Visu.m_BoardScale;

	/* calcul de l'altitude de chaque couche */
	for ( ii = 0; ii < 32; ii++ )
	{
		if ( ii < g_Parm_3D_Visu.m_Layers )
			g_Parm_3D_Visu.m_LayerZcoord[ii] = g_Parm_3D_Visu.m_Epoxy_Width * ii
				/ (g_Parm_3D_Visu.m_Layers-1);
		else g_Parm_3D_Visu.m_LayerZcoord[ii] = g_Parm_3D_Visu.m_Epoxy_Width;
	}
	GLfloat zpos_cu =  500 * g_Parm_3D_Visu.m_BoardScale;
	GLfloat zpos_cmp = g_Parm_3D_Visu.m_Epoxy_Width + zpos_cu;
	g_Parm_3D_Visu.m_LayerZcoord[ADHESIVE_N_CU] = -zpos_cu*2;
	g_Parm_3D_Visu.m_LayerZcoord[ADHESIVE_N_CMP] = zpos_cmp + zpos_cu;
	g_Parm_3D_Visu.m_LayerZcoord[SILKSCREEN_N_CU] = -zpos_cu;
	g_Parm_3D_Visu.m_LayerZcoord[SILKSCREEN_N_CMP] = zpos_cmp;
	g_Parm_3D_Visu.m_LayerZcoord[DRAW_N] = zpos_cmp + zpos_cu;
	g_Parm_3D_Visu.m_LayerZcoord[COMMENT_N] = zpos_cmp + zpos_cu;
	g_Parm_3D_Visu.m_LayerZcoord[ECO1_N] = zpos_cmp + zpos_cu;
	g_Parm_3D_Visu.m_LayerZcoord[ECO2_N] = zpos_cmp + zpos_cu;

	glNewList( gllist, GL_COMPILE_AND_EXECUTE );

    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

	/* draw axes*/
	glEnable(GL_COLOR_MATERIAL);
	SetGLColor(WHITE);
    glBegin(GL_LINES);
 	glNormal3f( 0.0, 0.0, 1.0);	// Normal is Z axis
	glVertex3f( 0.0 , 0.0, 0.0); glVertex3f(1.0, 0.0, 0.0);	// X axis
	glVertex3f( 0.0 , 0.0, 0.0); glVertex3f(0.0, -1.0, 0.0);	// y axis
 	glNormal3f( 1.0, 0.0, 0.0);	// Normal is Y axis
	glVertex3f( 0.0 , 0.0, 0.0); glVertex3f(0.0, 0.0, 0.3);	// z axis
    glEnd();

	/* Draw epoxy limits (do not use, works and test in progress)*/
#if 0
glEnable(GL_FOG);
	GLfloat param;
//	param = GL_LINEAR; glFogfv(GL_FOG_MODE,& param);
	param = 0.2; glFogfv(GL_FOG_DENSITY,& param);
	param = g_Parm_3D_Visu.m_LayerZcoord[15]; glFogfv(GL_FOG_END,& param);
	glBegin(GL_QUADS);
	SetGLColor(g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[CMP_N]);
	double sx = DataScale3D * g_Parm_3D_Visu.m_BoardSize.x / 2;
	double sy = DataScale3D * g_Parm_3D_Visu.m_BoardSize.y / 2;
	double zpos = g_Parm_3D_Visu.m_LayerZcoord[15];
	glNormal3f( 0.0, 0.0, 1.0);	// Normal is Z axis
	sx = sy = 0.5;
	glVertex3f( -sx, -sy , zpos);
	glVertex3f( -sx, sy , zpos);
	glVertex3f( sx, sy , zpos);
	glVertex3f( sx, -sy , zpos);
	glEnd();
	glBegin(GL_QUADS);
	SetGLColor(g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[CUIVRE_N]);
	glNormal3f( 0.0, 0.0, -1.0);	// Normal is -Z axis
	glVertex3f( -sx, -sy , 0);
	glVertex3f( -sx, sy , 0);
	glVertex3f( sx, sy , 0);
	glVertex3f( sx, -sy , 0);
	glEnd();
#endif

	/* Translation du trac� du BOARD pour placer son centre en 0, 0 */
    glTranslatef(-g_Parm_3D_Visu.m_BoardPos.x * g_Parm_3D_Visu.m_BoardScale,
			-g_Parm_3D_Visu.m_BoardPos.y * g_Parm_3D_Visu.m_BoardScale,
			0.0F);

 	glNormal3f( 0.0, 0.0, 1.0);	// Normal is Z axis
	/* Trac� des pistes : */
	for (pt_piste = pcb->m_Track ; pt_piste != NULL ; pt_piste = (TRACK*) pt_piste->Pnext )
		{
		if ( pt_piste->m_StructType == TYPEVIA )
			Draw3D_Via((SEGVIA*)pt_piste);
		else Draw3D_Track( pt_piste);
		}

	/* Trac� des edges */
EDA_BaseStruct * PtStruct;
	for ( PtStruct = pcb->m_Drawings; PtStruct != NULL; PtStruct = PtStruct->Pnext)
	{
		#define STRUCT ((DRAWSEGMENT *) PtStruct)
		if( PtStruct->m_StructType != TYPEDRAWSEGMENT ) continue;
		Draw3D_DrawSegment(STRUCT);
	}

	/* trac� des modules */
MODULE * Module = (MODULE*) pcb->m_Modules;
	for ( ; Module != NULL; Module = (MODULE *) Module->Pnext )
	{
		Module->Draw3D(this);
	}
    glEndList();

	/* Test for errors */
	GLenum err = glGetError();
	if ( err != GL_NO_ERROR )
		DisplayError(this, wxT("Error in GL commands") );
	return gllist;
}

/************************************************/
void Pcb3D_GLCanvas::Draw3D_Track(TRACK * track)
/************************************************/
{
double zpos;
int color = g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[track->m_Layer];
int layer = track->m_Layer;
double ox, oy, fx, fy;
double w;

	if ( color & ITEM_NOT_SHOW ) return;
	if ( track->m_Layer == CMP_N ) layer = g_Parm_3D_Visu.m_Layers -1;
	zpos = g_Parm_3D_Visu.m_LayerZcoord[layer];

	SetGLColor(color);
	glNormal3f( 0.0, 0.0, (layer == CUIVRE_N) ? -1.0 : 1.0);

	w = track->m_Width * g_Parm_3D_Visu.m_BoardScale;
	ox = track->m_Start.x * g_Parm_3D_Visu.m_BoardScale;
	oy = track->m_Start.y * g_Parm_3D_Visu.m_BoardScale;
	fx = track->m_End.x * g_Parm_3D_Visu.m_BoardScale;
	fy = track->m_End.y * g_Parm_3D_Visu.m_BoardScale;
	Draw3D_FilledSegment(ox, -oy, fx, -fy, w, zpos);
}

/********************************************/
void Pcb3D_GLCanvas::Draw3D_Via(SEGVIA * via)
/*********************************************/
/* 3D drawing for a VIA (cylinder + filled circles)
*/
{
double x, y, r, hole;
int layer, top_layer, bottom_layer;
double zpos, height;
int color;

	r = via->m_Width * g_Parm_3D_Visu.m_BoardScale / 2;
	hole = g_Parm_3D_Visu.m_BoardSettings->m_ViaDrill * g_Parm_3D_Visu.m_BoardScale / 2;
	x = via->m_Start.x  * g_Parm_3D_Visu.m_BoardScale;
	y = via->m_Start.y * g_Parm_3D_Visu.m_BoardScale;

	via->ReturnLayerPair(&top_layer, &bottom_layer);

	// Drawing filled circles:
	for ( layer = bottom_layer; layer < g_Parm_3D_Visu.m_Layers; layer++ )
	{
		zpos = g_Parm_3D_Visu.m_LayerZcoord[layer];
		if ( layer < g_Parm_3D_Visu.m_Layers-1 )
			color = g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[layer];
		else color = g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[CMP_N];
		if ( color & ITEM_NOT_SHOW ) continue;
		SetGLColor(color);
		glNormal3f( 0.0, 0.0, (layer == CUIVRE_N) ? -1.0 : 1.0);
		Draw3D_FilledCircle(x, -y, r, hole, zpos);
		if ( layer >= top_layer) break;

	}
	// Drawing hole:
	SetGLColor(DARKGRAY);
	height = g_Parm_3D_Visu.m_LayerZcoord[top_layer] - g_Parm_3D_Visu.m_LayerZcoord[bottom_layer];
	Draw3D_FilledCylinder(x, -y, hole, height, g_Parm_3D_Visu.m_LayerZcoord[bottom_layer]);
}


/*************************************************************/
void Pcb3D_GLCanvas::Draw3D_DrawSegment(DRAWSEGMENT * segment)
/*************************************************************/
{
int layer;
double x, y, xf, yf;
double zpos, w;
int color = g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[segment->m_Layer];

	if ( color & ITEM_NOT_SHOW ) return;

	SetGLColor(color);
	w = segment->m_Width * g_Parm_3D_Visu.m_BoardScale;
	x = segment->m_Start.x * g_Parm_3D_Visu.m_BoardScale;
	y = segment->m_Start.y * g_Parm_3D_Visu.m_BoardScale;
	xf = segment->m_End.x * g_Parm_3D_Visu.m_BoardScale;
	yf = segment->m_End.y  * g_Parm_3D_Visu.m_BoardScale;

	if ( segment->m_Layer == EDGE_N)
	{
		for ( layer = 0; layer < g_Parm_3D_Visu.m_Layers; layer++ )
		{
			glNormal3f( 0.0, 0.0, (layer == CUIVRE_N) ? -1.0 : 1.0);
			zpos = g_Parm_3D_Visu.m_LayerZcoord[layer];
			Draw3D_FilledSegment( x, -y, xf, -yf, w, zpos);
		}
	}

	else
	{
		zpos = g_Parm_3D_Visu.m_LayerZcoord[segment->m_Layer];
		Draw3D_FilledSegment( x, -y, xf, -yf, w, zpos);
	}
}



/*********************************************/
void MODULE::Draw3D(Pcb3D_GLCanvas * glcanvas)
/*********************************************/
{
D_PAD * pad = m_Pads;

#if 0
	if( ! DisplayOpt.Show_Modules_Cmp )
	{
		if(m_Layer == CMP_N) return;
	}
	if( ! DisplayOpt.Show_Modules_Cu )
	{
		if(m_Layer == CUIVRE_N) return;
	}
#endif

	/* Draw pads */
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
 	glNormal3f( 0.0, 0.0, 1.0);	// Normal is Z axis
	for( ; pad != NULL; pad = (D_PAD*) pad->Pnext )
	{
		pad->Draw3D(glcanvas);
	}

	/* Draw module shape: 3D shape if exists (or module edge if not exists)*/
Struct3D_Master * Struct3D = m_3D_Drawings;
bool As3dShape = FALSE;
	glPushMatrix();
	glTranslatef(m_Pos.x * g_Parm_3D_Visu.m_BoardScale,
			-m_Pos.y * g_Parm_3D_Visu.m_BoardScale,
			g_Parm_3D_Visu.m_LayerZcoord[m_Layer] );
	if ( m_Orient )
	{
		glRotatef( (double)m_Orient / 10, 0.0, 0.0, 1.0 );
	}
	if ( m_Layer == CUIVRE_N )
	{
		glRotatef( 180.0, 0.0, 1.0, 0.0 );
		glRotatef( 180.0, 0.0, 0.0, 1.0 );
	}
	DataScale3D = g_Parm_3D_Visu.m_BoardScale*UNITS3D_TO_UNITSPCB;

	for ( ; Struct3D != NULL; Struct3D = (Struct3D_Master *) Struct3D->Pnext )
	{
		if ( ! Struct3D->m_Shape3DName.IsEmpty() )
		{
			As3dShape = TRUE;
			Struct3D->ReadData();
		}
	}
	glPopMatrix();

	if ( ! As3dShape)
	{
	EDA_BaseStruct * Struct = m_Drawings;
		glNormal3f( 0.0, 0.0, 1.0);	// Normal is Z axis
		for( ;Struct != NULL; Struct = Struct->Pnext )
		{
			switch( Struct->m_StructType )
			{
				case TYPETEXTEMODULE:
					break;

				case TYPEEDGEMODULE:
					((EDGE_MODULE *) Struct)->Draw3D(glcanvas);
					break;

				default: break;
			}
		}
	}
}

/***************************************************/
void EDGE_MODULE::Draw3D(Pcb3D_GLCanvas * glcanvas)
/***************************************************/
{
int ux0, uy0, dx, dy,rayon, StAngle, EndAngle;
double scale, x, y, fx, fy, w, zpos ;
int color = g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[m_Layer];

	if ( color & ITEM_NOT_SHOW ) return;

	SetGLColor(color);
	glNormal3f( 0.0, 0.0, (m_Layer == CUIVRE_N) ? -1.0 : 1.0);
	scale = g_Parm_3D_Visu.m_BoardScale;

	ux0 = m_Start.x;
	uy0 = m_Start.y;
	dx = m_End.x;
	dy = m_End.y;
	zpos = g_Parm_3D_Visu.m_LayerZcoord[m_Layer];
	w = m_Width * g_Parm_3D_Visu.m_BoardScale;
	switch (m_Shape )
		{
		case S_SEGMENT:
			x = m_Start.x * g_Parm_3D_Visu.m_BoardScale;
			y = m_Start.y * g_Parm_3D_Visu.m_BoardScale;
			fx = dx * g_Parm_3D_Visu.m_BoardScale;
			fy = dy * g_Parm_3D_Visu.m_BoardScale;
			Draw3D_FilledSegment(x, -y, fx, -fy, w, zpos);
			break ;

		case S_CIRCLE:
			rayon = (int)hypot((double)(dx-ux0),(double)(dy-uy0) );
			/* TO DO */
			break;

		case S_ARC:
			rayon = (int)hypot((double)(dx-ux0),(double)(dy-uy0) );
			StAngle = (int)ArcTangente( dy-uy0, dx-ux0 );
			EndAngle = StAngle + m_Angle;
			/* TO DO */
			break;
		}
}


/***********************************************/
void D_PAD::Draw3D(Pcb3D_GLCanvas * glcanvas)
/***********************************************/
/* Dessin 3D des pads avec leur trou de percage
*/
{
int ii, ll, layer, nlmax;
int	ux0,uy0,
	dx,dx0,dy,dy0,
	delta_cx, delta_cy,
	xc, yc;
int angle, delta_angle;
int coord[4][2];
double fcoord[8][2], f_hole_coord[8][2];
float scale;
double zpos;
wxPoint shape_pos;
double x, y, r, w, hole, holeX, holeY;
double drillx, drilly;
bool Oncu, Oncmp, Both;
int color;

	scale = g_Parm_3D_Visu.m_BoardScale;
	holeX = (double)m_Drill.x * scale / 2;
	holeY = (double)m_Drill.y * scale / 2;
	hole = MIN (holeX,holeY);

	/* calcul du centre des formes des pads : */
    shape_pos = ReturnShapePos();
	ux0 = shape_pos.x;
	uy0 = shape_pos.y;
	xc = ux0;
	yc = uy0;

	/* le trace depend de la rotation de l'empreinte */
	dx = dx0 = m_Size.x >> 1 ;
	dy = dy0 = m_Size.y >> 1 ; /* demi dim  dx et dy */

	angle = m_Orient;
	drillx = m_Pos.x  * scale;
	drilly = m_Pos.y * scale;
	/* Draw the pad hole (TODO: draw OBLONG hole)*/
	if ( holeX && holeY )
	{
		SetGLColor(DARKGRAY);
		Draw3D_FilledCylinder(drillx, -drilly, hole, g_Parm_3D_Visu.m_LayerZcoord[CMP_N], 0.0);
	}

 	glNormal3f( 0.0, 0.0, 1.0);	// Normal is Z axis
	nlmax = g_Parm_3D_Visu.m_Layers-1;
	Oncu = (m_Masque_Layer & CUIVRE_LAYER) ? TRUE : FALSE;
	Oncmp = (m_Masque_Layer & CMP_LAYER) ? TRUE : FALSE;
	Both = Oncu && Oncmp;
	switch (m_PadShape & 0x7F)
		{
		case CIRCLE :
			x = xc * scale;
			y = yc * scale;
			r = (double)dx * scale;
			for ( layer = CUIVRE_N; layer <= CMP_N; layer ++)
			{
				if (layer && (layer == nlmax) ) layer = CMP_N;
				if ( (layer == CMP_N) && ! Oncmp ) continue;
				if ( (layer == CUIVRE_N) && ! Oncu ) continue;
				if ( (layer > CUIVRE_N) && (layer < CMP_N) && !Both) continue;
				color = g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[layer];
				if ( color & ITEM_NOT_SHOW ) continue;
				SetGLColor(color);
				glNormal3f( 0.0, 0.0, (layer == CUIVRE_N) ? -1.0 : 1.0);
				zpos = g_Parm_3D_Visu.m_LayerZcoord[layer];
				Draw3D_FilledCircle(x, -y, r, hole, zpos);
			}
			break;

		case OVALE :
			/* calcul de l'entraxe de l'ellipse */
			if( dx > dy )	/* ellipse horizontale */
				{
				delta_cx = dx - dy; delta_cy = 0;
				w = m_Size.y * scale;
				delta_angle = angle+900;
				}
			else			/* ellipse verticale */
				{
				delta_cx = 0; delta_cy = dy - dx;
				w = m_Size.x * scale;
				delta_angle = angle;
				}
			RotatePoint(&delta_cx, &delta_cy, angle);
			{
			double ox, oy, fx, fy;
			ox = (double)(ux0 + delta_cx) * scale;
			oy = (double)(uy0 + delta_cy) * scale;
			fx = (double)(ux0 - delta_cx) * scale;
			fy = (double)(uy0 - delta_cy) * scale;
			for ( layer = CUIVRE_N; layer <= CMP_N; layer ++)
			{
				if (layer && (layer == nlmax) ) layer = CMP_N;
				if ( (layer == CMP_N) && ! Oncmp ) continue;
				if ( (layer == CUIVRE_N) && ! Oncu ) continue;
				if ( (layer > CUIVRE_N) && (layer < CMP_N) && !Both) continue;
				color = g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[layer];
				glNormal3f( 0.0, 0.0, (layer == CUIVRE_N) ? -1.0 : 1.0);
				if ( color & ITEM_NOT_SHOW ) continue;
				SetGLColor(color);
				zpos = g_Parm_3D_Visu.m_LayerZcoord[layer];
				Draw3D_FilledSegmentWithHole(ox, -oy, fx, -fy, w, drillx, -drilly, hole, zpos);
			}
			}
			break;

		case RECT :
		case SPECIAL_PAD:
		case TRAPEZE:
			{
			int ddx, ddy ;
			ddx = m_DeltaSize.x >> 1 ;
			ddy = m_DeltaSize.y >> 1 ; /* demi dim  dx et dy */

			 coord[0][0] = - dx - ddy;
			 coord[0][1] = + dy + ddx;

			 coord[1][0] = - dx + ddy;
			 coord[1][1] = - dy - ddx;

			 coord[2][0] = + dx - ddy;
			 coord[2][1] = - dy + ddx;

			 coord[3][0] = + dx + ddy;
			 coord[3][1] = + dy - ddx;

			for (ii = 0; ii < 4; ii++)
				{
				RotatePoint(&coord[ii][0],&coord[ii][1], angle);
				coord[ii][0] += ux0;
				coord[ii][1] += uy0;
				ll = ii*2;
				fcoord[ll][0] = coord[ii][0] * scale;
				fcoord[ll][1] = coord[ii][1] * scale;
				}
			for (ii = 0; ii < 7; ii+=2)
				{
				ll = ii+2; if (ll > 7) ll -= 8;
				fcoord[ii+1][0] = (fcoord[ii][0] + fcoord[ll][0])/2;
				fcoord[ii+1][1] = (fcoord[ii][1] + fcoord[ll][1])/2;
				}
			for (ii = 0; ii < 8; ii++)
				{
				f_hole_coord[ii][0] = -hole*0.707;
				f_hole_coord[ii][1] = hole*0.707;
				RotatePoint(&f_hole_coord[ii][0], &f_hole_coord[ii][1],
					angle -(ii * 450) );
				f_hole_coord[ii][0] += drillx;
				f_hole_coord[ii][1] += drilly;
				}

			for ( layer = CUIVRE_N; layer <= CMP_N; layer ++)
			{
				if (layer && (layer == nlmax) ) layer = CMP_N;
				if ( (layer == CMP_N) && ! Oncmp ) continue;
				if ( (layer == CUIVRE_N) && ! Oncu ) continue;
				if ( (layer > CUIVRE_N) && (layer < CMP_N) && !Both) continue;
				color = g_Parm_3D_Visu.m_BoardSettings->m_LayerColor[layer];
				glNormal3f( 0.0, 0.0, (layer == CUIVRE_N) ? -1.0 : 1.0);
				if ( color & ITEM_NOT_SHOW ) continue;
				SetGLColor(color);
				zpos = g_Parm_3D_Visu.m_LayerZcoord[layer];
				glBegin(GL_QUAD_STRIP);
				for ( ii = 0; ii < 8; ii++ )
				{
					glVertex3f( f_hole_coord[ii][0], -f_hole_coord[ii][1] , zpos);
					glVertex3f( fcoord[ii][0], -fcoord[ii][1] , zpos);
				}
				glVertex3f( f_hole_coord[0][0], -f_hole_coord[0][1] , zpos);
				glVertex3f( fcoord[0][0], -fcoord[0][1] , zpos);
				glEnd();
			}
			break;

			default:
				break;
			}
		}

}

/*************************/
void SetGLColor(int color)
/*************************/
{
double red, green,blue;
StructColors colordata = ColorRefs[color &  MASKCOLOR];

	red = colordata.m_Red / 255.0;
	blue =  colordata.m_Blue / 255.0;
	green =  colordata.m_Green / 255.0;
	glColor3f(red, green,blue);
}

/********************************************************/
static void Draw3D_FilledCircle(double posx, double posy,
				double rayon, double hole, double zpos)
/********************************************************/
{
int ii, slice = 16;
double x, y;

	glBegin(GL_QUAD_STRIP);
	for ( ii = 0; ii <= slice; ii++ )
	{
		x = hole; y = 0.0;
		RotatePoint(&x, &y, ii * 225);
		glVertex3f( x + posx, y + posy, zpos);
		x = rayon; y = 0.0;
		RotatePoint(&x, &y, ii * 225);
		glVertex3f( x + posx, y + posy, zpos);
	}
	glEnd();
}

/*********************************************************/
static void Draw3D_FilledCylinder(double posx, double posy,
				double rayon, double height, double zpos)
/*********************************************************/
{
int ii;
double x, y;
#define NB_SEGM 12
S3D_Vertex coords[4];
double tmp = DataScale3D;

	DataScale3D = 1.0;	//les coord sont deja a l'echelle pour Set_Object_Data();
	coords[0].x = coords[1].x = posx + rayon;
	coords[0].y = coords[1].y = posy;
	coords[0].z = coords[3].z = zpos;
	coords[1].z = coords[2].z = zpos + height;

	for ( ii = 0; ii <= NB_SEGM; ii++ )
	{
		x = rayon; y = 0.0;
		RotatePoint(&x, &y, ii * (3600/NB_SEGM));
		coords[2].x = coords[3].x = posx + x;
		coords[2].y = coords[3].y = posy + y;
		Set_Object_Data(coords, 4 );
		coords[0].x = coords[2].x;
		coords[0].y = coords[2].y;
		coords[1].x = coords[3].x;
		coords[1].y = coords[3].y;
	}
 	glNormal3f( 0.0, 0.0, 1.0);	// Normal is Z axis
	DataScale3D = tmp;
}

/*****************************************************************/
static void Draw3D_FilledSegment(double startx, double starty,
				double endx, double endy,double width, double zpos)
/*****************************************************************/
/* trace un polygone semblable a un segment a bouts ronds
*/
{
double dx, dy, x, y, firstx=0, firsty=0;
int ii, angle;

	/* on va calculer les coordonn�es du segment suppos� horizontal,
	puis tourner les  cordonnes de l'angle voulu */

	dx = endx - startx; dy = endy - starty;
	angle = (int)(( atan2( dy, dx ) / M_PI * 1800)+0.5) ;

	RotatePoint(&dx, &dy, angle);	// apres rotation: dx = longueur du segment
									// dy = 0;
	width /= 2;

	glBegin(GL_POLYGON);
	// trac� de l'arrondi a droite (1er demi polygone a la fin du segment)
	for ( ii = 0; ii <= 8; ii++ )
	{
		x = 0.0; y =-width;
		RotatePoint(&x, &y, -ii * 225);
		x += dx;
		RotatePoint(&x, &y, -angle);
		glVertex3f( startx + x, starty+y, zpos);
		if ( ii == 0 )
		{
			firstx = startx + x;
			firsty = starty + y;
		}
	}
	// trac� de l'arrondi a gauche (2ieme demi polygone a l'origine du segment)
	for ( ii = 0; ii <= 8; ii++ )
	{
		int jj = ii * 225;
		x = 0.0; y = width;
		RotatePoint(&x, &y, -angle -jj);
		glVertex3f( startx + x, starty+y, zpos);
	}

	glVertex3f( firstx, firsty, zpos);
	glEnd();
}

/*****************************************************************/
static void Draw3D_FilledSegmentWithHole(double startx, double starty,
				double endx, double endy,double width,
				double holex, double holey, double holeradius, double zpos)
/*****************************************************************/
/* trace un polygone semblable a un segment a bouts ronds avec trou
*/
{
double x, y, xin, yin;
double firstx=0, firsty=0, firstxin=0, firstyin=0;
int ii, angle, theta;

	/* on va calculer les coordonn�es du segment suppos� horizontal,
	puis tourner les  cordonnes de l'angle voulu
	Tous des calculs se font avec startx, starty comme origine du trac� */

	endx -= startx; endy -= starty;
	holex -= startx; holey -= starty;
	angle = (int)(( atan2( endy, endx ) / M_PI * 1800)+0.5) ;

	RotatePoint(&endx, &endy, angle);	// apres rotation: endx = longueur du segment
									// endy = 0;
	RotatePoint(&holex, &holey, angle);
	width /= 2;

	glBegin(GL_QUAD_STRIP);
	// trac� de l'arrondi a droite (1er demi polygone a la fin du segment)
	// autour du demi-trou de percage
	for ( ii = 0; ii <= 8; ii++ )
	{
		x = 0.0; y = -width;
		xin = 0.0; yin = - holeradius;
		theta = -ii * 225;
		RotatePoint(&x, &y, theta);
		RotatePoint(&xin, &yin, theta);
		x += endx;
		RotatePoint(&x, &y, -angle);
		xin += holex;
		RotatePoint(&xin, &yin, -angle);
		glVertex3f( startx + xin, starty+yin, zpos);
		glVertex3f( startx + x, starty+y, zpos);
		if ( ii == 0 )	// Memorisation du point de d�part du trac�
		{
			firstx = startx + x;
			firsty = starty + y;
			firstxin = startx + xin;
			firstyin = starty + yin;
		}
	}
	// trac� de l'arrondi a gauche (2ieme demi polygone a l'origine du segment)
	for ( ii = 0; ii <= 8; ii++ )
	{
		theta = - ii * 225;
		x = 0.0; y = width;
		RotatePoint(&x, &y, -angle + theta);
		xin = 0.0; yin = holeradius;
		RotatePoint(&xin, &yin, theta);
		xin += holex;
		RotatePoint(&xin, &yin, -angle);
		glVertex3f( startx + xin,starty + yin, zpos);
		glVertex3f( startx + x, starty + y, zpos);
	}

	glVertex3f( firstxin, firstyin, zpos);
	glVertex3f( firstx, firsty, zpos);
	glEnd();
}

