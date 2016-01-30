	/********************************************************************/
	/*			EESchema - symbdraw.cpp									*/
	/* Menus et Routines de dessin des elements graphiques des symboles */
	/********************************************************************/


#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
#include "trigo.h"

#include "protos.h"

#include "id.h"

/* Routines importees */

/* Routines locales */
static void SymbolDisplayDraw(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);
static void ComputeArc(LibDrawArc * DrawItem, wxPoint ArcCentre);
static void RedrawWhileMovingCursor(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);
static void MoveLibDrawItemAt(LibEDA_BaseStruct * DrawItem, wxPoint newpos);

/* Variables locales */
static int StateDrawArc, ArcStartX, ArcStartY, ArcEndX, ArcEndY;
static wxPoint InitPosition, StartCursor, ItemPreviousPos;
static int FlSymbol_Fill = NO_FILL;


	/************************************/
	/* class WinEDA_PartPropertiesFrame */
	/************************************/
#include "dialog_cmp_graphic_properties.cpp"


/************************************************************/
void WinEDA_bodygraphics_PropertiesFrame::
        bodygraphics_PropertiesAccept(wxCommandEvent& event)
/************************************************************/
/* Update the current draw item
*/
{
	g_FlDrawSpecificConvert = m_CommonConvert->GetValue() ? FALSE : TRUE;
	g_FlDrawSpecificUnit = m_CommonUnit->GetValue() ? FALSE : TRUE;
	if ( m_Filled )
		FlSymbol_Fill = m_Filled->GetSelection();

	if ( CurrentDrawItem )
	{
		if ( ! (CurrentDrawItem->m_Flags & IS_NEW) )	// if IS_NEW, copy for undo is done before place
			m_Parent->SaveCopyInUndoList();
		wxClientDC dc(m_Parent->DrawPanel);
		m_Parent->DrawPanel->PrepareGraphicContext(&dc);

		DrawLibraryDrawStruct(m_Parent->DrawPanel, &dc, CurrentLibEntry, 0 , 0,
						CurrentDrawItem, CurrentUnit, g_XorMode);

		if( g_FlDrawSpecificUnit ) CurrentDrawItem->m_Unit = CurrentUnit;
		else CurrentDrawItem->m_Unit = 0;
		if( g_FlDrawSpecificConvert ) CurrentDrawItem->m_Convert = CurrentConvert;
		else CurrentDrawItem->m_Convert = 0;
        if ( m_Filled  )
		{
            switch(CurrentDrawItem->m_StructType)
			{
                case COMPONENT_ARC_DRAW_TYPE:
                    ((LibDrawArc*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
                    break;
        
                case COMPONENT_CIRCLE_DRAW_TYPE:
                    ((LibDrawCircle*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
                    break;
        
                case COMPONENT_RECT_DRAW_TYPE:
                    ((LibDrawSquare*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
                    break;
        
                case  COMPONENT_POLYLINE_DRAW_TYPE:
                    ((LibDrawPolyline*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
                    break;
        
                default: break;
			}
		}
		CurrentLibEntry->SortDrawItems();

		m_Parent->m_CurrentScreen->SetModify();

		DrawLibraryDrawStruct(m_Parent->DrawPanel, &dc, CurrentLibEntry, 0 , 0,
						CurrentDrawItem, CurrentUnit, g_XorMode);
	}

	Close();

	if ( CurrentDrawItem )
		CurrentDrawItem->Display_Infos_DrawEntry(m_Parent);
	m_Parent->ReDrawPanel();
}

/**********************************************************/
void WinEDA_LibeditFrame::EditGraphicSymbol(wxDC * DC,
			LibEDA_BaseStruct * DrawItem)
/**********************************************************/
/* Install the dialog box for editing a graphical item properties
*/
{
	if ( DrawItem == NULL ) return;
	
	WinEDA_bodygraphics_PropertiesFrame * frame = new
				WinEDA_bodygraphics_PropertiesFrame(this);
	frame->ShowModal(); frame->Destroy();
}


/****************************************************************/
static void AbortSymbolTraceOn(WinEDA_DrawFrame * frame, wxDC * DC)
/****************************************************************/
{

	StateDrawArc = 0;
	frame->m_CurrentScreen->ManageCurseur = NULL;
	frame->m_CurrentScreen->ForceCloseManageCurseur = NULL;
	if(CurrentDrawItem == NULL) return;

	if(CurrentDrawItem->m_Flags & IS_NEW)
	{
		if ( CurrentDrawItem->m_StructType == COMPONENT_ARC_DRAW_TYPE )
		{
			frame->RedrawActiveWindow( DC,TRUE);
		}
		else DrawLibraryDrawStruct(frame->DrawPanel, DC, CurrentLibEntry, 0 , 0,
						CurrentDrawItem, CurrentUnit, g_XorMode);
		delete CurrentDrawItem;
	}

	else
	{
		wxPoint curpos;
		curpos = frame->m_CurrentScreen->m_Curseur;
		frame->m_CurrentScreen->m_Curseur = StartCursor;
		RedrawWhileMovingCursor(frame->DrawPanel, DC, TRUE);
		frame->m_CurrentScreen->m_Curseur = curpos;
		DrawLibraryDrawStruct(frame->DrawPanel, DC, CurrentLibEntry, 0 , 0,
						CurrentDrawItem, CurrentUnit, GR_DEFAULT_DRAWMODE);

		CurrentDrawItem->m_Flags = 0;
	}

	CurrentDrawItem = NULL;
}



/*********************************************************************/
LibEDA_BaseStruct * WinEDA_LibeditFrame::CreateGraphicItem(wxDC * DC)
/*********************************************************************/
/* Routine de creation d'un nouvel element type LibraryDrawStruct
	POLYLINE
	ARC
	CIRCLE
	RECTANGLE
*/
{
int DrawType;
int * ptpoly;

	if(CurrentDrawItem)	return NULL;

	DrawPanel->m_IgnoreMouseEvents = TRUE;
	// Creation du nouvel element
	switch ( m_ID_current_state )
		{
		case ID_LIBEDIT_BODY_LINE_BUTT:
			DrawType = COMPONENT_POLYLINE_DRAW_TYPE;
			break;

		case ID_LIBEDIT_BODY_ARC_BUTT:
			DrawType = COMPONENT_ARC_DRAW_TYPE;
			break;

		case ID_LIBEDIT_BODY_CIRCLE_BUTT:
			DrawType = COMPONENT_CIRCLE_DRAW_TYPE;
			break;

		case ID_LIBEDIT_BODY_RECT_BUTT:
			DrawType = COMPONENT_RECT_DRAW_TYPE;
			break;

		case ID_LIBEDIT_BODY_TEXT_BUTT:
			DrawType = COMPONENT_GRAPHIC_TEXT_DRAW_TYPE;
			break;

		default:
			DisplayError(this, wxT("SymbolBeginDrawItem Internal err: Id error"));
			return NULL;
		}

	m_CurrentScreen->ManageCurseur = SymbolDisplayDraw;
	m_CurrentScreen->ForceCloseManageCurseur = AbortSymbolTraceOn;

	switch ( DrawType )
	{
		case COMPONENT_ARC_DRAW_TYPE:
		{
			LibDrawArc * Arc = new LibDrawArc();
			CurrentDrawItem = Arc;
			ArcStartX = ArcEndX = m_CurrentScreen->m_Curseur.x;
			ArcStartY = ArcEndY = - m_CurrentScreen->m_Curseur.y;
			StateDrawArc = 1;
			Arc->m_Fill = FlSymbol_Fill;
		}
			break;

		case COMPONENT_CIRCLE_DRAW_TYPE:
		{
			LibDrawCircle * Circle = new LibDrawCircle();
			CurrentDrawItem = Circle;
			Circle->m_Pos.x = m_CurrentScreen->m_Curseur.x;
			Circle->m_Pos.y = - m_CurrentScreen->m_Curseur.y;
			Circle->m_Fill = FlSymbol_Fill;
		}
			break;

		case COMPONENT_RECT_DRAW_TYPE:
		{
			LibDrawSquare * Square = new LibDrawSquare();
			CurrentDrawItem = Square;
			Square->m_Start.x = m_CurrentScreen->m_Curseur.x;
			Square->m_Start.y = - m_CurrentScreen->m_Curseur.y;
			Square->m_End = Square->m_Start;
			Square->m_Fill = FlSymbol_Fill;
		}
			break;

		case COMPONENT_POLYLINE_DRAW_TYPE:
		{
			LibDrawPolyline* polyline = new LibDrawPolyline();
			CurrentDrawItem = polyline;
			polyline->n = 2;
			ptpoly = (int*)MyZMalloc( 4 * sizeof(int));
			polyline->PolyList = ptpoly;
			ptpoly[0] = ptpoly[2] = m_CurrentScreen->m_Curseur.x;
			ptpoly[1] = ptpoly[3] = - m_CurrentScreen->m_Curseur.y;
			polyline->m_Fill = FlSymbol_Fill;
		}
			break;

		case COMPONENT_LINE_DRAW_TYPE:
		{
			LibDrawSegment* Segment = new LibDrawSegment();
			CurrentDrawItem = Segment;
			Segment->m_Start.x = m_CurrentScreen->m_Curseur.x;
			Segment->m_Start.y = -m_CurrentScreen->m_Curseur.y;
			Segment->m_End = Segment->m_Start;
		}
			break;

		case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:
		{
			LibDrawText* Text = new LibDrawText();
			CurrentDrawItem = Text;
			Text->m_Size.x = Text->m_Size.y = g_LastTextSize;
			Text->m_Horiz = g_LastTextOrient;
			Text->m_Pos.x = m_CurrentScreen->m_Curseur.x;
			Text->m_Pos.y = - m_CurrentScreen->m_Curseur.y;
			EditSymbolText(NULL, Text);
			if ( Text->m_Text.IsEmpty() )
			{
				delete Text;
				CurrentDrawItem = NULL;
				m_CurrentScreen->ManageCurseur = NULL;
				m_CurrentScreen->ForceCloseManageCurseur = NULL;
			}
			else
			{
				StartMoveDrawSymbol(DC);
				DrawLibraryDrawStruct(DrawPanel, DC, CurrentLibEntry, 0 , 0,
					Text, CurrentUnit, g_XorMode);
			}
		}
			break;
	}

	if ( CurrentDrawItem )
	{
		CurrentDrawItem->m_Flags |= IS_NEW;
		if(g_FlDrawSpecificUnit) CurrentDrawItem->m_Unit = CurrentUnit;
		if(g_FlDrawSpecificConvert) CurrentDrawItem->m_Convert = CurrentConvert;
	}

	DrawPanel->MouseToCursorSchema();
	DrawPanel->m_IgnoreMouseEvents = FALSE;

	return CurrentDrawItem;
}


/********************************************************/
void WinEDA_LibeditFrame::GraphicItemBeginDraw(wxDC * DC)
/********************************************************/
/* Routine de creation d'un nouvel element type LibraryDrawStruct
*/
{
	if(CurrentDrawItem == NULL)	return;

	switch ( CurrentDrawItem->m_StructType )
		{
		case COMPONENT_ARC_DRAW_TYPE:
			if(StateDrawArc == 1)
				{
				SymbolDisplayDraw(DrawPanel,DC, FALSE);
				StateDrawArc = 2;
				SymbolDisplayDraw(DrawPanel,DC, FALSE);
				break;
				}
			if(StateDrawArc > 1)
				{
				EndDrawGraphicItem(DC);
				return;
				}
			break;

		case COMPONENT_CIRCLE_DRAW_TYPE:
		case COMPONENT_RECT_DRAW_TYPE:
		case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:
			EndDrawGraphicItem(DC);
			return;

		case COMPONENT_POLYLINE_DRAW_TYPE:
			{
			wxPoint pos = m_CurrentScreen->m_Curseur;
			((LibDrawPolyline*)CurrentDrawItem)->AddPoint(pos);
			}
			break;

		case COMPONENT_LINE_DRAW_TYPE:
			break;
		}

}

/**************************************************************************/
static void RedrawWhileMovingCursor(WinEDA_DrawPanel * panel, wxDC * DC, bool erase)
/**************************************************************************/
/* Redessine le "Draw Symbol" en cours de deplacement
*/
{
BASE_SCREEN * Screen = panel->m_Parent->m_CurrentScreen;
int mx, my;
	
	/* Effacement ancien dessin */
	if( erase )
	{
		mx = ItemPreviousPos.x - StartCursor.x ,
		my = ItemPreviousPos.y - StartCursor.y ;
		
		DrawLibraryDrawStruct(panel, DC, CurrentLibEntry, mx, my,
				CurrentDrawItem, CurrentUnit, g_XorMode);
	}

	/* Redraw moved item */
	mx = Screen->m_Curseur.x - StartCursor.x ,
	my = Screen->m_Curseur.y - StartCursor.y ;
	DrawLibraryDrawStruct(panel, DC, CurrentLibEntry, mx , my,
				CurrentDrawItem, CurrentUnit, g_XorMode);
	ItemPreviousPos = Screen->m_Curseur;
}



/*****************************************************************/
void MoveLibDrawItemAt(LibEDA_BaseStruct * DrawItem, wxPoint newpos)
/*****************************************************************/
{
int mx = newpos.x, my = newpos.y;
wxSize size;
	
	switch ( DrawItem->m_StructType )
	{
		case COMPONENT_ARC_DRAW_TYPE:
		{
			int dx = mx - ((LibDrawArc*)CurrentDrawItem)->m_Pos.x;
			int dy = - my - ((LibDrawArc*)CurrentDrawItem)->m_Pos.y;
			((LibDrawArc*)CurrentDrawItem)->m_Pos.x = mx;
			((LibDrawArc*)CurrentDrawItem)->m_Pos.y = - my;
			((LibDrawArc*)CurrentDrawItem)->m_Start.x += dx;
			((LibDrawArc*)CurrentDrawItem)->m_Start.y += dy;
			((LibDrawArc*)CurrentDrawItem)->m_End.x += dx;
			((LibDrawArc*)CurrentDrawItem)->m_End.y += dy;
			break;
		}

		case COMPONENT_CIRCLE_DRAW_TYPE:
			((LibDrawCircle*)CurrentDrawItem)->m_Pos.x = mx;
			((LibDrawCircle*)CurrentDrawItem)->m_Pos.y = - my;
			break;

		case COMPONENT_RECT_DRAW_TYPE:
			size.x = ((LibDrawSquare*)CurrentDrawItem)->m_End.x -
					 ((LibDrawSquare*)CurrentDrawItem)->m_Start.x;
			size.y = ((LibDrawSquare*)CurrentDrawItem)->m_End.y -
						((LibDrawSquare*)CurrentDrawItem)->m_Start.y;
			((LibDrawSquare*)CurrentDrawItem)->m_Start.x = mx;
			((LibDrawSquare*)CurrentDrawItem)->m_Start.y = - my;
			((LibDrawSquare*)CurrentDrawItem)->m_End.x = mx + size.x;
			((LibDrawSquare*)CurrentDrawItem)->m_End.y = - my + size.y;
			break;

		case COMPONENT_POLYLINE_DRAW_TYPE:
		{
			int ii , imax = ((LibDrawPolyline*)CurrentDrawItem)->n * 2;
			int * ptpoly = ((LibDrawPolyline*)CurrentDrawItem)->PolyList;
			int dx = mx - ptpoly[0];
			int dy = - my - ptpoly[1];
			for ( ii = 0; ii < imax; ii += 2)
			{
				ptpoly[ii] += dx;
				ptpoly[ii + 1] += dy;
			}
		}
			break;

		case COMPONENT_LINE_DRAW_TYPE:
			break;

		case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:
			((LibDrawText*)CurrentDrawItem)->m_Pos.x = mx;
			((LibDrawText*)CurrentDrawItem)->m_Pos.y = - my;
			break;
	}
}

/************************************************************/
void WinEDA_LibeditFrame::StartMoveDrawSymbol(wxDC * DC)
/************************************************************/
{
	if(CurrentDrawItem == NULL) return;

	SetCursor(wxCURSOR_HAND);

	CurrentDrawItem->m_Flags |= IS_MOVED;
	StartCursor = m_CurrentScreen->m_Curseur;

	switch ( CurrentDrawItem->m_StructType )
		{
		case COMPONENT_ARC_DRAW_TYPE:
			InitPosition = ((LibDrawArc*)CurrentDrawItem)->m_Pos;
			break;

		case COMPONENT_CIRCLE_DRAW_TYPE:
			InitPosition = ((LibDrawCircle*)CurrentDrawItem)->m_Pos;
			break;

		case COMPONENT_RECT_DRAW_TYPE:
			InitPosition = ((LibDrawSquare*)CurrentDrawItem)->m_Start;
			break;

		case COMPONENT_POLYLINE_DRAW_TYPE:
			InitPosition.x = * ((LibDrawPolyline*)CurrentDrawItem)->PolyList;
			InitPosition.y = * (((LibDrawPolyline*)CurrentDrawItem)->PolyList + 1);
			break;

		case COMPONENT_LINE_DRAW_TYPE:
			break;

		case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:
			InitPosition = ((LibDrawText*)CurrentDrawItem)->m_Pos;
			break;
		}

	ItemPreviousPos = m_CurrentScreen->m_Curseur;
	m_CurrentScreen->ManageCurseur = RedrawWhileMovingCursor;
	m_CurrentScreen->ForceCloseManageCurseur = AbortSymbolTraceOn;
	m_CurrentScreen->ManageCurseur(DrawPanel, DC, TRUE);
}



/****************************************************************/
/* Routine de Gestion des evenements souris lors de la creation */
/* d'un nouvel element type LibraryDrawStruct					*/
/****************************************************************/
static void SymbolDisplayDraw(WinEDA_DrawPanel * panel, wxDC * DC, bool erase)
{
int DrawMode = g_XorMode;
int * ptpoly;
int dx, dy;
BASE_SCREEN * Screen = panel->m_Parent->m_CurrentScreen;
int mx = Screen->m_Curseur.x,
	my = Screen->m_Curseur.y;

	GRSetDrawMode(DC, DrawMode);

	if( erase )
		{ 
		if( StateDrawArc == 1 )
			{
			int Color = ReturnLayerColor(LAYER_DEVICE);
			GRLine(&panel->m_ClipBox, DC, ArcStartX, - ArcStartY, ArcEndX, - ArcEndY, Color);
			}
		else
			{
			DrawLibraryDrawStruct(panel, DC, CurrentLibEntry, 0 , 0,
						CurrentDrawItem, CurrentUnit, DrawMode);
			if(CurrentDrawItem->m_StructType == COMPONENT_ARC_DRAW_TYPE)
				{
				int Color = ReturnLayerColor(LAYER_DEVICE);
				GRDashedLine(&panel->m_ClipBox, DC, ArcStartX, - ArcStartY,
						((LibDrawArc*)CurrentDrawItem)->m_Pos.x,
						- ((LibDrawArc*)CurrentDrawItem)->m_Pos.y,
						Color);
				GRDashedLine(&panel->m_ClipBox, DC, ArcEndX, - ArcEndY,
						((LibDrawArc*)CurrentDrawItem)->m_Pos.x,
						- ((LibDrawArc*)CurrentDrawItem)->m_Pos.y,
						Color);
				}
			}
		}

	switch ( CurrentDrawItem->m_StructType )
		{
		case COMPONENT_ARC_DRAW_TYPE:
			if( StateDrawArc == 1)
				{
				ArcEndX = mx; ArcEndY = - my;
				}

			if( StateDrawArc == 2)
				{
				ComputeArc((LibDrawArc*)CurrentDrawItem, Screen->m_Curseur);
				}
			((LibDrawArc*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
			break;

		case COMPONENT_CIRCLE_DRAW_TYPE:
			dx = ((LibDrawCircle*)CurrentDrawItem)->m_Pos.x - mx;
			dy = ((LibDrawCircle*)CurrentDrawItem)->m_Pos.y + my;
			((LibDrawCircle*)CurrentDrawItem)->m_Rayon = (int)sqrt( (dx*dx) + (dy*dy) );
			((LibDrawCircle*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
			break;

		case COMPONENT_RECT_DRAW_TYPE:
			((LibDrawSquare*)CurrentDrawItem)->m_End.x = mx;
			((LibDrawSquare*)CurrentDrawItem)->m_End.y = - my;
			((LibDrawSquare*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
			break;

		case COMPONENT_POLYLINE_DRAW_TYPE:
			ptpoly = ((LibDrawPolyline*)CurrentDrawItem)->PolyList;
			ptpoly += 2 * (((LibDrawPolyline*)CurrentDrawItem)->n - 1);
			ptpoly[0] = mx;
			ptpoly[1] = - my;
			((LibDrawPolyline*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
			break;

		case COMPONENT_LINE_DRAW_TYPE:
			((LibDrawSegment*)CurrentDrawItem)->m_End.x = mx;
			((LibDrawSegment*)CurrentDrawItem)->m_End.y = - my;
			break;

		case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:	/* Traite par des routines specifiques */
			break;
		}

	if( StateDrawArc == 1 )
		{
		int Color = ReturnLayerColor(LAYER_DEVICE);
		GRLine(&panel->m_ClipBox, DC, ArcStartX, - ArcStartY, ArcEndX, - ArcEndY, Color);
		}
	else
		{
		DrawLibraryDrawStruct(panel, DC, CurrentLibEntry, 0 , 0,
						CurrentDrawItem, CurrentUnit, DrawMode);
		if(CurrentDrawItem->m_StructType == COMPONENT_ARC_DRAW_TYPE)
			{
			int Color = ReturnLayerColor(LAYER_DEVICE);
			GRDashedLine(&panel->m_ClipBox, DC, ArcStartX, - ArcStartY,
					((LibDrawArc*)CurrentDrawItem)->m_Pos.x,
					- ((LibDrawArc*)CurrentDrawItem)->m_Pos.y,
					Color);
			GRDashedLine(&panel->m_ClipBox, DC, ArcEndX, - ArcEndY,
					((LibDrawArc*)CurrentDrawItem)->m_Pos.x,
					- ((LibDrawArc*)CurrentDrawItem)->m_Pos.y,
					Color);
			}
		}
}


/******************************************************/
void WinEDA_LibeditFrame::EndDrawGraphicItem(wxDC * DC)
/******************************************************/
/* Place la structure courante en liste des structures du composant
	courant, si elle existe et redessine toujours celle ci
	Parametres: (tous globaux)
		CurrentDrawItem
		CurrentLibEntry
*/
{
	if(CurrentLibEntry == NULL) return;
	if(CurrentDrawItem == NULL) return;

	if( CurrentDrawItem->m_StructType == COMPONENT_ARC_DRAW_TYPE )
	{
		if (StateDrawArc == 1 )	/* Trace d'arc en cours: doit etre termine */
		{
			DisplayError(this, wxT("Arc in progress.."), 10 ); return;
		}
		else
		{
			if ( (CurrentDrawItem->m_Flags & IS_MOVED) == 0 )
				SymbolDisplayDraw(DrawPanel, DC, FALSE);
		}
	}

	StateDrawArc = 0;

	if ( CurrentDrawItem->m_Flags & IS_NEW )
	{
		SaveCopyInUndoList();
		CurrentDrawItem->Pnext = CurrentLibEntry->m_Drawings;
		CurrentLibEntry->m_Drawings = CurrentDrawItem;
		switch ( CurrentDrawItem->m_StructType )
		{
			case COMPONENT_ARC_DRAW_TYPE:
				((LibDrawArc*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
				break;

			case COMPONENT_CIRCLE_DRAW_TYPE:
				((LibDrawCircle*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
				break;

			case COMPONENT_RECT_DRAW_TYPE:
				((LibDrawSquare*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
				break;

			case COMPONENT_POLYLINE_DRAW_TYPE:
				((LibDrawPolyline*)CurrentDrawItem)->m_Fill = FlSymbol_Fill;
				break;

			case COMPONENT_PIN_DRAW_TYPE:
			case COMPONENT_LINE_DRAW_TYPE:
			case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:
				break;
		}
		CurrentLibEntry->SortDrawItems();
	}

	if ( m_ID_current_state ) SetCursor(wxCURSOR_PENCIL);
	else SetCursor(wxCURSOR_ARROW);

	if ( (CurrentDrawItem->m_Flags & IS_MOVED) )
	{
	wxPoint pos;
		pos.x = GetScreen()->m_Curseur.x + InitPosition.x - StartCursor.x ,
		pos.y = GetScreen()->m_Curseur.y - InitPosition.y - StartCursor.y ;
		MoveLibDrawItemAt(CurrentDrawItem, pos );
	}		

	DrawLibEntry(DrawPanel, DC, CurrentLibEntry, 0, 0, CurrentUnit,
						CurrentConvert, GR_DEFAULT_DRAWMODE );

	CurrentDrawItem->m_Flags = 0;
	CurrentDrawItem = NULL;

	m_CurrentScreen->SetModify();

	m_CurrentScreen->ManageCurseur = NULL;
	m_CurrentScreen->ForceCloseManageCurseur = NULL;
}



/***************************************************************/
static void ComputeArc(LibDrawArc * DrawItem, wxPoint ArcCentre)
/***************************************************************/
/* routine d'ajustage des parametres de l'arc en cours de trace
	calcule le centre, rayon, angles pour que l'arc en cours
	passe par les points ArcStartX,Y et ArcEndX,Y avec le centre le plus proche
	de la pos souris
	Remarque: le centre n'est evidemment pas sur la grille
*/
{
int dx, dy;
int cX, cY;			/* Coord centre de l'arc */
int angle;

	cX = ArcCentre.x; cY = ArcCentre.y;

	cY = -cY;	/* Attention a l'orientation de l'axe Y */

	/* calcul de cX et cY pour que l'arc passe par ArcStartX,Y et ArcEndX,Y */
	dx = ArcEndX - ArcStartX; dy = ArcEndY - ArcStartY;
	cX -= ArcStartX; cY -= ArcStartY;
	angle = (int)(atan2(dy, dx) *1800 /M_PI);
	RotatePoint( &dx, &dy, angle);	/* Le segment dx, dy est horizontal */
									/* -> dx = longueur, dy = 0 */
	RotatePoint( &cX, &cY, angle);
	cX = dx / 2;	/* cX, cY est sur la mediane du segment 0,0 a dx,0 */

	RotatePoint( &cX, &cY, -angle);
	cX += ArcStartX; cY += ArcStartY;

	DrawItem->m_Pos.x = cX; DrawItem->m_Pos.y = cY;

	dx = ArcStartX - DrawItem->m_Pos.x;
	dy = ArcStartY - DrawItem->m_Pos.y;

	DrawItem->m_Rayon = (int)sqrt( (dx*dx) + (dy*dy) );

	DrawItem->t1 = (int)(atan2(dy, dx) *1800 /M_PI);

	dx = ArcEndX - DrawItem->m_Pos.x;
	dy = ArcEndY - DrawItem->m_Pos.y;

	DrawItem->t2 = (int)(atan2(dy, dx) *1800 /M_PI);
	
	DrawItem->m_Start.x = ArcStartX;
	DrawItem->m_Start.y = ArcStartY;
	DrawItem->m_End.x = ArcEndX;
	DrawItem->m_End.y = ArcEndY;
	
	NORMALIZE_ANGLE(DrawItem->t1);
	NORMALIZE_ANGLE(DrawItem->t2);	// angles = 0 .. 3600
	
	// limitation val abs a < 1800 (1/2 cercle) pour eviter Pbs d'affichage en miroir
	// car en trace on suppose que l'arc fait moins de 180 deg pour trouver
	// son orientation apres rot, miroir...
	if ( (DrawItem->t2 - DrawItem->t1) > 1800 ) DrawItem->t2 -= 3600;
	else if ( (DrawItem->t2 - DrawItem->t1) <= -1800 ) DrawItem->t2 += 3600;
	
wxString msg;
	angle = DrawItem->t2 - DrawItem->t1;
	msg.Printf(_("Arc %.1f deg"), (float)angle/10 );
	EDA_Appl->LibeditFrame->PrintMsg(msg);
	
	while ( (DrawItem->t2 - DrawItem->t1) >= 1800 )
	{
			DrawItem->t2--;
			DrawItem->t1++;
	}
	while ( (DrawItem->t1 - DrawItem->t2) >= 1800 )
	{
			DrawItem->t2++;
			DrawItem->t1--;
	}

	NORMALIZE_ANGLE(DrawItem->t1);
	NORMALIZE_ANGLE(DrawItem->t2);

}

/***************************************************/
void WinEDA_LibeditFrame::DeleteDrawPoly(wxDC * DC)
/**************************************************/
/* Used for deleting last entered segment while creating a Polyline
*/
{
	
	if ( CurrentDrawItem == NULL ) return;
	if( CurrentDrawItem->m_StructType != COMPONENT_POLYLINE_DRAW_TYPE )
		return;

int * ptpoly;
LibDrawPolyline * Poly = (LibDrawPolyline*)CurrentDrawItem;

	DrawLibraryDrawStruct(DrawPanel, DC, CurrentLibEntry, 0 , 0,
						CurrentDrawItem, CurrentUnit, g_XorMode);

	while( Poly->n > 2 )	// First segment is kept, only its end point is changed
	{
		Poly->n --;
		ptpoly = Poly->PolyList + (2 * (Poly->n - 1));
		if ( (ptpoly[0] != m_CurrentScreen->m_Curseur.x)  ||
			 (ptpoly[1] != - m_CurrentScreen->m_Curseur.y) )
		{
			ptpoly[0] = m_CurrentScreen->m_Curseur.x;
			ptpoly[1] = - m_CurrentScreen->m_Curseur.y;
			break;
		}
	}

	int allocsize =  2 * sizeof(int) * Poly->n;
	Poly->PolyList = (int*)realloc(Poly->PolyList, allocsize );

	DrawLibraryDrawStruct(DrawPanel, DC, CurrentLibEntry, 0 , 0,
					CurrentDrawItem, CurrentUnit, g_XorMode);
}

