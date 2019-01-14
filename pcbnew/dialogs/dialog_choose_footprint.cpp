/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2014 Henner Zeller <h.zeller@acm.org>
 * Copyright (C) 2016-2018 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <dialog_choose_footprint.h>

#include <algorithm>
#include <set>
#include <wx/utils.h>

#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/timer.h>
#include <wx/utils.h>

#include <pcb_base_frame.h>
#include <fp_lib_table.h>
#include <widgets/lib_tree.h>
#include <widgets/footprint_preview_widget.h>
#include <widgets/footprint_select_widget.h>


wxSize DIALOG_CHOOSE_FOOTPRINT::m_last_dlg_size( -1, -1 );
int DIALOG_CHOOSE_FOOTPRINT::m_h_sash_pos = 0;
int DIALOG_CHOOSE_FOOTPRINT::m_v_sash_pos = 0;


DIALOG_CHOOSE_FOOTPRINT::DIALOG_CHOOSE_FOOTPRINT( PCB_BASE_FRAME* aParent,
                                                  const wxString& aTitle,
                                                  FP_TREE_MODEL_ADAPTER::PTR& aAdapter,
                                                  bool aAllowBrowser )
        : DIALOG_SHIM( aParent, wxID_ANY, aTitle, wxDefaultPosition, wxDefaultSize,
                       wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
          m_browser_button( nullptr ),
          m_hsplitter( nullptr ),
          m_vsplitter( nullptr ),
          m_parent( aParent ),
          m_external_browser_requested( false )
{
    auto          sizer = new wxBoxSizer( wxVERTICAL );
    wxHtmlWindow* details = nullptr;

    m_vsplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxSP_LIVE_UPDATE );

    m_hsplitter = new wxSplitterWindow( m_vsplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxSP_LIVE_UPDATE );

    //Avoid the splitter window being assigned as the Parent to additional windows
    m_hsplitter->SetExtraStyle( wxWS_EX_TRANSIENT );

    auto detailsPanel = new wxPanel( m_vsplitter );
    auto detailsSizer = new wxBoxSizer( wxVERTICAL );
    detailsPanel->SetSizer( detailsSizer );

    details = new wxHtmlWindow( detailsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxHW_SCROLLBAR_AUTO );
    detailsSizer->Add( details, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5 );
    detailsPanel->Layout();
    detailsSizer->Fit( detailsPanel );

    m_vsplitter->SetSashGravity( 0.5 );
    m_vsplitter->SetMinimumPaneSize( 20 );
    m_vsplitter->SplitHorizontally( m_hsplitter, detailsPanel );

    sizer->Add( m_vsplitter, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5 );

    m_tree = new LIB_TREE( m_hsplitter, Prj().PcbFootprintLibs(), aAdapter,
                                 LIB_TREE::WIDGETS::ALL, details );

    m_hsplitter->SetSashGravity( 0.8 );
    m_hsplitter->SetMinimumPaneSize( 20 );
    m_hsplitter->SplitVertically( m_tree,  ConstructRightPanel( m_hsplitter ) );

    m_dbl_click_timer = new wxTimer( this );

    auto buttonsSizer = new wxBoxSizer( wxHORIZONTAL );

    if( aAllowBrowser )
    {
        m_browser_button = new wxButton( this, wxID_ANY, _( "Select with Browser" ) );
        buttonsSizer->Add( m_browser_button, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    }

    auto sdbSizer = new wxStdDialogButtonSizer();
    auto okButton = new wxButton( this, wxID_OK );
    auto cancelButton = new wxButton( this, wxID_CANCEL );
    sdbSizer->AddButton( okButton );
    sdbSizer->AddButton( cancelButton );
    sdbSizer->Realize();

    buttonsSizer->Add( sdbSizer, 1, wxALL, 5 );

    sizer->Add( buttonsSizer, 0, wxEXPAND | wxLEFT, 5 );
    SetSizer( sizer );

    Bind( wxEVT_TIMER, &DIALOG_CHOOSE_FOOTPRINT::OnCloseTimer, this, m_dbl_click_timer->GetId() );
    Bind( COMPONENT_PRESELECTED, &DIALOG_CHOOSE_FOOTPRINT::OnComponentPreselected, this );
    Bind( COMPONENT_SELECTED, &DIALOG_CHOOSE_FOOTPRINT::OnComponentSelected, this );

    if( m_browser_button )
        m_browser_button->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &DIALOG_CHOOSE_FOOTPRINT::OnUseBrowser, this );

    Layout();

    // We specify the width of the right window (m_symbol_view_panel), because specify
    // the width of the left window does not work as expected when SetSashGravity() is called
    m_hsplitter->SetSashPosition( m_h_sash_pos ? m_h_sash_pos : HorizPixelsFromDU( 220 ) );

    if( m_vsplitter )
        m_vsplitter->SetSashPosition( m_v_sash_pos ? m_v_sash_pos : VertPixelsFromDU( 230 ) );

    if( m_last_dlg_size == wxSize( -1, -1 ) )
        SetSizeInDU( 440, 340 );
    else
        SetSize( m_last_dlg_size );

    SetInitialFocus( m_tree );
    okButton->SetDefault();
}


DIALOG_CHOOSE_FOOTPRINT::~DIALOG_CHOOSE_FOOTPRINT()
{
    Unbind( wxEVT_TIMER, &DIALOG_CHOOSE_FOOTPRINT::OnCloseTimer, this );
    Unbind( COMPONENT_PRESELECTED, &DIALOG_CHOOSE_FOOTPRINT::OnComponentPreselected, this );
    Unbind( COMPONENT_SELECTED, &DIALOG_CHOOSE_FOOTPRINT::OnComponentSelected, this );

    if( m_browser_button )
        m_browser_button->Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &DIALOG_CHOOSE_FOOTPRINT::OnUseBrowser, this );

    // I am not sure the following two lines are necessary,
    // but they will not hurt anyone
    m_dbl_click_timer->Stop();
    delete m_dbl_click_timer;

    m_last_dlg_size = GetSize();
    m_h_sash_pos = m_hsplitter->GetSashPosition();

    if( m_vsplitter )
        m_v_sash_pos = m_vsplitter->GetSashPosition();
}


wxPanel* DIALOG_CHOOSE_FOOTPRINT::ConstructRightPanel( wxWindow* aParent )
{
    auto panel = new wxPanel( aParent );
    auto sizer = new wxBoxSizer( wxVERTICAL );

    m_preview_ctrl = new FOOTPRINT_PREVIEW_WIDGET( panel, Kiway() );
    sizer->Add( m_preview_ctrl, 1, wxEXPAND | wxTOP | wxRIGHT, 5 );

    panel->SetSizer( sizer );
    panel->Layout();
    sizer->Fit( panel );

    return panel;
}


LIB_ID DIALOG_CHOOSE_FOOTPRINT::GetSelectedLibId() const
{
    return m_tree->GetSelectedLibId();
}


void DIALOG_CHOOSE_FOOTPRINT::OnUseBrowser( wxCommandEvent& aEvent )
{
    m_external_browser_requested = true;
    EndQuasiModal( wxID_OK );
}


void DIALOG_CHOOSE_FOOTPRINT::OnCloseTimer( wxTimerEvent& aEvent )
{
    // Hack handler because of eaten MouseUp event. See
    // DIALOG_CHOOSE_FOOTPRINT::OnComponentSelected for the beginning
    // of this spaghetti noodle.

    auto state = wxGetMouseState();

    if( state.LeftIsDown() )
    {
        // Mouse hasn't been raised yet, so fire the timer again. Otherwise the
        // purpose of this timer is defeated.
        m_dbl_click_timer->StartOnce( DIALOG_CHOOSE_FOOTPRINT::DblClickDelay );
    }
    else
    {
        EndQuasiModal( wxID_OK );
    }
}


void DIALOG_CHOOSE_FOOTPRINT::OnComponentPreselected( wxCommandEvent& aEvent )
{
    if( !m_preview_ctrl || !m_preview_ctrl->IsInitialized() )
        return;

    LIB_ID lib_id = m_tree->GetSelectedLibId();

    if( !lib_id.IsValid() )
    {
        m_preview_ctrl->SetStatusText( _( "No footprint selected" ) );
    }
    else
    {
        m_preview_ctrl->ClearStatus();
        m_preview_ctrl->CacheFootprint( lib_id );
        m_preview_ctrl->DisplayFootprint( lib_id );
    }
}


void DIALOG_CHOOSE_FOOTPRINT::OnComponentSelected( wxCommandEvent& aEvent )
{
    if( m_tree->GetSelectedLibId().IsValid() )
    {
        // Got a selection. We can't just end the modal dialog here, because
        // wx leaks some events back to the parent window (in particular, the
        // MouseUp following a double click).
        //
        // NOW, here's where it gets really fun. wxTreeListCtrl eats MouseUp.
        // This isn't really feasible to bypass without a fully custom
        // wxDataViewCtrl implementation, and even then might not be fully
        // possible (docs are vague). To get around this, we use a one-shot
        // timer to schedule the dialog close.
        //
        // See DIALOG_CHOOSE_FOOTPRINT::OnCloseTimer for the other end of this
        // spaghetti noodle.
        m_dbl_click_timer->StartOnce( DIALOG_CHOOSE_FOOTPRINT::DblClickDelay );
    }
}
