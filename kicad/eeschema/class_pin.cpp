/*****************/
/* class_pin.cpp */
/*****************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "gr_basic.h"
#include "common.h"
#include "class_drawpanel.h"
#include "drawtxt.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
#include "plot_common.h"
#include "protos.h"


/**********************************************************************************************/
void LibDrawPin::Draw( WinEDA_DrawPanel* aPanel,
                       wxDC*             aDC,
                       const wxPoint&    aOffset,
                       int               aColor,
                       int               aDrawMode,
                       void*             aData,
                       const int         aTransformMatrix[2][2] )
/**********************************************************************************************/
{
    // Invisibles pins are only drawn on request.
    // But in libedit they are drawn in g_InvisibleItemColor because we must see them
    WinEDA_SchematicFrame* frame =
        (WinEDA_SchematicFrame*) wxGetApp().GetTopWindow();

    if( ( m_Attributs & PINNOTDRAW ) )
    {
        if( frame->m_LibeditFrame && frame->m_LibeditFrame->IsActive() )
            aColor = g_InvisibleItemColor;
        else if( !g_ShowAllPins )
            return;
    }


    EDA_LibComponentStruct* Entry = ( (DrawPinPrms*) aData )->m_Entry;
    bool    DrawPinText = ( (DrawPinPrms*) aData )->m_DrawPinText;

    /* Calculate Pin orient takin in account the component orientation */
    int     orient = ReturnPinDrawOrient( aTransformMatrix );

    /* Calculate the pin position */
    wxPoint pos1 = TransformCoordinate( aTransformMatrix, m_Pos ) + aOffset;

    /* Dessin de la pin et du symbole special associe */
    DrawPinSymbol( aPanel, aDC, pos1, orient, aDrawMode, aColor );

    if( DrawPinText )
    {
        DrawPinTexts( aPanel, aDC, pos1, orient,
                      Entry->m_TextInside,
                      Entry->m_DrawPinNum, Entry->m_DrawPinName,
                      aColor, aDrawMode );
    }
}


/********************************************************************************/
void LibDrawPin::DrawPinSymbol( WinEDA_DrawPanel* aPanel,
                                wxDC*             aDC,
                                const wxPoint&    aPinPos,
                                int               aOrient,
                                int               aDrawMode,
                                int               aColor )
/*******************************************************************************/

/* Draw the pin symbol (without texts)
 *  if Color != 0 draw with Color, else with the normal pin color
 */
{
    int MapX1, MapY1, x1, y1;
    int color;
    int width = MAX( m_Width, g_DrawMinimunLineWidth );
    int posX  = aPinPos.x, posY = aPinPos.y, len = m_PinLen;


    color = ReturnLayerColor( LAYER_PIN );
    if( aColor < 0 )       // Used normal color or selected color
    {
        if( (m_Selected & IS_SELECTED) )
            color = g_ItemSelectetColor;
    }
    else
        color = aColor;

    GRSetDrawMode( aDC, aDrawMode );

    MapX1 = MapY1 = 0; x1 = posX; y1 = posY;

    switch( aOrient )
    {
    case PIN_UP:
        y1 = posY - len; MapY1 = 1;
        break;

    case PIN_DOWN:
        y1 = posY + len; MapY1 = -1;
        break;

    case PIN_LEFT:
        x1 = posX - len, MapX1 = 1;
        break;

    case PIN_RIGHT:
        x1 = posX + len; MapX1 = -1;
        break;
    }

    if( m_PinShape & INVERT )
    {
        GRCircle( &aPanel->m_ClipBox, aDC, MapX1 * INVERT_PIN_RADIUS + x1,
                  MapY1 * INVERT_PIN_RADIUS + y1,
                  INVERT_PIN_RADIUS, width, color );

        GRMoveTo( MapX1 * INVERT_PIN_RADIUS * 2 + x1,
                  MapY1 * INVERT_PIN_RADIUS * 2 + y1 );
        GRLineTo( &aPanel->m_ClipBox, aDC, posX, posY, width, color );
    }
    else
    {
        GRMoveTo( x1, y1 );
        GRLineTo( &aPanel->m_ClipBox, aDC, posX, posY, width, color );
    }

    if( m_PinShape & CLOCK )
    {
        if( MapY1 == 0 ) /* MapX1 = +- 1 */
        {
            GRMoveTo( x1, y1 + CLOCK_PIN_DIM );
            GRLineTo( &aPanel->m_ClipBox,
                      aDC,
                      x1 - MapX1 * CLOCK_PIN_DIM,
                      y1,
                      width,
                      color );
            GRLineTo( &aPanel->m_ClipBox,
                      aDC,
                      x1,
                      y1 - CLOCK_PIN_DIM,
                      width,
                      color );
        }
        else    /* MapX1 = 0 */
        {
            GRMoveTo( x1 + CLOCK_PIN_DIM, y1 );
            GRLineTo( &aPanel->m_ClipBox,
                      aDC,
                      x1,
                      y1 - MapY1 * CLOCK_PIN_DIM,
                      width,
                      color );
            GRLineTo( &aPanel->m_ClipBox,
                      aDC,
                      x1 - CLOCK_PIN_DIM,
                      y1,
                      width,
                      color );
        }
    }

    if( m_PinShape & LOWLEVEL_IN )  /* IEEE symbol "Active Low Input" */
    {
        if( MapY1 == 0 )            /* MapX1 = +- 1 */
        {
            GRMoveTo( x1 + MapX1 * IEEE_SYMBOL_PIN_DIM * 2, y1 );
            GRLineTo( &aPanel->m_ClipBox,
                      aDC,
                      x1 + MapX1 * IEEE_SYMBOL_PIN_DIM * 2,
                      y1 - IEEE_SYMBOL_PIN_DIM,
                      width,
                      color );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1, y1, width, color );
        }
        else    /* MapX1 = 0 */
        {
            GRMoveTo( x1, y1 + MapY1 * IEEE_SYMBOL_PIN_DIM * 2 );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1 - IEEE_SYMBOL_PIN_DIM,
                      y1 + MapY1 * IEEE_SYMBOL_PIN_DIM * 2, width, color );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1, y1, width, color );
        }
    }


    if( m_PinShape & LOWLEVEL_OUT ) /* IEEE symbol "Active Low Output" */
    {
        if( MapY1 == 0 )            /* MapX1 = +- 1 */
        {
            GRMoveTo( x1, y1 - IEEE_SYMBOL_PIN_DIM );
            GRLineTo( &aPanel->m_ClipBox,
                      aDC,
                      x1 + MapX1 * IEEE_SYMBOL_PIN_DIM * 2,
                      y1,
                      width,
                      color );
        }
        else    /* MapX1 = 0 */
        {
            GRMoveTo( x1 - IEEE_SYMBOL_PIN_DIM, y1 );
            GRLineTo( &aPanel->m_ClipBox,
                      aDC,
                      x1,
                      y1 + MapY1 * IEEE_SYMBOL_PIN_DIM * 2,
                      width,
                      color );
        }
    }

    /* Draw the pin end target (active end of the pin) */
    if( !g_IsPrinting )  // Draw but do not print the pin end target 1 pixel width */
        GRCircle( &aPanel->m_ClipBox,
                  aDC,
                  posX,
                  posY,
                  TARGET_PIN_DIAM,
                  0,
                  color );
}


/*****************************************************************************
*  Put out pin number and pin text info, given the pin line coordinates.
*  The line must be vertical or horizontal.
*  If PinText == NULL nothing is printed. If PinNum = 0 no number is printed.
*  Current Zoom factor is taken into account.
*  If TextInside then the text is been put inside,otherwise all is drawn outside.
*  Pin Name:	substring beteween '~' is negated
*****************************************************************************/
void LibDrawPin::DrawPinTexts( WinEDA_DrawPanel* panel,
                               wxDC*             DC,
                               wxPoint&          pin_pos,
                               int               orient,
                               int               TextInside,
                               bool              DrawPinNum,
                               bool              DrawPinName,
                               int               Color,
                               int               DrawMode )
/* DrawMode = GR_OR, XOR ... */
{
    int      ii, x, y, x1, y1, dx, dy, len;
    wxString StringPinNum;
    wxString PinText;
    int      PinTextBarPos[256];
    int      PinTextBarCount;
    EDA_Colors       NameColor, NumColor;
    int      PinTxtLen;

    wxSize   PinNameSize( m_PinNameSize, m_PinNameSize );
    wxSize   PinNumSize( m_PinNumSize, m_PinNumSize );

    int      LineWidth = g_DrawMinimunLineWidth;

    GRSetDrawMode( DC, DrawMode );

    /* Get the num and name colors */
    if( (Color < 0) && (m_Selected & IS_SELECTED) )
        Color = g_ItemSelectetColor;
    NameColor = (EDA_Colors) (Color == -1 ? ReturnLayerColor( LAYER_PINNAM ) : Color);
    NumColor  = (EDA_Colors) (Color == -1 ? ReturnLayerColor( LAYER_PINNUM ) : Color);

    /* Create the pin num string */
    ReturnPinStringNum( StringPinNum );

    x1 = pin_pos.x; y1 = pin_pos.y;

    switch( orient )
    {
    case PIN_UP:
        y1 -= m_PinLen; break;

    case PIN_DOWN:
        y1 += m_PinLen; break;

    case PIN_LEFT:
        x1 -= m_PinLen; break;

    case PIN_RIGHT:
        x1 += m_PinLen; break;
    }

    const wxChar* textsrc = m_PinName.GetData();
    float         fPinTextPitch = (PinNameSize.x * 1.1) + LineWidth;
    /* Do we need to invert the string? Is this string has only "~"? */
    PinTextBarCount = 0; PinTxtLen = 0;
    ii = 0;
    while( *textsrc )
    {
        if( *textsrc == '~' )
        {
            PinTextBarPos[PinTextBarCount++] =
                (int) ( PinTxtLen * fPinTextPitch );
        }
        else
        {
            PinText.Append( *textsrc );
            PinTxtLen++;
        }

        textsrc++;
    }

    PinTxtLen = (int) ( fPinTextPitch * PinTxtLen );
    PinTextBarPos[PinTextBarCount] = PinTxtLen; // Needed if no end '~'

    if( PinText[0] == 0 )
        DrawPinName = FALSE;

    if( TextInside )  /* Draw the text inside, but the pin numbers outside. */
    {
        if( (orient == PIN_LEFT) || (orient == PIN_RIGHT) )

        // It is an horizontal line
        {
            if( PinText && DrawPinName )
            {
                if( orient == PIN_RIGHT )
                {
                    x = x1 + TextInside;
                    DrawGraphicText( panel, DC, wxPoint( x,
                                                         y1 ), NameColor,
                                     PinText,
                                     TEXT_ORIENT_HORIZ,
                                     PinNameSize,
                                     GR_TEXT_HJUSTIFY_LEFT,
                                     GR_TEXT_VJUSTIFY_CENTER, LineWidth );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        GRMoveTo( x, y1 - TXTMARGE );
                        dy = -PinNameSize.y / 2;
                        GRMoveRel( 0, dy );
                        dx = PinTextBarPos[ii++];       // Get the line pos
                        GRMoveRel( dx, 0 );
                        len = PinTextBarPos[ii++] - dx; // Get the line length
                        GRLineRel( &panel->m_ClipBox,
                                   DC,
                                   len,
                                   0,
                                   LineWidth,
                                   NameColor );
                    }
                }
                else    // Orient == PIN_LEFT
                {
                    x = x1 - TextInside;
                    DrawGraphicText( panel, DC, wxPoint( x,
                                                         y1 ), NameColor,
                                     PinText,
                                     TEXT_ORIENT_HORIZ,
                                     PinNameSize,
                                     GR_TEXT_HJUSTIFY_RIGHT,
                                     GR_TEXT_VJUSTIFY_CENTER, LineWidth );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        GRMoveTo( x, y1 - TXTMARGE );
                        dy = -PinNameSize.y / 2;
                        GRMoveRel( 0, dy );
                        dx = PinTextBarPos[ii++];       // Get the line pos
                        GRMoveRel( dx - PinTxtLen, 0 );
                        len = PinTextBarPos[ii++] - dx; // Get the line length
                        GRLineRel( &panel->m_ClipBox,
                                   DC,
                                   len,
                                   0,
                                   LineWidth,
                                   NameColor );
                    }
                }
            }

            if( DrawPinNum )
            {
                DrawGraphicText( panel, DC,
                                 wxPoint( (x1 + pin_pos.x) / 2,
                                         y1 - TXTMARGE ), NumColor,
                                 StringPinNum,
                                 TEXT_ORIENT_HORIZ, PinNumSize,
                                 GR_TEXT_HJUSTIFY_CENTER,
                                 GR_TEXT_VJUSTIFY_BOTTOM, LineWidth );
            }
        }
        else            /* Its a vertical line. */
        {
            // Text is drawn from bottom to top (i.e. to negative value for Y axis)
            if( PinText && DrawPinName )
            {
                if( orient == PIN_DOWN )
                {
                    y = y1 + TextInside;

                    DrawGraphicText( panel, DC, wxPoint( x1,
                                                         y ), NameColor,
                                     PinText,
                                     TEXT_ORIENT_VERT, PinNameSize,
                                     GR_TEXT_HJUSTIFY_CENTER,
                                     GR_TEXT_VJUSTIFY_TOP, LineWidth );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        GRMoveTo( x1 - TXTMARGE, y );
                        dy = -PinNameSize.y / 2;
                        GRMoveRel( dy, 0 );
                        dx = PinTextBarPos[ii++];       // Get the line pos
                        GRMoveRel( 0, PinTxtLen - dx );
                        len = PinTextBarPos[ii++] - dx; // Get the line length
                        GRLineRel( &panel->m_ClipBox,
                                   DC,
                                   0,
                                   -len,
                                   LineWidth,
                                   NameColor );
                    }
                }
                else    /* PIN_UP */
                {
                    y = y1 - TextInside;

                    DrawGraphicText( panel, DC, wxPoint( x1,
                                                         y ), NameColor,
                                     PinText,
                                     TEXT_ORIENT_VERT, PinNameSize,
                                     GR_TEXT_HJUSTIFY_CENTER,
                                     GR_TEXT_VJUSTIFY_BOTTOM, LineWidth );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        GRMoveTo( x1 - TXTMARGE, y );
                        dy = -PinNameSize.y / 2;
                        GRMoveRel( dy, 0 );
                        dx = PinTextBarPos[ii++];       // Get the line pos
                        GRMoveRel( 0, -dx );
                        len = PinTextBarPos[ii++] - dx; // Get the line length
                        GRLineRel( &panel->m_ClipBox,
                                   DC,
                                   0,
                                   -len,
                                   LineWidth,
                                   NameColor );
                    }
                }
            }

            if( DrawPinNum )
            {
                DrawGraphicText( panel, DC,
                                 wxPoint( x1 - TXTMARGE,
                                          (y1 + pin_pos.y) / 2 ), NumColor,
                                 StringPinNum,
                                 TEXT_ORIENT_VERT, PinNumSize,
                                 GR_TEXT_HJUSTIFY_RIGHT,
                                 GR_TEXT_VJUSTIFY_CENTER, LineWidth );
            }
        }
    }
    else     /**** Draw num & text pin outside  ****/
    {
        if( (orient == PIN_LEFT) || (orient == PIN_RIGHT) )
        /* Its an horizontal line. */
        {
            if( PinText && DrawPinName )
            {
                x = (x1 + pin_pos.x) / 2;
                DrawGraphicText( panel, DC, wxPoint( x,
                                                     y1 - TXTMARGE ),
                                 NameColor, PinText,
                                 TEXT_ORIENT_HORIZ, PinNameSize,
                                 GR_TEXT_HJUSTIFY_CENTER,
                                 GR_TEXT_VJUSTIFY_BOTTOM, LineWidth );

                for( ii = 0; ii < PinTextBarCount; )
                {
                    GRMoveTo( x, y1 - TXTMARGE * 2 );
                    GRMoveRel( -PinTxtLen / 2, -PinNameSize.y );
                    dx = PinTextBarPos[ii++];       // Get the line pos
                    GRMoveRel( dx, 0 );
                    len = PinTextBarPos[ii++] - dx; // Get the line length
                    GRLineRel( &panel->m_ClipBox,
                               DC,
                               len,
                               0,
                               LineWidth,
                               NameColor );
                }
            }
            if( DrawPinNum )
            {
                x = (x1 + pin_pos.x) / 2;
                DrawGraphicText( panel, DC, wxPoint( x,
                                                     y1 + TXTMARGE ),
                                 NumColor, StringPinNum,
                                 TEXT_ORIENT_HORIZ, PinNumSize,
                                 GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_TOP,
                                 LineWidth );
            }
        }
        else     /* Its a vertical line. */
        {
            if( PinText && DrawPinName )
            {
                y = (y1 + pin_pos.y) / 2;
                DrawGraphicText( panel, DC, wxPoint( x1 - TXTMARGE,
                                                     y ),
                                 NameColor, PinText,
                                 TEXT_ORIENT_VERT, PinNameSize,
                                 GR_TEXT_HJUSTIFY_RIGHT,
                                 GR_TEXT_VJUSTIFY_CENTER, LineWidth );

                for( ii = 0; ii < PinTextBarCount; )
                {
                    GRMoveTo( x1 - (TXTMARGE * 2), y );
                    GRMoveRel( -PinNameSize.y, -PinTxtLen / 2 );
                    dx = PinTextBarPos[ii++];       // Get the line pos
                    GRMoveRel( 0, PinTxtLen - dx );
                    len = PinTextBarPos[ii++] - dx; // Get the line length
                    GRLineRel( &panel->m_ClipBox,
                               DC,
                               0,
                               -len,
                               LineWidth,
                               NameColor );
                }
            }

            if( DrawPinNum )
            {
                DrawGraphicText( panel, DC,
                                 wxPoint( x1 + TXTMARGE,
                                          (y1 + pin_pos.y) / 2 ),
                                 NumColor, StringPinNum,
                                 TEXT_ORIENT_VERT, PinNumSize,
                                 GR_TEXT_HJUSTIFY_LEFT,
                                 GR_TEXT_VJUSTIFY_CENTER, LineWidth );
            }
        }
    }
}


extern void Move_Plume( wxPoint pos, int plume ); // see plot.cpp


/*****************************************************************************
* Plot pin number and pin text info, given the pin line coordinates.	  *
* Same as DrawPinTexts((), but output is the plotter
* The line must be vertical or horizontal.						  *
* If PinNext == NULL nothing is printed.									*
* Current Zoom factor is taken into account.					 *
* If TextInside then the text is been put inside (moving from x1, y1 in		 *
* the opposite direction to x2,y2), otherwise all is drawn outside.		 *
*****************************************************************************/
void LibDrawPin::PlotPinTexts( wxPoint& pin_pos,
                               int      orient,
                               int      TextInside,
                               bool     DrawPinNum,
                               bool     DrawPinName,
                                int aWidth, bool aItalic )
{
    int      dx, len, start;
    int      ii, x, y, x1, y1, cte;
    wxString StringPinNum;
    wxString PinText;
    int      PinTextBarPos[256];
    int      PinTextBarCount;
    EDA_Colors       NameColor, NumColor;
    int      PinTxtLen   = 0;
    wxSize   PinNameSize = wxSize( m_PinNameSize, m_PinNameSize );
    wxSize   PinNumSize  = wxSize( m_PinNumSize, m_PinNumSize );
    bool     plot_color  = (g_PlotFormat == PLOT_FORMAT_POST)
                           && g_PlotPSColorOpt;

    /* Get the num and name colors */
    NameColor = (EDA_Colors) (plot_color ? ReturnLayerColor( LAYER_PINNAM ) : -1);
    NumColor  = (EDA_Colors) (plot_color ? ReturnLayerColor( LAYER_PINNUM ) : -1);

    /* Create the pin num string */
    ReturnPinStringNum( StringPinNum );
    x1 = pin_pos.x; y1 = pin_pos.y;

    switch( orient )
    {
    case PIN_UP:
        y1 -= m_PinLen; break;

    case PIN_DOWN:
        y1 += m_PinLen; break;

    case PIN_LEFT:
        x1 -= m_PinLen; break;

    case PIN_RIGHT:
        x1 += m_PinLen; break;
    }

    const wxChar* textsrc = m_PinName.GetData();
    float         fPinTextPitch = (PinNameSize.x * 1.1) + aWidth;
    /* Do we need to invert the string? Is this string has only "~"? */
    PinTextBarCount = 0; PinTxtLen = 0;
    ii = 0;
    while( *textsrc )
    {
        if( *textsrc == '~' )
        {
            PinTextBarPos[PinTextBarCount++] =
                (int) ( fPinTextPitch * PinTxtLen );
        }
        else
        {
            PinText.Append( *textsrc );
            PinTxtLen++;
        }

        textsrc++;
    }

    PinTxtLen = (int) ( fPinTextPitch * PinTxtLen );
    PinTextBarPos[PinTextBarCount] = PinTxtLen; // Needed if no end '~'

    if( PinText[0] == 0 )
        DrawPinName = FALSE;

    if( TextInside )  /* Draw the text inside, but the pin numbers outside. */
    {
        if( (orient == PIN_LEFT) || (orient == PIN_RIGHT) )
        {     /* Its an horizontal line. */
            if( PinText && DrawPinName )
            {
                if( orient == PIN_RIGHT )
                {
                    x = x1 + TextInside;
                    PlotGraphicText( g_PlotFormat, wxPoint( x,
                                                            y1 ), NameColor,
                                     PinText,
                                     TEXT_ORIENT_HORIZ,
                                     PinNameSize,
                                     GR_TEXT_HJUSTIFY_LEFT,
                                     GR_TEXT_VJUSTIFY_CENTER,
                                    aWidth, aItalic );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        cte = y1 - PinNameSize.y / 2 - TXTMARGE;
                        dx  = PinTextBarPos[ii++];  // Get the line pos
                        Move_Plume( wxPoint( x + dx, cte ), 'U' );
                        len = PinTextBarPos[ii++];  // Get the line end
                        Move_Plume( wxPoint( x + len, cte ), 'D' );
                    }
                }
                else    // orient == PIN_LEFT
                {
                    x = x1 - TextInside;
                    PlotGraphicText( g_PlotFormat, wxPoint( x,
                                                            y1 ),
                                     NameColor, PinText, TEXT_ORIENT_HORIZ,
                                     PinNameSize,
                                     GR_TEXT_HJUSTIFY_RIGHT,
                                     GR_TEXT_VJUSTIFY_CENTER,
                                    aWidth, aItalic );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        cte = y1 - PinNameSize.y / 2 - TXTMARGE;
                        dx  = PinTextBarPos[ii++];  // Get the line pos
                        Move_Plume( wxPoint( x + dx - PinTxtLen, cte ), 'U' );
                        len = PinTextBarPos[ii++];  // Get the line end
                        Move_Plume( wxPoint( x + len - PinTxtLen, cte ), 'D' );
                    }
                }
            }

            if( DrawPinNum )
            {
                PlotGraphicText( g_PlotFormat,
                                 wxPoint( (x1 + pin_pos.x) / 2,
                                         y1 - TXTMARGE ),
                                 NumColor, StringPinNum,
                                 TEXT_ORIENT_HORIZ, PinNumSize,
                                 GR_TEXT_HJUSTIFY_CENTER,
                                 GR_TEXT_VJUSTIFY_BOTTOM,
                                aWidth, aItalic );
            }
        }
        else         /* Its a vertical line. */
        {
            if( PinText && DrawPinName )
            {
                if( orient == PIN_DOWN )
                {
                    y = y1 + TextInside;

                    PlotGraphicText( g_PlotFormat, wxPoint( x1,
                                                            y ), NameColor,
                                     PinText,
                                     TEXT_ORIENT_VERT, PinNameSize,
                                     GR_TEXT_HJUSTIFY_CENTER,
                                     GR_TEXT_VJUSTIFY_TOP,
                                    aWidth, aItalic);

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        cte = x1 - PinNameSize.y / 2 - TXTMARGE;
                        dx  = PinTextBarPos[ii++];  // Get the line pos
                        Move_Plume( wxPoint( cte, y + PinTxtLen - dx ), 'U' );
                        len = PinTextBarPos[ii++];  // Get the line end
                        Move_Plume( wxPoint( cte, y + PinTxtLen - len ), 'D' );
                    }
                }
                else    /* PIN_UP */
                {
                    y = y1 - TextInside;

                    PlotGraphicText( g_PlotFormat, wxPoint( x1,
                                                            y ), NameColor,
                                     PinText,
                                     TEXT_ORIENT_VERT, PinNameSize,
                                     GR_TEXT_HJUSTIFY_CENTER,
                                     GR_TEXT_VJUSTIFY_BOTTOM,
                                    aWidth, aItalic);

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        cte = x1 - PinNameSize.y / 2 - TXTMARGE;
                        dx  = PinTextBarPos[ii++];  // Get the line pos
                        Move_Plume( wxPoint( cte, y - dx ), 'U' );
                        len = PinTextBarPos[ii++];  // Get the line end
                        Move_Plume( wxPoint( cte, y - len ), 'D' );
                    }
                }
            }

            if( DrawPinNum )
            {
                PlotGraphicText( g_PlotFormat,
                                 wxPoint( x1 - TXTMARGE,
                                          (y1 + pin_pos.y) / 2 ),
                                 NumColor, StringPinNum,
                                 TEXT_ORIENT_VERT, PinNumSize,
                                 GR_TEXT_HJUSTIFY_RIGHT,
                                 GR_TEXT_VJUSTIFY_CENTER,
                                aWidth, aItalic);
            }
        }
    }
    else     /* Draw num & text pin outside */
    {
        if( (orient == PIN_LEFT) || (orient == PIN_RIGHT) )
        /* Its an horizontal line. */
        {
            if( PinText && DrawPinName )
            {
                x = (x1 + pin_pos.x) / 2;
                PlotGraphicText( g_PlotFormat, wxPoint( x,
                                                        y1 - TXTMARGE ),
                                 NameColor, PinText,
                                 TEXT_ORIENT_HORIZ, PinNameSize,
                                 GR_TEXT_HJUSTIFY_CENTER,
                                 GR_TEXT_VJUSTIFY_BOTTOM,
                                aWidth, aItalic);

                for( ii = 0; ii < PinTextBarCount; )
                {
                    cte   = y1 - PinNameSize.y - TXTMARGE * 2;
                    start = x - (PinTxtLen / 2);
                    dx    = PinTextBarPos[ii++];    // Get the line pos
                    Move_Plume( wxPoint( start + dx, cte ), 'U' );
                    len = PinTextBarPos[ii++];      // Get the line end
                    Move_Plume( wxPoint( start + len, cte ), 'D' );
                }
            }
            if( DrawPinNum )
            {
                x = (x1 + pin_pos.x) / 2;
                PlotGraphicText( g_PlotFormat, wxPoint( x, y1 + TXTMARGE ),
                                 NumColor, StringPinNum,
                                 TEXT_ORIENT_HORIZ, PinNumSize,
                                 GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_TOP,
                                aWidth, aItalic);
            }
        }
        else     /* Its a vertical line. */
        {
            if( PinText && DrawPinName )
            {
                y = (y1 + pin_pos.y) / 2;
                PlotGraphicText( g_PlotFormat, wxPoint( x1 - TXTMARGE,
                                                        y ),
                                 NameColor, PinText,
                                 TEXT_ORIENT_VERT, PinNameSize,
                                 GR_TEXT_HJUSTIFY_RIGHT,
                                 GR_TEXT_VJUSTIFY_CENTER,
                                aWidth, aItalic);

                for( ii = 0; ii < PinTextBarCount; )
                {
                    cte   = x1 - PinNameSize.y - TXTMARGE * 2;
                    start = y + (PinTxtLen / 2);
                    dx    = PinTextBarPos[ii++];    // Get the line pos
                    Move_Plume( wxPoint( cte, start - dx ), 'U' );
                    len = PinTextBarPos[ii++];      // Get the line end
                    Move_Plume( wxPoint( cte, start - len ), 'D' );
                }
            }

            if( DrawPinNum )
            {
                PlotGraphicText( g_PlotFormat,
                                 wxPoint( x1 + TXTMARGE,
                                          (y1 + pin_pos.y) / 2 ),
                                 NumColor, StringPinNum,
                                 TEXT_ORIENT_VERT, PinNumSize,
                                 GR_TEXT_HJUSTIFY_LEFT,
                                 GR_TEXT_VJUSTIFY_CENTER,
                                aWidth, aItalic);
            }
        }
    }
}


/***************************************************************/
LibDrawPin::LibDrawPin() : LibEDA_BaseStruct( COMPONENT_PIN_DRAW_TYPE )
/***************************************************************/
{
    m_PinLen = 300;                     /* default Pin len */
    m_Orient = PIN_RIGHT;               /* Pin oprient: Up, Down, Left, Right */
    m_PinShape    = NONE;               /* Bit a bit: Pin shape (voir enum prec) */
    m_PinType     = PIN_UNSPECIFIED;    /* electrical type of pin */
    m_Attributs   = 0;                  /* bit 0 != 0: pin invisible */
    m_PinNum      = 0;                  /*pin number ( i.e. 4 codes Ascii ) */
    m_PinNumSize  = 50;
    m_PinNameSize = 50;                 /* Default size for pin name and num */
    m_Width = 0;

//	m_PinNumWidth = m_PinNameWidth = 0;	// Unused
}


/******************************************/
wxPoint LibDrawPin::ReturnPinEndPoint()
/******************************************/

/* return the pin end position, for a component in normal orient
 */
{
    wxPoint pos = m_Pos;

    switch( m_Orient )
    {
    case PIN_UP:
        pos.y += m_PinLen; break;

    case PIN_DOWN:
        pos.y -= m_PinLen; break;

    case PIN_LEFT:
        pos.x -= m_PinLen; break;

    case PIN_RIGHT:
        pos.x += m_PinLen; break;
    }

    return pos;
}


/********************************************************/
int LibDrawPin::ReturnPinDrawOrient( const int TransMat[2][2] )
/********************************************************/

/** Function ReturnPinDrawOrient
 * Return the pin real orientation (PIN_UP, PIN_DOWN, PIN_RIGHT, PIN_LEFT),
 *  according to its orientation and the matrix transform (rot, mirror) TransMat
 * @param  TransMat = transform matrix
 */
{
    int     orient;
    wxPoint end;    // position of a end pin starting at 0,0 according to its orientation, lenght = 1

    switch( m_Orient )
    {
    case PIN_UP:
        end.y = 1; break;

    case PIN_DOWN:
        end.y = -1; break;

    case PIN_LEFT:
        end.x = -1; break;

    case PIN_RIGHT:
        end.x = 1; break;
    }

    end    = TransformCoordinate( TransMat, end );    // = pos of end point, accordint to the component orientation
    orient = PIN_UP;
    if( end.x == 0 )
    {
        if( end.y > 0 )
            orient = PIN_DOWN;
    }
    else
    {
        orient = PIN_RIGHT;
        if( end.x < 0 )
            orient = PIN_LEFT;
    }

    return orient;
}


/***********************************************************/
void LibDrawPin::ReturnPinStringNum( wxString& buffer ) const
/***********************************************************/

/* fill the buffer with pin num as a wxString
 *  Pin num is coded as a long
 *  Used to print/draw the pin num
 */
{
    char ascii_buf[5];

    memcpy( ascii_buf, &m_PinNum, 4 );
    ascii_buf[4] = 0;

    buffer = CONV_FROM_UTF8( ascii_buf );
}


/****************************************************/
void LibDrawPin::SetPinNumFromString( wxString& buffer )
/****************************************************/

/* fill the buffer with pin num as a wxString
 *  Pin num is coded as a long
 *  Used to print/draw the pin num
 */
{
    char     ascii_buf[4];
    unsigned ii, len = buffer.Len();

    ascii_buf[0] = ascii_buf[1] = ascii_buf[2] = ascii_buf[3] = 0;
    if( len > 4 )
        len = 4;
    for( ii = 0; ii < len; ii++ )
    {
        ascii_buf[ii] = buffer.GetChar( ii ) & 0xFF;
    }

    strncpy( (char*) &m_PinNum, ascii_buf, 4 );
}


/*************************************/
LibDrawPin* LibDrawPin::GenCopy()
/*************************************/
{
    LibDrawPin* newpin = new LibDrawPin();

    newpin->m_Pos    = m_Pos;
    newpin->m_PinLen = m_PinLen;
    newpin->m_Orient = m_Orient;
    newpin->m_PinShape    = m_PinShape;
    newpin->m_PinType     = m_PinType;
    newpin->m_Attributs   = m_Attributs;
    newpin->m_PinNum      = m_PinNum;
    newpin->m_PinNumSize  = m_PinNumSize;
    newpin->m_PinNameSize = m_PinNameSize;
    newpin->m_Unit        = m_Unit;
    newpin->m_Convert     = m_Convert;
    newpin->m_Flags       = m_Flags;
    newpin->m_Width       = m_Width;

    newpin->m_PinName = m_PinName;

    return newpin;
}
