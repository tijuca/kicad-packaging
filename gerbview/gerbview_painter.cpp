/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Jon Evans <jon@craftyjon.com>
 * Copyright (C) 2017 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <colors_design_settings.h>

#include <gerbview_painter.h>
#include <gal/graphics_abstraction_layer.h>
#include <convert_basic_shapes_to_polygon.h>
#include <convert_to_biu.h>
#include <gerbview.h>

#include <gerber_draw_item.h>
#include <gerber_file_image.h>

using namespace KIGFX;

GERBVIEW_RENDER_SETTINGS::GERBVIEW_RENDER_SETTINGS()
{
    m_backgroundColor = COLOR4D( 0.0, 0.0, 0.0, 1.0 );

    m_spotFill          = true;
    m_lineFill          = true;
    m_polygonFill       = true;
    m_showNegativeItems = false;
    m_showCodes         = false;
    m_diffMode          = true;

    m_componentHighlightString = "";
    m_netHighlightString       = "";
    m_attributeHighlightString = "";

    update();
}


void GERBVIEW_RENDER_SETTINGS::ImportLegacyColors( const COLORS_DESIGN_SETTINGS* aSettings )
{
    for( int i = GERBVIEW_LAYER_ID_START;
         i < GERBVIEW_LAYER_ID_START + GERBER_DRAWLAYERS_COUNT; i++ )
    {
        COLOR4D baseColor = aSettings->GetLayerColor( i );

        if( m_diffMode )
            baseColor.a = 0.75;

        m_layerColors[i] = baseColor;
        m_layerColorsHi[i] = baseColor.Brightened( 0.5 );
        m_layerColorsSel[i] = baseColor.Brightened( 0.8 );
        m_layerColorsDark[i] = baseColor.Darkened( 0.25 );
    }

    for( int i = LAYER_DCODES; i < GERBVIEW_LAYER_ID_END; i++ )
        m_layerColors[i] = aSettings->GetLayerColor( i );

    for( int i = GAL_LAYER_ID_START; i < GAL_LAYER_ID_END; i++ )
        m_layerColors[i] = aSettings->GetLayerColor( i );

    update();
}


void GERBVIEW_RENDER_SETTINGS::LoadDisplayOptions( const GBR_DISPLAY_OPTIONS* aOptions )
{
    if( aOptions == NULL )
        return;

    m_spotFill          = aOptions->m_DisplayFlashedItemsFill;
    m_lineFill          = aOptions->m_DisplayLinesFill;
    m_polygonFill       = aOptions->m_DisplayPolygonsFill;
    m_showNegativeItems = aOptions->m_DisplayNegativeObjects;
    m_showCodes         = aOptions->m_DisplayDCodes;
    m_diffMode          = aOptions->m_DiffMode;
    m_hiContrastEnabled = aOptions->m_HighContrastMode;

    update();
}


const COLOR4D& GERBVIEW_RENDER_SETTINGS::GetColor( const VIEW_ITEM* aItem, int aLayer ) const
{
    const GERBER_DRAW_ITEM* item = static_cast<const GERBER_DRAW_ITEM*>( aItem );
    static const COLOR4D transparent = COLOR4D( 0, 0, 0, 0 );

    // All DCODE layers stored under a single color setting
    if( IsDCodeLayer( aLayer ) )
        return m_layerColors[ LAYER_DCODES ];

    if( item )
    {
        if( item->IsSelected() )
            return m_layerColorsSel[aLayer];

        if( item->GetLayerPolarity() )
        {
            if( m_showNegativeItems )
                return m_layerColors[LAYER_NEGATIVE_OBJECTS];
            else
                return transparent;
        }
    }

    if( !m_netHighlightString.IsEmpty() && item &&
        m_netHighlightString == item->GetNetAttributes().m_Netname )
        return m_layerColorsHi[aLayer];

    if( !m_componentHighlightString.IsEmpty() && item &&
        m_componentHighlightString == item->GetNetAttributes().m_Cmpref )
        return m_layerColorsHi[aLayer];

    if( !m_attributeHighlightString.IsEmpty() && item && item->GetDcodeDescr() &&
        m_attributeHighlightString == item->GetDcodeDescr()->m_AperFunction )
        return m_layerColorsHi[aLayer];

    // Return grayish color for non-highlighted layers in the high contrast mode
    if( m_hiContrastEnabled && m_activeLayers.count( aLayer ) == 0)
        return m_hiContrastColor;

    // Catch the case when highlight and high-contraste modes are enabled
    // and we are drawing a not highlighted track
    if( m_highlightEnabled )
        return m_layerColorsDark[aLayer];

    // No special modificators enabled
    return m_layerColors[aLayer];
}


GERBVIEW_PAINTER::GERBVIEW_PAINTER( GAL* aGal ) :
    PAINTER( aGal )
{
}


// TODO(JE): Pull up to PAINTER?
int GERBVIEW_PAINTER::getLineThickness( int aActualThickness ) const
{
    // if items have 0 thickness, draw them with the outline
    // width, otherwise respect the set value (which, no matter
    // how small will produce something)
    if( aActualThickness == 0 )
        return m_gerbviewSettings.m_outlineWidth;

    return aActualThickness;
}


bool GERBVIEW_PAINTER::Draw( const VIEW_ITEM* aItem, int aLayer )
{
    const EDA_ITEM* item = static_cast<const EDA_ITEM*>( aItem );

    // the "cast" applied in here clarifies which overloaded draw() is called
    switch( item->Type() )
    {
    case GERBER_DRAW_ITEM_T:
        draw( static_cast<GERBER_DRAW_ITEM*>( const_cast<EDA_ITEM*>( item ) ), aLayer );
        break;

    default:
        // Painter does not know how to draw the object
        return false;
    }

    return true;
}


// TODO(JE) aItem can't be const because of GetDcodeDescr()
// Probably that can be refactored in GERBER_DRAW_ITEM to allow const here.
void GERBVIEW_PAINTER::draw( /*const*/ GERBER_DRAW_ITEM* aItem, int aLayer )
{
    VECTOR2D start( aItem->GetABPosition( aItem->m_Start ) );   // TODO(JE) Getter
    VECTOR2D end( aItem->GetABPosition( aItem->m_End ) );       // TODO(JE) Getter
    int      width = aItem->m_Size.x;   // TODO(JE) Getter
    bool     isFilled = true;
    COLOR4D  color;
    // TODO(JE) This doesn't actually work properly for ImageNegative
    bool     isNegative = ( aItem->GetLayerPolarity() ^ aItem->m_GerberImageFile->m_ImageNegative );

    // Draw DCODE overlay text
    if( IsDCodeLayer( aLayer ) )
    {
        wxString codeText;
        VECTOR2D textPosition;
        double textSize;
        double orient;

        if( !aItem->GetTextD_CodePrms( textSize, textPosition, orient ) )
            return;

        color = m_gerbviewSettings.GetColor( aItem, aLayer );
        codeText.Printf( "D%d", aItem->m_DCode );

        m_gal->SetIsStroke( true );
        m_gal->SetIsFill( false );
        m_gal->SetStrokeColor( color );
        m_gal->SetFillColor( COLOR4D( 0, 0, 0, 0 ) );
        m_gal->SetLineWidth( 2 );
        m_gal->SetFontBold( false );
        m_gal->SetFontItalic( false );
        m_gal->SetTextMirrored( false );
        m_gal->SetGlyphSize( VECTOR2D( textSize, textSize) );
        m_gal->SetHorizontalJustify( GR_TEXT_HJUSTIFY_CENTER );
        m_gal->SetVerticalJustify( GR_TEXT_VJUSTIFY_CENTER );
        m_gal->BitmapText( codeText, textPosition, orient );

        return;
    }

    color = m_gerbviewSettings.GetColor( aItem, aLayer );

    // TODO: Should brightened color be a preference?
    if( aItem->IsBrightened() )
        color = COLOR4D( 0.0, 1.0, 0.0, 0.75 );

    m_gal->SetNegativeDrawMode( isNegative );
    m_gal->SetStrokeColor( color );
    m_gal->SetFillColor( color );
    m_gal->SetIsFill( isFilled );
    m_gal->SetIsStroke( !isFilled );

    switch( aItem->m_Shape )
    {
    case GBR_POLYGON:
    {
        isFilled = m_gerbviewSettings.m_polygonFill;
        m_gal->SetIsFill( isFilled );
        m_gal->SetIsStroke( !isFilled );

        if( isNegative && !isFilled )
        {
            m_gal->SetNegativeDrawMode( false );
            m_gal->SetStrokeColor( GetSettings()->GetColor( aItem, aLayer ) );
        }

        if( !isFilled )
            m_gal->SetLineWidth( m_gerbviewSettings.m_outlineWidth );

        SHAPE_POLY_SET absolutePolygon = aItem->m_Polygon;

        for( auto it = absolutePolygon.Iterate( 0 ); it; ++it )
            *it = aItem->GetABPosition( *it );

        if( !isFilled )
            m_gal->DrawPolyline( absolutePolygon.COutline( 0 ) );
        else
            m_gal->DrawPolygon( absolutePolygon );
        break;
    }

    case GBR_CIRCLE:
    {
        isFilled = m_gerbviewSettings.m_lineFill;
        double radius = GetLineLength( aItem->m_Start, aItem->m_End );
        m_gal->DrawCircle( start, radius );
        break;
    }

    case GBR_ARC:
    {
        isFilled = m_gerbviewSettings.m_lineFill;

        // These are swapped because wxDC fills arcs counterclockwise and GAL
        // fills them clockwise.
        wxPoint arcStart = aItem->m_End;
        wxPoint arcEnd = aItem->m_Start;

        // Gerber arcs are 3-point (start, center, end)
        // GAL needs center, radius, start angle, end angle
        double   radius = GetLineLength( arcStart, aItem->m_ArcCentre );
        VECTOR2D center = aItem->GetABPosition( aItem->m_ArcCentre );
        VECTOR2D startVec = VECTOR2D( aItem->GetABPosition( arcStart ) ) - center;
        VECTOR2D endVec = VECTOR2D( aItem->GetABPosition( arcEnd ) ) - center;

        m_gal->SetIsFill( isFilled );
        m_gal->SetIsStroke( !isFilled );
        m_gal->SetLineWidth( isFilled ? width : m_gerbviewSettings.m_outlineWidth );

        double startAngle = startVec.Angle();
        double endAngle = endVec.Angle();

        // GAL fills in direction of increasing angle, so we have to convert
        // the angle from the -PI to PI domain of atan2() to ensure that
        // the arc goes in the right direction
        if( startAngle > endAngle )
            endAngle += (2 * M_PI);

        // 360-degree arcs are stored in the file with start equal to end
        if( arcStart == arcEnd )
        {
            startAngle = 0;
            endAngle = 2 * M_PI;
        }

        m_gal->DrawArcSegment( center, radius, startAngle, endAngle, width );

        // Arc Debugging
        // m_gal->SetLineWidth( 5 );
        // m_gal->SetStrokeColor( COLOR4D( 0.0, 1.0, 0.0, 1.0 ) );
        // m_gal->DrawLine( center, aItem->GetABPosition( arcStart ) );
        // m_gal->SetStrokeColor( COLOR4D( 1.0, 0.0, 0.0, 1.0 ) );
        // m_gal->DrawLine( center, aItem->GetABPosition( arcEnd ) );
        break;
    }

    case GBR_SPOT_CIRCLE:
    case GBR_SPOT_RECT:
    case GBR_SPOT_OVAL:
    case GBR_SPOT_POLY:
    case GBR_SPOT_MACRO:
    {
        isFilled = m_gerbviewSettings.m_spotFill;
        drawFlashedShape( aItem, isFilled );
        break;
    }

    case GBR_SEGMENT:
    {
        /* Plot a line from m_Start to m_End.
         * Usually, a round pen is used, but some gerber files use a rectangular pen
         * In fact, any aperture can be used to plot a line.
         * currently: only a square pen is handled (I believe using a polygon gives a strange plot).
         */
        isFilled = m_gerbviewSettings.m_lineFill;
        m_gal->SetIsFill( isFilled );
        m_gal->SetIsStroke( !isFilled );

        if( isNegative && !isFilled )
            m_gal->SetStrokeColor( GetSettings()->GetColor( aItem, aLayer ) );

        // TODO(JE) Refactor this to allow const aItem
        D_CODE* code = aItem->GetDcodeDescr();
        if( code && code->m_Shape == APT_RECT )
        {
            if( aItem->m_Polygon.OutlineCount() == 0 )
                aItem->ConvertSegmentToPolygon();
            drawPolygon( aItem, aItem->m_Polygon, isFilled );
        }
        else
        {
            if( !isFilled )
                m_gal->SetLineWidth( m_gerbviewSettings.m_outlineWidth );

            m_gal->DrawSegment( start, end, width );
        }
        break;
    }

    default:
        wxASSERT_MSG( false, wxT( "GERBER_DRAW_ITEM shape is unknown!" ) );
        break;
    }

    // Enable for bounding box debugging
    #if 0
    const BOX2I& bb = aItem->ViewBBox();
    m_gal->SetIsStroke( true );
    m_gal->SetIsFill( true );
    m_gal->SetLineWidth( 3 );
    m_gal->SetStrokeColor( COLOR4D(0.9, 0.9, 0, 0.4) );
    m_gal->SetFillColor( COLOR4D(0.9, 0.9, 0, 0.1) );
    m_gal->DrawRectangle( bb.GetOrigin(), bb.GetEnd() );
    #endif
}


void GERBVIEW_PAINTER::drawPolygon( GERBER_DRAW_ITEM* aParent,
                                    SHAPE_POLY_SET& aPolygon,
                                    bool aFilled )
{
    for( auto it = aPolygon.Iterate( 0 ); it; ++it )
        *it = aParent->GetABPosition( *it );

    if( !m_gerbviewSettings.m_polygonFill )
        m_gal->SetLineWidth( m_gerbviewSettings.m_outlineWidth );

    if( !aFilled )
    {
        for( int i = 0; i < aPolygon.OutlineCount(); i++ )
            m_gal->DrawPolyline( aPolygon.COutline( i ) );
    }
    else
        m_gal->DrawPolygon( aPolygon );
}


void GERBVIEW_PAINTER::drawFlashedShape( GERBER_DRAW_ITEM* aItem, bool aFilled )
{
    D_CODE* code = aItem->GetDcodeDescr();

    wxASSERT_MSG( code, wxT( "drawFlashedShape: Item has no D_CODE!" ) );

    m_gal->SetIsFill( aFilled );
    m_gal->SetIsStroke( !aFilled );
    m_gal->SetLineWidth( m_gerbviewSettings.m_outlineWidth );

    switch( aItem->m_Shape )
    {
    case GBR_SPOT_CIRCLE:
    {
        int radius = code->m_Size.x >> 1;
        VECTOR2D start( aItem->GetABPosition( aItem->m_Start ) );

        if( !aFilled )
        {
            m_gal->DrawCircle( start, radius );
        }
        else
        {
            if( code->m_DrillShape == APT_DEF_NO_HOLE )
            {
                m_gal->DrawCircle( start, radius );
            }
            else    // rectangular hole
            {
                if( code->m_Polygon.OutlineCount() == 0 )
                    code->ConvertShapeToPolygon();

                SHAPE_POLY_SET poly = code->m_Polygon;
                poly.Move( aItem->m_Start );

                drawPolygon( aItem, poly, aFilled );
            }
        }
        break;
    }

    case GBR_SPOT_RECT:
    {
        wxPoint codeStart;
        wxPoint aShapePos = aItem->m_Start;
        codeStart.x = aShapePos.x - code->m_Size.x / 2;
        codeStart.y = aShapePos.y - code->m_Size.y / 2;
        wxPoint codeEnd = codeStart + code->m_Size;
        codeStart = aItem->GetABPosition( codeStart );
        codeEnd = aItem->GetABPosition( codeEnd );

        if( !aFilled || code->m_DrillShape == APT_DEF_NO_HOLE  )
        {
            m_gal->DrawRectangle( VECTOR2D( codeStart ), VECTOR2D( codeEnd ) );
        }
        else
        {
            if( code->m_Polygon.OutlineCount() == 0 )
                code->ConvertShapeToPolygon();

            SHAPE_POLY_SET poly = code->m_Polygon;
            poly.Move( aItem->m_Start );

            drawPolygon( aItem, poly, aFilled );
        }
        break;
    }

    case GBR_SPOT_OVAL:
    {
        int radius = 0;

        wxPoint codeStart = aItem->m_Start;
        wxPoint codeEnd = aItem->m_Start;

        if( code->m_Size.x > code->m_Size.y )   // horizontal oval
        {
            int delta = (code->m_Size.x - code->m_Size.y) / 2;
            codeStart.x -= delta;
            codeEnd.x   += delta;
            radius   = code->m_Size.y;
        }
        else   // horizontal oval
        {
            int delta = (code->m_Size.y - code->m_Size.x) / 2;
            codeStart.y -= delta;
            codeEnd.y   += delta;
            radius   = code->m_Size.x;
        }

        codeStart = aItem->GetABPosition( codeStart );
        codeEnd = aItem->GetABPosition( codeEnd );

        if( !aFilled || code->m_DrillShape == APT_DEF_NO_HOLE )
        {
            m_gal->DrawSegment( codeStart, codeEnd, radius );
        }
        else
        {
            if( code->m_Polygon.OutlineCount() == 0 )
                code->ConvertShapeToPolygon();

            SHAPE_POLY_SET poly = code->m_Polygon;
            poly.Move( aItem->m_Start );

            drawPolygon( aItem, poly, aFilled );
        }
        break;
    }

    case GBR_SPOT_POLY:
    {
        if( code->m_Polygon.OutlineCount() == 0 )
            code->ConvertShapeToPolygon();

        SHAPE_POLY_SET poly = code->m_Polygon;
        poly.Move( aItem->m_Start );

        drawPolygon( aItem, poly, aFilled );
        break;
    }

    case GBR_SPOT_MACRO:
        drawApertureMacro( aItem, aFilled );
        break;

    default:
        wxASSERT_MSG( false, wxT( "Unknown Gerber flashed shape!" ) );
        break;
    }
}


void GERBVIEW_PAINTER::drawApertureMacro( GERBER_DRAW_ITEM* aParent, bool aFilled )
{
    D_CODE* code = aParent->GetDcodeDescr();
    APERTURE_MACRO* macro = code->GetMacro();

    SHAPE_POLY_SET* macroShape = macro->GetApertureMacroShape( aParent, aParent->m_Start );

    if( !m_gerbviewSettings.m_polygonFill )
        m_gal->SetLineWidth( m_gerbviewSettings.m_outlineWidth );

    if( !aFilled )
    {
        for( int i = 0; i < macroShape->OutlineCount(); i++ )
            m_gal->DrawPolyline( macroShape->COutline( i ) );
    }
    else
        m_gal->DrawPolygon( *macroShape );
}


const double GERBVIEW_RENDER_SETTINGS::MAX_FONT_SIZE = Millimeter2iu( 10.0 );
