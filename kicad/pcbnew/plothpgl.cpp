/*******************************/
/**** Routine de trace HPGL ****/
/*******************************/

#include "fctsys.h"
#include "common.h"
#include "plot_common.h"
#include "confirm.h"
#include "pcbnew.h"
#include "pcbplot.h"
#include "trigo.h"

#include "protos.h"


/* Variables locales : */
static int pen_rayon;           /* Rayon de la plume en unites pcb */
static int pen_diam;            /* Diametre de la plume en unites pcb */
static int pen_recouvrement;    /* recouvrement en remplissage en unites pcb */
static int s_Nb_Plot_Errors;    // Error count (errors when a line thichness is less than pen width

/* Routines Locales */

/*****************************************************************************/
void WinEDA_BasePcbFrame::Genere_HPGL( const wxString& FullFileName, int Layer )
/*****************************************************************************/
{
    int     modetrace;
    wxSize  SheetSize;
    wxSize  BoardSize;
    wxPoint BoardCenter;
    double  scale_x, scale_y;
    int     marge  = 0 * U_PCB; // Extra margin (set to 0)
    bool    Center = FALSE;

    modetrace = g_Plot_Mode;

    /* Calcul des echelles de conversion */
    scale_x = Scale_X * SCALE_HPGL;
    scale_y = Scale_Y * SCALE_HPGL;

    // calcul en unites internes des dimensions de la feuille ( connues en 1/1000 pouce )
    SheetSize.x = GetScreen()->m_CurrentSheetDesc->m_Size.x * U_PCB;
    SheetSize.y = GetScreen()->m_CurrentSheetDesc->m_Size.y * U_PCB;

    g_PlotOffset.x = 0;
    g_PlotOffset.y = (int) (SheetSize.y * scale_y);

    // Compute pen_dim (from g_HPGL_Pen_Diam in mils) in pcb units,
    // with plot scale (if Scale is 2, pen diametre is always g_HPGL_Pen_Diam
    // so apparent pen diam is real pen diam / Scale
    pen_diam  = (int) round( (g_HPGL_Pen_Diam * U_PCB) / Scale_X ); // Assume Scale_X # Scale_Y
    pen_rayon = pen_diam / 2;

    s_Nb_Plot_Errors = 0;

    // compute pen_recouvrement (from g_HPGL_Pen_Recouvrement in mils)
    // with plot scale
    if( g_HPGL_Pen_Recouvrement < 0 )
        g_HPGL_Pen_Recouvrement = 0;
    if( g_HPGL_Pen_Recouvrement >= g_HPGL_Pen_Diam )
        g_HPGL_Pen_Recouvrement = g_HPGL_Pen_Diam - 1;
    pen_recouvrement = (int) round( g_HPGL_Pen_Recouvrement * 10.0 / Scale_X );

    dest = wxFopen( FullFileName, wxT( "wt" ) );
    if( dest == NULL )
    {
        wxString msg = _( "Unable to create " ) + FullFileName;
        DisplayError( this, msg );
        return;
    }

    SetLocaleTo_C_standard();

    Affiche_1_Parametre( this, 0, _( "File" ), FullFileName, CYAN );

    PrintHeaderHPGL( dest, g_HPGL_Pen_Speed, g_HPGL_Pen_Num );

    if( g_Plot_Frame_Ref && (g_PlotScaleOpt == 1) )
    {
        int tmp = g_PlotOrient; g_PlotOrient = 0;
        InitPlotParametresHPGL( g_PlotOffset, scale_x, scale_y, g_PlotOrient );
        PlotWorkSheet( PLOT_FORMAT_HPGL, GetScreen() );
        g_PlotOrient = tmp;
    }

    /* calcul des dimensions et centre du PCB */
    m_Pcb->ComputeBoundaryBox();
    BoardSize   = m_Pcb->m_BoundaryBox.GetSize();
    BoardCenter = m_Pcb->m_BoundaryBox.Centre();

    if( g_PlotScaleOpt == 0 )       // Optimum scale
    {
        float Xscale, Yscale;
        Xscale  = (float) ( SheetSize.x - ( 2 * marge) ) / BoardSize.x;
        Yscale  = (float) ( SheetSize.y - ( 2 * marge) ) / BoardSize.y;
        scale_x = scale_y = MIN( Xscale, Yscale ) * SCALE_HPGL;
    }

    BoardCenter.x = (int) (BoardCenter.x * scale_x);
    BoardCenter.y = (int) (BoardCenter.y * scale_y);

    if( g_PlotScaleOpt != 1 )
        Center = TRUE; // Echelle != 1

    /* Calcul du cadrage */
    marge = (int) (marge * SCALE_HPGL);
    if( Center )
        g_PlotOffset.x = (int) (-SheetSize.x / 2 * SCALE_HPGL) +
                         BoardCenter.x + marge;

    switch( g_PlotOrient )
    {
    default:
        if( Center )
        {
            g_PlotOffset.y = (int) (SheetSize.y / 2 * SCALE_HPGL) +
                             BoardCenter.y + marge;
        }
        break;

    case PLOT_MIROIR:
        if( Center )
            g_PlotOffset.y = (int) (-SheetSize.y / 2 * SCALE_HPGL) + BoardCenter.y;
        else
            g_PlotOffset.y = (int) ( ( -SheetSize.y +
                                      m_Pcb->m_BoundaryBox.GetBottom() +
                                      m_Pcb->m_BoundaryBox.GetY() ) * SCALE_HPGL );
        break;
    }

    InitPlotParametresHPGL( g_PlotOffset, scale_x, scale_y, g_PlotOrient );

    // Specify that the contents of the "Edges Pcb" layer are to be plotted
    // in addition to the contents of the currently specified layer.
    int layer_mask = g_TabOneLayerMask[Layer] | EDGE_LAYER;

    switch( Layer )
    {
    case FIRST_COPPER_LAYER:
    case LAYER_N_2:
    case LAYER_N_3:
    case LAYER_N_4:
    case LAYER_N_5:
    case LAYER_N_6:
    case LAYER_N_7:
    case LAYER_N_8:
    case LAYER_N_9:
    case LAYER_N_10:
    case LAYER_N_11:
    case LAYER_N_12:
    case LAYER_N_13:
    case LAYER_N_14:
    case LAYER_N_15:
    case LAST_COPPER_LAYER:
        Plot_Layer_HPGL( dest, layer_mask, 0, 1, modetrace );
        break;

    case SILKSCREEN_N_CU:
    case SILKSCREEN_N_CMP:
        Plot_Serigraphie( PLOT_FORMAT_HPGL, dest, layer_mask );
        break;

    case SOLDERMASK_N_CU:
    case SOLDERMASK_N_CMP:       /* Trace du vernis epargne */
    {
        int tracevia;
        if( g_DrawViaOnMaskLayer )
            tracevia = 1;
        else
            tracevia = 0;
        Plot_Layer_HPGL( dest, layer_mask,
                         g_DesignSettings.m_MaskMargin, tracevia, modetrace );
    }
        break;

    case SOLDERPASTE_N_CU:
    case SOLDERPASTE_N_CMP:       /* Trace du masque de pate de soudure */
        Plot_Layer_HPGL( dest, layer_mask, 0, 0, modetrace );
        break;

    default:       /* Trace des autres couches (dessin, adhesives,eco,comment) */
        Plot_Serigraphie( PLOT_FORMAT_HPGL, dest, layer_mask );
        break;
    }

    /* fin */
    CloseFileHPGL( dest );
    SetLocaleTo_Default();
}


/*********************************************************************/
void WinEDA_BasePcbFrame::Plot_Layer_HPGL( FILE* File, int masque_layer,
                                           int garde, int tracevia, int modetrace )
/*********************************************************************/

/* Trace en format HPGL. d'une couche cuivre ou masque
 *  1 unite HPGL = 0.98 mils ( 1 mil = 1.02041 unite HPGL ) .
 */
{
    wxSize      size;
    wxPoint     start, end;
    MODULE*     Module;
    D_PAD*      PtPad;
    TRACK*      pts;
    BOARD_ITEM* PtStruct;
    wxString    msg;

    /* trace des elements type Drawings Pcb : */
    PtStruct = m_Pcb->m_Drawings;
    for( ; PtStruct != NULL; PtStruct = PtStruct->Next() )
    {
        switch( PtStruct->Type() )
        {
        case TYPE_DRAWSEGMENT:
            PlotDrawSegment( (DRAWSEGMENT*) PtStruct, PLOT_FORMAT_HPGL,
                            masque_layer );
            break;

        case TYPE_TEXTE:
            PlotTextePcb( (TEXTE_PCB*) PtStruct, PLOT_FORMAT_HPGL,
                         masque_layer );
            break;

        case TYPE_COTATION:
            PlotCotation( (COTATION*) PtStruct, PLOT_FORMAT_HPGL,
                         masque_layer );
            break;

        case TYPE_MIRE:
            PlotMirePcb( (MIREPCB*) PtStruct, PLOT_FORMAT_HPGL,
                        masque_layer );
            break;

        case TYPE_MARKER:
            break;

        default:
            DisplayError( this, wxT( "Type Draw non gere" ) );
            break;
        }
    }

    /* Trace des Elements des modules autres que pads */
    Module = m_Pcb->m_Modules;
    for( ; Module != NULL; Module = Module->Next() )
    {
        PtStruct = Module->m_Drawings;
        for( ; PtStruct != NULL; PtStruct = PtStruct->Next() )
        {
            switch( PtStruct->Type() )
            {
            case TYPE_EDGE_MODULE:
                if( masque_layer &
                    g_TabOneLayerMask[ PtStruct->GetLayer() ] )
                    Plot_1_EdgeModule( PLOT_FORMAT_HPGL, (EDGE_MODULE*) PtStruct );
                break;

            default:
                break;
            }
        }
    }

    /* Trace des Elements des modules : Pastilles */
    Module = m_Pcb->m_Modules;
    for( ; Module != NULL; Module = Module->Next() )
    {
        PtPad = (D_PAD*) Module->m_Pads;
        for( ; PtPad != NULL; PtPad = (D_PAD*) PtPad->Next() )
        {
            wxPoint shape_pos;
            if( (PtPad->m_Masque_Layer & masque_layer) == 0 )
                continue;

            shape_pos = PtPad->ReturnShapePos();
            start   = shape_pos;
            size    = PtPad->m_Size;
            size.x += garde * 2; size.y += garde * 2;

            switch( PtPad->m_PadShape & 0x7F )
            {
            case PAD_CIRCLE:
                trace_1_pastille_RONDE_HPGL( start, size.x, modetrace );
                break;

            case PAD_OVAL:
            {
                trace_1_pastille_OVALE_HPGL( start, size, PtPad->m_Orient, modetrace );
                break;
            }

            case PAD_TRAPEZOID:
            {
                wxSize delta;
                delta = PtPad->m_DeltaSize;
                trace_1_pad_TRAPEZE_HPGL( start, size, delta,
                                          PtPad->m_Orient, modetrace );
                break;
            }

            case PAD_RECT:
            default:
                PlotRectangularPad_HPGL( start, size,
                                         PtPad->m_Orient, modetrace );
                break;
            }
        }
    }

    /* trace des VIAS : */
    if( tracevia )
    {
        TRACK* pts;
        for( pts = m_Pcb->m_Track; pts != NULL; pts = pts->Next() )
        {
            if( pts->Type() != TYPE_VIA )
                continue;
            SEGVIA* Via = (SEGVIA*) pts;

            /* vias not plotted if not on selected layer, but if layer
             *  == SOLDERMASK_LAYER_CU or SOLDERMASK_LAYER_CMP, vias are drawn ,
             *  if they are on a external copper layer
             */
            int     via_mask_layer = Via->ReturnMaskLayer();
            if( (via_mask_layer & CUIVRE_LAYER ) )
                via_mask_layer |= SOLDERMASK_LAYER_CU;
            if( (via_mask_layer & CMP_LAYER ) )
                via_mask_layer |= SOLDERMASK_LAYER_CMP;
            if( (via_mask_layer & masque_layer) == 0 )
                continue;

            start  = Via->m_Start;
            size.x = Via->m_Width + (garde * 2);

            trace_1_pastille_RONDE_HPGL( start, size.x, modetrace );
        }

        fputs( "PU;\n", dest );
    }

    /* trace des segments pistes */
    for( pts = m_Pcb->m_Track; pts != NULL; pts = pts->Next() )
    {
        if( pts->Type() == TYPE_VIA )
            continue;

        if( (g_TabOneLayerMask[pts->GetLayer()] & masque_layer) )
            Plot_Filled_Segment_HPGL( pts->m_Start, pts->m_End, pts->m_Width, (GRFillMode)g_Plot_Mode );
    }

    /* trace des segments pistes et zones */
    for( pts = m_Pcb->m_Zone; pts != NULL; pts = pts->Next() )
    {
        if( (g_TabOneLayerMask[pts->GetLayer()] & masque_layer) )
            Plot_Filled_Segment_HPGL( pts->m_Start, pts->m_End, pts->m_Width, (GRFillMode)g_Plot_Mode );
    }

    /* Plot filled ares */
    for( int ii = 0; ii < m_Pcb->GetAreaCount(); ii++ )
    {
        ZONE_CONTAINER* zone = m_Pcb->GetArea( ii );
        if( ( ( 1 << zone->GetLayer() ) & masque_layer ) == 0 )
            continue;
        PlotFilledAreas( zone, PLOT_FORMAT_HPGL );
    }
}


/*********************************************************************************************/
bool Plot_Filled_Segment_HPGL( wxPoint aStart, wxPoint aEnd, int aWidth, GRFillMode aPlotMode )
/*********************************************************************************************/

/** Function Plot a filled segment (track)
 * @param aStart = starting point
 * @param aEnd = ending point
 * @param aWidth = segment width (thickness)
 * @param aPlotMode = FILLED, SKETCH ..
 * @return true if Ok, false if aWidth > pen size (the segment is always plotted)
 */
{
    wxPoint center;
    wxSize  size;
    int     orient;

    if( (pen_diam >= aWidth) || (g_Plot_Mode == FILAIRE) )  /* just a line is Ok */
    {
        Move_Plume_HPGL( aStart, 'U' );
        Move_Plume_HPGL( aEnd, 'D' );
        Plume_HPGL( 'U' );
        return pen_diam <= aWidth;;
    }

    // A segment is like an oval pal, so use trace_1_pastille_OVALE_HPGL to do the work.
    center.x = (aStart.x + aEnd.x) / 2;
    center.y = (aStart.y + aEnd.y) / 2;

    size.x = aEnd.x - aStart.x;
    size.y = aEnd.y - aStart.y;
    if ( size.y == 0 )
        orient = 0;
    else if ( size.x == 0 )
        orient = 900;
    else orient = - (int) (atan2( (double)size.y, (double)size.x ) * 1800.0 / M_PI);
    size.x = (int) sqrt( ((double)size.x * size.x) + ((double)size.y * size.y) ) + aWidth;   // module.
    size.y = aWidth;

    trace_1_pastille_OVALE_HPGL( center, size, orient, aPlotMode );

    return pen_diam <= aWidth;
}


/************************************************************************************/
void trace_1_pastille_OVALE_HPGL( wxPoint pos, wxSize size, int aOrient, int modetrace )
/************************************************************************************/
/* Trace 1 pastille PAD_OVAL en position pos_X,Y , de dim size.x, size.y */
{
    int rayon, deltaxy, cx, cy;

    /* la pastille est ramenee a une pastille ovale avec size.y > size.x
     *  ( ovale vertical en orientation 0 ) */
    if( size.x > size.y )
    {
        EXCHG( size.x, size.y ); aOrient += 900;
        if( aOrient >= 3600 )
            aOrient -= 3600;
    }
    deltaxy = size.y - size.x; /* = distance entre centres de l'ovale */
    rayon   = size.x / 2;

    if( modetrace == FILLED )
    {
        PlotRectangularPad_HPGL( pos, wxSize( size.x, deltaxy+pen_diam ),
                                 aOrient, modetrace );
        cx = 0; cy = deltaxy / 2;
        RotatePoint( &cx, &cy, aOrient );
        trace_1_pastille_RONDE_HPGL( wxPoint( cx + pos.x, cy + pos.y ), size.x, modetrace );
        Plume_HPGL( 'U' );
        cx = 0; cy = -deltaxy / 2;
        RotatePoint( &cx, &cy, aOrient );
        trace_1_pastille_RONDE_HPGL( wxPoint( cx + pos.x, cy + pos.y ), size.x, modetrace );
    }
    else    /* Trace en mode SKETCH */
    {
        cx = -rayon; cy = -deltaxy / 2;
        RotatePoint( &cx, &cy, aOrient );
        Move_Plume_HPGL( wxPoint( cx + pos.x, cy + pos.y ), 'U' );
        cx = -rayon; cy = deltaxy / 2;
        RotatePoint( &cx, &cy, aOrient );
        Move_Plume_HPGL( wxPoint( cx + pos.x, cy + pos.y ), 'D' );

        cx = rayon; cy = -deltaxy / 2;
        RotatePoint( &cx, &cy, aOrient );
        Move_Plume_HPGL( wxPoint( cx + pos.x, cy + pos.y ), 'U' );
        cx = rayon; cy = deltaxy / 2;
        RotatePoint( &cx, &cy, aOrient );
        Move_Plume_HPGL( wxPoint( cx + pos.x, cy + pos.y ), 'D' );
        Plume_HPGL( 'U' );

        cx = 0; cy = deltaxy / 2;
        RotatePoint( &cx, &cy, aOrient );
        PlotArc( PLOT_FORMAT_HPGL, wxPoint( cx + pos.x, cy + pos.y ),
                 -aOrient, -aOrient - 1800,
                 size.x / 2, pen_diam );
        cx = 0; cy = -deltaxy / 2;
        RotatePoint( &cx, &cy, aOrient );
        PlotArc( PLOT_FORMAT_HPGL, wxPoint( cx + pos.x, cy + pos.y ),
                 -aOrient - 1800, -aOrient,
                 size.x / 2, pen_diam );
    }

    Plume_HPGL( 'U' );
}


/**************************************************************************/
void trace_1_pastille_RONDE_HPGL( wxPoint pos, int diametre, int modetrace )
/**************************************************************************/
/* Trace 1 pastille RONDE (via,pad rond) en position pos */
{
    char cbuf[1024];
    int  rayon, delta;

    UserToDeviceCoordinate( pos );

    delta = pen_diam - pen_recouvrement;
    rayon = diametre / 2;
    if( modetrace != FILAIRE )
    {
        rayon = (diametre - pen_diam ) / 2;
    }

    if( rayon < 0 )
    {
        rayon = 0; s_Nb_Plot_Errors++;
    }
    wxSize rsize( rayon, rayon );

    UserToDeviceSize( rsize );

    Plume_HPGL( 'U' );
    sprintf( cbuf, "PA %d,%d;CI %d;\n", pos.x, pos.y, rsize.x );
    fputs( cbuf, dest );
    if( modetrace == FILLED ) /* Trace en mode Remplissage */
    {
        if( delta > 0 )
        {
            while( (rayon -= delta ) >= 0 )
            {
                rsize.x = rsize.y = rayon;
                UserToDeviceSize( rsize );
                sprintf( cbuf, "PA %d,%d; CI %d;\n", pos.x, pos.y, rsize.x );
                fputs( cbuf, dest );
            }
        }
    }
    Plume_HPGL( 'U' ); return;
}


/***************************************************************/
void PlotRectangularPad_HPGL( wxPoint padpos, wxSize padsize,
                              int orient, int modetrace )
/****************************************************************/

/*
 *  Trace 1 pad rectangulaire vertical ou horizontal ( Pad rectangulaire )
 *  donne par son centre et ses dimensions X et Y
 *  Units are user units
 */
{
    wxSize size;
    int    delta;
    int    ox, oy, fx, fy;

    size.x = padsize.x / 2;  size.y = padsize.y / 2;
    if( modetrace != FILAIRE )
    {
        size.x = (padsize.x - (int) pen_diam) / 2;
        size.y = (padsize.y - (int) pen_diam) / 2;
    }

    if( (size.x < 0 ) || (size.y < 0) )
    {
        s_Nb_Plot_Errors++;
    }
    if( size.x < 0 )
        size.x = 0;if( size.y < 0 )
        size.y = 0;

    /* Si une des dimensions est nulle, le trace se reduit a 1 trait */
    if( size.x == 0 )
    {
        ox = padpos.x; oy = padpos.y - size.y;
        RotatePoint( &ox, &oy, padpos.x, padpos.y, orient );
        fx = padpos.x; fy = padpos.y + size.y;
        RotatePoint( &fx, &fy, padpos.x, padpos.y, orient );
        Move_Plume_HPGL( wxPoint( ox, oy ), 'U' );
        Move_Plume_HPGL( wxPoint( fx, fy ), 'D' );
        Plume_HPGL( 'U' ); return;
    }
    if( size.y == 0 )
    {
        ox = padpos.x - size.x; oy = padpos.y;
        RotatePoint( &ox, &oy, padpos.x, padpos.y, orient );
        fx = padpos.x + size.x; fy = padpos.y;
        RotatePoint( &fx, &fy, padpos.x, padpos.y, orient );
        Move_Plume_HPGL( wxPoint( ox, oy ), 'U' );
        Move_Plume_HPGL( wxPoint( fx, fy ), 'D' );
        Plume_HPGL( 'U' ); return;
    }

    ox = padpos.x - size.x; oy = padpos.y - size.y;
    RotatePoint( &ox, &oy, padpos.x, padpos.y, orient );
    Move_Plume_HPGL( wxPoint( ox, oy ), 'U' );

    fx = padpos.x - size.x; fy = padpos.y + size.y;
    RotatePoint( &fx, &fy, padpos.x, padpos.y, orient );
    Move_Plume_HPGL( wxPoint( fx, fy ), 'D' );

    fx = padpos.x + size.x; fy = padpos.y + size.y;
    RotatePoint( &fx, &fy, padpos.x, padpos.y, orient );
    Move_Plume_HPGL( wxPoint( fx, fy ), 'D' );

    fx = padpos.x + size.x; fy = padpos.y - size.y;
    RotatePoint( &fx, &fy, padpos.x, padpos.y, orient );
    Move_Plume_HPGL( wxPoint( fx, fy ), 'D' );

    Move_Plume_HPGL( wxPoint( ox, oy ), 'D' );

    if( modetrace != FILLED )
    {
        Plume_HPGL( 'U' ); return;
    }

    /* Trace en mode Remplissage */
    delta = (int) (pen_diam - pen_recouvrement);
    if( delta > 0 )
        while( (size.x > 0) && (size.y > 0) )
        {
            size.x -= delta; size.y -= delta;
            if( size.x < 0 )
                size.x = 0;if( size.y < 0 )
                size.y = 0;

            ox = padpos.x - size.x; oy = padpos.y - size.y;
            RotatePoint( &ox, &oy, padpos.x, padpos.y, orient );
            Move_Plume_HPGL( wxPoint( ox, oy ), 'D' );

            fx = padpos.x - size.x; fy = padpos.y + size.y;
            RotatePoint( &fx, &fy, padpos.x, padpos.y, orient );
            Move_Plume_HPGL( wxPoint( fx, fy ), 'D' );

            fx = padpos.x + size.x; fy = padpos.y + size.y;
            RotatePoint( &fx, &fy, padpos.x, padpos.y, orient );
            Move_Plume_HPGL( wxPoint( fx, fy ), 'D' );

            fx = padpos.x + size.x; fy = padpos.y - size.y;
            RotatePoint( &fx, &fy, padpos.x, padpos.y, orient );
            Move_Plume_HPGL( wxPoint( fx, fy ), 'D' );

            Move_Plume_HPGL( wxPoint( ox, oy ), 'D' );
        }

    Plume_HPGL( 'U' );
}


/********************************************************************/
void trace_1_pad_TRAPEZE_HPGL( wxPoint padpos, wxSize size, wxSize delta,
                               int orient, int modetrace )
/********************************************************************/

/*
 *  Trace 1 pad trapezoidal donne par :
 *  son centre padpos.x,padpos.y
 *  ses dimensions dimX et dimY
 *  les variations deltaX et deltaY
 *  son orientation orient et 0.1 degres
 *  le mode de trace (FILLED, SKETCH, FILAIRE)
 *  Le trace n'est fait que pour un trapeze, c.a.d que deltaX ou deltaY
 *  = 0.
 *
 *  les notation des sommets sont ( vis a vis de la table tracante )
 *      0 ------------- 3
 *        .			   .
 *          .		  .
 *           .		 .
 *            1 --- 2
 */
{
    int     ii, jj;
    wxPoint polygone[4];    /* coord des sommets / centre du pad */
    wxPoint coord[4];       /* coord reelles des sommets du trapeze a tracer */
    float   fangle;         /* angle d'inclinaison des cotes du trapeze */
    int     rayon;          /* rayon de la plume */
    int     moveX, moveY; /* variation de position plume selon axe X et Y , lors
                          *  du remplissage du trapeze */

    rayon = (int) pen_rayon; if( modetrace == FILAIRE )
        rayon = 0;
    moveX = moveY = rayon;

    size.x  /= 2;  size.y /= 2;
    delta.x /= 2; delta.y /= 2;

    polygone[0].x = -size.x - delta.y; polygone[0].y = +size.y + delta.x;
    polygone[1].x = -size.x + delta.y; polygone[1].y = -size.y - delta.x;
    polygone[2].x = +size.x - delta.y; polygone[2].y = -size.y + delta.x;
    polygone[3].x = +size.x + delta.y; polygone[3].y = +size.y - delta.x;

    /* Calcul du demi angle d'inclinaison des cotes du trapeze */
    if( delta.y ) /* Trapeze horizontal */
    {
        fangle = atan2( (double) (polygone[1].y - polygone[0].y),
                       (double) (polygone[1].x - polygone[0].x) ) / 2;
    }
    else
    {
        fangle = atan2( (double) (polygone[3].y - polygone[0].y),
                       (double) (polygone[3].x - polygone[0].x) ) / 2;
    }

    /* Trace du contour */
    polygone[0].x += moveX; polygone[0].y -= moveY;
    polygone[1].x += moveX; polygone[1].y += moveY;
    polygone[2].x -= moveX; polygone[2].y += moveY;
    polygone[3].x -= moveX; polygone[3].y -= moveY;

    for( ii = 0; ii < 4; ii++ )
    {
        coord[ii].x = polygone[ii].x + padpos.x;
        coord[ii].y = polygone[ii].y + padpos.y;
        RotatePoint( &coord[ii], padpos, orient );
    }

    // Plot edge:
    Move_Plume_HPGL( coord[0], 'U' );
    Move_Plume_HPGL( coord[1], 'D' );
    Move_Plume_HPGL( coord[2], 'D' );
    Move_Plume_HPGL( coord[3], 'D' );
    Move_Plume_HPGL( coord[0], 'D' );

    if( modetrace != FILLED )
    {
        Plume_HPGL( 'U' ); return;
    }

    /* Fill the shape */

    moveX = moveY = pen_diam - pen_recouvrement;
    /* calcul de jj = hauteur du remplissage */
    if( delta.y ) /* Trapeze horizontal */
    {
        jj = size.y - (int) ( pen_diam + (2 * pen_recouvrement) );
    }
    else
    {
        jj = size.x - (int) ( pen_diam + (2 * pen_recouvrement) );
    }

    /* Calcul de jj = nombre de segments a tracer pour le remplissage */
    jj = jj / (int) (pen_diam - pen_recouvrement);


    /* Trace du contour */
    for( ; jj > 0; jj-- )
    {
        polygone[0].x += moveX; polygone[0].y -= moveY;
        polygone[1].x += moveX; polygone[1].y += moveY;
        polygone[2].x -= moveX; polygone[2].y += moveY;
        polygone[3].x -= moveX; polygone[3].y -= moveY;

        /* Test de limitation de variation des dimensions :
         *  si les sommets se "croisent", il ne faut plus modifier les
         *  coordonnees correspondantes */
        if( polygone[0].x > polygone[3].x )
        {  /* croisement sur axe X des 2 sommets 0 et 3 */
            polygone[0].x = polygone[3].x = 0;
        }
        if( polygone[1].x > polygone[2].x )
        {  /* croisement sur axe X des 2 sommets 1 et 2 */
            polygone[1].x = polygone[2].x = 0;
        }
        if( polygone[1].y > polygone[0].y )
        {  /* croisement sur axe Y des 2 sommets 0 et 1 */
            polygone[0].y = polygone[1].y = 0;
        }
        if( polygone[2].y > polygone[3].y )
        {  /* croisement sur axe Y des 2 sommets 2 et 3 */
            polygone[2].y = polygone[3].y = 0;
        }

        for( ii = 0; ii < 4; ii++ )
        {
            coord[ii].x = polygone[ii].x + padpos.x;
            coord[ii].y = polygone[ii].y + padpos.y;
            RotatePoint( &coord[ii], padpos, orient );
        }

        Move_Plume_HPGL( coord[0], 'U' );
        Move_Plume_HPGL( coord[1], 'D' );
        Move_Plume_HPGL( coord[2], 'D' );
        Move_Plume_HPGL( coord[3], 'D' );
        Move_Plume_HPGL( coord[0], 'D' );
    }

    Plume_HPGL( 'U' );
}
