/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 CERN
 * Author: Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
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

#include <widgets/progress_reporter.h>
#include <wx/evtloop.h>
#include <thread>

PROGRESS_REPORTER::PROGRESS_REPORTER( int aNumPhases ) :
    m_phase( 0 ),
    m_numPhases( aNumPhases ),
    m_progress( 0 ),
    m_maxProgress( 1 )
{
}


void PROGRESS_REPORTER::BeginPhase( int aPhase )
{
    m_phase.store( aPhase );
    m_progress.store( 0 );
}


void PROGRESS_REPORTER::AdvancePhase( )
{
    m_phase.fetch_add( 1 );
    m_progress.store( 0 );
}


void PROGRESS_REPORTER::Report( const wxString& aMessage )
{
    std::lock_guard<std::mutex> guard( m_mutex );
    m_rptMessage = aMessage;
}


void PROGRESS_REPORTER::SetMaxProgress( int aMaxProgress )
{
    m_maxProgress.store( aMaxProgress );
}


void PROGRESS_REPORTER::AdvanceProgress()
{
    m_progress.fetch_add( 1 );
}


int PROGRESS_REPORTER::currentProgress() const
{
    double current = ( 1.0 / (double) m_numPhases ) *
                     ( (double) m_phase + ( (double) m_progress.load() / (double) m_maxProgress ) );

    return (int)( current * 1000 );
}


bool PROGRESS_REPORTER::KeepRefreshing( bool aWait )
{
    if( aWait )
    {
        while( m_progress < m_maxProgress && m_maxProgress > 0 )
        {
            if( !updateUI() )
                return false;

            wxMilliSleep( 20 );
        }
        return true;
    }
    else
    {
        wxMilliSleep( 20 );

        return updateUI();
    }
}


WX_PROGRESS_REPORTER::WX_PROGRESS_REPORTER( wxWindow* aParent, const wxString& aTitle,
                                            int aNumPhases, bool aCanAbort ) :
    PROGRESS_REPORTER( aNumPhases ),
    wxProgressDialog( aTitle, wxT( "" ), 1, aParent,
                      // wxPD_APP_MODAL |   // Don't use; messes up OSX when called from
                                            // quasi-modal dialog
                      wxPD_AUTO_HIDE |      // *MUST* use; otherwise wxWidgets will spin
                                            // up another event loop on completion which
                                            // causes all sorts of grief
                      ( aCanAbort ? wxPD_CAN_ABORT : 0 ) |
                      wxPD_ELAPSED_TIME )
{
}


WX_PROGRESS_REPORTER::~WX_PROGRESS_REPORTER()
{
    Destroy();
}


bool WX_PROGRESS_REPORTER::updateUI()
{
    int cur = currentProgress();

    if( cur < 0 || cur > 1000 )
        cur = 0;

    wxString message;
    {
        std::lock_guard<std::mutex> guard( m_mutex );
        message = m_rptMessage;
    }

    SetRange( 1000 );
    return wxProgressDialog::Update( cur, message );
}


GAUGE_PROGRESS_REPORTER::GAUGE_PROGRESS_REPORTER( wxWindow* aParent, int aNumPhases ) :
        PROGRESS_REPORTER( aNumPhases ),
        wxGauge( aParent, wxID_ANY, 1000, wxDefaultPosition, wxDefaultSize,
                 wxGA_HORIZONTAL, wxDefaultValidator, wxGaugeNameStr )
{
}


bool GAUGE_PROGRESS_REPORTER::updateUI()
{
    int cur = currentProgress();

    if( cur < 0 || cur > 1000 )
        cur = 0;

    wxGauge::SetValue( cur );
    wxEventLoopBase::GetActive()->YieldFor(wxEVT_CATEGORY_UI);

    return true;  // No cancel button on a wxGauge
}



