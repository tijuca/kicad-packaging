/**********************************/
/* class_pad.h : Pads description */
/**********************************/

class Pcb3D_GLCanvas;

#include "pad_shapes.h"
#include "PolyLine.h"

/* Default layers used for pads, according to the pad type.
 * this is default values only, they can be changed for a given pad
 */

// PAD_STANDARD:
#define PAD_STANDARD_DEFAULT_LAYERS ALL_CU_LAYERS | SILKSCREEN_LAYER_FRONT | SOLDERMASK_LAYER_BACK | \
    SOLDERMASK_LAYER_FRONT

// PAD_CONN:
#define PAD_CONN_DEFAULT_LAYERS LAYER_FRONT | SOLDERPASTE_LAYER_FRONT | SOLDERMASK_LAYER_FRONT

// PAD_SMD:
#define PAD_SMD_DEFAULT_LAYERS LAYER_FRONT | SOLDERMASK_LAYER_FRONT

//PAD_HOLE_NOT_PLATED:
#define PAD_HOLE_NOT_PLATED_DEFAULT_LAYERS LAYER_BACK | SILKSCREEN_LAYER_FRONT | \
    SOLDERMASK_LAYER_BACK | SOLDERMASK_LAYER_FRONT


class D_PAD : public BOARD_CONNECTED_ITEM
{
private:
    wxString m_Netname;         // Full net name like /mysheet/mysubsheet/vout used by eeschema
    wxString m_ShortNetname;    // short net name, like vout from /mysheet/mysubsheet/vout


public:
    wxPoint m_Pos;                  // pad Position on board

    union
    {
        unsigned long m_NumPadName;
        char          m_Padname[4]; /* Pad name (4 char) or a long identifier
                                     *  (used in pad name comparisons because  this is faster than string comparison)
                                     */
    };

    int m_Masque_Layer;             // Bitwise layer :1= copper layer, 15= cmp,
                                    // 2..14 = internal layers
                                    // 16 .. 31 = technical layers

    int    m_PadShape;              // Shape: PAD_CIRCLE, PAD_RECT, PAD_OVAL, PAD_TRAPEZOID
    int    m_DrillShape;            // Shape PAD_CIRCLE, PAD_OVAL

    wxSize m_Drill;                 // Drill diam (drill shape = PAD_CIRCLE) or drill size(shape = OVAL)
                                    // for drill shape = PAD_CIRCLE, drill diam = m_Drill.x

    wxSize m_Offset;    /* This parameter is useful only for oblong pads (it can be used for other
                         * shapes, but without any interest).
                         * this is the offset between the pad hole and the pad shape (you must
                         * understand here pad shape = copper area around the hole)
                         * Most of cases, the hole is the center of the shape (m_Offset = 0).
                         * But some board designers use oblong pads with a hole moved to one of the
                         * oblong pad shape ends.
                         * In all cases the pad position is the pad hole.
                         * The physical shape position (used to draw it for instance) is pad
                         * position (m_Pos) + m_Offset.
                         * D_PAD::ReturnShapePos() returns the physical shape position according to
                         * the offset and the pad rotation.*/

    wxSize  m_Size;                 // X and Y size ( relative to orient 0)

    wxSize  m_DeltaSize;            // delta on rectangular shapes

    wxPoint m_Pos0;                 // Initial Pad position (i.e. pas position relative to the module anchor, orientation 0

    int     m_Rayon;                // radius of pad circle
    int     m_Attribut;             // NORMAL, PAD_SMD, PAD_CONN
    int     m_Orient;               // in 1/10 degrees
    static int m_PadSketchModePenSize;      // Pen size used to draw pads in sketch mode
                                    // (mode used to print pads on silkscreen layer)

    // Local clearance. When null, the module default value is used.
    // when the module default value is null, the netclass value is used
    // Usually the local clearance is null
    int    m_LocalClearance;

    // Local mask margins: when NULL, the parent footprint design values are used
    int    m_LocalSolderMaskMargin;                             // Local solder mask margin
    int    m_LocalSolderPasteMargin;                            // Local solder paste margin absolute value
    double m_LocalSolderPasteMarginRatio;                       // Local solder mask margin ratio value of pad size
    // The final margin is the sum of these 2 values

private:
    int m_SubRatsnest;                  // variable used in rats nest computations
                                        // handle subnet (block) number in ratsnet connection

public:
    D_PAD( MODULE* parent );
    D_PAD( D_PAD* pad );
    ~D_PAD();

    void Copy( D_PAD* source );

    D_PAD* Next() { return (D_PAD*) Pnext; }


    /**
     * Function GetNetname
     * @return const wxString * , a pointer to the full netname
     */
    wxString GetNetname() const { return m_Netname; }

    /**
     * Function GetShortNetname
     * @return const wxString * , a pointer to the short netname
     */
    wxString GetShortNetname() const { return m_ShortNetname; }

    /**
     * Function SetNetname
     * @param const wxString : the new netname
     */
    void SetNetname( const wxString& aNetname );

    /**
     * Function GetShape
     * @return the shape of this pad.
     */
    int GetShape() { return m_PadShape & 0xFF;  }

    /**
     * Function GetPosition
     * returns the position of this object.
     * @return const wxPoint& - The position of this object.
     */
    wxPoint& GetPosition()
    {
        return m_Pos;
    }


    void SetPosition( const wxPoint& aPos )
    {
        m_Pos = aPos;
    }

    /** function TransformShapeWithClearanceToPolygon
     * Convert the pad shape to a closed polygon
     * Used in filling zones calculations
     * Circles and arcs are approximated by segments
     * @param aCornerBuffer = a buffer to store the polygon
     * @param aClearanceValue = the clearance around the pad
     * @param aCircleToSegmentsCount = the number of segments to approximate a circle
     * @param aCorrectionFactor = the correction to apply to circles radius to keep
     * clearance when the circle is approxiamted by segment bigger or equal
     * to the real clearance value (usually near from 1.0)
    */
    void TransformShapeWithClearanceToPolygon( std::vector <CPolyPt>& aCornerBuffer,
            int aClearanceValue, int aCircleToSegmentsCount, double aCorrectionFactor );

     /**
     * Function GetClearance
     * returns the clearance in 1/10000 inches.  If \a aItem is not NULL then the
     * returned clearance is the greater of this object's NETCLASS clearance and
     * aItem's NETCLASS clearance.  If \a aItem is NULL, then this objects clearance
     * is returned.
     * @param aItem is another BOARD_CONNECTED_ITEM or NULL
     * @return int - the clearance in 1/10000 inches.
     */
    virtual int GetClearance( BOARD_CONNECTED_ITEM* aItem = NULL ) const;

   // Mask margins handling:

    /** Function GetSolderMaskMargin
     * @return the margin for the solder mask layer
     * usually > 0 (mask shape bigger than pad
     * value is
     * 1 - the local value
     * 2 - if null, the parent footprint value
     * 1 - if null, the global value
     */
    int           GetSolderMaskMargin();

    /** Function GetSolderPasteMargin
     * @return the margin for the solder mask layer
     * usually < 0 (mask shape smaller than pad
     * because the margin can be dependent on the pad size, the margin has a x and a y value
     * value is
     * 1 - the local value
     * 2 - if null, the parent footprint value
     * 1 - if null, the global value
     */
    wxSize        GetSolderPasteMargin();

    /* Reading and writing data on files */
    int           ReadDescr( FILE* File, int* LineNum = NULL );

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool          Save( FILE* aFile ) const;


    /* drawing functions */
    void          Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                        int aDrawMode, const wxPoint& offset = ZeroOffset );

    void          Draw3D( Pcb3D_GLCanvas* glcanvas );

    // others
    void          SetPadName( const wxString& name );       // Change pad name
    wxString      ReturnStringPadName();                    // Return pad name as string in a wxString
    void          ReturnStringPadName( wxString& text );    // Return pad name as string in a buffer
    void          ComputeRayon();                           // compute radius
    const wxPoint ReturnShapePos();


    /**
     * Function GetNet
     * @return int - the netcode
     */
    int GetSubRatsnest() const { return m_SubRatsnest; }
    void SetSubRatsnest( int aSubRatsnest ) { m_SubRatsnest = aSubRatsnest; }


    /**
     * Function DisplayInfo
     * has knowledge about the frame and how and where to put status information
     * about this object into the frame's message panel.
     * Is virtual from EDA_BaseStruct.
     * @param frame A WinEDA_DrawFrame in which to print status information.
     */
    void DisplayInfo( WinEDA_DrawFrame* frame );


    /**
     * Function IsOnLayer
     * tests to see if this object is on the given layer.  Is virtual so
     * objects like D_PAD, which reside on multiple layers can do their own
     * form of testing.
     * @param aLayer The layer to test for.
     * @return bool - true if on given layer, else false.
     */
    bool IsOnLayer( int aLayer ) const;


    /**
     * Function HitTest
     * tests if the given wxPoint is within the bounds of this object.
     * @param refPos A wxPoint to test
     * @return bool - true if a hit, else false
     */
    bool HitTest( const wxPoint& refPos );

    /**
     * Function GetClass
     * returns the class name.
     * @return wxString
     */
    virtual wxString GetClass() const
    {
        return wxT( "PAD" );
    }


    /**
     * Function GetBoundingBox
     * returns the bounding box of this pad
     * Mainly used to redraw the screen area occupied by the pad
     */
    EDA_Rect   GetBoundingBox();

    /**
     * Function Compare
     * compares two pads and return 0 if they are equal.
     * @return int - <0 if left less than right, 0 if equal, >0 if left greater than right.
     */
    static int Compare( const D_PAD* padref, const D_PAD* padcmp );

    /**
     * Function Move
     * move this object.
     * @param const wxPoint& aMoveVector - the move vector for this object.
     */
    virtual void Move( const wxPoint& aMoveVector )
    {
        m_Pos += aMoveVector;
    }


#if defined(DEBUG)

    /**
     * Function Show
     * is used to output the object tree, currently for debugging only.
     * @param nestLevel An aid to prettier tree indenting, and is the level
     *          of nesting of this object within the overall tree.
     * @param os The ostream& to output to.
     */
    virtual void Show( int nestLevel, std::ostream& os );

#endif
};

typedef class D_PAD* LISTE_PAD;
