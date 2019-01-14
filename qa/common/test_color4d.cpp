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

#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>

#include <unit_test_utils/numeric.h>
#include <unit_test_utils/unit_test_utils.h>

#include <gal/color4d.h>

#ifdef WX_COMPATIBILITY
#include <wx/colour.h>
#endif

// All these tests are of a class in KIGFX
using namespace KIGFX;


/**
 * Checks if a COLOR4D is close enough to another
 */
bool pred_colour_is_near( const COLOR4D& aCol, const COLOR4D aOther, double aTol )
{
    return KI_TEST::IsWithin<double>( aCol.r, aOther.r, aTol )
           && KI_TEST::IsWithin<double>( aCol.g, aOther.g, aTol )
           && KI_TEST::IsWithin<double>( aCol.b, aOther.b, aTol )
           && KI_TEST::IsWithin<double>( aCol.a, aOther.a, aTol );
}

/**
 * Checks if a COLOR4D is close enough to a given RGB char value
 */
bool pred_colour_is_near_hex(
        const COLOR4D& aCol, unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
    const double tol = 0.5 / 255.0; // One bit of quantised error

    return KI_TEST::IsWithin<double>( aCol.r, r / 255.0, tol )
           && KI_TEST::IsWithin<double>( aCol.g, g / 255.0, tol )
           && KI_TEST::IsWithin<double>( aCol.b, b / 255.0, tol )
           && KI_TEST::IsWithin<double>( aCol.a, a / 255.0, tol );
}


/**
 * Declares a struct as the Boost test fixture.
 */
BOOST_AUTO_TEST_SUITE( Color4D )


/**
 * Check basic setting and getting of values
 */
BOOST_AUTO_TEST_CASE( BasicOps )
{
    const auto c = COLOR4D{ 0.4, 0.5, 0.6, 0.7 };

    BOOST_CHECK_EQUAL( c.r, 0.4 );
    BOOST_CHECK_EQUAL( c.g, 0.5 );
    BOOST_CHECK_EQUAL( c.b, 0.6 );
    BOOST_CHECK_EQUAL( c.a, 0.7 );

    const auto copied = c;

    // Test equality
    BOOST_CHECK_EQUAL( c, copied );

    const auto c2 = COLOR4D{ 0.1, 0.2, 0.3, 0.4 };

    // Test inequality
    BOOST_CHECK_NE( c, c2 );
}


/**
 * Test case data for a test that takes a colour and a scalar factor
 * and returns a result.
 */
struct COLOR_SCALAR_CASE
{
    COLOR4D start;
    double  factor;
    COLOR4D expected;
};


/**
 * Check inversion
 */
BOOST_AUTO_TEST_CASE( Invert )
{
    // Inverts RGB, A is the same
    static const std::vector<COLOR_SCALAR_CASE> cases = {
        { { 0.0, 0.25, 1.0, 1.0 }, 0.0, { 1.0, 0.75, 0.0, 1.0 } },
    };

    for( const auto& c : cases )
    {
        auto col = c.start;

        const auto inverted = col.Inverted();
        BOOST_CHECK_EQUAL( inverted, c.expected );

        // Test in-place function
        col.Invert();
        BOOST_CHECK_EQUAL( col, c.expected );
    }
}


/**
 * Check inversion
 */
BOOST_AUTO_TEST_CASE( Brighten )
{
    static const std::vector<COLOR_SCALAR_CASE> cases = {
        { { 0.0, 0.0, 0.0, 1.0 }, 0.5, { 0.5, 0.5, 0.5, 1.0 } },
        { { 0.0, 0.5, 1.0, 1.0 }, 0.5, { 0.5, 0.75, 1.0, 1.0 } },
    };

    for( const auto& c : cases )
    {
        auto col = c.start;

        const auto brightened = col.Brightened( c.factor );
        BOOST_CHECK_EQUAL( brightened, c.expected );

        // Test in-place function
        col.Brighten( c.factor );
        BOOST_CHECK_EQUAL( col, c.expected );
    }
}


/**
 * Check darken
 */
BOOST_AUTO_TEST_CASE( Darken )
{
    static const std::vector<COLOR_SCALAR_CASE> cases = {
        { { 0.0, 0.0, 0.0, 1.0 }, 0.5, { 0.0, 0.0, 0.0, 1.0 } },
        { { 1.0, 1.0, 1.0, 1.0 }, 0.5, { 0.5, 0.5, 0.5, 1.0 } },
    };

    for( const auto& c : cases )
    {
        auto col = c.start;

        const auto brightened = col.Darkened( c.factor );
        BOOST_CHECK_EQUAL( brightened, c.expected );

        // Test in-place function
        col.Darken( c.factor );
        BOOST_CHECK_EQUAL( col, c.expected );
    }
}


/**
 * Check alpha setting
 */
BOOST_AUTO_TEST_CASE( WithAlpha )
{
    static const std::vector<COLOR_SCALAR_CASE> cases = {
        { { 0.0, 0.0, 0.0, 1.0 }, 0.5, { 0.0, 0.0, 0.0, 0.5 } },
        { { 0.0, 0.5, 1.0, 1.0 }, 0.5, { 0.0, 0.5, 1.0, 0.5 } },
    };

    for( const auto& c : cases )
    {
        auto col = c.start;

        const auto with_alpha = col.WithAlpha( c.factor );
        BOOST_CHECK_EQUAL( with_alpha, c.expected );
    }

    // Note: If COLOR4D::WithAlpha raised an exception, we could check
    // the bounds-checking with BOOST_REQUIRE_THROW,
    // but it assert()s, so we can't.
}

struct FROM_HSV_TO_HEX_CASE
{
    double        h;
    double        s;
    double        v;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};


/**
 * Check FromHSV
 */
BOOST_AUTO_TEST_CASE( FromHsv )
{
    static const std::vector<FROM_HSV_TO_HEX_CASE> cases = {
        { 90.0, 0.5, 0.5, 96, 128, 64 },
    };

    for( const auto& c : cases )
    {
        auto col = COLOR4D{};
        col.FromHSV( c.h, c.s, c.v );
        const unsigned char alpha = 0xFF;

        BOOST_CHECK_PREDICATE( pred_colour_is_near_hex, ( col )( c.r )( c.g )( c.b )( alpha ) );
    }
}


#ifdef WX_COMPATIBILITY

struct WX_CONV_CASE
{
    wxColour wx;
    COLOR4D  c4d;
};


static std::vector<WX_CONV_CASE> wx_conv_cases = {
    { { 0x00, 0x00, 0x00, 0x00 }, { 0.0, 0.0, 0.0, 0.0 } },
    { { 0x66, 0x80, 0x99, 0xB3 }, { 0.4, 0.5, 0.6, 0.7 } },
    { { 0xFF, 0xFF, 0xFF, 0xFF }, { 1.0, 1.0, 1.0, 1.0 } },
    { { 0xFF, 0x00, 0x00, 0xFF }, { 0.999, 0.001, 0.0, 1.0 } },
};


/**
 * Check conversion to WxColour
 */
BOOST_AUTO_TEST_CASE( ToWx )
{
    for( const auto& c : wx_conv_cases )
    {
        wxColour wx_col = c.c4d.ToColour();

        // A hack, but avoids having to define a custom operator<<
        BOOST_CHECK_EQUAL( wx_col.Red(), c.wx.Red() );
        BOOST_CHECK_EQUAL( wx_col.Green(), c.wx.Green() );
        BOOST_CHECK_EQUAL( wx_col.Blue(), c.wx.Blue() );
        BOOST_CHECK_EQUAL( wx_col.Alpha(), c.wx.Alpha() );
    }
}


/**
 * Check conversion from WxColour
 */
BOOST_AUTO_TEST_CASE( FromWx )
{
    const double tol = 0.5 / 255.0; // One bit of quantised error

    for( const auto& c : wx_conv_cases )
    {
        const auto col = COLOR4D{ c.wx };

        BOOST_CHECK_PREDICATE( pred_colour_is_near, ( col )( c.c4d )( tol ) );
    }
}

#endif // WX_COMPATIBILITY

BOOST_AUTO_TEST_SUITE_END()
