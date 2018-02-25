/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
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
 * @file sch_line.h
 */

#ifndef _SCH_LINE_H_
#define _SCH_LINE_H_


#include <sch_item_struct.h>

class NETLIST_OBJECT_LIST;

/**
 * Segment description base class to describe items which have 2 end points (track, wire,
 * draw line ...)
 */
class SCH_LINE : public SCH_ITEM
{
    bool    m_startIsDangling;  ///< True if start point is not connected.
    bool    m_endIsDangling;    ///< True if end point is not connected.
    wxPoint m_start;            ///< Line start point
    wxPoint m_end;              ///< Line end point
    int     m_size;             ///< Line pensize
    int     m_style;            ///< Line style
    COLOR4D m_color;            ///< Line color

public:

    static const enum wxPenStyle PenStyle[];

    SCH_LINE( const wxPoint& pos = wxPoint( 0, 0 ), int layer = LAYER_NOTES );

    SCH_LINE( const SCH_LINE& aLine );

    ~SCH_LINE() { }

    SCH_LINE* Next() const { return (SCH_LINE*) Pnext; }
    SCH_LINE* Back() const { return (SCH_LINE*) Pback; }

    wxString GetClass() const override
    {
        return wxT( "SCH_LINE" );
    }

    bool IsEndPoint( const wxPoint& aPoint ) const
    {
        return aPoint == m_start || aPoint == m_end;
    }

    bool IsNull() const { return m_start == m_end; }

    wxPoint GetStartPoint() const { return m_start; }

    void SetStartPoint( const wxPoint& aPosition ) { m_start = aPosition; }

    wxPoint GetEndPoint() const { return m_end; }

    void SetEndPoint( const wxPoint& aPosition ) { m_end = aPosition; }

    int GetDefaultStyle() const;

    void SetLineStyle( const int aStyle );

    int GetLineStyle() const;

    /// @return the style name from the style id
    /// (mainly to write it in .sch file
    static const char* GetLineStyleName( int aStyle );

    /// @return the style id from the style  name
    /// (mainly to read style from .sch file
    static int GetLineStyleInternalId( const wxString& aStyleName );

    void SetLineColor( const COLOR4D aColor );

    void SetLineColor( const double r, const double g, const double b, const double a );

    COLOR4D GetLineColor() const;

    COLOR4D GetDefaultColor() const;

    int GetDefaultWidth() const;

    void SetLineWidth( const int aSize );

    int GetLineSize() const { return m_size; }

    const EDA_RECT GetBoundingBox() const override;

    /**
     * Function GetLength
     * @return The length of the line segment.
     */
    double GetLength() const;

    void Draw( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aOffset,
               GR_DRAWMODE aDrawMode, COLOR4D aColor = COLOR4D::UNSPECIFIED ) override;

    int GetPenSize() const override;

    void Move( const wxPoint& aMoveVector ) override;

    void MirrorX( int aXaxis_position ) override;

    void MirrorY( int aYaxis_position ) override;

    void Rotate( wxPoint aPosition ) override;

    /**
     * Check line against \a aLine to see if it overlaps and merge if it does.
     *
     * This method will return an equivalent of the union of line and \a aLine if the
     * two lines overlap.  This method is used to merge multiple line segments into a single
     * line.
     *
     * @param aLine - Line to compare.
     * @return New line that combines the two or NULL on non-overlapping segments.
     */
    EDA_ITEM* MergeOverlap( SCH_LINE* aLine );

    /**
     * Check if two lines are in the same quadrant as each other, using a reference point as
     * the origin
     *
     * @param aLine - Line to compare
     * @param aPosition - Point to reference against lines
     * @return true if lines are mostly in different quadrants of aPosition, false otherwise
     */
    bool IsSameQuadrant( SCH_LINE* aLine, const wxPoint& aPosition );

    bool IsParallel( SCH_LINE* aLine );

    void GetEndPoints( std::vector<DANGLING_END_ITEM>& aItemList ) override;

    bool IsDanglingStateChanged( std::vector< DANGLING_END_ITEM >& aItemList ) override;

    bool IsDangling() const override { return m_startIsDangling || m_endIsDangling; }

    bool IsSelectStateChanged( const wxRect& aRect ) override;

    bool IsConnectable() const override;

    void GetConnectionPoints(std::vector< wxPoint >& aPoints ) const override;

    bool CanConnect( const SCH_ITEM* aItem ) const override;

    wxString GetSelectMenuText() const override;

    BITMAP_DEF GetMenuImage() const override;

    void GetNetListItem( NETLIST_OBJECT_LIST& aNetListItems, SCH_SHEET_PATH* aSheetPath ) override;

    bool operator <( const SCH_ITEM& aItem ) const override;

    wxPoint GetPosition() const override { return m_start; }

    void SetPosition( const wxPoint& aPosition ) override;

    bool HitTest( const wxPoint& aPosition, int aAccuracy ) const override;

    bool HitTest( const EDA_RECT& aRect, bool aContained = false, int aAccuracy = 0 ) const override;

    void Plot( PLOTTER* aPlotter ) override;

    wxPoint MidPoint();

    EDA_ITEM* Clone() const override;

    void SwapData( SCH_ITEM* aItem ) override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override;
#endif

private:
    bool doIsConnected( const wxPoint& aPosition ) const override;
};


#endif    // _SCH_LINE_H_
