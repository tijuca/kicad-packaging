/******************************************/
/* Kicad: Common plot Postscript Routines */
/******************************************/

#include "fctsys.h"
#include "gr_basic.h"
#include "trigo.h"
#include "wxstruct.h"
#include "base_struct.h"
#include "common.h"
#include "plot_common.h"
#include "worksheet.h"
#include "macros.h"


// Variables partagees avec Common plot Postscript et HPLG Routines
wxPoint LastPenPosition;
wxPoint PlotOffset;
FILE*   PlotOutputFile;
double  XScale, YScale;
int     g_DefaultPenWidth;
int     g_CurrentPenWidth = -1;
int     PlotOrientOptions, etat_plume;


// Locales
static Ki_PageDescr* SheetPS;

/*************************/
void ForcePenReinit()
/*************************/

/* set the flag g_CurrentPenWidth to -1 in order to force a pen width redefinition
  * for the next draw command
 */
{
    g_CurrentPenWidth = -1;
}


/**********************************************/
void SetPlotScale( double xscale, double yscale )
/**********************************************/

/* Set the plot scale for the current plotting)
 */
{
    XScale = xscale;
    YScale = yscale;
}


/*********************************/
void SetPlotOffset( wxPoint offset )
/*********************************/

/* Set the plot offset for the current plotting)
 */
{
    PlotOffset = offset;
}


/***************************************************************************/
void InitPlotParametresGERBER( wxPoint offset, double xscale, double yscale )
/***************************************************************************/

/* Set the plot offset for the current plotting
  * xscale,yscale = coordinate scale (scale coefficient for coordinates)
 */
{
    PlotOrientOptions = 0;
    PlotOffset = offset;
    SheetPS    = NULL;
    XScale = xscale;
    YScale = yscale;
    g_DefaultPenWidth = 120;            /* epaisseur du trait standard en 1/1000 pouce */
    g_CurrentPenWidth = -1;
}


/*******************************************************/
void PlotWorkSheet( int format_plot, BASE_SCREEN* screen )
/*******************************************************/

/* Plot sheet references
  * margin is in mils (1/1000 inch)
 */
{
#define WSTEXTSIZE 50   // Text size in mils
    Ki_PageDescr*     Sheet = screen->m_CurrentSheetDesc;
    int               ii, jj, xg, yg, ipas, gxpas, gypas;
    wxSize            PageSize;
    wxPoint           pos, ref;
    int               color;
    Ki_WorkSheetData* WsItem;
    int               conv_unit = screen->GetInternalUnits() / 1000; /* Scale to convert dimension in 1/1000 in into internal units
                                                      * (1/1000 inc for EESchema, 1/10000 for pcbnew */
    wxString          msg;
    wxSize            text_size;
    void              (*FctPlume)( wxPoint pos, int state );
    int               UpperLimit = VARIABLE_BLOCK_START_POSITION;

    switch( format_plot )
    {
    case PLOT_FORMAT_POST:
        FctPlume = LineTo_PS;
        break;

    case PLOT_FORMAT_HPGL:
        FctPlume = Move_Plume_HPGL;
        break;

    case PLOT_FORMAT_GERBER:
    default:
        return;
    }

    color = BLACK;

    PageSize.x = Sheet->m_Size.x;
    PageSize.y = Sheet->m_Size.y;

    /* trace de la bordure */
    ref.x = Sheet->m_LeftMargin * conv_unit;
    ref.y = Sheet->m_TopMargin * conv_unit;                     /* Upper left corner */
    xg    = (PageSize.x - Sheet->m_RightMargin) * conv_unit;
    yg    = (PageSize.y - Sheet->m_BottomMargin) * conv_unit;   /* lower right corner */

    for( ii = 0; ii < 2; ii++ )
    {
        FctPlume( ref, 'U' );
        pos.x = xg; pos.y = ref.y;
        FctPlume( pos, 'D' );
        pos.x = xg; pos.y = yg;
        FctPlume( pos, 'D' );
        pos.x = ref.x; pos.y = yg;
        FctPlume( pos, 'D' );
        FctPlume( ref, 'D' );
        ref.x += GRID_REF_W * conv_unit; ref.y += GRID_REF_W * conv_unit;
        xg -= GRID_REF_W * conv_unit; yg -= GRID_REF_W * conv_unit;
    }

    /* trace des reperes */
    text_size.x = WSTEXTSIZE * conv_unit;
    text_size.y = WSTEXTSIZE * conv_unit;

    ref.x = Sheet->m_LeftMargin;
    ref.y = Sheet->m_TopMargin;                     /* Upper left corner in 1/1000 inch */
    xg    = (PageSize.x - Sheet->m_RightMargin);
    yg    = (PageSize.y - Sheet->m_BottomMargin);   /* lower right corner in 1/1000 inch */

    /* Trace des reperes selon l'axe X */
    ipas  = (xg - ref.x) / PAS_REF;
    gxpas = ( xg - ref.x) / ipas;
    for( ii = ref.x + gxpas, jj = 1; ipas > 0; ii += gxpas, jj++, ipas-- )
    {
        msg.Empty(); msg << jj;
        if( ii < xg - PAS_REF / 2 )
        {
            pos.x = ii * conv_unit; pos.y = ref.y * conv_unit;
            FctPlume( pos, 'U' );
            pos.x = ii * conv_unit; pos.y = (ref.y + GRID_REF_W) * conv_unit;
            FctPlume( pos, 'D' );
        }
        pos.x = (ii - gxpas / 2) * conv_unit;
        pos.y = (ref.y + GRID_REF_W / 2) * conv_unit;
        PlotGraphicText( format_plot, pos, color,
            msg, TEXT_ORIENT_HORIZ, text_size,
            GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_CENTER );

        if( ii < xg - PAS_REF / 2 )
        {
            pos.x = ii * conv_unit; pos.y = yg * conv_unit;
            FctPlume( pos, 'U' );
            pos.x = ii * conv_unit; pos.y = (yg - GRID_REF_W) * conv_unit;
            FctPlume( pos, 'D' );
        }
        pos.x = (ii - gxpas / 2) * conv_unit;
        pos.y = (yg - GRID_REF_W / 2) * conv_unit;
        PlotGraphicText( format_plot, pos, color,
            msg, TEXT_ORIENT_HORIZ, text_size,
            GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_CENTER );
    }

    /* Trace des reperes selon l'axe Y */
    ipas  = (yg - ref.y) / PAS_REF;
    gypas = ( yg - ref.y) / ipas;
    for( ii = ref.y + gypas, jj = 0; ipas > 0; ii += gypas, jj++, ipas-- )
    {
        msg.Empty(); msg << jj;
        if( ii < yg - PAS_REF / 2 )
        {
            pos.x = ref.x * conv_unit; pos.y = ii * conv_unit;
            FctPlume( pos, 'U' );
            pos.x = (ref.x + GRID_REF_W) * conv_unit; pos.y = ii * conv_unit;
            FctPlume( pos, 'D' );
        }
        pos.x = (ref.x + GRID_REF_W / 2) * conv_unit;
        pos.y = (ii - gypas / 2) * conv_unit;
        PlotGraphicText( format_plot, pos, color,
            msg, TEXT_ORIENT_HORIZ, text_size,
            GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_CENTER );

        if( ii < yg - PAS_REF / 2 )
        {
            pos.x = xg * conv_unit; pos.y = ii * conv_unit;
            FctPlume( pos, 'U' );
            pos.x = (xg - GRID_REF_W) * conv_unit; pos.y = ii * conv_unit;
            FctPlume( pos, 'D' );
        }
        pos.x = (xg - GRID_REF_W / 2) * conv_unit;
        pos.y = (ii - gypas / 2) * conv_unit;
        PlotGraphicText( format_plot, pos, color, msg, TEXT_ORIENT_HORIZ, text_size,
            GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_CENTER );
    }

    /* Trace du cartouche */
    text_size.x = SIZETEXT * conv_unit;
    text_size.y = SIZETEXT * conv_unit;
    ref.x = PageSize.x - GRID_REF_W - Sheet->m_RightMargin;
    ref.y = PageSize.y - GRID_REF_W - Sheet->m_BottomMargin;

    for( WsItem = &WS_Date; WsItem != NULL; WsItem = WsItem->Pnext )
    {
        pos.x = (ref.x - WsItem->m_Posx) * conv_unit;
        pos.y = (ref.y - WsItem->m_Posy) * conv_unit;
        if( WsItem->m_Legende )
            msg = WsItem->m_Legende;
        else
            msg.Empty();

        switch( WsItem->m_Type )
        {
        case WS_DATE:
            msg += screen->m_Date;
            break;

        case WS_REV:
            msg += screen->m_Revision;
            break;

        case WS_KICAD_VERSION:
            msg += g_ProductName;
            break;

        case WS_SIZESHEET:
            msg += screen->m_CurrentSheetDesc->m_Name;
            break;

        case WS_IDENTSHEET:
            msg << screen->m_ScreenNumber << wxT( "/" ) << screen->m_NumberOfScreen;
            break;

        case WS_FILENAME:
        {
            wxString fname, fext;
            wxFileName::SplitPath( screen->m_FileName, (wxString*) NULL, &fname, &fext );
            msg << fname << wxT( "." ) << fext;
        }
            break;

        case WS_FULLSHEETNAME:

//				msg += GetScreenDesc();
            break;

        case WS_COMPANY_NAME:
            msg += screen->m_Company;
            if( !msg.IsEmpty() )
                UpperLimit = MAX( UpperLimit, WsItem->m_Posy + SIZETEXT );
            break;

        case WS_TITLE:
            msg += screen->m_Title;
            break;

        case WS_COMMENT1:
            msg += screen->m_Commentaire1;
            if( !msg.IsEmpty() )
                UpperLimit = MAX( UpperLimit, WsItem->m_Posy + SIZETEXT );
            break;

        case WS_COMMENT2:
            msg += screen->m_Commentaire2;
            if( !msg.IsEmpty() )
                UpperLimit = MAX( UpperLimit, WsItem->m_Posy + SIZETEXT );
            break;

        case WS_COMMENT3:
            msg += screen->m_Commentaire3;
            if( !msg.IsEmpty() )
                UpperLimit = MAX( UpperLimit, WsItem->m_Posy + SIZETEXT );
            break;

        case WS_COMMENT4:
            msg += screen->m_Commentaire4;
            if( !msg.IsEmpty() )
                UpperLimit = MAX( UpperLimit, WsItem->m_Posy + SIZETEXT );
            break;

        case WS_UPPER_SEGMENT:
            if( UpperLimit == 0 )
                break;

        case WS_LEFT_SEGMENT:
            WS_MostUpperLine.m_Posy        =
                WS_MostUpperLine.m_Endy    =
                    WS_MostLeftLine.m_Posy = UpperLimit;
            pos.y = (ref.y - WsItem->m_Posy) * conv_unit;

        case WS_SEGMENT:
        {
            wxPoint auxpos;
            auxpos.x = (ref.x - WsItem->m_Endx) * conv_unit;;
            auxpos.y = (ref.y - WsItem->m_Endy) * conv_unit;;
            FctPlume( pos, 'U' );
            FctPlume( auxpos, 'D' );
        }
            break;
        }

        if( !msg.IsEmpty() )
        {
            PlotGraphicText( format_plot, pos, color,
                msg.GetData(), TEXT_ORIENT_HORIZ, text_size,
                GR_TEXT_HJUSTIFY_LEFT, GR_TEXT_VJUSTIFY_CENTER );
        }
    }

    switch( format_plot )
    {
    case PLOT_FORMAT_HPGL:
        Plume_HPGL( 'U' );
        break;

    case PLOT_FORMAT_POST:
        break;
    }
}


/******************************************/
void UserToDeviceCoordinate( wxPoint& pos )
/******************************************/

/* modifie les coord pos.x et pos.y pour le trace selon l'orientation,
  * l'echelle, les offsets de trace */
{
    pos.x = (int) (pos.x * XScale);
    pos.y = (int) (pos.y * YScale);

    switch( PlotOrientOptions ) /* Calcul du cadrage */
    {
    default:
        pos.x -= PlotOffset.x; pos.y = PlotOffset.y - pos.y;
        break;

    case PLOT_MIROIR:
        pos.x -= PlotOffset.x; pos.y = -PlotOffset.y + pos.y;
        break;
    }
}


/************************************/
void UserToDeviceSize( wxSize& size )
/************************************/
/* modifie les dimension size.x et size.y pour le trace selon l'echelle */
{
    size.x = (int) (size.x * XScale);
    size.y = (int) (size.y * YScale);
}
