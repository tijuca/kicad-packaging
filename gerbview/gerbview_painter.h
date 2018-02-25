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

#ifndef __GERBVIEW_PAINTER_H
#define __GERBVIEW_PAINTER_H

#include <layers_id_colors_and_visibility.h>
#include <painter.h>
#include <gbr_display_options.h>
#include <geometry/shape_poly_set.h>

#include <memory>


class EDA_ITEM;
class COLORS_DESIGN_SETTINGS;

class GERBER_DRAW_ITEM;
class GERBER_FILE_IMAGE;


namespace KIGFX
{
class GAL;

/**
 * Class GERBVIEW_RENDER_SETTINGS
 * Stores GerbView specific render settings.
 */
class GERBVIEW_RENDER_SETTINGS : public RENDER_SETTINGS
{
public:
    friend class GERBVIEW_PAINTER;

    GERBVIEW_RENDER_SETTINGS();

    /// @copydoc RENDER_SETTINGS::ImportLegacyColors()
    void ImportLegacyColors( const COLORS_DESIGN_SETTINGS* aSettings ) override;

    /**
     * Function LoadDisplayOptions
     * Loads settings related to display options
     * @param aOptions are settings that you want to use for displaying items.
     */
    void LoadDisplayOptions( const GBR_DISPLAY_OPTIONS* aOptions );

    /// @copydoc RENDER_SETTINGS::GetColor()
    virtual const COLOR4D& GetColor( const VIEW_ITEM* aItem, int aLayer ) const override;

    /**
     * Function GetLayerColor
     * Returns the color used to draw a layer.
     * @param aLayer is the layer number.
     */
    inline const COLOR4D& GetLayerColor( int aLayer ) const
    {
        return m_layerColors[aLayer];
    }

    /**
     * Function SetLayerColor
     * Changes the color used to draw a layer.
     * @param aLayer is the layer number.
     * @param aColor is the new color.
     */
    inline void SetLayerColor( int aLayer, const COLOR4D& aColor )
    {
        m_layerColors[aLayer] = aColor;

        update();       // recompute other shades of the color
    }

    inline bool IsSpotFill() const
    {
        return m_spotFill;
    }

    inline bool IsLineFill() const
    {
        return m_lineFill;
    }

    inline bool IsPolygonFill() const
    {
        return m_polygonFill;
    }

    inline bool IsShowNegativeItems() const
    {
        return m_showNegativeItems;
    }

    inline bool IsShowCodes() const
    {
        return m_showCodes;
    }

    inline bool IsDiffMode() const
    {
        return m_diffMode;
    }

    /// If set to anything but an empty string, will highlight items with matching component
    wxString m_componentHighlightString;

    /// If set to anything but an empty string, will highlight items with matching net
    wxString m_netHighlightString;

    /// If set to anything but an empty string, will highlight items with matching attribute
    wxString m_attributeHighlightString;

protected:
    /// Flag determining if spots should be drawn with fill
    bool    m_spotFill;

    /// Flag determining if lines should be drawn with fill
    bool    m_lineFill;

    /// Flag determining if polygons should be drawn with fill
    bool    m_polygonFill;

    /// Flag determining if negative items should be drawn with a "ghost" color
    bool    m_showNegativeItems;

    /// Flag determining if D-Codes should be drawn
    bool    m_showCodes;

    /// Flag determining if layers should be rendered in "diff" mode
    bool    m_diffMode;

    /// Maximum font size for D-Codes and other strings
    static const double MAX_FONT_SIZE;
};


/**
 * Class GERBVIEW_PAINTER
 * Contains methods for drawing GerbView-specific items.
 */
class GERBVIEW_PAINTER : public PAINTER
{
public:
    GERBVIEW_PAINTER( GAL* aGal );

    /// @copydoc PAINTER::ApplySettings()
    virtual void ApplySettings( const RENDER_SETTINGS* aSettings ) override
    {
        m_gerbviewSettings = *static_cast<const GERBVIEW_RENDER_SETTINGS*>( aSettings );
    }

    /// @copydoc PAINTER::GetSettings()
    virtual GERBVIEW_RENDER_SETTINGS* GetSettings() override
    {
        return &m_gerbviewSettings;
    }

    /// @copydoc PAINTER::Draw()
    virtual bool Draw( const VIEW_ITEM* aItem, int aLayer ) override;

protected:
    GERBVIEW_RENDER_SETTINGS m_gerbviewSettings;

    // Drawing functions
    void draw( /*const*/ GERBER_DRAW_ITEM* aVia, int aLayer );

    /// Helper routine to draw a polygon
    void drawPolygon( GERBER_DRAW_ITEM* aParent, SHAPE_POLY_SET& aPolygon, bool aFilled );

    /// Helper to draw a flashed shape (aka spot)
    void drawFlashedShape( GERBER_DRAW_ITEM* aItem, bool aFilled );

    /// Helper to draw an aperture macro shape
    void drawApertureMacro( GERBER_DRAW_ITEM* aParent, bool aFilled );

    /**
     * Function getLineThickness()
     * Get the thickness to draw for a line (e.g. 0 thickness lines
     * get a minimum value).
     * @param aActualThickness line own thickness
     * @return the thickness to draw
     */
    int getLineThickness( int aActualThickness ) const;
};
} // namespace KIGFX

#endif /* __GERBVIEW_PAINTER_H */
