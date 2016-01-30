/**************************************************************/
/*  Lib component definitions (libentry) definition of fields */
/**************************************************************/

#ifndef CLASS_LIBENTRY_FIELDS_H
#define CLASS_LIBENTRY_FIELDS_H


#include "classes_body_items.h"


class LIB_FIELD;


typedef std::vector< LIB_FIELD > LIB_FIELD_LIST;


/* Fields , same as component fields.
 * can be defined in libraries (mandatory for ref and value, ca be useful for
 * footprints)
 * 2 Fields are always defined :
 *     Prefix (U, IC..) with gives the reference in schematic)
 *     Name (74LS00..) used to find the component in libraries, and give the
 * default value in schematic
 */

class LIB_FIELD : public LIB_DRAW_ITEM, public EDA_TextStruct
{
public:
    int m_FieldId;     /*  0 = REFERENCE
                        *  1 = VALUE
                        *  3 = FOOTPRINT (default Footprint)
                        *  4 = DOCUMENTATION (user doc link)
                        *  others = free fields
                        */
    wxString m_Name;   /* Field Name (not the field text itself, that is
                        * .m_Text) */

public:

    LIB_FIELD* Next() const { return (LIB_FIELD*) Pnext; }
    LIB_FIELD* Back() const { return (LIB_FIELD*) Pback; }


    LIB_FIELD( int idfield = 2 );
    LIB_FIELD( LIB_COMPONENT * aParent, int idfield = 2 );
    LIB_FIELD( const LIB_FIELD& field );
    ~LIB_FIELD();

    virtual wxString GetClass() const
    {
        return wxT( "LIB_FIELD" );
    }

    /**
     * Object constructor initialization helper.
     */
    void Init( int idfield );

    /** Function GetPenSize virtual pure
     * @return the size of the "pen" that be used to draw or plot this item
     */
    int GetPenSize( );

    /**
     * Writes field object out to a FILE in "*.lib" format.
     *
     * @param aFile The FILE to write to.
     * @return True if success writing else false.
     */
    virtual bool Save( FILE* aFile );
    virtual bool Load( char* line, wxString& errorMsg );

    /**
     * Copy parameters of this field to another field. Pointers are not copied.
     *
     * @param aTarget = Target field to copy values to.
     */
    void          Copy( LIB_FIELD* aTarget ) const;

    void          SetFields( const std::vector <LIB_FIELD> aFields );

    void Draw( WinEDA_DrawPanel * aPanel, wxDC * aDC, const wxPoint &aOffset,
               int aColor, int aDrawMode, void* aData,
               const int aTransformMatrix[2][2] );

    /** Function IsVisible
     * @return true is this field is visible, false if flagged invisible
     */
    bool IsVisible()
    {
        return (m_Attributs & TEXT_NO_VISIBLE) == 0 ? true : false;
    }

    /**
     * Return the bounding rectangle of the field text.
     * @return Bounding rectangle.
     */
    virtual EDA_Rect GetBoundingBox();

    /**
     * Test if the given point is within the bounds of this object.
     *
     * @param refPos A point to test in field coordinate system
     * @return True if a hit, else false
     */
    bool HitTest( const wxPoint& refPos );

     /**
      * @param aPosRef = a wxPoint to test
      * @param aThreshold = max distance to this object (usually the half
      *                     thickness of a line)
      * @param aTransMat = the transform matrix
      * @return True if the point aPosRef is near this object
      */
    virtual bool HitTest( wxPoint aPosRef, int aThreshold,
                          const int aTransMat[2][2] );

    void operator=( const LIB_FIELD& field )
    {
        m_FieldId = field.m_FieldId;
        m_Text = field.m_Text;
        m_Name = field.m_Name;
        m_Pos = field.m_Pos;
        m_Size = field.m_Size;
        m_Width = field.m_Width;
        m_Orient = field.m_Orient;
        m_Mirror = field.m_Mirror;
        m_Attributs = field.m_Attributs;
        m_Italic = field.m_Italic;
        m_Bold = field.m_Bold;
        m_HJustify = field.m_HJustify;
        m_VJustify = field.m_VJustify;
        m_Parent = field.m_Parent;
    }

    /**
     * Return the text of a field.
     *
     * If the field is the reference field, the unit number is used to
     * create a pseudo reference text.  If the base reference field is U,
     * the string U?A will be returned for unit = 1.
     *
     * @param unit - The package unit number.  Only effects reference field.
     * @return Field text.
     */
    wxString GetFullText( int unit = 1 );

protected:
    virtual LIB_DRAW_ITEM* DoGenCopy();

    /**
     * Provide the field draw object specific comparison.
     *
     * The sort order for field is as follows:
     *
     *      - Field ID, REFERENCE, VALUE, etc.
     *      - Field string, case insensitive compare.
     *      - Field horizontal (X) position.
     *      - Field vertical (Y) position.
     *      - Field width.
     *      - Field height.
     */
    virtual int DoCompare( const LIB_DRAW_ITEM& other ) const;

    virtual void DoOffset( const wxPoint& offset );
    virtual bool DoTestInside( EDA_Rect& rect );
    virtual void DoMove( const wxPoint& newPosition );
    virtual wxPoint DoGetPosition( void ) { return m_Pos; }
    virtual void DoMirrorHorizontal( const wxPoint& center );
    virtual void DoPlot( PLOTTER* plotter, const wxPoint& offset, bool fill,
                         const int transform[2][2] );
    virtual int DoGetWidth( void ) { return m_Width; }
    virtual void DoSetWidth( int width ) { m_Width = width; }
};

#endif  //  CLASS_LIBENTRY_FIELDS_H
