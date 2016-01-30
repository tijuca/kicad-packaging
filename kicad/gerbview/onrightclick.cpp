/********************/
/* onrightclick.cpp */
/********************/

#include "fctsys.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"

#include "gerbview.h"
#include "pcbplot.h"
#include "protos.h"


/* Prepare the right-click pullup menu.
 * The menu already has a list of zoom commands.
 */
bool WinEDA_GerberFrame::OnRightClick( const wxPoint& MousePos,
                                       wxMenu*        PopMenu )
{
    BOARD_ITEM* DrawStruct = GetScreen()->GetCurItem();
    wxString    msg;
    bool        BlockActive =
        (GetScreen()->m_BlockLocate.m_Command !=  BLOCK_IDLE);

    // Do not initiate a start block validation on menu.
    DrawPanel->m_CanStartBlock = -1;

    // Simple location of elements where possible.
    if( ( DrawStruct == NULL ) || ( DrawStruct->m_Flags == 0 ) )
    {
        DrawStruct = GerberGeneralLocateAndDisplay();
    }

    // If command in progress, end command.
    if(  m_ID_current_state )
    {
        if( DrawStruct && DrawStruct->m_Flags )
            PopMenu->Append( ID_POPUP_CANCEL_CURRENT_COMMAND, _( "Cancel" ) );
        else
            PopMenu->Append( ID_POPUP_CLOSE_CURRENT_TOOL, _( "End Tool" ) );
        PopMenu->AppendSeparator();
    }
    else
    {
        if( (DrawStruct && DrawStruct->m_Flags) || BlockActive )
        {
            if( BlockActive )
            {
                PopMenu->Append( ID_POPUP_CANCEL_CURRENT_COMMAND,
                                 _( "Cancel Block" ) );
                PopMenu->Append( ID_POPUP_ZOOM_BLOCK,
                                 _( "Zoom Block (drag middle mouse)" ) );
                PopMenu->AppendSeparator();
                PopMenu->Append( ID_POPUP_PLACE_BLOCK, _( "Place Block" ) );
                PopMenu->Append( ID_POPUP_COPY_BLOCK,
                                 _( "Copy Block (shift mouse)" ) );
                PopMenu->Append( ID_POPUP_DELETE_BLOCK,
                                 _( "Delete Block (ctrl + drag mouse)" ) );
                PopMenu->Append( ID_POPUP_MIRROR_X_BLOCK, _( "Mirror Block" ) );
            }
            else
                PopMenu->Append( ID_POPUP_CANCEL_CURRENT_COMMAND,
                                 _( "Cancel" ) );
            PopMenu->AppendSeparator();
        }
    }

    if( BlockActive )
        return true;

    PopMenu->Append( ID_GERBVIEW_POPUP_DELETE_DCODE_ITEMS,
                     _( "Delete Dcode items" ) );

    if( DrawStruct == NULL )
        return true;

    GetScreen()->SetCurItem( DrawStruct );

    switch( DrawStruct->Type() )
    {
    case TYPE_TRACK:
        break;


    default:
        msg.Printf( wxT( "WinEDA_GerberFrame::OnRightClick Error: illegal or unknown DrawType %d" ),
                    DrawStruct->Type() );
        DisplayError( this, msg );
        break;
    }

    PopMenu->AppendSeparator();
    return true;
}
