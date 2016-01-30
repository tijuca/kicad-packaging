/*********************************************************************/
/*  base_struct.h :  Basic classes for most kicad item descriptions  */
/*********************************************************************/

#ifndef BASE_STRUCT_H
#define BASE_STRUCT_H


#if defined (DEBUG)
#include <iostream>         // needed for Show()
extern std::ostream& operator   <<( std::ostream& out, const wxSize& size );

extern std::ostream& operator   <<( std::ostream& out, const wxPoint& pt );

#endif


/* Id for class identification, at run time */
enum KICAD_T {
    NOT_USED = -1,          // the 3d code uses this value

    EOT = 0,                // search types array terminator (End Of Types)

    TYPE_NOT_INIT = 0,
    TYPEPCB,

    // Items in pcb
    PCB_EQUIPOT_STRUCT_TYPE,
    TYPEMODULE,
    TYPEPAD,
    TYPEDRAWSEGMENT,
    TYPETEXTE,
    TYPETEXTEMODULE,
    TYPEEDGEMODULE,
    TYPETRACK,
    TYPEZONE,
    TYPEVIA,
    TYPEMARKER,
    TYPECOTATION,
    TYPEMIRE,
    TYPESCREEN,
    TYPEBLOCK,
    TYPEZONE_UNUSED,
    TYPEZONE_EDGE_CORNER,
    TYPEZONE_CONTAINER,

    // Draw Items in schematic
    DRAW_POLYLINE_STRUCT_TYPE,
    DRAW_JUNCTION_STRUCT_TYPE,
    TYPE_SCH_TEXT,
    TYPE_SCH_LABEL,
    TYPE_SCH_GLOBALLABEL,
    TYPE_SCH_HIERLABEL,
    TYPE_SCH_COMPONENT,
    DRAW_PICK_ITEM_STRUCT_TYPE,
    DRAW_SEGMENT_STRUCT_TYPE,
    DRAW_BUSENTRY_STRUCT_TYPE,
    DRAW_SHEET_STRUCT_TYPE,
    DRAW_HIERARCHICAL_PIN_SHEET_STRUCT_TYPE,
    DRAW_MARKER_STRUCT_TYPE,
    DRAW_NOCONNECT_STRUCT_TYPE,
    DRAW_PART_TEXT_STRUCT_TYPE,

    // General
    SCREEN_STRUCT_TYPE,
    BLOCK_LOCATE_STRUCT_TYPE,

    // Draw Items in library component
    LIBCOMPONENT_STRUCT_TYPE,
    COMPONENT_ARC_DRAW_TYPE,
    COMPONENT_CIRCLE_DRAW_TYPE,
    COMPONENT_GRAPHIC_TEXT_DRAW_TYPE,
    COMPONENT_RECT_DRAW_TYPE,
    COMPONENT_POLYLINE_DRAW_TYPE,
    COMPONENT_LINE_DRAW_TYPE,
    COMPONENT_PIN_DRAW_TYPE,
    COMPONENT_FIELD_DRAW_TYPE,

    // End value
    MAX_STRUCT_TYPE_ID
};


enum SEARCH_RESULT {
    SEARCH_QUIT,
    SEARCH_CONTINUE
};


class EDA_BaseStruct;
class WinEDA_DrawFrame;
class BOARD;
class EDA_Rect;

/**
 * Class INSPECTOR
 * is an abstract class that is used to inspect and possibly collect the
 * (search) results of Iterating over a list or tree of KICAD_T objects.
 * Extend from this class and implement the Inspect function and provide for
 * a way for the extension to collect the results of the search/scan data and
 * provide them to the caller.
 */
class INSPECTOR
{
public:
    virtual ~INSPECTOR()
    {
    }


    /**
     * Function Inspect
     * is the examining function within the INSPECTOR which is passed to the
     * Iterate function.  It is used primarily for searching, but not limited to
     * that.  It can also collect or modify the scanned objects.
     *
     * @param testItem An EDA_BaseStruct to examine.
     * @param testData is arbitrary data needed by the inspector to determine
     *   if the BOARD_ITEM under test meets its match criteria.
     * @return SEARCH_RESULT - SEARCH_QUIT if the Iterator is to stop the scan,
     *   else SCAN_CONTINUE;
     */
    SEARCH_RESULT virtual Inspect( EDA_BaseStruct * testItem,
                                   const void* testData ) = 0;
};



/**
 * Class EDA_Rect
 * handles the component boundary box.
 * This class is similar to wxRect, but some wxRect functions are very curious,
 * so I prefer this suitable class
 */
class EDA_Rect
{
public:
    wxPoint m_Pos;      // Rectangle Origin
    wxSize  m_Size;     // Rectangle Size

public:
    EDA_Rect() { };

    EDA_Rect( const wxPoint& aPos, const wxSize& aSize ) :
            m_Pos( aPos ), m_Size( aSize )
    {}

    wxPoint Centre()
    {
        return wxPoint( m_Pos.x + (m_Size.x >> 1), m_Pos.y + (m_Size.y >> 1) );
    }


    void    Normalize();                    // Ensure the height and width are >= 0
    bool    Inside( const wxPoint& point ); // Return TRUE if point is in Rect

    bool Inside( int x, int y ) { return Inside( wxPoint( x, y ) ); }
    wxSize GetSize() { return m_Size; }
    int GetX() { return m_Pos.x; }
    int GetY() { return m_Pos.y; }
    wxPoint GetOrigin() { return m_Pos; }
    wxPoint GetPosition() { return m_Pos; }
    wxPoint GetEnd() { return wxPoint( GetRight(), GetBottom() ); }
    int GetWidth() { return m_Size.x; }
    int GetHeight() { return m_Size.y; }
    int GetRight() { return m_Pos.x + m_Size.x; }
    int GetBottom() { return m_Pos.y + m_Size.y; }
    void SetOrigin( const wxPoint& pos ) { m_Pos = pos; }
    void SetOrigin( int x, int y ) { m_Pos.x = x; m_Pos.y = y; }
    void SetSize( const wxSize& size ) { m_Size = size; }
    void SetSize( int w, int h ) { m_Size.x = w; m_Size.y = h; }
    void Offset( int dx, int dy ) { m_Pos.x += dx; m_Pos.y += dy; }
    void Offset( const wxPoint& offset ) { m_Pos.x += offset.x; m_Pos.y += offset.y; }
    void SetX( int val ) { m_Pos.x = val; }
    void SetY( int val ) { m_Pos.y = val; }
    void SetWidth( int val ) { m_Size.x = val; }
    void SetHeight( int val ) { m_Size.y = val; }
    void SetEnd( const wxPoint& pos )
    {
        m_Size.x = pos.x - m_Pos.x; m_Size.y = pos.y - m_Pos.y;
    }

    /**
     * Function Intersects
     * @return bool - true if the argument rectangle intersects this rectangle.
     */
    bool Intersects( const EDA_Rect aRect ) const;


    /**
     * Function operator(wxRect)
     * overloads the cast operator to return a wxRect
     */
    operator wxRect() const { return wxRect( m_Pos, m_Size ); }

    EDA_Rect& Inflate( wxCoord dx, wxCoord dy );

    /** Function Merge
     * Modify Position and Size of this in order to contain the given rect
     * mainly used to calculate bounding boxes
     * @param aRect = given rect to merge with this
    */
    void Merge( const EDA_Rect & aRect );

};


/******************************************************/
/* Basic Classes : used classes are derived from them */
/******************************************************/

/** class EDA_BaseStruct
 * Basic class, not directly used.
 * All the kicad classes used to describe a shematic or a board item are derived from.
 */
class EDA_BaseStruct
{
private:

    /**
     * Run time identification, _keep private_ so it can never be changed after
     * a constructor sets it.  See comment near SetType() regarding virtual functions.
     */
    KICAD_T         m_StructType;

public:
    EDA_BaseStruct* Pnext;              /* Linked list: Link (next struct) */
    EDA_BaseStruct* Pback;              /* Linked list: Link (previous struct) */
    EDA_BaseStruct* m_Parent;           /* Linked list: Link (parent struct) */
    EDA_BaseStruct* m_Son;              /* Linked list: Link (son struct) */
    EDA_BaseStruct* m_Image;            /* Link to an image copy for undelete or abort command */

    int             m_Flags;            // flags for editing and other misc. uses
#define IS_CHANGED      (1 << 0)
#define IS_LINKED       (1 << 1)
#define IN_EDIT         (1 << 2)
#define IS_MOVED        (1 << 3)
#define IS_NEW          (1 << 4)
#define IS_RESIZED      (1 << 5)
#define IS_DRAGGED      (1 << 6)
#define IS_DELETED      (1 << 7)
#define IS_WIRE_IMAGE   (1 << 8)
#define STARTPOINT      (1 << 9)
#define ENDPOINT        (1 << 10)
#define SELECTED        (1 << 11)
#define SELECTEDNODE    (1 << 12)           ///< flag indiquant que la structure a deja selectionnee
#define STRUCT_DELETED  (1 << 13)           ///< Bit flag de Status pour structures effacee
#define CANDIDATE       (1 << 14)           ///< flag indiquant que la structure est connectee
#define SKIP_STRUCT     (1 << 15)           ///< flag indiquant que la structure ne doit pas etre traitee
#define DO_NOT_DRAW     (1 << 16)           ///< Used to disable draw function

    unsigned long m_TimeStamp;          // Time stamp used for logical links
    int           m_Selected;           /* Used by block commands, and selective editing */

private:
    int           m_Status;

private:
    void InitVars();


public:

    EDA_BaseStruct( EDA_BaseStruct* parent, KICAD_T idType );
    EDA_BaseStruct( KICAD_T idType );
    virtual ~EDA_BaseStruct() { };

    /**
     * Function Type
     * returns the type of object.  This attribute should never be changed after
     * a constructor sets it, so there is no public "setter" method.
     * @return KICAD_T - the type of object.
     */
    KICAD_T Type()  const { return m_StructType; }


    EDA_BaseStruct* Next() const { return (EDA_BaseStruct*) Pnext; }
    EDA_BaseStruct* Back() const { return (EDA_BaseStruct*) Pback; }
    EDA_BaseStruct* GetParent() const { return (EDA_BaseStruct*) m_Parent; }


    /* Gestion de l'etat (status) de la structure (active, deleted..) */

    int GetState( int type ) const
    {
        return m_Status & type;
    }


    void SetState( int type, int state )
    {
        if( state )
            m_Status |= type; // state = ON or OFF
        else
            m_Status &= ~type;
    }


    int ReturnStatus() const {  return m_Status;  }

    void SetStatus( int new_status )
    {
        m_Status = new_status;
    }

    /**
     * Function Display_Infos
     * has knowledge about the frame and how and where to put status information
     * about this object into the frame's message panel.
     * @param frame A WinEDA_DrawFrame in which to print status information.
     */
    virtual void    Display_Infos( WinEDA_DrawFrame* frame )
    {
        // derived classes may implement this
    }


    /**
     * Function HitTest
     * tests if the given wxPoint is within the bounds of this object.
     * @param refPos A wxPoint to test
     * @return bool - true if a hit, else false
     */
    virtual bool    HitTest( const wxPoint& refPos )
    {
        return false;   // derived classes should override this function
    }

    /**
     * Function HitTest (overlayed)
     * tests if the given EDA_Rect intersect this object.
     * For now, an ending point must be inside this rect.
     * @param refArea : the given EDA_Rect
     * @return bool - true if a hit, else false
     */
    virtual bool    HitTest( EDA_Rect& refArea )
    {
        return false;   // derived classes should override this function
    }

    /**
     * Function GetBoundingBox
     * returns the orthogonal, bounding box of this object for display purposes.
     * This box should be an enclosing perimeter for visible components of this
     * object, and the units should be in the pcb or schematic coordinate system.
     * It is OK to overestimate the size by a few counts.
     */
    virtual EDA_Rect GetBoundingBox()
    {
#if defined (DEBUG)
        printf("Missing GetBoundingBox()\n");
        Show( 0, std::cout ); // tell me which classes still need GetBoundingBox support
#endif
        // return a zero-sized box per default. derived classes should override this
        EDA_Rect ret( wxPoint( 0, 0 ), wxSize( 0, 0 ) );
        return ret;
    }

    /**
     * Function IterateForward
     * walks through the object tree calling the inspector() on each object
     * type requested in scanTypes.
     *
     * @param listStart The first in a list of EDA_BaseStructs to iterate over.
     * @param inspector Is an INSPECTOR to call on each object that is one of
     *  the requested scanTypes.
     * @param testData Is an aid to testFunc, and should be sufficient to
     *  allow it to fully determine if an item meets the match criteria, but it
     *  may also be used to collect output.
     * @param scanTypes A KICAD_T array that is EOT
     *  terminated, and provides both the order and interest level of of
     *  the types of objects to be iterated over.
     * @return SEARCH_RESULT - SEARCH_QUIT if the called INSPECTOR returned
     *  SEARCH_QUIT, else SCAN_CONTINUE;
     */
    static SEARCH_RESULT    IterateForward( EDA_BaseStruct* listStart,
                                            INSPECTOR*      inspector,
                                            const void*     testData,
                                            const KICAD_T   scanTypes[] );


    /**
     * Function Visit
     * may be re-implemented for each derived class in order to handle
     * all the types given by its member data.  Implementations should call
     * inspector->Inspect() on types in scanTypes[], and may use IterateForward()
     * to do so on lists of such data.
     * @param inspector An INSPECTOR instance to use in the inspection.
     * @param testData Arbitrary data used by the inspector.
     * @param scanTypes Which KICAD_T types are of interest and the order
     *  is significant too, terminated by EOT.
     * @return SEARCH_RESULT - SEARCH_QUIT if the Iterator is to stop the scan,
     *  else SCAN_CONTINUE, and determined by the inspector.
     */
    virtual SEARCH_RESULT   Visit( INSPECTOR* inspector, const void* testData,
                                   const KICAD_T scanTypes[] );


    /**
     * Function GetClass
     * returns the class name.
     * @return wxString
     */
    virtual wxString GetClass() const
    {
        return wxT( "EDA_BaseStruct" );
    }


    /**
     * Function DeleteStructList
     * deletes each item in a linked list of EDA_BaseStructs, starting with
     * "this" object.
     */
    void DeleteStructList();


    /**
     * Function AddToChain
     * adds this item just after laststruct in a linked list established
     * by the Prev and Back pointers of my base EDA_BaseStruct.
     * @deprecated
     * @param laststruct The item to add after
     */
    void AddToChain( EDA_BaseStruct* laststruct );


#if defined (DEBUG)

    /**
     * Function Show
     * is used to output the object tree, currently for debugging only.
     * @param nestLevel An aid to prettier tree indenting, and is the level
     *          of nesting of this object within the overall tree.
     * @param os The ostream& to output to.
     */
    virtual void            Show( int nestLevel, std::ostream& os );


    /**
     * Function NestedSpace
     * outputs nested space for pretty indenting.
     * @param nestLevel The nest count
     * @param os The ostream&, where to output
     * @return std::ostream& - for continuation.
     **/
    static std::ostream&    NestedSpace( int nestLevel, std::ostream& os );

#endif
};


// Text justify:
// Values -1,0,1 are used in computations, do not change them
typedef enum {
    GR_TEXT_HJUSTIFY_LEFT   = -1,
    GR_TEXT_HJUSTIFY_CENTER = 0,
    GR_TEXT_HJUSTIFY_RIGHT  = 1
} GRTextHorizJustifyType;


typedef enum {
    GR_TEXT_VJUSTIFY_TOP    = -1,
    GR_TEXT_VJUSTIFY_CENTER = 0,
    GR_TEXT_VJUSTIFY_BOTTOM = 1
} GRTextVertJustifyType;


/* controle des remplissages a l'ecran (Segments textes...)*/
#define FILAIRE  0
#define FILLED   1
#define SKETCH   2


#define DEFAULT_SIZE_TEXT 60        /* default text height (in mils or  1/1000") */

/** class EDA_TextStruct
 * basic class to handle texts (labels, texts on components or footprints ..)
 * not used directly.
 * the text classes are derived from EDA_BaseStruct and EDA_TextStruct
 */
class EDA_TextStruct
{
public:
    wxString m_Text;                    /* text! */
    wxPoint  m_Pos;                     /* XY position of anchor text. */
    wxSize   m_Size;                    /* XY size of text */
    int      m_Width;                   /* text width */
    int      m_Orient;                  /* Orient in 0.1 degrees */
    int      m_Miroir;                  // Display Normal / mirror
    int      m_Attributs;               /* flags (visible...) */
    int      m_CharType;                /* normal, bold, italic ... */
    int      m_HJustify, m_VJustify;    /* Horiz and  Vert Justifications */
    int      m_ZoomLevelDrawable;       /* zoom level to draw text.
                                          * if zoom < m_ZoomLevelDrawable: the text is drawn as a single line */
    int*     m_TextDrawings;            /* list of segments to draw, for the Draw function */
    int      m_TextDrawingsSize;        /* segment count */

public:
    EDA_TextStruct( const wxString& text = wxEmptyString );
    virtual ~EDA_TextStruct();
    void    CreateDrawData();

    int     GetLength() const { return m_Text.Length(); };

    /** Function Pitch()
     * @return distance between 2 caracteres
     */
    int     Pitch();
    void    Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                  const wxPoint& offset, int color,
                  int draw_mode, int display_mode = FILAIRE, int anchor_color = -1 );

    /**
     * Function HitTest
     * tests if the given wxPoint is within the bounds of this object.
     * @param ref_pos A wxPoint to test
     * @return bool - true if a hit, else false
     */
    bool    HitTest( const wxPoint& ref_pos );

    /**
     * Function HitTest (overlayed)
     * tests if the given EDA_Rect intersect this object.
     * For now, the anchor must be inside this rect.
     * @param refArea : the given EDA_Rect
     * @return bool - true if a hit, else false
     */
    bool    HitTest( EDA_Rect& refArea );

    /**
     * Function Len_Size
     * Return the text lenght in internal units
     */
    int     Len_Size();
};

#endif /* BASE_STRUCT_H */
