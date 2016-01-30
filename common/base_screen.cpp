	/******************************************************************/
	/* drawpanel.cpp - fonctions des classes du type WinEDA_DrawPanel */
	/******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "fctsys.h"
#include "common.h"


/* defines locaux */
#define CURSOR_SIZE 12	/* taille de la croix du curseur PCB */

	/*******************************************************/
	/* Class BASE_SCREEN: classe de gestion d'un affichage */
	/*******************************************************/
BASE_SCREEN::BASE_SCREEN(EDA_BaseStruct * parent, WinEDA_DrawFrame * frame_source, int idscreen):
		EDA_BaseStruct(SCREEN_STRUCT_TYPE)
{
	m_Parent = parent;
	m_Type = idscreen;
	SetParentFrame(frame_source);
	m_ZoomList = NULL;
	m_GridList = NULL;
	EEDrawList = NULL;	 /* pointeur sur la liste des objets a tracer*/
	m_UndoList = NULL;
	m_RedoList = NULL;
	m_UndoRedoCountMax = 1;
	m_FirstRedraw = TRUE;
	InitDatas();
}


/******************************/
BASE_SCREEN::~BASE_SCREEN(void)
/******************************/
{
	if ( m_ZoomList ) free(m_ZoomList);
	if ( m_GridList ) free(m_GridList);
	ClearUndoRedoList();
}

/*******************************/
void BASE_SCREEN::InitDatas(void)
/*******************************/
{
	m_SheetNumber = m_NumberOfSheet = 1; /* gestion hierarchie: Root: SheetNumber = 1 */
	m_Zoom = 32;
	m_Grid = wxSize(50,50);			/* pas de la grille */
	m_GridColor = DARKGRAY;
	m_UserGrid = g_UserGrid;			/* pas de la grille "utilisateur" */
	m_UserGridIsON = FALSE;
	m_UserGridUnit = g_UserGrid_Unit;
	m_Diviseur_Grille = 1;
	m_Center = TRUE;

	/* offsets pour tracer le circuit sur l'ecran */
	switch (m_Type ) // Init taille sheet par defaut
		{
		case SCHEMATIC_FRAME :
			m_Center = FALSE;
			m_CurrentSheet = &g_Sheet_A4;
			break;

		default:
		case CVPCB_DISPLAY_FRAME:
		case MODULE_EDITOR_FRAME:
		case PCB_FRAME:
			m_CurrentSheet = &g_Sheet_A3;
			break;

		case GERBER_FRAME:
			m_CurrentSheet = &g_Sheet_GERBER;
			break;
		}

	if ( m_Center )
		{
		m_Curseur.x = m_Curseur.y = 0;
		m_DrawOrg.x = - ReturnPageSize().x/2;
		m_DrawOrg.y = - ReturnPageSize().y/2;
		}
	else
		{
		m_DrawOrg.x = m_DrawOrg.y = 0;
		m_Curseur.x = ReturnPageSize().x/2;
		m_Curseur.y = ReturnPageSize().y/2;
		}

	// DrawOrg est rendu multiple du zoom min :
	m_DrawOrg.x -= m_DrawOrg.x % 256; m_DrawOrg.y -= m_DrawOrg.y % 256;

	m_O_Curseur = m_Curseur;

	/* gestion du curseur et de la souris */
	ManageCurseur = NULL;			/* Fonction d'affichage sur deplacement souris */
	ForceCloseManageCurseur = NULL;
	m_CurseurShape = 0;				/* indique une option de forme */

	m_CurrentItem = NULL;

	/* indicateurs divers */
	m_FlagRefreshReq = 0;				/* indique que l'ecran doit redessine */
	m_FlagModified = 0;				// indique modif du PCB,utilise pour eviter une sortie sans sauvegarde
	m_FlagSave = 1;					// indique sauvegarde auto faite
}


/*****************************************************************/
void BASE_SCREEN::SetParentFrame(WinEDA_DrawFrame * frame_source)
/****************************************************************/
{
	m_FrameSource = frame_source;
}

/*********************************************************/
WinEDA_DrawFrame * BASE_SCREEN::GetParentFrame(void)
/*********************************************************/
{
	return m_FrameSource;
}
	
/***************************************/
int BASE_SCREEN::GetInternalUnits(void)
/***************************************/
{
	if ( m_FrameSource ) return m_FrameSource->m_InternalUnits;
	else switch (m_Type )
		{
		default:
		case SCHEMATIC_FRAME :
			return EESCHEMA_INTERNAL_UNIT;
			break;

		case GERBER_FRAME:
		case CVPCB_DISPLAY_FRAME:
		case MODULE_EDITOR_FRAME:
		case PCB_FRAME:
			return PCB_INTERNAL_UNIT;
		}
}

/*****************************************/
wxSize BASE_SCREEN::ReturnPageSize(void)
/*****************************************/
/* Retourne en unites internes la taille de la feuille de dessin
	(la taille de la feuille est connue en 1/1000 ")
*/
{
wxSize PageSize;
	
	switch (m_Type )
		{
		default:
		case SCHEMATIC_FRAME :
			PageSize = m_CurrentSheet->m_Size;
			break;

		case GERBER_FRAME:
		case CVPCB_DISPLAY_FRAME:
		case MODULE_EDITOR_FRAME:
		case PCB_FRAME:
			PageSize.x = m_CurrentSheet->m_Size.x * (PCB_INTERNAL_UNIT/1000);
			PageSize.y = m_CurrentSheet->m_Size.y * (PCB_INTERNAL_UNIT/1000);
			break;
		}
		
	return PageSize;
}


/********************************************/
void BASE_SCREEN::SetZoomList(int * zoomlist)
/********************************************/
/* init liste des zoom (NULL terminated)
*/
{
int ii, nbitems, * zoom;
	// Decompte des items
	for ( nbitems = 1, zoom = zoomlist;  ; zoom ++, nbitems++ )
	{
		if ( *zoom == 0 ) break;
	}
	
	// Init liste
	if ( m_ZoomList ) free(m_ZoomList);
	m_ZoomList = (int*) MyZMalloc( nbitems * sizeof ( int) );
	
	for ( ii = 0, zoom = zoomlist; ii < nbitems; zoom ++, ii++ )
	{
		m_ZoomList[ii] = *zoom;
	}
}

/***********************************/
void BASE_SCREEN::SetFirstZoom(void)
/***********************************/
/* ajuste le coeff de zoom a 1*/
{
	m_Zoom = 1;
}


/****************************/
int BASE_SCREEN::GetZoom(void)
/****************************/
/* retourne le coeff de zoom */
{
	return m_Zoom;
}

/***********************************/
void BASE_SCREEN::SetZoom(int coeff)
/***********************************/
/* ajuste le coeff de zoom a coeff */
{
	m_Zoom = coeff;
	if ( m_Zoom < 1 ) m_Zoom = 1;
}


/********************************/
void BASE_SCREEN::SetNextZoom(void)
/********************************/
/* Selectionne le prochain coeff de zoom
*/
{
	m_Zoom *= 2;

	if ( m_ZoomList == NULL ) return;

int ii, zoom_max = 512;
	for ( ii = 0; m_ZoomList[ii] != 0; ii ++ ) zoom_max = m_ZoomList[ii];
	if ( m_Zoom > zoom_max ) m_Zoom = zoom_max;
}


/*************************************/
void BASE_SCREEN::SetPreviousZoom(void)
/*************************************/
/* Selectionne le precedent coeff de zoom
*/
{
	m_Zoom /= 2;
	if ( m_Zoom < 1 ) m_Zoom = 1;
}


/**********************************/
void BASE_SCREEN::SetLastZoom(void)
/**********************************/
/* ajuste le coeff de zoom au max
*/
{
	if ( m_ZoomList == NULL ) return;
int ii;
	for ( ii = 0; m_ZoomList[ii] != 0; ii ++ ) m_Zoom = m_ZoomList[ii];
}


/********************************************/
void BASE_SCREEN::SetGridList(wxSize * gridlist)
/********************************************/
/* init liste des zoom (NULL terminated)
*/
{
int ii, nbitems;
wxSize * grid;
	// Decompte des items
	for ( nbitems = 0, grid = gridlist;  ; grid ++, nbitems++ )
	{
		if ( (grid->x <= 0) || (grid->y <= 0) ) break;
	}
	
	// Init liste
	if ( m_GridList ) free(m_GridList);
	m_GridList = (wxSize*) MyZMalloc( nbitems * sizeof (wxSize) );
	
	for ( ii = 0, grid = gridlist; ii < nbitems; grid ++, ii++ )
	{
		m_GridList[ii] = *grid;
	}
}

/**********************************************/
void BASE_SCREEN::SetGrid( const wxSize & size )
/**********************************************/
{
	if ( m_GridList == NULL ) return;
		
	if ( (size.x <= 0) || (size.y <= 0) )
	{
		m_UserGrid = g_UserGrid;
		m_UserGridIsON = TRUE;
	}
	else
	{
		m_Grid = size;
		m_UserGridIsON = FALSE;
	}
}

/*********************************/
wxSize BASE_SCREEN::GetGrid(void)
/*********************************/
{
wxSize grid = m_Grid;
double xx, scale;
	
	if ( m_GridList == NULL ) return wxSize(1,1);

	if ( m_UserGridIsON || m_Grid.x < 0 || m_Grid.y < 0 )
	{
		if ( m_UserGridUnit == INCHES ) scale = 10000;
		else scale = 10000/25.4;
		xx = m_UserGrid.x * scale;
		grid.x = (int)(xx + 0.5);
		xx = m_UserGrid.y * scale;
		grid.y = (int)(xx + 0.5);
	}
	return grid;
}


/*********************************/
void BASE_SCREEN::SetNextGrid(void)
/*********************************/
/* Selectionne la prochaine grille
*/
{
int ii;

	if ( m_GridList == NULL ) return;

	for ( ii = 0; ; ii++ )
		{
		if ( m_GridList[ii].x <= 0 ) break;
		if ( (m_Grid.x == m_GridList[ii].x) && (m_Grid.y == m_GridList[ii].y) )
			break;
		}
	if ( (m_GridList[ii].x > 0) && (ii > 0) )
		m_Grid = m_GridList[ii - 1];
}


/*************************************/
void BASE_SCREEN::SetPreviousGrid(void)
/*************************************/
/* Selectionne le precedent coeff de grille
*/
{
int ii;

	if ( m_GridList == NULL ) return;

	for ( ii = 0; ; ii++ )
		{
		if ( m_GridList[ii].x <= 0 ) break;
		if ( (m_Grid.x == m_GridList[ii].x) && (m_Grid.y == m_GridList[ii].y) )
			break;
		}
	if ( (m_GridList[ii].x > 0) && (m_GridList[ii+1].x > 0) )
		m_Grid = m_GridList[ii + 1];
}


/**********************************/
void BASE_SCREEN::SetFirstGrid(void)
/**********************************/
/* ajuste le coeff de grille a 1
*/
{
	if ( m_GridList == NULL ) return;

int ii = 0;
	while ( m_GridList[ii].x > 0 ) ii ++;
	m_Grid = m_GridList[ii-1];
}



/**********************************/
void BASE_SCREEN::SetLastGrid(void)
/**********************************/
/* ajuste le coeff de grille au max
*/
{
	if ( m_GridList == NULL ) return;
	m_Grid = m_GridList[0];
}


/*******************************************************************/
void BASE_SCREEN::Trace_Curseur(WinEDA_DrawPanel * panel, wxDC * DC)
/*******************************************************************/
/*
 Trace Le curseur sur la zone PCB , se deplacant sur la grille
*/
{
int color = WHITE;

	if (panel->m_CursorLevel != 0) {
		return;
	}
	
	GRSetDrawMode(DC, GR_XOR);
	if( g_CursorShape == 1 )	/* Trace d'un reticule */
		{
		int dx = panel->m_ClipBox.GetWidth() * GetZoom();
		int dy = panel->m_ClipBox.GetHeight() * GetZoom();
		GRLine(&panel->m_ClipBox, DC, m_Curseur.x - dx, m_Curseur.y,
							m_Curseur.x + dx, m_Curseur.y, color); // axe Y
		GRLine(&panel->m_ClipBox, DC, m_Curseur.x, m_Curseur.y - dx,
				m_Curseur.x, m_Curseur.y + dy, color);  // axe X
		}

	else
		{
		int len = CURSOR_SIZE * GetZoom();
		GRLine(&panel->m_ClipBox, DC, m_Curseur.x - len, m_Curseur.y,
				m_Curseur.x + len, m_Curseur.y, color);
		GRLine(&panel->m_ClipBox, DC, m_Curseur.x, m_Curseur.y - len,
				m_Curseur.x, m_Curseur.y + len, color);
		}
}

/*******************************************************************/
void BASE_SCREEN::CursorOff(WinEDA_DrawPanel * panel, wxDC * DC)
/*******************************************************************/
/*
 Remove the grid cursor from the display in preparation for other drawing operations
*/
{
	Trace_Curseur(panel, DC);
	--panel->m_CursorLevel;
}

/*******************************************************************/
void BASE_SCREEN::CursorOn(WinEDA_DrawPanel * panel, wxDC * DC)
/*******************************************************************/
/*
 Display the grid cursor
*/
{
	++panel->m_CursorLevel;
	Trace_Curseur(panel, DC);

	if (panel->m_CursorLevel > 0)     // Shouldn't happen, but just in case ..
		panel->m_CursorLevel = 0;
}



/*****************************************/
void BASE_SCREEN::ClearUndoRedoList(void)
/*****************************************/
/* free the undo and the redo lists
*/
{
EDA_BaseStruct *nextitem;
	
	while ( m_UndoList )
	{
		nextitem = m_UndoList->Pnext;
		delete m_UndoList;
		m_UndoList = nextitem;
	}
	
	while ( m_RedoList )
	{
		nextitem = m_RedoList->Pnext;
		delete m_RedoList;
		m_RedoList = nextitem;
	}
	
}

/***********************************************************/
void BASE_SCREEN::AddItemToUndoList(EDA_BaseStruct * newitem)
/************************************************************/
/* Put newitem in head of undo list
	Deletes olds items if > count max.
*/
{
int ii;
EDA_BaseStruct * item, *nextitem;
	
	if ( newitem == NULL ) return;
		
	newitem->Pnext = m_UndoList;
	m_UndoList = newitem;

	/* Free first items, if count max reached */
	for ( ii = 0, item = m_UndoList; ii < m_UndoRedoCountMax; ii++ )
	{
		if ( item->Pnext == NULL ) return;
		item = item->Pnext;
	}
	
	if ( item == NULL ) return;
	
	nextitem = item->Pnext;
	item->Pnext = NULL;	// Set end of chain
	
	// Delete the extra  items
	for(item = nextitem ; item != NULL; item = nextitem )
	{
		nextitem = item->Pnext;
		delete item;
	}
	
}

/***********************************************************/
void BASE_SCREEN::AddItemToRedoList(EDA_BaseStruct * newitem)
/***********************************************************/
{
int ii;
EDA_BaseStruct * item, *nextitem;
	
	if ( newitem == NULL ) return;
		
	newitem->Pnext = m_RedoList;
	m_RedoList = newitem;
	/* Free first items, if count max reached */
	for ( ii = 0, item = m_RedoList; ii < m_UndoRedoCountMax; ii++ )
	{
		if ( item->Pnext == NULL ) break;
		item = item->Pnext;
	}
	
	if ( item == NULL ) return;
	
	nextitem = item->Pnext;
	item->Pnext = NULL;	// Set end of chain
	
	// Delete the extra items
	for(item = nextitem ; item != NULL; item = nextitem )
	{
		nextitem = item->Pnext;
		delete item;
	}
	
}

/*****************************************************/
EDA_BaseStruct * BASE_SCREEN::GetItemFromUndoList(void)
/*****************************************************/
{
EDA_BaseStruct * item = m_UndoList;

	if ( item ) m_UndoList = item->Pnext;
	return item;
}

/******************************************************/
EDA_BaseStruct * BASE_SCREEN::GetItemFromRedoList(void)
/******************************************************/
{
EDA_BaseStruct * item = m_RedoList;
	
	if ( item ) m_RedoList = item->Pnext;
	return item;
}

