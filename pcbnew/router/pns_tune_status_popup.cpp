/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2014-2015  CERN
 * Copyright (C) 2016-2019 KiCad Developers, see AUTHORS.txt for contributors.
 * Author: Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "draw_frame.h"
#include "pns_tune_status_popup.h"
#include "pns_router.h"
#include "pns_meander_placer.h"

void PNS_TUNE_STATUS_POPUP::UpdateStatus( PNS::ROUTER* aRouter )
{
    PNS::MEANDER_PLACER_BASE* placer = dynamic_cast<PNS::MEANDER_PLACER_BASE*>( aRouter->Placer() );

    if( !placer )
        return;

    SetText( placer->TuningInfo( m_frame->GetUserUnits() ) );

    // Determine the background color first and choose a contrasting value
    COLOR4D bg( m_panel->GetBackgroundColour() );
    double h, s, l;
    bg.ToHSL( h, s, l );

    switch( placer->TuningStatus() )
    {
    case PNS::MEANDER_PLACER::TUNED:
        if( l < 0.5 )
            SetTextColor( wxColor( 0, 255, 0 ) );
        else
            SetTextColor( wxColor( 0, 128, 0 ) );

        break;
    case PNS::MEANDER_PLACER::TOO_SHORT:
        if( l < 0.5 )
            SetTextColor( wxColor( 255, 128, 128 ) );
        else
            SetTextColor( wxColor( 128, 64, 64 ) );

        break;
    case PNS::MEANDER_PLACER::TOO_LONG:
        if( l < 0.5 )
            SetTextColor( wxColor( 24, 24, 255 ) );
        else
            SetTextColor( wxColor( 19, 19, 195 ) );

        break;
    }
}

