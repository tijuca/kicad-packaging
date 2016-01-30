/********************************************************/
/* Routines generales de gestion des commandes usuelles */
/********************************************************/

/* controle.cpp */

#include "fctsys.h"
#include "common.h"
#include "class_drawpanel.h"
#include "pcbnew.h"
#include "protos.h"

#include "id.h"
#include "collectors.h"

#include "bitmaps.h"

/*************************************************************************************/
static BOARD_ITEM* AllAreModulesAndReturnSmallestIfSo( GENERAL_COLLECTOR* aCollector )
/*************************************************************************************/
/**
 * Function AllAreModulesAndReturnSmallestIfSo
 * tests that all items in the collection are MODULEs and if so, returns the
 * smallest MODULE.
 * @return BOARD_ITEM* - The smallest or NULL.
 */
{
    int count = aCollector->GetCount();

    for( int i = 0; i<count;  ++i )
    {
        if( (*aCollector)[i]->Type() != TYPE_MODULE )
            return NULL;
    }

    // all are modules, now find smallest MODULE

    int minDim = 0x7FFFFFFF;
    int minNdx = 0;

    for( int i = 0;  i<count;  ++i )
    {
        MODULE* module = (MODULE*) (*aCollector)[i];

        int     lx = module->m_BoundaryBox.GetWidth();
        int     ly = module->m_BoundaryBox.GetHeight();

        int     lmin = MIN( lx, ly );

        if( lmin <= minDim )
        {
            minDim = lmin;
            minNdx = i;
        }
    }

    return (*aCollector)[minNdx];
}


/****************************************************************************/
BOARD_ITEM* WinEDA_BasePcbFrame::PcbGeneralLocateAndDisplay( int aHotKeyCode )
/****************************************************************************/
{
    BOARD_ITEM* item;

    GENERAL_COLLECTORS_GUIDE guide = GetCollectorsGuide();

    // Assign to scanList the proper item types desired based on tool type
    // or hotkey that is in play.

    const KICAD_T* scanList = NULL;

    if( aHotKeyCode )
    {
        // @todo: add switch here and add calls to PcbGeneralLocateAndDisplay( int aHotKeyCode )
        // when searching is needed from a hotkey handler
    }
    else if( m_ID_current_state == 0 )
    {
        switch( m_HTOOL_current_state )
        {
        case ID_TOOLBARH_PCB_AUTOPLACE:
            scanList = GENERAL_COLLECTOR::ModuleItems;
            break;

        default:
            scanList = DisplayOpt.DisplayZonesMode == 0 ?
                       GENERAL_COLLECTOR::AllBoardItems :
                       GENERAL_COLLECTOR::AllButZones;
            break;
        }
    }
    else
    {
        switch( m_ID_current_state )
        {
        case ID_PCB_SHOW_1_RATSNEST_BUTT:
            scanList = GENERAL_COLLECTOR::PadsOrModules;
            break;

        case ID_TRACK_BUTT:
            scanList = GENERAL_COLLECTOR::Tracks;
            break;

        case ID_COMPONENT_BUTT:
            scanList = GENERAL_COLLECTOR::ModuleItems;
            break;

        default:
            scanList = DisplayOpt.DisplayZonesMode == 0 ?
                       GENERAL_COLLECTOR::AllBoardItems :
                       GENERAL_COLLECTOR::AllButZones;
        }
    }

    m_Collector->Collect( m_Pcb, scanList, GetScreen()->RefPos( true ), guide );

#if 0
    // debugging: print out the collected items, showing their priority order too.
    for( int i = 0; i<m_Collector->GetCount();  ++i )
        (*m_Collector)[i]->Show( 0, std::cout );
#endif

    /* Remove redundancies: sometime, zones are found twice,
     * because zones can be are filled by overlapping segments (this is a fill option)
     */
    unsigned long timestampzone = 0;

    for( int ii = 0;  ii < m_Collector->GetCount(); ii++ )
    {
        item = (*m_Collector)[ii];
        if( item->Type() != TYPE_ZONE )
            continue;

        /* Found a TYPE ZONE */
        if( item->m_TimeStamp == timestampzone )    // Remove it, redundant, zone already found
        {
            m_Collector->Remove( ii );
            ii--;
        }
        else
            timestampzone = item->m_TimeStamp;
    }

    if( m_Collector->GetCount() <= 1 )
    {
        item = (*m_Collector)[0];
        SetCurItem( item );
    }
    // If the count is 2, and first item is a pad or moduletext, and the 2nd item is its parent module:
    else if( m_Collector->GetCount() == 2
             && ( (*m_Collector)[0]->Type() == TYPE_PAD || (*m_Collector)[0]->Type() ==
                 TYPE_TEXTE_MODULE )
             && (*m_Collector)[1]->Type() == TYPE_MODULE && (*m_Collector)[0]->GetParent()==
             (*m_Collector)[1] )
    {
        item = (*m_Collector)[0];
        SetCurItem( item );
    }
    // if all are modules, find the smallest one amoung the primary choices
    else if( ( item = AllAreModulesAndReturnSmallestIfSo( m_Collector ) ) != NULL )
    {
        SetCurItem( item );
    }

    else    // we can't figure out which item user wants, do popup menu so user can choose
    {
        wxMenu itemMenu;

        /* Give a title to the selection menu. This is also a cancel menu item */
        wxMenuItem * item_title = new wxMenuItem(&itemMenu, -1, _( "Selection Clarification" ) );
#ifdef __WINDOWS__
        wxFont bold_font(*wxNORMAL_FONT);
        bold_font.SetWeight(wxFONTWEIGHT_BOLD);
        bold_font.SetStyle( wxFONTSTYLE_ITALIC);
        item_title->SetFont(bold_font);
#endif
        itemMenu.Append(item_title);
        itemMenu.AppendSeparator();

        int limit = MIN( MAX_ITEMS_IN_PICKER, m_Collector->GetCount() );

        for( int i = 0;  i<limit;  ++i )
        {
            wxString     text;
            const char** xpm;

            item = (*m_Collector)[i];

            text = item->MenuText( m_Pcb );
            xpm  = item->MenuIcon();

            ADD_MENUITEM( &itemMenu, ID_POPUP_PCB_ITEM_SELECTION_START + i, text, xpm );
        }

        /* @todo: rather than assignment to TRUE, these should be increment and decrement operators throughout _everywhere_.
         *  That way we can handle nesting.
         *  But I tried that and found there cases where the assignment to TRUE (converted to a m_IgnoreMouseEvents++ )
         *  was not balanced with the -- (now m_IgnoreMouseEvents=FALSE), so I had to revert.
         *  Somebody should track down these and make them balanced.
         *  DrawPanel->m_IgnoreMouseEvents = TRUE;
         */

        // this menu's handler is void WinEDA_BasePcbFrame::ProcessItemSelection()
        // and it calls SetCurItem() which in turn calls Display_Infos() on the item.
        DrawPanel->m_AbortRequest = true;   // changed in false if an item
        PopupMenu( &itemMenu ); // m_AbortRequest = false if an item is selected

        DrawPanel->MouseToCursorSchema();

//        DrawPanel->m_IgnoreMouseEvents = FALSE;

        // The function ProcessItemSelection() has set the current item, return it.
        item = GetCurItem();
    }

    return item;
}


/**
 * Function Join
 * finds the point where line segment (b1,b0) intersects with segment (a1,a0).
 * If that point would be outside of (a0,a1), the respective endpoint is used.
 * Join returns the point in "res" and "true" if a suitable point was found,
 * "false" if both lines are parallel or if the length of either segment is zero.
 */
static bool Join( wxPoint* res, wxPoint a0, wxPoint a1, wxPoint b0, wxPoint b1 )
{
    /* References:
        http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
        http://www.gekkou.co.uk/blogs/monologues/2007/12/13/1197586800000.html
    */

    double  denom;
    double  t;

    // if either segment is zero length
    if( a1.x==a0.x && a1.y==a0.y )
        return false;

    if( b1.x==b0.x && b1.y==b0.y )
        return false;

    a1 -= a0;
    b1 -= b0;

    b0 -= a0;

    denom = (double) b1.y * a1.x - (double) b1.x * a1.y;
    if( !denom )
    {
        return false;       // parallel
    }

    t = ((double) b1.y * b0.x - (double) b1.x * b0.y ) / denom;

    t = min( max( t, 0.0 ), 1.0 );

    res->x = (int) round( a0.x + t * a1.x );
    res->y = (int) round( a0.y + t * a1.y );

    return true;
}


/*
 * "Project" finds the projection of a grid point on a track. This is the point
 * from where we want to draw new orthogonal tracks when starting on a track.
 */
bool Project( wxPoint* res, wxPoint on_grid, const TRACK* track )
{
    if( track->m_Start == track->m_End )
        return false;

    wxPoint vec = track->m_End - track->m_Start;

    double t = double( on_grid.x - track->m_Start.x ) * vec.x +
        double( on_grid.y - track->m_Start.y ) * vec.y;

    t /= (double) vec.x * vec.x + (double) vec.y * vec.y;
    t = min( max( t, 0.0 ), 1.0 );

    res->x = (int) round( track->m_Start.x + t * vec.x );
    res->y = (int) round( track->m_Start.y + t * vec.y );

    return true;
}


/**
 * Function Magnetize
 * tests to see if there are any magnetic items within near reach of the given
 * "curpos".  If yes, then curpos is adjusted appropriately according to that
 * near magnetic item and true is returned.
 * @param curpos The initial position, and what to adjust if a change is needed.
 * @return bool - true if the position was adjusted magnetically, else false.
 */
static bool Magnetize( BOARD* m_Pcb, WinEDA_PcbFrame* frame,
                       int aCurrentTool, wxSize grid, wxPoint on_grid, wxPoint* curpos )
{
    bool    doCheckNet = g_MagneticPadOption != capture_always && Drc_On;
    bool    doTrack = false;
    bool    doPad = false;
    bool    amMovingVia = false;

    TRACK*      currTrack = g_CurrentTrackSegment;
    BOARD_ITEM* currItem  = frame->GetCurItem();

    // D( printf( "currTrack=%p currItem=%p currTrack->Type()=%d currItem->Type()=%d\n",  currTrack, currItem, currTrack ? currTrack->Type() : 0, currItem ? currItem->Type() : 0 ); )

    if( !currTrack && currItem && currItem->Type()==TYPE_VIA && currItem->m_Flags )
    {
        // moving a VIA
        currTrack = (TRACK*) currItem;
        amMovingVia = true;

        return false;   // comment this return out and play with it.
    }
    else if( currItem != currTrack )
    {
        currTrack = NULL;
    }


    if( g_MagneticPadOption == capture_always )
        doPad = true;

    if( g_MagneticTrackOption == capture_always )
        doTrack = true;

    if( aCurrentTool == ID_TRACK_BUTT || amMovingVia )
    {
        int q = capture_cursor_in_track_tool;

        if( g_MagneticPadOption == q )
            doPad = true;

        if( g_MagneticTrackOption == q )
            doTrack = true;
    }

    // D(printf("doPad=%d doTrack=%d aCurrentTool=%d amMovingVia=%d\n", doPad, doTrack, aCurrentTool, amMovingVia );)

    //  The search precedence order is pads, then tracks/vias

    if( doPad )
    {
        D_PAD* pad = Locate_Any_Pad( m_Pcb, CURSEUR_OFF_GRILLE, TRUE );
        if( pad )
        {
            if( doCheckNet && currTrack && currTrack->GetNet() != pad->GetNet() )
                return false;

            *curpos = pad->m_Pos;
            return true;
        }
    }

    // after pads, only track & via tests remain, skip them if not desired
    if( doTrack )
    {
        int     layer = ( (PCB_SCREEN*) ActiveScreen )->m_Active_Layer;

        for( TRACK* via = m_Pcb->m_Track;
             via && (via = Locate_Via_Area( via, *curpos, layer )) != NULL;
             via = via->Next() )
        {
            if( via != currTrack )   // a via cannot influence itself
            {
                if( !doCheckNet || !currTrack || currTrack->GetNet() == via->GetNet() )
                {
                    *curpos = via->m_Start;
                    // D(printf("via hit\n");)
                    return true;
                }
            }
            else
            {
                //D( printf( "skipping self\n" ); )
            }
        }

        if( !currTrack )
        {
            int layer_mask = g_TabOneLayerMask[layer];

            TRACK* track = Locate_Pistes( m_Pcb->m_Track, layer_mask, CURSEUR_OFF_GRILLE );
            if( !track || track->Type() != TYPE_TRACK )
            {
                // D(printf("!currTrack and track=%p not found, layer_mask=0x%X\n", track, layer_mask );)
                return false;
            }

            // D( printf( "Project\n" ); )
            return Project( curpos, on_grid, track );
        }

        /*
         * In two segment mode, ignore the final segment if it's inside a grid
         * square.
         */
        if( !amMovingVia && currTrack && g_TwoSegmentTrackBuild && currTrack->Back()
            && currTrack->m_Start.x - grid.x < currTrack->m_End.x
            && currTrack->m_Start.x + grid.x > currTrack->m_End.x
            && currTrack->m_Start.y - grid.y < currTrack->m_End.y
            && currTrack->m_Start.y + grid.y > currTrack->m_End.y )
        {
            currTrack = currTrack->Back();
        }


        for( TRACK* track = m_Pcb->m_Track;  track;  track = track->Next() )
        {
            if( track->Type() != TYPE_TRACK )
                continue;

            if( doCheckNet && currTrack && currTrack->GetNet() != track->GetNet() )
                continue;

            if( g_DesignSettings.m_LayerColor[track->GetLayer()] & ITEM_NOT_SHOW )
                continue;

            // omit the layer check if moving a via
            if( !amMovingVia && !track->IsOnLayer( layer ) )
                continue;

            if( !track->HitTest( *curpos ) )
                continue;

            D(printf( "have track prospect\n");)

            if( Join( curpos, track->m_Start, track->m_End, currTrack->m_Start, currTrack->m_End ) )
            {
                // D(printf( "join currTrack->Type()=%d\n", currTrack->Type() );)
                return true;
            }

            if( aCurrentTool == ID_TRACK_BUTT || amMovingVia )
            {
                // At this point we have a drawing mouse on a track, we are drawing
                // a new track and that new track is parallel to the track the
                // mouse is on. Find the nearest end point of the track under mouse
                // to the mouse and return that.
                double distStart = hypot( double( curpos->x - track->m_Start.x ),
                                          double( curpos->y - track->m_Start.y ));

                double distEnd   = hypot( double( curpos->x - track->m_End.x ),
                                          double( curpos->y - track->m_End.y ));

                // if track not via, or if its a via dragging but not with its adjacent track
                if( currTrack->Type() != TYPE_VIA
                    || ( currTrack->m_Start != track->m_Start && currTrack->m_Start != track->m_End ))
                {
                    if( distStart <= currTrack->m_Width/2 )
                    {
                        // D(printf("nearest end is start\n");)
                        *curpos = track->m_Start;
                        return true;
                    }

                    if( distEnd <= currTrack->m_Width/2 )
                    {
                        // D(printf("nearest end is end\n");)
                        *curpos = track->m_End;
                        return true;
                    }

                    // @todo otherwise confine curpos such that it stays centered
                    // within "track"
                }
            }
        }
    }

    return false;
}


/****************************************************************/
void WinEDA_PcbFrame::GeneralControle( wxDC* DC, wxPoint Mouse )
/*****************************************************************/
{
    wxRealPoint delta;
    wxPoint curpos, oldpos;
    int     hotkey = 0;

    ActiveScreen = GetScreen();

    // Save the board after the time out :
    int CurrentTime = time( NULL );

    if( !GetScreen()->IsModify() || GetScreen()->IsSave() )
    {
        /* If no change, reset the time out */
        g_SaveTime = CurrentTime;
    }

    if( (CurrentTime - g_SaveTime) > g_TimeOut )
    {
        wxString tmpFileName = GetScreen()->m_FileName;
        wxString filename    = g_SaveFileName + PcbExtBuffer;
        bool     flgmodify   = GetScreen()->IsModify();

        ( (WinEDA_PcbFrame*) this )->SavePcbFile( filename );

        if( flgmodify ) // Set the flags m_Modify cleared by SavePcbFile()
        {
            GetScreen()->SetModify();
            GetScreen()->SetSave(); // Set the flags m_FlagSave cleared by SetModify()
        }
        GetScreen()->m_FileName = tmpFileName;
        SetTitle( GetScreen()->m_FileName );
    }

    curpos = DrawPanel->CursorRealPosition( Mouse );
    oldpos = GetScreen()->m_Curseur;

    delta = GetScreen()->GetGrid();
    GetScreen()->Scale( delta );

    if( delta.x <= 0 )
        delta.x = 1;

    if( delta.y <= 0 )
        delta.y = 1;

    switch( g_KeyPressed )
    {
    case WXK_NUMPAD8:       /* Deplacement curseur vers le haut */
    case WXK_UP:
        Mouse.y -= (int) round(delta.y);
        DrawPanel->MouseTo( Mouse );
        break;

    case WXK_NUMPAD2:       /* Deplacement curseur vers le bas */
    case WXK_DOWN:
        Mouse.y += (int) round(delta.y);
        DrawPanel->MouseTo( Mouse );
        break;

    case WXK_NUMPAD4:       /* Deplacement curseur vers la gauche */
    case WXK_LEFT:
        Mouse.x -= (int) round(delta.x);
        DrawPanel->MouseTo( Mouse );
        break;

    case WXK_NUMPAD6:      /* Deplacement curseur vers la droite */
    case WXK_RIGHT:
        Mouse.x += (int) round(delta.x);
        DrawPanel->MouseTo( Mouse );
        break;

    default:
        hotkey = g_KeyPressed;
        break;
    }

    /* Put cursor in new position, according to the zoom keys (if any) */
    GetScreen()->m_Curseur = curpos;

    /* Put cursor on grid or a pad centre if requested
     * But if the tool DELETE is active the cursor is left off grid
     * this is better to reach items to delete off grid
     */
    bool   keep_on_grid = TRUE;
    if( m_ID_current_state == ID_PCB_DELETE_ITEM_BUTT )
        keep_on_grid = FALSE;

    /* Cursor is left off grid if no block in progress and no moving object */
    if( GetScreen()->BlockLocate.m_State != STATE_NO_BLOCK )
        keep_on_grid = TRUE;

    EDA_BaseStruct* DrawStruct = GetScreen()->GetCurItem();
    if( DrawStruct && DrawStruct->m_Flags )
        keep_on_grid = TRUE;

    if( keep_on_grid )
    {
        wxPoint on_grid = curpos;

        PutOnGrid( &on_grid );
        wxSize grid;
        grid.x = (int) GetScreen()->GetGrid().x;
        grid.y = (int) GetScreen()->GetGrid().y;
        if( Magnetize(m_Pcb, (WinEDA_PcbFrame *) this, m_ID_current_state,
                        grid, on_grid, &curpos) )
        {
            GetScreen()->m_Curseur = curpos;
        }
        else
        {
            // If there's no intrusion and DRC is active, we pass the cursor
            // "as is", and let ShowNewTrackWhenMovingCursor figure out what to do.
            if( !Drc_On || !g_CurrentTrackSegment
                || g_CurrentTrackSegment != this->GetCurItem()
                || !LocateIntrusion( m_Pcb->m_Track,
                                     g_CurrentTrackSegment->GetNet(),
                                     g_CurrentTrackSegment->m_Width ) )
            {
                GetScreen()->m_Curseur = on_grid;
            }
        }
    }

    if( oldpos != GetScreen()->m_Curseur )
    {
        curpos = GetScreen()->m_Curseur;
        GetScreen()->m_Curseur = oldpos;
        DrawPanel->CursorOff( DC );

        GetScreen()->m_Curseur = curpos;
        DrawPanel->CursorOn( DC );

        if( DrawPanel->ManageCurseur )
        {
            DrawPanel->ManageCurseur( DrawPanel, DC, TRUE );
        }
    }

    if( hotkey )
    {
        OnHotKey( DC, hotkey, NULL );
    }

    if( GetScreen()->IsRefreshReq() )
    {
        RedrawActiveWindow( DC, TRUE );
    }

    SetToolbars();

    Affiche_Status_Box();    /* Display new cursor coordinates */
}
