/**************************************************************/
/* class_sch_component.cpp : handle the  class SCH_COMPONENT  */
/**************************************************************/

#include "fctsys.h"
#include "class_drawpanel.h"
#include "gr_basic.h"
#include "common.h"

#include "program.h"
#include "general.h"
#include "macros.h"
#include "protos.h"
#include "class_library.h"
#include "dialog_schematic_find.h"

#include <wx/tokenzr.h>


static LIB_COMPONENT* DummyCmp;


/* Descr component <DUMMY> used when a component is not found in library,
 *  to draw a dummy shape
 *  This component is a 400 mils square with the text ??
 *  DEF DUMMY U 0 40 Y Y 1 0 N
 *  F0 "U" 0 -350 60 H V
 *  F1 "DUMMY" 0 350 60 H V
 *  DRAW
 *  T 0 0 0 150 0 0 0 ??
 *  S -200 200 200 -200 0 1 0
 *  ENDDRAW
 *  ENDDEF
 */
void CreateDummyCmp()
{
    DummyCmp = new LIB_COMPONENT( wxEmptyString );

    LIB_RECTANGLE* Square = new LIB_RECTANGLE(DummyCmp);

    Square->m_Pos   = wxPoint( -200, 200 );
    Square->m_End   = wxPoint( 200, -200 );

    LIB_TEXT* Text = new LIB_TEXT(DummyCmp);

    Text->m_Size.x = Text->m_Size.y = 150;
    Text->m_Text   = wxT( "??" );

    DummyCmp->AddDrawItem( Square );
    DummyCmp->AddDrawItem( Text );
}


SCH_COMPONENT::SCH_COMPONENT( const wxPoint& aPos, SCH_ITEM* aParent ) :
    SCH_ITEM( aParent, TYPE_SCH_COMPONENT )
{
    Init( aPos );
}


SCH_COMPONENT::SCH_COMPONENT( LIB_COMPONENT& libComponent,
                              SCH_SHEET_PATH* sheet, int unit, int convert,
                              const wxPoint& pos, bool setNewItemFlag ) :
    SCH_ITEM( NULL, TYPE_SCH_COMPONENT )
{

    Init( pos );

    m_Multi     = unit;
    m_Convert   = convert;
    m_ChipName  = libComponent.GetName();
    m_TimeStamp = GetTimeStamp();

    if( setNewItemFlag )
        m_Flags = IS_NEW | IS_MOVED;

    // Import predefined fields from the library component:
    LIB_FIELD_LIST libFields;
    libComponent.GetFields( libFields );

    for(  size_t i = 0; i < libFields.size(); i++ )
    {
        if( libFields[i].m_Text.IsEmpty() && libFields[i].m_Name.IsEmpty() )
            continue;

        int field_idx = libFields[i].m_FieldId;
        /* Add extra fields if library component has more than the default
         * number of fields.
         */
        if( field_idx >= GetFieldCount() )
        {
            while( field_idx >= GetFieldCount() )
            {
                SCH_FIELD field( wxPoint( 0, 0 ), GetFieldCount(), this,
                                 ReturnDefaultFieldName( field_idx ) );
                AddField( field );
            }
        }
        SCH_FIELD* schField = GetField( field_idx );

        schField->m_Pos = m_Pos + libFields[i].m_Pos;
        schField->ImportValues( libFields[i] );
        schField->m_Text = libFields[i].m_Text;
        schField->m_Name = ( field_idx < FIELD1 ) ? ReturnDefaultFieldName( field_idx ) :
            libFields[i].m_Name;
    }


    wxString msg = libComponent.GetReferenceField().m_Text;

    if( msg.IsEmpty() )
        msg = wxT( "U" );
    m_PrefixString = msg;

    // update the reference -- just the prefix for now.
    msg += wxT( "?" );
    SetRef( sheet, msg );

    /* Use the schematic component name instead of the library value field
     * name.
     */
    GetField( VALUE )->m_Text = m_ChipName;
}


SCH_COMPONENT::SCH_COMPONENT( const SCH_COMPONENT& aTemplate ) :
    SCH_ITEM( NULL, TYPE_SCH_COMPONENT )
{
    /* assignment of all fields, including field vector elements, and linked
     * list pointers */
    *this = aTemplate;

    /* set linked list pointers to null, before this they were copies of
     * aTemplate's */
    Pback = NULL;
    Pnext = NULL;
    m_Son = NULL;

    // Re-parent the fields, which before this had aTemplate as parent
    for( int i=0; i<GetFieldCount(); ++i )
    {
        GetField( i )->SetParent( this );
    }
}


void SCH_COMPONENT::Init( const wxPoint& pos )
{
    m_Pos = pos;
    m_Multi = 0;    /* In multi unit chip - which unit to draw. */
    m_Convert = 0;  /* De Morgan Handling  */

    /* The rotation/mirror transformation matrix. pos normal */
    m_Transform[0][0] = 1;
    m_Transform[0][1] = 0;
    m_Transform[1][0] = 0;
    m_Transform[1][1] = -1;

    m_Fields.reserve( DEFAULT_NUMBER_OF_FIELDS );

    for( int i = 0; i < DEFAULT_NUMBER_OF_FIELDS; ++i )
    {
        SCH_FIELD field( pos, i, this, ReturnDefaultFieldName( i ) );

        if( i==REFERENCE )
            field.SetLayer( LAYER_REFERENCEPART );
        else if( i==VALUE )
            field.SetLayer( LAYER_VALUEPART );

        // else keep LAYER_FIELDS from SCH_FIELD constructor

        // SCH_FIELD's implicitly created copy constructor is called in here
        AddField( field );
    }

    m_PrefixString = wxString( _( "U" ) );
}


/*****************************************************************************
* Routine to draw the given part at given position, transformed/mirror as    *
* specified, and in the given drawing mode. Only this one is visible...      *
*****************************************************************************/
void SCH_COMPONENT::Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                          const wxPoint& offset, int DrawMode, int Color,
                          bool DrawPinText )
{
    LIB_COMPONENT* Entry;
    int  ii;
    bool dummy = FALSE;

    Entry = CMP_LIBRARY::FindLibraryComponent( m_ChipName );

    if( Entry == NULL )
    {
        /* Create a dummy component if the actual component can not be found. */
        dummy = TRUE;
        if( DummyCmp == NULL )
            CreateDummyCmp();
        Entry = DummyCmp;
    }

    Entry->Draw( panel, DC, m_Pos + offset, dummy ? 0 : m_Multi,
                 dummy ? 0 : m_Convert, DrawMode, Color, m_Transform,
                 DrawPinText, false );

    SCH_FIELD* field = GetField( REFERENCE );

    if( field->IsVisible() && !( field->m_Flags & IS_MOVED ) )
    {
        if( Entry->GetPartCount() > 1 )
        {
            field->m_AddExtraText = true;
            field->Draw( panel, DC, offset, DrawMode );
        }
        else
        {
            field->m_AddExtraText = false;
            field->Draw( panel, DC, offset, DrawMode );
        }
    }

    for( ii = VALUE; ii < GetFieldCount(); ii++ )
    {
        field = GetField( ii );

        if( field->m_Flags & IS_MOVED )
            continue;

        field->Draw( panel, DC, offset, DrawMode );
    }


#if 0
    /* Draw the component boundary box */
    {
        EDA_Rect BoundaryBox;
        BoundaryBox = GetBoundaryBox();
        GRRect( &panel->m_ClipBox, DC, BoundaryBox, BROWN );
#if 1
        if( GetField( REFERENCE )->IsVisible() )
        {
            BoundaryBox = GetField( REFERENCE )->GetBoundaryBox();
            GRRect( &panel->m_ClipBox, DC, BoundaryBox, BROWN );
        }

        if( GetField( VALUE )->IsVisible() )
        {
            BoundaryBox = GetField( VALUE )->GetBoundaryBox();
            GRRect( &panel->m_ClipBox, DC, BoundaryBox, BROWN );
        }
#endif
    }
#endif
}


/**
 * Function AddHierarchicalReference
 * adds a full hierarchical reference (path + local reference)
 * @param aPath = hierarchical path (/<sheet timestamp>/component timestamp>
 * like /05678E50/A23EF560)
 * @param aRef = local reference like C45, R56
 * @param aMulti = part selection, used in multi part per package (0 or 1 for
 * non multi)
 */
void SCH_COMPONENT::AddHierarchicalReference( const wxString& aPath,
                                              const wxString& aRef,
                                              int             aMulti )
{
    wxString          h_path, h_ref;
    wxStringTokenizer tokenizer;
    wxString          separators( wxT( " " ) );

    // Search for an existing path and remove it if found (should not occur)
    for( unsigned ii = 0; ii < m_PathsAndReferences.GetCount(); ii++ )
    {
        tokenizer.SetString( m_PathsAndReferences[ii], separators );
        h_path = tokenizer.GetNextToken();

        if( h_path.Cmp( aPath ) == 0 )
        {
            m_PathsAndReferences.RemoveAt( ii );
            ii--;
        }
    }

    h_ref = aPath + wxT( " " ) + aRef;
    h_ref << wxT( " " ) << aMulti;
    m_PathsAndReferences.Add( h_ref );
}


wxString SCH_COMPONENT::ReturnFieldName( int aFieldNdx ) const
{
    SCH_FIELD* field = GetField( aFieldNdx );

    if( field )
    {
        if( !field->m_Name.IsEmpty() )
            return field->m_Name;
        else
            return ReturnDefaultFieldName( aFieldNdx );
    }

    return wxEmptyString;
}


wxString SCH_COMPONENT::GetPath( SCH_SHEET_PATH* sheet )
{
    wxString str;

    str.Printf( wxT( "%8.8lX" ), m_TimeStamp );
    return sheet->Path() + str;
}


const wxString SCH_COMPONENT::GetRef( SCH_SHEET_PATH* sheet )
{
    wxString          path = GetPath( sheet );
    wxString          h_path, h_ref;
    wxStringTokenizer tokenizer;
    wxString          separators( wxT( " " ) );

    for( unsigned ii = 0; ii < m_PathsAndReferences.GetCount(); ii++ )
    {
        tokenizer.SetString( m_PathsAndReferences[ii], separators );
        h_path = tokenizer.GetNextToken();

        if( h_path.Cmp( path ) == 0 )
        {
            h_ref = tokenizer.GetNextToken();

            /* printf( "GetRef hpath: %s\n",
                    CONV_TO_UTF8( m_PathsAndReferences[ii] ) ); */
            return h_ref;
        }
    }

    // if it was not found in m_Paths array, then see if it is in
    // m_Field[REFERENCE] -- if so, use this as a default for this path.
    // this will happen if we load a version 1 schematic file.
    // it will also mean that multiple instances of the same sheet by default
    // all have the same component references, but perhaps this is best.
    if( !GetField( REFERENCE )->m_Text.IsEmpty() )
    {
        SetRef( sheet, GetField( REFERENCE )->m_Text );
        return GetField( REFERENCE )->m_Text;
    }
    return m_PrefixString;
}


void SCH_COMPONENT::SetRef( SCH_SHEET_PATH* sheet, const wxString& ref )
{
    wxString          path = GetPath( sheet );

    bool              notInArray = true;

    wxString          h_path, h_ref;
    wxStringTokenizer tokenizer;
    wxString          separators( wxT( " " ) );

    // check to see if it is already there before inserting it
    for( unsigned ii = 0; ii < m_PathsAndReferences.GetCount(); ii++ )
    {
        tokenizer.SetString( m_PathsAndReferences[ii], separators );
        h_path = tokenizer.GetNextToken();
        if( h_path.Cmp( path ) == 0 )
        {
            // just update the reference text, not the timestamp.
            h_ref  = h_path + wxT( " " ) + ref;
            h_ref += wxT( " " );
            tokenizer.GetNextToken();               // Skip old reference
            h_ref += tokenizer.GetNextToken();      // Add part selection
            // Ann the part selection
            m_PathsAndReferences[ii] = h_ref;
            notInArray = false;
        }
    }

    if( notInArray )
        AddHierarchicalReference( path, ref, m_Multi );

    SCH_FIELD* rf = GetField( REFERENCE );

    if( rf->m_Text.IsEmpty()
        || ( abs( rf->m_Pos.x - m_Pos.x ) +
             abs( rf->m_Pos.y - m_Pos.y ) > 10000 ) )
    {
        // move it to a reasonable position
        rf->m_Pos    = m_Pos;
        rf->m_Pos.x += 50;         // a slight offset
        rf->m_Pos.y += 50;
    }

    rf->m_Text = ref;  // for drawing.

    // Reinit the m_PrefixString member if needed
    wxString prefix = ref;
    while( prefix.Last() == '?' or isdigit(prefix.Last()) )
        prefix.RemoveLast();
    if( m_PrefixString != prefix )
        m_PrefixString = prefix;
}

/** function SetTimeStamp
 * Change the old time stamp to the new time stamp.
 * the time stamp is also modified in paths
 * @param aNewTimeStamp = new time stamp
 */
void SCH_COMPONENT::SetTimeStamp( long aNewTimeStamp)
{
    wxString string_timestamp, string_oldtimestamp;
    string_timestamp.Printf(wxT("%8.8X"), aNewTimeStamp);
    string_oldtimestamp.Printf(wxT("%8.8X"), m_TimeStamp);
    m_TimeStamp = aNewTimeStamp;
    for( unsigned ii = 0; ii < m_PathsAndReferences.GetCount(); ii++ )
    {
        m_PathsAndReferences[ii].Replace( string_oldtimestamp.GetData(),
                                          string_timestamp.GetData() );
   }
}


/***********************************************************/
//returns the unit selection, for the given sheet path.
/***********************************************************/
int SCH_COMPONENT::GetUnitSelection( SCH_SHEET_PATH* aSheet )
{
    wxString          path = GetPath( aSheet );
    wxString          h_path, h_multi;
    wxStringTokenizer tokenizer;
    wxString          separators( wxT( " " ) );

    for( unsigned ii = 0; ii < m_PathsAndReferences.GetCount(); ii++ )
    {
        tokenizer.SetString( m_PathsAndReferences[ii], separators );
        h_path = tokenizer.GetNextToken();

        if( h_path.Cmp( path ) == 0 )
        {
            tokenizer.GetNextToken();   // Skip reference
            h_multi = tokenizer.GetNextToken();
            long imulti = 1;
            h_multi.ToLong( &imulti );
            return imulti;
        }
    }

    // if it was not found in m_Paths array, then use m_Multi.
    // this will happen if we load a version 1 schematic file.
    return m_Multi;
}


/****************************************************************************/
//Set the unit selection, for the given sheet path.
/****************************************************************************/
void SCH_COMPONENT::SetUnitSelection( SCH_SHEET_PATH* aSheet,
                                      int             aUnitSelection )
{
    wxString          path = GetPath( aSheet );

    bool              notInArray = true;

    wxString          h_path, h_ref;
    wxStringTokenizer tokenizer;
    wxString          separators( wxT( " " ) );

    //check to see if it is already there before inserting it
    for( unsigned ii = 0; ii < m_PathsAndReferences.GetCount(); ii++ )
    {
        tokenizer.SetString( m_PathsAndReferences[ii], separators );
        h_path = tokenizer.GetNextToken();

        if( h_path.Cmp( path ) == 0 )
        {
            //just update the unit selection.
            h_ref  = h_path + wxT( " " );
            h_ref += tokenizer.GetNextToken();      // Add reference
            h_ref += wxT( " " );
            h_ref << aUnitSelection;                // Add part selection
            // Ann the part selection
            m_PathsAndReferences[ii] = h_ref;
            notInArray = false;
        }
    }

    if( notInArray )
        AddHierarchicalReference( path, m_PrefixString, aUnitSelection );
}


SCH_FIELD* SCH_COMPONENT::GetField( int aFieldNdx ) const
{
    const SCH_FIELD* field;

    if( (unsigned) aFieldNdx < m_Fields.size() )
        field = &m_Fields[aFieldNdx];
    else
        field = NULL;

    wxASSERT( field );

    // use case to remove const-ness
    return (SCH_FIELD*) field;
}


void SCH_COMPONENT::AddField( const SCH_FIELD& aField )
{
    m_Fields.push_back( aField );
}


LIB_PIN* SCH_COMPONENT::GetPin( const wxString& number )
{
    LIB_COMPONENT* Entry = CMP_LIBRARY::FindLibraryComponent( m_ChipName );

    if( Entry == NULL )
        return NULL;

    wxASSERT( Entry->isComponent() );

    return Entry->GetPin( number, m_Multi, m_Convert );
}

/**
 * Function GetBoundaryBox
 * returns the orthogonal, bounding box of this object for display purposes.
 * This box should be an enclosing perimeter for graphic items and pins.
 * this include only fields defined in library
 * use GetBoundingBox() to include fields in schematic
 */
EDA_Rect SCH_COMPONENT::GetBoundaryBox() const
{
    LIB_COMPONENT* Entry = CMP_LIBRARY::FindLibraryComponent( m_ChipName );
    EDA_Rect BoundaryBox;
    int      x0, xm, y0, ym;

    /* Get the basic Boundary box */
    if( Entry )
    {
        BoundaryBox = Entry->GetBoundaryBox( m_Multi, m_Convert );
        x0 = BoundaryBox.GetX();
        xm = BoundaryBox.GetRight();

        // We must reverse Y values, because matrix orientation
        // suppose Y axis normal for the library items coordinates,
        // m_Transform reverse Y values, but BoundaryBox is already reversed!
        y0 = -BoundaryBox.GetY();
        ym = -BoundaryBox.GetBottom();
    }
    else    /* if lib Entry not found, give a reasonable size */
    {
        x0 = y0 = -50;
        xm = ym = 50;
    }

    /* Compute the real Boundary box (rotated, mirrored ...)*/
    int x1 = m_Transform[0][0] * x0 + m_Transform[0][1] * y0;
    int y1 = m_Transform[1][0] * x0 + m_Transform[1][1] * y0;
    int x2 = m_Transform[0][0] * xm + m_Transform[0][1] * ym;
    int y2 = m_Transform[1][0] * xm + m_Transform[1][1] * ym;

    // H and W must be > 0:
    if( x2 < x1 )
        EXCHG( x2, x1 );
    if( y2 < y1 )
        EXCHG( y2, y1 );

    BoundaryBox.SetX( x1 );
    BoundaryBox.SetY( y1 );
    BoundaryBox.SetWidth( x2 - x1 );
    BoundaryBox.SetHeight( y2 - y1 );

    BoundaryBox.Offset( m_Pos );
    return BoundaryBox;
}


/* Used in undo / redo command:
 *  swap data between this and copyitem
 */
void SCH_COMPONENT::SwapData( SCH_COMPONENT* copyitem )
{
    EXCHG( m_ChipName, copyitem->m_ChipName );
    EXCHG( m_Pos, copyitem->m_Pos );
    EXCHG( m_Multi, copyitem->m_Multi );
    EXCHG( m_Convert, copyitem->m_Convert );
    EXCHG( m_Transform[0][0], copyitem->m_Transform[0][0] );
    EXCHG( m_Transform[0][1], copyitem->m_Transform[0][1] );
    EXCHG( m_Transform[1][0], copyitem->m_Transform[1][0] );
    EXCHG( m_Transform[1][1], copyitem->m_Transform[1][1] );

    m_Fields.swap( copyitem->m_Fields );    // std::vector's swap()

    // Reparent items after copying data
    // (after swap(), m_Parent member does not point to the right parent):
    for( int ii = 0; ii < copyitem->GetFieldCount();  ++ii )
    {
       copyitem->GetField(ii)->SetParent( copyitem );
    }
    for( int ii = 0; ii < GetFieldCount();  ++ii )
    {
       GetField(ii)->SetParent( this );
    }
    
    EXCHG( m_PathsAndReferences, copyitem->m_PathsAndReferences);
}


void SCH_COMPONENT::Place( WinEDA_SchematicFrame* frame, wxDC* DC )
{
    /* save old text in undo list */
    if( g_ItemToUndoCopy
        && ( g_ItemToUndoCopy->Type() == Type() )
        && ( ( m_Flags & IS_NEW ) == 0 ) )
    {
        /* restore old values and save new ones */
        SwapData( (SCH_COMPONENT*) g_ItemToUndoCopy );

        /* save in undo list */
        frame->SaveCopyInUndoList( this, UR_CHANGED );

        /* restore new values */
        SwapData( (SCH_COMPONENT*) g_ItemToUndoCopy );

        SAFE_DELETE( g_ItemToUndoCopy );
    }

    SCH_ITEM::Place( frame, DC );
}


/**
 * Suppress annotation ( i.i IC23 changed to IC? and part reset to 1)
 * @param aSheet: SCH_SHEET_PATH value: if NULL remove all annotations,
 *                else remove annotation relative to this sheetpath
 */
void SCH_COMPONENT::ClearAnnotation( SCH_SHEET_PATH* aSheet )
{
    wxString       defRef    = m_PrefixString;
    bool           KeepMulti = false;
    LIB_COMPONENT* Entry;
    wxString       separators( wxT( " " ) );
    wxArrayString  reference_fields;

    Entry = CMP_LIBRARY::FindLibraryComponent( m_ChipName );

    if( Entry && Entry->m_UnitSelectionLocked )
        KeepMulti = true;

    while( defRef.Last() == '?' )
        defRef.RemoveLast();

    defRef.Append( wxT( "?" ) );

    wxString multi = wxT( "1" );

    // We cannot remove all annotations: part selection must be kept
    if( KeepMulti )
    {
        wxString NewHref;
        wxString path;
        if( aSheet )
            path = GetPath( aSheet );
        for( unsigned int ii = 0; ii < m_PathsAndReferences.GetCount(); ii++ )
        {
            // Break hierarchical reference in path, ref and multi selection:
            reference_fields = wxStringTokenize( m_PathsAndReferences[ii],
                                                 separators );
            if( aSheet == NULL || reference_fields[0].Cmp( path ) == 0 )
            {
                if( KeepMulti )  // Get and keep part selection
                    multi = reference_fields[2];
                NewHref = reference_fields[0];
                NewHref << wxT( " " ) << defRef << wxT( " " ) << multi;
                m_PathsAndReferences[ii] = NewHref;
            }
        }
    }
    else
    {
        // Empty strings, but does not free memory because a new annotation
        // will reuse it
        m_PathsAndReferences.Empty();
        m_Multi = 1;
    }


    // These 2 changes do not work in complex hierarchy.
    // When a clear annotation is made, the calling function must call a
    // UpdateAllScreenReferences for the active sheet.
    // But this call cannot made here.
    m_Fields[REFERENCE].m_Text = defRef; //for drawing.
}


/******************************************************************/
/* Compute the new matrix transform for a schematic component
 *  in order to have the requested transform (type_rotate = rot, mirror..)
 *  which is applied to the initial transform.
 */
/*****************************************************************/
void SCH_COMPONENT::SetOrientation( int aOrientation )
{
    int  TempMat[2][2];
    bool Transform = FALSE;

    switch( aOrientation )
    {
    case CMP_ORIENT_0:
    case CMP_NORMAL:            /* Position Initiale */
        m_Transform[0][0] = 1;
        m_Transform[1][1] = -1;
        m_Transform[1][0] = m_Transform[0][1] = 0;
        break;

    case CMP_ROTATE_CLOCKWISE:            /* Rotate + */
        TempMat[0][0] = TempMat[1][1] = 0;
        TempMat[0][1] = 1;
        TempMat[1][0] = -1;
        Transform = TRUE;
        break;

    case CMP_ROTATE_COUNTERCLOCKWISE:             /* Rotate - */
        TempMat[0][0] = TempMat[1][1] = 0;
        TempMat[0][1] = -1;
        TempMat[1][0] = 1;
        Transform = TRUE;
        break;

    case CMP_MIRROR_Y:          /* MirrorY */
        TempMat[0][0] = -1;
        TempMat[1][1] = 1;
        TempMat[0][1] = TempMat[1][0] = 0;
        Transform = TRUE;
        break;

    case CMP_MIRROR_X:            /* MirrorX */
        TempMat[0][0] = 1;
        TempMat[1][1] = -1;
        TempMat[0][1] = TempMat[1][0] = 0;
        Transform = TRUE;
        break;

    case CMP_ORIENT_90:
        SetOrientation( CMP_ORIENT_0 );
        SetOrientation( CMP_ROTATE_COUNTERCLOCKWISE );
        break;

    case CMP_ORIENT_180:
        SetOrientation( CMP_ORIENT_0 );
        SetOrientation( CMP_ROTATE_COUNTERCLOCKWISE );
        SetOrientation( CMP_ROTATE_COUNTERCLOCKWISE );
        break;

    case CMP_ORIENT_270:
        SetOrientation( CMP_ORIENT_0 );
        SetOrientation( CMP_ROTATE_CLOCKWISE );
        break;

    case ( CMP_ORIENT_0 + CMP_MIRROR_X ):
        SetOrientation( CMP_ORIENT_0 );
        SetOrientation( CMP_MIRROR_X );
        break;

    case ( CMP_ORIENT_0 + CMP_MIRROR_Y ):
        SetOrientation( CMP_ORIENT_0 );
        SetOrientation( CMP_MIRROR_Y );
        break;

    case ( CMP_ORIENT_90 + CMP_MIRROR_X ):
        SetOrientation( CMP_ORIENT_90 );
        SetOrientation( CMP_MIRROR_X );
        break;

    case ( CMP_ORIENT_90 + CMP_MIRROR_Y ):
        SetOrientation( CMP_ORIENT_90 );
        SetOrientation( CMP_MIRROR_Y );
        break;

    case ( CMP_ORIENT_180 + CMP_MIRROR_X ):
        SetOrientation( CMP_ORIENT_180 );
        SetOrientation( CMP_MIRROR_X );
        break;

    case ( CMP_ORIENT_180 + CMP_MIRROR_Y ):
        SetOrientation( CMP_ORIENT_180 );
        SetOrientation( CMP_MIRROR_Y );
        break;

    case ( CMP_ORIENT_270 + CMP_MIRROR_X ):
        SetOrientation( CMP_ORIENT_270 );
        SetOrientation( CMP_MIRROR_X );
        break;

    case ( CMP_ORIENT_270 + CMP_MIRROR_Y ):
        SetOrientation( CMP_ORIENT_270 );
        SetOrientation( CMP_MIRROR_Y );
        break;

    default:
        Transform = FALSE;
        wxMessageBox( wxT( "SetRotateMiroir() error: ill value" ) );
        break;
    }

    if( Transform )
    {
        /* The new matrix transform is the old matrix transform modified by the
         *  requested transformation, which is the TempMat transform (rot,
         *  mirror ..) in order to have (in term of matrix transform):
         *     transform coord = new_m_Transform * coord
         *  where transform coord is the coord modified by new_m_Transform from
         *  the initial value coord.
         *  new_m_Transform is computed (from old_m_Transform and TempMat) to
         *  have:
         *     transform coord = old_m_Transform * coord * TempMat
         */
        int NewMatrix[2][2];

        NewMatrix[0][0] = m_Transform[0][0] * TempMat[0][0] +
                          m_Transform[1][0] * TempMat[0][1];

        NewMatrix[0][1] = m_Transform[0][1] * TempMat[0][0] +
                          m_Transform[1][1] * TempMat[0][1];

        NewMatrix[1][0] = m_Transform[0][0] * TempMat[1][0] +
                          m_Transform[1][0] * TempMat[1][1];

        NewMatrix[1][1] = m_Transform[0][1] * TempMat[1][0] +
                          m_Transform[1][1] * TempMat[1][1];

        m_Transform[0][0] = NewMatrix[0][0];
        m_Transform[0][1] = NewMatrix[0][1];
        m_Transform[1][0] = NewMatrix[1][0];
        m_Transform[1][1] = NewMatrix[1][1];
    }
}


/** function GetOrientation()
 * Used to display component orientation (in dialog editor or info)
 * @return the orientation and mirror
 * Note: Because there are different ways to have a given orientation/mirror,
 * the orientation/mirror is not necessary what the used does
 * (example : a mirrorX then a mirrorY give no mirror but rotate the component).
 * So this function find a rotation and a mirror value
 * ( CMP_MIRROR_X because this is the first mirror option tested)
 *  but can differs from the orientation made by an user
 * ( a CMP_MIRROR_Y is find as a CMP_MIRROR_X + orientation 180, because they
 * are equivalent)
 *
 */
int SCH_COMPONENT::GetOrientation()
{
    int  type_rotate = CMP_ORIENT_0;
    int  ComponentMatOrient[2][2];
    int  ii;

    #define ROTATE_VALUES_COUNT 12

    // list of all possibilities, but only the first 8 are actually used
    int rotate_value[ROTATE_VALUES_COUNT] =
    {
        CMP_ORIENT_0, CMP_ORIENT_90, CMP_ORIENT_180, CMP_ORIENT_270,
        CMP_MIRROR_X + CMP_ORIENT_0, CMP_MIRROR_X + CMP_ORIENT_90,
        CMP_MIRROR_X + CMP_ORIENT_180, CMP_MIRROR_X + CMP_ORIENT_270,
        CMP_MIRROR_Y + CMP_ORIENT_0, CMP_MIRROR_Y + CMP_ORIENT_90,
        CMP_MIRROR_Y + CMP_ORIENT_180, CMP_MIRROR_Y + CMP_ORIENT_270
    };

    // Try to find the current transform option:
    memcpy( ComponentMatOrient, m_Transform, sizeof( ComponentMatOrient ) );

    for( ii = 0; ii < ROTATE_VALUES_COUNT; ii++ )
    {
        type_rotate = rotate_value[ii];
        SetOrientation( type_rotate );
        if( memcmp( ComponentMatOrient, m_Transform,
                    sizeof(ComponentMatOrient) ) == 0 )
            return type_rotate;
    }

    // Error: orientation not found in list (should not happen)
    wxMessageBox(wxT("Component orientation matrix internal error") );
    memcpy( m_Transform, ComponentMatOrient, sizeof( ComponentMatOrient ) );
    return CMP_NORMAL;
}


/**
 * Returns the coordinated point, depending on the orientation of the
 * component (rotation, mirror).
 * The coordinates are always relative to the anchor position of the component.
 */
wxPoint SCH_COMPONENT::GetScreenCoord( const wxPoint& coord )
{
    wxPoint screenpos;

    screenpos.x = m_Transform[0][0] * coord.x + m_Transform[0][1] * coord.y;
    screenpos.y = m_Transform[1][0] * coord.x + m_Transform[1][1] * coord.y;
    return screenpos;
}


#if defined (DEBUG)

/**
 * Function Show
 * is used to output the object tree, currently for debugging only.
 * @param nestLevel An aid to prettier tree indenting, and is the level
 *          of nesting of this object within the overall tree.
 * @param os The ostream& to output to.
 */
void SCH_COMPONENT::Show( int nestLevel, std::ostream& os )
{
    // for now, make it look like XML:
    NestedSpace( nestLevel, os ) << '<' << GetClass().Lower().mb_str()
                                 << " ref=\"" << ReturnFieldName( 0 )
                                 << '"' << " chipName=\""
                                 << m_ChipName.mb_str() << '"' <<  m_Pos
                                 << " layer=\"" << m_Layer
                                 << '"' << "/>\n";

    // skip the reference, it's been output already.
    for( int i = 1; i < GetFieldCount();  ++i )
    {
        wxString value = GetField( i )->m_Text;

        if( !value.IsEmpty() )
        {
            NestedSpace( nestLevel + 1, os ) << "<field" << " name=\""
                                             << ReturnFieldName( i ).mb_str()
                                             << '"' <<  " value=\""
                                             << value.mb_str() << "\"/>\n";
        }
    }

    NestedSpace( nestLevel, os ) << "</" << GetClass().Lower().mb_str()
                                 << ">\n";
}

#endif


bool SCH_COMPONENT::Save( FILE* f ) const
{
    int             ii;
    char            Name1[256], Name2[256];
    wxArrayString   reference_fields;

    static wxString delimiters( wxT( " " ) );

    //this is redundant with the AR entries below, but it makes the
    //files backwards-compatible.
    if( m_PathsAndReferences.GetCount() > 0 )
    {
        reference_fields = wxStringTokenize( m_PathsAndReferences[0],
                                             delimiters );
        strncpy( Name1, CONV_TO_UTF8( reference_fields[1] ), sizeof( Name1 ) );
    }
    else
    {
        if( GetField( REFERENCE )->m_Text.IsEmpty() )
            strncpy( Name1, CONV_TO_UTF8( m_PrefixString ), sizeof( Name1 ) );
        else
            strncpy( Name1, CONV_TO_UTF8( GetField( REFERENCE )->m_Text ),
                     sizeof( Name1 ) );
    }
    for( ii = 0; ii < (int) strlen( Name1 ); ii++ )
    {
#if defined(KICAD_GOST)
            if( Name1[ii] == ' ' )
#else
            if( Name1[ii] <= ' ' )
#endif
            Name1[ii] = '~';
    }

    if( !m_ChipName.IsEmpty() )
    {
        strncpy( Name2, CONV_TO_UTF8( m_ChipName ), sizeof( Name2 ) );
        for( ii = 0; ii < (int) strlen( Name2 ); ii++ )
#if defined(KICAD_GOST)
            if( Name2[ii] == ' ' )
#else
            if( Name2[ii] <= ' ' )
#endif
                Name2[ii] = '~';
    }
    else
        strncpy( Name2, NULL_STRING, sizeof( Name2 ) );

    if ( fprintf( f, "$Comp\n" ) == EOF )
        return false;

    if( fprintf( f, "L %s %s\n", Name2, Name1 ) == EOF )
        return false;

    /* Generate unit number, convert and time stamp*/
    if( fprintf( f, "U %d %d %8.8lX\n", m_Multi, m_Convert,
                 m_TimeStamp ) == EOF )
        return false;

    /* Save the position */
    if( fprintf( f, "P %d %d\n", m_Pos.x, m_Pos.y ) == EOF )
        return false;

    /* If this is a complex hierarchy; save hierarchical references.
     * but for simple hierarchies it is not necessary.
     * the reference inf is already saved
     * this is useful for old eeschema version compatibility
     */
    if( m_PathsAndReferences.GetCount() > 1 )
    {
        for( unsigned int ii = 0; ii <  m_PathsAndReferences.GetCount(); ii++ )
        {
            /*format:
             * AR Path="/140/2" Ref="C99"   Part="1"
             * where 140 is the uid of the containing sheet
             * and 2 is the timestamp of this component.
             * (timestamps are actually 8 hex chars)
             * Ref is the conventional component reference for this 'path'
             * Part is the conventional component part selection for this 'path'
             */
            reference_fields = wxStringTokenize( m_PathsAndReferences[ii],
                                                 delimiters );
            if( fprintf( f, "AR Path=\"%s\" Ref=\"%s\"  Part=\"%s\" \n",
                         CONV_TO_UTF8( reference_fields[0] ),
                         CONV_TO_UTF8( reference_fields[1] ),
                         CONV_TO_UTF8( reference_fields[2] ) ) == EOF )
                return false;
        }
    }

    for( int fieldNdx = 0; fieldNdx < GetFieldCount(); ++fieldNdx )
    {
        SCH_FIELD* field = GetField( fieldNdx );
        wxString   defaultName = ReturnDefaultFieldName( fieldNdx );

        // only save the field if there is a value in the field or if field name
        // is different than the default field name
        if( field->m_Text.IsEmpty() && defaultName == field->m_Name )
            continue;

        if( !field->Save( f ) )
            return false;
    }

    /* Unit number, position, box ( old standard ) */
    if( fprintf( f, "\t%-4d %-4d %-4d\n", m_Multi, m_Pos.x, m_Pos.y ) == EOF )
        return false;

    if( fprintf( f, "\t%-4d %-4d %-4d %-4d\n",
                 m_Transform[0][0], m_Transform[0][1],
                 m_Transform[1][0], m_Transform[1][1] ) == EOF )
        return false;

    if( fprintf( f, "$EndComp\n" ) == EOF )
        return false;

    return true;
}


/**
 * Function GetBoundingBox
 * returns the orthogonal, bounding box of this object for display purposes.
 * This box should be an enclosing perimeter for visible components of this
 * object, and the units should be in the pcb or schematic coordinate system.
 * It is OK to overestimate the size by a few counts.
 */
EDA_Rect SCH_COMPONENT::GetBoundingBox()
{
    const int PADDING = 40;

    // This gives a reasonable approximation (but some things are missing so...)
    EDA_Rect  bbox = GetBoundaryBox();

    // Include BoundingBoxes of fields
    for( int ii = 0; ii < GetFieldCount(); ii++ )
    {
        if( ! GetField( ii )->IsVisible() )
            continue;
        bbox.Merge( GetField( ii )->GetBoundaryBox() );
    }

    // ... add padding
    bbox.Inflate( PADDING );

    return bbox;
}


void SCH_COMPONENT::DisplayInfo( WinEDA_DrawFrame* frame )
{
    // search for the component in lib
    // Entry and root_component can differ if Entry is an alias
    CMP_LIB_ENTRY* Entry = CMP_LIBRARY::FindLibraryEntry( m_ChipName );
    LIB_COMPONENT* root_component = CMP_LIBRARY::FindLibraryComponent( m_ChipName );

    if( (Entry == NULL) || (root_component == NULL) )
        return;

    wxString msg;

    frame->ClearMsgPanel();

    frame->AppendMsgPanel( _( "Reference" ),
                           GetRef(((WinEDA_SchematicFrame*)frame)->GetSheet()),
                           DARKCYAN );

    if( root_component->isPower() )
        msg = _( "Power symbol" );
    else
        msg = _( "Name" );
    frame->AppendMsgPanel( msg, GetField( VALUE )->m_Text, DARKCYAN );

    // Display component reference in library and library
    frame->AppendMsgPanel( _( "Component" ), m_ChipName, BROWN );
    if( Entry->isAlias( ) )
        frame->AppendMsgPanel( _( "Alias of" ), root_component->GetName(), BROWN );
    frame->AppendMsgPanel( _( "Library" ), Entry->GetLibraryName(), BROWN );

    // Display description of the component, and keywords found in lib
    frame->AppendMsgPanel( _( "Description" ), Entry->GetDescription(), DARKCYAN );
    frame->AppendMsgPanel( _( "Key words" ), Entry->GetKeyWords(), DARKCYAN );
}

/** virtual function Mirror_Y
 * mirror item relative to an Y axis
 * @param aYaxis_position = the y axis position
 */
void SCH_COMPONENT::Mirror_Y(int aYaxis_position)
{
    int dx = m_Pos.x;
    SetOrientation( CMP_MIRROR_Y );
    m_Pos.x -= aYaxis_position;
    NEGATE( m_Pos.x );
    m_Pos.x += aYaxis_position;
    dx -= m_Pos.x;     // dx,0 is the move vector for this transform

    for( int ii = 0; ii < GetFieldCount(); ii++ )
    {
        /* move the fields to the new position because the component itself
         * has moved */
        GetField( ii )->m_Pos.x -= dx;
    }
}


bool SCH_COMPONENT::Matches( wxFindReplaceData& aSearchData, void * aAuxData )
{
    // Search reference.
    // reference is a special field because a part identifier is added
    // in multi parts per package
    // the .m_AddExtraText of the field msut be set to add this identifier:
    LIB_COMPONENT* Entry = CMP_LIBRARY::FindLibraryComponent( m_ChipName );
    if( Entry && Entry->GetPartCount() > 1 )
            GetField( REFERENCE )->m_AddExtraText = true;
    else
        GetField( REFERENCE )->m_AddExtraText = false;

    if( GetField( REFERENCE )->Matches( aSearchData, aAuxData ) )
        return true;

    if( GetField( VALUE )->Matches( aSearchData, aAuxData ) )
        return true;

    if( !( aSearchData.GetFlags() & FR_SEARCH_ALL_FIELDS ) )
        return false;

    for( size_t i = VALUE+1; i < m_Fields.size(); i++ )
    {
        if( GetField( i )->Matches( aSearchData, aAuxData ) )
            return true;
    }

    return false;
}
