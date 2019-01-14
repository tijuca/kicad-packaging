/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
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

#ifndef GERBVIEW_DRAW_PANEL_GAL_H_
#define GERBVIEW_DRAW_PANEL_GAL_H_

#include <class_draw_panel_gal.h>
#include <worksheet_viewitem.h>

namespace KIGFX
{
    class WORKSHEET_VIEWITEM;
}

class COLORS_DESIGN_SETTINGS;


class GERBVIEW_DRAW_PANEL_GAL : public EDA_DRAW_PANEL_GAL
{
public:
    GERBVIEW_DRAW_PANEL_GAL( wxWindow* aParentWindow, wxWindowID aWindowId, const wxPoint& aPosition,
                        const wxSize& aSize, KIGFX::GAL_DISPLAY_OPTIONS& aOptions,
                        GAL_TYPE aGalType = GAL_TYPE_OPENGL );

    virtual ~GERBVIEW_DRAW_PANEL_GAL();

    /**
     * Function UseColorScheme
     * Applies layer color settings.
     * @param aSettings are the new settings.
     */
    void UseColorScheme( const COLORS_DESIGN_SETTINGS* aSettings );

    ///> @copydoc EDA_DRAW_PANEL_GAL::SetHighContrastLayer()
    virtual void SetHighContrastLayer( int aLayer ) override;

    ///> @copydoc EDA_DRAW_PANEL_GAL::GetMsgPanelInfo()
    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector<MSG_PANEL_ITEM>& aList ) override;

    ///> @copydoc EDA_DRAW_PANEL_GAL::OnShow()
    void OnShow() override;

    bool SwitchBackend( GAL_TYPE aGalType ) override;

    ///> @copydoc EDA_DRAW_PANEL_GAL::SetTopLayer
    virtual void SetTopLayer( int aLayer ) override;

    ///> @copydoc EDA_DRAW_PANEL_GAL::GetDefaultViewBBox()
    BOX2I GetDefaultViewBBox() const override;

    /**
     * Sets (or updates) worksheet used by the draw panel.
     * @param aWorksheet is the worksheet to be used.
     *        The object is then owned by GERBVIEW_DRAW_PANEL_GAL.
     */
    void SetWorksheet( KIGFX::WORKSHEET_VIEWITEM* aWorksheet );

protected:
    ///> Sets rendering targets & dependencies for layers.
    void setDefaultLayerDeps();

    ///> Currently used worksheet
    std::unique_ptr<KIGFX::WORKSHEET_VIEWITEM> m_worksheet;
};


#endif /* GERBVIEW_DRAW_PANEL_GAL_H_ */
