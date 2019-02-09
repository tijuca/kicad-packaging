/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 KiCad Developers, see CHANGELOG.TXT for contributors.
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

/**
 * @file test_array_options.cpp
 * Test suite for #ARRAY_OPTIONS
 */

#include <unit_test_utils/geometry.h>
#include <unit_test_utils/unit_test_utils.h>

#include <base_units.h>
#include <trigo.h>

#include <array_options.h>

/**
 * Define a stream function for logging this type.
 *
 * TODO: convert to boost_test_print_type when Boost minver > 1.64
 */
std::ostream& operator<<( std::ostream& os, const ARRAY_OPTIONS::TRANSFORM& aObj )
{
    os << "TRANSFORM[ " << aObj.m_offset << " r " << aObj.m_rotation << "deg"
       << " ]";
    return os;
}


/**
 * Predicate to see if a #ARRAY_OPTIONS::TRANSFORM is equal or nearly equal
 */
bool TransformIsClose( const ARRAY_OPTIONS::TRANSFORM& aL, const ARRAY_OPTIONS::TRANSFORM& aR )
{
    return KI_TEST::IsVecWithinTol<VECTOR2I>( aL.m_offset, aR.m_offset, 1 )
           && KI_TEST::IsWithin<double>( aL.m_rotation, aR.m_rotation, 0.001 );
}


/**
 * Generate all array transforms for an array descriptor and compare
 * against a list of expected transforms
 * @param aOpts the array descriptor
 * @param aPos  the position of the reference item
 * @param aExp  expected transform list
 */
void CheckArrayTransforms( const ARRAY_OPTIONS& aOpts, const VECTOR2I& aPos,
        const std::vector<ARRAY_OPTIONS::TRANSFORM>& aExp )
{
    std::vector<ARRAY_OPTIONS::TRANSFORM> transforms;

    for( int i = 0; i < aOpts.GetArraySize(); ++i )
    {
        transforms.push_back( aOpts.GetTransform( i, aPos ) );
    }

    BOOST_CHECK_EQUAL( transforms.size(), aExp.size() );

    for( unsigned i = 0; i < std::min( transforms.size(), aExp.size() ); ++i )
    {
        BOOST_TEST_CONTEXT( "Index " << i )
        {
            BOOST_CHECK_PREDICATE( TransformIsClose, ( transforms[i] )( aExp[i] ) );
        }
    }
}


/**
 * Declare the test suite
 */
BOOST_AUTO_TEST_SUITE( ArrayOptions )


struct GRID_ARRAY_GEOM_PARAMS
{
    int      m_nx;
    int      m_ny;
    VECTOR2I m_delta;
    VECTOR2I m_offset;
    int      m_stagger;
    bool     m_stagger_by_row;
    bool     m_alternate_numbers;
    bool     m_h_then_v;
};

struct GRID_ARRAY_TEST_CASE
{
    std::string                           m_case_name;
    GRID_ARRAY_GEOM_PARAMS                m_geom;
    VECTOR2I                              m_item_pos;
    std::vector<ARRAY_OPTIONS::TRANSFORM> m_exp_transforms;
};


// clang-format off
static const std::vector<GRID_ARRAY_TEST_CASE> grid_geom_cases = {
    {
        "2x3 rect grid",
        {
            2,
            3,
            { Millimeter2iu( 2 ), Millimeter2iu( 2 ) },
            { 0, 0 },
            1,
            true,
            false,
            true,
        },
        { 0, 0 },
        {
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 0 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 0 ), Millimeter2iu( 4 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 4 ) }, 0 },
        },
    },
    {
        "2x3 offset grid",
        {
            2,
            3,
            { Millimeter2iu( 2 ), Millimeter2iu( 2 ) },
            { Millimeter2iu( 0.1 ), Millimeter2iu( 0.2 ) },
            1,
            true,
            false,
            true,
        },
        { 0, 0 },
        {
            // add the offsets for each positions
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 0.2 ) }, 0 },
            { { Millimeter2iu( 0.1 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 2.1 ), Millimeter2iu( 2.2 ) }, 0 },
            { { Millimeter2iu( 0.2 ), Millimeter2iu( 4.0 ) }, 0 },
            { { Millimeter2iu( 2.2 ), Millimeter2iu( 4.2 ) }, 0 },
        },
    },
    {
        "2x3 stagger rows",
        {
            2,
            3,
            { Millimeter2iu( 3 ), Millimeter2iu( 2 ) },
            { 0, 0 },
            3,
            true,
            false,
            true,
        },
        { 0, 0 },
        {
            // add the offsets for each positions
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 3 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 1 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 4 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 4 ) }, 0 },
            { { Millimeter2iu( 5 ), Millimeter2iu( 4 ) }, 0 },
        },
    },
    {
        "2x3 stagger cols",
        {
            2,
            3,
            { Millimeter2iu( 3 ), Millimeter2iu( 2 ) },
            { 0, 0 },
            2,
            false,
            false,
            true,
        },
        { 0, 0 },
        {
            // add the offsets for each positions
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 3 ), Millimeter2iu( 1 ) }, 0 },
            { { Millimeter2iu( 0 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 3 ), Millimeter2iu( 3 ) }, 0 },
            { { Millimeter2iu( 0 ), Millimeter2iu( 4 ) }, 0 },
            { { Millimeter2iu( 3 ), Millimeter2iu( 5 ) }, 0 },
        },
    },
    {
        "2x3 rect alternate",
        {
            2,
            3,
            { Millimeter2iu( 2 ), Millimeter2iu( 2 ) },
            { 0, 0 },
            1,
            true,
            true,
            true,
        },
        { 0, 0 },
        {
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 0 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 0 ), Millimeter2iu( 4 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 4 ) }, 0 },
        },
    },
    {
        "2x3 rect v then h",
        {
            2,
            3,
            { Millimeter2iu( 2 ), Millimeter2iu( 2 ) },
            { 0, 0 },
            1,
            true,
            false,
            false,
        },
        { 0, 0 },
        {
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 0 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 0 ), Millimeter2iu( 4 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 0 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 2 ) }, 0 },
            { { Millimeter2iu( 2 ), Millimeter2iu( 4 ) }, 0 },
        },
    },
};
// clang-format on


/**
 * Test of grid array geometry
 */
BOOST_AUTO_TEST_CASE( GridGeometry )
{
    for( const auto& c : grid_geom_cases )
    {
        BOOST_TEST_CONTEXT( c.m_case_name )
        {
            ARRAY_GRID_OPTIONS grid_opts;

            grid_opts.m_nx = c.m_geom.m_nx;
            grid_opts.m_ny = c.m_geom.m_ny;
            grid_opts.m_delta = (wxPoint) c.m_geom.m_delta;
            grid_opts.m_offset = (wxPoint) c.m_geom.m_offset;
            grid_opts.m_stagger = c.m_geom.m_stagger;
            grid_opts.m_stagger_rows = c.m_geom.m_stagger_by_row;
            grid_opts.m_reverseNumberingAlternate = c.m_geom.m_alternate_numbers;
            grid_opts.m_horizontalThenVertical = c.m_geom.m_h_then_v;

            CheckArrayTransforms( grid_opts, c.m_item_pos, c.m_exp_transforms );
        }
    }
}


struct CIRC_ARRAY_GEOM_PARAMS
{
    int      n;
    double   angle_offset;
    VECTOR2I centre;
    bool     rotate;
};

struct CIRC_ARRAY_TEST_CASE
{
    std::string                           m_case_name;
    CIRC_ARRAY_GEOM_PARAMS                m_geom;
    VECTOR2I                              m_item_pos;
    std::vector<ARRAY_OPTIONS::TRANSFORM> m_exp_transforms;
};


// clang-format off
static const std::vector<CIRC_ARRAY_TEST_CASE> circ_geom_cases = {
    {
        "Quad, no rotate items",
        {
            4,
            0,
            { 0, 0 },
            false,
        },
        { Millimeter2iu( 10 ), 0 },
        {
            // diamond shape
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) } , 0 },
            { { Millimeter2iu( -10 ), Millimeter2iu( -10 ) } , 0 },
            { { Millimeter2iu( -20 ), Millimeter2iu( 0 ) } , 0 },
            { {Millimeter2iu( -10 ), Millimeter2iu( 10 ) } , 0 },
        },
    },
    {
        "Quad, rotate items",
        {
            4,
            0,
            { 0, 0 },
            true,
        },
        { Millimeter2iu( 10 ), 0 },
        {
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) } , 0 },
            { { Millimeter2iu( -10 ), Millimeter2iu( -10 ) } , 90 },
            { { Millimeter2iu( -20 ), Millimeter2iu( 0 ) } , 180 },
            { {Millimeter2iu( -10 ), Millimeter2iu( 10 ) } , 270 },
        },
    },
    {
        "Three pts, 90 deg angle",
        {
            3,
            45.0,
            { 0, 0 },
            true,
        },
        { Millimeter2iu( 10 ), 0 },
        {
            { { Millimeter2iu( 0 ), Millimeter2iu( 0 ) } , 0 },
            // 10 * [ 1-sin(45), sin(45) ]
            { { Millimeter2iu( -2.9289321881 ), Millimeter2iu( -7.0710678118 ) } , 45 },
            { { Millimeter2iu( -10 ), Millimeter2iu( -10 ) } , 90 },
        },
    },
};
// clang-format on

/**
 * Test of circular array geometry
 */
BOOST_AUTO_TEST_CASE( CircularGeometry )
{
    for( const auto& c : circ_geom_cases )
    {
        BOOST_TEST_CONTEXT( c.m_case_name )
        {
            ARRAY_CIRCULAR_OPTIONS grid_opts;

            grid_opts.m_nPts = c.m_geom.n;
            grid_opts.m_angle = 10 * c.m_geom.angle_offset;
            grid_opts.m_centre = c.m_geom.centre;
            grid_opts.m_rotateItems = c.m_geom.rotate;

            CheckArrayTransforms( grid_opts, c.m_item_pos, c.m_exp_transforms );
        }
    }
}

/**
 * Generate all array names and check against expected
 * @param aOpts the array descriptor
 * @param aExp  expected name list
 */
void CheckArrayNumbering( const ARRAY_OPTIONS& aOpts, const std::vector<std::string>& aExp )
{
    std::vector<std::string> names;

    for( int i = 0; i < aOpts.GetArraySize(); ++i )
    {
        names.push_back( aOpts.GetItemNumber( i ).ToStdString() );
    }

    BOOST_CHECK_EQUAL_COLLECTIONS( names.begin(), names.end(), aExp.begin(), aExp.end() );
}


struct GRID_ARRAY_NAMING_PARAMS
{
    ARRAY_OPTIONS::NUMBERING_TYPE_T m_pri_type;
    ARRAY_OPTIONS::NUMBERING_TYPE_T m_sec_type;
    std::string                     m_start_at_x;
    std::string                     m_start_at_y;
    bool                            m_2d_numbering;
    int                             m_nx;
    int                             m_ny;
};


struct GRID_ARRAY_NAMING_CASE
{
    std::string              m_case_name;
    GRID_ARRAY_NAMING_PARAMS m_prms;
    std::vector<std::string> m_exp_names;
};


// clang-format off
static const std::vector<GRID_ARRAY_NAMING_CASE> grid_name_cases = {
    {
        "Linear grid",
        {
            ARRAY_OPTIONS::NUMBERING_TYPE_T::NUMBERING_NUMERIC,
            ARRAY_OPTIONS::NUMBERING_TYPE_T::NUMBERING_NUMERIC, // doesn't matter here
            "1",
            "2",
            false,
            2,
            3,
        },
        { "1", "2", "3", "4", "5", "6" },
    },
    {
        // Tests a 2d grid, with different types and offsets (and alphabet wrap)
        "2D grid",
        {
            ARRAY_OPTIONS::NUMBERING_TYPE_T::NUMBERING_NUMERIC,
            ARRAY_OPTIONS::NUMBERING_TYPE_T::NUMBERING_ALPHA_FULL,
            "5",
            "Z",
            true,
            2,
            3,
        },
        { "5Z", "6Z", "5AA", "6AA", "5AB", "6AB" },
    },
};
// clang-format on


/**
 * Test of grid array geometry
 */
BOOST_AUTO_TEST_CASE( GridNaming )
{
    for( const auto& c : grid_name_cases )
    {
        BOOST_TEST_CONTEXT( c.m_case_name )
        {
            ARRAY_GRID_OPTIONS grid_opts;

            grid_opts.m_nx = c.m_prms.m_nx;
            grid_opts.m_ny = c.m_prms.m_ny;

            ARRAY_OPTIONS::GetNumberingOffset(
                    c.m_prms.m_start_at_x, c.m_prms.m_pri_type, grid_opts.m_numberingOffsetX );
            ARRAY_OPTIONS::GetNumberingOffset(
                    c.m_prms.m_start_at_y, c.m_prms.m_sec_type, grid_opts.m_numberingOffsetY );

            grid_opts.m_priAxisNumType = c.m_prms.m_pri_type;
            grid_opts.m_secAxisNumType = c.m_prms.m_sec_type;

            grid_opts.m_2dArrayNumbering = c.m_prms.m_2d_numbering;

            // other grid settings (geom) can be defaulted, as they don't affect  numbering

            CheckArrayNumbering( grid_opts, c.m_exp_names );
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()