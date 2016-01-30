/////////////////////////////////////////////////////////////////////////////

// Name:        polygon_test_point_inside.cpp
/////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <vector>
#include "PolyLine.h"

using namespace std;

/* this algo uses the the Jordan curve theorem to find if a point is inside or outside a polygon:
 * It run a semi-infinite line horizontally (increasing x, fixed y)
 * out from the test point, and count how many edges it crosses.
 * At each crossing, the ray switches between inside and outside.
 * If odd count, the test point is inside the polygon
 * This is called the Jordan curve theorem, or sometimes referred to as the "even-odd" test.
 * Take care to starting and ending points of segments outlines, when the horizontal line crosses a segment outline 
 * exactly on an ending point:
 * Because the starting point of a segment is also the ending point of the previous, only one must be used.
 * And we do no use twice the same segment, so we do NOT use both starting and ending points of these 2 segments.
 * So we must use only one ending point of each segment when calculating intersections
 * but it cannot be always the starting or the endind point. This depend on relative position of 2 consectutive segments
 * Here, the ending point above the Y reference position is used
 *   and the ending point below or equal the Y reference position is NOT used
 * Obviously, others cases are irrelevant because there is not intersection.
 */

#define OUTSIDE false
#define INSIDE true

bool TestPointInsidePolygon( std::vector <CPolyPt> aPolysList,
                             int istart, int iend, int refx, int refy )

/** Function TestPointInsidePolygon
 * test if a point is inside or outside a polygon.
 * the polygon must have only lines (not arcs) for outlines.
 * Use TestPointInside or TestPointInsideContour for more complex polygons
 * @param aPolysList: the list of polygons
 * @param istart: the starting point of a given polygon in m_FilledPolysList.
 * @param iend: the ending point of the polygon in m_FilledPolysList.
 * @param refx,refy: the point coordinate to test
 * @return true if the point is inside, false for outside
 */
{
    // count intersection points to right of (refx,refy). If odd number, point (refx,refy) is inside polyline
    int ics, ice;
    int count = 0;

    // find all intersection points of line with polyline sides
    for( ics = istart, ice = iend; ics <= iend; ice = ics++ )
    {
        int seg_startX = aPolysList[ics].x;
        int seg_startY = aPolysList[ics].y;
        int seg_endX   = aPolysList[ice].x;
        int seg_endY   = aPolysList[ice].y;

        /* Trivial cases: skip if ref above or below the segment to test */
        if( ( seg_startY > refy ) && (seg_endY > refy ) )
            continue;

        // segment below ref point, or one of its ends has the same Y pos as the ref point: skip
        // So we eliminate one end point of 2 consecutive segments.
        // Note: also we skip horizontal segments if ref point is on this horizontal line
        // So reference points on horizontal segments outlines always are seen as outside the polygon
        if( ( seg_startY <= refy ) && (seg_endY <= refy ) )
            continue;

        /* refy is between seg_startY and seg_endY.
         * note: here: horizontal segments (seg_startY == seg_endY) are skipped,
         * either by the first test or by the second test
         * see if an horizontal semi infinite line from refx is intersecting the segment
         */

        // calculate the x position of the intersection of this segment and the semi infinite line
        // this is more easier if we move the X,Y axis origin to the segment start point:
        seg_endX -= seg_startX;
        seg_endY -= seg_startY;
        double newrefx = (double) (refx - seg_startX);
        double newrefy = (double) (refy - seg_startY);

        // Now calculate the x intersection coordinate of the line from (0,0) to (seg_endX,seg_endY)
        // with the horizontal line at the new refy position
        // the line slope  = seg_endY/seg_endX;
        // and the x pos relative to the new origin is intersec_x = refy/slope
        // Note: because horizontal segments are skipped, 1/slope exists (seg_end_y never == O)
        double intersec_x = newrefy * seg_endX / seg_endY;
        if( newrefx < intersec_x )  // Intersection found with the semi-infinite line from refx to infinite
            count++;
    }

    return count & 1 ? INSIDE : OUTSIDE;
}
