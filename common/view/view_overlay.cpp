/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2013-2017 CERN
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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

#include <view/view.h>
#include <view/view_item.h>
#include <view/view_overlay.h>
#include <gal/graphics_abstraction_layer.h>
#include <painter.h>

#include <layers_id_colors_and_visibility.h>
#include <geometry/seg.h>

namespace KIGFX {

struct VIEW_OVERLAY::COMMAND
{
    virtual ~COMMAND() {};
    virtual void Execute( VIEW* aView ) const = 0;
};


struct VIEW_OVERLAY::COMMAND_LINE : public VIEW_OVERLAY::COMMAND
{
    COMMAND_LINE( const VECTOR2D& aP0, const VECTOR2D& aP1 ) :
        m_p0( aP0 ),
        m_p1( aP1 ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawLine( m_p0, m_p1 );
    }

    VECTOR2D m_p0;
    VECTOR2D m_p1;
};


struct VIEW_OVERLAY::COMMAND_RECTANGLE : public VIEW_OVERLAY::COMMAND
{
    COMMAND_RECTANGLE( const VECTOR2D& aP0, const VECTOR2D& aP1 ) :
        m_p0( aP0 ), m_p1( aP1 )
    {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawRectangle( m_p0, m_p1 );
    }

    VECTOR2D m_p0;
    VECTOR2D m_p1;
};


struct VIEW_OVERLAY::COMMAND_CIRCLE : public VIEW_OVERLAY::COMMAND
{
    COMMAND_CIRCLE( const VECTOR2D& aCenter, double aRadius ) :
        m_center(aCenter),
        m_radius(aRadius) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawCircle( m_center, m_radius );
    }

    VECTOR2D m_center;
    double m_radius;
};


struct VIEW_OVERLAY::COMMAND_ARC : public VIEW_OVERLAY::COMMAND
{
    COMMAND_ARC( const VECTOR2D& aCenter, double aRadius, double aStartAngle, double aEndAngle ) :
        m_center( aCenter ),
        m_radius( aRadius ),
        m_startAngle( aStartAngle ),
        m_endAngle( aEndAngle )
    { }

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawArc( m_center, m_radius, m_startAngle, m_endAngle );
    }

    VECTOR2D m_center;
    double m_radius;
    double m_startAngle;
    double m_endAngle;
};


struct VIEW_OVERLAY::COMMAND_POLYLINE : public VIEW_OVERLAY::COMMAND
{
    COMMAND_POLYLINE( const std::deque<VECTOR2D>& aPointList ) :
       m_pointList( aPointList ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawPolyline( m_pointList );
    }

    std::deque<VECTOR2D> m_pointList;
};


struct VIEW_OVERLAY::COMMAND_POLY_POLYLINE : public VIEW_OVERLAY::COMMAND
{
    COMMAND_POLY_POLYLINE( const SHAPE_LINE_CHAIN& aLineChain ) :
       m_polyLine( aLineChain ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawPolyline( m_polyLine );
    }

    SHAPE_LINE_CHAIN m_polyLine;
};


struct VIEW_OVERLAY::COMMAND_POINT_POLYLINE : public VIEW_OVERLAY::COMMAND
{
    COMMAND_POINT_POLYLINE( const VECTOR2D aPointList[], int aListSize )
    {
        m_pointList.reserve( aListSize );

        for( int ii = 0; ii < aListSize; ii++ )
            m_pointList.push_back( aPointList[ii] );
    }

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawPolyline( &m_pointList[0], (int)m_pointList.size() );
    }

    std::vector<VECTOR2D> m_pointList;
};


struct VIEW_OVERLAY::COMMAND_POLYGON : public VIEW_OVERLAY::COMMAND
{
    COMMAND_POLYGON( const std::deque<VECTOR2D>& aPointList ) :
       m_pointList( aPointList ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawPolygon( m_pointList );
    }

    std::deque<VECTOR2D> m_pointList;
};


struct VIEW_OVERLAY::COMMAND_POLY_POLYGON : public VIEW_OVERLAY::COMMAND
{
    COMMAND_POLY_POLYGON( const SHAPE_POLY_SET& aPolySet ) :
       m_polySet( aPolySet ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawPolygon( m_polySet );
    }

    SHAPE_POLY_SET m_polySet;
};


struct VIEW_OVERLAY::COMMAND_POINT_POLYGON : public VIEW_OVERLAY::COMMAND
{
    COMMAND_POINT_POLYGON( const VECTOR2D aPointList[], int aListSize )
    {
        m_pointList.reserve( aListSize );

        for( int ii = 0; ii < aListSize; ii++ )
            m_pointList.push_back( aPointList[ii] );
    }

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->DrawPolygon( &m_pointList[0], (int)m_pointList.size() );
    }

    std::vector<VECTOR2D> m_pointList;
};


struct VIEW_OVERLAY::COMMAND_SET_STROKE : public VIEW_OVERLAY::COMMAND
{
    COMMAND_SET_STROKE( bool aIsStroke ) :
        m_isStroke( aIsStroke ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->SetIsStroke( m_isStroke );
    }

    bool m_isStroke;
};


struct VIEW_OVERLAY::COMMAND_SET_FILL : public VIEW_OVERLAY::COMMAND
{
    COMMAND_SET_FILL( bool aIsFill ) :
        m_isFill(  aIsFill ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->SetIsFill( m_isFill );
    }

    bool m_isFill;
};


struct VIEW_OVERLAY::COMMAND_SET_COLOR : public VIEW_OVERLAY::COMMAND
{
    COMMAND_SET_COLOR( bool aIsStroke, const COLOR4D& aColor ) :
        m_isStroke( aIsStroke ),
        m_color( aColor ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        if( m_isStroke )
            aView->GetGAL()->SetStrokeColor( m_color );
        else
            aView->GetGAL()->SetFillColor( m_color );
    }

    bool m_isStroke;
    COLOR4D m_color;
};


struct VIEW_OVERLAY::COMMAND_SET_WIDTH : public VIEW_OVERLAY::COMMAND
{
    COMMAND_SET_WIDTH( double aWidth ) :
        m_width( aWidth ) {}

    virtual void Execute( VIEW* aView ) const override
    {
        aView->GetGAL()->SetLineWidth( m_width );
    }

    double m_width;
};


VIEW_OVERLAY::VIEW_OVERLAY()
{
}


VIEW_OVERLAY::~VIEW_OVERLAY()
{
    releaseCommands();
}


void VIEW_OVERLAY::releaseCommands()
{
    for( auto cmd : m_commands )
        delete cmd;

    m_commands.clear();
}


void VIEW_OVERLAY::Clear()
{
    releaseCommands();
}


const BOX2I VIEW_OVERLAY::ViewBBox() const
{
    BOX2I maxBox;

    maxBox.SetMaximum();
    return maxBox;
}


void VIEW_OVERLAY::ViewDraw( int aLayer, VIEW* aView ) const
{
    for( const auto& cmd : m_commands )
        cmd->Execute( aView );
}


void VIEW_OVERLAY::ViewGetLayers( int aLayers[], int& aCount ) const
{
    aLayers[0] = LAYER_GP_OVERLAY;
    aCount = 1;
}


void VIEW_OVERLAY::Line( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint )
{
    m_commands.push_back( new COMMAND_LINE( aStartPoint, aEndPoint ) );
}


void VIEW_OVERLAY::Line( const SEG& aSeg )
{
    Line( aSeg.A, aSeg.B );
}


void VIEW_OVERLAY::Segment( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, double aWidth )
{
    SetLineWidth( aWidth );
    Line( aStartPoint, aEndPoint );
}


void VIEW_OVERLAY::Polyline( std::deque<VECTOR2D>& aPointList )
{
}


void VIEW_OVERLAY::Polyline( const VECTOR2D aPointList[], int aListSize )
{
}


void VIEW_OVERLAY::Polyline( const SHAPE_LINE_CHAIN& aLineChain )
{
}


void VIEW_OVERLAY::Polygon( const SHAPE_POLY_SET& aPolySet )
{
    m_commands.push_back( new COMMAND_POLY_POLYGON( aPolySet ) );
}


void VIEW_OVERLAY::Polygon( const std::deque<VECTOR2D>& aPointList )
{
    m_commands.push_back( new COMMAND_POLYGON( aPointList ) );
}


void VIEW_OVERLAY::Polygon( const VECTOR2D aPointList[], int aListSize )
{
    m_commands.push_back( new COMMAND_POINT_POLYGON( aPointList, aListSize ) );
}


void VIEW_OVERLAY::Circle( const VECTOR2D& aCenterPoint, double aRadius )
{
    m_commands.push_back( new COMMAND_CIRCLE( aCenterPoint, aRadius ) );
}


void VIEW_OVERLAY::Arc( const VECTOR2D& aCenterPoint,
                        double aRadius, double aStartAngle, double aEndAngle )
{
    m_commands.push_back( new COMMAND_ARC( aCenterPoint, aRadius, aStartAngle, aEndAngle ) );
}


void VIEW_OVERLAY::Rectangle( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint )
{
    m_commands.push_back( new COMMAND_RECTANGLE( aStartPoint, aEndPoint ) );
}


void VIEW_OVERLAY::SetIsFill( bool aIsFillEnabled )
{
    m_commands.push_back( new COMMAND_SET_FILL( aIsFillEnabled ) );
}


void VIEW_OVERLAY::SetIsStroke( bool aIsStrokeEnabled )
{
    m_commands.push_back( new COMMAND_SET_STROKE( aIsStrokeEnabled ) );
}


void VIEW_OVERLAY::SetFillColor( const COLOR4D& aColor )
{
    m_commands.push_back( new COMMAND_SET_COLOR( false, aColor ) );
}


void VIEW_OVERLAY::SetStrokeColor( const COLOR4D& aColor )
{
    m_commands.push_back( new COMMAND_SET_COLOR( true, aColor ) );
}

void VIEW_OVERLAY::SetLineWidth( double aLineWidth )
{
    m_commands.push_back( new COMMAND_SET_WIDTH( aLineWidth ) );
}

} // namespace
