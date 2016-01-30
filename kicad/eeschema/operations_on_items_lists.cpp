/***************************************************
 *  operations_on_item_lists.cpp
 * functions used in block commands, on lists of schematic items:
 * move, mirror, delete and copy
 ****************************************************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "common.h"
#include "class_drawpanel.h"

#include "program.h"
#include "general.h"
#include "class_marker_sch.h"
#include "protos.h"


void MoveItemsInList( PICKED_ITEMS_LIST& aItemsList, const wxPoint aMoveVector );
void MirrorListOfItems( PICKED_ITEMS_LIST& aItemsList, wxPoint& aMirrorPoint );
void DeleteItemsInList( WinEDA_DrawPanel*  panel,
                        PICKED_ITEMS_LIST& aItemsList );
void DuplicateItemsInList( SCH_SCREEN* screen, PICKED_ITEMS_LIST& aItemsList,
                           const wxPoint aMoveVector  );


void MirrorListOfItems( PICKED_ITEMS_LIST& aItemsList, wxPoint& aMirrorPoint )
{
    for( unsigned ii = 0; ii < aItemsList.GetCount(); ii++ )
    {
        SCH_ITEM* item = (SCH_ITEM*) aItemsList.GetPickedItem( ii );
        item->Mirror_Y( aMirrorPoint.x );      // Place it in its new position.
        item->m_Flags = 0;
    }
}


/** Function MoveItemsInList
 *  Move a list of items to a given move vector
 * @param aItemsList = list of picked items
 * @param aMoveVector = the move vector value
 */
void MoveItemsInList( PICKED_ITEMS_LIST& aItemsList, const wxPoint aMoveVector )
{
    for( unsigned ii = 0; ii < aItemsList.GetCount(); ii++ )
    {
        SCH_ITEM* item = (SCH_ITEM*) aItemsList.GetPickedItem( ii );
        item->Move( aMoveVector );
    }
}


/** function DeleteItemsInList
 * delete schematic items in aItemsList
 * deleted items are put in undo list
 */
void DeleteItemsInList( WinEDA_DrawPanel* panel, PICKED_ITEMS_LIST& aItemsList )
{
    SCH_SCREEN*            screen = (SCH_SCREEN*) panel->GetScreen();
    WinEDA_SchematicFrame* frame  = (WinEDA_SchematicFrame*) panel->GetParent();
    PICKED_ITEMS_LIST      itemsList;
    ITEM_PICKER            itemWrapper;

    for( unsigned ii = 0; ii < aItemsList.GetCount(); ii++ )
    {
        SCH_ITEM* item = (SCH_ITEM*) aItemsList.GetPickedItem( ii );
        itemWrapper.m_PickedItem     = item;
        itemWrapper.m_UndoRedoStatus = UR_DELETED;
        if( item->Type() == DRAW_HIERARCHICAL_PIN_SHEET_STRUCT_TYPE )
        {
            /* this item is depending on a sheet, and is not in global list */
            wxMessageBox( wxT( "DeleteItemsInList() err: unexpected \
DRAW_HIERARCHICAL_PIN_SHEET_STRUCT_TYPE" ) );
#if 0
            SCH_SHEET_PIN* pinlabel = (SCH_SHEET_PIN*) item;
            frame->DeleteSheetLabel( false, pinlabel->m_Parent );
            itemWrapper.m_PickedItem     = pinlabel->m_Parent;
            itemWrapper.m_UndoRedoStatus = UR_CHANGED;
            itemsList.PushItem( itemWrapper );
#endif
        }
        else
        {
            screen->RemoveFromDrawList( item );

            /* Unlink the structure */
            item->SetNext( 0 );
            item->SetBack( 0 );
            itemsList.PushItem( itemWrapper );
        }
    }

    frame->SaveCopyInUndoList( itemsList, UR_DELETED );
}


/* Routine to delete an object from global drawing object list.
 *  Object is put in Undo list
 */
void DeleteStruct( WinEDA_DrawPanel* panel, wxDC* DC, SCH_ITEM* DrawStruct )
{
    SCH_SCREEN*            screen = (SCH_SCREEN*) panel->GetScreen();
    WinEDA_SchematicFrame* frame  = (WinEDA_SchematicFrame*) panel->GetParent();

    if( !DrawStruct )
        return;

    if( DrawStruct->Type() == DRAW_HIERARCHICAL_PIN_SHEET_STRUCT_TYPE )
    {
        /* This structure is attached to a node, and is not accessible by
         * the global list directly. */
        frame->SaveCopyInUndoList(
            (SCH_ITEM*) ( (SCH_SHEET_PIN*) DrawStruct )-> GetParent(),
            UR_CHANGED );
        frame->DeleteSheetLabel( DC ? true : false,
                                 (SCH_SHEET_PIN*) DrawStruct );
        return;
    }
    else
    {
        screen->RemoveFromDrawList( DrawStruct );

        panel->PostDirtyRect( DrawStruct->GetBoundingBox() );

        /* Unlink the structure */
        DrawStruct->SetNext( 0 );
        DrawStruct->SetBack( 0 );  // Only one struct -> no link

        frame->SaveCopyInUndoList( DrawStruct, UR_DELETED );
    }
}


/* Routine to copy a new entity of an object for each object in list and
 * reposition it.
 * Return the new created object list in aItemsList
 */
void DuplicateItemsInList( SCH_SCREEN* screen, PICKED_ITEMS_LIST& aItemsList,
                           const wxPoint aMoveVector )
{
    SCH_ITEM* newitem;

    if( aItemsList.GetCount() == 0 )
        return;

    for( unsigned ii = 0; ii < aItemsList.GetCount(); ii++ )
    {
        newitem = DuplicateStruct( (SCH_ITEM*) aItemsList.GetPickedItem( ii ) );
        aItemsList.SetPickedItem( newitem, ii );
        aItemsList.SetPickedItemStatus( UR_NEW, ii );
        {
            switch( newitem->Type() )
            {
            case DRAW_POLYLINE_STRUCT_TYPE:
            case DRAW_JUNCTION_STRUCT_TYPE:
            case DRAW_SEGMENT_STRUCT_TYPE:
            case DRAW_BUSENTRY_STRUCT_TYPE:
            case TYPE_SCH_TEXT:
            case TYPE_SCH_LABEL:
            case TYPE_SCH_GLOBALLABEL:
            case TYPE_SCH_HIERLABEL:
            case DRAW_HIERARCHICAL_PIN_SHEET_STRUCT_TYPE:
            case TYPE_SCH_MARKER:
            case DRAW_NOCONNECT_STRUCT_TYPE:
            default:
                break;

            case DRAW_SHEET_STRUCT_TYPE:
            {
                SCH_SHEET* sheet = (SCH_SHEET*) newitem;
                sheet->m_TimeStamp = GetTimeStamp();
                sheet->SetSon( NULL );
                break;
            }

            case TYPE_SCH_COMPONENT:
                ( (SCH_COMPONENT*) newitem )->m_TimeStamp = GetTimeStamp();
                ( (SCH_COMPONENT*) newitem )->ClearAnnotation( NULL );
                break;
            }

            SetaParent( newitem, screen );
            newitem->SetNext( screen->EEDrawList );
            screen->EEDrawList = newitem;
        }
    }

    MoveItemsInList( aItemsList, aMoveVector );
}


/* Routine to create a new copy of given struct.
 *  The new object is not put in draw list (not linked)
 */
SCH_ITEM* DuplicateStruct( SCH_ITEM* DrawStruct )
{
    SCH_ITEM* NewDrawStruct = NULL;

    if( DrawStruct == NULL )
    {
        wxMessageBox( wxT( "DuplicateStruct error: NULL struct" ) );
        return NULL;
    }

    switch( DrawStruct->Type() )
    {
    case DRAW_POLYLINE_STRUCT_TYPE:
        NewDrawStruct = ( (SCH_POLYLINE*) DrawStruct )->GenCopy();
        break;

    case DRAW_SEGMENT_STRUCT_TYPE:
        NewDrawStruct = ( (SCH_LINE*) DrawStruct )->GenCopy();
        break;

    case DRAW_BUSENTRY_STRUCT_TYPE:
        NewDrawStruct = ( (SCH_BUS_ENTRY*) DrawStruct )->GenCopy();
        break;

    case DRAW_JUNCTION_STRUCT_TYPE:
        NewDrawStruct = ( (SCH_JUNCTION*) DrawStruct )->GenCopy();
        break;

    case TYPE_SCH_MARKER:
        NewDrawStruct = ( (SCH_MARKER*) DrawStruct )->GenCopy();
        break;

    case DRAW_NOCONNECT_STRUCT_TYPE:
        NewDrawStruct = ( (SCH_NO_CONNECT*) DrawStruct )->GenCopy();
        break;

    case TYPE_SCH_TEXT:
        NewDrawStruct = ( (SCH_TEXT*) DrawStruct )->GenCopy();
        break;

    case TYPE_SCH_LABEL:
        NewDrawStruct = ( (SCH_LABEL*) DrawStruct )->GenCopy();
        break;

    case TYPE_SCH_HIERLABEL:
        NewDrawStruct = ( (SCH_HIERLABEL*) DrawStruct )->GenCopy();
        break;

    case TYPE_SCH_GLOBALLABEL:
        NewDrawStruct = ( (SCH_GLOBALLABEL*) DrawStruct )->GenCopy();
        break;

    case TYPE_SCH_COMPONENT:
        NewDrawStruct = ( (SCH_COMPONENT*) DrawStruct )->GenCopy();
        break;

    case DRAW_SHEET_STRUCT_TYPE:
        NewDrawStruct = ( (SCH_SHEET*) DrawStruct )->GenCopy();
        break;

    default:
    {
        wxString msg;
        msg << wxT( "DuplicateStruct error: unexpected StructType " )
            << DrawStruct->Type() << wxT( " " ) << DrawStruct->GetClass();
        wxMessageBox( msg );
    }
    break;
    }

    NewDrawStruct->m_Image = DrawStruct;
    return NewDrawStruct;
}