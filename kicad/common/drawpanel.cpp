/******************************************/
/* drawpanel.cpp - WinEDA_DrawPanel class */
/******************************************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "gr_basic.h"
#include "common.h"
#include "macros.h"
#include "id.h"
#include "class_drawpanel.h"
#include "class_base_screen.h"
#include "wxstruct.h"


// Local defines
#define CURSOR_SIZE 12           // Cursor size in pixels

// Locad variables

/* Used to inhibit a response to a mouse left button release, after a double click
 * (when releasing the left button at the end of the second click
 * Used in eeschema to inhibit a mouse left release command when switching between
 * hierarchical sheets on a double click
 */
static bool s_IgnoreNextLeftButtonRelease = false;


// Events used by WinEDA_DrawPanel
BEGIN_EVENT_TABLE( WinEDA_DrawPanel, wxScrolledWindow )
    EVT_LEAVE_WINDOW( WinEDA_DrawPanel::OnMouseLeaving )
    EVT_MOUSEWHEEL( WinEDA_DrawPanel::OnMouseWheel )
    EVT_MOUSE_EVENTS( WinEDA_DrawPanel::OnMouseEvent )
    EVT_CHAR( WinEDA_DrawPanel::OnKeyEvent )
    EVT_CHAR_HOOK( WinEDA_DrawPanel::OnKeyEvent )
    EVT_PAINT( WinEDA_DrawPanel::OnPaint )
    EVT_SIZE( WinEDA_DrawPanel::OnSize )
    EVT_ERASE_BACKGROUND( WinEDA_DrawPanel::OnEraseBackground )
    EVT_SCROLLWIN( WinEDA_DrawPanel::OnScroll )
    EVT_ACTIVATE( WinEDA_DrawPanel::OnActivate )

    EVT_MENU_RANGE( ID_POPUP_GRID_LEVEL_1000, ID_POPUP_GRID_USER,
                    WinEDA_DrawPanel::OnPopupGridSelect )
    EVT_MENU_RANGE( ID_PAN_UP, ID_PAN_RIGHT, WinEDA_DrawPanel::OnPan )
END_EVENT_TABLE()

/************************************************************************/
/* WinEDA_DrawPanel basic functions (WinEDA_DrawPanel is the main panel)*/
/************************************************************************/

WinEDA_DrawPanel::WinEDA_DrawPanel( WinEDA_DrawFrame* parent, int id,
                                    const wxPoint& pos, const wxSize& size ) :
    wxScrolledWindow( parent, id, pos, size,
                      wxBORDER | wxNO_FULL_REPAINT_ON_RESIZE )
{
    m_Parent          = parent;
    m_ScrollButt_unit = 40;

    SetBackgroundColour( wxColour( ColorRefs[g_DrawBgColor].m_Red,
                                   ColorRefs[g_DrawBgColor].m_Green,
                                   ColorRefs[g_DrawBgColor].m_Blue ) );

    EnableScrolling( TRUE, TRUE );
    m_ClipBox.SetSize( size );
    m_ClipBox.SetX( 0 );
    m_ClipBox.SetY( 0 );
    m_CanStartBlock     = -1; // Command block can start if >= 0
    m_AbortEnable       = m_AbortRequest = FALSE;
    m_AutoPAN_Enable    = TRUE;
    m_IgnoreMouseEvents = 0;

    ManageCurseur = NULL;
    ForceCloseManageCurseur = NULL;

    if( wxGetApp().m_EDA_Config )
        m_AutoPAN_Enable = wxGetApp().m_EDA_Config->Read( wxT( "AutoPAN" ),
                                                          TRUE );

    m_AutoPAN_Request    = FALSE;
    m_Block_Enable       = FALSE;
    m_PanelDefaultCursor = m_PanelCursor = wxCURSOR_ARROW;
    m_CursorLevel = 0;
    m_PrintIsMirrored = false;
}


BASE_SCREEN* WinEDA_DrawPanel::GetScreen()
{
    WinEDA_DrawFrame* parentFrame = m_Parent;

    wxASSERT( parentFrame );

    return parentFrame->GetBaseScreen();
}


/*****************************************************************************
 *
 *  Draw the schematic cursor which is usually on grid
 *
 *****************************************************************************/
void WinEDA_DrawPanel::Trace_Curseur( wxDC* DC, int color )
{
    if( m_CursorLevel != 0 ||  DC == NULL )
        return;

    wxPoint Cursor = GetScreen()->m_Curseur;

    GRSetDrawMode( DC, GR_XOR );
    if( g_CursorShape == 1 )    /* Trace d'un reticule */
    {
        int dx = GetScreen()->Unscale( m_ClipBox.GetWidth() );
        int dy = GetScreen()->Unscale( m_ClipBox.GetHeight() );

        GRLine( &m_ClipBox, DC, Cursor.x - dx, Cursor.y,
                Cursor.x + dx, Cursor.y, 0, color );            // axe Y
        GRLine( &m_ClipBox, DC, Cursor.x, Cursor.y - dx,
                Cursor.x, Cursor.y + dy, 0, color );            // axe X
    }
    else
    {
        int len = GetScreen()->Unscale( CURSOR_SIZE );

        GRLine( &m_ClipBox, DC, Cursor.x - len, Cursor.y,
                Cursor.x + len, Cursor.y, 0, color );
        GRLine( &m_ClipBox, DC, Cursor.x, Cursor.y - len,
                Cursor.x, Cursor.y + len, 0, color );
    }
}


/*******************************************************************/
void WinEDA_DrawPanel::CursorOff( wxDC* DC )
/*******************************************************************/

/*
 *  Remove the grid cursor from the display in preparation for other drawing operations
 */
{
    Trace_Curseur( DC );
    --m_CursorLevel;
}


/*******************************************************************/
void WinEDA_DrawPanel::CursorOn( wxDC* DC )
/*******************************************************************/

/*
 *  Display the grid cursor
 */
{
    ++m_CursorLevel;
    Trace_Curseur( DC );

    if( m_CursorLevel > 0 )  // Shouldn't happen, but just in case ..
        m_CursorLevel = 0;
}


/***********************************/
int WinEDA_DrawPanel::GetZoom()
/***********************************/
{
    return GetScreen()->GetZoom();
}


/***************************************/
void WinEDA_DrawPanel::SetZoom( int zoom )
/***************************************/
{
    GetScreen()->SetZoom( zoom );
}


/************************************/
wxRealPoint WinEDA_DrawPanel::GetGrid()
/************************************/
{
    return GetScreen()->GetGrid();
}


/******************************************************/
void WinEDA_DrawPanel::PrepareGraphicContext( wxDC* DC )
/******************************************************/
{
    GRResetPenAndBrush( DC );
    DC->SetBackgroundMode( wxTRANSPARENT );
#ifdef WX_ZOOM
    double scale = GetScreen()->GetScalingFactor( );
    DC->SetUserScale( scale, scale );
    DoPrepareDC( *DC );
#endif
    SetBoundaryBox();
}


/**********************************************************************/
wxPoint WinEDA_DrawPanel::CursorRealPosition( const wxPoint& ScreenPos )
/**********************************************************************/

/** CursorRealPosition (used to calculate the cursor position in internal units)
 * @return  position (in internal units)
 * @param  ScreenPos = absolute position in pixels
 */
{
    return GetScreen()->CursorRealPosition( ScreenPos );
}


/********************************************************/
bool WinEDA_DrawPanel::IsPointOnDisplay( wxPoint ref_pos )
/********************************************************/

/** Funcion IsPointOnDisplay
 * @param ref_pos is the position to test in pixels, relative to the panel.
 * @return TRUE if ref_pos is a point currently visible on screen
 *         FALSE if ref_pos is out of screen
 */
{
    wxPoint  pos;
    EDA_Rect display_rect;

    SetBoundaryBox();
    display_rect = m_ClipBox;

    // Reduction legere des dimension de l'ecran utile pour eviter cadrage
    // en limite d'ecran
    #define PIXEL_MARGIN 8
    display_rect.Inflate( -PIXEL_MARGIN, -PIXEL_MARGIN );

    // Conversion en coord physiques
    pos = CalcUnscrolledPosition( display_rect.GetPosition() );

    GetScreen()->Unscale( pos );
    pos += GetScreen()->m_DrawOrg;
    display_rect.m_Pos = pos;
    GetScreen()->Unscale( display_rect.m_Size );

    return display_rect.Inside( ref_pos );
}


void WinEDA_DrawPanel::PostDirtyRect( EDA_Rect aRect )
{
    // D( printf( "1) PostDirtyRect( x=%d, y=%d, width=%d, height=%d)\n", aRect.m_Pos.x, aRect.m_Pos.y, aRect.m_Size.x, aRect.m_Size.y ); )

    // Convert the rect coordinates and size to pixels (make a draw clip box):
    ConvertPcbUnitsToPixelsUnits( &aRect );

    // Ensure the rectangle is large enough after truncations.
    // The pcb units have finer granularity than the pixels, so it can happen
    // that the rectangle is not large enough for the erase portion.

    aRect.m_Size.x += 2;  // += 1 is not enough!
    aRect.m_Size.y += 2;

    // D( printf( "2) PostDirtyRect( x=%d, y=%d, width=%d, height=%d)\n",  aRect.m_Pos.x, aRect.m_Pos.y, aRect.m_Size.x, aRect.m_Size.y ); )

    // pass wxRect() via EDA_Rect::operator wxRect() overload
    RefreshRect( aRect, TRUE );
}


/************************************************************************/
void WinEDA_DrawPanel::ConvertPcbUnitsToPixelsUnits( EDA_Rect* aRect )
/************************************************************************/
{
    // Calculate the draw area origin in internal units:
    wxPoint pos = aRect->GetPosition();

    ConvertPcbUnitsToPixelsUnits( &pos );
    aRect->SetOrigin( pos );                // rect origin in pixel units
    GetScreen()->Scale( aRect->m_Size );
}


/***************************************************************************/
void WinEDA_DrawPanel::ConvertPcbUnitsToPixelsUnits( wxPoint* aPosition )
/***************************************************************************/
{
    // Calculate the draw area origin in internal units:
    wxPoint drwOrig;
    int     x_axis_scale, y_axis_scale;

    // Origin in scroll units;
    GetViewStart( &drwOrig.x, &drwOrig.y );
    GetScrollPixelsPerUnit( &x_axis_scale, &y_axis_scale );

    // Origin in pixels units
    drwOrig.x *= x_axis_scale;
    drwOrig.y *= y_axis_scale;

    // Origin in internal units
    GetScreen()->Unscale( drwOrig );

    // Real origin, according to the "plot" origin
    drwOrig += GetScreen()->m_DrawOrg;

    // position in internal units, relative to the visible draw area origin
    *aPosition -= drwOrig;

    // position in pixels, relative to the visible draw area origin
    GetScreen()->Scale( *aPosition );
}


/********************************************************/
wxPoint WinEDA_DrawPanel::CursorScreenPosition()
/********************************************************/

/** Function CursorScreenPosition
 * @return the curseur current position in pixels in the screen draw area
 */
{
    wxPoint pos = GetScreen()->m_Curseur;
    pos -= GetScreen()->m_DrawOrg;
    GetScreen()->Scale( pos );
    return pos;
}


/*********************************************************/
wxPoint WinEDA_DrawPanel::GetScreenCenterRealPosition( void )
/*********************************************************/

/** Function GetScreenCenterRealPosition()
 * @return position (in internal units) of the current area centre showed on screen
 */
{
    wxSize  size;
    wxPoint realpos;

    size = GetClientSize() / 2;
    realpos = CalcUnscrolledPosition( wxPoint( size.x, size.y ) );

    GetScreen()->Unscale( realpos );
    realpos += GetScreen()->m_DrawOrg;

    return realpos;
}


/**********************************************/
void WinEDA_DrawPanel::MouseToCursorSchema()
/**********************************************/

/* Move the mouse cursor to the current schematic cursor
 */
{
    wxPoint Mouse = CursorScreenPosition();

    MouseTo( Mouse );
}


/****************************************************/
void WinEDA_DrawPanel::MouseTo( const wxPoint& Mouse )
/****************************************************/

/** Move the mouse cursor to the position "Mouse"
 * @param Mouse = mouse cursor position, in pixels units
 */
{
    int     x, y, xPpu, yPpu;
    wxPoint screenPos, drawingPos;
    wxRect  clientRect( wxPoint( 0, 0 ), GetClientSize() );

#ifdef WX_ZOOM
    CalcScrolledPosition( Mouse.x, Mouse.y, &screenPos.x, &screenPos.y );
#else
    screenPos = Mouse - GetScreen()->m_StartVisu;
#endif

    /* Scroll if the requested mouse position cursor is outside the drawing
     * area. */
    if( !clientRect.Contains( screenPos ) )
    {
        GetViewStart( &x, &y );
        GetScrollPixelsPerUnit( &xPpu, &yPpu );
        CalcUnscrolledPosition( screenPos.x, screenPos.y,
                                &drawingPos.x, &drawingPos.y );

        wxLogDebug( wxT( "MouseTo() initial screen position(%d, %d) " \
                         "rectangle(%d, %d, %d, %d) view(%d, %d)" ),
                    screenPos.x, screenPos.y, clientRect.x, clientRect.y,
                    clientRect.width, clientRect.height, x, y );

        if( screenPos.y < clientRect.GetTop() )
            y -= m_ScrollButt_unit * yPpu;
        else if( screenPos.y > clientRect.GetBottom() )
            y += m_ScrollButt_unit * yPpu;
        else if( clientRect.GetRight() < screenPos.x )
            x += m_ScrollButt_unit * xPpu;
        else
            x -= m_ScrollButt_unit * xPpu;

        Scroll( x, y );
        CalcScrolledPosition( drawingPos.x, drawingPos.y,
                              &screenPos.x, &screenPos.y );

        wxLogDebug( wxT( "MouseTo() scrolled screen position(%d, %d) " \
                         "view(%d, %d)" ), screenPos.x, screenPos.y, x, y );
    }

    WarpPointer( screenPos.x, screenPos.y );
}


/********************************************************/
void WinEDA_DrawPanel::OnActivate( wxActivateEvent& event )
/********************************************************/

/**
 * Called on window activation.
 * init the member m_CanStartBlock to avoid a block start command
 * on activation (because a left mouse buton can be pressed and no block command wanted
 * This happens when enter on a hierarchycat sheet on double click
 */
{
    m_CanStartBlock = -1;   // Block Command can't start
    event.Skip();
}


/***********************************************************/
void WinEDA_DrawPanel::OnEraseBackground( wxEraseEvent& event )
/***********************************************************/
{
    event.Skip();
}


/*********************************************************/
void WinEDA_DrawPanel::OnScroll( wxScrollWinEvent& event )
/*********************************************************/
{
    int id = event.GetEventType();
    int dir, value = 0;
    int x, y;

    GetViewStart( &x, &y );
    dir = event.GetOrientation();   // wxHORIZONTAL or wxVERTICAL

    if( id == wxEVT_SCROLLWIN_LINEUP )
        value = -m_ScrollButt_unit;

    else if( id == wxEVT_SCROLLWIN_LINEDOWN )
        value = m_ScrollButt_unit;

    else if( id == wxEVT_SCROLLWIN_THUMBTRACK )
    {
        value = event.GetPosition();
        if( dir == wxHORIZONTAL )
            Scroll( value, -1 );
        else
            Scroll( -1, value );
        return;
    }
    else
    {
        event.Skip();
        return;
    }

    if( dir == wxHORIZONTAL )
    {
        Scroll( x + value, -1 );
    }
    else
    {
        Scroll( -1, y + value );
    }
    event.Skip();
}


/*************************************************/
void WinEDA_DrawPanel::OnSize( wxSizeEvent& event )
/*************************************************/
{
    SetBoundaryBox();
    event.Skip();
}


/******************************************/
void WinEDA_DrawPanel::SetBoundaryBox()
/******************************************/

/** Function SetBoundaryBox()
 * set the m_ClipBox member to the current displayed rectangle dimensions
 */
{
    BASE_SCREEN* Screen = GetScreen();;

    if( !Screen )
        return;
    wxPoint org;
    int     ii, jj;

    GetViewStart( &org.x, &org.y );
    GetScrollPixelsPerUnit( &ii, &jj );
    org.x *= ii;
    org.y *= jj;

    Screen->m_StartVisu = org;

    m_ClipBox.SetOrigin( org );
    m_ClipBox.SetSize( GetClientSize() );

#ifdef WX_ZOOM
    CalcUnscrolledPosition( m_ClipBox.m_Pos.x, m_ClipBox.m_Pos.y,
                            &m_ClipBox.m_Pos.x, &m_ClipBox.m_Pos.y );
#else
    m_ClipBox.m_Pos -= GetScreen()->m_StartVisu;
#endif

    m_ScrollButt_unit = MIN( Screen->m_SizeVisu.x, Screen->m_SizeVisu.y ) / 4;
    if( m_ScrollButt_unit < 2 )
        m_ScrollButt_unit = 2;

    Screen->m_ScrollbarPos.x = GetScrollPos( wxHORIZONTAL );
    Screen->m_ScrollbarPos.y = GetScrollPos( wxVERTICAL );
}


/*********************************************/
void WinEDA_DrawPanel::EraseScreen( wxDC* DC )
/*********************************************/
{
    GRSetDrawMode( DC, GR_COPY );

    GRSFilledRect( &m_ClipBox, DC, m_ClipBox.GetX(), m_ClipBox.GetY(),
                   m_ClipBox.GetRight(), m_ClipBox.GetBottom(),
                   g_DrawBgColor, g_DrawBgColor );
}


#if wxUSE_GRAPHICS_CONTEXT
// note: wxUSE_GRAPHICS_CONTEXT must be set to 1 in wxWidgets
// see setup.h in wx Widgets.
// wxWidgets configure can need option  --enable-graphics_ctx
// Currently, **only for tests**
//#define USE_GCDC_IN_KICAD     // uncommment it to use wxGCDC
#endif

/***************************************************/
void WinEDA_DrawPanel::OnPaint( wxPaintEvent& event )
/***************************************************/
{
#ifdef USE_GCDC_IN_KICAD
    wxPaintDC pDC( this );
    wxGCDC paintDC(pDC);									// Following line should be disabled on MSW and OS X
    paintDC.GetGraphicsContext()->Translate(0.5, 0.5);		// Fix for pixel offset bug http://trac.wxwidgets.org/ticket/4187
#else
    wxPaintDC paintDC( this );
#endif
    EDA_Rect  tmp;
    wxRect    PaintClipBox;
    wxPoint   org;

    PrepareGraphicContext( &paintDC );
    tmp = m_ClipBox;

    org = m_ClipBox.GetOrigin();

    wxRegion upd = GetUpdateRegion(); // get the update rect list

    // get the union of all rectangles in the update region, 'upd'
    PaintClipBox = upd.GetBox();

#if 0 && defined (DEBUG)
    printf( "1) PaintClipBox=(%d, %d, %d, %d) org=(%d, %d) m_ClipBox=(%d, %d, %d, %d)\n",
            PaintClipBox.x,
            PaintClipBox.y,
            PaintClipBox.width,
            PaintClipBox.height,
            org.x, org.y,
            m_ClipBox.m_Pos.x, m_ClipBox.m_Pos.y,
            m_ClipBox.m_Size.x, m_ClipBox.m_Size.y
            );
#endif

    PaintClipBox.Offset( org );

#ifdef WX_ZOOM
    BASE_SCREEN* screen = GetScreen();
    screen->Unscale( m_ClipBox.m_Pos );
    screen->Unscale( m_ClipBox.m_Size );
#else
    m_ClipBox.SetX( PaintClipBox.GetX() );
    m_ClipBox.SetY( PaintClipBox.GetY() );
    m_ClipBox.SetWidth( PaintClipBox.GetWidth() );
    m_ClipBox.SetHeight( PaintClipBox.GetHeight() );
#endif


#if 0 && defined (DEBUG)
    printf( "2) PaintClipBox=(%d, %d, %d, %d) org=(%d, %d) m_ClipBox=(%d, %d, %d, %d)\n",
            PaintClipBox.x,
            PaintClipBox.y,
            PaintClipBox.width,
            PaintClipBox.height,
            org.x, org.y,
            m_ClipBox.m_Pos.x, m_ClipBox.m_Pos.y,
            m_ClipBox.m_Size.x, m_ClipBox.m_Size.y
            );
#endif


    PaintClipBox = m_ClipBox;

    // call ~wxDCClipper() before ~wxPaintDC()
    {
        wxDCClipper dcclip( paintDC, PaintClipBox );

        ReDraw( &paintDC, TRUE );
    }

    m_ClipBox = tmp;
    event.Skip();
}


/****************************************************/
void WinEDA_DrawPanel::ReDraw( wxDC* DC, bool erasebg )
/****************************************************/
{
    BASE_SCREEN* Screen = GetScreen();

    if( Screen == NULL )
        return;

    if( ( g_DrawBgColor != WHITE ) && ( g_DrawBgColor != BLACK ) )
        g_DrawBgColor = BLACK;

    if( g_DrawBgColor == WHITE )
    {
        g_XorMode    = GR_NXOR;
        g_GhostColor = BLACK;
    }
    else
    {
        g_XorMode    = GR_XOR;
        g_GhostColor = WHITE;
    }

#ifdef WX_ZOOM
    double scale = Screen->GetScalingFactor( );
    DC->SetUserScale( scale, scale );
#endif

    if( erasebg )
        PrepareGraphicContext( DC );

    DC->SetFont( *g_StdFont );

    SetBackgroundColour( wxColour( ColorRefs[g_DrawBgColor].m_Red,
                                   ColorRefs[g_DrawBgColor].m_Green,
                                   ColorRefs[g_DrawBgColor].m_Blue ) );

    GRResetPenAndBrush( DC );

    DC->SetBackground( *wxBLACK_BRUSH );
    DC->SetBackgroundMode( wxTRANSPARENT );
    m_Parent->RedrawActiveWindow( DC, erasebg );
}


/***********************************************/
void WinEDA_DrawPanel::DrawBackGround( wxDC* DC )
/***********************************************/

/**  Function DrawBackGround
 * @param DC = current Device Context
 * Draws X , Y axis
 * draws the grid
 *  - the grid is drawn only if the zoom level allows a good visibility
 *  - the grid is always centered on the screen centre
 */
{
    int          Color  = BLUE;
    BASE_SCREEN* screen = GetScreen();
    int          ii, jj, xg, yg, color;
    wxRealPoint  screen_grid_size;
    bool         drawgrid = FALSE;
    wxSize       size;
    wxPoint      org;

    color = g_GridColor;

    GRSetDrawMode( DC, GR_COPY );

    /* The grid must be visible. this is possible only is grid value
     * and zoom value are sufficient
     */
    drawgrid = m_Parent->m_Draw_Grid;

    screen_grid_size = screen->GetGrid();

    wxRealPoint  dgrid = screen_grid_size;
    screen->Scale( dgrid );     // dgrid = grid size in pixels
    // if the grid size is sall ( < 5 pixels) do not display all points
    if( dgrid.x  < 5 )
    {
        screen_grid_size.x *= 2;
        dgrid.x *= 2;
    }
    if( dgrid.x < 5 )
        drawgrid = FALSE; // The gris is too small: do not show it

    if( dgrid.y < 5 )
    {
        screen_grid_size.y *= 2;
        dgrid.y *= 2;
    }
    if( dgrid.y < 5 )
        drawgrid = FALSE; // The gris is too small

    GetViewStart( &org.x, &org.y );
    GetScrollPixelsPerUnit( &ii, &jj );
    org.x *= ii;
    org.y *= jj;
    screen->m_StartVisu = org;
    screen->Unscale( org );

    org += screen->m_DrawOrg;

    size = GetClientSize();
    screen->Unscale( size );

    if( drawgrid )
    {
        m_Parent->PutOnGrid( &org );

        GRSetColorPen( DC, color );
        for( ii = 0; ; ii++ )
        {
            xg =  (int) round(ii * screen_grid_size.x);
            int xpos = org.x + xg;

            for( jj = 0; ; jj++ )
            {
                yg = (int) round(jj * screen_grid_size.y);
                GRPutPixel( &m_ClipBox, DC, xpos, org.y + yg, color );
                if( yg > size.y )
                    break;
            }

            if( xg > size.x )
                break;
        }
    }

    /* Draw axis */
    if(  m_Parent->m_Draw_Axis )
    {
        /* Draw the Y axis */
        GRDashedLine( &m_ClipBox, DC, 0, -screen->ReturnPageSize().y,
                      0, screen->ReturnPageSize().y, 0, Color );

        /* Draw the X axis */
        GRDashedLine( &m_ClipBox, DC, -screen->ReturnPageSize().x, 0,
                      screen->ReturnPageSize().x, 0, 0, Color );
    }

    /* Draw auxiliary axis */
    if( m_Parent->m_Draw_Auxiliary_Axis )
    {
        m_Draw_Auxiliary_Axis( DC, FALSE );
    }
}


/********************************************************************/
void WinEDA_DrawPanel::m_Draw_Auxiliary_Axis( wxDC* DC, int drawmode )
/********************************************************************/

/** m_Draw_Auxiliary_Axis
 * Draw the Auxiliary Axis, used in pcbnew which as origin coordinates
 * for gerber and excellon files
 */
{
    if( m_Parent->m_Auxiliary_Axis_Position.x == 0
        && m_Parent->m_Auxiliary_Axis_Position.y == 0 )
        return;

    int          Color  = DARKRED;
    BASE_SCREEN* screen = GetScreen();

    GRSetDrawMode( DC, drawmode );

    /* Draw the Y axis */
    GRDashedLine( &m_ClipBox, DC,
                  m_Parent->m_Auxiliary_Axis_Position.x,
                  -screen->ReturnPageSize().y,
                  m_Parent->m_Auxiliary_Axis_Position.x,
                  screen->ReturnPageSize().y,
                  0, Color );

    /* Draw the X axis */
    GRDashedLine( &m_ClipBox, DC,
                  -screen->ReturnPageSize().x,
                  m_Parent->m_Auxiliary_Axis_Position.y,
                  screen->ReturnPageSize().x,
                  m_Parent->m_Auxiliary_Axis_Position.y,
                  0, Color );
}


/*******************************************************/
bool WinEDA_DrawPanel::OnRightClick( wxMouseEvent& event )
/*******************************************************/

/** Build and display a Popup menu on a right mouse button click
 * @return true if a popup menu is shown, or false
 */
{
    wxPoint pos;
    wxMenu  MasterMenu;

    pos = event.GetPosition();

    if( !m_Parent->OnRightClick( pos, &MasterMenu ) )
        return false;

    AddMenuZoom( &MasterMenu );

    m_IgnoreMouseEvents = TRUE;
    PopupMenu( &MasterMenu, pos );
    MouseToCursorSchema();
    m_IgnoreMouseEvents = FALSE;

    return true;
}


/******************************************************/
void WinEDA_DrawPanel::OnMouseLeaving( wxMouseEvent& event )
/*******************************************************/

// Called when the canvas receives a mouse event leaving frame. //
{
    if( ManageCurseur == NULL )  // Pas de commande encours
        m_AutoPAN_Request = FALSE;

    if( !m_AutoPAN_Enable || !m_AutoPAN_Request || m_IgnoreMouseEvents )
        return;

    // Auto pan if mouse is leave working aera:
    wxSize size = GetClientSize();

    if( ( size.x < event.GetX() ) || ( size.y < event.GetY() )
        || ( event.GetX() <= 0) || ( event.GetY() <= 0 ) )
    {
        wxCommandEvent cmd( wxEVT_COMMAND_MENU_SELECTED, ID_POPUP_ZOOM_CENTER );
        cmd.SetEventObject( this );
        GetEventHandler()->ProcessEvent( cmd );
    }
}


/*
 * Handle mouse wheel events.
 *
 * The mouse wheel is used to provide support for zooming and panning.  This
 * is accomplished by converting mouse wheel events in psuedo menu command
 * events.
 */
void WinEDA_DrawPanel::OnMouseWheel( wxMouseEvent& event )
{
    wxRect rect = GetRect();

    /* This fixes a bad rectangle horizontal position returned by the
     * schematic library veiwer panel.  It may have something to do with
     * the sash window. */
    rect.Offset( -rect.x, -rect.y );

    /* Ignore scroll events if the cursor is outside the drawing area. */
    if( event.GetWheelRotation() == 0 || !GetParent()->IsEnabled()
        || !rect.Contains( event.GetPosition() ) )
    {
        wxLogDebug( wxT( "OnMouseWheel() position(%d, %d) " \
                         "rectangle(%d, %d, %d, %d)" ),
                    event.GetPosition().x, event.GetPosition().y,
                    rect.x, rect.y, rect.width, rect.height );

        event.Skip();
        return;
    }

    wxCommandEvent cmd( wxEVT_COMMAND_MENU_SELECTED );
    cmd.SetEventObject( this );

    // This is a zoom in or out command
    if( event.GetWheelRotation() > 0 )
    {
        if( event.ShiftDown() && !event.ControlDown() )
            cmd.SetId( ID_PAN_UP );
        else if( event.ControlDown() && !event.ShiftDown() )
            cmd.SetId( ID_PAN_LEFT );
        else
            cmd.SetId( ID_POPUP_ZOOM_IN );
    }
    else if ( event.GetWheelRotation() < 0 )
    {
        if( event.ShiftDown() && !event.ControlDown() )
            cmd.SetId( ID_PAN_DOWN );
        else if( event.ControlDown() && !event.ShiftDown() )
            cmd.SetId( ID_PAN_RIGHT );
        else
            cmd.SetId( ID_POPUP_ZOOM_OUT );
    }

    GetEventHandler()->ProcessEvent( cmd );
}

/******************************************************/
void WinEDA_DrawPanel::OnMouseEvent( wxMouseEvent& event )
/*******************************************************/

// Called when the canvas receives a mouse event. //
{
    int                      localrealbutt = 0, localbutt = 0, localkey = 0;
    BASE_SCREEN*             screen = GetScreen();
    static WinEDA_DrawPanel* LastPanel;

    if( !screen )
        return;
    #define MIN_DRAG_COUNT_FOR_START_BLOCK_COMMAND 5        /* Adjust value to filter mouse deplacement before
                                                             * consider the drag mouse is really a drag command, not just a movement while click
                                                             */
    static int MinDragEventCount;               /* counts the drag events.
                                                 * used to filter mouse moves before starting a block command
                                                 * a block comman can be started only if MinDragEventCount > MIN_DRAG_COUNT_FOR_START_BLOCK_COMMAND
                                                 * in order to avoid spurious block commands
                                                 */
    if( event.Leaving() || event.Entering() )
    {
        m_CanStartBlock = -1;
    }

    if( ManageCurseur == NULL )  // No command in progress
        m_AutoPAN_Request = FALSE;

    if( m_Parent->m_FrameIsActive )
        SetFocus();
    else
        return;

    if( !event.IsButton() && !event.Moving()
        && !event.Dragging() && !localkey )
    {
        return;
    }

    if( event.RightDown() )
    {
        OnRightClick( event );
        return;
    }

    if( m_IgnoreMouseEvents )
        return;

    if( event.LeftIsDown() )
        localrealbutt |= GR_M_LEFT_DOWN;

    if( event.MiddleIsDown() )
        localrealbutt |= GR_M_MIDDLE_DOWN;

    if( event.LeftDown() )
        localbutt = GR_M_LEFT_DOWN;

    if( event.ButtonDClick( 1 ) )
        localbutt = GR_M_LEFT_DOWN | GR_M_DCLICK;

    if( event.MiddleDown() )
        localbutt = GR_M_MIDDLE_DOWN;

    if( event.ButtonDClick( 2 ) )
    {
    }
    ;                               // Unused

    localrealbutt |= localbutt;     /* compensation defaut wxGTK */

    /* Compute absolute m_MousePosition in pixel units: */
    screen->m_MousePositionInPixels = CalcUnscrolledPosition( event.GetPosition() );

    /* Compute absolute m_MousePosition in user units: */
    screen->m_MousePosition = CursorRealPosition( screen->m_MousePositionInPixels );

    wxClientDC DC( this );
    int        kbstat = 0;

    DC.SetBackground( *wxBLACK_BRUSH );
    PrepareGraphicContext( &DC );

    g_KeyPressed = localkey;

    if( event.ShiftDown() )
        kbstat |= GR_KB_SHIFT;
    if( event.ControlDown() )
        kbstat |= GR_KB_CTRL;
    if( event.AltDown() )
        kbstat |= GR_KB_ALT;

    g_MouseOldButtons = localrealbutt;

    // Calling Double Click and Click functions :
    if( localbutt == (int) (GR_M_LEFT_DOWN | GR_M_DCLICK) )
    {
        m_Parent->OnLeftDClick( &DC, screen->m_MousePositionInPixels );

        // inhibit a response to the mouse left button release,
        // because we have a double click, and we do not want a new OnLeftClick command at end of this Double Click
        s_IgnoreNextLeftButtonRelease = true;
    }
    else if( event.LeftUp() )
    {
        if( screen->BlockLocate.m_State==STATE_NO_BLOCK     // A block command is in progress: a left up is the end of block
            && !s_IgnoreNextLeftButtonRelease )             // This is the end of a double click, already seen
            m_Parent->OnLeftClick( &DC, screen->m_MousePositionInPixels );

        s_IgnoreNextLeftButtonRelease = false;
    }

    if( !event.LeftIsDown() )
    {
        /* be sure there is a response to a left button release command
         * even when a LeftUp event is not seen
         * happens when a double click opens a dialog box, and the release mouse button is made when the dialog box is open
         */
        s_IgnoreNextLeftButtonRelease = false;
    }

    if( event.ButtonUp( 2 ) && (screen->BlockLocate.m_State == STATE_NO_BLOCK) )
    {
        // The middle button has been relached, with no block command:
        // We use it for a zoom center at cursor position command
        wxCommandEvent cmd( wxEVT_COMMAND_MENU_SELECTED,
                            ID_POPUP_ZOOM_CENTER );
        cmd.SetEventObject( this );
        GetEventHandler()->ProcessEvent( cmd );
    }


    /* Calling the  general function on mouse changes (and pseudo key commands) */
    m_Parent->GeneralControle( &DC, screen->m_MousePositionInPixels );


    /*******************************/
    /* Control of block commands : */
    /*******************************/

    // Command block can't start if mouse is dragging a new panel
    if( LastPanel != this )
    {
        MinDragEventCount = 0;
        m_CanStartBlock   = -1;
    }

    /* A new command block can start after a release buttons
     * and if the drag is enougth
     * This is to avoid a false start block when a dialog box is demiss,
     * or when changing panels in hierachy navigation
     * or when clicking while and moving mouse
     */
    if( !event.LeftIsDown() && !event.MiddleIsDown() )
    {
        MinDragEventCount = 0;
        m_CanStartBlock   = 0;

        /* remember the last cursor position when a drag mouse starts
         * this is the last postion ** before ** clicking a button
         * this is usefull to start a block command from the point where the mouse was clicked first
         * (a filter creates a delay for the real block command start, and we must remember this point)
         */
        m_CursorStartPos = screen->m_Curseur;
    }

    if( m_Block_Enable && !(localbutt & GR_M_DCLICK) )
    {
        if( (screen->BlockLocate.m_Command == BLOCK_IDLE)
            || (screen->BlockLocate.m_State == STATE_NO_BLOCK) )
        {
            screen->BlockLocate.SetOrigin( m_CursorStartPos );
        }
        if( event.LeftDown() || event.MiddleDown() )
        {
            if( screen->BlockLocate.m_State == STATE_BLOCK_MOVE )
            {
                m_AutoPAN_Request = FALSE;
                m_Parent->HandleBlockPlace( &DC );
                s_IgnoreNextLeftButtonRelease = true;
            }
        }
        else if( (m_CanStartBlock >= 0 )
                 && ( event.LeftIsDown() || event.MiddleIsDown() )
                 && ManageCurseur == NULL
                 && ForceCloseManageCurseur == NULL )
        {       // Mouse is dragging: if no block in progress:  start a block command
            if( screen->BlockLocate.m_State == STATE_NO_BLOCK )
            {   //  Start a block command
                int cmd_type = kbstat;

                if( event.MiddleIsDown() )
                    cmd_type |= MOUSE_MIDDLE;

                /* A block command is started if the drag is enought.
                 * A small drag is ignored (it is certainly a little mouse move when clicking)
                 * not really a drag mouse
                 */
                if( MinDragEventCount < MIN_DRAG_COUNT_FOR_START_BLOCK_COMMAND )
                    MinDragEventCount++;
                else
                {
                    if( !m_Parent->HandleBlockBegin( &DC, cmd_type, m_CursorStartPos ) )
                    {
                        // should not occurs: error
                        m_Parent->DisplayToolMsg(
                            wxT( "WinEDA_DrawPanel::OnMouseEvent() Block Error" ) );
                    }
                    else
                    {
                        m_AutoPAN_Request = TRUE;
                        SetCursor( m_PanelCursor = wxCURSOR_SIZING );
                    }
                }
            }
        }

        if( event.ButtonUp( 1 ) || event.ButtonUp( 2 ) )
        {
            /* Release the mouse button: end of block.
             * The command can finish (DELETE) or have a next command (MOVE, COPY).
             * However the block command is cancelled if the block size is small
             * Because a block command filtering is already made, this case happens,
             * but only when the on grid cursor has not moved.
             */
            #define BLOCK_MINSIZE_LIMIT 1
            bool BlockIsSmall =
                ( ABS( screen->Scale( screen->BlockLocate.GetWidth() ) ) < BLOCK_MINSIZE_LIMIT)
                  && ( ABS( screen->Scale( screen->BlockLocate.GetHeight() ) ) < BLOCK_MINSIZE_LIMIT);

            if( (screen->BlockLocate.m_State != STATE_NO_BLOCK) && BlockIsSmall )
            {
                if( ForceCloseManageCurseur )
                {
                    ForceCloseManageCurseur( this, &DC );
                    m_AutoPAN_Request = FALSE;
                }
                SetCursor( m_PanelCursor = m_PanelDefaultCursor );
            }
            else if( screen->BlockLocate.m_State == STATE_BLOCK_END )
            {
                m_AutoPAN_Request = FALSE;
                m_Parent->HandleBlockEnd( &DC );
                SetCursor( m_PanelCursor = m_PanelDefaultCursor );
                if( screen->BlockLocate.m_State == STATE_BLOCK_MOVE )
                {
                    m_AutoPAN_Request = TRUE;
                    SetCursor( m_PanelCursor = wxCURSOR_HAND );
                }
            }
        }
    }

    // End of block command on a double click
    // To avoid an unwanted block move command if the move is moved while double click
    if( localbutt == (int) (GR_M_LEFT_DOWN | GR_M_DCLICK) )
    {
        if( screen->BlockLocate.m_Command != BLOCK_IDLE )
        {
            if( ForceCloseManageCurseur )
            {
                ForceCloseManageCurseur( this, &DC );
                m_AutoPAN_Request = FALSE;
            }
        }
    }


#if 0
    wxString msg_debug;
    msg_debug.Printf( " block state %d, cmd %d",
                      screen->BlockLocate.m_State, screen->BlockLocate.m_Command );
    m_Parent->PrintMsg( msg_debug );
#endif

    LastPanel = this;
}


/****************************************************/
void WinEDA_DrawPanel::OnKeyEvent( wxKeyEvent& event )
/****************************************************/
{
    long    key, localkey;
    bool    escape = FALSE;
    wxPoint pos;

    key = localkey = event.GetKeyCode();

    switch( localkey )
    {
    case WXK_CONTROL:
    case WXK_CAPITAL:
    case WXK_SHIFT:
    case WXK_NUMLOCK:
    case WXK_LBUTTON:
    case WXK_RBUTTON:
    case WXK_ALT:
        return;

    case WXK_ESCAPE:
        escape = m_AbortRequest = TRUE;
        break;
    }

    if( event.ControlDown() )
        localkey |= GR_KB_CTRL;
    if( event.AltDown() )
        localkey |= GR_KB_ALT;
    if( event.ShiftDown() && (key > 256) )
        localkey |= GR_KB_SHIFT;

    wxClientDC DC( this );
    BASE_SCREEN* Screen = GetScreen();

    PrepareGraphicContext( &DC );

    g_KeyPressed = localkey;

    if( escape )
    {
        if( ManageCurseur && ForceCloseManageCurseur )
        {
            SetCursor( m_PanelCursor = m_PanelDefaultCursor );
            ForceCloseManageCurseur( this, &DC );
            SetCursor( m_PanelCursor = m_PanelDefaultCursor );
        }
        else
        {
            m_PanelCursor = m_PanelDefaultCursor = wxCURSOR_ARROW;
            m_Parent->SetToolID( 0, m_PanelCursor, wxEmptyString );
        }
    }

    /* Some key commands use the current mouse position: refresh it */
    pos = CalcUnscrolledPosition( wxGetMousePosition() - GetScreenPosition() );

    /* Compute absolute mouse position in pixel units (i.e. considering the
       current scrool) : */
    Screen->m_MousePositionInPixels = pos;

    /* Compute absolute mouse position in user units: */
    Screen->m_MousePosition = CursorRealPosition( pos );

    m_Parent->GeneralControle( &DC, pos );

#if 0
    event.Skip();   // Allow menu shortcut processing
#endif
}

void WinEDA_DrawPanel::OnPan( wxCommandEvent& event )
{
    int        x, y;

    GetViewStart( &x, &y );       // x and y are in scroll units, not in pixels

    switch( event.GetId() )
    {
    case ID_PAN_UP:
        y -= m_ScrollButt_unit;
        break;

    case ID_PAN_DOWN:
        y += m_ScrollButt_unit;
        break;

    case ID_PAN_LEFT:
        x -= m_ScrollButt_unit;
        break;

    case ID_PAN_RIGHT:
        x += m_ScrollButt_unit;
        break;

    default:
        wxLogDebug( wxT( "Unknown ID %d in WinEDA_DrawPanel::OnPan()." ),
                    event.GetId() );
    }

    Scroll( x, y );
    MouseToCursorSchema();
}
