	/******************************************************************/
	/* schframe.cpp  - fonctions des classes du type WinEDA_DrawFrame */
	/******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "fctsys.h"
#include "common.h"

#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "bitmaps.h"
#include "protos.h"
#include "id.h"


/************************************************/
void WinEDA_SchematicFrame::ReCreateMenuBar(void)
/************************************************/
/* Cree ou reinitialise le menu du haut d'ecran
*/
{
int ii;
wxMenuBar * menuBar = GetMenuBar();

	if( menuBar == NULL )
		{
		menuBar = new wxMenuBar();
			
		m_FilesMenu = new wxMenu;

		wxMenuItem *item = new wxMenuItem(m_FilesMenu, ID_LOAD_PROJECT,
						_("&Load Schematic Project"),
						_("Load a schematic project (Schematic, libraries...)") );
	    item->SetBitmap(open_xpm);
		m_FilesMenu->Append(item);

		m_FilesMenu->AppendSeparator();
		item = new wxMenuItem(m_FilesMenu,ID_SAVE_PROJECT,
					 _("&Save Schematic Project"),
					 _("Save all") );
	    item->SetBitmap(save_project_xpm);
		m_FilesMenu->Append(item);

		m_FilesMenu->AppendSeparator();
		item = new wxMenuItem(m_FilesMenu, ID_SAVE_ONE_SHEET,
					 _("Save &Current sheet"),
					 _("Save current sheet only") );
	    item->SetBitmap(save_xpm);
		m_FilesMenu->Append(item);

		item = new wxMenuItem(m_FilesMenu, ID_SAVE_ONE_SHEET_AS,
					 _("Save Current sheet &as.."),
					 _("Save current sheet as..") );
	    item->SetBitmap(save_as_xpm);
		m_FilesMenu->Append(item);

		/* Plot Menu */
		item = new wxMenuItem(m_FilesMenu, ID_GEN_PRINT,
					_("P&rint"), _("Print on current printer") );
	    item->SetBitmap(print_button);
		m_FilesMenu->Append(item);

		wxMenu *choice_plot_fmt = new wxMenu;
		item = new wxMenuItem(choice_plot_fmt, ID_GEN_PLOT_PS,
			_("Plot Postscript"), _("Plotting in Postscript format") );
	    item->SetBitmap(plot_PS_xpm);
		choice_plot_fmt->Append(item);

		item = new wxMenuItem(choice_plot_fmt, ID_GEN_PLOT_HPGL,
			_("Plot HPGL"), _("Plotting in HPGL format") );
	    item->SetBitmap(plot_HPG_xpm);
		choice_plot_fmt->Append(item);

		item = new wxMenuItem(choice_plot_fmt, ID_GEN_PLOT_SVG,
			_("Plot SVG"), _("Plotting in SVG format") );
	    item->SetBitmap(plot_xpm);
		choice_plot_fmt->Append(item);

		m_FilesMenu->AppendSeparator();
		ADD_MENUITEM_WITH_HELP_AND_SUBMENU(m_FilesMenu, choice_plot_fmt,
			ID_GEN_PLOT, _("&Plot"),  _("Plot Hplg, Postscript, SVG"), plot_xpm);

		m_FilesMenu->AppendSeparator();
		item = new wxMenuItem(m_FilesMenu, ID_EXIT, _("E&xit"), _("Quit Eeschema") );
	    item->SetBitmap(exit_xpm);
		m_FilesMenu->Append(item);

// Creation des selections des anciens fichiers
		m_FilesMenu->AppendSeparator();
		int max_file = m_Parent->m_LastProjectMaxCount;
		for ( ii = 0; ii < max_file; ii++ )
		{
			if ( GetLastProject(ii).IsEmpty() ) break;
			item = new wxMenuItem(m_FilesMenu, ID_LOAD_FILE_1 + ii,
						GetLastProject(ii));
			m_FilesMenu->Append(item);
		}

		// Menu Configuration:
		wxMenu * configmenu = new wxMenu;
		item = new wxMenuItem(configmenu, ID_CONFIG_REQ,
			_("&Libs and Dir"),
			_("Setting Libraries, Directories and others...") );
	    item->SetBitmap(library_xpm);
		configmenu->Append(item);

		item = new wxMenuItem(configmenu, ID_COLORS_SETUP,
			_("&Colors"),
			_("Setting colors ...") );
	    item->SetBitmap(palette_xpm);
		configmenu->Append(item);

		ADD_MENUITEM(configmenu, ID_OPTIONS_SETUP, _("&Options"), preference_xpm);

		// Font selection and setup
		AddFontSelectionMenu(configmenu);
		
		m_Parent->SetLanguageList(configmenu);

		configmenu->AppendSeparator();
		item = new wxMenuItem(configmenu, ID_CONFIG_SAVE, _("&Save Eeschema Setup"),
			_("Save options in <project>.pro") );
	    item->SetBitmap(save_setup_xpm);
		configmenu->Append(item);
		item = new wxMenuItem(configmenu, ID_CONFIG_READ, _("&Read Setup"),
				_("Read options from a selected config file"));
	    item->SetBitmap(read_setup_xpm);
		configmenu->Append(item);

		// Menu Help:
		wxMenu *helpMenu = new wxMenu;
		item = new wxMenuItem(helpMenu , ID_GENERAL_HELP,
				_("Kicad &Help"), _("On line doc"));
	    item->SetBitmap(help_xpm);
		helpMenu->Append(item);

		item = new wxMenuItem(helpMenu , ID_KICAD_ABOUT,
				_("Eeschema &About"), _("Eeschema Infos"));
	    item->SetBitmap(info_xpm);
		helpMenu->Append(item);


		menuBar->Append(m_FilesMenu, _("&Files") );
		menuBar->Append(configmenu, _("&Preferences") );
		menuBar->Append(helpMenu, _("&Help") );

		// Associate the menu bar with the frame
		SetMenuBar(menuBar);
		}

	else		// simple mise a jour de la liste des fichiers anciens
		{
		wxMenuItem * item;
		int max_file = m_Parent->m_LastProjectMaxCount;
		for ( ii = max_file-1; ii >=0 ; ii-- )
			{
			if( m_FilesMenu->FindItem(ID_LOAD_FILE_1 + ii) )
				{
				item = m_FilesMenu->Remove(ID_LOAD_FILE_1 + ii);
				if ( item ) delete item;
				}
			}
		for ( ii = 0; ii < max_file; ii++ )
			{
			if ( GetLastProject(ii).IsEmpty() ) break;
			item = new wxMenuItem(m_FilesMenu, ID_LOAD_FILE_1 + ii,
						GetLastProject(ii));
			m_FilesMenu->Append(item);
			}
		}
}


