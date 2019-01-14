/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 1992-2017 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file class_zone.cpp
 * @brief Implementation of class to handle copper zones.
 */

#include <fctsys.h>
#include <trigo.h>
#include <pcb_screen.h>
#include <class_drawpanel.h>
#include <kicad_string.h>
#include <richio.h>
#include <macros.h>
#include <pcb_base_frame.h>
#include <msgpanel.h>
#include <bitmaps.h>

#include <convert_to_biu.h>
#include <class_board.h>
#include <class_zone.h>

#include <pcbnew.h>
#include <zones.h>
#include <math_for_graphics.h>
#include <polygon_test_point_inside.h>


ZONE_CONTAINER::ZONE_CONTAINER( BOARD* aBoard ) :
    BOARD_CONNECTED_ITEM( aBoard, PCB_ZONE_AREA_T )
{
    m_CornerSelection = nullptr;                // no corner is selected
    m_IsFilled = false;                         // fill status : true when the zone is filled
    m_FillMode = ZFM_POLYGONS;
    m_hatchStyle = DIAGONAL_EDGE;
    m_hatchPitch = GetDefaultHatchPitch();
    m_hv45 = false;
    m_priority = 0;
    m_cornerSmoothingType = ZONE_SETTINGS::SMOOTHING_NONE;
    SetIsKeepout( false );
    SetDoNotAllowCopperPour( false );           // has meaning only if m_isKeepout == true
    SetDoNotAllowVias( true );                  // has meaning only if m_isKeepout == true
    SetDoNotAllowTracks( true );                // has meaning only if m_isKeepout == true
    m_cornerRadius = 0;
    SetLocalFlags( 0 );                         // flags tempoarry used in zone calculations
    m_Poly = new SHAPE_POLY_SET();              // Outlines
    aBoard->GetZoneSettings().ExportSetting( *this );
}


ZONE_CONTAINER::ZONE_CONTAINER( const ZONE_CONTAINER& aZone ) :
    BOARD_CONNECTED_ITEM( aZone )
{
    // Should the copy be on the same net?
    SetNetCode( aZone.GetNetCode() );
    m_Poly = new SHAPE_POLY_SET( *aZone.m_Poly );

    // For corner moving, corner index to drag, or nullptr if no selection
    m_CornerSelection = nullptr;
    m_IsFilled = aZone.m_IsFilled;
    m_ZoneClearance = aZone.m_ZoneClearance;     // clearance value
    m_ZoneMinThickness = aZone.m_ZoneMinThickness;
    m_FillMode = aZone.m_FillMode;               // Filling mode (segments/polygons)
    m_hv45 = aZone.m_hv45;
    m_priority = aZone.m_priority;
    m_ArcToSegmentsCount = aZone.m_ArcToSegmentsCount;
    m_PadConnection = aZone.m_PadConnection;
    m_ThermalReliefGap = aZone.m_ThermalReliefGap;
    m_ThermalReliefCopperBridge = aZone.m_ThermalReliefCopperBridge;
    m_FilledPolysList.Append( aZone.m_FilledPolysList );
    m_FillSegmList = aZone.m_FillSegmList;      // vector <> copy

    m_isKeepout = aZone.m_isKeepout;
    m_doNotAllowCopperPour = aZone.m_doNotAllowCopperPour;
    m_doNotAllowVias = aZone.m_doNotAllowVias;
    m_doNotAllowTracks = aZone.m_doNotAllowTracks;

    m_cornerSmoothingType = aZone.m_cornerSmoothingType;
    m_cornerRadius = aZone.m_cornerRadius;

    m_hatchStyle = aZone.m_hatchStyle;
    m_hatchPitch = aZone.m_hatchPitch;
    m_HatchLines = aZone.m_HatchLines;

    SetLayerSet( aZone.GetLayerSet() );
    SetLocalFlags( aZone.GetLocalFlags() );
}


ZONE_CONTAINER& ZONE_CONTAINER::operator=( const ZONE_CONTAINER& aOther )
{
    BOARD_CONNECTED_ITEM::operator=( aOther );

    // Replace the outlines for aOther outlines.
    delete m_Poly;
    m_Poly = new SHAPE_POLY_SET( *aOther.m_Poly );

    m_CornerSelection  = nullptr; // for corner moving, corner index to (null if no selection)
    m_ZoneClearance    = aOther.m_ZoneClearance;            // clearance value
    m_ZoneMinThickness = aOther.m_ZoneMinThickness;
    m_FillMode = aOther.m_FillMode;                         // filling mode (segments/polygons)
    m_ArcToSegmentsCount = aOther.m_ArcToSegmentsCount;
    m_PadConnection = aOther.m_PadConnection;
    m_ThermalReliefGap = aOther.m_ThermalReliefGap;
    m_ThermalReliefCopperBridge = aOther.m_ThermalReliefCopperBridge;
    SetHatchStyle( aOther.GetHatchStyle() );
    SetHatchPitch( aOther.GetHatchPitch() );
    m_HatchLines = aOther.m_HatchLines;     // copy vector <SEG>
    m_FilledPolysList.RemoveAllContours();
    m_FilledPolysList.Append( aOther.m_FilledPolysList );
    m_FillSegmList.clear();
    m_FillSegmList = aOther.m_FillSegmList;

    SetLayerSet( aOther.GetLayerSet() );

    return *this;
}


ZONE_CONTAINER::~ZONE_CONTAINER()
{
    delete m_Poly;
    delete m_CornerSelection;
}


EDA_ITEM* ZONE_CONTAINER::Clone() const
{
    return new ZONE_CONTAINER( *this );
}


bool ZONE_CONTAINER::UnFill()
{
    bool change = ( !m_FilledPolysList.IsEmpty() ) ||
                  ( m_FillSegmList.size() > 0 );

    m_FilledPolysList.RemoveAllContours();
    m_FillSegmList.clear();
    m_IsFilled = false;

    return change;
}


const wxPoint ZONE_CONTAINER::GetPosition() const
{
    return (wxPoint) GetCornerPosition( 0 );
}


PCB_LAYER_ID ZONE_CONTAINER::GetLayer() const
{
    return BOARD_ITEM::GetLayer();
}


bool ZONE_CONTAINER::IsOnCopperLayer() const
{
    if( GetIsKeepout() )
    {
        return ( m_layerSet & LSET::AllCuMask() ).count() > 0;
    }
    else
    {
        return IsCopperLayer( GetLayer() );
    }
}


bool ZONE_CONTAINER::CommonLayerExists( const LSET aLayerSet ) const
{
    LSET common = GetLayerSet() & aLayerSet;

    return common.count() > 0;
}


void ZONE_CONTAINER::SetLayer( PCB_LAYER_ID aLayer )
{
    SetLayerSet( LSET( aLayer ) );

    m_Layer = aLayer;
}


void ZONE_CONTAINER::SetLayerSet( LSET aLayerSet )
{
    if( GetIsKeepout() )
    {
        // Keepouts can only exist on copper layers
        aLayerSet &= LSET::AllCuMask();
    }

    if( aLayerSet.count() == 0 )
    {
        return;
    }

    m_layerSet = aLayerSet;

    // Set the single layer to the first selected layer
    m_Layer = aLayerSet.Seq()[0];
}


LSET ZONE_CONTAINER::GetLayerSet() const
{
    // TODO - Enable multi-layer zones for all zone types
    // not just keepout zones
    if( GetIsKeepout() )
    {
        return m_layerSet;
    }
    else
    {
        return LSET( m_Layer );
    }
}

void ZONE_CONTAINER::ViewGetLayers( int aLayers[], int& aCount ) const
{
    if( GetIsKeepout() )
    {
        LSEQ layers = m_layerSet.Seq();

        for( unsigned int idx = 0; idx < layers.size(); idx++ )
        {
            aLayers[idx] = layers[idx];
        }

        aCount = layers.size();
    }
    else
    {
        aLayers[0] = m_Layer;
        aCount = 1;
    }
}


bool ZONE_CONTAINER::IsOnLayer( PCB_LAYER_ID aLayer ) const
{
    if( GetIsKeepout() )
    {
        return m_layerSet.test( aLayer );
    }

    return BOARD_ITEM::IsOnLayer( aLayer );
}


void ZONE_CONTAINER::Draw( EDA_DRAW_PANEL* panel, wxDC* DC, GR_DRAWMODE aDrawMode,
                           const wxPoint& offset )
{
    if( !DC )
        return;

    wxPoint     seg_start, seg_end;
    PCB_LAYER_ID    curr_layer = ( (PCB_SCREEN*) panel->GetScreen() )->m_Active_Layer;
    BOARD*      brd   = GetBoard();

    auto frame = static_cast<PCB_BASE_FRAME*> ( panel->GetParent() );

    PCB_LAYER_ID draw_layer = UNDEFINED_LAYER;

    LSET layers = GetLayerSet() & brd->GetVisibleLayers();

    // If there are no visible layers and the zone is not highlighted, return
    if( layers.count() == 0 && !( aDrawMode & GR_HIGHLIGHT ) )
    {
        return;
    }

    /* Keepout zones can exist on multiple layers
     * Thus, determining which color to use to render them is a bit tricky.
     * In descending order of priority:
     *
     * 1. If in GR_HIGHLIGHT mode:
     *   a. If zone is on selected layer, use layer color!
     *   b. Else, use grey
     * 1. Not in GR_HIGHLIGHT mode
     *   a. If zone is on selected layer, use layer color
     *   b. Else, use color of top-most (visible) layer
     *
     */
    if( GetIsKeepout() )
    {
        // At least one layer must be provided!
        assert( GetLayerSet().count() > 0 );

        // Not on any visible layer?
        if( layers.count() == 0 && !( aDrawMode & GR_HIGHLIGHT ) )
        {
            return;
        }

        // Is keepout zone present on the selected layer?
        if( layers.test( curr_layer ) )
        {
            draw_layer = curr_layer;
        }
        else
        {
            // Select the first (top) visible layer
            if( layers.count() > 0 )
            {
                draw_layer = layers.Seq()[0];
            }
            else
            {
                draw_layer = GetLayerSet().Seq()[0];
            }
        }

    }
    /* Non-keepout zones are easier to deal with
     */
    else
    {
        if( brd->IsLayerVisible( GetLayer() ) == false && !( aDrawMode & GR_HIGHLIGHT ) )
        {
            return;
        }

        draw_layer = GetLayer();
    }

    assert( draw_layer != UNDEFINED_LAYER );

    auto color = frame->Settings().Colors().GetLayerColor( draw_layer );

    GRSetDrawMode( DC, aDrawMode );
    auto displ_opts = (PCB_DISPLAY_OPTIONS*)( panel->GetDisplayOptions() );

    if( displ_opts->m_ContrastModeDisplay )
    {
        if( !IsOnLayer( curr_layer ) )
        {
            color = COLOR4D( DARKDARKGRAY );
        }
    }

    if( ( aDrawMode & GR_HIGHLIGHT ) && !( aDrawMode & GR_AND ) )
    {
        color.SetToLegacyHighlightColor();
    }

    color.a = 0.588;

    // draw the lines
    std::vector<wxPoint> lines;
    lines.reserve( (GetNumCorners() * 2) + 2 );

    // Iterate through the segments of the outline
    for( auto iterator = m_Poly->IterateSegmentsWithHoles(); iterator; iterator++ )
    {
        // Create the segment
        SEG segment = *iterator;

        lines.push_back( static_cast<wxPoint>( segment.A ) + offset );
        lines.push_back( static_cast<wxPoint>( segment.B ) + offset );
    }

    GRLineArray( panel->GetClipBox(), DC, lines, 0, color );

    // draw hatches
    lines.clear();
    lines.reserve( (m_HatchLines.size() * 2) + 2 );

    for( unsigned ic = 0; ic < m_HatchLines.size(); ic++ )
    {
        seg_start = static_cast<wxPoint>( m_HatchLines[ic].A ) + offset;
        seg_end   = static_cast<wxPoint>( m_HatchLines[ic].B ) + offset;
        lines.push_back( seg_start );
        lines.push_back( seg_end );
    }

    GRLineArray( panel->GetClipBox(), DC, lines, 0, color );
}


void ZONE_CONTAINER::DrawFilledArea( EDA_DRAW_PANEL* panel,
                                     wxDC* DC, GR_DRAWMODE aDrawMode, const wxPoint& offset )
{

    static std::vector <wxPoint> CornersBuffer;
    auto displ_opts = (PCB_DISPLAY_OPTIONS*)( panel->GetDisplayOptions() );

    // outline_mode is false to show filled polys,
    // and true to show polygons outlines only (test and debug purposes)
    bool outline_mode = displ_opts->m_DisplayZonesMode == 2 ? true : false;

    if( DC == NULL )
        return;

    if( displ_opts->m_DisplayZonesMode == 1 )     // Do not show filled areas
        return;

    if( m_FilledPolysList.IsEmpty() )  // Nothing to draw
        return;

    BOARD*      brd = GetBoard();
    PCB_LAYER_ID    curr_layer = ( (PCB_SCREEN*) panel->GetScreen() )->m_Active_Layer;

    auto frame = static_cast<PCB_BASE_FRAME*> ( panel->GetParent() );
    auto color = frame->Settings().Colors().GetLayerColor( GetLayer() );

    if( brd->IsLayerVisible( GetLayer() ) == false && !( aDrawMode & GR_HIGHLIGHT ) )
        return;

    GRSetDrawMode( DC, aDrawMode );

    if( displ_opts->m_ContrastModeDisplay )
    {
        if( !IsOnLayer( curr_layer ) )
            color = COLOR4D( DARKDARKGRAY );
    }

    if( ( aDrawMode & GR_HIGHLIGHT ) && !( aDrawMode & GR_AND ) )
        color.SetToLegacyHighlightColor();

    color.a = 0.588;


    for ( int ic = 0; ic < m_FilledPolysList.OutlineCount(); ic++ )
    {
        const SHAPE_LINE_CHAIN& path = m_FilledPolysList.COutline( ic );

        CornersBuffer.clear();

        wxPoint p0;

        for( int j = 0; j < path.PointCount(); j++ )
        {
            const VECTOR2I& corner = path.CPoint( j );

            wxPoint coord( corner.x + offset.x, corner.y + offset.y );

            if( j == 0 )
                p0 = coord;

            CornersBuffer.push_back( coord );
        }

        CornersBuffer.push_back( p0 );

        // Draw outlines:
        if( ( m_ZoneMinThickness > 1 ) || outline_mode )
        {
            int ilim = CornersBuffer.size() - 1;

            for( int is = 0, ie = ilim; is <= ilim; ie = is, is++ )
            {
                int x0 = CornersBuffer[is].x;
                int y0 = CornersBuffer[is].y;
                int x1 = CornersBuffer[ie].x;
                int y1 = CornersBuffer[ie].y;

                // Draw only basic outlines, not extra segments.
                if( !displ_opts->m_DisplayPcbTrackFill || GetState( FORCE_SKETCH ) )
                    GRCSegm( panel->GetClipBox(), DC,
                             x0, y0, x1, y1,
                             m_ZoneMinThickness, color );
                else
                    GRFillCSegm( panel->GetClipBox(), DC,
                                 x0, y0, x1, y1, m_ZoneMinThickness, color );
            }
        }

        // Draw areas:
        if( m_FillMode == ZFM_POLYGONS && !outline_mode )
            GRPoly( panel->GetClipBox(), DC, CornersBuffer.size(), &CornersBuffer[0],
                    true, 0, color, color );
    }

    if( m_FillMode == 1  && !outline_mode )     // filled with segments
    {
        for( unsigned ic = 0; ic < m_FillSegmList.size(); ic++ )
        {
            wxPoint start = (wxPoint) ( m_FillSegmList[ic].A + VECTOR2I(offset) );
            wxPoint end   = (wxPoint) ( m_FillSegmList[ic].B + VECTOR2I(offset) );

            if( !displ_opts->m_DisplayPcbTrackFill || GetState( FORCE_SKETCH ) )
                GRCSegm( panel->GetClipBox(), DC, start.x, start.y, end.x, end.y,
                         m_ZoneMinThickness, color );
            else
                GRFillCSegm( panel->GetClipBox(), DC, start.x, start.y, end.x, end.y,
                             m_ZoneMinThickness, color );
        }
    }
}


const EDA_RECT ZONE_CONTAINER::GetBoundingBox() const
{
    const int PRELOAD = 0x7FFFFFFF;     // Biggest integer (32 bits)

    int       ymax = -PRELOAD;
    int       ymin = PRELOAD;
    int       xmin = PRELOAD;
    int       xmax = -PRELOAD;

    int       count = GetNumCorners();

    for( int i = 0; i<count; ++i )
    {
        wxPoint corner = static_cast<wxPoint>( GetCornerPosition( i ) );

        ymax = std::max( ymax, corner.y );
        xmax = std::max( xmax, corner.x );
        ymin = std::min( ymin, corner.y );
        xmin = std::min( xmin, corner.x );
    }

    EDA_RECT ret( wxPoint( xmin, ymin ), wxSize( xmax - xmin + 1, ymax - ymin + 1 ) );

    return ret;
}


void ZONE_CONTAINER::DrawWhileCreateOutline( EDA_DRAW_PANEL* panel, wxDC* DC,
                                             GR_DRAWMODE draw_mode )
{
    GR_DRAWMODE current_gr_mode  = draw_mode;
    bool    is_close_segment = false;

    if( !DC )
        return;

    PCB_LAYER_ID    curr_layer = ( (PCB_SCREEN*) panel->GetScreen() )->m_Active_Layer;

    auto frame = static_cast<PCB_BASE_FRAME*> ( panel->GetParent() );
    auto color = frame->Settings().Colors().GetLayerColor( GetLayer() );

    auto displ_opts = (PCB_DISPLAY_OPTIONS*)( panel->GetDisplayOptions() );

    if( displ_opts->m_ContrastModeDisplay )
    {
        if( !IsOnLayer( curr_layer ) )
            color = COLOR4D( DARKDARKGRAY );
    }

    // Object to iterate through the corners of the outlines
    SHAPE_POLY_SET::ITERATOR iterator = m_Poly->Iterate();

    // Segment start and end
    VECTOR2I seg_start, seg_end;

    // Remember the first point of this contour
    VECTOR2I contour_first_point = *iterator;

    // Iterate through all the corners of the outlines and build the segments to draw
    while( iterator )
    {
        // Get the first point of the current segment
        seg_start = *iterator;

        // Get the last point of the current segment, handling the case where the end of the
        // contour is reached, when the last point of the segment is the first point of the
        // contour
        if( !iterator.IsEndContour() )
        {
            // Set GR mode to default
            current_gr_mode = draw_mode;

            SHAPE_POLY_SET::ITERATOR iterator_copy = iterator;
            iterator_copy++;
            if( iterator_copy.IsEndContour() )
                current_gr_mode = GR_XOR;

            is_close_segment = false;

            iterator++;
            seg_end = *iterator;
        }
        else
        {
            is_close_segment = true;

            seg_end = contour_first_point;

            // Reassign first point of the contour to the next contour start
            iterator++;

            if( iterator )
                contour_first_point = *iterator;

            // Set GR mode to XOR
            current_gr_mode = GR_XOR;
        }

        GRSetDrawMode( DC, current_gr_mode );

        if( is_close_segment )
            GRLine( panel->GetClipBox(), DC, seg_start.x, seg_start.y, seg_end.x, seg_end.y, 0,
                    WHITE );
        else
            GRLine( panel->GetClipBox(), DC, seg_start.x, seg_start.y, seg_end.x, seg_end.y, 0,
                    color );
    }
}


int ZONE_CONTAINER::GetThermalReliefGap( D_PAD* aPad ) const
{
    if( aPad == NULL || aPad->GetThermalGap() == 0 )
        return m_ThermalReliefGap;
    else
        return aPad->GetThermalGap();
}


int ZONE_CONTAINER::GetThermalReliefCopperBridge( D_PAD* aPad ) const
{
    if( aPad == NULL || aPad->GetThermalWidth() == 0 )
        return m_ThermalReliefCopperBridge;
    else
        return aPad->GetThermalWidth();
}


void ZONE_CONTAINER::SetCornerRadius( unsigned int aRadius )
{
    m_cornerRadius = aRadius;
}


bool ZONE_CONTAINER::HitTest( const wxPoint& aPosition ) const
{
    // Normally accuracy is zoom-relative, but for the generic HitTest we just use
    // a fixed (small) value.
    int accuracy = Millimeter2iu( 0.05 );

    return HitTestForCorner( aPosition, accuracy * 2 ) || HitTestForEdge( aPosition, accuracy );
}


void ZONE_CONTAINER::SetSelectedCorner( const wxPoint& aPosition, int aAccuracy )
{
    SHAPE_POLY_SET::VERTEX_INDEX corner;

    // If there is some corner to be selected, assign it to m_CornerSelection
    if( HitTestForCorner( aPosition, aAccuracy * 2, corner )
        || HitTestForEdge( aPosition, aAccuracy, corner ) )
    {
        if( m_CornerSelection == nullptr )
            m_CornerSelection = new SHAPE_POLY_SET::VERTEX_INDEX;

        *m_CornerSelection = corner;
    }
}

bool ZONE_CONTAINER::HitTestForCorner( const wxPoint& refPos, int aAccuracy,
                                       SHAPE_POLY_SET::VERTEX_INDEX& aCornerHit ) const
{
    return m_Poly->CollideVertex( VECTOR2I( refPos ), aCornerHit, aAccuracy );
}


bool ZONE_CONTAINER::HitTestForCorner( const wxPoint& refPos, int aAccuracy ) const
{
    SHAPE_POLY_SET::VERTEX_INDEX dummy;
    return HitTestForCorner( refPos, aAccuracy, dummy );
}


bool ZONE_CONTAINER::HitTestForEdge( const wxPoint& refPos, int aAccuracy,
                                     SHAPE_POLY_SET::VERTEX_INDEX& aCornerHit ) const
{
    return m_Poly->CollideEdge( VECTOR2I( refPos ), aCornerHit, aAccuracy );
}


bool ZONE_CONTAINER::HitTestForEdge( const wxPoint& refPos, int aAccuracy ) const
{
    SHAPE_POLY_SET::VERTEX_INDEX dummy;
    return HitTestForEdge( refPos, aAccuracy, dummy );
}


bool ZONE_CONTAINER::HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy ) const
{
    // Calculate bounding box for zone
    EDA_RECT bbox = GetBoundingBox();
    bbox.Normalize();

    EDA_RECT arect = aRect;
    arect.Normalize();
    arect.Inflate( aAccuracy );

    if( aContained )
    {
         return arect.Contains( bbox );
    }
    else    // Test for intersection between aBox and the polygon
            // For a polygon, using its bounding box has no sense here
    {
        // Fast test: if aBox is outside the polygon bounding box,
        // rectangles cannot intersect
        if( !arect.Intersects( bbox ) )
            return false;

        // aBox is inside the polygon bounding box,
        // and can intersect the polygon: use a fine test.
        // aBox intersects the polygon if at least one aBox corner
        // is inside the polygon

        /*
        wxPoint origin = arect.GetOrigin();

        int w = arect.GetWidth();
        int h = arect.GetHeight();


        if ( HitTestInsideZone( origin ) ||
             HitTestInsideZone( origin + wxPoint( w, 0 ) ) ||
             HitTestInsideZone( origin + wxPoint( w, h ) ) ||
             HitTestInsideZone( origin + wxPoint( 0, h ) ) )
        {
            return true;
        }
        */

        // No corner inside aBox, but outlines can intersect aBox
        // if one of outline corners is inside aBox
        int count = m_Poly->TotalVertices();
        for( int ii =0; ii < count; ii++ )
        {
            auto vertex = m_Poly->Vertex( ii );
            auto vertexNext = m_Poly->Vertex( ( ii + 1 ) % count );

            // Test if the point is within the rect
            if( arect.Contains( ( wxPoint ) vertex ) )
            {
                return true;
            }

            // Test if this edge intersects the rect
            if( arect.Intersects( ( wxPoint ) vertex, ( wxPoint ) vertexNext ) )
            {
                return true;
            }
        }

        return false;
    }
}


int ZONE_CONTAINER::GetClearance( BOARD_CONNECTED_ITEM* aItem ) const
{
    int         myClearance = m_ZoneClearance;

#if 1   // Maybe the netclass clearance should not come into play for a zone?
        // At least the policy decision can be controlled by the zone
        // itself, i.e. here.  On reasons of insufficient documentation,
        // the user will be less bewildered if we simply respect the
        // "zone clearance" setting in the zone properties dialog.  (At least
        // until there is a UI boolean for this.)

    NETCLASSPTR   myClass  = GetNetClass();

    if( myClass )
        myClearance = std::max( myClearance, myClass->GetClearance() );
#endif

    if( aItem )
    {
        int hisClearance = aItem->GetClearance( NULL );
        myClearance = std::max( hisClearance, myClearance );
    }

    return myClearance;
}


bool ZONE_CONTAINER::HitTestFilledArea( const wxPoint& aRefPos ) const
{
    return m_FilledPolysList.Contains( VECTOR2I( aRefPos.x, aRefPos.y ) );
}


void ZONE_CONTAINER::GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList )
{
    wxString msg;

    msg = _( "Zone Outline" );

    // Display Cutout instead of Outline for holes inside a zone
    // i.e. when num contour !=0
    // Check whether the selected corner is in a hole; i.e., in any contour but the first one.
    if( m_CornerSelection != nullptr && m_CornerSelection->m_contour > 0 )
        msg << wxT( " " ) << _( "(Cutout)" );

    aList.push_back( MSG_PANEL_ITEM( _( "Type" ), msg, DARKCYAN ) );

    if( GetIsKeepout() )
    {
        msg.Empty();

        if( GetDoNotAllowVias() )
            AccumulateDescription( msg, _( "No via" ) );

        if( GetDoNotAllowTracks() )
            AccumulateDescription( msg, _("No track") );

        if( GetDoNotAllowCopperPour() )
            AccumulateDescription( msg, _("No copper pour") );

        aList.push_back( MSG_PANEL_ITEM( _( "Keepout" ), msg, RED ) );
    }
    else if( IsOnCopperLayer() )
    {
        if( GetNetCode() >= 0 )
        {
            NETINFO_ITEM* net = GetNet();

            if( net )
                msg = net->GetNetname();
            else    // Should not occur
                msg = _( "<unknown>" );
        }
        else    // a netcode < 0 is an error
            msg = wxT( "<error>" );

        aList.push_back( MSG_PANEL_ITEM( _( "NetName" ), msg, RED ) );

        // Display net code : (useful in test or debug)
        msg.Printf( wxT( "%d" ), GetNetCode() );
        aList.push_back( MSG_PANEL_ITEM( _( "NetCode" ), msg, RED ) );

        // Display priority level
        msg.Printf( wxT( "%d" ), GetPriority() );
        aList.push_back( MSG_PANEL_ITEM( _( "Priority" ), msg, BLUE ) );
    }
    else
    {
        aList.push_back( MSG_PANEL_ITEM( _( "Non Copper Zone" ), wxEmptyString, RED ) );
    }

    aList.push_back( MSG_PANEL_ITEM( _( "Layer" ), GetLayerName(), BROWN ) );

    msg.Printf( wxT( "%d" ), (int) m_Poly->TotalVertices() );
    aList.push_back( MSG_PANEL_ITEM( _( "Corners" ), msg, BLUE ) );

    if( m_FillMode )
        msg = _( "Segments" );
    else
        msg = _( "Polygons" );

    aList.push_back( MSG_PANEL_ITEM( _( "Fill Mode" ), msg, BROWN ) );

    // Useful for statistics :
    msg.Printf( wxT( "%d" ), (int) m_HatchLines.size() );
    aList.push_back( MSG_PANEL_ITEM( _( "Hatch Lines" ), msg, BLUE ) );

    if( !m_FilledPolysList.IsEmpty() )
    {
        msg.Printf( wxT( "%d" ), m_FilledPolysList.TotalVertices() );
        aList.push_back( MSG_PANEL_ITEM( _( "Corner Count" ), msg, BLUE ) );
    }
}


/* Geometric transforms: */

void ZONE_CONTAINER::Move( const wxPoint& offset )
{
    /* move outlines */
    m_Poly->Move( VECTOR2I( offset ) );

    Hatch();

    m_FilledPolysList.Move( VECTOR2I( offset.x, offset.y ) );

    for( unsigned ic = 0; ic < m_FillSegmList.size(); ic++ )
    {
        m_FillSegmList[ic].A += VECTOR2I(offset);
        m_FillSegmList[ic].B += VECTOR2I(offset);
    }
}


void ZONE_CONTAINER::MoveEdge( const wxPoint& offset, int aEdge )
{
    int next_corner;

    if( m_Poly->GetNeighbourIndexes( aEdge, nullptr, &next_corner ) )
    {
        m_Poly->Vertex( aEdge ) += VECTOR2I( offset );
        m_Poly->Vertex( next_corner ) += VECTOR2I( offset );
        Hatch();
    }
}


void ZONE_CONTAINER::Rotate( const wxPoint& centre, double angle )
{
    wxPoint pos;

    for( auto iterator = m_Poly->IterateWithHoles(); iterator; iterator++ )
    {
        pos = static_cast<wxPoint>( *iterator );
        RotatePoint( &pos, centre, angle );
        iterator->x = pos.x;
        iterator->y = pos.y;
    }

    Hatch();

    /* rotate filled areas: */
    for( auto ic = m_FilledPolysList.Iterate(); ic; ++ic )
        RotatePoint( &ic->x, &ic->y, centre.x, centre.y, angle );

    for( unsigned ic = 0; ic < m_FillSegmList.size(); ic++ )
    {
        wxPoint a( m_FillSegmList[ic].A );
        RotatePoint( &a, centre, angle );
        m_FillSegmList[ic].A = a;
        wxPoint b( m_FillSegmList[ic].B );
        RotatePoint( &b, centre, angle );
        m_FillSegmList[ic].B = a;
    }
}


void ZONE_CONTAINER::Flip( const wxPoint& aCentre )
{
    Mirror( aCentre );
    int copperLayerCount = GetBoard()->GetCopperLayerCount();

    if( GetIsKeepout() )
    {
        SetLayerSet( FlipLayerMask( GetLayerSet(), copperLayerCount ) );
    }
    else
    {
        SetLayer( FlipLayer( GetLayer(), copperLayerCount ) );
    }
}


void ZONE_CONTAINER::Mirror( const wxPoint& mirror_ref )
{
    for( auto iterator = m_Poly->IterateWithHoles(); iterator; iterator++ )
    {
        int py = mirror_ref.y - iterator->y;
        iterator->y = py + mirror_ref.y;
    }

    Hatch();

    for( auto ic = m_FilledPolysList.Iterate(); ic; ++ic )
    {
        int py = mirror_ref.y - ic->y;
        ic->y = py + mirror_ref.y;
    }

    for( unsigned ic = 0; ic < m_FillSegmList.size(); ic++ )
    {
        MIRROR( m_FillSegmList[ic].A.y, mirror_ref.y );
        MIRROR( m_FillSegmList[ic].B.y, mirror_ref.y );
    }
}


ZoneConnection ZONE_CONTAINER::GetPadConnection( D_PAD* aPad ) const
{
    if( aPad == NULL || aPad->GetZoneConnection() == PAD_ZONE_CONN_INHERITED )
        return m_PadConnection;
    else
        return aPad->GetZoneConnection();
}


void ZONE_CONTAINER::AddPolygon( std::vector< wxPoint >& aPolygon )
{
    if( aPolygon.empty() )
        return;

    SHAPE_LINE_CHAIN outline;

    // Create an outline and populate it with the points of aPolygon
    for( unsigned i = 0;  i < aPolygon.size();  i++ )
    {
        outline.Append( VECTOR2I( aPolygon[i] ) );
    }

    outline.SetClosed( true );

    // Add the outline as a new polygon in the polygon set
    if( m_Poly->OutlineCount() == 0 )
        m_Poly->AddOutline( outline );
    else
        m_Poly->AddHole( outline );
}


bool ZONE_CONTAINER::AppendCorner( wxPoint aPosition, int aHoleIdx, bool aAllowDuplication )
{
    // Ensure the main outline exists:
    if( m_Poly->OutlineCount() == 0 )
        m_Poly->NewOutline();

    // If aHoleIdx >= 0, the corner musty be added to the hole, index aHoleIdx.
    // (remember: the index of the first hole is 0)
    // Return error if if does dot exist.
    if( aHoleIdx >= m_Poly->HoleCount( 0 ) )
        return false;

    m_Poly->Append( aPosition.x, aPosition.y, -1, aHoleIdx, aAllowDuplication );

    return true;
}


wxString ZONE_CONTAINER::GetSelectMenuText( EDA_UNITS_T aUnits ) const
{
    wxString text;

    // Check whether the selected contour is a hole (contour index > 0)
    if( m_CornerSelection != nullptr &&  m_CornerSelection->m_contour > 0 )
        text << wxT( " " ) << _( "(Cutout)" );

    if( GetIsKeepout() )
        text << wxT( " " ) << _( "(Keepout)" );
    else
        text << GetNetnameMsg();

    return wxString::Format( _( "Zone Outline %s on %s" ), text, GetLayerName() );
}


int ZONE_CONTAINER::GetHatchPitch() const
{
    return m_hatchPitch;
}


void ZONE_CONTAINER::SetHatch( int aHatchStyle, int aHatchPitch, bool aRebuildHatch )
{
    SetHatchPitch( aHatchPitch );
    m_hatchStyle = (ZONE_CONTAINER::HATCH_STYLE) aHatchStyle;

    if( aRebuildHatch )
        Hatch();
}


void ZONE_CONTAINER::SetHatchPitch( int aPitch )
{
    m_hatchPitch = aPitch;
}


void ZONE_CONTAINER::UnHatch()
{
    m_HatchLines.clear();
}


// Creates hatch lines inside the outline of the complex polygon
// sort function used in ::Hatch to sort points by descending wxPoint.x values
bool sortEndsByDescendingX( const VECTOR2I& ref, const VECTOR2I& tst )
{
    return tst.x < ref.x;
}


void ZONE_CONTAINER::Hatch()
{
    UnHatch();

    if( m_hatchStyle == NO_HATCH || m_hatchPitch == 0 || m_Poly->IsEmpty() )
        return;

    // define range for hatch lines
    int min_x = m_Poly->Vertex( 0 ).x;
    int max_x = m_Poly->Vertex( 0 ).x;
    int min_y = m_Poly->Vertex( 0 ).y;
    int max_y = m_Poly->Vertex( 0 ).y;

    for( auto iterator = m_Poly->IterateWithHoles(); iterator; iterator++ )
    {
        if( iterator->x < min_x )
            min_x = iterator->x;

        if( iterator->x > max_x )
            max_x = iterator->x;

        if( iterator->y < min_y )
            min_y = iterator->y;

        if( iterator->y > max_y )
            max_y = iterator->y;
    }

    // Calculate spacing between 2 hatch lines
    int spacing;

    if( m_hatchStyle == DIAGONAL_EDGE )
        spacing = m_hatchPitch;
    else
        spacing = m_hatchPitch * 2;

    // set the "length" of hatch lines (the length on horizontal axis)
    int  hatch_line_len = m_hatchPitch;

    // To have a better look, give a slope depending on the layer
    LAYER_NUM layer = GetLayer();
    int     slope_flag = (layer & 1) ? 1 : -1;  // 1 or -1
    double  slope = 0.707106 * slope_flag;      // 45 degrees slope
    int     max_a, min_a;

    if( slope_flag == 1 )
    {
        max_a   = KiROUND( max_y - slope * min_x );
        min_a   = KiROUND( min_y - slope * max_x );
    }
    else
    {
        max_a   = KiROUND( max_y - slope * max_x );
        min_a   = KiROUND( min_y - slope * min_x );
    }

    min_a = (min_a / spacing) * spacing;

    // calculate an offset depending on layer number,
    // for a better look of hatches on a multilayer board
    int offset = (layer * 7) / 8;
    min_a += offset;

    // loop through hatch lines
    #define MAXPTS 200      // Usually we store only few values per one hatch line
                            // depending on the complexity of the zone outline

    static std::vector<VECTOR2I> pointbuffer;
    pointbuffer.clear();
    pointbuffer.reserve( MAXPTS + 2 );

    for( int a = min_a; a < max_a; a += spacing )
    {
        // get intersection points for this hatch line

        // Note: because we should have an even number of intersections with the
        // current hatch line and the zone outline (a closed polygon,
        // or a set of closed polygons), if an odd count is found
        // we skip this line (should not occur)
        pointbuffer.clear();

        // Iterate through all vertices
        for( auto iterator = m_Poly->IterateSegmentsWithHoles(); iterator; iterator++ )
        {
            double  x, y, x2, y2;
            int     ok;

            SEG segment = *iterator;

            ok = FindLineSegmentIntersection( a, slope,
                                              segment.A.x, segment.A.y,
                                              segment.B.x, segment.B.y,
                                              &x, &y, &x2, &y2 );

              if( ok )
              {
                  VECTOR2I point( KiROUND( x ), KiROUND( y ) );
                  pointbuffer.push_back( point );
              }

              if( ok == 2 )
              {
                  VECTOR2I point( KiROUND( x2 ), KiROUND( y2 ) );
                  pointbuffer.push_back( point );
              }

              if( pointbuffer.size() >= MAXPTS )    // overflow
              {
                  wxASSERT( 0 );
                  break;
              }
        }

        // ensure we have found an even intersection points count
        // because intersections are the ends of segments
        // inside the polygon(s) and a segment has 2 ends.
        // if not, this is a strange case (a bug ?) so skip this hatch
        if( pointbuffer.size() % 2 != 0 )
            continue;

        // sort points in order of descending x (if more than 2) to
        // ensure the starting point and the ending point of the same segment
        // are stored one just after the other.
        if( pointbuffer.size() > 2 )
            sort( pointbuffer.begin(), pointbuffer.end(), sortEndsByDescendingX );

        // creates lines or short segments inside the complex polygon
        for( unsigned ip = 0; ip < pointbuffer.size(); ip += 2 )
        {
            int dx = pointbuffer[ip + 1].x - pointbuffer[ip].x;

            // Push only one line for diagonal hatch,
            // or for small lines < twice the line length
            // else push 2 small lines
            if( m_hatchStyle == DIAGONAL_FULL || std::abs( dx ) < 2 * hatch_line_len )
            {
                m_HatchLines.push_back( SEG( pointbuffer[ip], pointbuffer[ip + 1] ) );
            }
            else
            {
                double dy = pointbuffer[ip + 1].y - pointbuffer[ip].y;
                slope = dy / dx;

                if( dx > 0 )
                    dx = hatch_line_len;
                else
                    dx = -hatch_line_len;

                int x1 = KiROUND( pointbuffer[ip].x + dx );
                int x2 = KiROUND( pointbuffer[ip + 1].x - dx );
                int y1 = KiROUND( pointbuffer[ip].y + dx * slope );
                int y2 = KiROUND( pointbuffer[ip + 1].y - dx * slope );

                m_HatchLines.push_back(SEG(pointbuffer[ip].x, pointbuffer[ip].y, x1, y1));

                m_HatchLines.push_back( SEG( pointbuffer[ip+1].x, pointbuffer[ip+1].y, x2, y2 ) );
            }
        }
    }
}


int ZONE_CONTAINER::GetDefaultHatchPitch()
{
    return Mils2iu( 20 );
}


BITMAP_DEF ZONE_CONTAINER::GetMenuImage() const
{
    return add_zone_xpm;
}


void ZONE_CONTAINER::SwapData( BOARD_ITEM* aImage )
{
    assert( aImage->Type() == PCB_ZONE_AREA_T );

    std::swap( *((ZONE_CONTAINER*) this), *((ZONE_CONTAINER*) aImage) );
}


void ZONE_CONTAINER::CacheTriangulation()
{
    m_FilledPolysList.CacheTriangulation();
}


bool ZONE_CONTAINER::BuildSmoothedPoly( SHAPE_POLY_SET& aSmoothedPoly ) const
{
    if( GetNumCorners() <= 2 )  // malformed zone. polygon calculations do not like it ...
        return false;

    // Make a smoothed polygon out of the user-drawn polygon if required
    switch( m_cornerSmoothingType )
    {
    case ZONE_SETTINGS::SMOOTHING_CHAMFER:
        aSmoothedPoly = m_Poly->Chamfer( m_cornerRadius );
        break;

    case ZONE_SETTINGS::SMOOTHING_FILLET:
        // Note: we're now using m_ArcToSegmentsCount only as a hint to determine accuracy
        // vs. speed.
        if( m_ArcToSegmentsCount > SEGMENT_COUNT_CROSSOVER )
            aSmoothedPoly = m_Poly->Fillet( m_cornerRadius, ARC_HIGH_DEF );
        else
            aSmoothedPoly = m_Poly->Fillet( m_cornerRadius, ARC_LOW_DEF );
        break;

    default:
        // Acute angles between adjacent edges can create issues in calculations,
        // in inflate/deflate outlines transforms, especially when the angle is very small.
        // We can avoid issues by creating a very small chamfer which remove acute angles,
        // or left it without chamfer and use only CPOLYGONS_LIST::InflateOutline to create
        // clearance areas
        aSmoothedPoly = m_Poly->Chamfer( Millimeter2iu( 0.0 ) );
        break;
    }

    return true;
};

/* Function TransformOutlinesShapeWithClearanceToPolygon
 * Convert the zone filled areas polygons to polygons
 * inflated (optional) by max( aClearanceValue, the zone clearance)
 * and copy them in aCornerBuffer
 * param aClearanceValue = the clearance around polygons
 * param aAddClearance = true to add a clearance area to the polygon
 *                      false to create the outline polygon.
 */
void ZONE_CONTAINER::TransformOutlinesShapeWithClearanceToPolygon(
        SHAPE_POLY_SET& aCornerBuffer, int aMinClearanceValue, bool aUseNetClearance ) const
{
    // Creates the zone outline polygon (with holes if any)
    SHAPE_POLY_SET polybuffer;
    BuildSmoothedPoly( polybuffer );

    // add clearance to outline
    int clearance = aMinClearanceValue;

    if( aUseNetClearance && IsOnCopperLayer() )
    {
        clearance = GetClearance();
        if( aMinClearanceValue > clearance )
            clearance = aMinClearanceValue;
    }

    // Calculate the polygon with clearance
    // holes are linked to the main outline, so only one polygon is created.
    if( clearance )
        polybuffer.Inflate( clearance, ARC_APPROX_SEGMENTS_COUNT_HIGH_DEF );

    polybuffer.Fracture( SHAPE_POLY_SET::PM_FAST );
    aCornerBuffer.Append( polybuffer );
}
