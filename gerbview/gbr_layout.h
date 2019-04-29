/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2012-2014 Jean-Pierre Charras  jp.charras at wanadoo.fr
 * Copyright (C) 1992-2014 KiCad Developers, see change_log.txt for contributors.
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
 * @file gbr_layout.h
 * @brief Class CLASS_GBR_LAYOUT to handle info to draw loaded Gerber images
 * and page frame reference
 */

#ifndef GBR_LAYOUT_H
#define GBR_LAYOUT_H


#include <dlist.h>

#include <colors_design_settings.h>
#include <common.h>                         // PAGE_INFO
#include <gerbview.h>                       // GERBER_DRAWLAYERS_COUNT
#include <title_block.h>
#include <gerber_draw_item.h>
#include <gbr_display_options.h>

#include <gr_basic.h>
#include <eda_rect.h>

class GERBER_FILE_IMAGE_LIST;

/**
 * Class GBR_LAYOUT
 * holds list of GERBER_DRAW_ITEM currently loaded.
 */
class GBR_LAYOUT : public EDA_ITEM
{
private:
    mutable EDA_RECT    m_BoundingBox;
    TITLE_BLOCK         m_titles;
    wxPoint             m_originAxisPosition;

public:

    GBR_LAYOUT();
    ~GBR_LAYOUT();

    wxString GetClass() const override
    {
        return wxT( "GBR_LAYOUT" );
    }

    // Accessor to the GERBER_FILE_IMAGE_LIST,
    // which handles the list of gerber files (and drill files) images loaded
    GERBER_FILE_IMAGE_LIST* GetImagesList() const;

    const wxPoint&      GetAuxOrigin() const
    {
        return m_originAxisPosition;
    }

    void SetAuxOrigin( const wxPoint& aPosition )
    {
        m_originAxisPosition = aPosition;
    }

    TITLE_BLOCK& GetTitleBlock()
    {
        return m_titles;
    }

    void SetTitleBlock( const TITLE_BLOCK& aTitleBlock )
    {
        m_titles = aTitleBlock;
    }

    /**
     * Function ComputeBoundingBox
     * calculates the bounding box containing all Gerber items.
     * @return EDA_RECT - the full item list bounding box
     */
    EDA_RECT ComputeBoundingBox() const;

    /**
     * Function GetBoundingBox
     */
    const EDA_RECT GetBoundingBox() const override
    {
        return ComputeBoundingBox();
    }

    void SetBoundingBox( const EDA_RECT& aBox ) { m_BoundingBox = aBox; }

    /**
     * Function Draw.
     * Redraw the CLASS_GBR_LAYOUT items but not cursors, axis or grid.
     * @param aPanel = the draw canvas
     * @param aDC = the current device context
     * @param aDrawMode = GR_COPY, GR_OR ... (not always used)
     * @param aOffset = an draw offset value
     * @param aDisplayOptions = a GBR_DISPLAY_OPTIONS for draw display opts
     */
    void Draw( EDA_DRAW_PANEL* aPanel, wxDC* aDC,
               GR_DRAWMODE aDrawMode, const wxPoint& aOffset,
               GBR_DISPLAY_OPTIONS* aDisplayOptions );

    /**
     * Function DrawItemsDCodeID
     * Draw the DCode value (if exists) corresponding to gerber item
     * (polygons do not have a DCode)
     * @param aPanel = the draw canvas
     * @param aDC = the current device context
     * @param aDrawMode = GR_COPY, GR_OR ...
     * @param aDrawColor = the color of dcode texts
     */
    void DrawItemsDCodeID( EDA_DRAW_PANEL* aPanel, wxDC* aDC,
                           GR_DRAWMODE aDrawMode, COLOR4D aDrawColor );

    ///> @copydoc EDA_ITEM::Visit()
    SEARCH_RESULT Visit( INSPECTOR inspector, void* testData, const KICAD_T scanTypes[] ) override;


#if defined(DEBUG)

    void    Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }

#endif
};

#endif      // #ifndef GBR_LAYOUT_H
