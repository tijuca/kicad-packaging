/*********************************************************/
/* Modules de creations de Traits, Wires, Bus, Junctions */
/*********************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"


/* Routines Locales */
static void Polyline_in_Ghost(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);
static void Segment_in_Ghost(WinEDA_DrawPanel * panel, wxDC * DC, bool erase);
static void AbortCreateNewLine(WinEDA_DrawFrame * frame, wxDC * DC);
static bool IsTerminalPoint(SCH_SCREEN * screen, const wxPoint & pos, int layer );
static bool IsJunctionNeeded (WinEDA_SchematicFrame * frame, wxPoint & pos );
static void ComputeBreakPoint(EDA_DrawLineStruct * segment, const wxPoint & new_pos);


wxPoint s_ConnexionStartPoint;

/*************************************************************/
void WinEDA_SchematicFrame::BeginSegment(wxDC * DC, int type)
/*************************************************************/
/* Create a new segment ( WIRE, BUS ).
*/
{
EDA_DrawLineStruct * oldsegment, * newsegment, * nextsegment;
wxPoint cursorpos = GetScreen()->m_Curseur;
	
	if ( GetScreen()->m_CurrentItem && (GetScreen()->m_CurrentItem->m_Flags == 0) )
		GetScreen()->m_CurrentItem = NULL;

	if ( GetScreen()->m_CurrentItem )
	{
		switch (GetScreen()->m_CurrentItem->m_StructType )
		{
			case DRAW_SEGMENT_STRUCT_TYPE:
			case DRAW_POLYLINE_STRUCT_TYPE:
				break;

			default:
				return;
		}
	}

	oldsegment = newsegment =
			(EDA_DrawLineStruct *) GetScreen()->m_CurrentItem;

	if (!newsegment)  /* first point : Create first wire ou bus */
	{
		s_ConnexionStartPoint = cursorpos;
		SchematicCleanUp(NULL);
		switch(type)
		{
			default:
				newsegment = new EDA_DrawLineStruct(cursorpos, LAYER_NOTES);
				break;
			case LAYER_WIRE:
				newsegment = new EDA_DrawLineStruct(cursorpos, LAYER_WIRE);
				/* A junction will be created later, when w'll know the
				segment end position, and if the junction is really needed */
				break;
			case LAYER_BUS:
				newsegment = new EDA_DrawLineStruct(cursorpos, LAYER_BUS);
				break;
		}

		newsegment->m_Flags = IS_NEW;
		if( g_HVLines )	// We need 2 segments to go from a given start pint to an end point
		{
			nextsegment = newsegment->GenCopy();
			nextsegment->m_Flags = IS_NEW;
			newsegment->Pnext = nextsegment;
			nextsegment->Pback = newsegment;
		}
		GetScreen()->m_CurrentItem = newsegment;
		GetScreen()->ManageCurseur = Segment_in_Ghost;
		GetScreen()->ForceCloseManageCurseur = AbortCreateNewLine;
		g_ItemToRepeat = NULL;
	}

	else	/* Trace en cours: Placement d'un point supplementaire */
	{
		nextsegment = (EDA_DrawLineStruct*)oldsegment->Pnext;
		if( ! g_HVLines )
		{ /* if only one segment is needed and the current is has len = 0, do not create a new one*/
			if( oldsegment->IsNull() ) return;
		}
		else
		{ 	/* if we want 2 segment and the last two have len = 0, do not create a new one*/
			if ( oldsegment->IsNull() && nextsegment && nextsegment->IsNull() )
				return;
		}

		GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);

		/* Creation du segment suivant ou fin de tracé si point sur pin, jonction ...*/
		if ( IsTerminalPoint(GetScreen(), cursorpos, oldsegment->m_Layer) )
		{
			EndSegment(DC); return;
		}

		/* Placement en liste generale */
		oldsegment->Pnext = GetScreen()->EEDrawList;
		GetScreen()->EEDrawList = oldsegment;
		GetScreen()->CursorOff(DrawPanel, DC);	// Erase schematic cursor
		RedrawOneStruct(DrawPanel,DC, oldsegment, GR_DEFAULT_DRAWMODE);
		GetScreen()->CursorOn(DrawPanel, DC);	// Display schematic cursor

		/* Create a new segment, and chain it after the current new segment */
		if ( nextsegment ) 
		{
			newsegment = nextsegment->GenCopy();
			nextsegment->m_Start = newsegment->m_End;
			nextsegment->Pnext = NULL;
			nextsegment->Pback = newsegment;
			newsegment->Pnext = nextsegment;
			newsegment->Pback = NULL;
		}
		else
		{
			newsegment = oldsegment->GenCopy();
			newsegment->m_Start = oldsegment->m_End;
		}
		newsegment->m_End = cursorpos;
		oldsegment->m_Flags = SELECTED;
		newsegment->m_Flags = IS_NEW;
		GetScreen()->m_CurrentItem = newsegment;
		GetScreen()->ManageCurseur(DrawPanel, DC, FALSE);
		if ( oldsegment->m_Start == s_ConnexionStartPoint )
		{	/* This is the first segment: Now we know the start segment position.
			Create a junction if needed. Note: a junction can be needed
			later, if the new segment is merged (after a cleanup) with an older one 
			(tested when the connection will be finished)*/
			if ( IsJunctionNeeded(this, s_ConnexionStartPoint) )
				CreateNewJunctionStruct(DC, s_ConnexionStartPoint);
		}
	}
}


/***********************************************/
void WinEDA_SchematicFrame::EndSegment(wxDC *DC)
/***********************************************/
/* Called to terminate a bus, wire, or line creation
*/
{
EDA_DrawLineStruct * firstsegment = (EDA_DrawLineStruct *)GetScreen()->m_CurrentItem;
EDA_DrawLineStruct * lastsegment = firstsegment;
EDA_DrawLineStruct * segment;
	
	if ( firstsegment == NULL ) return;
	if ( (firstsegment->m_Flags & IS_NEW) == 0) return;

	/* Delete Null segments and Put line it in Drawlist */
	lastsegment = firstsegment;
	while ( lastsegment )
	{
		EDA_DrawLineStruct * nextsegment = (EDA_DrawLineStruct *)lastsegment->Pnext;
		if ( lastsegment->IsNull() )
		{
		EDA_DrawLineStruct * previous_segment = (EDA_DrawLineStruct *)lastsegment->Pback;
			if ( firstsegment == lastsegment ) firstsegment = nextsegment;
			if ( nextsegment ) nextsegment->Pback = NULL;
			if ( previous_segment ) previous_segment->Pnext = nextsegment;
			delete lastsegment;
		}
		lastsegment = nextsegment;
	}
	/* put the segment list to the main linked list */
	segment = lastsegment = firstsegment;
	while ( segment )
	{
		lastsegment = segment;
		segment = (EDA_DrawLineStruct *)segment->Pnext;
		lastsegment->Pnext = GetScreen()->EEDrawList;
		GetScreen()->EEDrawList = lastsegment;
	}
	
	/* Fin de trace */
	GetScreen()->ManageCurseur = NULL;
	GetScreen()->ForceCloseManageCurseur = NULL;
	GetScreen()->m_CurrentItem = NULL;

wxPoint end_point, alt_end_point;
	/* A junction can be needed to connect the last segment
	usually to m_End coordinate.
	But if the last segment is removed by a cleanup, because od redundancy,
	a junction can be needed to connect the previous segment m_End coordinate
	with is also the lastsegment->m_Start coordinate */
	if ( lastsegment )
	{
		end_point = lastsegment->m_End;
		alt_end_point = lastsegment->m_Start;
	}

	SchematicCleanUp(NULL);

	/* clear flags and find last segment entered, for repeat function */
	segment = (EDA_DrawLineStruct *) GetScreen()->EEDrawList;
	while ( segment )
	{
		if ( segment->m_Flags )
		{
			if ( ! g_ItemToRepeat ) g_ItemToRepeat = segment;
		}
		segment->m_Flags = 0;
		segment = (EDA_DrawLineStruct *)segment->Pnext;
	}

	// Automatic place of a junction on the end point, if needed
	if ( lastsegment )
	{
		if( IsJunctionNeeded(this, end_point) )
			CreateNewJunctionStruct(DC, end_point);

		else if( IsJunctionNeeded(this, alt_end_point) )
			CreateNewJunctionStruct(DC, alt_end_point);
	}
	
	/* Automatic place of a junction on the start point if necessary because the 
	Cleanup can suppress intermediate points by merging wire segments*/
	if ( IsJunctionNeeded(this, s_ConnexionStartPoint) )
		CreateNewJunctionStruct(DC, s_ConnexionStartPoint);

	TestDanglingEnds(GetScreen()->EEDrawList, DC);

	
	/* Redraw wires and junctions which can be changed by TestDanglingEnds() */
	GetScreen()->CursorOff(DrawPanel, DC);	// Erase schematic cursor
	EDA_BaseStruct *item = GetScreen()->EEDrawList;
	while ( item )
	{
		switch ( item->m_StructType )
		{
			case DRAW_JUNCTION_STRUCT_TYPE:
			case DRAW_SEGMENT_STRUCT_TYPE:
				RedrawOneStruct(DrawPanel,DC, item, GR_DEFAULT_DRAWMODE);
				break;
				
			default:
				break;
		}
		item = item->Pnext;
	}
	

	GetScreen()->CursorOn(DrawPanel, DC);	// Display schematic cursor

	SetFlagModify(GetScreen());
}

/****************************************************************************/
static void Segment_in_Ghost(WinEDA_DrawPanel * panel, wxDC * DC, bool erase)
/****************************************************************************/
/*  Redraw the segment (g_HVLines == FALSE ) or the two segments (g_HVLines == TRUE )
	from the start point to the cursor, when moving the mouse
*/
{
EDA_DrawLineStruct * CurrentLine =
		(EDA_DrawLineStruct *) panel->m_Parent->GetScreen()->m_CurrentItem;
EDA_DrawLineStruct * segment;
int color;

	if ( CurrentLine == NULL ) return;

	color = ReturnLayerColor(CurrentLine->m_Layer) ^ HIGHT_LIGHT_FLAG;

	if( erase )		// Redraw if segment lengtht != 0
	{
		segment = CurrentLine;
		while ( segment )
		{
			if ( ! segment->IsNull() )
				RedrawOneStruct(panel,DC, segment, g_XorMode, color);
			segment = (EDA_DrawLineStruct*)segment->Pnext;
		}
	}

wxPoint endpos = panel->m_Parent->GetScreen()->m_Curseur;
	if( g_HVLines )	/* Coerce the line to vertical or horizontal one: */
	{
		 ComputeBreakPoint( CurrentLine, endpos);
	}

	else CurrentLine->m_End = endpos;
		
	// Redraw if segment lengtht != 0
	segment = CurrentLine;
	while ( segment )
	{
		if ( ! segment->IsNull() )
			RedrawOneStruct(panel,DC, segment, g_XorMode, color);
		segment = (EDA_DrawLineStruct*)segment->Pnext;
	}
}


/**************************************************************************************/
static void ComputeBreakPoint( EDA_DrawLineStruct * segment, const wxPoint & new_pos )
/**************************************************************************************/
/* compute the middle coordinate for 2 segments, from the start point to new_pos
	with the 2 segments kept H or V only
*/
{
EDA_DrawLineStruct  * nextsegment = (EDA_DrawLineStruct *)segment->Pnext;
wxPoint middle_position = new_pos;
	
	if( nextsegment == NULL ) return;
#if 0		
	if (ABS(middle_position.x - segment->m_Start.x) < ABS(middle_position.y - segment->m_Start.y))
		middle_position.x = segment->m_Start.x;
	else
		middle_position.y = segment->m_Start.y;
#else
int iDx = segment->m_End.x - segment->m_Start.x;
int iDy = segment->m_End.y - segment->m_Start.y;
	if ( iDy != 0 )			// keep the first segment orientation (currently horizontal)
	{
		middle_position.x = segment->m_Start.x;  
	}
	else if ( iDx != 0 )	// keep the first segment orientation (currently vertical)
	{
		middle_position.y = segment->m_Start.y;  
	}
	else
	{
		if (ABS(middle_position.x - segment->m_Start.x) < ABS(middle_position.y - segment->m_Start.y))
			middle_position.x = segment->m_Start.x;
		else
			middle_position.y = segment->m_Start.y;
	}
#endif
	
	segment->m_End = middle_position;
	
	nextsegment->m_Start = middle_position;
	nextsegment->m_End = new_pos;
}

/*****************************************************************************/
static void Polyline_in_Ghost(WinEDA_DrawPanel * panel, wxDC * DC, bool erase)
/*****************************************************************************/
/*  Dessin du du Polyline Fantome lors des deplacements du curseur
*/
{
DrawPolylineStruct * NewPoly =
		(DrawPolylineStruct *)panel->m_Parent->GetScreen()->m_CurrentItem;
int color;
wxPoint endpos;

	endpos = panel->m_Parent->GetScreen()->m_Curseur;
	color = ReturnLayerColor(NewPoly->m_Layer);

	GRSetDrawMode(DC, g_XorMode);

	if( g_HVLines )
		{
		/* Coerce the line to vertical or horizontal one: */
		if (ABS(endpos.x - NewPoly->m_Points[NewPoly->m_NumOfPoints * 2 - 2]) <
			 ABS(endpos.y - NewPoly->m_Points[NewPoly->m_NumOfPoints * 2 - 1]))
			endpos.x = NewPoly->m_Points[NewPoly->m_NumOfPoints * 2 - 2];
		else
			endpos.y = NewPoly->m_Points[NewPoly->m_NumOfPoints * 2 - 1];
		}

	NewPoly->m_NumOfPoints++;
	if( erase )
		RedrawOneStruct(panel,DC, NewPoly, g_XorMode, color);

	NewPoly->m_Points[NewPoly->m_NumOfPoints * 2 - 2] = endpos.x;
	NewPoly->m_Points[NewPoly->m_NumOfPoints * 2 - 1] = endpos.y;
	RedrawOneStruct(panel,DC, NewPoly, g_XorMode, color);
	NewPoly->m_NumOfPoints--;
}

/**********************************************************/
void WinEDA_SchematicFrame::DeleteCurrentSegment(wxDC * DC)
/**********************************************************/
/*
Routine effacant le dernier trait trace, ou l'element pointe par la souris
*/
{

	g_ItemToRepeat = NULL;

	if( (GetScreen()->m_CurrentItem == NULL) ||
		((GetScreen()->m_CurrentItem->m_Flags & IS_NEW) == 0) )
	{
		return;
	}

	/* Trace en cours: annulation */
	if (GetScreen()->m_CurrentItem->m_StructType == DRAW_POLYLINE_STRUCT_TYPE)
	{
		Polyline_in_Ghost(DrawPanel, DC, FALSE); /* Effacement du trace en cours */
	}

	else
	{
		Segment_in_Ghost(DrawPanel, DC, FALSE); /* Effacement du trace en cours */
	}

	EraseStruct(GetScreen()->m_CurrentItem, GetScreen());
	GetScreen()->ManageCurseur = NULL;
	GetScreen()->m_CurrentItem = NULL;
}


/***************************************************************************/
DrawJunctionStruct * WinEDA_SchematicFrame::CreateNewJunctionStruct(
		wxDC * DC, const wxPoint & pos)
/***************************************************************************/
/* Routine to create new connection struct.
*/
{
DrawJunctionStruct *NewConnect;

	NewConnect = new DrawJunctionStruct(pos);

	g_ItemToRepeat = NewConnect;

	GetScreen()->CursorOff(DrawPanel, DC);	// Erase schematic cursor
	RedrawOneStruct(DrawPanel,DC, NewConnect, GR_DEFAULT_DRAWMODE);
	GetScreen()->CursorOn(DrawPanel, DC);	// Display schematic cursor

	NewConnect->Pnext = GetScreen()->EEDrawList;
	GetScreen()->EEDrawList = NewConnect;
	SetFlagModify(GetScreen());
	return(NewConnect);
}

/*************************************************************************/
EDA_BaseStruct *WinEDA_SchematicFrame::CreateNewNoConnectStruct(wxDC * DC)
/*************************************************************************/
/*Routine to create new NoConnect struct. ( Symbole de Non Connexion)
*/
{
DrawNoConnectStruct *NewNoConnect;

	NewNoConnect = new DrawNoConnectStruct(GetScreen()->m_Curseur);
	g_ItemToRepeat = NewNoConnect;

	GetScreen()->CursorOff(DrawPanel, DC);	// Erase schematic cursor
	RedrawOneStruct(DrawPanel,DC, NewNoConnect,  GR_DEFAULT_DRAWMODE);
	GetScreen()->CursorOn(DrawPanel, DC);	// Display schematic cursor

	NewNoConnect->Pnext = GetScreen()->EEDrawList;
	GetScreen()->EEDrawList = NewNoConnect;
	SetFlagModify(GetScreen());
	return(NewNoConnect);
}


/*****************************************************************/
static void AbortCreateNewLine(WinEDA_DrawFrame * frame, wxDC * DC)
/*****************************************************************/
/* Abort function for wire, bus or line creation
*/
{
BASE_SCREEN * Screen = frame->GetScreen();

	if( Screen->m_CurrentItem)  /* trace en cours */
	{
		Screen->ManageCurseur(frame->DrawPanel, DC, FALSE);
		Screen->ManageCurseur = NULL;
		Screen->ForceCloseManageCurseur = NULL;
		EraseStruct(Screen->m_CurrentItem,(SCH_SCREEN*) Screen);
		Screen->m_CurrentItem = NULL;
	}

	else g_ItemToRepeat = NULL;	// Fin de commande generale
		
	/* Clear m_Flags wich is used in edit functions: */
	EDA_BaseStruct * item = Screen->EEDrawList;
	while ( item )
	{
		item->m_Flags = 0;
		item = item->Pnext;
	}
}


/***************************************************/
void WinEDA_SchematicFrame::RepeatDrawItem(wxDC *DC)
/***************************************************/
/* Routine de recopie du dernier element dessine
	Les elements duplicables sont
		fils, bus, traits, textes, labels
		Les labels termines par un nombre seront incrementes 
*/
{
int ox = 0, oy = 0;

	if( g_ItemToRepeat == NULL ) return;

	switch( g_ItemToRepeat->m_StructType )
		{
		case DRAW_JUNCTION_STRUCT_TYPE:
			#undef STRUCT
			#define STRUCT ((DrawJunctionStruct*) g_ItemToRepeat)
			g_ItemToRepeat = STRUCT->GenCopy();
			STRUCT->m_Pos.x += g_RepeatStep.x; ox = STRUCT->m_Pos.x;
			STRUCT->m_Pos.y += g_RepeatStep.y; oy = STRUCT->m_Pos.y;
			break;

		case DRAW_NOCONNECT_STRUCT_TYPE:
			#undef STRUCT
			#define STRUCT ((DrawNoConnectStruct*) g_ItemToRepeat)
			g_ItemToRepeat = STRUCT->GenCopy();
			STRUCT->m_Pos.x += g_RepeatStep.x; ox = STRUCT->m_Pos.x;
			STRUCT->m_Pos.y += g_RepeatStep.y; oy = STRUCT->m_Pos.y;
			break;

		case DRAW_TEXT_STRUCT_TYPE:
			#undef STRUCT
			#define STRUCT ((DrawTextStruct*) g_ItemToRepeat)
			g_ItemToRepeat = STRUCT->GenCopy();
			STRUCT->m_Pos.x += g_RepeatStep.x; ox = STRUCT->m_Pos.x;
			STRUCT->m_Pos.y += g_RepeatStep.y; oy = STRUCT->m_Pos.y;
			/*** Increment du numero de label ***/
			IncrementLabelMember(STRUCT->m_Text);
			break;

 
		case DRAW_LABEL_STRUCT_TYPE:
			#undef STRUCT
			#define STRUCT ((DrawLabelStruct*) g_ItemToRepeat)
			g_ItemToRepeat = STRUCT->GenCopy();
			STRUCT->m_Pos.x += g_RepeatStep.x; ox = STRUCT->m_Pos.x;
			STRUCT->m_Pos.y += g_RepeatStep.y; oy = STRUCT->m_Pos.y;
			/*** Increment du numero de label ***/
			IncrementLabelMember(STRUCT->m_Text);
			break;


		case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
			#undef STRUCT
			#define STRUCT ((DrawGlobalLabelStruct*) g_ItemToRepeat)
			g_ItemToRepeat = STRUCT->GenCopy();
			STRUCT->m_Pos.x += g_RepeatStep.x; ox = STRUCT->m_Pos.x;
			STRUCT->m_Pos.y += g_RepeatStep.y; oy = STRUCT->m_Pos.y;
			/*** Increment du numero de label ***/
			IncrementLabelMember(STRUCT->m_Text);
			break;

		case DRAW_SEGMENT_STRUCT_TYPE:
			#undef STRUCT
			#define STRUCT ((EDA_DrawLineStruct*) g_ItemToRepeat)
			g_ItemToRepeat = STRUCT->GenCopy();
			STRUCT->m_Start.x += g_RepeatStep.x; ox = STRUCT->m_Start.x;
			STRUCT->m_Start.y += g_RepeatStep.y; oy = STRUCT->m_Start.y;
			STRUCT->m_End.x += g_RepeatStep.x;
			STRUCT->m_End.y += g_RepeatStep.y;
			break;

		case DRAW_BUSENTRY_STRUCT_TYPE:
			#undef STRUCT
			#define STRUCT ((DrawBusEntryStruct*) g_ItemToRepeat)
			g_ItemToRepeat = STRUCT->GenCopy();
			STRUCT->m_Pos.x += g_RepeatStep.x; ox = STRUCT->m_Pos.x;
			STRUCT->m_Pos.y += g_RepeatStep.y; oy = STRUCT->m_Pos.y;
			break;

		default:
			g_ItemToRepeat = NULL;
			DisplayError(this, wxT("Repeat Type Error"), 10);
			break;
		}

	if ( g_ItemToRepeat )
		{
		g_ItemToRepeat->Pnext = GetScreen()->EEDrawList;
		GetScreen()->EEDrawList = g_ItemToRepeat;
		TestDanglingEnds(GetScreen()->EEDrawList, NULL);
		RedrawOneStruct(DrawPanel,DC, g_ItemToRepeat, GR_DEFAULT_DRAWMODE);
//		GetScreen()->Curseur.x = ox; GetScreen()->Curseur.x = oy; 
//		GRMouseWarp(DrawPanel, DrawPanel->CursorScreenPosition() );
		}
}


/******************************************/
void IncrementLabelMember(wxString & name)
/******************************************/
/* Routine incrementant les labels, c'est a dire pour les textes finissant
par un nombre, ajoutant <RepeatDeltaLabel> a ce nombre
*/
{
int ii, nn;
long number = 0;
	
 	ii = name.Len() - 1; nn = 0;
	if( !isdigit(name.GetChar(ii)) ) return;

	while( (ii >= 0) && isdigit(name.GetChar(ii)) ) { ii--; nn++ ; }
	ii++;	/* digits are starting at ii position */
wxString litt_number = name.Right(nn);
	if ( litt_number.ToLong(&number) )
	{
		number += g_RepeatDeltaLabel;
		name.Remove(ii); name << number;
	}
}

/***************************************************************************/
static bool IsTerminalPoint(SCH_SCREEN * screen, const wxPoint & pos, int layer)
/***************************************************************************/
/* Returne TRUE si pos est un point possible pour terminer automatiquement un
segment, c'est a dire pour
	- type WIRE, si il y a
		- une jonction
		- ou une pin
		- ou une extrémité unique de fil

	- type BUS, si il y a
		- ou une extrémité unique de BUS
*/
{
EDA_BaseStruct * item;
LibDrawPin * pin;
EDA_SchComponentStruct * LibItem = NULL;
DrawSheetLabelStruct * pinsheet;
wxPoint itempos;

	switch ( layer )
		{
		case LAYER_BUS:
			item = PickStruct(pos, screen->EEDrawList, BUSITEM);
			if ( item ) return TRUE;
			pinsheet = LocateAnyPinSheet(pos, screen->EEDrawList );
			if ( pinsheet && IsBusLabel(pinsheet->m_Text) )
			{
				itempos = pinsheet->m_Pos;
				if ( (itempos.x == pos.x) && (itempos.y == pos.y) )	return TRUE;
			}
			break;

		case LAYER_NOTES:
			item = PickStruct(pos, screen->EEDrawList, DRAWITEM);
			if ( item )
				return TRUE;
			break;

		case LAYER_WIRE:
			item = PickStruct(pos, screen->EEDrawList, RACCORDITEM |JUNCTIONITEM);
			if ( item ) return TRUE;

			pin = LocateAnyPin( screen->EEDrawList, pos, &LibItem );
			if ( pin && LibItem )
				{
				// calcul de la position exacte du point de connexion de la pin,
				// selon orientation du composant:
				itempos = LibItem->GetScreenCoord(pin->m_Pos);
				itempos.x += LibItem->m_Pos.x;
				itempos.y += LibItem->m_Pos.y;
				if ( (itempos.x == pos.x) && (itempos.y == pos.y) )	return TRUE;
				}

			item = PickStruct(pos, screen->EEDrawList, WIREITEM);
			if ( item ) return TRUE;

			item = PickStruct(pos, screen->EEDrawList, LABELITEM);
			if ( item && (item->m_StructType != DRAW_TEXT_STRUCT_TYPE) &&
				( ((DrawGlobalLabelStruct*)item)->m_Pos.x == pos.x) &&
				( ((DrawGlobalLabelStruct*)item)->m_Pos.y == pos.y) )
					return TRUE;

			pinsheet = LocateAnyPinSheet( pos, screen->EEDrawList );
			if ( pinsheet && ! IsBusLabel(pinsheet->m_Text) )
			{
				itempos = pinsheet->m_Pos;
				if ( (itempos.x == pos.x) && (itempos.y == pos.y) )	return TRUE;
			}

			break;

		default:
			break;
		}

	return FALSE;
}


/****************************************************************/
bool IsJunctionNeeded (WinEDA_SchematicFrame * frame, wxPoint & pos )
/****************************************************************/
/* Return True when a wire is located at pos "pos" if
	- there is no junction.
	- The wire has no ends at pos "pos",
		and therefore it is considered as no connected.
	- One (or more) wire has one end at pos "pos"
	or
	- a pin is on location pos
*/
{
	if ( PickStruct(pos,frame->GetScreen()->EEDrawList, JUNCTIONITEM ) ) return FALSE;
	
	if ( PickStruct(pos,frame->GetScreen()->EEDrawList, WIREITEM |EXCLUDE_WIRE_BUS_ENDPOINTS ) )
	{
		if ( PickStruct(pos,frame->GetScreen()->EEDrawList, WIREITEM |WIRE_BUS_ENDPOINTS_ONLY ) )
			return TRUE;
		if ( frame->LocatePinEnd(frame->GetScreen()->EEDrawList, pos) )
			return TRUE;
	}

	return FALSE;
}

