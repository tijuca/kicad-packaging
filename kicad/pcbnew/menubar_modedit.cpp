/************************************************/
/* menubarmodedit.cpp - Module editor menu bar. */
/************************************************/

#include "fctsys.h"
#include "common.h"

#include "pcbnew.h"
#include "wxPcbStruct.h"

#include "bitmaps.h"
#include "protos.h"
#include "pcbnew_id.h"


void WinEDA_ModuleEditFrame::ReCreateMenuBar()
{
    wxMenuBar*  menuBar = GetMenuBar();
    wxMenuItem* item;

    if( !menuBar )
    {
        menuBar = new wxMenuBar();

        wxMenu* sizes_menu = new wxMenu;

        item = new wxMenuItem( sizes_menu, ID_PCB_DRAWINGS_WIDTHS_SETUP,
                               _( "Sizes and Widths" ),
                               _( "Adjust width for texts and drawings" ) );
        item->SetBitmap( options_text_xpm );
        sizes_menu->Append( item );

        item = new wxMenuItem( sizes_menu, ID_PCB_PAD_SETUP, _( "Pad Settings" ),
                               _( "Adjust size, shape, layers... for pads" ) );
        item->SetBitmap( pad_xpm );
        sizes_menu->Append( item );

        item =
            new wxMenuItem( sizes_menu, ID_PCB_USER_GRID_SETUP,
                            _( "User Grid Size" ),
                            _( "Adjust user grid" ) );
        item->SetBitmap( grid_xpm );
        sizes_menu->Append( item );


        ////////////////
        // Menu Help: //
        ////////////////
        wxMenu* helpMenu = new wxMenu;

        // Contents
        item = new wxMenuItem( helpMenu,
                               ID_GENERAL_HELP,
                               _( "&Contents" ),
                               _( "Open the pcbnew manual" ) );
        item->SetBitmap( help_xpm );
        helpMenu->Append( item );


        // About pcbnew
        item = new wxMenuItem( helpMenu,
                               ID_KICAD_ABOUT,
                               _( "&About pcbnew" ),
                               _( "About pcbnew PCB designer" ) );
        item->SetBitmap( info_xpm );
        helpMenu->Append( item );

        //////////////////////
        // Menu Display 3D: //
        //////////////////////
        wxMenu* Display3DMenu = new wxMenu;
        item = new wxMenuItem( Display3DMenu,
                               ID_MENU_PCB_SHOW_3D_FRAME,
                               _( "3D Display" ),
                               _( "Show board in 3D viewer" ) );
        item->SetBitmap( show_3d_xpm );
        Display3DMenu->Append( item );

        menuBar->Append( sizes_menu, _( "&Dimensions" ) );
        menuBar->Append( Display3DMenu, _( "&3D Display" ) );
        menuBar->Append( helpMenu, _( "&Help" ) );

        // Associate the menu bar with the frame
        SetMenuBar( menuBar );
    }
}
