/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2004-2017 KiCad Developers, see change_log.txt for contributors.
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
 * @file lib_arc.h
 */

#ifndef _LIB_ARC_H_
#define _LIB_ARC_H_

#include <lib_draw_item.h>


class TRANSFORM;


class LIB_ARC : public LIB_ITEM
{
    enum SELECT_T               // When creating an arc: status of arc
    {
        ARC_STATUS_START,
        ARC_STATUS_END,
        ARC_STATUS_OUTLINE,
    };

    int      m_Radius;
    int      m_t1;              // First radius angle of the arc in 0.1 degrees.
    int      m_t2;              /* Second radius angle of the arc in 0.1 degrees. */
    wxPoint  m_ArcStart;
    wxPoint  m_ArcEnd;          /* Arc end position. */
    wxPoint  m_Pos;             /* Radius center point. */
    int      m_Width;           /* Line width */
    double   m_editCenterDistance;
    SELECT_T m_editSelectPoint;
    int      m_editState;
    int      m_editDirection;
    int      m_lastEditState;

    /**
     * Draws the arc.
     */
    void drawGraphic( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aOffset,
                      COLOR4D aColor, GR_DRAWMODE aDrawMode, void* aData,
                      const TRANSFORM& aTransform ) override;

    /**
     * Draw the graphics when the arc is being edited.
     */
    void drawEditGraphics( EDA_RECT* aClipBox, wxDC* aDC, COLOR4D aColor ) override;

    /**
     * Calculates the center, radius, and angles at \a aPosition when the arc is being edited.
     *
     * Note: The center may not necessarily be on the grid.
     *
     * @param aPosition - The current mouse position in drawing coordinates.
     */
    void calcEdit( const wxPoint& aPosition ) override;


public:
    LIB_ARC( LIB_PART * aParent );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    ~LIB_ARC() { }

    wxString GetClass() const override
    {
        return wxT( "LIB_ARC" );
    }

    wxString GetTypeName() override
    {
        return _( "Arc" );
    }

    bool HitTest( const wxPoint& aPosition ) const override;

    bool HitTest( const wxPoint& aPosition, int aThreshold, const TRANSFORM& aTransform ) const override;

    const EDA_RECT GetBoundingBox() const override;

    void GetMsgPanelInfo( std::vector< MSG_PANEL_ITEM >& aList ) override;

    int GetPenSize() const override;

    void BeginEdit( STATUS_FLAGS aEditMode, const wxPoint aStartPoint = wxPoint( 0, 0 ) ) override;

    bool ContinueEdit( const wxPoint aNextPoint ) override;

    void EndEdit( const wxPoint& aPosition, bool aAbort = false ) override;

    void SetOffset( const wxPoint& aOffset ) override;

    bool Inside( EDA_RECT& aRect ) const override;

    void Move( const wxPoint& aPosition ) override;

    wxPoint GetPosition() const override { return m_Pos; }

    void MirrorHorizontal( const wxPoint& aCenter ) override;

    void MirrorVertical( const wxPoint& aCenter ) override;

    void Rotate( const wxPoint& aCenter, bool aRotateCCW = true ) override;

    void Plot( PLOTTER* aPlotter, const wxPoint& aOffset, bool aFill,
               const TRANSFORM& aTransform ) override;

    int GetWidth() const override { return m_Width; }

    void SetWidth( int aWidth ) override { m_Width = aWidth; }

    void SetRadius( int aRadius ) { m_Radius = aRadius; }

    int GetRadius() const { return m_Radius; }

    void SetFirstRadiusAngle( int aAngle ) { m_t1 = aAngle; }

    int GetFirstRadiusAngle() const { return m_t1; }

    void SetSecondRadiusAngle( int aAngle ) { m_t2 = aAngle; }

    int GetSecondRadiusAngle() const { return m_t2; }

    wxPoint GetStart() const { return m_ArcStart; }

    void SetStart( const wxPoint& aPoint ) { m_ArcStart = aPoint; }

    wxPoint GetEnd() const { return m_ArcEnd; }

    void SetEnd( const wxPoint& aPoint ) { m_ArcEnd = aPoint; }

    /**
     * Calculate the radius and angle of an arc using the start, end, and center points.
     */
    void CalcRadiusAngles();


    wxString GetSelectMenuText() const override;

    BITMAP_DEF GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

private:

    /**
     * @copydoc LIB_ITEM::compare()
     *
     * The arc specific sort order is as follows:
     *      - Arc horizontal (X) position.
     *      - Arc vertical (Y) position.
     *      - Arc start angle.
     *      - Arc end angle.
     */
    int compare( const LIB_ITEM& aOther ) const override;
};


#endif    // _LIB_ARC_H_
