/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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

#include <dialogs/dialog_track_via_properties.h>
#include <pcb_layer_box_selector.h>
#include <tools/selection_tool.h>
#include <class_track.h>
#include <pcb_edit_frame.h>
#include <confirm.h>
#include <widgets/text_ctrl_eval.h>

#include <widgets/widget_net_selector.h>
#include <board_commit.h>

DIALOG_TRACK_VIA_PROPERTIES::DIALOG_TRACK_VIA_PROPERTIES( PCB_BASE_FRAME* aParent, const SELECTION& aItems ) :
    DIALOG_TRACK_VIA_PROPERTIES_BASE( aParent ), m_items( aItems ),
    m_trackStartX( aParent, m_TrackStartXCtrl, m_TrackStartXUnit ),
    m_trackStartY( aParent, m_TrackStartYCtrl, m_TrackStartYUnit ),
    m_trackEndX( aParent, m_TrackEndXCtrl, m_TrackEndXUnit ),
    m_trackEndY( aParent, m_TrackEndYCtrl, m_TrackEndYUnit ),
    m_trackWidth( aParent, m_TrackWidthCtrl, m_TrackWidthUnit ),
    m_viaX( aParent, m_ViaXCtrl, m_ViaXUnit ), m_viaY( aParent, m_ViaYCtrl, m_ViaYUnit ),
    m_viaDiameter( aParent, m_ViaDiameterCtrl, m_ViaDiameterUnit ),
    m_viaDrill( aParent, m_ViaDrillCtrl, m_ViaDrillUnit ),
    m_tracks( false ), m_vias( false )
{
    wxASSERT( !m_items.Empty() );

    // This is a way to trick gcc into considering these variables as initialized
    OPT<int> trackStartX( []()->OPT<int> { return NULLOPT; }() );
    OPT<int> trackStartY( []()->OPT<int> { return NULLOPT; }() );
    OPT<int> trackEndX( []()->OPT<int> { return NULLOPT; }() );
    OPT<int> trackEndY( []()->OPT<int> { return NULLOPT; }() );
    OPT<int> trackWidth( []()->OPT<int> { return NULLOPT; }() );
    OPT<PCB_LAYER_ID> trackLayer( []()->OPT<PCB_LAYER_ID> { return NULLOPT; }() );
    OPT<int> viaX( []()->OPT<int> { return NULLOPT; }() );
    OPT<int> viaY( []()->OPT<int> { return NULLOPT; }() );
    OPT<int> viaDiameter( []()->OPT<int> { return NULLOPT; }() );
    OPT<int> viaDrill( []()->OPT<int> { return NULLOPT; }() );

    VIATYPE_T viaType = VIA_NOT_DEFINED;
    PCB_LAYER_ID viaStartLayer = UNDEFINED_LAYER;
    PCB_LAYER_ID viaEndLayer = UNDEFINED_LAYER;

    m_haveUniqueNet = true;
    int prevNet = -1;

    m_NetComboBox->SetBoard( aParent->GetBoard() );
    m_NetComboBox->Enable( true );

    bool hasLocked = false;
    bool hasUnlocked = false;

    for( auto& item : m_items )
    {
        int net = static_cast<BOARD_CONNECTED_ITEM*>(item)->GetNetCode();

        if( prevNet >= 0 && net != prevNet )
        {
            DBG( printf("prev %d net %d\n", net, prevNet ) );
            m_haveUniqueNet = false;
            break;
        }

        prevNet = net;
    }

    if ( m_haveUniqueNet )
    {
        m_NetComboBox->SetSelectedNet( prevNet );
    }
    else
    {
        m_NetComboBox->SetMultiple( true );
    }


    // Look for values that are common for every item that is selected
    for( auto& item : m_items )
    {
        switch( item->Type() )
        {
            case PCB_TRACE_T:
            {
                const TRACK* t = static_cast<const TRACK*>( item );

                if( !m_tracks )     // first track in the list
                {
                    trackStartX = t->GetStart().x;
                    trackStartY = t->GetStart().y;
                    trackEndX   = t->GetEnd().x;
                    trackEndY   = t->GetEnd().y;
                    trackWidth  = t->GetWidth();
                    trackLayer  = t->GetLayer();
                    m_tracks    = true;
                }
                else        // check if values are the same for every selected track
                {
                    if( trackStartX && ( *trackStartX != t->GetStart().x ) )
                        trackStartX = NULLOPT;

                    if( trackStartY && ( *trackStartY != t->GetStart().y ) )
                        trackStartY = NULLOPT;

                    if( trackEndX && ( *trackEndX != t->GetEnd().x ) )
                        trackEndX = NULLOPT;

                    if( trackEndY && ( *trackEndY != t->GetEnd().y ) )
                        trackEndY = NULLOPT;

                    if( trackWidth && ( *trackWidth != t->GetWidth() ) )
                        trackWidth = NULLOPT;

                    if( trackLayer && ( *trackLayer != t->GetLayer() ) )
                        trackLayer = NULLOPT;
                }

                if( t->IsLocked() )
                    hasLocked = true;
                else
                    hasUnlocked = true;

                break;
            }

            case PCB_VIA_T:
            {
                const VIA* v = static_cast<const VIA*>( item );

                if( !m_vias )       // first via in the list
                {
                    viaX = v->GetPosition().x;
                    viaY = v->GetPosition().y;
                    viaDiameter = v->GetWidth();
                    viaDrill = v->GetDrillValue();
                    m_vias = true;
                    viaType = v->GetViaType();
                    viaStartLayer = v->TopLayer();
                    viaEndLayer = v->BottomLayer();
                }
                else        // check if values are the same for every selected via
                {
                    if( viaX && ( *viaX != v->GetPosition().x ) )
                        viaX = NULLOPT;

                    if( viaY && ( *viaY != v->GetPosition().y ) )
                        viaY = NULLOPT;

                    if( viaDiameter && ( *viaDiameter != v->GetWidth() ) )
                        viaDiameter = NULLOPT;

                    if( viaDrill && ( *viaDrill != v->GetDrillValue() ) )
                        viaDrill = NULLOPT;

                    if( viaType != v->GetViaType() )
                        viaType = VIA_NOT_DEFINED;

                    if( viaStartLayer != v->TopLayer() )
                        viaStartLayer = UNDEFINED_LAYER;

                    if( viaEndLayer != v->BottomLayer() )
                        viaEndLayer = UNDEFINED_LAYER;
                }

                if( v->IsLocked() )
                    hasLocked = true;
                else
                    hasUnlocked = true;

                break;
            }

            default:
            {
                wxASSERT( false );
                break;
            }
        }
    }

    wxASSERT( m_tracks || m_vias );

    if( m_vias )
    {
        setCommonVal( viaX, m_ViaXCtrl, m_viaX );
        setCommonVal( viaY, m_ViaYCtrl, m_viaY );
        setCommonVal( viaDiameter, m_ViaDiameterCtrl, m_viaDiameter );
        setCommonVal( viaDrill, m_ViaDrillCtrl, m_viaDrill );

        m_DesignRuleViasUnit->SetLabel( GetAbbreviatedUnitsLabel( g_UserUnit ) );

        int viaSelection = wxNOT_FOUND;

        for( unsigned ii = 0; ii < aParent->GetDesignSettings().m_ViasDimensionsList.size(); ii++ )
        {
            VIA_DIMENSION* viaDimension = &aParent->GetDesignSettings().m_ViasDimensionsList[ii];
            wxString msg = StringFromValue( g_UserUnit, viaDimension->m_Diameter, false )
                + " / " + StringFromValue( g_UserUnit, viaDimension->m_Drill, false );
            m_DesignRuleViasCtrl->Append( msg, viaDimension );

            if( viaSelection == wxNOT_FOUND && viaDiameter == viaDimension->m_Diameter
                && viaDrill == viaDimension->m_Drill )
            {
                viaSelection = ii;
            }
        }

        m_DesignRuleViasCtrl->SetSelection( viaSelection );

        m_DesignRuleViasCtrl->Connect( wxEVT_CHOICE, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES::onViaSelect ), NULL, this );
        m_ViaDiameterCtrl->Connect( wxEVT_TEXT, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES::onViaEdit ), NULL, this );
        m_ViaDrillCtrl->Connect( wxEVT_TEXT, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES::onViaEdit ), NULL, this );
        m_ViaTypeChoice->Connect( wxEVT_CHOICE, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES::onViaEdit ), NULL, this );

        m_ViaDiameterCtrl->SetFocus();

        m_ViaTypeChoice->Enable();

        if( viaType == VIA_THROUGH )
            m_ViaTypeChoice->SetSelection( 0 );
        else if( viaType == VIA_MICROVIA )
            m_ViaTypeChoice->SetSelection( 1 );
        else if ( viaType == VIA_BLIND_BURIED )
            m_ViaTypeChoice->SetSelection( 2 );
        else if( viaType == VIA_NOT_DEFINED )
            m_ViaTypeChoice->SetSelection( 3 );


        m_ViaStartLayer->SetLayersHotkeys( false );
        m_ViaStartLayer->SetNotAllowedLayerSet( LSET::AllNonCuMask() );
        m_ViaStartLayer->SetBoardFrame( aParent );
        m_ViaStartLayer->Resync();
        m_ViaEndLayer->SetLayersHotkeys( false );
        m_ViaEndLayer->SetNotAllowedLayerSet( LSET::AllNonCuMask() );
        m_ViaEndLayer->SetBoardFrame( aParent );
        m_ViaEndLayer->Resync();


        m_ViaStartLayer->SetLayerSelection( viaStartLayer );
        m_ViaEndLayer->SetLayerSelection( viaEndLayer );

        m_ViaStartLayer->Enable( false );
        m_ViaEndLayer->Enable( false );

        if( viaType != VIA_THROUGH ) // check if selected type isnt through.
        {
            m_ViaStartLayer->Enable();
            m_ViaEndLayer->Enable();
        }

    }
    else
    {
        m_MainSizer->Hide( m_sbViaSizer, true );
    }

    if( m_tracks )
    {
        setCommonVal( trackStartX, m_TrackStartXCtrl, m_trackStartX );
        setCommonVal( trackStartY, m_TrackStartYCtrl, m_trackStartY );
        setCommonVal( trackEndX, m_TrackEndXCtrl, m_trackEndX );
        setCommonVal( trackEndY, m_TrackEndYCtrl, m_trackEndY );
        setCommonVal( trackWidth, m_TrackWidthCtrl, m_trackWidth );

        for( unsigned ii = 0; ii < aParent->GetDesignSettings().m_TrackWidthList.size(); ii++ )
        {
            int width = aParent->GetDesignSettings().m_TrackWidthList[ii];
            wxString msg = StringFromValue( g_UserUnit, width, false );
            m_TrackWidthCtrl->Append( msg );
        }

        m_TrackLayerCtrl->SetLayersHotkeys( false );
        m_TrackLayerCtrl->SetNotAllowedLayerSet( LSET::AllNonCuMask() );
        m_TrackLayerCtrl->SetBoardFrame( aParent );
        m_TrackLayerCtrl->Resync();

        if( trackLayer )
            m_TrackLayerCtrl->SetLayerSelection( *trackLayer );

        m_TrackWidthCtrl->SetFocus();
    }
    else
    {
        m_MainSizer->Hide( m_sbTrackSizer, true );
    }

    if( hasLocked && hasUnlocked )
    {
         m_lockedCbox->Set3StateValue( wxCHK_UNDETERMINED );
    }
    else if( hasLocked )
    {
        m_lockedCbox->Set3StateValue( wxCHK_CHECKED );
    }
    else
    {
        m_lockedCbox->Set3StateValue( wxCHK_UNCHECKED );
    }


    m_StdButtonsOK->SetDefault();

    // Pressing ENTER when any of the text input fields is active applies changes
    Connect( wxEVT_TEXT_ENTER, wxCommandEventHandler( DIALOG_TRACK_VIA_PROPERTIES::onOkClick ),
             NULL, this );
}


bool DIALOG_TRACK_VIA_PROPERTIES::Apply( COMMIT& aCommit )
{
    if( !check() )
        return false;

    bool changeLock = m_lockedCbox->Get3StateValue() != wxCHK_UNDETERMINED;
    bool setLock = m_lockedCbox->Get3StateValue() == wxCHK_CHECKED;

    for( auto item : m_items )
    {
        aCommit.Modify( item );

        switch( item->Type() )
        {
            case PCB_TRACE_T:
            {
                wxASSERT( m_tracks );
                TRACK* t = static_cast<TRACK*>( item );

                if( m_trackStartX.Valid() || m_trackStartY.Valid() )
                {
                    wxPoint start = t->GetStart();

                    if( m_trackStartX.Valid() )
                        start.x = m_trackStartX.GetValue();

                    if( m_trackStartY.Valid() )
                        start.y = m_trackStartY.GetValue();

                    t->SetStart( start );
                }

                if( m_trackEndX.Valid() || m_trackEndY.Valid() )
                {
                    wxPoint end = t->GetEnd();

                    if( m_trackEndX.Valid() )
                        end.x = m_trackEndX.GetValue();

                    if( m_trackEndY.Valid() )
                        end.y = m_trackEndY.GetValue();

                    t->SetEnd( end );
                }

                if( m_trackNetclass->IsChecked() )
                {
                    t->SetWidth( t->GetNetClass()->GetTrackWidth() );
                }
                else if( m_trackWidth.Valid() )
                {
                    t->SetWidth( m_trackWidth.GetValue() );
                }

                LAYER_NUM layer = m_TrackLayerCtrl->GetLayerSelection();

                if( layer != UNDEFINED_LAYER )
                    t->SetLayer( (PCB_LAYER_ID) layer );

                if( changeLock )
                    t->SetLocked( setLock );

                if ( m_NetComboBox->IsUniqueNetSelected() )
                {
                    DBG( printf( "snc %d\n", m_NetComboBox->GetSelectedNet() ) );
                    t->SetNetCode( m_NetComboBox->GetSelectedNet() );
                }


                break;
            }

            case PCB_VIA_T:
            {
                wxASSERT( m_vias );

                VIA* v = static_cast<VIA*>( item );

                if( m_viaX.Valid() || m_viaY.Valid() )
                {
                    wxPoint pos = v->GetPosition();

                    if( m_viaX.Valid() )
                        pos.x = m_viaX.GetValue();

                    if( m_viaY.Valid() )
                        pos.y = m_viaY.GetValue();

                    v->SetPosition( pos );
                }

                if( m_ViaTypeChoice->GetSelection() != 3)
                {
                    switch( m_ViaTypeChoice->GetSelection() )
                    {
                        case 0:
                            v->SetViaType( VIA_THROUGH );
                            v->SanitizeLayers();
                            break;
                        case 1:
                            v->SetViaType( VIA_MICROVIA );
                            break;
                        case 2:
                            v->SetViaType( VIA_BLIND_BURIED );
                            break;
                        default:
                            break;
                    }

                }

                auto startLayer = static_cast<PCB_LAYER_ID>( m_ViaStartLayer->GetLayerSelection() );
                auto endLayer = static_cast<PCB_LAYER_ID>( m_ViaEndLayer->GetLayerSelection() );

                if (startLayer != UNDEFINED_LAYER )
                    v->SetTopLayer( startLayer );

                if (endLayer != UNDEFINED_LAYER )
                    v->SetBottomLayer( endLayer );

                v->SanitizeLayers();

                if( m_viaNetclass->IsChecked() )
                {
                    switch( v->GetViaType() )
                    {
                    default:
                        wxFAIL_MSG("Unhandled via type");
                        // fall through

                    case VIA_THROUGH:
                    case VIA_BLIND_BURIED:
                        v->SetWidth( v->GetNetClass()->GetViaDiameter() );
                        v->SetDrill( v->GetNetClass()->GetViaDrill() );
                        break;

                    case VIA_MICROVIA:
                        v->SetWidth( v->GetNetClass()->GetuViaDiameter() );
                        v->SetDrill( v->GetNetClass()->GetuViaDrill() );

                        break;
                    }
                }
                else
                {
                    if( m_viaDiameter.Valid() )
                        v->SetWidth( m_viaDiameter.GetValue() );

                    if( m_viaDrill.Valid() )
                        v->SetDrill( m_viaDrill.GetValue() );

                }

                if ( m_NetComboBox->IsUniqueNetSelected() )
                {
                    DBG( printf( "snc %d\n", m_NetComboBox->GetSelectedNet() ) );
                    v->SetNetCode( m_NetComboBox->GetSelectedNet() );
                }

                if( changeLock )
                    v->SetLocked( setLock );

                break;
            }

            default:
                wxASSERT( false );
                break;
        }
    }

    return true;
}


void DIALOG_TRACK_VIA_PROPERTIES::onClose( wxCloseEvent& aEvent )
{
    EndModal( 0 );
}


void DIALOG_TRACK_VIA_PROPERTIES::onTrackNetclassCheck( wxCommandEvent& aEvent )
{
    bool enableNC = aEvent.IsChecked();

    m_TrackWidthLabel->Enable( !enableNC );
    m_TrackWidthCtrl->Enable( !enableNC );
    m_TrackWidthUnit->Enable( !enableNC );
}


void DIALOG_TRACK_VIA_PROPERTIES::onViaNetclassCheck( wxCommandEvent& aEvent )
{
    bool enableNC = aEvent.IsChecked();

    m_DesignRuleVias->Enable( !enableNC );
    m_DesignRuleViasCtrl->Enable( !enableNC );
    m_DesignRuleViasUnit->Enable( !enableNC );

    m_ViaDiameterLabel->Enable( !enableNC );
    m_ViaDiameterCtrl->Enable( !enableNC );
    m_ViaDiameterUnit->Enable( !enableNC );

    m_ViaDrillLabel->Enable( !enableNC );
    m_ViaDrillCtrl->Enable( !enableNC );
    m_ViaDrillUnit->Enable( !enableNC );
}


void DIALOG_TRACK_VIA_PROPERTIES::onCancelClick( wxCommandEvent& aEvent )
{
    EndModal( 0 );
}


void DIALOG_TRACK_VIA_PROPERTIES::onOkClick( wxCommandEvent& aEvent )
{
    if( check() )
        EndModal( 1 );
}


void DIALOG_TRACK_VIA_PROPERTIES::onViaSelect( wxCommandEvent& aEvent )
{
    VIA_DIMENSION* viaDimension = static_cast<VIA_DIMENSION*> ( aEvent.GetClientData() );

    wxString msg = StringFromValue( g_UserUnit, viaDimension->m_Diameter, false );
    m_ViaDiameterCtrl->ChangeValue( msg );

    msg = StringFromValue( g_UserUnit, viaDimension->m_Drill, false );
    m_ViaDrillCtrl->ChangeValue( msg );
}


void DIALOG_TRACK_VIA_PROPERTIES::onViaEdit( wxCommandEvent& aEvent )
{
    m_DesignRuleViasCtrl->SetSelection( wxNOT_FOUND );

    if( m_vias )
    {
        if( m_ViaTypeChoice->GetSelection() != 0 ) // check if selected type isnt through.
        {
            m_ViaStartLayer->Enable();
            m_ViaEndLayer->Enable();
        }
        else
        {
            m_ViaStartLayer->SetLayerSelection( F_Cu );
            m_ViaEndLayer->SetLayerSelection( B_Cu );

            m_ViaStartLayer->Enable( false );
            m_ViaEndLayer->Enable( false );
        }
    }

}


bool DIALOG_TRACK_VIA_PROPERTIES::check() const
{
    bool trackNetclass = m_trackNetclass->IsChecked();
    bool viaNetclass = m_trackNetclass->IsChecked();

    if( m_tracks && !trackNetclass && m_trackWidth.Valid() && m_trackWidth.GetValue() <= 0 )
    {
        DisplayError( GetParent(), _( "Invalid track width" ) );
        m_TrackWidthCtrl->SetFocus();
        return false;
    }

    if( m_vias && !viaNetclass )
    {
        if( m_viaDiameter.Valid() && m_viaDiameter.GetValue() <= 0 )
        {
            DisplayError( GetParent(), _( "Invalid via diameter" ) );
            m_ViaDiameterCtrl->SetFocus();
            return false;
        }

        if( m_viaDrill.Valid() && m_viaDrill.GetValue() <= 0 )
        {
            DisplayError( GetParent(), _( "Invalid via drill size" ) );
            m_ViaDrillCtrl->SetFocus();
            return false;
        }

        if( m_viaDiameter.Valid() && m_viaDrill.Valid() && m_viaDiameter.GetValue() <= m_viaDrill.GetValue() )
        {
            DisplayError( GetParent(), _( "Via drill size has to be smaller than via diameter" ) );
            m_ViaDrillCtrl->SetFocus();
            return false;
        }

    }

    if( m_vias)
    {
        if( m_ViaStartLayer->GetLayerSelection() == m_ViaEndLayer->GetLayerSelection() )
        {
            DisplayError( GetParent(), _( "Via start layer and end layer cannot be the same" ) );
            return false;
        }
    }

    return true;
}
