/***********************************/
/* Edit segments and edges of PCB. */
/***********************************/

#include "fctsys.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"

#include "pcbnew.h"
#include "wxPcbStruct.h"
#include "class_board_design_settings.h"

#include "protos.h"


static void Exit_EditEdge( WinEDA_DrawPanel* Panel, wxDC* DC );
static void Montre_Position_NewSegment( WinEDA_DrawPanel* panel, wxDC* DC, bool erase );
static void Move_Segment( WinEDA_DrawPanel* panel, wxDC* DC, bool erase );


static wxPoint s_InitialPosition;  // Initial cursor position.
static wxPoint s_LastPosition;     // Current cursor position.


/* Start move of a graphic element type DRAWSEGMENT */
void WinEDA_PcbFrame::Start_Move_DrawItem( DRAWSEGMENT* drawitem, wxDC* DC )
{
    if( drawitem == NULL )
        return;
    drawitem->Draw( DrawPanel, DC, GR_XOR );
    drawitem->m_Flags |= IS_MOVED;
    s_InitialPosition = s_LastPosition = GetScreen()->m_Curseur;
    drawitem->DisplayInfo( this );
    DrawPanel->ManageCurseur = Move_Segment;
    DrawPanel->ForceCloseManageCurseur = Exit_EditEdge;
    SetCurItem( drawitem );
    DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
}


/*
 * Place graphic element of type DRAWSEGMENT.
 */
void WinEDA_PcbFrame::Place_DrawItem( DRAWSEGMENT* drawitem, wxDC* DC )
{
    if( drawitem == NULL )
        return;

    SaveCopyInUndoList(drawitem, UR_MOVED, s_LastPosition - s_InitialPosition);
    drawitem->Draw( DrawPanel, DC, GR_OR );
    DrawPanel->ManageCurseur = NULL;
    DrawPanel->ForceCloseManageCurseur = NULL;
    SetCurItem( NULL );
    OnModify();
    drawitem->m_Flags = 0;
}

/*
 * Redraw segment during cursor movement.
 */
static void Move_Segment( WinEDA_DrawPanel* panel, wxDC* DC, bool erase )
{
    DRAWSEGMENT* Segment = (DRAWSEGMENT*) panel->GetScreen()->GetCurItem();
    int          t_fill = DisplayOpt.DisplayDrawItems;

    if( Segment == NULL )
        return;

    DisplayOpt.DisplayDrawItems = SKETCH;

    if( erase )
        Segment->Draw( panel, DC, GR_XOR );

    wxPoint delta;
    delta = panel->GetScreen()->m_Curseur - s_LastPosition;
    Segment->m_Start += delta;
    Segment->m_End   += delta;
    s_LastPosition = panel->GetScreen()->m_Curseur;

    Segment->Draw( panel, DC, GR_XOR );
    DisplayOpt.DisplayDrawItems = t_fill;
}


void WinEDA_PcbFrame::Delete_Segment_Edge( DRAWSEGMENT* Segment, wxDC* DC )
{
    EDA_BaseStruct* PtStruct;
    int             track_fill_copy = DisplayOpt.DisplayDrawItems;

    if( Segment == NULL )
        return;

    if( Segment->m_Flags & IS_NEW )  // Trace in progress.
    {
        /* Delete current segment. */
        DisplayOpt.DisplayDrawItems = SKETCH;
        Segment->Draw( DrawPanel, DC, GR_XOR );
        PtStruct = Segment->Back();
        Segment ->DeleteStructure();
        if( PtStruct && (PtStruct->Type() == TYPE_DRAWSEGMENT ) )
            Segment = (DRAWSEGMENT*) PtStruct;
        DisplayOpt.DisplayDrawItems = track_fill_copy;
        SetCurItem( NULL );
    }
    else if( Segment->m_Flags == 0 )
    {
        Segment->Draw( DrawPanel, DC, GR_XOR );
        Segment->m_Flags = 0;
        SaveCopyInUndoList(Segment, UR_DELETED);
        Segment->UnLink();
        SetCurItem( NULL );
        OnModify();
    }
}


void WinEDA_PcbFrame::Delete_Drawings_All_Layer( int aLayer )
{
    if( aLayer <= LAST_COPPER_LAYER )
    {
        DisplayError( this, _( "Copper layer global delete not allowed!" ) );
        return;
    }

    wxString msg = _( "Delete Layer " ) + GetBoard()->GetLayerName( aLayer );
    if( !IsOK( this, msg ) )
        return;

    PICKED_ITEMS_LIST pickList;
    ITEM_PICKER picker(NULL, UR_DELETED);

    BOARD_ITEM*     PtNext;
    for( BOARD_ITEM* item = GetBoard()->m_Drawings;  item;  item = PtNext )
    {
        PtNext = item->Next();

        switch( item->Type() )
        {
        case TYPE_DRAWSEGMENT:
        case TYPE_TEXTE:
        case TYPE_COTATION:
        case TYPE_MIRE:
            if( item->GetLayer() == aLayer )
            {
                item->UnLink();
                picker.m_PickedItem = item;
                pickList.PushItem( picker );
            }
            break;

        default:
        {
            wxString msg;
            msg.Printf( wxT("Delete_Drawings_All_Layer() error: unknown type %d"),
                        item->Type() );
            wxMessageBox( msg );
            break;
        }
        }
    }

    if( pickList.GetCount() )
    {
        OnModify();
        SaveCopyInUndoList(pickList, UR_DELETED);
    }
}


static void Exit_EditEdge( WinEDA_DrawPanel* Panel, wxDC* DC )
{
    DRAWSEGMENT* Segment = (DRAWSEGMENT*) Panel->GetScreen()->GetCurItem();

    if( Segment == NULL )
        return;

    if( Segment->m_Flags & IS_NEW )
    {
        Panel->ManageCurseur( Panel, DC, FALSE );
        Segment ->DeleteStructure();
        Segment = NULL;
    }
    else
    {
        wxPoint pos = Panel->GetScreen()->m_Curseur;
        Panel->GetScreen()->m_Curseur = s_InitialPosition;
        Panel->ManageCurseur( Panel, DC, TRUE );
        Panel->GetScreen()->m_Curseur = pos;
        Segment->m_Flags = 0;
        Segment->Draw( Panel, DC, GR_OR );
    }
    Panel->ManageCurseur = NULL;
    Panel->ForceCloseManageCurseur = NULL;
    ( (WinEDA_PcbFrame*) Panel->GetParent() )->SetCurItem( NULL );
}


/* Initialize the drawing of a segment of type other than trace.
 */
DRAWSEGMENT* WinEDA_PcbFrame::Begin_DrawSegment( DRAWSEGMENT* Segment,
                                                 int shape, wxDC* DC )
{
    int          s_large;
    int          angle = 0;
    DRAWSEGMENT* DrawItem;

    s_large = GetBoard()->GetBoardDesignSettings()->m_DrawSegmentWidth;
    if( getActiveLayer() == EDGE_N )
    {
        s_large = GetBoard()->GetBoardDesignSettings()->m_EdgeSegmentWidth;
    }

    if( shape == S_ARC )
        angle = 900;

    if( Segment == NULL )        /* Create new trace. */
    {
        SetCurItem( Segment = new DRAWSEGMENT( GetBoard() ) );
        Segment->m_Flags = IS_NEW;
        Segment->SetLayer( getActiveLayer() );
        Segment->m_Width = s_large;
        Segment->m_Shape = shape;
        Segment->m_Angle = 900;
        Segment->m_Start = Segment->m_End = GetScreen()->m_Curseur;
        DrawPanel->ManageCurseur = Montre_Position_NewSegment;
        DrawPanel->ForceCloseManageCurseur = Exit_EditEdge;
    }
    else    /* The ending point ccordinate Segment->m_End was updated by he function
             * Montre_Position_NewSegment() called on a move mouse event
             * during the segment creation
             */
    {
        if( Segment->m_Start != Segment->m_End )
        {
            if( Segment->m_Shape == S_SEGMENT )
            {
                SaveCopyInUndoList(Segment, UR_NEW );
                GetBoard()->Add( Segment );

                OnModify();
                Segment->m_Flags = 0;

                Segment->Draw( DrawPanel, DC, GR_OR );

                DrawItem = Segment;

                SetCurItem( Segment = new DRAWSEGMENT( GetBoard() ) );

                Segment->m_Flags = IS_NEW;
                Segment->SetLayer( DrawItem->GetLayer() );
                Segment->m_Width = s_large;
                Segment->m_Shape = DrawItem->m_Shape;
                Segment->m_Type  = DrawItem->m_Type;
                Segment->m_Angle = DrawItem->m_Angle;
                Segment->m_Start = Segment->m_End = DrawItem->m_End;
                Montre_Position_NewSegment( DrawPanel, DC, FALSE );
            }
            else
            {
                End_Edge( Segment, DC );
                Segment = NULL;
            }
        }
    }
    return Segment;
}


void WinEDA_PcbFrame::End_Edge( DRAWSEGMENT* Segment, wxDC* DC )
{
    if( Segment == NULL )
        return;
    Segment->Draw( DrawPanel, DC, GR_OR );

    /* Delete if segment length is zero. */
    if( Segment->m_Start == Segment->m_End )
        Segment ->DeleteStructure();

    else
    {
        Segment->m_Flags = 0;
        GetBoard()->Add( Segment );
        OnModify();
        SaveCopyInUndoList( Segment, UR_NEW );
    }

    DrawPanel->ManageCurseur = NULL;
    DrawPanel->ForceCloseManageCurseur = NULL;
    SetCurItem( NULL );
}


/* Redraw segment during cursor movement
 */
static void Montre_Position_NewSegment( WinEDA_DrawPanel* panel,
                                        wxDC* DC, bool erase )
{
    DRAWSEGMENT* Segment = (DRAWSEGMENT*)
                           panel->GetScreen()->GetCurItem();
    int          t_fill = DisplayOpt.DisplayDrawItems;

    if( Segment == NULL )
        return;

    DisplayOpt.DisplayDrawItems = SKETCH;

    if( erase )
        Segment->Draw( panel, DC, GR_XOR );

    if( Segments_45_Only && ( Segment->m_Shape == S_SEGMENT ) )
    {
        Calcule_Coord_Extremite_45( Segment->m_Start.x, Segment->m_Start.y,
                                    &Segment->m_End.x, &Segment->m_End.y );
    }
    else    /* here the angle is arbitrary */
    {
        Segment->m_End = panel->GetScreen()->m_Curseur;
    }

    Segment->Draw( panel, DC, GR_XOR );
    DisplayOpt.DisplayDrawItems = t_fill;
}
