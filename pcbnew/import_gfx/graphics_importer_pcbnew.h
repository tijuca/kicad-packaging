/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2016 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 * Copyright (C) 2018 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef GRAPHICS_IMPORTER_PCBNEW_H
#define GRAPHICS_IMPORTER_PCBNEW_H

#include "graphics_importer.h"

#include <layers_id_colors_and_visibility.h>

class BOARD_ITEM;
class BOARD;
class MODULE;
class DRAWSEGMENT;
class EDA_TEXT;

class GRAPHICS_IMPORTER_PCBNEW : public GRAPHICS_IMPORTER
{
public:
    GRAPHICS_IMPORTER_PCBNEW();

    /**
     * @brief Sets the target layer for the imported shapes.
     * @param aLayer is the layer to be used by the imported shapes.
     */
    void SetLayer( PCB_LAYER_ID aLayer )
    {
        m_layer = aLayer;
    }

    /**
     * @brief Returns the target layer for the imported shapes.
     */
    PCB_LAYER_ID GetLayer() const
    {
        return m_layer;
    }

    void AddLine( const VECTOR2D& aOrigin, const VECTOR2D& aEnd, double aWidth ) override;

    void AddCircle( const VECTOR2D& aOrigin, double aRadius, double aWidth ) override;

    void AddArc( const VECTOR2D& aCenter, const VECTOR2D& aStart, double aAngle, double aWidth ) override;

    void AddPolygon( const std::vector< VECTOR2D >& aVertices, double aWidth ) override;

    void AddText( const VECTOR2D& aOrigin, const wxString& aText,
            double aHeight, double aWidth, double aThickness, double aOrientation,
            EDA_TEXT_HJUSTIFY_T aHJustify, EDA_TEXT_VJUSTIFY_T aVJustify ) override;

    void AddSpline( const VECTOR2D& aStart, const VECTOR2D& aBezierControl1,
                    const VECTOR2D& aBezierControl2, const VECTOR2D& aEnd , double aWidth ) override;

    /** convert a imported coordinate to a board coordinate, according to
     * the internal units, the user scale and offset
     * @param aCoordinate is the imported coordinate in mm
     */
    wxPoint MapCoordinate( const VECTOR2D& aCoordinate );

    /** @return a line thickness in a board Iu value, according to
     * the internal units.
     * if aLineWidth < 0, the default ine thickness value is returned
     * @param aLineWidth is the line thickness in mm to convert
     */
    int MapLineWidth( double aLineWidth );

protected:
    ///> Create an object representing a graphical shape.
    virtual std::unique_ptr<DRAWSEGMENT> createDrawing() = 0;

    ///> Create an object representing a text. Both pointers point to different parts of the
    ///> same object, the EDA_TEXT pointer is simply for convenience.
    virtual std::pair<std::unique_ptr<BOARD_ITEM>, EDA_TEXT*> createText() = 0;

    ///> Target layer for the imported shapes.
    PCB_LAYER_ID m_layer;
};


class GRAPHICS_IMPORTER_BOARD : public GRAPHICS_IMPORTER_PCBNEW
{
public:
    GRAPHICS_IMPORTER_BOARD( BOARD* aBoard )
        : m_board( aBoard )
    {
    }

protected:
    std::unique_ptr<DRAWSEGMENT> createDrawing() override;
    std::pair<std::unique_ptr<BOARD_ITEM>, EDA_TEXT*> createText() override;

    BOARD* m_board;
};


class GRAPHICS_IMPORTER_MODULE : public GRAPHICS_IMPORTER_PCBNEW
{
public:
    GRAPHICS_IMPORTER_MODULE( MODULE* aModule )
        : m_module( aModule )
    {
    }

protected:
    std::unique_ptr<DRAWSEGMENT> createDrawing() override;
    std::pair<std::unique_ptr<BOARD_ITEM>, EDA_TEXT*> createText() override;

    MODULE* m_module;
};

#endif /* GRAPHICS_IMPORTER_PCBNEW */
