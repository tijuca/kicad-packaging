/************************************************/
/*	 MODULE: gestfich.cpp						*/
/*	 ROLE: fonctions de gestion de fichiers	*/
/************************************************/

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/mimetype.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef  __WINDOWS__
#ifndef _MSC_VER
#include <dir.h>
#endif
#endif

#include "fctsys.h"
#include "common.h"
#include "wxstruct.h"


// Path list for online help
static wxString s_HelpPathList[] = {
#ifdef __WINDOWS__
	wxT("c:/kicad/help/"),
	wxT("d:/kicad/help/"),
#else
	wxT("/usr/local/kicad/help/"),
	wxT("/usr/share/doc/kicad/"),
#endif
	wxT("end_list")	// End of list symbol, do not change
};

// Path list for kicad data files
static wxString s_KicadDataPathList[] = {
#ifdef __WINDOWS__
	wxT("c:/kicad/"),
	wxT("d:/kicad/"),
#else
	wxT("/usr/local/kicad/"),
	wxT("/usr/share/kicad/"),
#endif
	wxT("end_list")	// End of list symbol, do not change
};

// Path list for kicad binary files
static wxString s_KicadBinaryPathList[] = {
#ifdef __WINDOWS__
	wxT("c:/kicad/winexe/"),
	wxT("d:/kicad/winexe/"),
#else
	wxT("/usr/local/kicad/linux/"),
	wxT("/usr/local/bin/"),
	wxT("/usr/bin/"),
#endif
	wxT("end_list")	// End of list symbol, do not change
};
extern WinEDA_App * EDA_Appl;


/***************************************************************************/
wxString MakeReducedFileName( const wxString & fullfilename,
						const wxString & default_path,
						const wxString & default_ext)
/***************************************************************************/

/* Calcule le nom "reduit" d'un fichier d'apres les chaines
	fullfilename = nom complet
	default_path = prefixe (chemin) par defaut
	default_ext = extension	par defaut

	retourne le nom reduit, c'est a dire:
	sans le chemin si le chemin est default_path
	avec ./ si si le chemin est le chemin courant
	sans l'extension si l'extension est default_ext

	Renvoie un chemin en notation unix ('/' en separateur de repertoire)
*/
{
wxString reduced_filename = fullfilename;
wxString Cwd, ext, path;

	Cwd = default_path;
	ext = default_ext;
	path = wxPathOnly(reduced_filename) + UNIX_STRING_DIR_SEP;
	reduced_filename.Replace(WIN_STRING_DIR_SEP,UNIX_STRING_DIR_SEP);
	Cwd.Replace(WIN_STRING_DIR_SEP,UNIX_STRING_DIR_SEP);
	if ( Cwd.Last() != '/' ) Cwd += UNIX_STRING_DIR_SEP;
	path.Replace(WIN_STRING_DIR_SEP,UNIX_STRING_DIR_SEP);

#ifdef __WINDOWS__
	path.MakeLower();
	Cwd.MakeLower();
	ext.MakeLower();
#endif

	// Si le fichier est dans chemin par defaut -> suppression du chemin par defaut
	wxString root_path = path.Left(Cwd.Length());
	if ( root_path == Cwd )
	{
		reduced_filename.Remove(0, Cwd.Length());
	}
	else	// Si fichier dans repertoire courant -> chemin = ./
	{
		Cwd = wxGetCwd() + UNIX_STRING_DIR_SEP;
#ifdef __WINDOWS__
		Cwd.MakeLower();
#endif
		Cwd.Replace(WIN_STRING_DIR_SEP,UNIX_STRING_DIR_SEP);
		if ( path == Cwd )
		{	// lib est dans r�pertoire courant -> Chemin = "./"
			reduced_filename.Remove(0, Cwd.Length());
			wxString tmp = wxT("./") + reduced_filename;
			reduced_filename = tmp;
		}
	}

	// Suppression extension standard:
	if ( !ext.IsEmpty() && reduced_filename.Contains(ext) )
		reduced_filename.Truncate(reduced_filename.Length() - ext.Length());

	return(reduced_filename);
}




/***************************************************************************/
wxString MakeFileName( const wxString & dir,
		const wxString & shortname, const wxString & ext)
/***************************************************************************/

/* Calcule le nom complet d'un fichier d'apres les chaines
	dir = prefixe (chemin)	  (peut etre "")
	shortname = nom avec ou sans chemin ou extension
	ext = extension	(peut etre "")

	si shortname possede deja un chemin ou une extension, elles
	ne seront pas modifiees

	retourne la chaine calculee
*/
{
wxString fullfilename;
int ii;

	if ( ! dir.IsEmpty() )
	{
		if( ! shortname.Contains( UNIX_STRING_DIR_SEP ) && ! shortname.Contains( WIN_STRING_DIR_SEP)
			&& ! shortname.Contains( wxT(":") ) )
		{ /* aucun chemin n'est donne */
			fullfilename = dir;
		}
	}

	fullfilename += shortname;
	fullfilename.Replace( WIN_STRING_DIR_SEP, UNIX_STRING_DIR_SEP );

	/* Placement de l'extension s'il n'y en a pas deja une */
	if( ext.IsEmpty() ) return(fullfilename);

	/* Recherche d'une eventuelle extension */
	ii = fullfilename.Length();	/* Pointe la fin du texte */
	for ( ; ii >= 0 ; ii-- )
	{
		if ( fullfilename.GetChar(ii) == '/' )
		{
		/* Pas d'extension: placement de l'extension standard */
			fullfilename += ext;
			break;
		}
		if ( fullfilename.GetChar(ii) == '.' )	/* extension trouvee */
			break;
	}

	return(fullfilename);
}


/*************************************************************************/
void ChangeFileNameExt( wxString & FullFileName, const wxString & NewExt )
/**************************************************************************/
/* Change l'extension du "filename FullFileName" en NewExt.
*/
{
wxString FileName;

	FileName = FullFileName.BeforeLast('.');
	if ( !FileName.IsEmpty() ) FileName += NewExt;
	else  FileName = FullFileName + NewExt;

	if ( FileName.StartsWith( wxT("\"")) && ( FileName.Last() != '"' ) )
		FileName += wxT("\"");
	FullFileName = FileName;
}

/*******************************************/
void AddDelimiterString( wxString & string )
/*******************************************/
/* ajoute un " en debut et fin de string s'il n'y en a pas deja.
*/
{
wxString text;

	if ( ! string.StartsWith( wxT("\"")) ) text = wxT("\"");
	text += string;
	if( (text.Last() != '"' ) || (text.length() <= 1) ) text += wxT("\"");
	string = text;
}



	/*************************************/
	/* Fonction de selection de Repertoires */
	/*************************************/

bool EDA_DirectorySelector(const wxString & Title,		/* Titre de la fenetre */
					wxString & Path,			/* Chemin par defaut */
					int flag,				/* reserve */
					wxWindow * Frame,	/* parent frame */
					const wxPoint & Pos)
{
int ii;
bool selected = FALSE;

	wxDirDialog * DirFrame = new wxDirDialog(
					Frame,
					wxString(Title),
					Path,		/* Chemin par defaut */
					flag,
					Pos );

	ii = DirFrame->ShowModal();
	if ( ii == wxID_OK )
		{
		Path = DirFrame->GetPath();
		selected = TRUE;
		}

	DirFrame->Destroy();
	return selected;
}

	/******************************/
	/* Selection file dialog box: */
	/******************************/

wxString EDA_FileSelector(const wxString & Title,	/* Dialog title */
					const wxString & Path,			/* Default path */
					const wxString &FileName,		/* default filename */
					const wxString & Ext,			/* default filename extension */
					const wxString & Mask,			/* filter for filename list */
					wxWindow * Frame,				/* parent frame */
					int flag,						/* wxFD_SAVE, wxFD_OPEN ..*/
					const bool keep_working_directory,	/* true = keep the current path */
					const wxPoint & Pos)
{
wxString fullfilename;
wxString curr_cwd = wxGetCwd();
wxString defaultname = FileName;
wxString defaultpath = Path;

	defaultname.Replace(wxT("/"), STRING_DIR_SEP);
	defaultpath.Replace(wxT("/"), STRING_DIR_SEP);
	if ( defaultpath.IsEmpty() ) defaultpath = wxGetCwd();

    wxSetWorkingDirectory( defaultpath );

	fullfilename = wxFileSelector( wxString(Title),
					defaultpath,
					defaultname,
					Ext,
					Mask,
					flag,				/* options d'affichage (wxFD_OPEN, wxFD_SAVE .. */
					Frame,
					Pos.x, Pos.y );

	if ( keep_working_directory ) wxSetWorkingDirectory(curr_cwd);

	return fullfilename;
}

/********************************************************/
wxString FindKicadHelpPath(void)
/********************************************************/
/* Find absolute path for kicad/help (or kicad/help/<language>) */
/* Find path kicad/help/xx/ ou kicad/help/:
	from BinDir
	else from environment variable KICAD
	else from one of s_HelpPathList
	typically c:\kicad\help or /usr/local/kicad/help or /usr/share/doc/kicad
	(must have kicad in path name)

	xx = iso639-1 language id (2 letters (generic) or 4 letters):
	fr = french (or fr_FR)
	en = English (or en_GB or en_US ...)
	de = deutch
	es = spanish
	pt = portuguese (or pt_BR ...)

	default = en (if not found = fr)

*/
{
wxString FullPath, LangFullPath, tmp;
wxString LocaleString;
bool PathFound = FALSE;

	/* find kicad/help/ */
	tmp = EDA_Appl->m_BinDir;
	if ( tmp.Last() == '/' ) tmp.RemoveLast();
	FullPath = tmp.BeforeLast('/'); 	// Idem cd ..
	FullPath += wxT("/help/");
	LocaleString = EDA_Appl->m_Locale->GetCanonicalName();

	if ( FullPath.Contains( wxT("kicad")) )
	{
		if ( wxDirExists(FullPath) ) PathFound = TRUE;
	}

	/* find kicad/help/ from environment variable  KICAD */
	if ( ! PathFound && EDA_Appl->m_Env_Defined )
	{
		FullPath = EDA_Appl->m_KicadEnv + wxT("/help/");
		if ( wxDirExists(FullPath) ) PathFound = TRUE;
	}

	/* find kicad/help/ from default path list:
		/usr/local/kicad/help or c:/kicad/help
	(see s_HelpPathList) */
	int ii = 0;
	while ( ! PathFound )
	{
		FullPath = s_HelpPathList[ii++];
		if ( FullPath == wxT("end_list") ) break;
		if ( wxDirExists(FullPath) ) PathFound = TRUE;
	}


	if ( PathFound )
	{
		LangFullPath = FullPath + LocaleString + UNIX_STRING_DIR_SEP;

		if ( wxDirExists(LangFullPath) ) return LangFullPath;

		LangFullPath = FullPath + LocaleString.Left(2) + UNIX_STRING_DIR_SEP;
		if ( wxDirExists(LangFullPath) ) return LangFullPath;

		LangFullPath = FullPath + wxT("en/");
		if ( wxDirExists(LangFullPath) ) return LangFullPath;
		else
		{
			LangFullPath = FullPath + wxT("fr/");
			if ( wxDirExists(LangFullPath) ) return LangFullPath;
		}
		return FullPath;
	}
	return wxEmptyString;
}

/********************************************************/
wxString FindKicadFile(const wxString & shortname)
/********************************************************/
/* Search the executable file shortname in kicad binary path
	and return full file name if found or shortname
	kicad binary path is
	kicad/winexe or kicad/linux

	kicad binary path is found from:
	BinDir
	or environment variable KICAD
	or (default) c:\kicad ou /usr/local/kicad
	or default binary path
*/
{
wxString FullFileName;

	/* test de la presence du fichier shortname dans le repertoire de
	des binaires de kicad */
	FullFileName = EDA_Appl->m_BinDir + shortname;
	if ( wxFileExists(FullFileName) ) return FullFileName;

	/* test de la presence du fichier shortname dans le repertoire
	defini par la variable d'environnement KICAD */
	if ( EDA_Appl->m_Env_Defined )
	{
		FullFileName = EDA_Appl->m_KicadEnv + shortname;
		if ( wxFileExists(FullFileName) ) return FullFileName;
	}

	/* find binary file from default path list:
		/usr/local/kicad/linux or c:/kicad/winexe
	(see s_KicadDataPathList) */
	int ii = 0;
	while ( 1 )
	{
		if ( s_KicadBinaryPathList[ii] == wxT("end_list") ) break;
		FullFileName = s_KicadBinaryPathList[ii++] + shortname;
		if ( wxFileExists(FullFileName) ) return FullFileName;
	}

	return shortname;
}

/***********************************************************************************/
int ExecuteFile(wxWindow * frame, const wxString & ExecFile, const wxString & param)
/***********************************************************************************/
/* Call the executable file "ExecFile", with params "param"
*/
{
wxString FullFileName;


#ifdef __WXMAC__
	// Mac part
	wxGetEnv("HOME", &FullFileName);
	FullFileName += wxString("/bin/") + newExecFile;
	if (! wxFileExists(FullFileName) )
	{
		FullFileName = FindKicadFile(ExecFile);
	}

#else
	FullFileName = FindKicadFile(ExecFile);
#endif

	if ( wxFileExists(FullFileName) )
	{
		if( !param.IsEmpty() ) FullFileName += wxT(" ") + param;
		wxExecute(FullFileName);
		return 0;
	}

	wxString msg;
	msg.Printf( wxT("Command file <%s> not found"), FullFileName.GetData() );
	DisplayError(frame, msg, 20);
	return -1;
}


/****************************************************/
void SetRealLibraryPath(const wxString & shortlibname)
/****************************************************/
/* met a jour le chemin des librairies g_RealLibDirBuffer (global)
a partir de UserLibDirBuffer (global):
	Si UserLibDirBuffer non vide g_RealLibDirBuffer = g_UserLibDirBuffer.
	Sinon si variable d'environnement KICAD definie (KICAD = chemin pour kicad),
		g_UserLibDirBuffer = <KICAD>/shortlibname;
	Sinon g_UserLibDirBuffer = <Chemin des binaires>../shortlibname/
	Sinon g_UserLibDirBuffer = /usr/share/kicad/shortlibname/

Remarque:
	Les \ sont remplac�s par / (a la mode Unix)
*/
{
bool PathFound = FALSE;

	if ( ! g_UserLibDirBuffer.IsEmpty() )	// Chemin impose par la configuration
	{
		g_RealLibDirBuffer = g_UserLibDirBuffer;
		PathFound = TRUE;
	}

	else
	{
		g_RealLibDirBuffer = ReturnKicadDatasPath();
		if ( EDA_Appl->m_Env_Defined )	// Chemin impose par la variable d'environnement
		{
			PathFound = TRUE;
		}
		g_RealLibDirBuffer += shortlibname;
		if ( wxDirExists(g_RealLibDirBuffer) ) PathFound = TRUE;
	}


	g_RealLibDirBuffer.Replace(WIN_STRING_DIR_SEP, UNIX_STRING_DIR_SEP);
	if ( g_RealLibDirBuffer.Last() != '/' ) g_RealLibDirBuffer +=  UNIX_STRING_DIR_SEP;
}

/***********************************/
wxString ReturnKicadDatasPath(void)
/***********************************/
/* Retourne le chemin des donnees communes de kicad.
	Si variable d'environnement KICAD definie (KICAD = chemin pour kicad),
		retourne <KICAD>/;
	Sinon retourne <Chemin des binaires>/ (si "kicad" est dans le nom du chemin)
	Sinon retourne /usr/share/kicad/

Remarque:
	Les \ sont remplac�s par / (a la mode Unix)
*/
{
bool PathFound = FALSE;
wxString data_path;

	if ( EDA_Appl->m_Env_Defined )	// Chemin impose par la variable d'environnement
	{
		data_path = EDA_Appl->m_KicadEnv;
		PathFound = TRUE;
	}
	else 	// Chemin cherche par le chemin des executables
	{		// le chemin est bindir../
		wxString tmp = EDA_Appl->m_BinDir;
		if ( tmp.Contains( wxT("kicad") ) )
		{
			if ( tmp.Last() == '/' ) tmp.RemoveLast();
			data_path = tmp.BeforeLast('/'); // id cd ../
			data_path += UNIX_STRING_DIR_SEP;
			if ( wxDirExists(data_path) ) PathFound = TRUE;
		}
	}

	/* find kicad from default path list:
		/usr/local/kicad/ or c:/kicad/
	(see s_KicadDataPathList) */
	int ii = 0;
	while ( ! PathFound )
	{
		if ( s_KicadDataPathList[ii] == wxT("end_list") ) break;
		data_path = s_KicadDataPathList[ii++];
		if ( wxDirExists(data_path) ) PathFound = TRUE;
	}

	if ( PathFound )
	{
		data_path.Replace(WIN_STRING_DIR_SEP, UNIX_STRING_DIR_SEP);
		if ( data_path.Last() != '/' ) data_path +=  UNIX_STRING_DIR_SEP;
	}

	else data_path.Empty();

	return data_path;
}


/***************************/
wxString GetEditorName(void)
/***************************/
/* Return the prefered editor name
*/
{
	wxString editorname = g_EditorName;

	if ( editorname.IsEmpty() )	// We get the prefered editor name from environment variable
	{
		wxGetEnv(wxT("EDITOR"), &editorname);
	}
	if ( editorname.IsEmpty() )	// We must get a prefered editor name
	{
	DisplayInfo(NULL, _("No default editor found, you must choose it") );
	wxString mask(wxT("*"));
#ifdef __WINDOWS__
	mask += wxT(".exe");
#endif
	editorname = EDA_FileSelector(_("Prefered Editor:"),
					wxEmptyString,						/* Default path */
					wxEmptyString,						/* default filename */
					wxEmptyString,					/* default filename extension */
					mask,				/* filter for filename list */
					NULL,					/* parent frame */
					wxFD_OPEN,					/* wxFD_SAVE, wxFD_OPEN ..*/
					TRUE					/* true = keep the current path */
					);
	}

	if ( (!editorname.IsEmpty()) && EDA_Appl->m_EDA_CommonConfig)
	{
		g_EditorName = editorname;
		EDA_Appl->m_EDA_CommonConfig->Write( wxT("Editor"), g_EditorName);
	}
	return g_EditorName;
}

void OpenPDF( const wxString & file )
{
	wxString command;
	wxString filename = file;
	wxString type;

	EDA_Appl->ReadPdfBrowserInfos();
	if ( !EDA_Appl->m_PdfBrowserIsDefault )
	{
		AddDelimiterString(filename);
		command = EDA_Appl->m_PdfBrowser + filename;
	}
	else
	{
		bool success = false;
		wxFileType * filetype = NULL;

		wxFileType::MessageParameters params(filename, type);
		filetype = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT(".pdf"));
		if (filetype ) success = filetype->GetOpenCommand( &command, params);
		delete filetype;

		if (!success)
		{
			AddDelimiterString(filename);
			command.Empty();
			wxString tries[] =
			{
				wxT("/usr/bin/evince"),
				wxT("/usr/bin/xpdf"),
				wxT("/usr/bin/konqueror"),
				wxT("/usr/bin/gpdf"),
				wxT(""),
			};
			for ( int i = 0;; i++ )
			{
				if (tries[i].IsEmpty()) break;
				if (wxFileExists(tries[i]))
				{
					command = tries[i] + wxT(" ") + filename;
				}
			}
		}
	}

	if (!command.IsEmpty()) wxExecute(command);
}


void OpenFile( const wxString & file )
{
	wxString command;
	wxString filename = file;

	wxFileName CurrentFileName(filename);
	wxString ext, type;
	ext = CurrentFileName.GetExt();
	wxFileType * filetype = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);

	bool success = false;

	wxFileType::MessageParameters params(filename, type);
	if (filetype) success = filetype->GetOpenCommand( &command, params);
	delete filetype;

	if (success && !command.IsEmpty()) wxExecute(command);
}
