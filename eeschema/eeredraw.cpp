/*****************************************************************************
*	Program to draw EE diagrams.											 *
* This module redraw/draw all structs.										 *
*****************************************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"


char marq_bitmap[]=
	{
	12, 12, 0, 0,	/* Dimensions x et y, offsets x et y du bitmap de marqueurs*/
	YELLOW,			/* Couleur */
	1,1,1,1,1,1,1,1,0,0,0,0,	/* bitmap: >= 1 : color, 0 = notrace */
	1,1,1,0,1,0,1,1,0,0,0,0,
	1,1,1,1,0,0,0,1,0,0,0,0,
	1,0,1,1,1,0,0,0,0,0,0,0,
	1,1,0,1,1,1,0,0,0,0,0,0,
	1,1,0,0,1,1,1,0,0,0,0,0,
	1,1,1,0,0,1,1,1,0,0,0,0,
	0,0,0,0,0,0,1,1,1,0,0,0,
	0,0,0,0,0,0,0,1,1,1,0,0,
	0,0,0,0,0,0,0,0,1,1,1,0,
	0,0,0,0,0,0,0,0,0,1,1,1,
	0,0,0,0,0,0,0,0,0,0,1,0
	};

char marqERC_bitmap[]=
	{
	8, 8, 0, 0,	/* Dimensions x et y , offsets x et y du bitmap de marqueurs*/
	-1,			/* Color: -1 = couleur non précisée */
	1,1,1,1,1,0,0,0,
	1,1,1,0,1,0,0,0,
	1,1,1,1,0,0,0,0,
	1,0,1,1,1,0,0,0,
	1,1,0,1,1,1,0,0,
	0,0,0,0,1,1,1,0,
	0,0,0,0,0,1,1,1,
	0,0,0,0,0,0,1,0,
	};


static EDA_BaseStruct *HighLightStruct = NULL;

/************************************************************/
void  DrawDanglingSymbol(WinEDA_DrawPanel * panel,wxDC * DC,
			const wxPoint & pos, int Color)
/************************************************************/
{
#define DANGLING_SYMBOL_SIZE 12
	if ( ! g_IsPrinting )	// Draw but do not print the Dangling Symbol */
	{
		GRRect(&panel->m_ClipBox, DC,
			pos.x - DANGLING_SYMBOL_SIZE, pos.y - DANGLING_SYMBOL_SIZE,
			pos.x + DANGLING_SYMBOL_SIZE, pos.y + DANGLING_SYMBOL_SIZE,
			Color);
	}
}


/*************************************************/
void SetHighLightStruct(EDA_BaseStruct *HighLight)
/*************************************************/
{
	HighLightStruct = HighLight;
}

/**********************************************************************/
void WinEDA_SchematicFrame::RedrawActiveWindow(wxDC * DC, bool EraseBg)
/**********************************************************************/
/*
 Redraws only the active window which is assumed to be whole visible.
*/
{
wxString title;
	if( GetScreen() == NULL ) return;

	ActiveScreen = GetScreen();
	/* Forcage de la reinit de la brosse et plume courante */
	GRResetPenAndBrush(DC);
	DC->SetBackground(*wxBLACK_BRUSH);
	DC->SetBackgroundMode(wxTRANSPARENT);

	GetScreen()->CursorOff(DrawPanel, DC); // effacement curseur
	if ( GetScreen()->m_FirstRedraw )
	{
		m_CurrentScreen->SetZoom(BestZoom());
		GetScreen()->m_FirstRedraw = FALSE;
	}

	if(GetScreen()->ManageCurseur)
	{
		GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
	}

	if ( EraseBg ) DrawPanel->EraseScreen(DC);

	DrawPanel->DrawBackGround(DC);

	RedrawStructList(DrawPanel, DC, GetScreen()->EEDrawList, GR_DEFAULT_DRAWMODE);

	TraceWorkSheet(DC, GetScreen());

	GetScreen()->CursorOn(DrawPanel, DC); // reaffichage curseur
	if(GetScreen()->ManageCurseur)
	{
		GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
	}

	Affiche_Status_Box();
	GetScreen()->ClrRefreshReq();
	if( GetScreen()->m_FileName == g_DefaultSchematicFileName )
	{
		wxString msg = g_Main_Title + wxT(" ") + GetBuildVersion();
		title.Printf( wxT("%s [%s]"), msg.GetData(),GetScreen()->m_FileName.GetData());
		SetTitle(title);
	}
	else
	{
		title.Printf( wxT("[%s]"), GetScreen()->m_FileName.GetData());
		SetTitle(title);
	}

}

/*******************************************************************************/
void WinEDA_DrawPanel::PrintPage(wxDC * DC, bool Print_Sheet_Ref, int PrintMask)
/*******************************************************************************/
{
BASE_SCREEN * screen, * oldscreen = m_Parent->GetScreen();

	wxBeginBusyCursor();
		

	screen = m_Parent->m_CurrentScreen = ActiveScreen;
	RedrawStructList(this,DC, screen->EEDrawList, GR_COPY);

	if ( Print_Sheet_Ref )
		m_Parent->TraceWorkSheet(DC, screen);

	m_Parent->m_CurrentScreen = oldscreen;
	wxEndBusyCursor();
}


/*****************************************************************************
* Routine to redraw list of structs.										 *
* If the list is of DrawPickStruct types then the picked item are drawn.	 *
*****************************************************************************/
void RedrawStructList(WinEDA_DrawPanel * panel, wxDC * DC,
				EDA_BaseStruct *Structs, int DrawMode, int Color)
{
	while (Structs)
		{
		if (Structs->m_StructType == DRAW_PICK_ITEM_STRUCT_TYPE)
			{
			RedrawOneStruct(panel, DC,
						((DrawPickedStruct *) Structs)->m_PickedStruct,
									 DrawMode, Color);
			}
		else
			{
			if ( ! (Structs->m_Flags & IS_MOVED) )
				RedrawOneStruct(panel, DC, Structs, DrawMode, Color);
			}

		Structs = Structs->Pnext;
		}
}

/*****************************************************************************
* Routine to redraw list of structs.										 *
*****************************************************************************/
void RedrawOneStruct(WinEDA_DrawPanel * panel, wxDC * DC,
				EDA_BaseStruct *Struct, int DrawMode, int Color)
{

	if ( Struct == NULL ) return;
	if (HighLightStruct == Struct) Color = HIGHLIGHT_COLOR;

	switch (Struct->m_StructType)
		{
	case DRAW_POLYLINE_STRUCT_TYPE:
		 RedrawPolylineStruct(panel, DC, (DrawPolylineStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_JUNCTION_STRUCT_TYPE:
		 RedrawConnectionStruct(panel, DC, (DrawJunctionStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_SEGMENT_STRUCT_TYPE:
		 Redraw_DrawLineStruct(panel, DC, (EDA_DrawLineStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_BUSENTRY_STRUCT_TYPE:
		 ReDrawBusEntryStruct(panel, DC, (DrawBusEntryStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_TEXT_STRUCT_TYPE:
		 RedrawTextStruct(panel, DC, (DrawTextStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_LABEL_STRUCT_TYPE:
		 RedrawTextStruct(panel, DC, (DrawTextStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
		 RedrawGlobalLabelStruct(panel, DC, (DrawGlobalLabelStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_LIB_ITEM_STRUCT_TYPE:
   	if ( Struct->m_Flags & IS_MOVED )
			DrawStructsInGhost(panel, DC, (EDA_SchComponentStruct *) Struct, 0, 0 );
      else
		 DrawLibPart(panel, DC, (EDA_SchComponentStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_SHEET_STRUCT_TYPE:
		RedrawSheetStruct(panel, DC, (DrawSheetStruct *) Struct, DrawMode, Color);
		break;

	case DRAW_SHEETLABEL_STRUCT_TYPE:
		 RedrawSheetLabelStruct(panel, DC, (DrawSheetLabelStruct *) Struct, DrawMode, Color);
		 break;

	case DRAW_MARKER_STRUCT_TYPE:
		RedrawMarkerStruct(panel, DC, (DrawMarkerStruct *) Struct, DrawMode, Color);
		break;

	case DRAW_NOCONNECT_STRUCT_TYPE:
		RedrawNoConnectStruct(panel, DC, (DrawNoConnectStruct *) Struct, DrawMode, Color);
		break;

	default:
		wxBell();
		break;
	}
}

/****************************************************************/
void RedrawSheetLabelStruct(WinEDA_DrawPanel * panel,wxDC * DC,
					DrawSheetLabelStruct *Struct,
					int DrawMode, int Color)
/****************************************************************/
/* Routine de dessin des Labels type hierarchie */
{
int side, txtcolor;
int posx , tposx, posy, size2;
wxSize size;
int NbSegm, coord[12];

	if( Color >= 0 ) txtcolor = Color;
	else txtcolor = ReturnLayerColor(Struct->m_Layer);
	GRSetDrawMode(DC, DrawMode);

	posx = Struct->m_Pos.x; posy = Struct->m_Pos.y; size = Struct->m_Size;
	if( !Struct->m_Text.IsEmpty() )
	{
		if( Struct->m_Edge )
		{
			tposx = posx - size.x;
			side = GR_TEXT_HJUSTIFY_RIGHT;
		}
		else
		{
			tposx = posx + size.x + (size.x /8) ;
			side = GR_TEXT_HJUSTIFY_LEFT;
		}
		DrawGraphicText(panel, DC, wxPoint(tposx, posy), txtcolor,
					Struct->m_Text, TEXT_ORIENT_HORIZ,size ,
					side, GR_TEXT_VJUSTIFY_CENTER);
	}
	/* dessin du symbole de connexion */

	if(Struct->m_Edge)
		{
		size.x = -size.x;
		size.y = -size.y;
		}
		
	coord[0] = posx; coord[1] = posy; size2 = size.x /2;
	NbSegm = 0;
	switch(Struct->m_Shape)
		{
		case 0:		/* input |> */
			coord[2] = posx ; coord[3] = posy - size2;
			coord[4] = posx + size2; coord[5] = posy - size2;
			coord[6] = posx + size.x; coord[7] = posy;
			coord[8] = posx + size2; coord[9] = posy + size2;
			coord[10] = posx ; coord[11] = posy + size2;
			coord[12] = coord[0] ; coord[13] = coord[1];
			NbSegm = 7;
			break;

		case 1:		/* output <| */
			coord[2] = posx + size2; coord[3] = posy - size2;
			coord[4] = posx + size.x; coord[5] = posy - size2;
			coord[6] = posx + size.x; coord[7] = posy + size2;
			coord[8] = posx + size2; coord[9] = posy + size2;
			coord[10] = coord[0] ; coord[11] = coord[1];
			NbSegm = 6;
			break;

		case 2:		/* bidi <> */
		case 3:		/* TriSt <> */
			coord[2] = posx + size2; coord[3] = posy - size2;
			coord[4] = posx + size.x; coord[5] = posy;
			coord[6] = posx + size2; coord[7] = posy +size2;
			coord[8] = coord[0];  coord[9] = coord[1];
			NbSegm = 5;
			break;

		default:	 /* unsp []*/
			coord[2] = posx ; coord[3] = posy - size2;
			coord[4] = posx + size.x; coord[5] = posy - size2;
			coord[6] = posx + size.x; coord[7] = posy + size2;
			coord[8] = posx ; coord[9] = posy + size2;
			coord[10] = coord[0] ; coord[11] = coord[1];
			NbSegm = 6;
			break;
		}
//	GRPoly(&panel->m_ClipBox, DC, NbSegm, coord, 1, txtcolor, txtcolor);	/* Poly rempli */
	GRPoly(&panel->m_ClipBox, DC, NbSegm, coord, 0, txtcolor, txtcolor);	/* Poly Non rempli */
}



/*************************************************************/
void RedrawSheetStruct(WinEDA_DrawPanel * panel,wxDC * DC,
		DrawSheetStruct *Struct, int DrawMode, int Color)
/*************************************************************/
/* Routine de dessin du bloc type hierarchie */
{
DrawSheetLabelStruct * SheetLabelStruct;
int txtcolor;
wxString Text;
int color;

	if( Color >= 0 ) color = Color;
	else color = ReturnLayerColor(Struct->m_Layer);
	GRSetDrawMode(DC, DrawMode);

	GRRect(&panel->m_ClipBox, DC, Struct->m_Pos.x, Struct->m_Pos.y,
				 Struct->m_Pos.x + Struct->m_End.x,
				 Struct->m_Pos.y + Struct->m_End.y, color);

	/* Trace des textes : SheetName */
	if( Color > 0 ) txtcolor = Color;
	else txtcolor = ReturnLayerColor(LAYER_SHEETNAME);

	Text = wxT("Sheet: ") + Struct->m_Field[VALUE].m_Text;
	DrawGraphicText(panel, DC,
				wxPoint(Struct->m_Pos.x, Struct->m_Pos.y - 8), txtcolor,
				Text, TEXT_ORIENT_HORIZ, Struct->m_Field[VALUE].m_Size,
				GR_TEXT_HJUSTIFY_LEFT, GR_TEXT_VJUSTIFY_BOTTOM);

	/* Trace des textes : FileName */
	if( Color >= 0 ) txtcolor = Color;
	else txtcolor = ReturnLayerColor(LAYER_SHEETFILENAME);
	Text = wxT("File: ") + Struct->m_Field[SHEET_FILENAME].m_Text;
	DrawGraphicText(panel, DC,
				wxPoint(Struct->m_Pos.x, Struct->m_Pos.y + Struct->m_End.y + 4),
				txtcolor,
				Text, TEXT_ORIENT_HORIZ, Struct->m_Field[SHEET_FILENAME].m_Size,
				GR_TEXT_HJUSTIFY_LEFT, GR_TEXT_VJUSTIFY_TOP);


	/* Trace des textes : SheetLabel */
	SheetLabelStruct = Struct->m_Label;
	while( SheetLabelStruct != NULL )
		{
		RedrawSheetLabelStruct(panel, DC, SheetLabelStruct,DrawMode, Color);
		SheetLabelStruct = (DrawSheetLabelStruct*)(SheetLabelStruct->Pnext);
		}
}


/*********************************************************************/
void Redraw_DrawLineStruct(WinEDA_DrawPanel * panel,wxDC * DC,
			EDA_DrawLineStruct *Struct, int DrawMode, int Color)
/*********************************************************************/
/* Routine de dessin des segments type wire, Bus .. */
{
int color;
int zoom = panel->GetZoom();

	if( Color >= 0 ) color = Color;
	else color = ReturnLayerColor(Struct->m_Layer);
	GRSetDrawMode(DC, DrawMode);

	if( Struct->m_Layer == LAYER_NOTES)
		GRDashedLine(&panel->m_ClipBox, DC, Struct->m_Start.x, Struct->m_Start.y,
					 Struct->m_End.x, Struct->m_End.y, color);

	else if( (Struct->m_Layer == LAYER_BUS) && (zoom <= 16) )
		GRBusLine(&panel->m_ClipBox, DC, Struct->m_Start.x, Struct->m_Start.y,
				  Struct->m_End.x, Struct->m_End.y, color);
	else
		GRLine(&panel->m_ClipBox, DC, Struct->m_Start.x, Struct->m_Start.y,
				Struct->m_End.x, Struct->m_End.y, color);
	if ( Struct->m_StartIsDangling )
		DrawDanglingSymbol(panel, DC, Struct->m_Start, color);

	if ( Struct->m_EndIsDangling )
		DrawDanglingSymbol(panel, DC, Struct->m_End, color);
}


/*******************************************************************/
void RedrawMarkerStruct(WinEDA_DrawPanel * panel,wxDC * DC,
		DrawMarkerStruct *Struct, int DrawMode, int Color)
/*******************************************************************/
/* Routine de dessin des marqueurs .. */
{
#define WAR 1	// utilisé aussi dans erc.cpp

	if( Struct->m_Type == MARQ_ERC )
		{
		int color = Color;
		if ( Color <= 0 )
			{
			color = (Struct->m_MarkFlags == WAR ) ?
				g_LayerDescr.LayerColor[LAYER_ERC_WARN] :
				g_LayerDescr.LayerColor[LAYER_ERC_ERR];
			}
		Draw_Marqueur(panel, DC, Struct->m_Pos, marqERC_bitmap, DrawMode, color);
		}

	else Draw_Marqueur(panel, DC, Struct->m_Pos, marq_bitmap, DrawMode, Color);
}

/*************************************************************************/
/* void RedrawNoConnectStruct(DrawNoConnectStruct *Struct, int DrawMode) */
/*************************************************************************/
/* Routine de dessin des symboles de "No Connexion" .. */
#define DELTA (DRAWNOCONNECT_SIZE/2)
void RedrawNoConnectStruct(WinEDA_DrawPanel * panel, wxDC * DC,
		DrawNoConnectStruct *Struct, int DrawMode, int Color)
{
int pX, pY, color;

	pX = Struct->m_Pos.x; pY = Struct->m_Pos.y;

	if( Color >= 0 ) color = Color;
	else color = ReturnLayerColor(LAYER_NOCONNECT);
	GRSetDrawMode(DC, DrawMode);

	GRLine(&panel->m_ClipBox, DC, pX - DELTA, pY - DELTA, pX + DELTA, pY + DELTA, color);
	GRLine(&panel->m_ClipBox, DC, pX + DELTA, pY - DELTA, pX - DELTA, pY + DELTA, color);

}

/**************************************************************/
void ReDrawBusEntryStruct(WinEDA_DrawPanel * panel,wxDC * DC,
		DrawBusEntryStruct *Struct, int DrawMode, int Color)
/***************************************************************/

/* Routine de dessin des Raccords a 45 degre type wire, Bus .. */

{
int color;
int zoom = panel->GetZoom();

	if( Color >= 0 ) color = Color;
	else color = ReturnLayerColor(Struct->m_Layer);
	GRSetDrawMode(DC, DrawMode);

	if( (Struct->m_Layer == LAYER_BUS) && (zoom <= 16) )
		GRBusLine(&panel->m_ClipBox, DC, Struct->m_Pos.x, Struct->m_Pos.y,
				  Struct->m_End().x, Struct->m_End().y, color);
	else
		GRLine(&panel->m_ClipBox, DC, Struct->m_Pos.x, Struct->m_Pos.y,
				  Struct->m_End().x, Struct->m_End().y, color);

}

/*****************************************************************************
* Routine to redraw polyline struct.										 *
*****************************************************************************/
void RedrawPolylineStruct(WinEDA_DrawPanel * panel, wxDC * DC,
					DrawPolylineStruct *Struct, int DrawMode, int Color)
{
int i, color ;
int zoom = panel->GetZoom();

	if( Color >= 0 ) color = Color;
	else color = ReturnLayerColor(Struct->m_Layer);
	GRSetDrawMode(DC, DrawMode);

	GRMoveTo(Struct->m_Points[0], Struct->m_Points[1]);
	if( Struct->m_Layer == LAYER_NOTES)
		{
		for (i = 1; i < Struct->m_NumOfPoints; i++)
			GRDashedLineTo(&panel->m_ClipBox, DC, Struct->m_Points[i * 2],
							Struct->m_Points[i * 2 + 1], color);
		}
	else if( (Struct->m_Layer == LAYER_BUS) && (zoom <= 16) )
		{
		for (i = 1; i < Struct->m_NumOfPoints; i++)
			GRBusLineTo(&panel->m_ClipBox, DC, Struct->m_Points[i * 2],
						Struct->m_Points[i * 2 + 1], color);
		}
	else
		{
		for (i = 1; i < Struct->m_NumOfPoints; i++)
			GRLineTo(&panel->m_ClipBox, DC, Struct->m_Points[i * 2], Struct->m_Points[i * 2 + 1], color);
		}
}

/*****************************************************************************
* Routine to redraw connection struct.										 *
*****************************************************************************/
void RedrawConnectionStruct(WinEDA_DrawPanel * panel,wxDC * DC,
						DrawJunctionStruct *Struct, int DrawMode,
								int Color)
{
int color;
int	Width = DRAWJUNCTION_SIZE;

	if( Color >= 0 ) color = Color;
	else color = ReturnLayerColor(Struct->m_Layer);
	GRSetDrawMode(DC, DrawMode);

	GRFilledRect(&panel->m_ClipBox, DC, Struct->m_Pos.x - Width, Struct->m_Pos.y - Width,
			Struct->m_Pos.x + Width, Struct->m_Pos.y + Width, color, color);
}
/***************************************************************/
void RedrawTextStruct(WinEDA_DrawPanel * panel,wxDC * DC,
		DrawTextStruct *Struct, int DrawMode, int Color)
/***************************************************************/
/* Les textes type label ou notes peuvent avoir 4 directions, mais
	sont tj cadres par rapport a la 1ere lettre du texte
*/
{
int color;

	if( Color >= 0 ) color = Color;
	else color = ReturnLayerColor(Struct->m_Layer);
	GRSetDrawMode(DC, DrawMode);

	switch(Struct->m_Orient)
		{
		case 0:		/* Orientation horiz normale */
			DrawGraphicText(panel, DC,
						wxPoint(Struct->m_Pos.x, Struct->m_Pos.y - TXTMARGE),
						color,
						Struct->m_Text, Struct->m_Orient*900, Struct->m_Size,
						GR_TEXT_HJUSTIFY_LEFT, GR_TEXT_VJUSTIFY_BOTTOM);
			break;

		case 1:		/* Orientation vert UP */
			DrawGraphicText(panel, DC,
						wxPoint(Struct->m_Pos.x - TXTMARGE, Struct->m_Pos.y), color,
						Struct->m_Text, Struct->m_Orient*900, Struct->m_Size,
						GR_TEXT_HJUSTIFY_RIGHT, GR_TEXT_VJUSTIFY_BOTTOM);
			break;

		case 2:		/* Orientation horiz inverse */
			DrawGraphicText(panel, DC,
						wxPoint(Struct->m_Pos.x, Struct->m_Pos.y + TXTMARGE), color,
						Struct->m_Text, Struct->m_Orient*900, Struct->m_Size,
						GR_TEXT_HJUSTIFY_RIGHT, GR_TEXT_VJUSTIFY_TOP);
			break;

		case 3:		/* Orientation vert BOTTOM */
			DrawGraphicText(panel, DC,
						wxPoint(Struct->m_Pos.x + TXTMARGE, Struct->m_Pos.y), color,
						Struct->m_Text, Struct->m_Orient*900, Struct->m_Size,
						GR_TEXT_HJUSTIFY_LEFT, GR_TEXT_VJUSTIFY_TOP);
			break;
		}
	if ( Struct->m_IsDangling )
		DrawDanglingSymbol(panel, DC, Struct->m_Pos, color);
}

/*****************************************************************************/
void RedrawGlobalLabelStruct(WinEDA_DrawPanel * panel, wxDC * DC,
		DrawGlobalLabelStruct *Struct, int DrawMode, int Color)
/*****************************************************************************/
/* Les textes type Global label peuvent avoir 4 directions, mais
	sont tj cadres par rapport au symbole graphique (icone)
*/
{
int * Template;
int Poly[12];
int ii, jj, imax, color, HalfSize;
wxSize Size = Struct->m_Size;

	if( Color >= 0 ) color = Color;
	else color = ReturnLayerColor(Struct->m_Layer);

	GRSetDrawMode(DC, DrawMode);

	HalfSize = Size.x / 2; ii = Size.x + TXTMARGE;

	switch(Struct->m_Orient)
		{
		case 0:		/* Orientation horiz normale */
			DrawGraphicText(panel, DC,
						wxPoint(Struct->m_Pos.x - ii, Struct->m_Pos.y), color,
						Struct->m_Text, TEXT_ORIENT_HORIZ, Size,
						GR_TEXT_HJUSTIFY_RIGHT, GR_TEXT_VJUSTIFY_CENTER);
			break;

		case 1:		/* Orientation vert UP */
			DrawGraphicText(panel, DC,
						wxPoint(Struct->m_Pos.x, Struct->m_Pos.y + ii), color,
						Struct->m_Text, TEXT_ORIENT_VERT, Size,
						GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_TOP);
			break;

		case 2:		/* Orientation horiz inverse */
			DrawGraphicText(panel, DC,
						wxPoint(Struct->m_Pos.x + ii, Struct->m_Pos.y), color,
						Struct->m_Text, TEXT_ORIENT_HORIZ, Size,
						GR_TEXT_HJUSTIFY_LEFT, GR_TEXT_VJUSTIFY_CENTER);
			break;

		case 3:		/* Orientation vert BOTTOM */
			DrawGraphicText(panel, DC,
						wxPoint(Struct->m_Pos.x, Struct->m_Pos.y - ii), color,
						Struct->m_Text, TEXT_ORIENT_VERT, Size,
						GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_BOTTOM);
			break;
		}


	Template = TemplateShape[Struct->m_Shape][Struct->m_Orient];

	imax = *Template; Template++;
	for ( ii = 0, jj = 0; ii < imax ; ii++ )
		{
		Poly[jj] = ( HalfSize * (*Template) ) + Struct->m_Pos.x;
		jj++; Template++;
		Poly[jj] = ( HalfSize * (*Template) ) + Struct->m_Pos.y;
		jj++; Template++;
		}

//	GRPoly(&panel->m_ClipBox, DC, imax,Poly,1, color, color );	/* Polygne Rempli */
	GRPoly(&panel->m_ClipBox, DC, imax,Poly,0, color, color );	/* Polygne Non Rempli */

	if ( Struct->m_IsDangling )
		DrawDanglingSymbol(panel, DC, Struct->m_Pos, color);
}

/**********************************************************/
void DrawStructsInGhost(WinEDA_DrawPanel * panel,wxDC * DC,
		EDA_BaseStruct * DrawStruct, int dx, int dy )
/**********************************************************/
/* Routine de redessin en mode fantome (Dessin simplifie en g_XorMode et
 g_GhostColor
de structures.
	Utilisee dans les deplacements de blocs
*/
{
int Width, ii;
int DrawMode = g_XorMode;

	GRSetDrawMode(DC, DrawMode);

	switch (DrawStruct->m_StructType)
		{
		case DRAW_POLYLINE_STRUCT_TYPE:
			{
			DrawPolylineStruct * Struct;
			Struct = (DrawPolylineStruct * ) DrawStruct;
			GRMoveTo(Struct->m_Points[0] + dx, Struct->m_Points[1] + dy);
			for (ii = 1; ii < Struct->m_NumOfPoints; ii++)
			GRLineTo(&panel->m_ClipBox, DC, Struct->m_Points[ii * 2] + dx,
					 Struct->m_Points[ii * 2 + 1] +dy, g_GhostColor);
			break;
			}

		case DRAW_SEGMENT_STRUCT_TYPE:
			{
			EDA_DrawLineStruct * Struct;
			Struct = (EDA_DrawLineStruct * ) DrawStruct;
			if( (Struct->m_Flags & STARTPOINT) == 0 )
				{
				GRMoveTo(Struct->m_Start.x + dx, Struct->m_Start.y + dy);
				}
			else
				{
				GRMoveTo(Struct->m_Start.x, Struct->m_Start.y);
				}
			if( (Struct->m_Flags & ENDPOINT) == 0 )
				{
				GRLineTo(&panel->m_ClipBox, DC, Struct->m_End.x + dx, Struct->m_End.y + dy, g_GhostColor);
				}
			else
				{
				GRLineTo(&panel->m_ClipBox, DC, Struct->m_End.x, Struct->m_End.y, g_GhostColor);
				}
			break;
			}

		case DRAW_BUSENTRY_STRUCT_TYPE:
			{
			DrawBusEntryStruct * Struct = (DrawBusEntryStruct *) DrawStruct;
			int xx = Struct->m_Pos.x + dx, yy = Struct->m_Pos.y + dy;
			GRMoveTo(xx, yy);
			GRLineTo(&panel->m_ClipBox, DC, Struct->m_Size.x + xx, Struct->m_Size.y + yy, g_GhostColor);
			break;
			}

		case DRAW_JUNCTION_STRUCT_TYPE:
			{
			DrawJunctionStruct * Struct;
			Struct = (DrawJunctionStruct * ) DrawStruct;
			Width = DRAWJUNCTION_SIZE;
			GRFilledRect(&panel->m_ClipBox, DC, Struct->m_Pos.x - Width + dx, Struct->m_Pos.y - Width + dy,
				Struct->m_Pos.x + Width + dx, Struct->m_Pos.y + Width + dy,
				g_GhostColor, g_GhostColor);
			break;
			}

		case DRAW_TEXT_STRUCT_TYPE:
			{
			DrawTextStruct * Struct;
			Struct = (DrawTextStruct * ) DrawStruct;
			Struct->m_Pos.x += dx; Struct->m_Pos.y += dy;
			RedrawTextStruct(panel, DC, Struct,DrawMode,g_GhostColor);
			Struct->m_Pos.x -= dx; Struct->m_Pos.y -= dy;
			break;
			}

		case DRAW_LABEL_STRUCT_TYPE:
			{
			DrawLabelStruct * Struct;
			Struct = (DrawLabelStruct * ) DrawStruct;
			Struct->m_Pos.x += dx; Struct->m_Pos.y += dy;
			RedrawTextStruct(panel, DC, (DrawTextStruct*)Struct,DrawMode,g_GhostColor);
			Struct->m_Pos.x -= dx; Struct->m_Pos.y -= dy;
			break;
			}

		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
			{
			DrawGlobalLabelStruct * Struct;
			Struct = (DrawGlobalLabelStruct * ) DrawStruct;
			Struct->m_Pos.x += dx; Struct->m_Pos.y += dy;
			RedrawGlobalLabelStruct(panel, DC, Struct,DrawMode,g_GhostColor);
			Struct->m_Pos.x -= dx; Struct->m_Pos.y -= dy;
			break;
			}

		case DRAW_NOCONNECT_STRUCT_TYPE:
			{
			DrawNoConnectStruct * Struct;
			Struct = (DrawNoConnectStruct * ) DrawStruct;
			Struct->m_Pos.x += dx; Struct->m_Pos.y += dy;
			RedrawNoConnectStruct(panel, DC, Struct,DrawMode,g_GhostColor);
			Struct->m_Pos.x -= dx; Struct->m_Pos.y -= dy;
			break;
			}

		case DRAW_LIB_ITEM_STRUCT_TYPE:
			{
			EDA_LibComponentStruct *LibEntry;
			EDA_SchComponentStruct *Struct;
			Struct = (EDA_SchComponentStruct * ) DrawStruct;
			LibEntry = FindLibPart(Struct->m_ChipName.GetData(), wxEmptyString, FIND_ROOT);
			if( LibEntry == NULL ) break;
			DrawingLibInGhost(panel, DC, LibEntry, Struct, Struct->m_Pos.x + dx,
								Struct->m_Pos.y + dy,
								Struct->m_Multi, Struct->m_Convert,
								g_GhostColor, FALSE);
			break;
			}

		case DRAW_SHEET_STRUCT_TYPE:
			{
			DrawSheetStruct *Struct = (DrawSheetStruct * ) DrawStruct;
			GRRect(&panel->m_ClipBox, DC, Struct->m_Pos.x + dx, Struct->m_Pos.y + dy,
						Struct->m_Pos.x + Struct->m_End.x + dx,
						Struct->m_Pos.y + Struct->m_End.y + dy, g_GhostColor);
			break;
			}

		case DRAW_SHEETLABEL_STRUCT_TYPE:
		case DRAW_MARKER_STRUCT_TYPE:
			break;

		default:
			break;
	}
}


/************************************************************/
void Draw_Marqueur(WinEDA_DrawPanel * panel, wxDC * DC,
			wxPoint pos, char* pt_bitmap, int DrawMode, int Color)
/************************************************************/
/*
 Place un repere sur l'ecran au point de coordonnees PCB pos_X, pos_Y
	Le marqueur est defini par un tableau de 2 + (lig*col) elements:
	 1er element: dim nbre ligne
	 2er element: dim nbre col
	 suite: lig * col elements a 0 ou 1 : si 1 mise a color du pixel

 copie la description du marqueur en current_marqueur (global)
*/
{
int px, py, color;
char ii, ii_max, jj, jj_max;
	
	if ( pt_bitmap == NULL ) pt_bitmap = marq_bitmap;

	px = GRMapX(pos.x); py = GRMapY(pos.y);

	/* Lecture des dimensions */
	ii_max = *(pt_bitmap++); jj_max = *(pt_bitmap++);

	/* lecture des offsets */
	px += *(pt_bitmap++); py += *(pt_bitmap++);

	color = *(pt_bitmap++);
	if ( (Color > 0) ) color = Color;
	if (color < 0) color = 0;
	GRSetDrawMode(DC, DrawMode);

	/* Trace du bitmap */
	for( ii = 0; ii < ii_max; ii++)
		{
		for( jj = 0; jj < jj_max; jj++, pt_bitmap++)
			{
			if(*pt_bitmap)  GRSPutPixel(&panel->m_ClipBox, DC, px+ii , py+jj , color);
			}
		}
}


