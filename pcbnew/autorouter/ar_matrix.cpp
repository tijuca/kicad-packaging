/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2012 Jean-Pierre Charras, jean-pierre.charras@ujf-grenoble.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2011 Wayne Stambaugh <stambaughw@verizon.net>
 *
 * Copyright (C) 1992-2015 KiCad Developers, see change_log.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "ar_matrix.h"
#include "ar_cell.h"

#include <common.h>
#include <math_for_graphics.h>
#include <trigo.h>

#include <class_drawsegment.h>
#include <class_pad.h>
#include <class_track.h>

AR_MATRIX::AR_MATRIX()
{
    m_BoardSide[0] = m_BoardSide[1] = nullptr;
    m_DistSide[0] = m_DistSide[1] = nullptr;
    m_DirSide[0] = m_DirSide[1] = nullptr;
    m_opWriteCell = nullptr;
    m_InitMatrixDone = false;
    m_Nrows = 0;
    m_Ncols = 0;
    m_MemSize = 0;
    m_RoutingLayersCount = 1;
    m_GridRouting = 0;
    m_RouteCount = 0;
}


AR_MATRIX::~AR_MATRIX()
{
}

// was: bool AR_MATRIX::ComputeMatrixSize(  BOARD* aPcb, bool aUseBoardEdgesOnly )
// aUseBoardEdgesOnly ? aPcb->GetBoardEdgesBoundingBox() : aPcb->GetBoundingBox();

bool AR_MATRIX::ComputeMatrixSize( const EDA_RECT& aBoundingBox )
{
    // The boundary box must have its start point on routing grid:
    m_BrdBox = aBoundingBox;

    m_BrdBox.SetX( m_BrdBox.GetX() - ( m_BrdBox.GetX() % m_GridRouting ) );
    m_BrdBox.SetY( m_BrdBox.GetY() - ( m_BrdBox.GetY() % m_GridRouting ) );

    // The boundary box must have its end point on routing grid:
    wxPoint end = m_BrdBox.GetEnd();

    end.x -= end.x % m_GridRouting;
    end.x += m_GridRouting;

    end.y -= end.y % m_GridRouting;
    end.y += m_GridRouting;

    m_BrdBox.SetEnd( end );

    m_Nrows = m_BrdBox.GetHeight() / m_GridRouting;
    m_Ncols = m_BrdBox.GetWidth() / m_GridRouting;

    // gives a small margin
    m_Ncols += 1;
    m_Nrows += 1;

    return true;
}


int AR_MATRIX::InitRoutingMatrix()
{
    if( m_Nrows <= 0 || m_Ncols <= 0 )
        return 0;

    m_InitMatrixDone = true; // we have been called

    // give a small margin for memory allocation:
    int ii = ( m_Nrows + 1 ) * ( m_Ncols + 1 );

    int side = AR_SIDE_BOTTOM;
    for( int jj = 0; jj < m_RoutingLayersCount; jj++ ) // m_RoutingLayersCount = 1 or 2
    {
        m_BoardSide[side] = nullptr;
        m_DistSide[side] = nullptr;
        m_DirSide[side] = nullptr;

        // allocate matrix & initialize everything to empty
        m_BoardSide[side] = (MATRIX_CELL*) operator new( ii * sizeof( MATRIX_CELL ) );
        memset( m_BoardSide[side], 0, ii * sizeof( MATRIX_CELL ) );

        if( m_BoardSide[side] == nullptr )
            return -1;

        // allocate Distances
        m_DistSide[side] = (DIST_CELL*) operator new( ii * sizeof( DIST_CELL ) );
        memset( m_DistSide[side], 0, ii * sizeof( DIST_CELL ) );

        if( m_DistSide[side] == nullptr )
            return -1;

        // allocate Dir (chars)
        m_DirSide[side] = (char*) operator new( ii );
        memset( m_DirSide[side], 0, ii );

        if( m_DirSide[side] == nullptr )
            return -1;

        side = AR_SIDE_TOP;
    }

    m_MemSize =
            m_RouteCount * ii * ( sizeof( MATRIX_CELL ) + sizeof( DIST_CELL ) + sizeof( char ) );

    return m_MemSize;
}


void AR_MATRIX::UnInitRoutingMatrix()
{
    int ii;

    m_InitMatrixDone = false;

    for( ii = 0; ii < AR_MAX_ROUTING_LAYERS_COUNT; ii++ )
    {
        // de-allocate Dir matrix
        if( m_DirSide[ii] )
        {
            delete m_DirSide[ii];
            m_DirSide[ii] = nullptr;
        }

        // de-allocate Distances matrix
        if( m_DistSide[ii] )
        {
            delete m_DistSide[ii];
            m_DistSide[ii] = nullptr;
        }

        // de-allocate cells matrix
        if( m_BoardSide[ii] )
        {
            delete m_BoardSide[ii];
            m_BoardSide[ii] = nullptr;
        }
    }

    m_Nrows = m_Ncols = 0;
}

// Initialize m_opWriteCell member to make the aLogicOp
void AR_MATRIX::SetCellOperation( AR_MATRIX::CELL_OP aLogicOp )
{
    switch( aLogicOp )
    {
    default:
    case WRITE_CELL: m_opWriteCell = &AR_MATRIX::SetCell; break;

    case WRITE_OR_CELL: m_opWriteCell = &AR_MATRIX::OrCell; break;

    case WRITE_XOR_CELL: m_opWriteCell = &AR_MATRIX::XorCell; break;

    case WRITE_AND_CELL: m_opWriteCell = &AR_MATRIX::AndCell; break;

    case WRITE_ADD_CELL: m_opWriteCell = &AR_MATRIX::AddCell; break;
    }
}


/* return the value stored in a cell
 */
AR_MATRIX::MATRIX_CELL AR_MATRIX::GetCell( int aRow, int aCol, int aSide )
{
    MATRIX_CELL* p;

    p = m_BoardSide[aSide];
    return p[aRow * m_Ncols + aCol];
}


/* basic cell operation : WRITE operation
 */
void AR_MATRIX::SetCell( int aRow, int aCol, int aSide, MATRIX_CELL x )
{
    MATRIX_CELL* p;

    p = m_BoardSide[aSide];
    p[aRow * m_Ncols + aCol] = x;
}


/* basic cell operation : OR operation
 */
void AR_MATRIX::OrCell( int aRow, int aCol, int aSide, MATRIX_CELL x )
{
    MATRIX_CELL* p;

    p = m_BoardSide[aSide];
    p[aRow * m_Ncols + aCol] |= x;
}


/* basic cell operation : XOR operation
 */
void AR_MATRIX::XorCell( int aRow, int aCol, int aSide, MATRIX_CELL x )
{
    MATRIX_CELL* p;

    p = m_BoardSide[aSide];
    p[aRow * m_Ncols + aCol] ^= x;
}


/* basic cell operation : AND operation
 */
void AR_MATRIX::AndCell( int aRow, int aCol, int aSide, MATRIX_CELL x )
{
    MATRIX_CELL* p;

    p = m_BoardSide[aSide];
    p[aRow * m_Ncols + aCol] &= x;
}


/* basic cell operation : ADD operation
 */
void AR_MATRIX::AddCell( int aRow, int aCol, int aSide, MATRIX_CELL x )
{
    MATRIX_CELL* p;

    p = m_BoardSide[aSide];
    p[aRow * m_Ncols + aCol] += x;
}


// fetch distance cell
AR_MATRIX::DIST_CELL AR_MATRIX::GetDist( int aRow, int aCol, int aSide ) // fetch distance cell
{
    DIST_CELL* p;

    p = m_DistSide[aSide];
    return p[aRow * m_Ncols + aCol];
}


// store distance cell
void AR_MATRIX::SetDist( int aRow, int aCol, int aSide, DIST_CELL x )
{
    DIST_CELL* p;

    p = m_DistSide[aSide];
    p[aRow * m_Ncols + aCol] = x;
}


// fetch direction cell
int AR_MATRIX::GetDir( int aRow, int aCol, int aSide )
{
    DIR_CELL* p;

    p = m_DirSide[aSide];
    return (int) ( p[aRow * m_Ncols + aCol] );
}


// store direction cell
void AR_MATRIX::SetDir( int aRow, int aCol, int aSide, int x )
{
    DIR_CELL* p;

    p = m_DirSide[aSide];
    p[aRow * m_Ncols + aCol] = (char) x;
}

/* The tables of distances and keep out areas are established on the basis of a
 * 50 units grid size (the pitch between the cells is 50 units).
 * The actual distance could be computed by a scaling factor, but this is
 * not needed, we can use only reduced values
 */

/* calculate approximate distance (manhattan distance)
 */
int AR_MATRIX::GetApxDist( int r1, int c1, int r2, int c2 )
{
    int d1, d2; /* row and column deltas */

    if( ( d1 = r1 - r2 ) < 0 ) /* get absolute row delta */
        d1 = -d1;

    if( ( d2 = c1 - c2 ) < 0 ) /* get absolute column delta */
        d2 = -d2;

    return ( d1 + d2 ) * 50;
}


/* distance to go thru a cell (en mils) */
static const int dist[10][10] = { /* OT=Otherside, OR=Origin (source) cell */
                                  /*..........N, NE,  E, SE,  S, SW,  W, NW,   OT, OR */
    /* N  */ { 50, 60, 35, 60, 99, 60, 35, 60, 12, 12 },
    /* NE */ { 60, 71, 60, 71, 60, 99, 60, 71, 23, 23 },
    /* E  */ { 35, 60, 50, 60, 35, 60, 99, 60, 12, 12 },
    /* SE */ { 60, 71, 60, 71, 60, 71, 60, 99, 23, 23 },
    /* S  */ { 99, 60, 35, 60, 50, 60, 35, 60, 12, 12 },
    /* SW */ { 60, 99, 60, 71, 60, 71, 60, 71, 23, 23 },
    /* W  */ { 35, 60, 99, 60, 35, 60, 50, 60, 12, 12 },
    /* NW */ { 60, 71, 60, 99, 60, 71, 60, 71, 23, 23 },

    /* OT */ { 12, 23, 12, 23, 12, 23, 12, 23, 99, 99 },
    /* OR */ { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 }
};


/* penalty for extraneous holes and corners, scaled by sharpness of turn */
static const int penalty[10][10] = { /* OT=Otherside, OR=Origin (source) cell */
                                     /*......... N, NE,  E, SE,  S, SW,  W, NW,   OT, OR */
    /* N  */ { 0, 5, 10, 15, 20, 15, 10, 5, 50, 0 },
    /* NE */ { 5, 0, 5, 10, 15, 20, 15, 10, 50, 0 },
    /* E  */ { 10, 5, 0, 5, 10, 15, 20, 15, 50, 0 },
    /* SE */ { 15, 10, 5, 0, 5, 10, 15, 20, 50, 0 },
    /* S  */ { 20, 15, 10, 5, 0, 5, 10, 15, 50, 0 },
    /* SW */ { 15, 20, 15, 10, 5, 0, 5, 10, 50, 0 },
    /* W  */ { 10, 15, 20, 15, 10, 5, 0, 5, 50, 0 },
    /* NW */ { 5, 10, 15, 20, 15, 10, 5, 0, 50, 0 },

    /* OT */ { 50, 50, 50, 50, 50, 50, 50, 50, 100, 0 },
    /* OR */ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};


/* penalty pour directions preferencielles */
#define PN 20
static const int dir_penalty_TOP[10][10] = {
    /* OT=Otherside, OR=Origin (source) cell */
    /*......... N, NE,  E, SE,  S, SW,  W, NW,   OT, OR */
    /* N  */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },
    /* NE */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },
    /* E  */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },
    /* SE */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },
    /* S  */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },
    /* SW */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },
    /* W  */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },
    /* NW */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },

    /* OT */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 },
    /* OR */ { PN, 0, 0, 0, PN, 0, 0, 0, 0, 0 }
};


static int dir_penalty_BOTTOM[10][10] = {
    /* OT=Otherside, OR=Origin (source) cell */
    /*......... N, NE,  E, SE,  S, SW,  W, NW,   OT, OR */
    /* N  */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },
    /* NE */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },
    /* E  */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },
    /* SE */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },
    /* S  */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },
    /* SW */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },
    /* W  */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },
    /* NW */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },

    /* OT */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 },
    /* OR */ { 0, 0, PN, 0, 0, 0, PN, 0, 0, 0 }
};

/*
** x is the direction to enter the cell of interest.
** y is the direction to exit the cell of interest.
** z is the direction to really exit the cell, if y=FROM_OTHERSIDE.
**
** return the distance of the trace through the cell of interest.
** the calculation is driven by the tables above.
*/


/* calculate distance (with penalty) of a trace through a cell
*/
int AR_MATRIX::CalcDist( int x, int y, int z, int side )
{
    int adjust, ldist;

    adjust = 0; /* set if hole is encountered */

    if( x == CELL_IS_EMPTY )
        x = 10;

    if( y == CELL_IS_EMPTY )
    {
        y = 10;
    }
    else if( y == FROM_OTHERSIDE )
    {
        if( z == CELL_IS_EMPTY )
            z = 10;

        adjust = penalty[x - 1][z - 1];
    }

    ldist = dist[x - 1][y - 1] + penalty[x - 1][y - 1] + adjust;

    if( m_RouteCount > 1 )
    {
        if( side == AR_SIDE_BOTTOM )
            ldist += dir_penalty_TOP[x - 1][y - 1];

        if( side == AR_SIDE_TOP )
            ldist += dir_penalty_BOTTOM[x - 1][y - 1];
    }

    return ldist * 10;
}


#define OP_CELL( layer, dy, dx )                             \
    {                                                        \
        if( layer == UNDEFINED_LAYER )                       \
        {                                                    \
            WriteCell( dy, dx, AR_SIDE_BOTTOM, color );      \
            if( m_RoutingLayersCount > 1 )                   \
                WriteCell( dy, dx, AR_SIDE_TOP, color );     \
        }                                                    \
        else                                                 \
        {                                                    \
            if( layer == m_routeLayerBottom )                \
                WriteCell( dy, dx, AR_SIDE_BOTTOM, color );  \
            if( m_RoutingLayersCount > 1 )                   \
                if( layer == m_routeLayerTop )               \
                    WriteCell( dy, dx, AR_SIDE_TOP, color ); \
        }                                                    \
    }

/* Fills all cells inside a segment
 * half-width = lg, org = ux0,uy0 end = ux1,uy1
 * coordinates are in PCB units
 */
void AR_MATRIX::drawSegmentQcq( int ux0, int uy0, int ux1, int uy1, int lg, LAYER_NUM layer,
        int color, AR_MATRIX::CELL_OP op_logic )
{
    int64_t row, col;
    int64_t inc;
    int64_t row_max, col_max, row_min, col_min;
    int64_t demi_pas;

    int cx, cy, dx, dy;


    //printf("segmQcq %d %d %d %d\n", ux0, uy0, ux1, uy1);

    SetCellOperation( op_logic );

    // Make coordinate ux1 tj > ux0 to simplify calculations
    if( ux1 < ux0 )
    {
        std::swap( ux1, ux0 );
        std::swap( uy1, uy0 );
    }

    // Calculating the incrementing the Y axis
    inc = 1;

    if( uy1 < uy0 )
        inc = -1;

    demi_pas = m_GridRouting / 2;

    col_min = ( ux0 - lg ) / m_GridRouting;

    if( col_min < 0 )
        col_min = 0;

    col_max = ( ux1 + lg + demi_pas ) / m_GridRouting;

    if( col_max > ( m_Ncols - 1 ) )
        col_max = m_Ncols - 1;

    if( inc > 0 )
    {
        row_min = ( uy0 - lg ) / m_GridRouting;
        row_max = ( uy1 + lg + demi_pas ) / m_GridRouting;
    }
    else
    {
        row_min = ( uy1 - lg ) / m_GridRouting;
        row_max = ( uy0 + lg + demi_pas ) / m_GridRouting;
    }

    if( row_min < 0 )
        row_min = 0;

    if( row_min > ( m_Nrows - 1 ) )
        row_min = m_Nrows - 1;

    if( row_max < 0 )
        row_max = 0;

    if( row_max > ( m_Nrows - 1 ) )
        row_max = m_Nrows - 1;

    dx = ux1 - ux0;
    dy = uy1 - uy0;

    double angle;
    if( dx )
    {
        angle = ArcTangente( dy, dx );
    }
    else
    {
        angle = 900;

        if( dy < 0 )
            angle = -900;
    }

    RotatePoint( &dx, &dy, angle ); // dx = length, dy = 0


    //printf("col_min %d max %d row_min %d max %d\n", col_min, col_max, row_min, row_max);

    for( col = col_min; col <= col_max; col++ )
    {
        int64_t cxr;
        cxr = ( col * m_GridRouting ) - ux0;

        for( row = row_min; row <= row_max; row++ )
        {
            cy = ( row * m_GridRouting ) - uy0;
            cx = cxr;
            RotatePoint( &cx, &cy, angle );

            if( abs( cy ) > lg )
                continue; // The point is too far on the Y axis.

            /* This point a test is close to the segment: the position
             * along the X axis must be tested.
             */
            if( ( cx >= 0 ) && ( cx <= dx ) )
            {
                OP_CELL( layer, row, col );
                continue;
            }

            // Examination of extremities are rounded.
            if( ( cx < 0 ) && ( cx >= -lg ) )
            {
                if( ( ( cx * cx ) + ( cy * cy ) ) <= ( lg * lg ) )
                    OP_CELL( layer, row, col );

                continue;
            }

            if( ( cx > dx ) && ( cx <= ( dx + lg ) ) )
            {
                if( ( ( ( cx - dx ) * ( cx - dx ) ) + ( cy * cy ) ) <= ( lg * lg ) )
                    OP_CELL( layer, row, col );

                continue;
            }
        }
    }
}


/* Fills all cells of the routing matrix contained in the circle
 * half-width = lg, center = ux0, uy0, ux1,uy1 is a point on the circle.
 * coord are in PCB units.
 */
void AR_MATRIX::traceCircle( int ux0, int uy0, int ux1, int uy1, int lg, LAYER_NUM layer, int color,
        AR_MATRIX::CELL_OP op_logic )
{
    int radius, nb_segm;
    int x0, y0,     // Starting point of the current segment trace.
            x1, y1; // End point.
    int ii;
    int angle;

    radius = KiROUND( Distance( ux0, uy0, ux1, uy1 ) );

    x0 = x1 = radius;
    y0 = y1 = 0;

    if( lg < 1 )
        lg = 1;

    nb_segm = ( 2 * radius ) / lg;

    if( nb_segm < 5 )
        nb_segm = 5;

    if( nb_segm > 100 )
        nb_segm = 100;

    for( ii = 1; ii < nb_segm; ii++ )
    {
        angle = ( 3600 * ii ) / nb_segm;
        x1 = KiROUND( cosdecideg( radius, angle ) );
        y1 = KiROUND( sindecideg( radius, angle ) );
        drawSegmentQcq( x0 + ux0, y0 + uy0, x1 + ux0, y1 + uy0, lg, layer, color, op_logic );
        x0 = x1;
        y0 = y1;
    }

    drawSegmentQcq( x1 + ux0, y1 + uy0, ux0 + radius, uy0, lg, layer, color, op_logic );
}


void AR_MATRIX::traceFilledCircle(
        int cx, int cy, int radius, LSET aLayerMask, int color, AR_MATRIX::CELL_OP op_logic )
{
    int    row, col;
    int    ux0, uy0, ux1, uy1;
    int    row_max, col_max, row_min, col_min;
    int    trace = 0;
    double fdistmin, fdistx, fdisty;
    int    tstwrite = 0;
    int    distmin;

    if( aLayerMask[m_routeLayerBottom] )
        trace = 1; // Trace on BOTTOM

    if( aLayerMask[m_routeLayerTop] )
        if( m_RoutingLayersCount > 1 )
            trace |= 2; // Trace on TOP

    if( trace == 0 )
        return;

    SetCellOperation( op_logic );

    cx -= GetBrdCoordOrigin().x;
    cy -= GetBrdCoordOrigin().y;

    distmin = radius;

    // Calculate the bounding rectangle of the circle.
    ux0 = cx - radius;
    uy0 = cy - radius;
    ux1 = cx + radius;
    uy1 = cy + radius;

    // Calculate limit coordinates of cells belonging to the rectangle.
    row_max = uy1 / m_GridRouting;
    col_max = ux1 / m_GridRouting;
    row_min = uy0 / m_GridRouting; // if (uy0 > row_min*Board.m_GridRouting) row_min++;
    col_min = ux0 / m_GridRouting; // if (ux0 > col_min*Board.m_GridRouting) col_min++;

    if( row_min < 0 )
        row_min = 0;

    if( row_max >= ( m_Nrows - 1 ) )
        row_max = m_Nrows - 1;

    if( col_min < 0 )
        col_min = 0;

    if( col_max >= ( m_Ncols - 1 ) )
        col_max = m_Ncols - 1;

    // Calculate coordinate limits of cell belonging to the rectangle.
    if( row_min > row_max )
        row_max = row_min;

    if( col_min > col_max )
        col_max = col_min;

    fdistmin = (double) distmin * distmin;

    for( row = row_min; row <= row_max; row++ )
    {
        fdisty = (double) ( cy - ( row * m_GridRouting ) );
        fdisty *= fdisty;

        for( col = col_min; col <= col_max; col++ )
        {
            fdistx = (double) ( cx - ( col * m_GridRouting ) );
            fdistx *= fdistx;

            if( fdistmin <= ( fdistx + fdisty ) )
                continue;

            if( trace & 1 )
                WriteCell( row, col, AR_SIDE_BOTTOM, color );

            if( trace & 2 )
                WriteCell( row, col, AR_SIDE_TOP, color );

            tstwrite = 1;
        }
    }

    if( tstwrite )
        return;

    /* If no cell has been written, it affects the 4 neighboring diagonal
     * (Adverse event: pad off grid in the center of the 4 neighboring
     * diagonal) */
    distmin = m_GridRouting / 2 + 1;
    fdistmin = ( (double) distmin * distmin ) * 2; // Distance to center point diagonally

    for( row = row_min; row <= row_max; row++ )
    {
        fdisty = (double) ( cy - ( row * m_GridRouting ) );
        fdisty *= fdisty;

        for( col = col_min; col <= col_max; col++ )
        {
            fdistx = (double) ( cx - ( col * m_GridRouting ) );
            fdistx *= fdistx;

            if( fdistmin <= ( fdistx + fdisty ) )
                continue;

            if( trace & 1 )
                WriteCell( row, col, AR_SIDE_BOTTOM, color );

            if( trace & 2 )
                WriteCell( row, col, AR_SIDE_TOP, color );
        }
    }
}


/* Fills all routing matrix cells contained in the arc
 * angle = ArcAngle, half-width lg
 * center = ux0,uy0, starting at ux1, uy1.  Coordinates are in
 * PCB units.
 */
void AR_MATRIX::traceArc( int ux0, int uy0, int ux1, int uy1, double ArcAngle, int lg,
        LAYER_NUM layer, int color, AR_MATRIX::CELL_OP op_logic )
{
    int radius, nb_segm;
    int x0, y0,     // Starting point of the current segment trace
            x1, y1; // End point
    int    ii;
    double angle, StAngle;


    radius = KiROUND( Distance( ux0, uy0, ux1, uy1 ) );

    x0 = ux1 - ux0;
    y0 = uy1 - uy0;
    StAngle = ArcTangente( uy1 - uy0, ux1 - ux0 );

    if( lg < 1 )
        lg = 1;

    nb_segm = ( 2 * radius ) / lg;
    nb_segm = ( nb_segm * std::abs( ArcAngle ) ) / 3600;

    if( nb_segm < 5 )
        nb_segm = 5;

    if( nb_segm > 100 )
        nb_segm = 100;

    for( ii = 1; ii <= nb_segm; ii++ )
    {
        angle = ( ArcAngle * ii ) / nb_segm;
        angle += StAngle;

        NORMALIZE_ANGLE_POS( angle );

        x1 = KiROUND( cosdecideg( radius, angle ) );
        y1 = KiROUND( cosdecideg( radius, angle ) );
        drawSegmentQcq( x0 + ux0, y0 + uy0, x1 + ux0, y1 + uy0, lg, layer, color, op_logic );
        x0 = x1;
        y0 = y1;
    }
}


void AR_MATRIX::TraceFilledRectangle( int ux0, int uy0, int ux1, int uy1, double angle,
        LSET aLayerMask, int color, AR_MATRIX::CELL_OP op_logic )
{
    int row, col;
    int cx, cy; // Center of rectangle
    int radius; // Radius of the circle
    int row_min, row_max, col_min, col_max;
    int rotrow, rotcol;
    int trace = 0;

    if( aLayerMask[m_routeLayerBottom] )
        trace = 1; // Trace on BOTTOM

    if( aLayerMask[m_routeLayerTop] )
    {
        if( m_RoutingLayersCount > 1 )
            trace |= 2; // Trace on TOP
    }

    if( trace == 0 )
        return;

    SetCellOperation( op_logic );

    ux0 -= GetBrdCoordOrigin().x;
    uy0 -= GetBrdCoordOrigin().y;
    ux1 -= GetBrdCoordOrigin().x;
    uy1 -= GetBrdCoordOrigin().y;

    cx = ( ux0 + ux1 ) / 2;
    cy = ( uy0 + uy1 ) / 2;
    radius = KiROUND( Distance( ux0, uy0, cx, cy ) );

    // Calculating coordinate limits belonging to the rectangle.
    row_max = ( cy + radius ) / m_GridRouting;
    col_max = ( cx + radius ) / m_GridRouting;
    row_min = ( cy - radius ) / m_GridRouting;

    if( uy0 > row_min * m_GridRouting )
        row_min++;

    col_min = ( cx - radius ) / m_GridRouting;

    if( ux0 > col_min * m_GridRouting )
        col_min++;

    if( row_min < 0 )
        row_min = 0;

    if( row_max >= ( m_Nrows - 1 ) )
        row_max = m_Nrows - 1;

    if( col_min < 0 )
        col_min = 0;

    if( col_max >= ( m_Ncols - 1 ) )
        col_max = m_Ncols - 1;

    for( row = row_min; row <= row_max; row++ )
    {
        for( col = col_min; col <= col_max; col++ )
        {
            rotrow = row * m_GridRouting;
            rotcol = col * m_GridRouting;
            RotatePoint( &rotcol, &rotrow, cx, cy, -angle );

            if( rotrow <= uy0 )
                continue;

            if( rotrow >= uy1 )
                continue;

            if( rotcol <= ux0 )
                continue;

            if( rotcol >= ux1 )
                continue;

            if( trace & 1 )
                WriteCell( row, col, AR_SIDE_BOTTOM, color );

            if( trace & 2 )
                WriteCell( row, col, AR_SIDE_TOP, color );
        }
    }
}


void AR_MATRIX::TraceFilledRectangle( int ux0, int uy0, int ux1, int uy1, LSET aLayerMask,
        int color, AR_MATRIX::CELL_OP op_logic )
{
    int row, col;
    int row_min, row_max, col_min, col_max;
    int trace = 0;

    if( aLayerMask[m_routeLayerBottom] )
        trace = 1; // Trace on BOTTOM

    if( aLayerMask[m_routeLayerTop] && m_RoutingLayersCount > 1 )
        trace |= 2; // Trace on TOP

    if( trace == 0 )
        return;

    SetCellOperation( op_logic );

    ux0 -= GetBrdCoordOrigin().x;
    uy0 -= GetBrdCoordOrigin().y;
    ux1 -= GetBrdCoordOrigin().x;
    uy1 -= GetBrdCoordOrigin().y;

    // Calculating limits coord cells belonging to the rectangle.
    row_max = uy1 / m_GridRouting;
    col_max = ux1 / m_GridRouting;
    row_min = uy0 / m_GridRouting;

    if( uy0 > row_min * m_GridRouting )
        row_min++;

    col_min = ux0 / m_GridRouting;

    if( ux0 > col_min * m_GridRouting )
        col_min++;

    if( row_min < 0 )
        row_min = 0;

    if( row_max >= ( m_Nrows - 1 ) )
        row_max = m_Nrows - 1;

    if( col_min < 0 )
        col_min = 0;

    if( col_max >= ( m_Ncols - 1 ) )
        col_max = m_Ncols - 1;

    for( row = row_min; row <= row_max; row++ )
    {
        for( col = col_min; col <= col_max; col++ )
        {
            if( trace & 1 )
                WriteCell( row, col, AR_SIDE_BOTTOM, color );

            if( trace & 2 )
                WriteCell( row, col, AR_SIDE_TOP, color );
        }
    }
}


/* Draws a line, if layer = -1 on all layers
 */
void AR_MATRIX::tracePcbLine(
        int x0, int y0, int x1, int y1, LAYER_NUM layer, int color, AR_MATRIX::CELL_OP op_logic )
{
    int dx, dy, lim;
    int cumul, inc, il, delta;

    SetCellOperation( op_logic );

    if( x0 == x1 ) // Vertical.
    {
        if( y1 < y0 )
            std::swap( y0, y1 );

        dy = y0 / m_GridRouting;
        lim = y1 / m_GridRouting;
        dx = x0 / m_GridRouting;

        // Clipping limits of board.
        if( ( dx < 0 ) || ( dx >= m_Ncols ) )
            return;

        if( dy < 0 )
            dy = 0;

        if( lim >= m_Nrows )
            lim = m_Nrows - 1;

        for( ; dy <= lim; dy++ )
        {
            OP_CELL( layer, dy, dx );
        }

        return;
    }

    if( y0 == y1 ) // Horizontal
    {
        if( x1 < x0 )
            std::swap( x0, x1 );

        dx = x0 / m_GridRouting;
        lim = x1 / m_GridRouting;
        dy = y0 / m_GridRouting;

        // Clipping limits of board.
        if( ( dy < 0 ) || ( dy >= m_Nrows ) )
            return;

        if( dx < 0 )
            dx = 0;

        if( lim >= m_Ncols )
            lim = m_Ncols - 1;

        for( ; dx <= lim; dx++ )
        {
            OP_CELL( layer, dy, dx );
        }

        return;
    }

    // Here is some perspective: using the algorithm LUCAS.
    if( abs( x1 - x0 ) >= abs( y1 - y0 ) ) // segment slightly inclined/
    {
        if( x1 < x0 )
        {
            std::swap( x1, x0 );
            std::swap( y1, y0 );
        }

        dx = x0 / m_GridRouting;
        lim = x1 / m_GridRouting;
        dy = y0 / m_GridRouting;
        inc = 1;

        if( y1 < y0 )
            inc = -1;

        il = lim - dx;
        cumul = il / 2;
        delta = abs( y1 - y0 ) / m_GridRouting;

        for( ; dx <= lim; )
        {
            if( ( dx >= 0 ) && ( dy >= 0 ) && ( dx < m_Ncols ) && ( dy < m_Nrows ) )
            {
                OP_CELL( layer, dy, dx );
            }

            dx++;
            cumul += delta;

            if( cumul > il )
            {
                cumul -= il;
                dy += inc;
            }
        }
    }
    else
    {
        if( y1 < y0 )
        {
            std::swap( x1, x0 );
            std::swap( y1, y0 );
        }

        dy = y0 / m_GridRouting;
        lim = y1 / m_GridRouting;
        dx = x0 / m_GridRouting;
        inc = 1;

        if( x1 < x0 )
            inc = -1;

        il = lim - dy;
        cumul = il / 2;
        delta = abs( x1 - x0 ) / m_GridRouting;

        for( ; dy <= lim; )
        {
            if( ( dx >= 0 ) && ( dy >= 0 ) && ( dx < m_Ncols ) && ( dy < m_Nrows ) )
            {
                OP_CELL( layer, dy, dx );
            }

            dy++;
            cumul += delta;

            if( cumul > il )
            {
                cumul -= il;
                dx += inc;
            }
        }
    }
}

void AR_MATRIX::TraceSegmentPcb(
        DRAWSEGMENT* pt_segm, int color, int marge, AR_MATRIX::CELL_OP op_logic )
{
    int half_width = ( pt_segm->GetWidth() / 2 ) + marge;

    // Calculate the bounding rectangle of the segment (if H, V or Via)
    int ux0 = pt_segm->GetStart().x - GetBrdCoordOrigin().x;
    int uy0 = pt_segm->GetStart().y - GetBrdCoordOrigin().y;
    int ux1 = pt_segm->GetEnd().x - GetBrdCoordOrigin().x;
    int uy1 = pt_segm->GetEnd().y - GetBrdCoordOrigin().y;

    //printf("traceSegm %d %d %d %d\n", ux0, uy0, ux1, uy1);

    LAYER_NUM layer = pt_segm->GetLayer();

    //if( color == VIA_IMPOSSIBLE )
    layer = UNDEFINED_LAYER;


    switch( pt_segm->GetShape() )
    {
    // The segment is here a straight line or a circle or an arc.:
    case S_CIRCLE: traceCircle( ux0, uy0, ux1, uy1, half_width, layer, color, op_logic ); break;

    case S_ARC:
        traceArc( ux0, uy0, ux1, uy1, pt_segm->GetAngle(), half_width, layer, color, op_logic );
        break;

    // The segment is here a line segment.
    default: drawSegmentQcq( ux0, uy0, ux1, uy1, half_width, layer, color, op_logic ); break;
    }
}


void AR_MATRIX::TraceSegmentPcb( TRACK* aTrack, int color, int marge, AR_MATRIX::CELL_OP op_logic )
{
    int half_width = ( aTrack->GetWidth() / 2 ) + marge;

    // Test if VIA (filled circle need to be drawn)
    if( aTrack->Type() == PCB_VIA_T )
    {
        LSET layer_mask;

        if( aTrack->IsOnLayer( m_routeLayerBottom ) )
            layer_mask.set( m_routeLayerBottom );

        if( aTrack->IsOnLayer( m_routeLayerTop ) )
        {
            if( !layer_mask.any() )
                layer_mask = LSET( m_routeLayerTop );
            else
                layer_mask.set();
        }

        if( color == VIA_IMPOSSIBLE )
            layer_mask.set();

        if( layer_mask.any() )
            traceFilledCircle( aTrack->GetStart().x, aTrack->GetStart().y, half_width, layer_mask,
                    color, op_logic );
    }
    else
    {
        // Calculate the bounding rectangle of the segment
        int ux0 = aTrack->GetStart().x - GetBrdCoordOrigin().x;
        int uy0 = aTrack->GetStart().y - GetBrdCoordOrigin().y;
        int ux1 = aTrack->GetEnd().x - GetBrdCoordOrigin().x;
        int uy1 = aTrack->GetEnd().y - GetBrdCoordOrigin().y;

        // Ordinary track
        PCB_LAYER_ID layer = aTrack->GetLayer();

        if( color == VIA_IMPOSSIBLE )
            layer = UNDEFINED_LAYER;

        drawSegmentQcq( ux0, uy0, ux1, uy1, half_width, layer, color, op_logic );
    }
}


/**
 * Function CreateKeepOutRectangle
 * builds the cost map:
 * Cells ( in Dist map ) inside the rect x0,y0 a x1,y1 are
 *  incremented by value aKeepOut
 *  Cell outside this rectangle, but inside the rectangle
 *  x0,y0 -marge to x1,y1 + marge are incremented by a decreasing value
 *  (aKeepOut ... 0). The decreasing value depends on the distance to the first rectangle
 *  Therefore the cost is high in rect x0,y0 to x1,y1, and decrease outside this rectangle
 */
void AR_MATRIX::CreateKeepOutRectangle(
        int ux0, int uy0, int ux1, int uy1, int marge, int aKeepOut, LSET aLayerMask )
{
    int       row, col;
    int       row_min, row_max, col_min, col_max, pmarge;
    int       trace = 0;
    DIST_CELL data, LocalKeepOut;
    int       lgain, cgain;

    if( aLayerMask[m_routeLayerBottom] )
        trace = 1; // Trace on bottom layer.

    if( aLayerMask[m_routeLayerTop] && m_RoutingLayersCount )
        trace |= 2; // Trace on top layer.

    if( trace == 0 )
        return;

    ux0 -= m_BrdBox.GetX();
    uy0 -= m_BrdBox.GetY();
    ux1 -= m_BrdBox.GetX();
    uy1 -= m_BrdBox.GetY();

    ux0 -= marge;
    ux1 += marge;
    uy0 -= marge;
    uy1 += marge;

    pmarge = marge / m_GridRouting;

    if( pmarge < 1 )
        pmarge = 1;

    // Calculate the coordinate limits of the rectangle.
    row_max = uy1 / m_GridRouting;
    col_max = ux1 / m_GridRouting;
    row_min = uy0 / m_GridRouting;

    if( uy0 > row_min * m_GridRouting )
        row_min++;

    col_min = ux0 / m_GridRouting;

    if( ux0 > col_min * m_GridRouting )
        col_min++;

    if( row_min < 0 )
        row_min = 0;

    if( row_max >= ( m_Nrows - 1 ) )
        row_max = m_Nrows - 1;

    if( col_min < 0 )
        col_min = 0;

    if( col_max >= ( m_Ncols - 1 ) )
        col_max = m_Ncols - 1;

    for( row = row_min; row <= row_max; row++ )
    {
        lgain = 256;

        if( row < pmarge )
            lgain = ( 256 * row ) / pmarge;
        else if( row > row_max - pmarge )
            lgain = ( 256 * ( row_max - row ) ) / pmarge;

        for( col = col_min; col <= col_max; col++ )
        {
            // RoutingMatrix Dist map containt the "cost" of the cell
            // at position (row, col)
            // in autoplace this is the cost of the cell, when
            // a footprint overlaps it, near a "master" footprint
            // this cost is hight near the "master" footprint
            // and decrease with the distance
            cgain = 256;
            LocalKeepOut = aKeepOut;

            if( col < pmarge )
                cgain = ( 256 * col ) / pmarge;
            else if( col > col_max - pmarge )
                cgain = ( 256 * ( col_max - col ) ) / pmarge;

            cgain = ( cgain * lgain ) / 256;

            if( cgain != 256 )
                LocalKeepOut = ( LocalKeepOut * cgain ) / 256;

            if( trace & 1 )
            {
                data = GetDist( row, col, AR_SIDE_BOTTOM ) + LocalKeepOut;
                SetDist( row, col, AR_SIDE_BOTTOM, data );
            }

            if( trace & 2 )
            {
                data = GetDist( row, col, AR_SIDE_TOP );
                data = std::max( data, LocalKeepOut );
                SetDist( row, col, AR_SIDE_TOP, data );
            }
        }
    }
}


void AR_MATRIX::PlacePad( D_PAD* aPad, int color, int marge, AR_MATRIX::CELL_OP op_logic )
{
    int     dx, dy;
    wxPoint shape_pos = aPad->ShapePos();

    dx = aPad->GetSize().x / 2;
    dx += marge;

    if( aPad->GetShape() == PAD_SHAPE_CIRCLE )
    {
        traceFilledCircle( shape_pos.x, shape_pos.y, dx, aPad->GetLayerSet(), color, op_logic );
        return;
    }

    dy = aPad->GetSize().y / 2;
    dy += marge;

    if( aPad->GetShape() == PAD_SHAPE_TRAPEZOID )
    {
        dx += abs( aPad->GetDelta().y ) / 2;
        dy += abs( aPad->GetDelta().x ) / 2;
    }

    // The pad is a rectangle ( horizontal or vertical )
    if( int( aPad->GetOrientation() ) % 900 == 0 )
    {
        // Orientation turned 90 deg.
        if( aPad->GetOrientation() == 900 || aPad->GetOrientation() == 2700 )
        {
            std::swap( dx, dy );
        }

        TraceFilledRectangle( shape_pos.x - dx, shape_pos.y - dy, shape_pos.x + dx,
                shape_pos.y + dy, aPad->GetLayerSet(), color, op_logic );
    }
    else
    {
        TraceFilledRectangle( shape_pos.x - dx, shape_pos.y - dy, shape_pos.x + dx,
                shape_pos.y + dy, aPad->GetOrientation(), aPad->GetLayerSet(), color, op_logic );
    }
}
