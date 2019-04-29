/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see CHANGELOG.TXT for contributors.
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

#include <geometry/shape_arc.h>

#include <geometry/shape_line_chain.h>

#include <unit_test_utils/geometry.h>
#include <unit_test_utils/numeric.h>
#include <unit_test_utils/unit_test_utils.h>

#include "geom_test_utils.h"

BOOST_AUTO_TEST_SUITE( ShapeArc )

/**
 * All properties of an arc (depending on how it's constructed, some of these
 * might be the same as the constructor params)
 */
struct ARC_PROPERTIES
{
    VECTOR2I m_center_point;
    VECTOR2I m_start_point;
    VECTOR2I m_end_point;
    double   m_center_angle;
    double   m_start_angle;
    double   m_end_angle;
    int      m_radius;
    BOX2I    m_bbox;
};

/**
 * Check a #SHAPE_ARC against a given set of geometric properties
 */
static void CheckArcGeom( const SHAPE_ARC& aArc, const ARC_PROPERTIES& aProps )
{
    // Angular error - not this can get quite large for very small arcs,
    // as the integral position rounding has a relatively greater effect
    const double angle_tol_deg = 0.01;

    // Position error - rounding to nearest integer
    const int pos_tol = 1;

    BOOST_CHECK_PREDICATE( KI_TEST::IsVecWithinTol<VECTOR2I>,
            ( aProps.m_start_point )( aProps.m_start_point )( pos_tol ) );
    BOOST_CHECK_PREDICATE(
            KI_TEST::IsVecWithinTol<VECTOR2I>, ( aArc.GetP1() )( aProps.m_end_point )( pos_tol ) );
    BOOST_CHECK_PREDICATE( KI_TEST::IsVecWithinTol<VECTOR2I>,
            ( aArc.GetCenter() )( aProps.m_center_point )( pos_tol ) );
    BOOST_CHECK_PREDICATE( KI_TEST::IsWithin<double>,
            ( aArc.GetCentralAngle() )( aProps.m_center_angle )( angle_tol_deg ) );
    BOOST_CHECK_PREDICATE( KI_TEST::IsWithin<double>,
            ( aArc.GetStartAngle() )( aProps.m_start_angle )( angle_tol_deg ) );
    BOOST_CHECK_PREDICATE( KI_TEST::IsWithin<double>,
            ( aArc.GetEndAngle() )( aProps.m_end_angle )( angle_tol_deg ) );
    BOOST_CHECK_PREDICATE(
            KI_TEST::IsWithin<double>, ( aArc.GetRadius() )( aProps.m_radius )( pos_tol ) );

    /// Check the chord agrees
    const auto chord = aArc.GetChord();

    BOOST_CHECK_PREDICATE(
            KI_TEST::IsVecWithinTol<VECTOR2I>, ( chord.A )( aProps.m_start_point )( pos_tol ) );
    BOOST_CHECK_PREDICATE(
            KI_TEST::IsVecWithinTol<VECTOR2I>, ( chord.B )( aProps.m_end_point )( pos_tol ) );

    /// All arcs are solid
    BOOST_CHECK_EQUAL( aArc.IsSolid(), true );

    BOOST_CHECK_PREDICATE(
            KI_TEST::IsBoxWithinTol<BOX2I>, ( aArc.BBox() )( aProps.m_bbox )( pos_tol ) );

    /// Collisions will be checked elsewhere.
}

/**
 * Check an arcs geometry and other class functions
 */
static void CheckArc( const SHAPE_ARC& aArc, const ARC_PROPERTIES& aProps )
{
    // Check the original arc
    CheckArcGeom( aArc, aProps );

    // Test the Clone function (also tests copy-ctor)
    std::unique_ptr<SHAPE> new_shape{ aArc.Clone() };

    BOOST_CHECK_EQUAL( new_shape->Type(), SH_ARC );

    SHAPE_ARC* new_arc = dynamic_cast<SHAPE_ARC*>( new_shape.get() );

    BOOST_REQUIRE( new_arc != nullptr );

    /// Should have identical geom props
    CheckArcGeom( *new_arc, aProps );
}

/**
 * Check correct handling of filter strings (as used by WX)
 */
BOOST_AUTO_TEST_CASE( NullCtor )
{
    auto arc = SHAPE_ARC();

    BOOST_CHECK_EQUAL( arc.GetWidth(), 0 );

    static ARC_PROPERTIES null_props{
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        0,
        0,
        0,
        0,
    };

    CheckArc( arc, null_props );
}

/**
 * Info to set up an arc by centre, start point and angle
 *
 * In future there may be more ways to set this up, so keep it separate
 */
struct ARC_CENTRE_PT_ANGLE
{
    VECTOR2I m_center_point;
    VECTOR2I m_start_point;
    double   m_center_angle;
};

struct ARC_CPA_CASE
{
    /// The text context name
    std::string m_ctx_name;

    /// Geom of the arc
    ARC_CENTRE_PT_ANGLE m_geom;

    /// Arc line width
    int m_width;

    /// Expected properties
    ARC_PROPERTIES m_properties;
};

static const std::vector<ARC_CPA_CASE> arc_cases = {
    {
            "C(0,0) 180 + 90 degree",
            {
                    { 0, 0 },
                    { -100, 0 },
                    90,
            },
            0,
            {
                    { 0, 0 },
                    { -100, 0 },
                    { 0, -100 },
                    90,
                    180,
                    270,
                    100,
                    { { -100, -100 }, { 100, 100 } },
            },
    },
    {
            "C(100,200)  0 - 30 degree",
            {
                    { 100, 200 },
                    { 300, 200 },
                    -30,
            },
            0,
            {
                    { 100, 200 },
                    { 300, 200 },
                    { 273, 100 }, // 200 * sin(30) = 100, 200* cos(30) = 173
                    -30,
                    0,
                    330,
                    200,
                    { { 100, 100 }, { 200, 100 } },
            },
    },
    {
            // This is a "fan shape" which includes the top quadrant point,
            // so it exercises the bounding box code (centre and end points
            // do not contain the top quadrant)
            "C(0,0) 30 + 120 degree",
            {
                    { 0, 0 },
                    { 17320, 10000 },
                    120,
            },
            0,
            {
                    { 0, 0 },
                    { 17320, 10000 },
                    { -17320, 10000 }, // 200 * sin(30) = 100, 200* cos(30) = 173
                    120,
                    30,
                    150,
                    20000,
                    // bbox defined by: centre, top quadrant point, two endpoints
                    { { -17320, 0 }, { 17320 * 2, 20000 } },
            },
    },
    {
            // An arc that covers three quadrant points (L/R, bottom)
            "C(0,0) 150 + 240 degree",
            {
                    { 0, 0 },
                    { -17320, 10000 },
                    240,
            },
            0,
            {
                    { 0, 0 },
                    { -17320, 10000 },
                    { 17320, 10000 },
                    240,
                    150,
                    30,
                    20000,
                    // bbox defined by: L/R quads, bottom quad and start/end
                    { { -20000, -20000 }, { 40000, 30000 } },
            },
    },
    {
            // Same as above but reverse direction
            "C(0,0) 30 - 300 degree",
            {
                    { 0, 0 },
                    { 17320, 10000 },
                    -240,
            },
            0,
            {
                    { 0, 0 },
                    { 17320, 10000 },
                    { -17320, 10000 },
                    -240,
                    30,
                    150,
                    20000,
                    // bbox defined by: L/R quads, bottom quad and start/end
                    { { -20000, -20000 }, { 40000, 30000 } },
            },
    },
};

BOOST_AUTO_TEST_CASE( BasicCPAGeom )
{
    for( const auto& c : arc_cases )
    {
        BOOST_TEST_CONTEXT( c.m_ctx_name )
        {

            const auto this_arc = SHAPE_ARC{ c.m_geom.m_center_point, c.m_geom.m_start_point,
                c.m_geom.m_center_angle, c.m_width };

            CheckArc( this_arc, c.m_properties );
        }
    }
}


struct ARC_TO_POLYLINE_CASE
{
    std::string         m_ctx_name;
    ARC_CENTRE_PT_ANGLE m_geom;
};


/**
 * Predicate for checking a polyline has all the points on (near) a circle of
 * given centre and radius
 * @param  aPolyline the polyline to check
 * @param  aCentre   the circle centre
 * @param  aRad      the circle radius
 * @param  aTolEnds  the tolerance for the endpoint-centre distance
 * @return           true if predicate met
 */
bool ArePolylinePointsNearCircle(
        const SHAPE_LINE_CHAIN& aPolyline, const VECTOR2I& aCentre, int aRad, int aTolEnds )
{
    std::vector<VECTOR2I> points;

    for( int i = 0; i < aPolyline.PointCount(); ++i )
    {
        points.push_back( aPolyline.CPoint( i ) );
    }

    return GEOM_TEST::ArePointsNearCircle( points, aCentre, aRad, aTolEnds );
}


BOOST_AUTO_TEST_CASE( ArcToPolyline )
{
    const std::vector<ARC_TO_POLYLINE_CASE> cases = {
        {
            "Zero rad",
            {
                { 0, 0 },
                { 0, 0 },
                180,
            },
        },
        {
            "Semicircle",
            {
                { 0, 0 },
                { -10, 0 },
                180,
            },
        },
        {
            "Larger semicircle",
            {
                { 0, 0 },
                { -1000, 0 },
                180,
            },
        },
    };

    const int    width = 0;
    const double accuracy = 1.0;

    for( const auto& c : cases )
    {
        BOOST_TEST_CONTEXT( c.m_ctx_name )
        {
            const SHAPE_ARC this_arc{ c.m_geom.m_center_point, c.m_geom.m_start_point,
                c.m_geom.m_center_angle, width };

            const SHAPE_LINE_CHAIN chain = this_arc.ConvertToPolyline( accuracy );

            BOOST_TEST_MESSAGE( "Polyline has " << chain.PointCount() << " points" );

            BOOST_CHECK_EQUAL( chain.CPoint( 0 ), c.m_geom.m_start_point );

            const int radius = ( c.m_geom.m_center_point - c.m_geom.m_start_point ).EuclideanNorm();
            const int tol = 2;

            BOOST_CHECK_PREDICATE( ArePolylinePointsNearCircle,
                    ( chain )( c.m_geom.m_center_point )( radius )( tol ) );

            // TODO: check midpoints are near circle too
        }
    }
}


BOOST_AUTO_TEST_SUITE_END()
