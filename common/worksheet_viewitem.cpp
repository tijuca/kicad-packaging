/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2013-2018 CERN
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

/**
 * @file worksheet_viewitem.cpp
 * @brief Class that handles properties and drawing of worksheet layout.
 */

#include <worksheet_viewitem.h>
#include <worksheet_shape_builder.h>
#include <worksheet_dataitem.h>
#include <gal/graphics_abstraction_layer.h>
#include <painter.h>
#include <layers_id_colors_and_visibility.h>
#include <page_info.h>
#include <view/view.h>

using namespace KIGFX;

WORKSHEET_VIEWITEM::WORKSHEET_VIEWITEM( int aMils2IUscalefactor,
            const PAGE_INFO* aPageInfo, const TITLE_BLOCK* aTitleBlock ) :
    EDA_ITEM( NOT_USED ), // this item is never added to a BOARD so it needs no type
    m_mils2IUscalefactor( aMils2IUscalefactor ),
    m_titleBlock( aTitleBlock ), m_pageInfo( aPageInfo ), m_sheetNumber( 1 ), m_sheetCount( 1 ) {}


void WORKSHEET_VIEWITEM::SetPageInfo( const PAGE_INFO* aPageInfo )
{
    m_pageInfo = aPageInfo;
}


void WORKSHEET_VIEWITEM::SetTitleBlock( const TITLE_BLOCK* aTitleBlock )
{
    m_titleBlock = aTitleBlock;
}


const BOX2I WORKSHEET_VIEWITEM::ViewBBox() const
{
    BOX2I bbox;

    if( m_pageInfo != NULL )
    {
        bbox.SetOrigin( VECTOR2I( 0, 0 ) );
        bbox.SetEnd( VECTOR2I( m_pageInfo->GetWidthMils() * m_mils2IUscalefactor,
                               m_pageInfo->GetHeightMils() * m_mils2IUscalefactor ) );
    }
    else
    {
        bbox.SetMaximum();
    }

    return bbox;
}


void WORKSHEET_VIEWITEM::ViewDraw( int aLayer, VIEW* aView ) const
{
    auto gal = aView->GetGAL();
    auto settings = aView->GetPainter()->GetSettings();
    wxString fileName( m_fileName.c_str(), wxConvUTF8 );
    wxString sheetName( m_sheetName.c_str(), wxConvUTF8 );
    WS_DRAW_ITEM_LIST drawList;

    drawList.SetPenSize( settings->GetWorksheetLineWidth() );
    // Adjust the scaling factor for worksheet items:
    // worksheet items coordinates and sizes are stored in mils,
    // and must be scaled to the same units as the caller
    drawList.SetMilsToIUfactor( m_mils2IUscalefactor );
    drawList.SetSheetNumber( m_sheetNumber );
    drawList.SetSheetCount( m_sheetCount );
    drawList.SetFileName( fileName );
    drawList.SetSheetName( sheetName );

    COLOR4D color = settings->GetColor( this, aLayer );
    drawList.BuildWorkSheetGraphicList( *m_pageInfo, *m_titleBlock, color, color );

    // Draw the title block normally even if the view is flipped
    bool flipped = gal->IsFlippedX();

    if( flipped )
    {
        gal->Save();
        gal->Translate( VECTOR2D( m_pageInfo->GetWidthMils() * m_mils2IUscalefactor, 0 ) );
        gal->Scale( VECTOR2D( -1.0, 1.0 ) );
    }

    // Draw all the components that make the page layout
    WS_DRAW_ITEM_BASE* item = drawList.GetFirst();
    while( item )
    {
        switch( item->GetType() )
        {
        case WS_DRAW_ITEM_BASE::wsg_line:
            draw( static_cast<const WS_DRAW_ITEM_LINE*>( item ), gal );
            break;

        case WS_DRAW_ITEM_BASE::wsg_rect:
            draw( static_cast<const WS_DRAW_ITEM_RECT*>( item ), gal );
            break;

        case WS_DRAW_ITEM_BASE::wsg_poly:
            draw( static_cast<const WS_DRAW_ITEM_POLYGON*>( item ), gal );
            break;

        case WS_DRAW_ITEM_BASE::wsg_text:
            draw( static_cast<const WS_DRAW_ITEM_TEXT*>( item ), gal );
            break;

        case WS_DRAW_ITEM_BASE::wsg_bitmap:
            draw( static_cast<const WS_DRAW_ITEM_BITMAP*>( item ), gal );
            break;
        }

        item = drawList.GetNext();
    }

    // Draw gray line that outlines the sheet size
    if( settings->GetShowPageLimits() )
        drawBorder( gal );

    if( flipped )
        gal->Restore();
}


void WORKSHEET_VIEWITEM::ViewGetLayers( int aLayers[], int& aCount ) const
{
    aCount = 1;
    aLayers[0] = LAYER_WORKSHEET;
}


void WORKSHEET_VIEWITEM::draw( const WS_DRAW_ITEM_LINE* aItem, GAL* aGal ) const
{
    aGal->SetIsStroke( true );
    aGal->SetIsFill( false );
    aGal->SetStrokeColor( COLOR4D( aItem->GetColor() ) );
    aGal->SetLineWidth( aItem->GetPenWidth() );
    aGal->DrawLine( VECTOR2D( aItem->GetStart() ), VECTOR2D( aItem->GetEnd() ) );
}


void WORKSHEET_VIEWITEM::draw( const WS_DRAW_ITEM_RECT* aItem, GAL* aGal ) const
{
    aGal->SetIsStroke( true );
    aGal->SetIsFill( false );
    aGal->SetStrokeColor( COLOR4D( aItem->GetColor() ) );
    aGal->SetLineWidth( aItem->GetPenWidth() );
    aGal->DrawRectangle( VECTOR2D( aItem->GetStart() ), VECTOR2D( aItem->GetEnd() ) );
}


void WORKSHEET_VIEWITEM::draw( const WS_DRAW_ITEM_POLYGON* aItem, GAL* aGal ) const
{
    std::deque<VECTOR2D> corners;
    for( wxPoint point : aItem->m_Corners )
    {
        corners.push_back( VECTOR2D( point ) );
    }

    if( aItem->IsFilled() )
    {
        aGal->SetFillColor( COLOR4D( aItem->GetColor() ) );
        aGal->SetIsFill( true );
        aGal->SetIsStroke( false );
        aGal->DrawPolygon( corners );
    }
    else
    {
        aGal->SetStrokeColor( COLOR4D( aItem->GetColor() ) );
        aGal->SetIsFill( false );
        aGal->SetIsStroke( true );
        aGal->SetLineWidth( aItem->GetPenWidth() );
        aGal->DrawPolyline( corners );
    }
}


void WORKSHEET_VIEWITEM::draw( const WS_DRAW_ITEM_TEXT* aItem, GAL* aGal ) const
{
    VECTOR2D position( aItem->GetTextPos().x, aItem->GetTextPos().y );

    aGal->Save();
    aGal->Translate( position );
    aGal->Rotate( -aItem->GetTextAngle() * M_PI / 1800.0 );
    aGal->SetStrokeColor( COLOR4D( aItem->GetColor() ) );
    aGal->SetLineWidth( aItem->GetThickness() );
    aGal->SetTextAttributes( aItem );
    aGal->StrokeText( aItem->GetShownText(), VECTOR2D( 0, 0 ), 0.0 );
    aGal->Restore();
}


void WORKSHEET_VIEWITEM::draw( const WS_DRAW_ITEM_BITMAP* aItem, GAL* aGal ) const
{
    aGal->Save();
    VECTOR2D position = aItem->GetPosition();
    aGal->Translate( position );
    WORKSHEET_DATAITEM_BITMAP* parent = static_cast<WORKSHEET_DATAITEM_BITMAP*>( aItem->GetParent() );

    // When the image scale factor is not 1.0, we need to modify the actual scale
    // as the image scale factor is similar to a local zoom
    double img_scale = parent->m_ImageBitmap->GetScale();

    if( img_scale != 1.0 )
        aGal->Scale( VECTOR2D( img_scale, img_scale ) );

    aGal->DrawBitmap( *parent->m_ImageBitmap );
    aGal->Restore();
}


void WORKSHEET_VIEWITEM::drawBorder( GAL* aGal ) const
{
    VECTOR2D origin = VECTOR2D( 0.0, 0.0 );
    VECTOR2D end = VECTOR2D( m_pageInfo->GetWidthMils() * m_mils2IUscalefactor,
                             m_pageInfo->GetHeightMils() * m_mils2IUscalefactor );

    aGal->SetIsStroke( true );
    // Use a gray color for the border color
    aGal->SetStrokeColor( COLOR4D( 0.4, 0.4, 0.4, 1.0 ) );
    aGal->SetIsFill( false );
    aGal->DrawRectangle( origin, end );
}
