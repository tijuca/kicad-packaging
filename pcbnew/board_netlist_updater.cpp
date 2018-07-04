/**
 * @file board_netlist_updater.h
 * @brief BOARD_NETLIST_UPDATER class definition
 */

/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2015 CERN
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2011 Wayne Stambaugh <stambaughw@verizon.net>
 *
 * Copyright (C) 1992-2015 KiCad Developers, see AUTHORS.txt for contributors.
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


#include <common.h>                         // for PAGE_INFO

#include <class_board.h>
#include <netinfo.h>
#include <class_module.h>
#include <class_pad.h>
#include <class_zone.h>

#include <pcb_netlist.h>
#include <connectivity_data.h>
#include <reporter.h>

#include <board_netlist_updater.h>

#include <pcb_edit_frame.h>


BOARD_NETLIST_UPDATER::BOARD_NETLIST_UPDATER( PCB_EDIT_FRAME* aFrame, BOARD* aBoard ) :
    m_commit( aFrame ),
    m_board( aBoard )
{
    m_reporter = &NULL_REPORTER::GetInstance();

    m_deleteSinglePadNets = true;
    m_deleteUnusedComponents = false;
    m_isDryRun = false;
    m_replaceFootprints = true;
    m_lookupByTimestamp = false;

    m_warningCount = 0;
    m_errorCount = 0;
}


BOARD_NETLIST_UPDATER::~BOARD_NETLIST_UPDATER()
{
}


// These functions allow inspection of pad nets during dry runs by keeping a cache of
// current pad netnames indexed by pad.

void BOARD_NETLIST_UPDATER::cacheNetname( D_PAD* aPad, const wxString& aNetname )
{
    m_padNets[ aPad ] = aNetname;
}

wxString BOARD_NETLIST_UPDATER::getNetname( D_PAD* aPad )
{
    if( m_isDryRun && m_padNets.count( aPad ) )
        return m_padNets[ aPad ];
    else
        return aPad->GetNetname();
}


wxPoint BOARD_NETLIST_UPDATER::estimateComponentInsertionPosition()
{
    wxPoint bestPosition;

    if( !m_board->IsEmpty() )
    {
        // Position new components below any existing board features.
        EDA_RECT bbox = m_board->GetBoardEdgesBoundingBox();

        if( bbox.GetWidth() || bbox.GetHeight() )
        {
            bestPosition.x = bbox.Centre().x;
            bestPosition.y = bbox.GetBottom() + Millimeter2iu( 10 );
        }
    }
    else
    {
        // Position new components in the center of the page when the board is empty.
        wxSize pageSize = m_board->GetPageSettings().GetSizeIU();

        bestPosition.x = pageSize.GetWidth() / 2;
        bestPosition.y = pageSize.GetHeight() / 2;
    }

    return bestPosition;
}


MODULE* BOARD_NETLIST_UPDATER::addNewComponent( COMPONENT* aComponent )
{
    wxString msg;

    if( aComponent->GetModule() != NULL )
    {
        msg.Printf( _( "Adding new symbol \"%s:%s\" footprint \"%s\".\n" ),
                    GetChars( aComponent->GetReference() ),
                    GetChars( aComponent->GetTimeStamp() ),
                    GetChars( aComponent->GetFPID().Format() ) );
        m_reporter->Report( msg, REPORTER::RPT_INFO );

        msg.Printf( _( "Add symbol %s, footprint: %s.\n" ),
                    GetChars( aComponent->GetReference() ),
                    GetChars( aComponent->GetFPID().Format() ) );
        m_reporter->Report( msg, REPORTER::RPT_ACTION );


        if( !m_isDryRun )
        {
            // Owned by NETLIST, can only copy it.
            MODULE* footprint = new MODULE( *aComponent->GetModule() );
            footprint->SetParent( m_board );
            footprint->SetPosition( estimateComponentInsertionPosition( ) );
            footprint->SetTimeStamp( GetNewTimeStamp() );

            m_addedComponents.push_back( footprint );
            m_commit.Add( footprint );

            return footprint;
        }
    }
    else
    {
        msg.Printf( _( "Cannot add symbol %s due to missing footprint %s.\n" ),
                    GetChars( aComponent->GetReference() ),
                    GetChars( aComponent->GetFPID().Format() ) );

        m_reporter->Report( msg, REPORTER::RPT_ERROR );

        msg.Printf( _( "Cannot add new symbol \"%s:%s\" due to missing "
                       "footprint \"%s\".\n" ),
                    GetChars( aComponent->GetReference() ),
                    GetChars( aComponent->GetTimeStamp() ),
                    GetChars( aComponent->GetFPID().Format() ) );

        m_reporter->Report( msg, REPORTER::RPT_INFO );
        ++m_errorCount;
    }

    return NULL;
}


MODULE* BOARD_NETLIST_UPDATER::replaceComponent( NETLIST& aNetlist, MODULE* aPcbComponent, COMPONENT* aNewComponent )
{
    wxString msg;

    if( !m_replaceFootprints )
        return NULL;

    // Test if the footprint has not changed
    if( aNewComponent->GetFPID().empty() || aPcbComponent->GetFPID() == aNewComponent->GetFPID() )
        return NULL;

    if( aNewComponent->GetModule() != NULL )
    {
        msg.Printf( _( "Change symbol %s footprint from %s to %s.\n"),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aPcbComponent->GetFPID().Format() ),
                    GetChars( aNewComponent->GetFPID().Format() ) );

        m_reporter->Report( msg, REPORTER::RPT_ACTION );

        msg.Printf( _( "Replacing symbol \"%s:%s\" footprint \"%s\" with "
                       "\"%s\".\n" ),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aPcbComponent->GetPath() ),
                    GetChars( aPcbComponent->GetFPID().Format() ),
                    GetChars( aNewComponent->GetFPID().Format() ) );

        m_reporter->Report( msg, REPORTER::RPT_INFO );

        if( !m_isDryRun )
        {
            wxASSERT( aPcbComponent != NULL );

            MODULE* newFootprint = new MODULE( *aNewComponent->GetModule() );
            newFootprint->SetParent( m_board );

            if( aNetlist.IsFindByTimeStamp() )
                newFootprint->SetReference( aPcbComponent->GetReference() );
            else
                newFootprint->SetPath( aPcbComponent->GetPath() );

            aPcbComponent->CopyNetlistSettings( newFootprint, false );
            m_commit.Remove( aPcbComponent );
            m_commit.Add( newFootprint );

            return newFootprint;
        }
    }
    else
    {
        msg.Printf( _( "Cannot change symbol %s footprint due to missing "
                       "footprint %s.\n" ),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aNewComponent->GetFPID().Format() ) );

        m_reporter->Report( msg, REPORTER::RPT_ERROR );

        msg.Printf( _( "Cannot replace symbol \"%s:%s\" due to missing "
                       "footprint \"%s\".\n" ),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aPcbComponent->GetPath() ),
                    GetChars( aNewComponent->GetFPID().Format() ) );

        m_reporter->Report( msg, REPORTER::RPT_INFO );

        ++m_errorCount;
    }

    return NULL;
}


bool BOARD_NETLIST_UPDATER::updateComponentParameters( MODULE* aPcbComponent, COMPONENT* aNewComponent )
{
    wxString msg;

    if( !aPcbComponent )
        return false;

    // Create a copy only if the module has not been added during this update
    MODULE* copy = m_commit.GetStatus( aPcbComponent ) ? nullptr : (MODULE*) aPcbComponent->Clone();
    bool changed = false;

    // Test for reference designator field change.
    if( aPcbComponent->GetReference() != aNewComponent->GetReference() )
    {
        msg.Printf( _( "Change symbol %s reference to %s.\n" ),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aNewComponent->GetReference() ) );

        m_reporter->Report( msg, REPORTER::RPT_ACTION );

        msg.Printf( _( "Changing symbol \"%s:%s\" reference to \"%s\".\n" ),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aPcbComponent->GetPath() ),
                    GetChars( aNewComponent->GetReference() ) );

        m_reporter->Report( msg, REPORTER::RPT_INFO );

        if ( !m_isDryRun )
        {
            changed = true;
            aPcbComponent->SetReference( aNewComponent->GetReference() );
        }
    }

    // Test for value field change.
    if( aPcbComponent->GetValue() != aNewComponent->GetValue() )
    {
        msg.Printf( _( "Change symbol %s value from %s to %s.\n" ),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aPcbComponent->GetValue() ),
                    GetChars( aNewComponent->GetValue() ) );

        m_reporter->Report( msg, REPORTER::RPT_ACTION );

        msg.Printf( _( "Changing symbol \"%s:%s\" value from \"%s\" to \"%s\".\n" ),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aPcbComponent->GetPath() ),
                    GetChars( aPcbComponent->GetValue() ),
                    GetChars( aNewComponent->GetValue() ) );

        m_reporter->Report( msg, REPORTER::RPT_ACTION );

        if( !m_isDryRun )
        {
            changed = true;
            aPcbComponent->SetValue( aNewComponent->GetValue() );
        }
    }

    // Test for time stamp change.
    if( aPcbComponent->GetPath() != aNewComponent->GetTimeStamp() )
    {
        msg.Printf( _( "Changing symbol path \"%s:%s\" to \"%s\".\n" ),
                    GetChars( aPcbComponent->GetReference() ),
                    GetChars( aPcbComponent->GetPath() ),
                    GetChars( aNewComponent->GetTimeStamp() ) );

        m_reporter->Report( msg, REPORTER::RPT_INFO );

        if( !m_isDryRun )
        {
            changed = true;
            aPcbComponent->SetPath( aNewComponent->GetTimeStamp() );
        }
    }

    if( changed && copy )
        m_commit.Modified( aPcbComponent, copy );
    else
        delete copy;

    return true;
}


bool BOARD_NETLIST_UPDATER::updateComponentPadConnections( MODULE* aPcbComponent, COMPONENT* aNewComponent )
{
    wxString msg;

    // Create a copy only if the module has not been added during this update
    MODULE* copy = m_commit.GetStatus( aPcbComponent ) ? nullptr : (MODULE*) aPcbComponent->Clone();
    bool changed = false;

    // At this point, the component footprint is updated.  Now update the nets.
    for( D_PAD* pad = aPcbComponent->PadsList(); pad; pad = pad->Next() )
    {
        COMPONENT_NET net = aNewComponent->GetNet( pad->GetName() );

        if( !net.IsValid() )                // New footprint pad has no net.
        {
            if( !pad->GetNetname().IsEmpty() )
            {
                msg.Printf( _( "Disconnect symbol %s pin %s.\n" ),
                            GetChars( aPcbComponent->GetReference() ),
                            GetChars( pad->GetName() ) );
                m_reporter->Report( msg, REPORTER::RPT_ACTION );

                msg.Printf( _( "Clearing symbol \"%s:%s\" pin \"%s\" net name.\n" ),
                            GetChars( aPcbComponent->GetReference() ),
                            GetChars( aPcbComponent->GetPath() ),
                            GetChars( pad->GetName() ) );
                m_reporter->Report( msg, REPORTER::RPT_INFO );
            }

            if( !m_isDryRun )
            {
                changed = true;
                pad->SetNetCode( NETINFO_LIST::UNCONNECTED );
            }
            else
                cacheNetname( pad, wxEmptyString );
        }
        else                                 // New footprint pad has a net.
        {
            if( net.GetNetName() != pad->GetNetname() )
            {
                const wxString& netName = net.GetNetName();
                NETINFO_ITEM* netinfo = m_board->FindNet( netName );

                if( netinfo == nullptr )
                {
                    // It might be a new net that has not been added to the board yet
                    if( m_addedNets.count( netName ) )
                        netinfo = m_addedNets[ netName ];
                }

                if( netinfo == nullptr )
                {
                    // It is a new net, we have to add it
                    if( !m_isDryRun )
                    {
                        changed = true;
                        netinfo = new NETINFO_ITEM( m_board, netName );
                        m_commit.Add( netinfo );
                        m_addedNets[netName] = netinfo;
                    }

                    msg.Printf( _( "Add net %s.\n" ), GetChars( netName ) );
                    m_reporter->Report( msg, REPORTER::RPT_ACTION );
                }

                if( !pad->GetNetname().IsEmpty() )
                {
                    msg.Printf( _( "Reconnect symbol %s pin %s from net %s to net %s.\n"),
                            GetChars( aPcbComponent->GetReference() ),
                            GetChars( pad->GetName() ),
                            GetChars( pad->GetNetname() ),
                            GetChars( netName ) );

                } else {
                    msg.Printf( _( "Connect symbol %s pin %s to net %s.\n"),
                            GetChars( aPcbComponent->GetReference() ),
                            GetChars( pad->GetName() ),
                            GetChars( netName ) );
                }

                m_reporter->Report( msg, REPORTER::RPT_ACTION );

                msg.Printf( _( "Changing symbol \"%s:%s\" pin \"%s\" net name from "
                               "\"%s\" to \"%s\".\n" ),
                            GetChars( aPcbComponent->GetReference() ),
                            GetChars( aPcbComponent->GetPath() ),
                            GetChars( pad->GetName() ),
                            GetChars( pad->GetNetname() ),
                            GetChars( netName ) );
                m_reporter->Report( msg, REPORTER::RPT_INFO );

                if( !m_isDryRun )
                {
                    changed = true;
                    pad->SetNet( netinfo );
                }
                else
                    cacheNetname( pad, netName );
            }
        }
    }

    if( changed && copy )
        m_commit.Modified( aPcbComponent, copy );
    else
        delete copy;

    return true;
}


void BOARD_NETLIST_UPDATER::cacheCopperZoneConnections()
{
    for( int ii = 0; ii < m_board->GetAreaCount(); ii++ )
    {
        ZONE_CONTAINER* zone = m_board->GetArea( ii );

        if( !zone->IsOnCopperLayer() || zone->GetIsKeepout() )
            continue;

        m_zoneConnectionsCache[ zone ] = m_board->GetConnectivity()->GetConnectedPads( zone );
    }
}


bool BOARD_NETLIST_UPDATER::updateCopperZoneNets( NETLIST& aNetlist )
{
    wxString msg;
    std::set<wxString> netlistNetnames;

    for( int ii = 0; ii < (int) aNetlist.GetCount(); ii++ )
    {
        const COMPONENT* component = aNetlist.GetComponent( ii );
        for( unsigned jj = 0; jj < component->GetNetCount(); jj++ )
        {
            const COMPONENT_NET& net = component->GetNet( jj );
            netlistNetnames.insert( net.GetNetName() );
        }
    }

    // Test copper zones to detect "dead" nets (nets without any pad):
    for( int i = 0; i < m_board->GetAreaCount(); i++ )
    {
        ZONE_CONTAINER* zone = m_board->GetArea( i );

        if( !zone->IsOnCopperLayer() || zone->GetIsKeepout() )
            continue;

        if( netlistNetnames.count( zone->GetNetname() ) == 0 )
        {
            // Look for a pad in the zone's connected-pad-cache which has been updated to
            // a new net and use that. While this won't always be the right net, the dead
            // net is guaranteed to be wrong.
            wxString updatedNetname = wxEmptyString;

            for( D_PAD* pad : m_zoneConnectionsCache[ zone ] )
            {
                if( getNetname( pad ) != zone->GetNetname() )
                {
                    updatedNetname = getNetname( pad );
                    break;
                }
            }

            if( !updatedNetname.IsEmpty() )
            {
                msg.Printf( _( "Reconnect copper zone from net \"%s\" to net \"%s\"." ),
                            zone->GetNetname(), updatedNetname );
                m_reporter->Report( msg, REPORTER::RPT_ACTION );

                msg.Printf( _( "Changing copper zone net name from \"%s\" to \"%s\"." ),
                            zone->GetNetname(), updatedNetname );
                m_reporter->Report( msg, REPORTER::RPT_INFO );

                if( !m_isDryRun )
                {
                    NETINFO_ITEM* netinfo = m_board->FindNet( updatedNetname );

                    if( !netinfo )
                        netinfo = m_addedNets[ updatedNetname ];

                    if( netinfo )
                    {
                        m_commit.Modify( zone );
                        zone->SetNet( netinfo );
                    }
                }
            }
            else
            {
                msg.Printf( _( "Copper zone (net \"%s\") has no pads connected." ),
                            zone->GetNetname() );
                m_reporter->Report( msg, REPORTER::RPT_WARNING );
                ++m_warningCount;
            }
        }
    }

    return true;
}


bool BOARD_NETLIST_UPDATER::deleteUnusedComponents( NETLIST& aNetlist )
{
    wxString msg;
    MODULE* nextModule;
    const COMPONENT* component;

    for( MODULE* module = m_board->m_Modules; module != NULL; module = nextModule )
    {
        nextModule = module->Next();

        if( m_lookupByTimestamp )
            component = aNetlist.GetComponentByTimeStamp( module->GetPath() );
        else
            component = aNetlist.GetComponentByReference( module->GetReference() );

        if( component == NULL )
        {
            if( module->IsLocked() )
            {
                msg.Printf( _( "Footprint %s is locked, skipping removal.\n" ),
                            GetChars( module->GetReference() ) );
                m_reporter->Report( msg, REPORTER::RPT_WARNING );
                continue;
            }

            msg.Printf( _( "Remove footprint %s." ),
                        GetChars( module->GetReference() ) );
            m_reporter->Report( msg, REPORTER::RPT_ACTION );

            msg.Printf( _( "Removing unused footprint \"%s:%s\".\n" ),
                        GetChars( module->GetReference() ),
                        GetChars( module->GetPath() ) );
            m_reporter->Report( msg, REPORTER::RPT_INFO );

            if( !m_isDryRun )
                m_commit.Remove( module );
        }
    }

    return true;
}


bool BOARD_NETLIST_UPDATER::deleteSinglePadNets()
{
    int         count = 0;
    wxString    netname;
    wxString    msg;
    D_PAD*      pad = NULL;
    D_PAD*      previouspad = NULL;

    // We need the pad list for next tests.

    m_board->BuildListOfNets();

    std::vector<D_PAD*> padlist = m_board->GetPads();

    if( m_isDryRun )
    {
        // During a dry run changes are only stored in the m_padNets cache, so we must sort
        // the list ourselves.
        std::sort( padlist.begin(), padlist.end(),
            [ this ]( D_PAD* a, D_PAD* b ) -> bool { return getNetname( a ) < getNetname( b ); } );
    }

    for( unsigned kk = 0; kk < padlist.size(); kk++ )
    {
        pad = padlist[kk];

        if( getNetname( pad ).IsEmpty() )
            continue;

        if( netname != getNetname( pad ) )  // End of net
        {
            if( previouspad && count == 1 )
            {
                // First, see if we have a copper zone attached to this pad.
                // If so, this is not really a single pad net

                for( int ii = 0; ii < m_board->GetAreaCount(); ii++ )
                {
                    ZONE_CONTAINER* zone = m_board->GetArea( ii );

                    if( !zone->IsOnCopperLayer() )
                        continue;

                    if( zone->GetIsKeepout() )
                        continue;

                    if( zone->GetNetname() == getNetname( previouspad ) )
                    {
                        count++;
                        break;
                    }
                }

                if( count == 1 )    // Really one pad, and nothing else
                {
                    msg.Printf( _( "Remove single pad net %s." ),
                                GetChars( getNetname( previouspad ) ) );
                    m_reporter->Report( msg, REPORTER::RPT_ACTION );

                    msg.Printf( _( "Remove single pad net \"%s\" on \"%s\" pad \"%s\"\n" ),
                                GetChars( getNetname( previouspad ) ),
                                GetChars( previouspad->GetParent()->GetReference() ),
                                GetChars( previouspad->GetName() ) );
                    m_reporter->Report( msg, REPORTER::RPT_ACTION );

                    if( !m_isDryRun )
                        previouspad->SetNetCode( NETINFO_LIST::UNCONNECTED );
                    else
                        cacheNetname( previouspad, wxEmptyString );
                }
            }

            netname = getNetname( pad );
            count = 1;
        }
        else
        {
            count++;
        }

        previouspad = pad;
    }

    // Examine last pad
    if( pad && count == 1 )
    {
        if( !m_isDryRun )
            pad->SetNetCode( NETINFO_LIST::UNCONNECTED );
        else
            cacheNetname( pad, wxEmptyString );
    }

    return true;
}


bool BOARD_NETLIST_UPDATER::testConnectivity( NETLIST& aNetlist )
{
    // Verify that board contains all pads in netlist: if it doesn't then footprints are
    // wrong or missing.
    // Note that we use references to find the footprints as they're already updated by this
    // point (whether by-reference or by-timestamp).

    wxString msg;
    wxString padname;

    for( int i = 0; i < (int) aNetlist.GetCount(); i++ )
    {
        const COMPONENT* component = aNetlist.GetComponent( i );
        MODULE* footprint = m_board->FindModuleByReference( component->GetReference() );

        if( footprint == NULL )    // It can be missing in partial designs
            continue;

        // Explore all pins/pads in component
        for( unsigned jj = 0; jj < component->GetNetCount(); jj++ )
        {
            const COMPONENT_NET& net = component->GetNet( jj );
            padname = net.GetPinName();

            if( footprint->FindPadByName( padname ) )
                continue;   // OK, pad found

            // not found: bad footprint, report error
            msg.Printf( _( "Component %s pad %s not found in footprint %s\n" ),
                        GetChars( component->GetReference() ),
                        GetChars( padname ),
                        GetChars( footprint->GetFPID().Format() ) );
            m_reporter->Report( msg, REPORTER::RPT_ERROR );
            ++m_errorCount;
        }
    }

    return true;
}


bool BOARD_NETLIST_UPDATER::UpdateNetlist( NETLIST& aNetlist )
{
    wxString msg;
    m_errorCount = 0;
    m_warningCount = 0;

    cacheCopperZoneConnections();

    if( !m_isDryRun )
    {
        m_board->SetStatus( 0 );
    }

    for( int i = 0; i < (int) aNetlist.GetCount();  i++ )
    {
        COMPONENT* component = aNetlist.GetComponent( i );
        MODULE* footprint = NULL;

        msg.Printf( _( "Processing component \"%s:%s:%s\".\n" ),
                    GetChars( component->GetReference() ),
                    GetChars( component->GetTimeStamp() ),
                    GetChars( component->GetFPID().Format() ) );
        m_reporter->Report( msg, REPORTER::RPT_INFO );

        if( aNetlist.IsFindByTimeStamp() )
            footprint = m_board->FindModule( component->GetTimeStamp(), true );
        else
            footprint = m_board->FindModule( component->GetReference() );

        if( footprint )        // An existing footprint.
        {
            MODULE* newFootprint = replaceComponent( aNetlist, footprint, component );

            if( newFootprint )
                footprint = newFootprint;
        }
        else
        {
            footprint = addNewComponent( component );
        }

        if( footprint )
        {
            updateComponentParameters( footprint, component );
            updateComponentPadConnections( footprint, component );
        }
    }

    updateCopperZoneNets( aNetlist );

    if( m_deleteUnusedComponents )
        deleteUnusedComponents( aNetlist );

    if( m_deleteSinglePadNets )
        deleteSinglePadNets();

    if( !m_isDryRun )
    {
        m_commit.Push( _( "Update netlist" ) );
        m_board->GetConnectivity()->Build( m_board );
        testConnectivity( aNetlist );
    }

    // Update the ratsnest
    m_reporter->ReportTail( wxT( "" ), REPORTER::RPT_ACTION );
    m_reporter->ReportTail( wxT( "" ), REPORTER::RPT_ACTION );

    msg.Printf( _( "Total warnings: %d, errors: %d." ), m_warningCount, m_errorCount );
    m_reporter->ReportTail( msg, REPORTER::RPT_ACTION );

    if( m_errorCount )
    {
        m_reporter->ReportTail( _( "Errors occurred during the netlist update. Unless you "
                               "fix them, your board will not be consistent with the schematics." ),
                            REPORTER::RPT_ERROR );

        return false;
    }
    else
    {
        m_reporter->ReportTail( _( "Netlist update successful!" ), REPORTER::RPT_ACTION );
    }

    return true;
}


bool BOARD_NETLIST_UPDATER::UpdateNetlist( const wxString& aNetlistFileName,
                                           const wxString& aCmpFileName )
{
    return false;
}
