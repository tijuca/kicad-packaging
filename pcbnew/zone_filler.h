/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2014-2017 CERN
 * Copyright (C) 2014-2017 KiCad Developers, see AUTHORS.txt for contributors.
 * @author Tomasz Włostowski <tomasz.wlostowski@cern.ch>
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

#ifndef __ZONE_FILLER_H
#define __ZONE_FILLER_H

#include <vector>
#include <class_zone.h>

class WX_PROGRESS_REPORTER;
class BOARD;
class COMMIT;
class SHAPE_POLY_SET;
class SHAPE_LINE_CHAIN;

class ZONE_FILLER
{
public:
    ZONE_FILLER( BOARD* aBoard, COMMIT* aCommit = nullptr,
            wxWindow* aActiveWindow = nullptr );
    ~ZONE_FILLER();

    void SetProgressReporter( WX_PROGRESS_REPORTER* aReporter );
    bool Fill( std::vector<ZONE_CONTAINER*> aZones, bool aCheck = false );

private:

    void buildZoneFeatureHoleList( const ZONE_CONTAINER* aZone,
            SHAPE_POLY_SET& aFeatures ) const;

    /**
     * Function computeRawFilledAreas
     * Add non copper areas polygons (pads and tracks with clearance)
     * to a filled copper area
     * used in BuildFilledSolidAreasPolygons when calculating filled areas in a zone
     * Non copper areas are pads and track and their clearance area
     * The filled copper area must be computed before
     * BuildFilledSolidAreasPolygons() call this function just after creating the
     *  filled copper area polygon (without clearance areas
     * @param aPcb: the current board
     * _NG version uses SHAPE_POLY_SET instead of Boost.Polygon
     */
    void computeRawFilledAreas( const ZONE_CONTAINER* aZone,
            const SHAPE_POLY_SET& aSmoothedOutline,
            SHAPE_POLY_SET& aRawPolys,
            SHAPE_POLY_SET& aFinalPolys ) const;

    bool fillPolygonWithHorizontalSegments( const SHAPE_LINE_CHAIN& aPolygon,
            ZONE_SEGMENT_FILL& aFillSegmList, int aStep ) const;

    /**
     * Function fillZoneWithSegments
     *  Fill sub areas in a zone with segments with m_ZoneMinThickness width
     * A scan is made line per line, on the whole filled areas, with a step of m_ZoneMinThickness.
     * all intersecting points with the horizontal infinite line and polygons to fill are calculated
     * a list of SEGZONE items is built, line per line
     * @return true if success, false on error
     */
    bool fillZoneWithSegments( const ZONE_CONTAINER* aZone,
            const SHAPE_POLY_SET& aFilledPolys,
            ZONE_SEGMENT_FILL& aFillSegs ) const;

    /**
     * Function buildUnconnectedThermalStubsPolygonList
     * Creates a set of polygons corresponding to stubs created by thermal shapes on pads
     * which are not connected to a zone (dangling bridges)
     * @param aCornerBuffer = a SHAPE_POLY_SET where to store polygons
     * @param aPcb = the board.
     * @param aZone = a pointer to the ZONE_CONTAINER  to examine.
     * @param aArcCorrection = a pointer to the ZONE_CONTAINER  to examine.
     * @param aRoundPadThermalRotation = the rotation in 1.0 degree for thermal stubs in round pads
     */
    void buildUnconnectedThermalStubsPolygonList( SHAPE_POLY_SET& aCornerBuffer,
            const ZONE_CONTAINER* aZone,
            const SHAPE_POLY_SET&       aRawFilledArea,
            double aArcCorrection,
            double aRoundPadThermalRotation ) const;

    /**
     * Build the filled solid areas polygons from zone outlines (stored in m_Poly)
     * The solid areas can be more than one on copper layers, and do not have holes
     *  ( holes are linked by overlapping segments to the main outline)
     * in order to have drawable (and plottable) filled polygons.
     * @return true if OK, false if the solid polygons cannot be built
     * @param aZone is the zone to fill
     * @param aRawPolys: A reference to a SHAPE_POLY_SET buffer to store
     * filled solid areas polygons (with holes)
     * @param aFinalPolys: A reference to a SHAPE_POLY_SET buffer to store polygons with no holes
     * (holes are linked to main outline by overlapping segments, and these polygons are shrinked
     * by aZone->GetMinThickness() / 2 to be drawn with a outline thickness = aZone->GetMinThickness()
     * aFinalPolys are polygons that will be drawn on screen and plotted
     */
    bool fillSingleZone( const ZONE_CONTAINER* aZone,
            SHAPE_POLY_SET& aRawPolys,
            SHAPE_POLY_SET& aFinalPolys ) const;

    BOARD* m_board;
    COMMIT* m_commit;
    wxWindow* m_activeWindow;
    WX_PROGRESS_REPORTER* m_progressReporter;
};

#endif
