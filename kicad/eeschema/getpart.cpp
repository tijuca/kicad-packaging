/*************************************************/
/*  Module to handle Get & Place Library Part    */
/*************************************************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "gr_basic.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"

#include "program.h"
#include "general.h"
#include "protos.h"
#include "class_library.h"
#include "viewlib_frame.h"

#include "get_component_dialog.h"

#include <boost/foreach.hpp>


static void ShowWhileMoving( WinEDA_DrawPanel* panel, wxDC* DC, bool erase );
static void ExitPlaceCmp( WinEDA_DrawPanel* Panel, wxDC* DC );

static int     OldTransMat[2][2];
static wxPoint OldPos;


wxString WinEDA_SchematicFrame::SelectFromLibBrowser( void )
{
    wxSemaphore semaphore( 0, 1 );

    /* Close the current Lib browser, if open, and open a new one, in
     * "modal" mode */
    if( m_ViewlibFrame )
    {
        m_ViewlibFrame->Destroy();
        m_ViewlibFrame = NULL;
    }

    m_ViewlibFrame = new WinEDA_ViewlibFrame( this, NULL, &semaphore );
    m_ViewlibFrame->AdjustScrollBars();

    // Show the library viewer frame until it is closed
    while( semaphore.TryWait() == wxSEMA_BUSY ) // Wait for viewer closing event
    {
        wxYield();
        wxMilliSleep( 50 );
    }

    return m_ViewlibFrame->GetEntryName();
}



/*
 * load from a library and place a component
 *  if libname != "", search in lib "libname"
 *  else search in all loaded libs
 */
SCH_COMPONENT* WinEDA_SchematicFrame::Load_Component( wxDC*           DC,
                                                      const wxString& libname,
                                                      wxArrayString&  HistoryList,
                                                      bool            UseLibBrowser )
{
    int             CmpCount  = 0;
    int             unit      = 1;
    int             convert   = 1;
    LIB_COMPONENT*  Entry     = NULL;
    SCH_COMPONENT*  Component = NULL;
    CMP_LIBRARY*    Library   = NULL;
    wxString        Name, keys, msg;
    bool            AllowWildSeach = TRUE;
    static wxString lastCommponentName;

    g_ItemToRepeat = NULL;
    DrawPanel->m_IgnoreMouseEvents = TRUE;

    if( !libname.IsEmpty() )
    {
        Library = CMP_LIBRARY::FindLibrary( libname );

        if( Library != NULL )
            CmpCount = Library->GetCount();
    }
    else
    {
        BOOST_FOREACH( CMP_LIBRARY& lib, CMP_LIBRARY::GetLibraryList() )
        {
            CmpCount += lib.GetCount();
        }
    }

    /* Ask for a component name or key words */
    msg.Printf( _( "component selection (%d items loaded):" ), CmpCount );

    WinEDA_SelectCmp dlg( this, GetComponentDialogPosition(), HistoryList,
                          msg, UseLibBrowser );
    dlg.SetComponentName( lastCommponentName );

    if ( dlg.ShowModal() == wxID_CANCEL )
    {
        DrawPanel->m_IgnoreMouseEvents = FALSE;
        DrawPanel->MouseToCursorSchema();
        return NULL;
    }

    if( dlg.m_GetExtraFunction )
    {
        Name = SelectFromLibBrowser();
        unit = m_ViewlibFrame->GetUnit();
        convert = m_ViewlibFrame->GetConvert();
    }
    else
    {
        Name = dlg.GetComponentName();
    }

    if( Name.IsEmpty() )
    {
        DrawPanel->m_IgnoreMouseEvents = FALSE;
        DrawPanel->MouseToCursorSchema();
        return NULL;
    }

    Name.MakeUpper();

    if( Name.GetChar( 0 ) == '=' )
    {
        AllowWildSeach = FALSE;
        keys = Name.AfterFirst( '=' );
        Name = DataBaseGetName( this, keys, Name );
        if( Name.IsEmpty() )
        {
            DrawPanel->m_IgnoreMouseEvents = FALSE;
            DrawPanel->MouseToCursorSchema();
            return NULL;
        }
    }
    else if( Name == wxT( "*" ) )
    {
        AllowWildSeach = FALSE;
        if( GetNameOfPartToLoad( this, Library, Name ) == 0 )
        {
            DrawPanel->m_IgnoreMouseEvents = FALSE;
            DrawPanel->MouseToCursorSchema();
            return NULL;
        }
    }
    else if( Name.Contains( wxT( "?" ) ) || Name.Contains( wxT( "*" ) ) )
    {
        AllowWildSeach = FALSE;
        Name = DataBaseGetName( this, keys, Name );
        if( Name.IsEmpty() )
        {
            DrawPanel->m_IgnoreMouseEvents = FALSE;
            DrawPanel->MouseToCursorSchema();
            return NULL;
        }
    }

    Entry = CMP_LIBRARY::FindLibraryComponent( Name, libname );

    if( ( Entry == NULL ) && AllowWildSeach ) /* Search with wildcard */
    {
        AllowWildSeach = FALSE;
        wxString wildname = wxChar( '*' ) + Name + wxChar( '*' );
        Name = wildname;
        Name = DataBaseGetName( this, keys, Name );

        if( !Name.IsEmpty() )
            Entry = CMP_LIBRARY::FindLibraryComponent( Name, libname );

        if( Entry == NULL )
        {
            DrawPanel->m_IgnoreMouseEvents = FALSE;
            DrawPanel->MouseToCursorSchema();
            return NULL;
        }
    }

    DrawPanel->m_IgnoreMouseEvents = FALSE;
    DrawPanel->MouseToCursorSchema();
    if( Entry == NULL )
    {
        msg = _( "Failed to find part " ) + Name + _( " in library" );
        DisplayError( this, msg );
        return NULL;
    }

    lastCommponentName = Name;
    AddHistoryComponentName( HistoryList, Name );

    DrawPanel->ManageCurseur = ShowWhileMoving;
    DrawPanel->ForceCloseManageCurseur = ExitPlaceCmp;

    Component = new SCH_COMPONENT( *Entry, GetSheet(), unit, convert,
                                   GetScreen()->m_Curseur, true );
    // Set the m_ChipName value, from component name in lib, for aliases
    // Note if Entry is found, and if Name is an alias of a component,
    // alias exists because its root component was found
    Component->m_ChipName = Name;

    // Set the component value that can differ from component name in lib, for aliases
    Component->GetField( VALUE )->m_Text = Name;
    Component->DisplayInfo( this );

    DrawStructsInGhost( DrawPanel, DC, Component, wxPoint( 0, 0 ) );

    return Component;
}


/**
 * Move a component.
 */
static void ShowWhileMoving( WinEDA_DrawPanel* panel, wxDC* DC, bool erase )
{
    wxPoint        move_vector;

    SCH_SCREEN*    screen = (SCH_SCREEN*) panel->GetScreen();

    SCH_COMPONENT* Component = (SCH_COMPONENT*) screen->GetCurItem();

    if( erase )
    {
        DrawStructsInGhost( panel, DC, Component, wxPoint(0,0) );
    }

    move_vector = screen->m_Curseur - Component->m_Pos;
    Component->Move( move_vector );

    DrawStructsInGhost( panel, DC, Component, wxPoint(0,0) );
}


/*
 * Routine to rotate and mirror a component.
 *
 ** If DC == NULL: no repaint
 */
void WinEDA_SchematicFrame::CmpRotationMiroir( SCH_COMPONENT* DrawComponent,
                                               wxDC* DC, int type_rotate )
{
    if( DrawComponent == NULL )
        return;

    /* Deletes the previous component. */
    if( DC )
    {
        DrawPanel->CursorOff( DC );
        if( DrawComponent->m_Flags )
            DrawStructsInGhost( DrawPanel, DC, DrawComponent, wxPoint( 0, 0 ) );
        else
        {
            DrawPanel->PostDirtyRect( DrawComponent->GetBoundingBox() );
        }
    }

    DrawComponent->SetOrientation( type_rotate );

    /* Redraw the component in the new position. */
    if( DC )
    {
        if( DrawComponent->m_Flags )
            DrawStructsInGhost( DrawPanel, DC, DrawComponent, wxPoint(0,0) );
        else
            DrawComponent->Draw( DrawPanel, DC, wxPoint( 0, 0 ),
                                 GR_DEFAULT_DRAWMODE );
        DrawPanel->CursorOn( DC );
    }

    TestDanglingEnds( GetScreen()->EEDrawList, DC );
    OnModify( );
}


/*
 * Abort a place component command in progress.
 */
static void ExitPlaceCmp( WinEDA_DrawPanel* Panel, wxDC* DC )
{
    SCH_SCREEN*    screen = (SCH_SCREEN*) Panel->GetScreen();

    SCH_COMPONENT* Component = (SCH_COMPONENT*) screen->GetCurItem();

    if( Component->m_Flags & IS_NEW )
    {
        Component->m_Flags = 0;
        SAFE_DELETE( Component );
    }
    else if( Component )
    {
        wxPoint move_vector = OldPos - Component->m_Pos;
        Component->Move( move_vector );
        memcpy( Component->m_Transform, OldTransMat, sizeof(OldTransMat) );
        Component->m_Flags = 0;
    }

    Panel->Refresh( TRUE );
    Panel->ManageCurseur = NULL;
    Panel->ForceCloseManageCurseur = NULL;
    screen->SetCurItem( NULL );
}


/*
 * Handle select part in multi-part component.
 */
void WinEDA_SchematicFrame::SelPartUnit( SCH_COMPONENT* DrawComponent,
                                         int unit, wxDC* DC )
{
    int m_UnitCount;
    LIB_COMPONENT* LibEntry;

    if( DrawComponent == NULL )
        return;

    LibEntry = CMP_LIBRARY::FindLibraryComponent( DrawComponent->m_ChipName );

    if( LibEntry == NULL )
        return;

    m_UnitCount = LibEntry->GetPartCount();

    if( m_UnitCount <= 1 )
        return;

    if( DrawComponent->m_Multi == unit )
        return;
    if( unit < 1 )
        unit = 1;
    if( unit > m_UnitCount )
        unit = m_UnitCount;

    if( DrawComponent->m_Flags )
        DrawStructsInGhost( DrawPanel, DC, DrawComponent, wxPoint( 0, 0 ) );
    else
        DrawComponent->Draw( DrawPanel, DC, wxPoint( 0, 0 ), g_XorMode );

    /* Update the unit number. */
    DrawComponent->SetUnitSelection( GetSheet(), unit );
    DrawComponent->m_Multi = unit;

    /* Redraw the component in the new position. */
    if( DrawComponent->m_Flags )
        DrawStructsInGhost( DrawPanel, DC, DrawComponent, wxPoint( 0, 0 ) );
    else
        DrawComponent->Draw( DrawPanel, DC, wxPoint( 0, 0 ),
                             GR_DEFAULT_DRAWMODE );

    TestDanglingEnds( GetScreen()->EEDrawList, DC );
    OnModify( );
}


void WinEDA_SchematicFrame::ConvertPart( SCH_COMPONENT* DrawComponent,
                                         wxDC*          DC )
{
    LIB_COMPONENT* LibEntry;

    if( DrawComponent == NULL )
        return;

    LibEntry = CMP_LIBRARY::FindLibraryComponent( DrawComponent->m_ChipName );

    if( LibEntry == NULL )
        return;

    if( !LibEntry->HasConversion() )
    {
        DisplayError( this, wxT( "No convert found" ) );
        return;
    }

    if( DrawComponent->m_Flags )
        DrawStructsInGhost( DrawPanel, DC, DrawComponent, wxPoint( 0, 0 ) );
    else
        DrawComponent->Draw( DrawPanel, DC, wxPoint( 0, 0 ), g_XorMode );

    DrawComponent->m_Convert++;
    // ensure m_Convert = 0, 1 or 2
    // 0 and 1 = shape 1 = not converted
    // 2 = shape 2 = first converted shape
    // > 2 is not used but could be used for more shapes
    // like multiple shapes for a programmable component
    // When m_Convert = val max, return to the first shape
    if( DrawComponent->m_Convert > 2 )
        DrawComponent->m_Convert = 1;

    /* Redraw the component in the new position. */
    if( DrawComponent->m_Flags & IS_MOVED )
        DrawStructsInGhost( DrawPanel, DC, DrawComponent, wxPoint( 0, 0 ) );
    else
        DrawComponent->Draw( DrawPanel, DC, wxPoint( 0, 0 ),
                             GR_DEFAULT_DRAWMODE );

    TestDanglingEnds( GetScreen()->EEDrawList, DC );
    OnModify( );
}


void WinEDA_SchematicFrame::StartMovePart( SCH_COMPONENT* Component,
                                           wxDC*          DC )
{
    if( Component == NULL )
        return;
    if( Component->Type() != TYPE_SCH_COMPONENT )
        return;

    if( Component->m_Flags == 0 )
    {
        if( g_ItemToUndoCopy )
        {
            SAFE_DELETE( g_ItemToUndoCopy );
        }
        g_ItemToUndoCopy = Component->GenCopy();
    }

    DrawPanel->CursorOff( DC );
    GetScreen()->m_Curseur = Component->m_Pos;
    DrawPanel->MouseToCursorSchema();

    DrawPanel->ManageCurseur = ShowWhileMoving;
    DrawPanel->ForceCloseManageCurseur = ExitPlaceCmp;
    GetScreen()->SetCurItem( Component );
    OldPos = Component->m_Pos;
    memcpy( OldTransMat, Component->m_Transform, sizeof(OldTransMat) );

#if 1

    // switch from normal mode to xor mode for the duration of the move, first
    // by erasing fully any "normal drawing mode" primitives with the
    // PostDirtyRect(), then by drawing the first time in xor mode so that
    // subsequent xor drawing modes will fully erase this first copy.

    Component->m_Flags |= IS_MOVED; // omit redrawing the component, erase only
    DrawPanel->PostDirtyRect( Component->GetBoundingBox() );

    DrawStructsInGhost( DrawPanel, DC, Component, wxPoint(0,0) );

#else

    RedrawOneStruct( DrawPanel, DC, Component, g_XorMode );

    Component->m_Flags |= IS_MOVED;

    DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
#endif

    DrawPanel->m_AutoPAN_Request = TRUE;

    DrawPanel->CursorOn( DC );
}
