/************************************/
/* Kicad: Common plot HPGL Routines */
/************************************/

#include "fctsys.h"
#include "gr_basic.h"
#include "trigo.h"
#include "wxstruct.h"
#include "base_struct.h"
#include "plot_common.h"
#include "macros.h"
#include "kicad_string.h"

/* HPGL scale factor. */
const double SCALE_HPGL = 0.102041;


/* Set the plot offset for the current plotting
 */
void HPGL_PLOTTER::set_viewport( wxPoint offset, double aScale, int orient )
{
    wxASSERT( !output_file );
    plot_offset  = offset;
    plot_scale   = aScale;
    device_scale = SCALE_HPGL;
    set_default_line_width( 100 ); /* default line width in 1 / 1000 inch */
    plot_orient_options = orient;
}


void HPGL_PLOTTER::start_plot( FILE* fout )
{
    wxASSERT( !output_file );
    output_file = fout;
    fprintf( output_file, "IN;VS%d;PU;PA;SP%d;\n", pen_speed, pen_number );
}


void HPGL_PLOTTER::end_plot()
{
    wxASSERT( output_file );
    fputs( "PU;PA;SP0;\n", output_file );
    fclose( output_file );
    output_file = 0;
}


void HPGL_PLOTTER::rect( wxPoint p1, wxPoint p2, FILL_T fill, int width )
{
    wxASSERT( output_file );
    user_to_device_coordinates( p2 );
    move_to( p1 );
    fprintf( output_file, "EA %d,%d;\n", p2.x, p2.y );
    pen_finish();
}


void HPGL_PLOTTER::circle( wxPoint centre,
                           int     diameter,
                           FILL_T  fill,
                           int     width )
{
    wxASSERT( output_file );
    double rayon = user_to_device_size( diameter / 2 );

    if( rayon > 0 )
    {
        move_to( centre );
        fprintf( output_file, "CI %g;\n", rayon );
        pen_finish();
    }
}


/* Plot a polygon (closed if completed) in HPGL
 * Coord = coord table tops
 * Nb = number of coord (coord 1 = 2 elements: X and Y table)
 * Fill: if! = 0 filled polygon
 */
void HPGL_PLOTTER::poly( int nb, int* coord, FILL_T fill, int width )
{
    wxASSERT( output_file );
    if( nb <= 1 )
        return;

    move_to( wxPoint( coord[0], coord[1] ) );
    for( int ii = 1; ii < nb; ii++ )
        line_to( wxPoint( coord[ii * 2], coord[(ii * 2) + 1] ) );

    /* Close polygon if filled. */
    if( fill )
    {
        int ii = (nb - 1) * 2;
        if( (coord[ii] != coord[0] ) || (coord[ii + 1] != coord[1]) )
            line_to( wxPoint( coord[0], coord[1] ) );
    }
    pen_finish();
}


/* Set pen up ('U') or down ('D').
 */
void HPGL_PLOTTER::pen_control( int plume )
{
    wxASSERT( output_file );
    switch( plume )
    {
    case 'U':
        if( pen_state != 'U' )
        {
            fputs( "PU;", output_file );
            pen_state = 'U';
        }
        break;

    case 'D':
        if( pen_state != 'D' )
        {
            fputs( "PD;", output_file );
            pen_state = 'D';
        }
        break;

    case 'Z':
        fputs( "PU;", output_file );
        pen_state     = 'U';
        pen_lastpos.x = -1;
        pen_lastpos.y = -1;
        break;
    }
}


/*
 * Move the pen to position with pen up or down.
 * At position x, y
 * Unit to unit DRAWING
 * If pen = 'Z' without changing pen during move.
 */
void HPGL_PLOTTER::pen_to( wxPoint pos, char plume )
{
    wxASSERT( output_file );
    if( plume == 'Z' )
    {
        pen_control( 'Z' );
        return;
    }
    pen_control( plume );
    user_to_device_coordinates( pos );

    if( pen_lastpos != pos )
        fprintf( output_file, "PA %d,%d;\n", pos.x, pos.y );
    pen_lastpos = pos;
}


void HPGL_PLOTTER::set_dash( bool dashed )
{
    wxASSERT( output_file );
    if( dashed )
        fputs( "LI 2;\n", stderr );
    else
        fputs( "LI;\n", stderr );
}


/** Function Plot a filled segment (track)
 * @param start = starting point
 * @param end = ending point
 * @param aWidth = segment width (thickness)
 * @param aPlotMode = FILLED, SKETCH ..
 */
void HPGL_PLOTTER::thick_segment( wxPoint start, wxPoint end, int width,
                                  GRTraceMode tracemode )
{
    wxASSERT( output_file );
    wxPoint center;
    wxSize  size;

    if( (pen_diameter >= width) || (tracemode == FILAIRE) )  /* just a line is
                                                              * Ok */
    {
        move_to( start );
        finish_to( end );
    }
    else
        segment_as_oval( start, end, width, tracemode );
}


/* Plot an arc:
 * Center = center coord
 * Stangl, endAngle = angle of beginning and end
 * Radius = radius of the arc
 * Command
 * PU PY x, y; PD start_arc_X AA, start_arc_Y, angle, NbSegm; PU;
 * Or PU PY x, y; PD start_arc_X AA, start_arc_Y, angle, PU;
 */
void HPGL_PLOTTER::arc( wxPoint centre, int StAngle, int EndAngle, int rayon,
                        FILL_T fill, int width )
{
    wxASSERT( output_file );
    wxPoint cmap;
    wxPoint cpos;
    float   angle;

    if( rayon <= 0 )
        return;

    cpos = centre;
    user_to_device_coordinates( cpos );

    if( plot_orient_options == PLOT_MIROIR )
        angle = (StAngle - EndAngle) / 10.0;
    else
        angle = (EndAngle - StAngle) / 10.0;
    /* Calculate start point, */
    cmap.x = (int) ( centre.x + ( rayon * cos( StAngle * M_PI / 1800 ) ) );
    cmap.y = (int) ( centre.y - ( rayon * sin( StAngle * M_PI / 1800 ) ) );
    user_to_device_coordinates( cmap );

    fprintf( output_file,
             "PU;PA %d,%d;PD;AA %d,%d, ",
             cmap.x,
             cmap.y,
             cpos.x,
             cpos.y );
    fprintf( output_file, "%f", angle );
    fprintf( output_file, ";PU;\n" );
    pen_finish();
}


/* Plot oval pad.
 */
void HPGL_PLOTTER::flash_pad_oval( wxPoint pos, wxSize size, int orient,
                                   GRTraceMode trace_mode )
{
    wxASSERT( output_file );
    int rayon, deltaxy, cx, cy;

    /* The pad is reduced to an oval with size.y > size.x
     * (Oval vertical orientation 0)
     */
    if( size.x > size.y )
    {
        EXCHG( size.x, size.y ); orient += 900;
        if( orient >= 3600 )
            orient -= 3600;
    }
    deltaxy = size.y - size.x;     /* distance between centers of the oval */
    rayon   = size.x / 2;

    if( trace_mode == FILLED )
    {
        flash_pad_rect( pos, wxSize( size.x, deltaxy + wxRound( pen_diameter ) ),
                        orient, trace_mode );
        cx = 0; cy = deltaxy / 2;
        RotatePoint( &cx, &cy, orient );
        flash_pad_circle( wxPoint( cx + pos.x,
                                   cy + pos.y ), size.x, trace_mode );
        cx = 0; cy = -deltaxy / 2;
        RotatePoint( &cx, &cy, orient );
        flash_pad_circle( wxPoint( cx + pos.x,
                                   cy + pos.y ), size.x, trace_mode );
    }
    else    /* Plot in SKETCH mode. */
    {
        sketch_oval( pos, size, orient, wxRound( pen_diameter ) );
    }
}


/* Plot round pad or via.
 */
void HPGL_PLOTTER::flash_pad_circle( wxPoint pos, int diametre,
                                     GRTraceMode trace_mode )
{
    wxASSERT( output_file );
    int rayon, delta;

    user_to_device_coordinates( pos );

    delta = wxRound( pen_diameter - pen_overlap );
    rayon = diametre / 2;
    if( trace_mode != FILAIRE )
    {
        rayon = ( diametre - wxRound( pen_diameter ) ) / 2;
    }

    if( rayon < 0 )
    {
        rayon = 0;
    }
    wxSize rsize( rayon, rayon );

    user_to_device_size( rsize );

    fprintf( output_file, "PA %d,%d;CI %d;\n", pos.x, pos.y, rsize.x );

    if( trace_mode == FILLED )        /* Plot in filled mode. */
    {
        if( delta > 0 )
        {
            while( (rayon -= delta ) >= 0 )
            {
                rsize.x = rsize.y = rayon;
                user_to_device_size( rsize );
                fprintf( output_file,
                         "PA %d,%d; CI %d;\n",
                         pos.x,
                         pos.y,
                         rsize.x );
            }
        }
    }
    pen_finish();
    return;
}


/*
 * Plot rectangular pad vertical or horizontal.
 * Gives its center and its dimensions X and Y
 * Units are user units
 */
void HPGL_PLOTTER::flash_pad_rect( wxPoint pos, wxSize padsize,
                                   int orient, GRTraceMode trace_mode )
{
    wxASSERT( output_file );
    wxSize size;
    int    delta;
    int    ox, oy, fx, fy;

    size.x = padsize.x / 2;
    size.y = padsize.y / 2;

    if( trace_mode != FILAIRE )
    {
        size.x = (padsize.x - (int) pen_diameter) / 2;
        size.y = (padsize.y - (int) pen_diameter) / 2;
    }

    if( size.x < 0 )
        size.x = 0;
    if( size.y < 0 )
        size.y = 0;

    /* If a dimension is zero, the trace is reduced to 1 line. */
    if( size.x == 0 )
    {
        ox = pos.x;
        oy = pos.y - size.y;
        RotatePoint( &ox, &oy, pos.x, pos.y, orient );
        fx = pos.x;
        fy = pos.y + size.y;
        RotatePoint( &fx, &fy, pos.x, pos.y, orient );
        move_to( wxPoint( ox, oy ) );
        finish_to( wxPoint( fx, fy ) );
        return;
    }
    if( size.y == 0 )
    {
        ox = pos.x - size.x;
        oy = pos.y;
        RotatePoint( &ox, &oy, pos.x, pos.y, orient );
        fx = pos.x + size.x;
        fy = pos.y;
        RotatePoint( &fx, &fy, pos.x, pos.y, orient );
        move_to( wxPoint( ox, oy ) );
        finish_to( wxPoint( fx, fy ) );
        return;
    }

    ox = pos.x - size.x;
    oy = pos.y - size.y;
    RotatePoint( &ox, &oy, pos.x, pos.y, orient );
    move_to( wxPoint( ox, oy ) );

    fx = pos.x - size.x;
    fy = pos.y + size.y;
    RotatePoint( &fx, &fy, pos.x, pos.y, orient );
    line_to( wxPoint( fx, fy ) );

    fx = pos.x + size.x;
    fy = pos.y + size.y;
    RotatePoint( &fx, &fy, pos.x, pos.y, orient );
    line_to( wxPoint( fx, fy ) );

    fx = pos.x + size.x;
    fy = pos.y - size.y;
    RotatePoint( &fx, &fy, pos.x, pos.y, orient );
    line_to( wxPoint( fx, fy ) );

    finish_to( wxPoint( ox, oy ) );

    if( trace_mode == FILLED )
    {
        /* Plot in filled mode. */
        delta = (int) (pen_diameter - pen_overlap);

        if( delta > 0 )
            while( (size.x > 0) && (size.y > 0) )
            {
                size.x -= delta;
                size.y -= delta;

                if( size.x < 0 )
                    size.x = 0;
                if( size.y < 0 )
                    size.y = 0;

                ox = pos.x - size.x;
                oy = pos.y - size.y;
                RotatePoint( &ox, &oy, pos.x, pos.y, orient );
                move_to( wxPoint( ox, oy ) );

                fx = pos.x - size.x;
                fy = pos.y + size.y;
                RotatePoint( &fx, &fy, pos.x, pos.y, orient );
                line_to( wxPoint( fx, fy ) );

                fx = pos.x + size.x;
                fy = pos.y + size.y;
                RotatePoint( &fx, &fy, pos.x, pos.y, orient );
                line_to( wxPoint( fx, fy ) );

                fx = pos.x + size.x;
                fy = pos.y - size.y;
                RotatePoint( &fx, &fy, pos.x, pos.y, orient );
                line_to( wxPoint( fx, fy ) );

                finish_to( wxPoint( ox, oy ) );
            }

    }
}


/* Plot trapezoidal pad.
 * Pos is pad center
 * Dimensions size.x and size.y
 * Changes delta.x and delta.y (1 of at least two must be zero)
 * Orientation east to 0.1 degrees
 * Plot mode (FILLED, SKETCH, WIRED)
 *
 * The evidence is that a trapezoid, ie that delta.x or delta.y = 0.
 *
 * The rating of the vertexes are (vis a vis the plotter)
 *
 * "       0 ------------- 3   "
 * "        .             .    "
 * "         .     O     .     "
 * "          .         .      "
 * "           1 ---- 2        "
 *
 *
 *   Example delta.y > 0, delta.x = 0
 * "           1 ---- 2        "
 * "          .         .      "
 * "         .     O     .     "
 * "        .             .    "
 * "       0 ------------- 3   "
 *
 *
 *   Example delta.y = 0, delta.x > 0
 * "       0                  "
 * "       . .                "
 * "       .     .            "
 * "       .           3      "
 * "       .           .      "
 * "       .     O     .      "
 * "       .           .      "
 * "       .           2      "
 * "       .     .            "
 * "       . .                "
 * "       1                  "
 */
void HPGL_PLOTTER::flash_pad_trapez( wxPoint pos, wxSize size, wxSize delta,
                                     int orient, GRTraceMode trace_mode )
{
    wxASSERT( output_file );
    wxPoint polygone[4];
    wxPoint coord[4];
    int     moveX, moveY;

    moveX = moveY = wxRound( pen_diameter );

    size.x  /= 2;
    size.y  /= 2;
    delta.x /= 2;
    delta.y /= 2;

    polygone[0].x = -size.x - delta.y;
    polygone[0].y = +size.y + delta.x;
    polygone[1].x = -size.x + delta.y;
    polygone[1].y = -size.y - delta.x;
    polygone[2].x = +size.x - delta.y;
    polygone[2].y = -size.y + delta.x;
    polygone[3].x = +size.x + delta.y;
    polygone[3].y = +size.y - delta.x;

    /* Trace the outline. */
    polygone[0].x += moveX;
    polygone[0].y -= moveY;
    polygone[1].x += moveX;
    polygone[1].y += moveY;
    polygone[2].x -= moveX;
    polygone[2].y += moveY;
    polygone[3].x -= moveX;
    polygone[3].y -= moveY;

    for( int ii = 0; ii < 4; ii++ )
    {
        coord[ii].x = polygone[ii].x + pos.x;
        coord[ii].y = polygone[ii].y + pos.y;
        RotatePoint( &coord[ii], pos, orient );
    }

    // Plot edge:
    move_to( coord[0] );
    line_to( coord[1] );
    line_to( coord[2] );
    line_to( coord[3] );
    finish_to( coord[0] );

    if( trace_mode == FILLED )
    {
        int jj;
        /* Fill the shape */
        moveX = moveY = wxRound( pen_diameter - pen_overlap );
        /* Calculate fill height. */

        if( delta.y ) /* Horizontal */
        {
            jj = size.y - (int) ( pen_diameter + ( 2 * pen_overlap ) );
        }
        else
        {
            jj = size.x - (int) ( pen_diameter + ( 2 * pen_overlap ) );
        }

        /* Calculation of dd = number of segments was traced to fill. */
        jj = jj / (int) ( pen_diameter - pen_overlap );

        /* Trace the outline. */
        for( ; jj > 0; jj-- )
        {
            polygone[0].x += moveX;
            polygone[0].y -= moveY;
            polygone[1].x += moveX;
            polygone[1].y += moveY;
            polygone[2].x -= moveX;
            polygone[2].y += moveY;
            polygone[3].x -= moveX;
            polygone[3].y -= moveY;

            /* Test for crossed vertexes. */
            if( polygone[0].x > polygone[3].x ) /* X axis intersection on
                                                 *vertexes 0 and 3 */
            {
                polygone[0].x = polygone[3].x = 0;
            }
            if( polygone[1].x > polygone[2].x ) /*  X axis intersection on
                                                 *vertexes 1 and 2 */
            {
                polygone[1].x = polygone[2].x = 0;
            }
            if( polygone[1].y > polygone[0].y ) /* Y axis intersection on
                                                 *vertexes 0 and 1 */
            {
                polygone[0].y = polygone[1].y = 0;
            }
            if( polygone[2].y > polygone[3].y ) /* Y axis intersection on
                                                 *vertexes 2 and 3 */
            {
                polygone[2].y = polygone[3].y = 0;
            }

            for( int ii = 0; ii < 4; ii++ )
            {
                coord[ii].x = polygone[ii].x + pos.x;
                coord[ii].y = polygone[ii].y + pos.y;
                RotatePoint( &coord[ii], pos, orient );
            }

            move_to( coord[0] );
            line_to( coord[1] );
            line_to( coord[2] );
            line_to( coord[3] );
            finish_to( coord[0] );
        }
    }
}