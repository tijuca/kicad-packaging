/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2007-2016 Jean-Pierre Charras  jp.charras at wanadoo.fr
 * Copyright (C) 1992-2016 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <fctsys.h>
#include <common.h>
#include <confirm.h>
#include <kicad_string.h>
#include <gerbview.h>
#include <gerbview_frame.h>
#include <gerber_file_image.h>
#include <gerber_file_image_list.h>
#include <view/view.h>

#include <html_messagebox.h>
#include <macros.h>

/* Read a gerber file, RS274D, RS274X or RS274X2 format.
 */
bool GERBVIEW_FRAME::Read_GERBER_File( const wxString& GERBER_FullFileName )
{
    wxString msg;

    int layer = GetActiveLayer();
    GERBER_FILE_IMAGE_LIST* images = GetImagesList();
    GERBER_FILE_IMAGE* gerber = GetGbrImage( layer );

    if( gerber != NULL )
    {
        Erase_Current_DrawLayer( false );
    }

    gerber = new GERBER_FILE_IMAGE( layer );
    images->AddGbrImage( gerber, layer );

    /* Read the gerber file */
    bool success = gerber->LoadGerberFile( GERBER_FullFileName );

    if( !success )
    {
        msg.Printf( _( "File \"%s\" not found" ), GetChars( GERBER_FullFileName ) );
        DisplayError( this, msg, 10 );
        return false;
    }

    // Display errors list
    if( gerber->GetMessages().size() > 0 )
    {
        HTML_MESSAGE_BOX dlg( this, _("Errors") );
        dlg.ListSet(gerber->GetMessages());
        dlg.ShowModal();
    }

    /* if the gerber file is only a RS274D file
     * (i.e. without any aperture information, but with items), warn the user:
     */
    if( !gerber->m_Has_DCode && gerber->GetItemsList() )
    {
        msg = _("Warning: this file has no D-Code definition\n"
                "It is perhaps an old RS274D file\n"
                "Therefore the size of items is undefined");
        wxMessageBox( msg );
    }

    auto canvas = GetGalCanvas();

    if( canvas )
    {
        auto view = canvas->GetView();

        if( gerber->m_ImageNegative )
        {
            // TODO: find a way to handle negative images
            // (maybe convert geometry into positives?)
        }

        for( auto item = gerber->GetItemsList(); item; item = item->Next() )
        {
            view->Add( (KIGFX::VIEW_ITEM*) item );
        }
    }

    return true;
}



// size of a single line of text from a gerber file.
// warning: some files can have *very long* lines, so the buffer must be large.
#define GERBER_BUFZ 1000000
// A large buffer to store one line
static char lineBuffer[GERBER_BUFZ+1];

bool GERBER_FILE_IMAGE::LoadGerberFile( const wxString& aFullFileName )
{
    int      G_command = 0;        // command number for G commands like G04
    int      D_commande = 0;       // command number for D commands like D02
    char*    text;

    ClearMessageList( );
    ResetDefaultValues();

    // Read the gerber file */
    m_Current_File = wxFopen( aFullFileName, wxT( "rt" ) );

    if( m_Current_File == 0 )
        return false;

    m_FileName = aFullFileName;

    LOCALE_IO toggleIo;

    wxString msg;

    while( true )
    {
        if( fgets( lineBuffer, GERBER_BUFZ, m_Current_File ) == NULL )
            break;

        m_LineNum++;
        text = StrPurge( lineBuffer );

        while( text && *text )
        {
            switch( *text )
            {
            case ' ':
            case '\r':
            case '\n':
                text++;
                break;

            case '*':       // End command
                m_CommandState = END_BLOCK;
                text++;
                break;

            case 'M':       // End file
                m_CommandState = CMD_IDLE;
                while( *text )
                    text++;
                break;

            case 'G':    /* Line type Gxx : command */
                G_command = GCodeNumber( text );
                Execute_G_Command( text, G_command );
                break;

            case 'D':       /* Line type Dxx : Tool selection (xx > 0) or
                             * command if xx = 0..9 */
                D_commande = DCodeNumber( text );
                Execute_DCODE_Command( text, D_commande );
                break;

            case 'X':
            case 'Y':                   /* Move or draw command */
                m_CurrentPos = ReadXYCoord( text );
                if( *text == '*' )      // command like X12550Y19250*
                {
                    Execute_DCODE_Command( text, m_Last_Pen_Command );
                }
                break;

            case 'I':
            case 'J':       /* Auxiliary Move command */
                m_IJPos = ReadIJCoord( text );

                if( *text == '*' )      // command like X35142Y15945J504*
                {
                    Execute_DCODE_Command( text, m_Last_Pen_Command );
                }
                break;

            case '%':
                if( m_CommandState != ENTER_RS274X_CMD )
                {
                    m_CommandState = ENTER_RS274X_CMD;
                    ReadRS274XCommand( lineBuffer, GERBER_BUFZ, text );
                }
                else        //Error
                {
                    AddMessageToList( "Expected RS274X Command"  );
                    m_CommandState = CMD_IDLE;
                    text++;
                }
                break;

            default:
                msg.Printf( "Unexpected char 0x%2.2X &lt;%c&lt;", *text, *text );
                AddMessageToList( msg );
                text++;
                break;
            }
        }
    }

    fclose( m_Current_File );

    m_InUse = true;

    return true;
}
