/******************/
/* Trace editing. */
/******************/

#include "fctsys.h"

#include "common.h"
#include "pcbnew.h"
#include "wxPcbStruct.h"

#include "protos.h"

/** function EraseRedundantTrack
 * Called after creating a track
 * Remove (if exists) the old track that have the same starting and the same
 * ending point as the new created track (this is the redunding track)
 * @param aDC = the current device context (can be NULL)
 * @param aNewTrack = the new created track (a pointer to a segment of the
 *                    track list)
 * @param aNewTrackSegmentsCount = number of segments in this new track
 * @param aItemsListPicker = the list picker to use for an undo command (can
 *                           be NULL)
 */
int WinEDA_PcbFrame::EraseRedundantTrack(
    wxDC*              aDC,
    TRACK*             aNewTrack,
    int                aNewTrackSegmentsCount,
    PICKED_ITEMS_LIST* aItemsListPicker )
{
    TRACK*  StartTrack, * EndTrack;
    TRACK*  pt_segm;
    TRACK*  pt_del;
    int     ii, jj, nb_segm, nbconnect;
    wxPoint start;
    wxPoint end;
    int     startmasklayer, endmasklayer;
    TRACK*  BufDeb, * BufEnd;

    int     netcode = aNewTrack->GetNet();


    /* Reconstruct the complete track (the new track has to start on a
     * segment of track).
     */
    ListSetState( aNewTrack, aNewTrackSegmentsCount, BUSY, OFF );

    /* If the new track begins with a via, complete the track segment using
     * the following segment as a reference because a  via is often a hub of
     * segments, and does not characterize track.
     */
    if( aNewTrack->Type() == TYPE_VIA && ( aNewTrackSegmentsCount > 1 ) )
        aNewTrack = aNewTrack->Next();

    aNewTrack = Marque_Une_Piste( GetBoard(), aNewTrack,
                                  &aNewTrackSegmentsCount, NULL, true );
    wxASSERT( aNewTrack );

#if 0 && defined(DEBUG)
    TRACK* EndNewTrack;      /* The last segment of the list chained to
                              * the track */

    EndNewTrack = aNewTrack;
    for( ii = 1;  ii < aNewTrackSegmentsCount; ii++ )
    {
        wxASSERT( EndNewTrack->GetState( -1 ) != 0 );
        D( printf( "track %p is newly part of net %d\n", EndNewTrack,
                   netcode ); )
        EndNewTrack = EndNewTrack->Next();
    }

    wxASSERT( EndNewTrack->GetState( -1 ) != 0 );
    D( printf( "track %p is newly part of net %d\n", EndNewTrack, netcode ); )

    for( TRACK* track = m_Pcb->m_Track;  track;  track = track->Next() )
        track->Show( 0, std::cout );

#endif

    /* Calculate search in terms of segments of track, 1st edge BufDeb useful
     * segment. */
    BufDeb = m_Pcb->m_Track->GetStartNetCode( netcode );

    /* Point BufEnd the last segment. */
    BufEnd = BufDeb->GetEndNetCode( netcode );

    /* Flags for cleaning the net. */
    for( pt_del = BufDeb;  pt_del;  pt_del = pt_del->Next() )
    {
        D( printf( "track %p turning off BUSY | EDIT | CHAIN\n", pt_del ); )
        pt_del->SetState( BUSY | EDIT | CHAIN, OFF );
        if( pt_del == BufEnd )  // Last segment reached
            break;
    }

    if( ReturnEndsTrack( aNewTrack, aNewTrackSegmentsCount,
                         &StartTrack, &EndTrack ) == 0 )
        return 0;

    if( ( StartTrack == NULL ) || ( EndTrack == NULL ) )
        return 0;

    start = StartTrack->m_Start;
    end   = EndTrack->m_End;

    /* The start and end points cannot be the same. */
    if( start == end )
        return 0;

    /* Determine layers interconnected these points. */
    startmasklayer = StartTrack->ReturnMaskLayer();
    endmasklayer   = EndTrack->ReturnMaskLayer();

    /* There may be a via or a pad on the end points. */
    pt_segm = Fast_Locate_Via( m_Pcb->m_Track, NULL, start, startmasklayer );
    if( pt_segm )
        startmasklayer |= pt_segm->ReturnMaskLayer();

    if( StartTrack->start && ( StartTrack->start->Type() == TYPE_PAD ) )
    {
        /* Start on pad. */
        D_PAD* pt_pad = (D_PAD*)(StartTrack->start);
        startmasklayer |= pt_pad->m_Masque_Layer;
    }

    pt_segm = Fast_Locate_Via( m_Pcb->m_Track, NULL, end, endmasklayer );
    if( pt_segm )
        endmasklayer |= pt_segm->ReturnMaskLayer();

    if( EndTrack->end && ( EndTrack->end->Type() == TYPE_PAD ) )
    {
        D_PAD* pt_pad = (D_PAD*)(EndTrack->end);
        endmasklayer |= pt_pad->m_Masque_Layer;
    }

    /* Mark as deleted a new track (which is not involved in the search for
     * other connections)
     */
    ListSetState( aNewTrack, aNewTrackSegmentsCount, DELETED, ON );

    /* A segment must be connected to the starting point, otherwise
     * it is unnecessary to analyze the other point
     */
    pt_segm = Fast_Locate_Piste( BufDeb, BufEnd, start, startmasklayer );

    if( pt_segm == NULL )     /* Not connected to the track starting point. */
    {
        /* Clear the delete flag. */
        ListSetState( aNewTrack, aNewTrackSegmentsCount, DELETED, OFF );
        return 0;
    }

    /* Marking a list of candidate segmented connect to endpoint
     * Note: the vias are not taken into account because they do
     * not define a track, since they are on an intersection.
     */
    for( pt_del = BufDeb, nbconnect = 0; ; )
    {
        pt_segm = Fast_Locate_Piste( pt_del, BufEnd, end, endmasklayer );
        if( pt_segm == NULL )
            break;

        if( pt_segm->Type() != TYPE_VIA )
        {
            if( pt_segm->GetState( CHAIN ) == 0 )
            {
                pt_segm->SetState( CHAIN, ON );
                nbconnect++;
            }
        }
        if( pt_del == BufEnd )
            break;

        pt_del = pt_segm->Next();
    }

    if( nbconnect == 0 )
    {
        /* Clear used flags */
        for( pt_del = BufDeb; pt_del; pt_del = pt_del->Next() )
        {
            pt_del->SetState( BUSY | DELETED | EDIT | CHAIN, OFF );
            if( pt_del == BufEnd )  // Last segment reached
                break;
        }

        return 0;
    }

    /* Mark trace as edited (which does not involve searching for other
     * tracks)
     */
    ListSetState( aNewTrack, aNewTrackSegmentsCount, DELETED, OFF );
    ListSetState( aNewTrack, aNewTrackSegmentsCount, EDIT, ON );

    /* Test all marked segments. */
    while( nbconnect )
    {
        for( pt_del = BufDeb; pt_del; pt_del = pt_del->Next() )
        {
            if( pt_del->GetState( CHAIN ) )
                break;
            if( pt_del == BufEnd )  // Last segment reached
                break;
        }

        nbconnect--;
        pt_del->SetState( CHAIN, OFF );

        pt_del = Marque_Une_Piste( GetBoard(), pt_del, &nb_segm, NULL, true );

        /* Test if the marked track is redundant, ie if one of marked segments
         * is connected to the starting point of the new track.
         */
        ii = 0;
        pt_segm = pt_del;
        for( ; pt_segm && (ii < nb_segm); pt_segm = pt_segm->Next(), ii++ )
        {
            if( pt_segm->GetState( BUSY ) == 0 )
                break;

            if( pt_segm->m_Start == start || pt_segm->m_End == start )
            {
                /* Marked track can be erased. */
                TRACK* NextS;
                Trace_Une_Piste( DrawPanel, aDC, pt_del, nb_segm,
                                 GR_XOR | GR_SURBRILL );

                for( jj = 0; jj < nb_segm; jj++, pt_del = NextS )
                {
                    NextS = pt_del->Next();
                    if( aItemsListPicker )
                    {
                        pt_del->UnLink();
                        pt_del->SetStatus( 0 );
                        pt_del->m_Flags = 0;
                        ITEM_PICKER picker( pt_del, UR_DELETED );
                        aItemsListPicker->PushItem( picker );
                    }
                    else
                        pt_del->DeleteStructure();
                }

                /* Clean up flags. */
                for( pt_del = m_Pcb->m_Track; pt_del != NULL;
                     pt_del = pt_del->Next() )
                {
                    if( pt_del->GetState( EDIT ) )
                    {
                        pt_del->SetState( EDIT, OFF );
                        if( aDC )
                            pt_del->Draw( DrawPanel, aDC, GR_OR );
                    }
                    pt_del->SetState( EDIT | CHAIN, OFF );
                }

                return 1;
            }
        }

        /* Clear BUSY flag here because the track did not get marked.
         */
        ListSetState( pt_del, nb_segm, BUSY, OFF );
    }

    /* Clear used flags */
    for( pt_del = m_Pcb->m_Track; pt_del; pt_del = pt_del->Next() )
    {
        pt_del->SetState( BUSY | DELETED | EDIT | CHAIN, OFF );
        if( pt_del == BufEnd )  // Last segment reached
            break;
    }

    return 0;
}