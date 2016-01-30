/***************************************************************
*   Tracks and Vias size edition:
*   Functions to modify sizes of segment, track, net , all vias and/or all tracks
***************************************************************/

#include "fctsys.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"

#include "pcbnew.h"
#include "wxPcbStruct.h"
#include "class_board_design_settings.h"
#include "protos.h"


/** Function SetTrackSegmentWidth
 *  Modify one track segment width or one via diameter and drill (using DRC control).
 *  Basic routine used by other routines when editing tracks or vias
 * @param aTrackItem = the track segment or via to modify
 * @param aItemsListPicker = the list picker to use for an undo command (can be NULL)
 * @param aUseNetclassValue = true to use NetClass value, false to use g_DesignSettings value
 * @return  true if done, false if no not change (because DRC error)
 */
bool WinEDA_PcbFrame::SetTrackSegmentWidth( TRACK*             aTrackItem,
                                            PICKED_ITEMS_LIST* aItemsListPicker,
                                            bool               aUseNetclassValue )
{
    int           initial_width, new_width, new_drill = -1;
    bool          change_ok = false;
    NETINFO_ITEM* net = NULL;

    if( aUseNetclassValue )
        net = GetBoard()->FindNet( aTrackItem->GetNet() );

    initial_width = aTrackItem->m_Width;
    if( net )
        new_width = net->GetTrackWidth();
    else
        new_width = GetBoard()->GetCurrentTrackWidth();
    if( aTrackItem->Type() == TYPE_VIA )
    {
        if( net )
            new_width = net->GetViaSize();
        else
        {
            new_width = GetBoard()->GetCurrentViaSize();
            new_drill = GetBoard()->GetCurrentViaDrill();
        }
        if( aTrackItem->m_Shape == VIA_MICROVIA )
        {
            if( net )
                new_width = net->GetViaSize();
            else
                new_width = net->GetMicroViaSize();
        }

        // Set drill value. Note: currently microvias have only a default drill value
        if( new_drill > 0 )
            aTrackItem->SetDrillValue(new_drill);
        else
            aTrackItem->SetDrillDefault( );
    }

    aTrackItem->m_Width = new_width;
    if( initial_width < new_width )     /* make a DRC test because the new size is bigger than the old size */
    {
        int diagdrc = OK_DRC;
        if( Drc_On )
            diagdrc = m_drc->Drc( aTrackItem, GetBoard()->m_Track );
        if( diagdrc == OK_DRC )
            change_ok = true;
    }
    else if( initial_width > new_width )
        change_ok = true;

    // if new width == initial_width: do nothing

    if( change_ok )
    {
        OnModify();
        if( aItemsListPicker )
        {
            aTrackItem->m_Width = initial_width;
            ITEM_PICKER picker( aTrackItem, UR_CHANGED );
            picker.m_Link = aTrackItem->Copy();
            aItemsListPicker->PushItem( picker );
            aTrackItem->m_Width = new_width;
        }
    }
    else
        aTrackItem->m_Width = initial_width;

    return change_ok;
}


/** Function Edit_TrackSegm_Width
 * Modify one track segment width or one via diameter (using DRC control).
 * @param  DC = the curred device context (can be NULL)
 * @param aTrackItem = the track segment or via to modify
 */
void WinEDA_PcbFrame::Edit_TrackSegm_Width( wxDC* DC, TRACK* aTrackItem )
{
    PICKED_ITEMS_LIST itemsListPicker;
    bool change = SetTrackSegmentWidth( aTrackItem, &itemsListPicker, false );

    if( change == 0 || aTrackItem->m_Flags )
        return;     // No change

    // The segment has changed: redraw it and save it in undo list
    if( DC )
    {
        TRACK* oldsegm = (TRACK*) itemsListPicker.GetPickedItemLink( 0 );
        wxASSERT( oldsegm );
        DrawPanel->CursorOff( DC );                     // Erase cursor shape
        oldsegm->Draw( DrawPanel, DC, GR_XOR );         // Erase old track shape
        aTrackItem->Draw( DrawPanel, DC, GR_OR );       // Display new track shape
        DrawPanel->CursorOn( DC );                      // Display cursor shape
    }
    SaveCopyInUndoList( itemsListPicker, UR_CHANGED );
}


/** Function Edit_Track_Width
 * Modify a full track width (using DRC control).
 * a full track is the set of track segments between 2 ends: pads or a point that has more than 2 segments ends connected
 * @param  DC = the curred device context (can be NULL)
 * @param aTrackSegment = a segment or via on the track to change
 * @return  none
 */
void WinEDA_PcbFrame::Edit_Track_Width( wxDC* DC, TRACK* aTrackSegment )
{
    TRACK* pt_track;
    int    nb_segm;

    if( aTrackSegment == NULL )
        return;

    pt_track = Marque_Une_Piste( GetBoard(), aTrackSegment, &nb_segm, NULL, true );

    PICKED_ITEMS_LIST itemsListPicker;
    bool change = false;
    for( int ii = 0; ii < nb_segm; ii++, pt_track = pt_track->Next() )
    {
        pt_track->SetState( BUSY, OFF );
        if( SetTrackSegmentWidth( pt_track, &itemsListPicker, false ) )
            change = true;
    }

    if( !change )
        return;

    // Some segment have changed: redraw them and save in undo list
    if( DC )
    {
        DrawPanel->CursorOff( DC );                     // Erase cursor shape
        for( unsigned ii = 0; ii < itemsListPicker.GetCount(); ii++ )
        {
            TRACK* segm = (TRACK*) itemsListPicker.GetPickedItemLink( ii );
            segm->Draw( DrawPanel, DC, GR_XOR );            // Erase old track shape
            segm = (TRACK*) itemsListPicker.GetPickedItem( ii );
            segm->Draw( DrawPanel, DC, GR_OR );             // Display new track shape
        }

        DrawPanel->CursorOn( DC );                   // Display cursor shape
    }

    SaveCopyInUndoList( itemsListPicker, UR_CHANGED );
}


/** function Change_Net_Tracks_And_Vias_Sizes
 * Reset all tracks width and vias diameters and drill
 * to their default Netclass value ou current values
 * @param aNetcode : the netcode of the net to edit
 * @param aUseNetclassValue : bool. True to use netclass values, false to use current values
 */
/***********************************************************/
bool WinEDA_PcbFrame::Change_Net_Tracks_And_Vias_Sizes(
    int aNetcode, bool aUseNetclassValue )
/***********************************************************/
{
    TRACK* pt_segm;

    if( aNetcode <= 0 )
        return false;

    /* Examine segments */
    PICKED_ITEMS_LIST itemsListPicker;
    bool change = false;
    for( pt_segm = GetBoard()->m_Track; pt_segm != NULL; pt_segm = pt_segm->Next() )
    {
        if( aNetcode != pt_segm->GetNet() )         /* not in net */
            continue;
        /* we have found a item member of the net */
        if( SetTrackSegmentWidth( pt_segm, &itemsListPicker, aUseNetclassValue ) )
            change = true;
    }

    if( !change )
        return false;

    // Some segment have changed: save them in undo list
    SaveCopyInUndoList( itemsListPicker, UR_CHANGED );
    return true;
}


/*************************************************************************/
bool WinEDA_PcbFrame::Reset_All_Tracks_And_Vias_To_Netclass_Values(
    bool aTrack, bool aVia )
/*************************************************************************/
{
    TRACK* pt_segm;

    /* read and edit tracks and vias if required */
    PICKED_ITEMS_LIST itemsListPicker;
    bool change = false;
    for( pt_segm = GetBoard()->m_Track; pt_segm != NULL; pt_segm = pt_segm->Next() )
    {
        if( (pt_segm->Type() == TYPE_VIA ) && aVia )
        {
            if( SetTrackSegmentWidth( pt_segm, &itemsListPicker, true ) )
                change = true;
        }

        if( (pt_segm->Type() == TYPE_TRACK ) && aTrack )
        {
            if( SetTrackSegmentWidth( pt_segm, &itemsListPicker, true ) )
                change = true;;
        }
    }

    if( !change )
        return false;

    // Some segment have changed: save them in undo list
    SaveCopyInUndoList( itemsListPicker, UR_CHANGED );

    return true;
}
