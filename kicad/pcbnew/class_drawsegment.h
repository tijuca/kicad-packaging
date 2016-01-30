/*************************************/
/* class to handle a graphic segment */
/**************************************/

#ifndef CLASS_DRAWSEGMENT_H
#define CLASS_DRAWSEGMENT_H
#include "PolyLine.h"

class DRAWSEGMENT : public BOARD_ITEM
{
public:
    int     m_Width;            // thickness of lines ...
    wxPoint m_Start;            // Line start point
    wxPoint m_End;              // Line end point

    int     m_Shape;            // Shape: line, Circle, Arc
    int     m_Type;             // Used in complex associations ( Dimensions.. )
    int     m_Angle;            // Used only for Arcs: Arc angle in 1/10 deg
    wxPoint m_BezierC1;         // Bezier Control Point 1
    wxPoint m_BezierC2;         // Bezier Control Point 1

    std::vector<wxPoint> m_BezierPoints;
public:
    DRAWSEGMENT( BOARD_ITEM* aParent, KICAD_T idtype = TYPE_DRAWSEGMENT );
    ~DRAWSEGMENT();

    DRAWSEGMENT* Next() const { return (DRAWSEGMENT*) Pnext; }
    DRAWSEGMENT* Back() const { return (DRAWSEGMENT*) Pback; }

    /**
     * Function GetPosition
     * returns the position of this object.
     * Required by pure virtual BOARD_ITEM::GetPosition()
     * @return const wxPoint& - The position of this object.
     */
    wxPoint& GetPosition()
    {
        return m_Start;
    }


    /**
     * Function GetStart
     * returns the starting point of the graphic
     */
    wxPoint      GetStart() const;

    /**
     * Function GetEnd
     * returns the ending point of the graphic
     */
    wxPoint      GetEnd() const;

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool         Save( FILE* aFile ) const;

    bool         ReadDrawSegmentDescr( FILE* File, int* LineNum );

    void         Copy( DRAWSEGMENT* source );


    void         Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                       int aDrawMode, const wxPoint& offset = ZeroOffset );

    /**
     * Function DisplayInfo
     * has knowledge about the frame and how and where to put status information
     * about this object into the frame's message panel.
     * Is virtual from EDA_BaseStruct.
     * @param frame A WinEDA_BasePcbFrame in which to print status information.
     */
    virtual void DisplayInfo( WinEDA_DrawFrame* frame );


    /**
     * Function HitTest
     * tests if the given wxPoint is within the bounds of this object.
     * @param ref_pos A wxPoint to test
     * @return bool - true if a hit, else false
     */
    bool         HitTest( const wxPoint& ref_pos );

    /**
     * Function HitTest (overlayed)
     * tests if the given EDA_Rect intersect this object.
     * For now, an ending point must be inside this rect.
     * @param refPos the given EDA_Rect to test
     * @return bool - true if a hit, else false
     */
    bool         HitTest( EDA_Rect& refArea );

    /**
     * Function GetClass
     * returns the class name.
     * @return wxString
     */
    wxString GetClass() const
    {
        return wxT( "DRAWSEGMENT" );
    }


    /**
     * Function GetLength
     * returns the length of the track using the hypotenuse calculation.
     * @return double - the length of the track
     */
    double  GetLength() const
    {
        wxPoint delta = GetEnd() - GetStart();

        return hypot( delta.x, delta.y );
    }


    /**
     * Function Move
     * move this object.
     * @param const wxPoint& aMoveVector - the move vector for this object.
     */
    virtual void Move( const wxPoint& aMoveVector )
    {
        m_Start += aMoveVector;
        m_End   += aMoveVector;
    }


    /**
     * Function Rotate
     * Rotate this object.
     * @param const wxPoint& aRotCentre - the rotation point.
     * @param aAngle - the rotation angle in 0.1 degree.
     */
    virtual void Rotate( const wxPoint& aRotCentre, int aAngle );

    /**
     * Function Flip
     * Flip this object, i.e. change the board side for this object
     * @param const wxPoint& aCentre - the rotation point.
     */
    virtual void Flip( const wxPoint& aCentre );

    /** Function TransformShapeWithClearanceToPolygon
     * Convert the track shape to a closed polygon
     * Used in filling zones calculations
     * Circles and arcs are approximated by segments
     * @param aCornerBuffer = a buffer to store the polygon
     * @param aClearanceValue = the clearance around the pad
     * @param aCircleToSegmentsCount = the number of segments to approximate a circle
     * @param aCorrectionFactor = the correction to apply to circles radius to keep
     * clearance when the circle is approxiamted by segment bigger or equal
     * to the real clearance value (usually near from 1.0)
     */
    void         TransformShapeWithClearanceToPolygon(
        std::vector <CPolyPt>& aCornerBuffer,
        int                    aClearanceValue,
        int
                               aCircleToSegmentsCount,
        double                 aCorrectionFactor );

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os );

#endif
};


#endif      // #ifndef CLASS_DRAWSEGMENT_H
