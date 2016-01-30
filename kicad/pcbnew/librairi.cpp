/***************************************/
/* Gestion de la LIBRAIRIE des MODULES */
/***************************************/

/* Fichier LIBRAIRI.CPP */

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"
#include "kicad_string.h"
#include "gestfich.h"
#include "pcbnew.h"
#include "protos.h"

/*
 *  Format de l'entete de la Librairie:
 *  chaine ENTETE-LIBRAIRIE date-heure
 *  $INDEX
 *  liste des noms modules ( 1 nom par ligne)
 *  $EndINDEX
 *  liste des descriptions des Modules
 *  $EndLIBRARY
 */

#define OLD_EXT                    wxT( ".bak" )
#define FILETMP_EXT                wxT( ".$$$" )
#define EXPORT_IMPORT_LASTPATH_KEY wxT( "import_last_path" )

/* Fonctions locales */
static bool CreateDocLibrary( const wxString& LibName );

/*********************************************************/
MODULE* WinEDA_ModuleEditFrame::Import_Module( wxDC* DC )
/*********************************************************/

/**
 * Function Import_Module
 * Read a file containing only one footprint.
 * Used to import (after exporting) a footprint
 * Exported files  have the standart ext .emp
 * This is the same format as .mod files but restricted to only one footprint
 * The import function can also read gpcb footprint file, in Newlib format
 * (One footprint per file, Newlib files have no special ext.)
 * @param DC = Current Device Context (can be NULL)
 */
{
    int       NbLine = 0;
    char      Line[1024];
    wxString  CmpFullFileName;
    FILE*     dest;
    MODULE*   module = NULL;
    bool      Footprint_Is_GPCB_Format = false;
    wxString  mask = wxT( "*.*;" ); mask += EXT_CMP_MASK;

    wxString  LastOpenedPathForLoading;
    wxConfig* Config = wxGetApp().m_EDA_Config;

    if( Config )
        Config->Read( EXPORT_IMPORT_LASTPATH_KEY, &LastOpenedPathForLoading );

    /* Lecture Fichier module */
    CmpFullFileName = EDA_FileSelector( _( "Import Module:" ),
                                        LastOpenedPathForLoading,   /* Chemin par defaut */
                                        wxEmptyString,              /* nom fichier par defaut */
                                        wxEmptyString,              /* extension par defaut */
                                        mask,                       /* Masque d'affichage */
                                        this,
                                        wxFD_OPEN,
                                        true
                                        );

    if( CmpFullFileName == wxEmptyString )
        return NULL;

    if( ( dest = wxFopen( CmpFullFileName, wxT( "rt" ) ) ) == NULL )
    {
        wxString msg;
        msg.Printf( _( "File <%s> not found" ), CmpFullFileName.GetData() );
        DisplayError( this, msg );
        return NULL;
    }

    if( Config )    // Save file path
    {
        LastOpenedPathForLoading = wxPathOnly( CmpFullFileName );
        Config->Write( EXPORT_IMPORT_LASTPATH_KEY, LastOpenedPathForLoading );
    }

    /* Read header and test file type */
    GetLine( dest, Line, &NbLine );
    if( strnicmp( Line, ENTETE_LIBRAIRIE, L_ENTETE_LIB ) != 0 )
    {
        if( strnicmp( Line, "Element", 7 ) == 0 )
            Footprint_Is_GPCB_Format = true;
        else
        {
            fclose( dest );
            DisplayError( this, _( "Not a module file" ) );
            return NULL;
        }
    }

    /* Read file: Search the description starting line (skip lib header)*/
    if( !Footprint_Is_GPCB_Format )
    {
        while( GetLine( dest, Line, &NbLine ) != NULL )
        {
            if( strnicmp( Line, "$MODULE", 7 ) == 0 )
                break;
        }
    }

    module = new MODULE( GetBoard() );

    if( Footprint_Is_GPCB_Format )
    {
        fclose( dest );
        module->Read_GPCB_Descr( CmpFullFileName );
    }
    else
    {
        module->ReadDescr( dest, &NbLine );
        fclose( dest );
    }

    /* Insert footprint in list*/
    GetBoard()->Add( module );

    /* Display info : */
    module->Display_Infos( this );
    Place_Module( module, DC );
    GetBoard()->m_Status_Pcb = 0;
    build_liste_pads();

    return module;
}


/************************************************************************/
void WinEDA_ModuleEditFrame::Export_Module( MODULE* ptmod, bool createlib )
/************************************************************************/

/**
 * Function Export_Module
 * Create a file containing only one footprint.
 * Used to export a footprint
 * Exported files  have the standart ext .emp
 * This is the same format as .mod files but restricted to only one footprint
 * So Create a new lib (which will contains one module) and export a footprint is basically the same thing
 * @param DC = Current Device Context (can be NULL)
 * @param createlib : true = use default lib path to create lib
 *                    false = use current path or last used path to export footprint
 */
{
    wxString  FullFileName, Mask( wxT( "*" ) );
    char      Line[1025];
    FILE*     dest;
    wxString  msg, path;
    wxConfig* Config = wxGetApp().m_EDA_Config;

    if( ptmod == NULL )
        return;

    ptmod->m_LibRef = ptmod->m_Reference->m_Text;
    FullFileName    = ptmod->m_LibRef;
    FullFileName   += createlib ? LibExtBuffer : EXT_CMP;

    Mask += createlib ? LibExtBuffer : EXT_CMP;

    if( createlib )
        path = g_RealLibDirBuffer;
    else if( Config )
        Config->Read( EXPORT_IMPORT_LASTPATH_KEY, &path );

    FullFileName = EDA_FileSelector( createlib ? _( "Create lib" ) : _( "Export Module:" ),
                                     path,                                  /* Chemin par defaut */
                                     FullFileName,                          /* nom fichier par defaut */
                                     createlib ? LibExtBuffer : EXT_CMP,    /* extension par defaut */
                                     Mask,                                  /* Masque d'affichage */
                                     this,
                                     wxFD_SAVE,
                                     true
                                     );

    if( FullFileName.IsEmpty() )
        return;

    if( wxFileExists( FullFileName ) )
    {
        msg.Printf( _( "File %s exists, OK to replace ?" ),
                   FullFileName.GetData() );
        if( !IsOK( this, msg ) )
            return;
    }

    /* Generation du fichier Empreinte */
    if( ( dest = wxFopen( FullFileName, wxT( "wt" ) ) ) == NULL )
    {
        msg.Printf( _( "Unable to create <%s>" ), FullFileName.GetData() );
        DisplayError( this, msg );
        return;
    }

    if( !createlib && Config )  // Save file path
    {
        path = wxPathOnly( FullFileName );
        Config->Write( EXPORT_IMPORT_LASTPATH_KEY, path );
    }

    fprintf( dest, "%s  %s\n", ENTETE_LIBRAIRIE, DateAndTime( Line ) );
    fputs( "$INDEX\n", dest );

    fprintf( dest, "%s\n", CONV_TO_UTF8( ptmod->m_LibRef ) );
    fputs( "$EndINDEX\n", dest );

    GetBoard()->m_Modules->Save( dest );

    fputs( "$EndLIBRARY\n", dest );
    fclose( dest );
    msg.Printf( _( "Module exported in file <%s>" ), FullFileName.GetData() );
    DisplayInfo( this, msg );
}


/**********************************************************/
void WinEDA_ModuleEditFrame::Delete_Module_In_Library( const
                                                       wxString& libname )
/**********************************************************/
{
    int      ii, NoFound = 1, LineNum = 0;
    char     Line[1024], Name[256];
    wxString NewLib, OldLib;
    FILE*    dest, * lib_module;
    wxString CmpName, msg;

    /* Demande du nom du composant a supprimer */
    CmpName = Select_1_Module_From_List( this, libname, wxEmptyString, wxEmptyString );
    if( CmpName == wxEmptyString )
        return;

    /* Confirmation */
    msg.Printf( _( "Ok to delete module %s in library %s" ),
               CmpName.GetData(), libname.GetData() );
    if( !IsOK( this, msg ) )
        return;

    OldLib = libname;

    if( ( lib_module = wxFopen( OldLib, wxT( "rt" ) ) )  == NULL )
    {
        wxString msg;
        msg = _( "Library " ) + OldLib + _( " not found" );
        DisplayError( this, msg );
        return;
    }


    /* lecture entete */
    GetLine( lib_module, Line, &LineNum );

    if( strnicmp( Line, ENTETE_LIBRAIRIE, L_ENTETE_LIB ) != 0 )
    {
        DisplayError( this, _( "Not a Library file" ) );
        fclose( lib_module );
        return;
    }

    /* lecture des nom des composants  */
    while( GetLine( lib_module, Line, &LineNum ) )
    {
        if( strnicmp( Line, "$INDEX", 6 ) == 0 )
        {
            while( GetLine( lib_module, Line, &LineNum ) )
            {
                StrPurge( Line );
                msg = CONV_FROM_UTF8( Line );
                if( CmpName.CmpNoCase( msg ) == 0 ) /* composant trouve */
                {
                    NoFound = 0; break;
                }
                if( strnicmp( Line, "$EndINDEX", 9 ) == 0 )
                    break;
            }
        }
        if( strnicmp( Line, "$EndINDEX", 9 ) == 0 )
            break;
    }

    if( NoFound )
    {
        fclose( lib_module );
        msg.Printf( _( "Module [%s] not found" ), CmpName.GetData() );
        DisplayError( this, msg );
        return;
    }

    /* Creation de la nouvelle librairie */
    NewLib = OldLib;
    ChangeFileNameExt( NewLib, FILETMP_EXT );
    if( ( dest = wxFopen( NewLib, wxT( "wt" ) ) ) == NULL )
    {
        fclose( lib_module );
        wxString msg;
        msg = _( "Unable to create " ) + NewLib;
        DisplayError( this, msg );
        return;
    }

    wxBeginBusyCursor();

    /* Creation de l'entete avec nouvelle date */
    fprintf( dest, ENTETE_LIBRAIRIE );
    fprintf( dest, "  %s\n$INDEX\n", DateAndTime( Line ) );

    fseek( lib_module, 0, 0 ); GetLine( lib_module, Line, &ii );
    while( GetLine( lib_module, Line, &ii ) )
    {
        if( strnicmp( Line, "$M", 2 ) == 0 )
            break;
        if( strnicmp( Line, "$INDEX", 6 ) == 0 )
        {
            while( GetLine( lib_module, Line, &ii ) )
            {
                if( strnicmp( Line, "$EndINDEX", 9 ) == 0 )
                    break;
                StrPurge( Line );
                msg = CONV_FROM_UTF8( Line );
                if( CmpName.CmpNoCase( msg ) != 0 )
                    fprintf( dest, "%s\n", Line );
            }
        }
        if( strnicmp( Line, "$EndINDEX", 9 ) == 0 )
            break;
    }

    fprintf( dest, "$EndINDEX\n" );

    /* Copie des modules */
    while( GetLine( lib_module, Line, &LineNum ) )
    {
        StrPurge( Line );
        if( strnicmp( Line, "$MODULE", 7 ) == 0 )
        {
            sscanf( Line + 7, " %s", Name );
            msg = CONV_FROM_UTF8( Name );
            if( msg.CmpNoCase( CmpName ) == 0 )
            {
                /* suppression ancien module */
                while( GetLine( lib_module, Line, &LineNum ) )
                {
                    if( strnicmp( Line, "$EndMODULE", 9 ) == 0 )
                        break;
                }

                continue;
            }
        }
        fprintf( dest, "%s\n", Line );
    }

    fclose( lib_module );
    fclose( dest );

    wxEndBusyCursor();

    /* Le fichier ancienne librairie est renommee en .bak */
    wxString BakFilename = OldLib;
    ChangeFileNameExt( BakFilename, OLD_EXT );

    if( wxFileExists( BakFilename ) )
        wxRemoveFile( BakFilename );

    if( !wxRenameFile( OldLib, BakFilename ) )
    {
        DisplayError( this, wxT( "Librairi.cpp: rename .bak err" ) );
        return;
    }

    /* Le fichier temporaire est renommee comme l'ancienne Lib */
    if( !wxRenameFile( NewLib, OldLib ) )
    {
        DisplayError( this, wxT( "Librairi.cpp: rename err 2" ) );
        return;
    }

    msg.Printf( _( "Component %s deleted in library %s" ), CmpName.GetData(), OldLib.GetData() );
    Affiche_Message( msg );

    CreateDocLibrary( OldLib );
}


/***********************************************************************/
void WinEDA_BasePcbFrame::Archive_Modules( const wxString& LibName,
                                           bool            NewModulesOnly )
/***********************************************************************/

/*
 *  Sauve en Librairie:
 *  tous les nouveaux modules ( c.a.d. les modules
 *      n'existant pas deja (si NewModulesOnly == true)
 *  tous les modules (si NewModulesOnly == FALSE)
 */
{
    int      ii, NbModules = 0;
    float    Pas;
    MODULE*  Module;
    wxString FullFileName = LibName;

    if( GetBoard()->m_Modules == NULL )
    {
        DisplayInfo( this, _( " No modules to archive!" ) );
        return;
    }

    if( FullFileName.IsEmpty() )
    {
        wxString Mask = wxT( "*" ) + LibExtBuffer;
        FullFileName = EDA_FileSelector( _( "Library" ),
                                         g_RealLibDirBuffer,    /* Chemin par defaut */
                                         FullFileName,          /* nom fichier par defaut */
                                         LibExtBuffer,          /* extension par defaut */
                                         Mask,                  /* Masque d'affichage */
                                         this,
                                         wxFD_SAVE,
                                         true
                                         );

        if( FullFileName.IsEmpty() )
            return;
    }

    bool file_exists = wxFileExists( FullFileName );
    if( !NewModulesOnly && file_exists )
    {
        wxString msg;
        msg.Printf( _( "File %s exists, OK to replace ?" ), FullFileName.GetData() );
        if( !IsOK( this, msg ) )
            return;
    }

    DrawPanel->m_AbortRequest = FALSE;

    // Create a new, empty library if no old lib, or if archive all modules
    if( !NewModulesOnly || !file_exists )
    {
        FILE* lib_module;
        if( ( lib_module = wxFopen( FullFileName, wxT( "w+t" ) ) )  == NULL )
        {
            wxString msg = _( "Unable to create " ) + FullFileName;
            DisplayError( this, msg );
            return;
        }
        char Line[256];
        fprintf( lib_module, "%s  %s\n", ENTETE_LIBRAIRIE, DateAndTime( Line ) );
        fputs( "$INDEX\n", lib_module );
        fputs( "$EndINDEX\n", lib_module );
        fputs( "$EndLIBRARY\n", lib_module );
        fclose( lib_module );
    }

    /* Calcul du nombre de modules */
    Module = (MODULE*) GetBoard()->m_Modules;
    for( ; Module != NULL; Module = (MODULE*) Module->Next() )
        NbModules++;

    Pas = (float) 100 / NbModules;
    DisplayActivity( 0, wxEmptyString );

    Module = (MODULE*) GetBoard()->m_Modules;
    for( ii = 1; Module != NULL; ii++, Module = (MODULE*) Module->Next() )
    {
        if( Save_Module_In_Library( FullFileName, Module,
                                    NewModulesOnly ? FALSE : true, FALSE, false ) == 0 )
            break;
        DisplayActivity( (int) ( ii * Pas ), wxEmptyString );
        /* Tst demande d'arret de sauvegarde ( key ESCAPE actionnee ) */
        if( DrawPanel->m_AbortRequest )
            break;
    }

    CreateDocLibrary( FullFileName );
}


/*****************************************************************/
int WinEDA_BasePcbFrame::Save_Module_In_Library( const wxString& aLibName,
                                                 MODULE* aModule, bool aOverwrite,
                                                 bool aDisplayDialog, bool aCreateDocFile )
/*****************************************************************/

/** Function Save_Module_In_Library
 *  Save in an existing library a given footprint
 * @param aLibName = name of the library to use
 * @param aModule = the given footprint
 * @param aOverwrite = true to overwrite an existing footprint, false to abort an existing footprint is found
 * @param aDisplayDialog = true to display a dialog to enter or confirm the footprint name
 * @param aCreateDocFile = true to creates the associated doc file
 * @return :  1 if OK, 0 if abort
 */
{
    int      newmodule, end;
    int      LineNum = 0, tmp;
    char     Name[256], Line[1024];
    wxString Name_Cmp;
    wxString NewLib, OldLib, msg;
    FILE*    lib_module, * dest;
    bool     added = true;

    aModule->Display_Infos( this );

    if( !wxFileExists( aLibName ) )
    {
        msg.Printf( _( "Library %s not found" ), aLibName.GetData() );
        DisplayError( this, msg );
        return 0;
    }


    /* Ask for the footprint name in lib */
    Name_Cmp = aModule->m_LibRef;

    if( aDisplayDialog )
    {
        Get_Message( _( "Name:" ), _( "Save module" ), Name_Cmp, this );
        if( Name_Cmp.IsEmpty() )
            return 0;
        Name_Cmp.Trim( true );
        Name_Cmp.Trim( FALSE );
        aModule->m_LibRef = Name_Cmp;
    }

    if( ( lib_module = wxFopen( aLibName, wxT( "rt" ) ) ) == NULL )
    {
        msg.Printf( _( "Unable to open %s" ), aLibName.GetData() );
        DisplayError( this, msg );
        return 0;
    }

    /* lRead library file : library header */
    GetLine( lib_module, Line, &LineNum );
    if( strnicmp( Line, ENTETE_LIBRAIRIE, L_ENTETE_LIB ) != 0 )
    {
        fclose( lib_module );
        msg.Printf( _( "File %s is not a eeschema library" ), aLibName.GetData() );
        DisplayError( this, msg );
        return 0;
    }

    /* Reaf footprints in lib: - search for an existing footprint */
    newmodule = 1; end = 0;
    while( !end && GetLine( lib_module, Line, &LineNum ) )
    {
        if( Line[0] != '$' )
            continue;
        if( strncmp( Line + 1, "INDEX", 5 ) != 0 )
            continue;

        while( GetLine( lib_module, Line, &LineNum ) )
        {
            if( strncmp( Line, "$EndINDEX", 9 ) == 0 )
            {
                end = 1; break;
            }

            StrPurge( Line );
            msg = CONV_FROM_UTF8( Line );
            if( Name_Cmp.CmpNoCase( msg ) == 0 ) /* an existing footprint is found */
            {
                added     = FALSE;
                newmodule = 0;
                if( aDisplayDialog )
                {
                    msg = _( "Module exists\n Line: " );
                    msg << LineNum;
                    Affiche_Message( msg );
                }
                if( !aOverwrite )    /* lDo not save the given footprint: an old one exists */
                {
                    fclose( lib_module ); return 1;
                }
                end = 1; break;
            }
        }
    }

    fclose( lib_module );

    /* Creates the new library */

    if( ( lib_module = wxFopen( aLibName, wxT( "rt" ) ) )  == NULL )
    {
        DisplayError( this, wxT( "Librairi.cpp: Error oldlib not found" ) );
        return 0;
    }

    NewLib = aLibName;
    ChangeFileNameExt( NewLib, FILETMP_EXT );
    if( ( dest = wxFopen( NewLib, wxT( "w+t" ) ) )  == NULL )
    {
        fclose( lib_module );
        msg = _( "Unable to create " ) + NewLib;
        DisplayError( this, msg );
        return 0;
    }

    wxBeginBusyCursor();

    /* Create the library header with a new date */
    fprintf( dest, ENTETE_LIBRAIRIE );
    fprintf( dest, "  %s\n$INDEX\n", DateAndTime( Line ) );

    LineNum = 0;
    GetLine( lib_module, Line, &LineNum );
    while( GetLine( lib_module, Line, &LineNum ) )
    {
        StrPurge( Line );
        if( strnicmp( Line, "$M", 2 ) == 0 )
            break;
        if( strnicmp( Line, "$INDEX", 6 ) == 0 )
        {
            while( GetLine( lib_module, Line, &LineNum ) )
            {
                if( strnicmp( Line, "$EndINDEX", 9 ) == 0 )
                    break;
                fprintf( dest, "%s\n", Line );
            }
        }
        if( newmodule )
            fprintf( dest, "%s\n", CONV_TO_UTF8( Name_Cmp ) );
        if( strnicmp( Line, "$EndINDEX", 0 ) == 0 )
            break;
    }

    fprintf( dest, "$EndINDEX\n" );

    /* Copy footprints, until the old footprint to delete */
    while( GetLine( lib_module, Line, &LineNum ) )
    {
        StrPurge( Line );
        if( strnicmp( Line, "$EndLIBRARY", 8 ) == 0 )
            continue;
        if( strnicmp( Line, "$MODULE", 7 ) == 0 )
        {
            sscanf( Line + 7, " %s", Name );
            msg = CONV_FROM_UTF8( Name );
            if( msg.CmpNoCase( Name_Cmp ) == 0 )
            {
                /* skip old footprint descr (delete from the lib)*/
                while( GetLine( lib_module, Line, &LineNum ) )
                {
                    if( strnicmp( Line, "$EndMODULE", 9 ) == 0 )
                        break;
                }

                continue;
            }
        }
        fprintf( dest, "%s\n", Line );
    }

    /* Write the new footprint ( append it to the list of footprint ) */
    tmp = aModule->m_TimeStamp; aModule->m_TimeStamp = 0;
    aModule->Save( dest );
    fprintf( dest, "$EndLIBRARY\n" );
    aModule->m_TimeStamp = tmp;

    fclose( dest );  fclose( lib_module );

    wxEndBusyCursor();

    /* The old library file is renamed .bak */
    OldLib = aLibName;
    ChangeFileNameExt( OldLib, OLD_EXT );

    if( wxFileExists( OldLib ) )
        wxRemoveFile( OldLib );

    if( !wxRenameFile( aLibName, OldLib ) )
        DisplayError( this, wxT( "Librairi.cpp: rename .bak err" ) );

    /* The new library file is renamed */
    if( !wxRenameFile( NewLib, aLibName ) )
    {
        DisplayError( this, wxT( "Librairi.cpp: rename NewLib err" ) );
        return 0;
    }

    /* creates the new .dcm doc file corresponding to the new library */
    if ( aCreateDocFile )
        CreateDocLibrary( aLibName );

    if( aDisplayDialog )
    {
        msg  = _( "Component " ); msg += Name_Cmp;
        msg += added ? _( " added in " ) : _( " replaced in " );
        msg += aLibName;
        Affiche_Message( msg );
    }

    return 1;
}


/************************************************************************************/
MODULE* WinEDA_BasePcbFrame::Create_1_Module( wxDC* DC, const wxString& module_name )
/************************************************************************************/

/* Create a new module or footprint : A new module is tartted with 2 texts :
 *          First = REFERENCE
 *          Second = VALUE: "VAL**"
 *  the new module is added on begining of the linked list of modules
 */

{
    MODULE*  Module;
    wxString Line;
    wxPoint  newpos;

    /* Ask fo the new module reference */
    if( module_name.IsEmpty() )
    {
        if( Get_Message( _( "Module Reference:" ), _( "Module Creation" ), Line, this ) != 0 )
        {
            DisplayInfo( this, _( "No reference, aborted" ) );
            return NULL;
        }
    }
    else
        Line = module_name;
    Line.Trim( true );
    Line.Trim( FALSE );

    // Creates the new module and add it to the head of the linked list of modules
    Module = new MODULE( GetBoard() );

    GetBoard()->Add( Module );

    /* Update parameters: position, timestamp ... */
    newpos = GetScreen()->m_Curseur;
    Module->SetPosition( newpos );
    Module->m_LastEdit_Time = time( NULL );

    /* Update its name in lib */
    Module->m_LibRef = Line;

    /* Update reference: */
    Module->m_Reference->m_Text = Line;
    Module->m_Reference->SetWidth( ModuleTextWidth );
    Module->m_Reference->m_Size = ModuleTextSize;

    /* Set the value field to a default value */
    Module->m_Value->m_Text = wxT( "VAL**" );
    Module->m_Value->SetWidth( ModuleTextWidth );
    Module->m_Value->m_Size = ModuleTextSize;

    Module->SetPosition( wxPoint( 0, 0 ) );

    Module->Display_Infos( this );
    return Module;
}


/*******************************************************/
void WinEDA_ModuleEditFrame::Select_Active_Library()
/*******************************************************/
{
    if( g_LibName_List.GetCount() == 0 )
        return;

    WinEDAListBox* LibListBox = new WinEDAListBox( this, _( "Active Lib:" ),
                                                  NULL, m_CurrentLib, NULL,
                                                  wxColour( 200, 200, 255 ) );

    LibListBox->InsertItems( g_LibName_List );

    int ii = LibListBox->ShowModal();
    if( ii >= 0 )
        m_CurrentLib = LibListBox->GetTextSelection();

    LibListBox->Destroy();

    SetTitle( _( "Module Editor (lib: " ) + m_CurrentLib + wxT( ")" ) );
    return;
}


/**********************************************************************/
int WinEDA_ModuleEditFrame::Create_Librairie( const wxString& LibName )
/**********************************************************************/
{
    FILE*    lib_module;
    wxString msg;
    char     cbuf[256];

    if( wxFileExists( LibName ) )
    {
        msg = _( "Library exists " ) + LibName;
        DisplayError( this, msg );
        return 0;
    }

    if( ( lib_module = wxFopen( LibName, wxT( "wt" ) ) )  == NULL )
    {
        msg = _( "Unable to create " ) + LibName;
        DisplayError( this, msg );
        return -1;
    }

    /* Ecriture de l'entete de la nouvelle librairie */
    if( fprintf( lib_module, ENTETE_LIBRAIRIE ) == 0 )
    {
        msg = _( "Create error " ) + LibName;
        DisplayError( this, msg );
        fclose( lib_module ); return -1;
    }

    fprintf( lib_module, "  %s\n", DateAndTime( cbuf ) );
    fputs( "$INDEX\n", lib_module );
    fputs( "$EndINDEX\n", lib_module );
    fclose( lib_module );

    return 1;
}


/******************************************************/
static bool CreateDocLibrary( const wxString& LibName )
/*****************************************************/

/* Creation du fichier .dcm associe a la librairie LibName
 *  (full file name)
 */
{
    char     Line[1024];
    char     cbuf[256];
    wxString Name, Doc, KeyWord;
    wxString LibDocName;
    FILE*    LibMod, * LibDoc;

    LibDocName = LibName;
    ChangeFileNameExt( LibDocName, EXT_DOC );

    LibMod = wxFopen( LibName, wxT( "rt" ) );
    if( LibMod == NULL )
        return FALSE;

    /* lecture entete librairie*/
    GetLine( LibMod, Line, NULL, sizeof(Line) - 1 );
    if( strnicmp( Line, ENTETE_LIBRAIRIE, L_ENTETE_LIB ) != 0 )
    {
        fclose( LibMod );
        return FALSE;
    }

    LibDoc = wxFopen( LibDocName, wxT( "wt" ) );
    if( LibDoc == NULL )
    {
        fclose( LibMod );
        return FALSE;
    }
    fprintf( LibDoc, ENTETE_LIBDOC );
    fprintf( LibDoc, "  %s\n", DateAndTime( cbuf ) );

    /* Lecture de la librairie */
    Name = Doc = KeyWord = wxEmptyString;
    while( GetLine( LibMod, Line, NULL, sizeof(Line) - 1 ) )
    {
        if( Line[0] != '$' )
            continue;
        if( strnicmp( Line, "$MODULE", 6 ) == 0 )
        {
            while( GetLine( LibMod, Line, NULL, sizeof(Line) - 1 ) )
            {
                if( Line[0] == '$' )
                {
                    if( Line[1] == 'E' )
                        break;
                    if( Line[1] == 'P' ) /* Pad Descr */
                    {
                        while( GetLine( LibMod, Line, NULL, sizeof(Line) - 1 ) )
                        {
                            if( (Line[0] == '$') && (Line[1] == 'E') )
                                break;
                        }
                    }
                }
                if( Line[0] == 'L' ) /* LibName */
                    Name = CONV_FROM_UTF8( StrPurge( Line + 3 ) );

                if( Line[0] == 'K' ) /* KeyWords */
                    KeyWord = CONV_FROM_UTF8( StrPurge( Line + 3 ) );

                if( Line[0] == 'C' ) /* Doc */
                    Doc = CONV_FROM_UTF8( StrPurge( Line + 3 ) );
            }

            if( (Name != wxEmptyString) && ( (Doc != wxEmptyString) || (KeyWord != wxEmptyString) ) ) /* Generation de la doc du composant */
            {
                fprintf( LibDoc, "#\n$MODULE %s\n", CONV_TO_UTF8( Name ) );
                fprintf( LibDoc, "Li %s\n", CONV_TO_UTF8( Name ) );
                if( Doc != wxEmptyString )
                    fprintf( LibDoc, "Cd %s\n", CONV_TO_UTF8( Doc ) );

                if( KeyWord  != wxEmptyString )
                    fprintf( LibDoc, "Kw %s\n", CONV_TO_UTF8( KeyWord ) );

                fprintf( LibDoc, "$EndMODULE\n" );
            }
            Name = Doc = KeyWord = wxEmptyString;
        }     /* Fin lecture desc 1 module */

        if( strnicmp( Line, "$INDEX", 6 ) == 0 )
        {
            while( GetLine( LibMod, Line, NULL, sizeof(Line) - 1 ) )
            {
                if( strnicmp( Line, "$EndINDEX", 9 ) == 0 )
                    break;
            }

            /* Fin Lecture INDEX */
        }
    }

    /* Fin lecture 1 Librairie */

    fclose( LibMod );
    fprintf( LibDoc, "#\n$EndLIBDOC\n" );
    fclose( LibDoc );
    return true;
}
