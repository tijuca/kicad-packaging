/***************************/
/* lib_text.cpp */
/***************************/

/**
* class LIB_TEXT : describes a graphic text used to draw component shapes
* This is only a graphic item
*/

#include "fctsys.h"
#include "gr_basic.h"
#include "common.h"
#include "macros.h"
#include "class_drawpanel.h"
#include "plot_common.h"
#include "drawtxt.h"
#include "trigo.h"
#include "wxstruct.h"

#include "lib_draw_item.h"
#include "general.h"
#include "protos.h"
#include "transform.h"
#include "lib_text.h"


LIB_TEXT::LIB_TEXT(LIB_COMPONENT * aParent) :
    LIB_ITEM( LIB_TEXT_T, aParent ),
    EDA_TEXT()
{
    m_Size       = wxSize( 50, 50 );
    m_typeName   = _( "Text" );
    m_rotate     = false;
    m_updateText = false;
}


bool LIB_TEXT::Save( FILE* ExportFile )
{
    wxString text = m_Text;

    if( text.Contains( wxT( "~" ) ) || text.Contains( wxT( "\"" ) ) )
    {
        // convert double quote to similar-looking two apostrophes
        text.Replace( wxT( "\"" ), wxT( "''" ) );
        text = wxT( "\"" ) + text + wxT( "\"" );
    }
    else
    {
        // Spaces are not allowed in text because it is not double quoted:
        // changed to '~'
        text.Replace( wxT( " " ), wxT( "~" ) );
    }

    if( fprintf( ExportFile, "T %d %d %d %d %d %d %d %s ", m_Orient, m_Pos.x, m_Pos.y,
                 m_Size.x, m_Attributs, m_Unit, m_Convert, TO_UTF8( text ) ) < 0 )
        return false;
    if( fprintf( ExportFile, " %s %d", m_Italic ? "Italic" : "Normal",
                 ( m_Bold > 0 ) ? 1 : 0 ) < 0 )
        return false;

    char hjustify = 'C';
    if( m_HJustify == GR_TEXT_HJUSTIFY_LEFT )
        hjustify = 'L';
    else if( m_HJustify == GR_TEXT_HJUSTIFY_RIGHT )
        hjustify = 'R';

    char vjustify = 'C';
    if( m_VJustify == GR_TEXT_VJUSTIFY_BOTTOM )
        vjustify = 'B';
    else if( m_VJustify == GR_TEXT_VJUSTIFY_TOP )
        vjustify = 'T';

    if( fprintf( ExportFile, " %c %c\n", hjustify, vjustify ) < 0 )
        return false;

    return true;
}


bool LIB_TEXT::Load( char* line, wxString& errorMsg )
{
    int  cnt, thickness;
    char hjustify = 'C', vjustify = 'C';
    char buf[256];
    char tmp[256];

    buf[0] = 0;
    tmp[0] = 0;         // For italic option, Not in old versions

    cnt = sscanf( &line[2], "%d %d %d %d %d %d %d \"%[^\"]\" %s %d %c %c",
                  &m_Orient, &m_Pos.x, &m_Pos.y, &m_Size.x, &m_Attributs,
                  &m_Unit, &m_Convert, buf, tmp, &thickness, &hjustify,
                  &vjustify );


    if( cnt >= 8 ) // if quoted loadng failed, load as not quoted
    {
        m_Text = FROM_UTF8( buf );
        // convert two apostrophes back to double quote
        m_Text.Replace( wxT( "''" ), wxT( "\"" ) );
    }
    else
    {
    cnt = sscanf( &line[2], "%d %d %d %d %d %d %d %s %s %d %c %c",
                  &m_Orient, &m_Pos.x, &m_Pos.y, &m_Size.x, &m_Attributs,
                  &m_Unit, &m_Convert, buf, tmp, &thickness, &hjustify,
                  &vjustify );

        if( cnt < 8 )
        {
            errorMsg.Printf( _( "text only had %d parameters of the required 8" ), cnt );
            return false;
        }

        /* Convert '~' to spaces (only if text is not quoted). */
        m_Text = FROM_UTF8( buf );
        m_Text.Replace( wxT( "~" ), wxT( " " ) );
    }
    m_Size.y = m_Size.x;

    if( strnicmp( tmp, "Italic", 6 ) == 0 )
        m_Italic = true;
    if( thickness > 0 )
    {
        m_Bold = true;
    }

    switch( hjustify )
    {
    case 'L':
        m_HJustify = GR_TEXT_HJUSTIFY_LEFT;
        break;

    case 'C':
        m_HJustify = GR_TEXT_HJUSTIFY_CENTER;
        break;

    case 'R':
        m_HJustify = GR_TEXT_HJUSTIFY_RIGHT;
        break;
    }

    switch( vjustify )
    {
    case 'T':
        m_VJustify = GR_TEXT_VJUSTIFY_TOP;
        break;

    case 'C':
        m_VJustify = GR_TEXT_VJUSTIFY_CENTER;
        break;

    case 'B':
        m_VJustify = GR_TEXT_VJUSTIFY_BOTTOM;
        break;
    }


    return true;
}


bool LIB_TEXT::HitTest( const wxPoint& aPosition )
{
    return HitTest( aPosition, 0, DefaultTransform );
}


bool LIB_TEXT::HitTest( wxPoint aPosition, int aThreshold, const TRANSFORM& aTransform )
{
    if( aThreshold < 0 )
        aThreshold = 0;

    wxPoint physicalpos = aTransform.TransformCoordinate( m_Pos );
    wxPoint tmp = m_Pos;
    m_Pos = physicalpos;

    /* The text orientation may need to be flipped if the
     *  transformation matrix causes xy axes to be flipped.
     * this simple algo works only for schematic matrix (rot 90 or/and mirror)
     */
    int t1 = ( aTransform.x1 != 0 ) ^ ( m_Orient != 0 );
    int orient = t1 ? TEXT_ORIENT_HORIZ : TEXT_ORIENT_VERT;
    EXCHG( m_Orient, orient );
    bool hit = TextHitTest( aPosition );
    EXCHG( m_Orient, orient );
    m_Pos = tmp;
    return hit;
}


EDA_ITEM* LIB_TEXT::doClone() const
{
    LIB_TEXT* newitem = new LIB_TEXT(NULL);

    newitem->m_Pos       = m_Pos;
    newitem->m_Orient    = m_Orient;
    newitem->m_Size      = m_Size;
    newitem->m_Attributs = m_Attributs;
    newitem->m_Unit      = m_Unit;
    newitem->m_Convert   = m_Convert;
    newitem->m_Flags     = m_Flags;
    newitem->m_Text      = m_Text;
    newitem->m_Thickness     = m_Thickness;
    newitem->m_Italic    = m_Italic;
    newitem->m_Bold      = m_Bold;
    newitem->m_HJustify  = m_HJustify;
    newitem->m_VJustify  = m_VJustify;
    return (EDA_ITEM*) newitem;
}


int LIB_TEXT::DoCompare( const LIB_ITEM& other ) const
{
    wxASSERT( other.Type() == LIB_TEXT_T );

    const LIB_TEXT* tmp = ( LIB_TEXT* ) &other;

    int result = m_Text.CmpNoCase( tmp->m_Text );

    if( result != 0 )
        return result;

    if( m_Pos.x != tmp->m_Pos.x )
        return m_Pos.x - tmp->m_Pos.x;

    if( m_Pos.y != tmp->m_Pos.y )
        return m_Pos.y - tmp->m_Pos.y;

    if( m_Size.x != tmp->m_Size.x )
        return m_Size.x - tmp->m_Size.x;

    if( m_Size.y != tmp->m_Size.y )
        return m_Size.y - tmp->m_Size.y;

    return 0;
}


void LIB_TEXT::DoOffset( const wxPoint& offset )
{
    m_Pos += offset;
}


bool LIB_TEXT::DoTestInside( EDA_RECT& rect ) const
{
    /*
     * FIXME: This should calculate the text size and justification and
     *        use rectangle instect.
     */
    return rect.Contains( m_Pos.x, -m_Pos.y );
}


void LIB_TEXT::DoMove( const wxPoint& newPosition )
{
    m_Pos = newPosition;
}


void LIB_TEXT::DoMirrorHorizontal( const wxPoint& center )
{
    m_Pos.x -= center.x;
    m_Pos.x *= -1;
    m_Pos.x += center.x;
}

void LIB_TEXT::DoMirrorVertical( const wxPoint& center )
{
    m_Pos.y -= center.y;
    m_Pos.y *= -1;
    m_Pos.y += center.y;
}

void LIB_TEXT::DoRotate( const wxPoint& center, bool aRotateCCW )
{
    int rot_angle = aRotateCCW ? -900 : 900;

    RotatePoint( &m_Pos, center, rot_angle );
    m_Orient = m_Orient ? 0 : 900;
}


void LIB_TEXT::DoPlot( PLOTTER* plotter, const wxPoint& offset, bool fill,
                       const TRANSFORM& aTransform )
{
    wxASSERT( plotter != NULL );

    /* The text orientation may need to be flipped if the
     * transformation matrix causes xy axes to be flipped. */
    int t1  = ( aTransform.x1 != 0 ) ^ ( m_Orient != 0 );
    wxPoint pos = aTransform.TransformCoordinate( m_Pos ) + offset;

    plotter->text( pos, UNSPECIFIED_COLOR, m_Text,
                   t1 ? TEXT_ORIENT_HORIZ : TEXT_ORIENT_VERT,
                   m_Size, GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_CENTER,
                   GetPenSize(), m_Italic, m_Bold );
}


/**
 * Function GetPenSize
 * @return the size of the "pen" that be used to draw or plot this item
 */
int LIB_TEXT::GetPenSize( ) const
{
    int     pensize = m_Thickness;

    if( pensize == 0 )   // Use default values for pen size
    {
        if( m_Bold  )
            pensize = GetPenSizeForBold( m_Size.x );
        else
            pensize = g_DrawDefaultLineThickness;
    }
    // Clip pen size for small texts:
    pensize = Clamp_Text_PenSize( pensize, m_Size, m_Bold );
    return pensize;
}


void LIB_TEXT::drawGraphic( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aOffset,
                            int aColor, int aDrawMode, void* aData, const TRANSFORM& aTransform )
{
    wxPoint pos1, pos2;

    int     color = GetDefaultColor();

    if( aColor < 0 )       // Used normal color or selected color
    {
        if( ( m_Selected & IS_SELECTED ) )
            color = g_ItemSelectetColor;
    }
    else
        color = aColor;

    pos1 = aTransform.TransformCoordinate( m_Pos ) + aOffset;

    GRSetDrawMode( aDC, aDrawMode );

    /* Calculate the text orientation, according to the component
     * orientation/mirror (needed when draw text in schematic)
     */
    int orient = m_Orient;
    if( aTransform.y1 )  // Rotate component 90 degrees.
    {
        if( orient == TEXT_ORIENT_HORIZ )
            orient = TEXT_ORIENT_VERT;
        else
            orient = TEXT_ORIENT_HORIZ;
    }

    /* Calculate the text justification, according to the component
     * orientation/mirror this is a bit complicated due to cumulative
     * calculations:
     * - numerous cases (mirrored or not, rotation)
     * - the DrawGraphicText function recalculate also H and H justifications
     *      according to the text orientation.
     * - When a component is mirrored, the text is not mirrored and
     *   justifications are complicated to calculate
     * so the more easily way is to use no justifications ( Centered text )
     * and use GetBoundaryBox to know the text coordinate considered as centered
    */
    EDA_RECT bBox = GetBoundingBox();
    pos1 = bBox.Centre();   // this is the coordinates of the graphic text relative to the
                            // component position in schematic Y axis orientation.

    /* convert y coordinate from schematic to library Y axis orientation
     * because we want to call TransformCoordinate to calculate real coordinates
     */
    NEGATE( pos1.y );
    pos1 = aTransform.TransformCoordinate( pos1 ) + aOffset;
    DrawGraphicText( aPanel, aDC, pos1, (EDA_Colors) color, m_Text, orient, m_Size,
                     GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_CENTER, GetPenSize(),
                     m_Italic, m_Bold );


    /* Enable this to draw the bounding box around the text field to validate
     * the bounding box calculations.
     */
#if 0
    EDA_RECT grBox;
    bBox.SetY( -bBox.GetY() );
    bBox.SetHeight( -bBox.GetHeight());
    grBox.SetOrigin( aTransform.TransformCoordinate( bBox.GetOrigin() ) );
    grBox.SetEnd( aTransform.TransformCoordinate( bBox.GetEnd() ) );
    grBox.Move( aOffset );
    GRRect( &aPanel->m_ClipBox, aDC, grBox.GetOrigin().x, grBox.GetOrigin().y,
            grBox.GetEnd().x, grBox.GetEnd().y, 0, LIGHTMAGENTA );
#endif
}


void LIB_TEXT::DisplayInfo( EDA_DRAW_FRAME* frame )
{
    wxString msg;

    LIB_ITEM::DisplayInfo( frame );

    msg = ReturnStringFromValue( g_UserUnit, m_Thickness, EESCHEMA_INTERNAL_UNIT, true );

    frame->AppendMsgPanel( _( "Line width" ), msg, BLUE );
}


/**
 * @return the boundary box for this, in schematic coordinates
 */
EDA_RECT LIB_TEXT::GetBoundingBox() const
{
    /* Y coordinates for LIB_ITEMS are bottom to top, so we must invert the Y position when
     * calling GetTextBox() that works using top to bottom Y axis orientation.
     */
    EDA_RECT rect = GetTextBox( -1, -1, true );

    wxPoint orig = rect.GetOrigin();
    wxPoint end = rect.GetEnd();
    wxPoint center = rect.Centre();

    RotatePoint( &orig, center, m_Orient );
    RotatePoint( &end, center, m_Orient );
    rect.SetOrigin( orig );
    rect.SetEnd( end );
    rect.Normalize();
    return rect;
}


void LIB_TEXT::Rotate()
{
    if( InEditMode() )
    {
        m_rotate = true;
    }
    else
    {
        m_Orient = ( m_Orient == TEXT_ORIENT_VERT ) ? TEXT_ORIENT_HORIZ : TEXT_ORIENT_VERT;
    }
}


void LIB_TEXT::SetText( const wxString& aText )
{
    if( aText == m_Text )
        return;

    if( InEditMode() )
    {
        m_savedText = aText;
        m_updateText = true;
    }
    else
    {
        m_Text = aText;
    }
}


wxString LIB_TEXT::GetSelectMenuText() const
{
    wxString tmp = GetText();
    tmp.Replace( wxT( "\n" ), wxT( " " ) );
    tmp.Replace( wxT( "\r" ), wxT( " " ) );
    tmp.Replace( wxT( "\t" ), wxT( " " ) );
    tmp =( tmp.Length() > 15 ) ? tmp.Left( 12 ) + wxT( "..." ) : tmp;

    wxString msg;
    msg.Printf( _( "Graphic Text %s" ), GetChars( tmp ) );
    return msg;
}


void LIB_TEXT::BeginEdit( int aEditMode, const wxPoint aPosition )
{
    wxCHECK_RET( ( aEditMode & ( IS_NEW | IS_MOVED ) ) != 0,
                 wxT( "Invalid edit mode for LIB_TEXT object." ) );

    if( aEditMode == IS_MOVED )
    {
        m_initialPos = m_Pos;
        m_initialCursorPos = aPosition;
        SetEraseLastDrawItem();
    }
    else
    {
        m_Pos = aPosition;
    }

    m_Flags = aEditMode;
}


bool LIB_TEXT::ContinueEdit( const wxPoint aPosition )
{
    wxCHECK_MSG( ( m_Flags & ( IS_NEW | IS_MOVED ) ) != 0, false,
                   wxT( "Bad call to ContinueEdit().  Text is not being edited." ) );

    return false;
}


void LIB_TEXT::EndEdit( const wxPoint& aPosition, bool aAbort )
{
    wxCHECK_RET( ( m_Flags & ( IS_NEW | IS_MOVED ) ) != 0,
                   wxT( "Bad call to EndEdit().  Text is not being edited." ) );

    m_Flags = 0;
    m_rotate = false;
    m_updateText = false;
    SetEraseLastDrawItem( false );
}


void LIB_TEXT::calcEdit( const wxPoint& aPosition )
{
    if( m_rotate )
    {
        m_Orient = ( m_Orient == TEXT_ORIENT_VERT ) ? TEXT_ORIENT_HORIZ : TEXT_ORIENT_VERT;
        m_rotate = false;
    }

    if( m_updateText )
    {
        EXCHG( m_Text, m_savedText );
        m_updateText = false;
    }

    if( m_Flags == IS_NEW )
    {
        SetEraseLastDrawItem();
        m_Pos = aPosition;
    }
    else if( m_Flags == IS_MOVED )
    {
        Move( m_initialPos + aPosition - m_initialCursorPos );
    }
}
