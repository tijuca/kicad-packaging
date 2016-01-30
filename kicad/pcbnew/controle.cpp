/********************************************************/
/* Routines generales de gestion des commandes usuelles */
/********************************************************/

/* controle.cpp */

#include "fctsys.h"
#include "common.h"
#include "class_drawpanel.h"
#include "pcbnew.h"
#include "wxPcbStruct.h"
#include "protos.h"
#include "pcbnew_id.h"

#include "collectors.h"

//external funtions used here:
extern bool Magnetize( BOARD* m_Pcb, WinEDA_PcbFrame* frame,
                       int aCurrentTool, wxSize grid, wxPoint on_grid, wxPoint* curpos );

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
     * because zones can be filled by overlapping segments (this is a fill option)
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
        // and it calls SetCurItem() which in turn calls DisplayInfo() on the item.
        DrawPanel->m_AbortRequest = true;   // changed in false if an item
        PopupMenu( &itemMenu ); // m_AbortRequest = false if an item is selected

        DrawPanel->MouseToCursorSchema();

//        DrawPanel->m_IgnoreMouseEvents = FALSE;

        // The function ProcessItemSelection() has set the current item, return it.
        item = GetCurItem();
    }

    return item;
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
        wxFileName fn = wxFileName( wxEmptyString, g_SaveFileName, PcbExtBuffer );
        bool     flgmodify   = GetScreen()->IsModify();

        SavePcbFile( fn.GetFullPath() );

        if( flgmodify ) // Set the flags m_Modify cleared by SavePcbFile()
        {
            OnModify();
            GetScreen()->SetSave(); // Set the flags m_FlagSave cleared by SetModify()
        }
        GetScreen()->m_FileName = tmpFileName;
        SetTitle( GetScreen()->m_FileName );
    }

    double scalar = GetScreen()->GetScalingFactor();

    curpos = DrawPanel->CursorRealPosition( Mouse );
    oldpos = GetScreen()->m_Curseur;

    delta = GetScreen()->GetGridSize();

    delta.x *= scalar;
    delta.y *= scalar;

    if( delta.x <= 0 )
        delta.x = 1;

    if( delta.y <= 0 )
        delta.y = 1;

    switch( g_KeyPressed )
    {
    case WXK_NUMPAD8:       /* Deplacement curseur vers le haut */
    case WXK_UP:
        Mouse.y -= wxRound(delta.y);
        DrawPanel->MouseTo( Mouse );
        break;

    case WXK_NUMPAD2:       /* Deplacement curseur vers le bas */
    case WXK_DOWN:
        Mouse.y += wxRound(delta.y);
        DrawPanel->MouseTo( Mouse );
        break;

    case WXK_NUMPAD4:       /* Deplacement curseur vers la gauche */
    case WXK_LEFT:
        Mouse.x -= wxRound(delta.x);
        DrawPanel->MouseTo( Mouse );
        break;

    case WXK_NUMPAD6:      /* Deplacement curseur vers la droite */
    case WXK_RIGHT:
        Mouse.x += wxRound(delta.x);
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
    if( GetScreen()->m_BlockLocate.m_State != STATE_NO_BLOCK )
        keep_on_grid = TRUE;

    EDA_BaseStruct* DrawStruct = GetScreen()->GetCurItem();
    if( DrawStruct && DrawStruct->m_Flags )
        keep_on_grid = TRUE;

    if( keep_on_grid )
    {
        wxPoint on_grid = curpos;

        PutOnGrid( &on_grid );
        wxSize grid;
        grid.x = (int) GetScreen()->GetGridSize().x;
        grid.y = (int) GetScreen()->GetGridSize().y;
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
                || !LocateIntrusion( m_Pcb->m_Track, g_CurrentTrackSegment ))
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
        DrawPanel->Refresh( );
        wxSafeYield();
    }

    SetToolbars();
    UpdateStatusBar();    /* Display new cursor coordinates */
}
