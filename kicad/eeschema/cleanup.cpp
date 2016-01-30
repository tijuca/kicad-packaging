/**************************************/
/* Code to handle schematic clean up. */
/**************************************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "common.h"
#include "trigo.h"
#include "confirm.h"
#include "macros.h"

#include "program.h"
#include "general.h"
#include "protos.h"
#include "netlist.h"


static int TstAlignSegment( SCH_LINE* RefSegm, SCH_LINE* TstSegm );


/* Routine cleaning:
 * - Includes segments or buses aligned in only 1 segment
 * - Detects identical objects superimposed
 */
bool SCH_SCREEN::SchematicCleanUp( wxDC* DC )
{
    SCH_ITEM* DrawList, * TstDrawList;
    int       flag;
    bool      Modify = FALSE;

    DrawList = EEDrawList;
    for( ; DrawList != NULL; DrawList = DrawList->Next() )
    {
        if( DrawList->Type() == DRAW_SEGMENT_STRUCT_TYPE )
        {
            TstDrawList = DrawList->Next();
            while( TstDrawList )
            {
                if( TstDrawList->Type() == DRAW_SEGMENT_STRUCT_TYPE )
                {
                    flag = TstAlignSegment( (SCH_LINE*) DrawList,
                                            (SCH_LINE*) TstDrawList );
                    if( flag )
                    {
                        /* keep the bits set in .m_Flags, because the deleted
                         * segment can be flagged */
                        DrawList->m_Flags |= TstDrawList->m_Flags;
                        EraseStruct( TstDrawList, this );
                        SetRefreshReq();
                        TstDrawList = EEDrawList;
                        Modify = TRUE;
                    }
                    else
                        TstDrawList = TstDrawList->Next();
                }
                else
                    TstDrawList = TstDrawList->Next();
            }
        }
    }

    WinEDA_SchematicFrame* frame;
    frame = (WinEDA_SchematicFrame*) wxGetApp().GetTopWindow();
    frame->TestDanglingEnds( EEDrawList, DC );

    return Modify;
}


/* Routine to start/end segment (BUS or wires) on junctions.
 */
void BreakSegmentOnJunction( SCH_SCREEN* Screen )
{
    SCH_ITEM* DrawList;

    if( Screen == NULL )
    {
        DisplayError( NULL,
                      wxT( "BreakSegmentOnJunction() error: NULL screen" ) );
        return;
    }

    DrawList = Screen->EEDrawList;
    while( DrawList )
    {
        switch( DrawList->Type() )
        {
        case DRAW_JUNCTION_STRUCT_TYPE:
            #undef STRUCT
            #define STRUCT ( (SCH_JUNCTION*) DrawList )
            BreakSegment( Screen, STRUCT->m_Pos );
            break;

        case DRAW_BUSENTRY_STRUCT_TYPE:
            #undef STRUCT
            #define STRUCT ( (SCH_BUS_ENTRY*) DrawList )
            BreakSegment( Screen, STRUCT->m_Pos );
            BreakSegment( Screen, STRUCT->m_End() );
            break;

        case DRAW_SEGMENT_STRUCT_TYPE:
        case DRAW_NOCONNECT_STRUCT_TYPE:
        case TYPE_SCH_LABEL:
        case TYPE_SCH_GLOBALLABEL:
        case TYPE_SCH_HIERLABEL:
        case TYPE_SCH_COMPONENT:
        case DRAW_POLYLINE_STRUCT_TYPE:
        case TYPE_SCH_MARKER:
        case TYPE_SCH_TEXT:
        case DRAW_SHEET_STRUCT_TYPE:
        case DRAW_HIERARCHICAL_PIN_SHEET_STRUCT_TYPE:
            break;

        default:
            break;
        }
        DrawList = DrawList->Next();
    }
}


/* Break a segment ( BUS, WIRE ) int 2 segments at location aBreakpoint,
 * if aBreakpoint in on segment segment
 * ( excluding ends)
 * fill aPicklist with modified items if non null
 */
void BreakSegment( SCH_SCREEN* aScreen, wxPoint aBreakpoint )
{
    SCH_LINE* segment, * NewSegment;

    for( SCH_ITEM* DrawList = aScreen->EEDrawList; DrawList;
         DrawList = DrawList->Next() )
    {
        if( DrawList->Type() != DRAW_SEGMENT_STRUCT_TYPE )
            continue;

        segment = (SCH_LINE*) DrawList;

        if( !TestSegmentHit( aBreakpoint, segment->m_Start, segment->m_End, 0 ) )
            continue;

        /* ???
         * Segment connecte: doit etre coupe en 2 si px,py
         * n'est
         *  pas une extremite */
        if( ( segment->m_Start == aBreakpoint )
           || ( segment->m_End == aBreakpoint ) )
            continue;
        /* Here we must cut the segment into 2. */
        NewSegment = segment->GenCopy();
        NewSegment->m_Start = aBreakpoint;
        segment->m_End = NewSegment->m_Start;
        NewSegment->SetNext( segment->Next() );
        segment->SetNext( NewSegment );
        DrawList = NewSegment;
    }
}


/* Search if the 2 segments RefSegm and TstSegm are on a line.
 *  Return 0 if no
 *      1 if yes, and RefSegm is modified to be the equivalent segment
 */
static int TstAlignSegment( SCH_LINE* RefSegm, SCH_LINE* TstSegm )
{
    if( RefSegm == TstSegm )
        return 0;
    if( RefSegm->GetLayer() != TstSegm->GetLayer() )
        return 0;

    // search for a common end, and modify coordinates to ensure RefSegm->m_End
    // == TstSegm->m_Start
    if( RefSegm->m_Start == TstSegm->m_Start )
    {
        if( RefSegm->m_End == TstSegm->m_End )
            return 1;
        EXCHG( RefSegm->m_Start, RefSegm->m_End );
    }
    else if( RefSegm->m_Start == TstSegm->m_End )
    {
        EXCHG( RefSegm->m_Start, RefSegm->m_End );
        EXCHG( TstSegm->m_Start, TstSegm->m_End );
    }
    else if( RefSegm->m_End == TstSegm->m_End )
    {
        EXCHG( TstSegm->m_Start, TstSegm->m_End );
    }
    else if( RefSegm->m_End != TstSegm->m_Start )
        // No common end point, segments cannot be merged.
        return 0;

    /* Test alignment: */
    if( RefSegm->m_Start.y == RefSegm->m_End.y )       // Horizontal segment
    {
        if( TstSegm->m_Start.y == TstSegm->m_End.y )
        {
            RefSegm->m_End = TstSegm->m_End;
            return 1;
        }
    }
    else if( RefSegm->m_Start.x == RefSegm->m_End.x )  // Vertical segment
    {
        if( TstSegm->m_Start.x == TstSegm->m_End.x )
        {
            RefSegm->m_End = TstSegm->m_End;
            return 1;
        }
    }
    else
    {
        if( atan2( (double) ( RefSegm->m_Start.x - RefSegm->m_End.x ),
                  (double) ( RefSegm->m_Start.y - RefSegm->m_End.y ) ) ==
            atan2( (double) ( TstSegm->m_Start.x - TstSegm->m_End.x ),
                   (double) ( TstSegm->m_Start.y - TstSegm->m_End.y ) ) )
        {
            RefSegm->m_End = TstSegm->m_End;
            return 1;
        }
    }

    return 0;
}
