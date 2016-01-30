	/******************************************************/
	/** eeconfig.cpp : routines et menus de configuration */
/*******************************************************/
	
#include "fctsys.h"
#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
#include "protos.h"
#include "eeconfig.h"
#include "worksheet.h"

#include "id.h"

/* Variables locales */


/*********************************************************************/
void WinEDA_SchematicFrame::Process_Config(wxCommandEvent& event)
/*********************************************************************/
{
int id = event.GetId();
wxPoint pos;

	wxGetMousePosition(&pos.x, &pos.y);

	pos.y += 5;
	switch ( id )
		{
		case ID_COLORS_SETUP :
			DisplayColorSetupFrame(this, pos);
			break;

		case ID_CONFIG_REQ :		// Creation de la fenetre de configuration
			{
			InstallConfigFrame(pos);
			break;
			}

		case ID_OPTIONS_SETUP:
			DisplayOptionFrame(this, pos);
			break;

		case ID_CONFIG_SAVE:
			Save_Config(this);
			break;

		case ID_CONFIG_READ:
			{
			wxString mask( wxT("*") ); mask += g_Prj_Config_Filename_ext;
			wxString FullFileName = ScreenSch->m_FileName;
			ChangeFileNameExt( FullFileName, g_Prj_Config_Filename_ext );
			
			FullFileName = EDA_FileSelector(_("Read config file"),
					wxGetCwd(),				/* Chemin par defaut */
					FullFileName,			/* nom fichier par defaut */
					g_Prj_Config_Filename_ext,	/* extension par defaut */
					mask,					/* Masque d'affichage */
					this,
					wxFD_OPEN,
					TRUE					/* ne change pas de repertoire courant */
					);
			if ( FullFileName.IsEmpty() ) break;
			if ( ! wxFileExists(FullFileName) )
				{
				wxString msg = _("File ") + FullFileName +_("not found");;
				DisplayError(this, msg); break;
				}
			Read_Config(FullFileName, TRUE );
			}
			break;

		default:
			DisplayError(this, wxT("WinEDA_SchematicFrame::Process_Config internal error") );
		}
}


/***********************************************************************/
bool Read_Config( const wxString & CfgFileName, bool ForceRereadConfig )
/***********************************************************************/
/* lit la configuration, si elle n'a pas deja ete lue
	1 - lit <nom fichier root>.pro
	2 - si non trouve lit <chemin des binaires>../template/kicad.pro
	3 - si non trouve: init des variables aux valeurs par defaut

	Retourne TRUE si lu, FALSE si config non lue
*/
{
wxString FullFileName;
bool IsRead = TRUE;
wxArrayString liblist_tmp = g_LibName_List;
	
	if ( CfgFileName.IsEmpty() ) FullFileName = ScreenSch->m_FileName;
	else FullFileName = CfgFileName;
	g_LibName_List.Clear();
	
	if ( ! EDA_Appl->ReadProjectConfig(FullFileName,
		GROUP, ParamCfgList, ForceRereadConfig ? FALSE : TRUE) )	// Config non lue
	{
		g_LibName_List = liblist_tmp;
		IsRead = FALSE;
	}

	/* Traitement des variables particulieres: */
	SetRealLibraryPath( wxT("library") );

	// If the list is void, load the libraries "power.lib" and "device.lib"
	if ( g_LibName_List.GetCount() == 0 )
	{
		g_LibName_List.Add( wxT("power") );
		g_LibName_List.Add( wxT("device") );
	}

	if ( EDA_Appl->SchematicFrame )
	{
		EDA_Appl->SchematicFrame->SetDrawBgColor(g_DrawBgColor);
		EDA_Appl->SchematicFrame->m_Draw_Grid = g_ShowGrid;
	}

	LoadLibraries(EDA_Appl->SchematicFrame);

	return IsRead;
}



/****************************************************************/
void WinEDA_SchematicFrame::Save_Config(wxWindow * displayframe)
/***************************************************************/
{
wxString path;
wxString FullFileName;
wxString mask( wxT("*") );
	
	mask += g_Prj_Config_Filename_ext;
	FullFileName = ScreenSch->m_FileName.AfterLast('/') /*ConfigFileName*/;
	ChangeFileNameExt( FullFileName, g_Prj_Config_Filename_ext );

	path = wxGetCwd();
	FullFileName = EDA_FileSelector(_("Save config file"),
					path,				/* Chemin par defaut */
					FullFileName,		/* nom fichier par defaut */
					g_Prj_Config_Filename_ext,				/* extension par defaut */
					mask,			/* Masque d'affichage */
					displayframe,
					wxFD_SAVE,
					TRUE
					);
	if ( FullFileName.IsEmpty() ) return;

	/* ecriture de la configuration */
	EDA_Appl->WriteProjectConfig(FullFileName, GROUP, ParamCfgList);
}
