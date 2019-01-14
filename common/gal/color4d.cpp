/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright 2012 Torsten Hueter, torstenhtr <at> gmx.de
 * Copyright 2017 Kicad Developers, see AUTHORS.txt for contributors.
 *
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

#include <map>

#include <gal/color4d.h>

using namespace KIGFX;

COLOR4D::COLOR4D( EDA_COLOR_T aColor )
{
    if( aColor <= UNSPECIFIED_COLOR || aColor >= NBCOLORS )
    {
        *this = COLOR4D::UNSPECIFIED;
        return;
    }

    r = g_ColorRefs[aColor].m_Red / 255.0;
    g = g_ColorRefs[aColor].m_Green / 255.0;
    b = g_ColorRefs[aColor].m_Blue / 255.0;
    a = 1.0;
}


#ifdef WX_COMPATIBILITY
    COLOR4D::COLOR4D( const wxColour& aColor )
    {
        r = aColor.Red() / 255.0;
        g = aColor.Green() / 255.0;
        b = aColor.Blue() / 255.0;
        a = aColor.Alpha() / 255.0;
    }


    bool COLOR4D::SetFromWxString( const wxString& aColorString )
    {
        wxColour c;

        if( c.Set( aColorString ) )
        {
            r = c.Red() / 255.0;
            g = c.Green() / 255.0;
            b = c.Blue() / 255.0;
            a = c.Alpha() / 255.0;

            return true;
        }

        return false;
    }


    wxString COLOR4D::ToWxString( long flags ) const
    {
        wxColour c = ToColour();
        return c.GetAsString( flags );
    }


    wxColour COLOR4D::ToColour() const
    {
        using CHAN_T = wxColourBase::ChannelType;

        const wxColour colour(
            static_cast<CHAN_T>( r * 255 + 0.5 ),
            static_cast<CHAN_T>( g * 255 + 0.5 ),
            static_cast<CHAN_T>( b * 255 + 0.5 ),
            static_cast<CHAN_T>( a * 255 + 0.5 )
        );
        return colour;
    }


    COLOR4D COLOR4D::LegacyMix( COLOR4D aColor ) const
    {
        COLOR4D candidate;

        // Blend the two colors (i.e. OR the RGB values)
        candidate.r = ( (unsigned)( 255.0 * r ) | (unsigned)( 255.0 * aColor.r ) ) / 255.0,
        candidate.g = ( (unsigned)( 255.0 * g ) | (unsigned)( 255.0 * aColor.g ) ) / 255.0,
        candidate.b = ( (unsigned)( 255.0 * b ) | (unsigned)( 255.0 * aColor.b ) ) / 255.0,

        // the alpha channel can be reinitialized
        // but what is the best value?
        candidate.a = ( aColor.a + a ) / 2;

        return candidate;
    }


    COLOR4D& COLOR4D::SetToLegacyHighlightColor()
    {
        EDA_COLOR_T legacyColor = GetNearestLegacyColor( *this );
        EDA_COLOR_T highlightColor = g_ColorRefs[legacyColor].m_LightColor;

        // The alpha channel is not modified. Only R, G, B values are set,
        // because legacy color does not know the alpha chanel.

        r = g_ColorRefs[highlightColor].m_Red / 255.0;
        g = g_ColorRefs[highlightColor].m_Green / 255.0;
        b = g_ColorRefs[highlightColor].m_Blue / 255.0;

        return *this;
    }


    COLOR4D& COLOR4D::SetToNearestLegacyColor()
    {
        EDA_COLOR_T legacyColor = GetNearestLegacyColor( *this );

        // The alpha channel is not modified. Only R, G, B values are set,
        // because legacy color does not know the alpha chanel.

        r = g_ColorRefs[legacyColor].m_Red / 255.0;
        g = g_ColorRefs[legacyColor].m_Green / 255.0;
        b = g_ColorRefs[legacyColor].m_Blue / 255.0;

        return *this;
    }


    unsigned int COLOR4D::ToU32() const
    {
        return ToColour().GetRGB();
    }


    void COLOR4D::FromU32( unsigned int aPackedColor )
    {
        wxColour c;
        c.SetRGB( aPackedColor );
        r = c.Red() / 255.0;
        g = c.Green() / 255.0;
        b = c.Blue() / 255.0;
        a = c.Alpha() / 255.0;
    }


    EDA_COLOR_T COLOR4D::GetNearestLegacyColor( const COLOR4D &aColor )
    {
        // Cache layer implemented here, because all callers are using wxColour
        static std::map< unsigned int, unsigned int > nearestCache;
        static double hues[NBCOLORS];
        static double values[NBCOLORS];

        unsigned int colorInt = aColor.ToU32();

        auto search = nearestCache.find( colorInt );

        if( search != nearestCache.end() )
            return static_cast<EDA_COLOR_T>( search->second );

        // First use ColorFindNearest to check for exact matches
        EDA_COLOR_T nearest = ColorFindNearest( aColor.r * 255.0, aColor.g * 255.0, aColor.b * 255.0 );

        if( COLOR4D( nearest ) == aColor )
        {
            nearestCache.insert( std::pair< unsigned int, unsigned int >(
                                 colorInt, static_cast<unsigned int>( nearest ) ) );
            return nearest;
        }

        // If not, use hue and value to match.
        // Hue will be NAN for grayscale colors.
        // The legacy color palette is a grid across hue and value.
        // We can exploit that to find a good match -- hue is most apparent to the user.
        // So, first we determine the closest hue match, and then the closest value from that
        // "grid row" in the legacy palette.

        double h, s, v;
        aColor.ToHSV( h, s, v );

        double minDist = 360.0;
        double legacyHue = 0.0;

        if( std::isnan( h ) )
        {
            legacyHue = NAN;
        }
        else
        {
            for( EDA_COLOR_T candidate = ::BLACK;
                    candidate < NBCOLORS; candidate = NextColor( candidate ) )
            {
                double ch;

                if( hues[candidate] == 0.0 && values[candidate] == 0.0 )
                {
                    COLOR4D candidate4d( candidate );
                    double cs, cv;

                    candidate4d.ToHSV( ch, cs, cv );

                    values[candidate] = cv;
                    // Set the hue to non-zero for black so that we won't do this more than once
                    hues[candidate] = ( cv == 0.0 ) ? 1.0 : ch;
                }
                else
                {
                    ch = hues[candidate];
                }

                if( fabs( ch - h ) < minDist )
                {
                    minDist = fabs( ch - h );
                    legacyHue = ch;
                }
            }
        }

        // Now we have the desired hue; let's find the nearest value
        minDist = 1.0;
        for( EDA_COLOR_T candidate = ::BLACK;
                candidate < NBCOLORS; candidate = NextColor( candidate ) )
        {
            // If the target hue is NAN, we didn't extract the value for any colors above
            if( std::isnan( legacyHue ) )
            {
                double ch, cs, cv;
                COLOR4D candidate4d( candidate );
                candidate4d.ToHSV( ch, cs, cv );
                values[candidate] = cv;
                hues[candidate] = ( cv == 0.0 ) ? 1.0 : ch;
            }

            if( ( std::isnan( legacyHue ) != std::isnan( hues[candidate] ) ) || hues[candidate] != legacyHue )
                continue;

            if( fabs( values[candidate] - v ) < minDist )
            {
                minDist = fabs( values[candidate] - v );
                nearest = candidate;
            }
        }

        nearestCache.insert( std::pair< unsigned int, unsigned int >(
                             colorInt, static_cast<unsigned int>( nearest ) ) );

        return nearest;
    }
#endif

namespace KIGFX {

const bool operator==( const COLOR4D& lhs, const COLOR4D& rhs )
{
    return lhs.a == rhs.a && lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}


const bool operator!=( const COLOR4D& lhs, const COLOR4D& rhs )
{
    return !( lhs == rhs );
}

std::ostream &operator<<( std::ostream &aStream, COLOR4D const &aColor )
{
    return aStream << aColor.ToWxString( wxC2S_CSS_SYNTAX );
}

}


void COLOR4D::ToHSV( double& aOutHue, double& aOutSaturation, double& aOutValue, bool aAlwaysDefineHue ) const
{
    double min, max, delta;

    min = r < g ? r : g;
    min = min < b ? min : b;

    max = r > g ? r : g;
    max = max > b ? max : b;

    aOutValue = max;                    // value
    delta = max - min;

    if( max > 0.0 )
    {
        aOutSaturation = ( delta / max );
    }
    else    // for black color (r = g = b = 0 )  saturation is set to 0.
    {
        aOutSaturation = 0.0;
        aOutHue = aAlwaysDefineHue ? 0.0 : NAN;
        return;
    }

    /* Hue in degrees (0...360) is coded according to this table
     * 0 or 360 : red
     * 60 : yellow
     * 120 : green
     * 180 : cyan
     * 240 : blue
     * 300 : magenta
    */
    if( delta != 0.0 )
    {
        if( r >= max )
            aOutHue = ( g - b ) / delta;          // between yellow & magenta
        else if( g >= max )
            aOutHue = 2.0 + ( b - r ) / delta;    // between cyan & yellow
        else
            aOutHue = 4.0 + ( r - g ) / delta;    // between magenta & cyan

        aOutHue *= 60.0;                          // degrees

        if( aOutHue < 0.0 )
            aOutHue += 360.0;
    }
    else   // delta = 0 means r = g = b. hue is set to 0.0
    {
        aOutHue = aAlwaysDefineHue ? 0.0 : NAN;
    }
}


void COLOR4D::FromHSV( double aInH, double aInS, double aInV )
{
    if( aInS <= 0.0 )
    {
        r = aInV;
        g = aInV;
        b = aInV;
        return;
    }

    double hh = aInH;

    while( hh >= 360.0 )
        hh -= 360.0;

    /* Hue in degrees (0...360) is coded according to this table
     * 0 or 360 : red
     * 60 : yellow
     * 120 : green
     * 180 : cyan
     * 240 : blue
     * 300 : magenta
    */
    hh /= 60.0;

    int i = (int) hh;
    double ff = hh - i;

    double p = aInV * ( 1.0 - aInS );
    double q = aInV * ( 1.0 - ( aInS * ff ) );
    double t = aInV * ( 1.0 - ( aInS * ( 1.0 - ff ) ) );

    switch( i )
    {
    case 0:
        r = aInV;
        g = t;
        b = p;
        break;

    case 1:
        r = q;
        g = aInV;
        b = p;
        break;

    case 2:
        r = p;
        g = aInV;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = aInV;
        break;

    case 4:
        r = t;
        g = p;
        b = aInV;
        break;

    case 5:
    default:
        r = aInV;
        g = p;
        b = q;
        break;
    }
}


COLOR4D& COLOR4D::Saturate( double aFactor )
{
    // One can saturate a color only when r, v, b are not equal
    if( r == g && r == b )
        return *this;

    double h, s, v;

    ToHSV( h, s, v, true );
    FromHSV( h, aFactor, 1.0 );

    return *this;
}

const COLOR4D COLOR4D::UNSPECIFIED( 0, 0, 0, 0 );
const COLOR4D COLOR4D::WHITE( 1, 1, 1, 1 );
const COLOR4D COLOR4D::BLACK( 0, 0, 0, 1 );
