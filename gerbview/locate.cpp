/************************************************/
/* Locate items at the current cursor position. */
/************************************************/

#include "fctsys.h"
#include "common.h"
#include "gerbview.h"
#include "class_gerber_draw_item.h"

/* localize a gerber item and return a pointer to it.
 * Display info about this item
 */
GERBER_DRAW_ITEM* GERBVIEW_FRAME::Locate( const wxPoint& aPosition, int aTypeloc )
{
    MsgPanel->EraseMsgBox();
    wxPoint ref = aPosition;
    bool found = false;

    if( aTypeloc == CURSEUR_ON_GRILLE )
        ref = GetScreen()->GetNearestGridPosition( ref );

    int layer = getActiveLayer();

    // Search first on active layer
    BOARD_ITEM* item = GetBoard()->m_Drawings;
    GERBER_DRAW_ITEM* gerb_item = NULL;

    for( ; item; item = item->Next() )
    {
        gerb_item = (GERBER_DRAW_ITEM*) item;

        if( gerb_item->GetLayer()!= layer )
            continue;

        if( gerb_item->HitTest( ref ) )
        {
            found = true;
            break;
        }
    }

    if( !found ) // Search on all layers
    {
        item = GetBoard()->m_Drawings;

        for( ; item; item = item->Next() )
        {
            gerb_item = (GERBER_DRAW_ITEM*) item;

            if( gerb_item->HitTest( ref ) )
            {
                found = true;
                break;
            }
        }
    }

    if( found )
    {
        gerb_item->DisplayInfo( this );
        return gerb_item;
    }

    return NULL;
}
