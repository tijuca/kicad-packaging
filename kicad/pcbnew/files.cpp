/***************************************/
/* files.cpp: read / write board files */
/***************************************/

#include "fctsys.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"
#include "kicad_string.h"
#include "gestfich.h"
#include "pcbnew.h"
#include "wxPcbStruct.h"
#include "protos.h"
#include "pcbnew_id.h"


void WinEDA_PcbFrame::OnFileHistory( wxCommandEvent& event )
{
    wxString fn;

    fn = GetFileFromHistory( event.GetId(), _( "Printed circuit board" ) );

    if( fn != wxEmptyString )
    {
        DrawPanel->UnManageCursor( 0, wxCURSOR_ARROW );
        ::wxSetWorkingDirectory( ::wxPathOnly( fn ) );
        LoadOnePcbFile( fn, false );
        ReCreateAuxiliaryToolbar();
        DrawPanel->MouseToCursorSchema();
    }
}

/****************************************************/
void WinEDA_PcbFrame::Files_io( wxCommandEvent& event )
/****************************************************/

/* Handle the read/write file commands
 */
{
    int        id = event.GetId();
    wxString   msg;


    // If an edition is in progress, stop it
    DrawPanel->UnManageCursor( 0, wxCURSOR_ARROW );

    switch( id )
    {
    case ID_LOAD_FILE:
        LoadOnePcbFile( wxEmptyString, false );
        ReCreateAuxiliaryToolbar();
        break;

    case ID_MENU_READ_LAST_SAVED_VERSION_BOARD:
    case ID_MENU_RECOVER_BOARD:
    {
        wxFileName fn;

        if( id == ID_MENU_RECOVER_BOARD )
        {
            fn = wxFileName( wxEmptyString, g_SaveFileName, PcbExtBuffer );
        }
        else
        {
            fn = GetScreen()->m_FileName;
            fn.SetExt( wxT( "000" ) );
        }

        if( !fn.FileExists() )
        {
            msg = _( "Recovery file " ) + fn.GetFullPath() + _( " not found" );
            DisplayInfoMessage( this, msg );
            break;
        }
        else
        {
            msg = _( "Ok to load Recovery file " ) + fn.GetFullPath();
            if( !IsOK( this, msg ) )
                break;
        }

        LoadOnePcbFile( fn.GetFullPath(), false );
        fn.SetExt( PcbExtBuffer );
        GetScreen()->m_FileName = fn.GetFullPath();
        SetTitle( GetScreen()->m_FileName );
        ReCreateAuxiliaryToolbar();
        break;
    }

    case ID_APPEND_FILE:
        LoadOnePcbFile( wxEmptyString, true );
        break;

    case ID_NEW_BOARD:
        Clear_Pcb( true );
        GetScreen()->m_FileName.Printf( wxT( "%s%cnoname%s" ),
                                        GetChars( wxGetCwd() ), DIR_SEP,
                                        GetChars( PcbExtBuffer ) );
        SetTitle( GetScreen()->m_FileName );
        ReCreateLayerBox( NULL );
        break;

    case ID_SAVE_BOARD:
        SavePcbFile( GetScreen()->m_FileName );
        break;

    case ID_SAVE_BOARD_AS:
        SavePcbFile( wxEmptyString );
        break;

    default:
        DisplayError( this, wxT( "File_io Internal Error" ) ); break;
    }
}


/**
 *  Read a board file
 *  @param FullFileName = file name. If empty, a file name will be asked
 *  @return 0 if fails or abort, 1 if OK
 */
bool WinEDA_PcbFrame::LoadOnePcbFile( const wxString& FullFileName, bool Append )
{
    int      ii;
    FILE*    source;
    wxString msg;
    char     cbuf[1024];

    ActiveScreen = GetScreen();

    if( GetScreen()->IsModify() && !Append )
    {
        if( !IsOK( this, _( "Board Modified: Continue ?" ) ) )
            return false;
    }

    m_TrackAndViasSizesList_Changed = true;

    if( Append )
    {
        GetScreen()->m_FileName = wxEmptyString;
        OnModify();
        GetBoard()->m_Status_Pcb = 0;
    }

    wxString fileName;

    if( FullFileName == wxEmptyString )
    {
        wxFileName fn = GetScreen()->m_FileName;

        wxFileDialog dlg( this, _( "Open Board File" ), wxEmptyString,
                          fn.GetFullName(), PcbFileWildcard,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST );

        if( dlg.ShowModal() == wxID_CANCEL )
            return false;

        fileName = dlg.GetPath();
    }
    else
        fileName = FullFileName;

    if( !Append )
        Clear_Pcb( false );     // pass false since we prompted above for a modified board

    GetScreen()->m_FileName = fileName;

    /* Start read PCB file
    */

    source = wxFopen( GetScreen()->m_FileName, wxT( "rt" ) );
    if( source == NULL )
    {
        msg.Printf( _( "File <%s> not found" ),
                    GetChars( GetScreen()->m_FileName ) );
        DisplayError( this, msg );
        return false;
    }


    /* Read header and TEST if it is a PCB file format */
    GetLine( source, cbuf, &ii );
    if( strncmp( cbuf, "PCBNEW-BOARD", 12 ) != 0 )
    {
        fclose( source );
        DisplayError( this, wxT( "Unknown file type" ) );
        return false;
    }

    int ver;
    sscanf(cbuf, "PCBNEW-BOARD Version %d date", &ver );
    if ( ver > g_CurrentVersionPCB )
    {
        DisplayInfoMessage( this, _( "This file was created by a more recent \
version of PCBnew and may not load correctly. Please consider updating!"));
    }
    else if ( ver < g_CurrentVersionPCB )
    {
        DisplayInfoMessage( this, _( "This file was created by an older \
version of PCBnew. It will be stored in the new file format when you save \
this file again."));
    }

    // Reload the corresponding configuration file:
    wxSetWorkingDirectory( wxPathOnly( GetScreen()->m_FileName ) );
    if( Append )
        ReadPcbFile( source, true );
    else
    {
        Read_Config( GetScreen()->m_FileName );

        // Update the option toolbar
        m_DisplayPcbTrackFill = DisplayOpt.DisplayPcbTrackFill;
        m_DisplayModText = DisplayOpt.DisplayModText;
        m_DisplayModEdge = DisplayOpt.DisplayModEdge;
        m_DisplayPadFill = DisplayOpt.DisplayPadFill;
        m_DisplayViaFill = DisplayOpt.DisplayViaFill;

        ReadPcbFile( source, false );
    }

    fclose( source );

    GetScreen()->ClrModify();

    /* If append option: change the initial board name to <oldname>-append.brd */
    if( Append )
    {
        wxString new_filename = GetScreen()->m_FileName.BeforeLast('.');
        if ( ! new_filename.EndsWith(wxT("-append")) )
            new_filename += wxT("-append");

        new_filename += PcbExtBuffer;

        OnModify();
        GetScreen()->m_FileName = new_filename;
    }

    GetScreen()->m_FileName.Replace( WIN_STRING_DIR_SEP, UNIX_STRING_DIR_SEP );

    SetTitle( GetScreen()->m_FileName );
    SetLastProject( GetScreen()->m_FileName );

    /* Rebuild the new pad list (for drc and ratsnet control ...) */
    GetBoard()->m_Status_Pcb = 0;

    /* Reset the items visibility flag when loading a new config
     *  Because it could creates SERIOUS mistakes for the user,
     * if board items are not visible after loading a board...
     * Grid and ratsnest can be left to their previous state
     */
    bool showGrid = IsElementVisible(GRID_VISIBLE);
    bool showRats = IsElementVisible(RATSNEST_VISIBLE);
    SetVisibleAlls( );
    SetElementVisibility(GRID_VISIBLE, showGrid);
    SetElementVisibility(RATSNEST_VISIBLE, showRats);

    // Update info shown by the horizontal toolbars
    GetBoard()->SetCurrentNetClass( NETCLASS::Default );
    m_TrackAndViasSizesList_Changed = true;

    ReFillLayerWidget();

    ReCreateLayerBox( NULL );
    AuxiliaryToolBar_Update_UI();
    syncLayerWidget( );

    // Display the loaded board:
    Zoom_Automatique( false );
    wxSafeYield();      // Needed if we want to see the board now.

    // Compile rastnest and displays net info
    Compile_Ratsnest( NULL, true );
    GetBoard()->DisplayInfo( this );

    /* reset the auto save timer */
    g_SaveTime = time( NULL );


#if 0 && defined(DEBUG)
    // note this freezes up pcbnew when run under the kicad project
    // manager.  runs fine from command prompt.  This is because the kicad
    // project manager redirects stdout of the child pcbnew process to itself,
    // but never reads from that pipe, and that in turn eventually blocks
    // the pcbnew program when the pipe it is writing to gets full.

    // Output the board object tree to stdout, but please run from command prompt:
    GetBoard()->Show( 0, std::cout );
#endif

    return true;
}


/***********************************************************/
bool WinEDA_PcbFrame::SavePcbFile( const wxString& FileName )
/************************************************************/

/* Write the board file
 */
{
    wxFileName  backupFileName;
    wxFileName  pcbFileName;
    wxString    upperTxt;
    wxString    lowerTxt;
    wxString    msg;

    bool        saveok = true;
    FILE*       dest;

    if( FileName == wxEmptyString )
    {
        wxFileDialog dlg( this, _( "Save Board File" ), wxEmptyString,
                          GetScreen()->m_FileName, BoardFileWildcard,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

        if( dlg.ShowModal() == wxID_CANCEL )
            return false;

        GetScreen()->m_FileName = dlg.GetPath();
    }
    else
        GetScreen()->m_FileName = FileName;

    /* If changes are made, update the board date */
    if( GetScreen()->IsModify() )
    {
        GetScreen()->m_Date = GenDate();
    }

    pcbFileName = GetScreen()->m_FileName;

    /* Get the backup file name */
    backupFileName = pcbFileName;
    backupFileName.SetExt( wxT( "000" ) );

    /* If an old backup file exists, delete it.
    if an old board file existes, rename it to the backup file name
    */
    if( backupFileName.FileExists() )
    {
        /* rename the "old" file" from xxx.brd to xxx.000 */
        wxRemoveFile( backupFileName.GetFullPath() ); /* Remove the old file xxx.000 (if exists) */
        if( !wxRenameFile( pcbFileName.GetFullPath(),
                           backupFileName.GetFullPath() ) )
        {
            msg = _( "Warning: unable to create backup file " ) +
                backupFileName.GetFullPath();
            DisplayError( this, msg, 15 );
            saveok = false;
        }
    }
    else
    {
        backupFileName.Clear();
        saveok = false;
    }

    /* Create the file */
    dest = wxFopen( pcbFileName.GetFullPath(), wxT( "wt" ) );

    if( dest == 0 )
    {
        msg = _( "Unable to create " ) + pcbFileName.GetFullPath();
        DisplayError( this, msg );
        saveok = false;
    }

    if( dest )
    {
        GetScreen()->m_FileName = pcbFileName.GetFullPath();
        SetTitle( GetScreen()->m_FileName );

        SavePcbFormatAscii( dest );
        fclose( dest );
    }

    /* Display the file names: */
    MsgPanel->EraseMsgBox();

    if( saveok )
    {
        upperTxt = _( "Backup file: " ) + backupFileName.GetFullPath();
    }

    if( dest )
        lowerTxt = _( "Wrote board file: " );
    else
        lowerTxt = _( "Failed to create " );
    lowerTxt += pcbFileName.GetFullPath();

    Affiche_1_Parametre( this, 1, upperTxt, lowerTxt, CYAN );

    g_SaveTime = time( NULL );    /* Reset timer for the automatic saving */

    GetScreen()->ClrModify();
    return true;
}
