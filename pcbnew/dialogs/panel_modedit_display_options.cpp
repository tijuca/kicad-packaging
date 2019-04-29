/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017-2018 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
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

#include <class_drawpanel.h>
#include <footprint_edit_frame.h>
#include <view/view.h>
#include <widgets/gal_options_panel.h>
#include <widgets/paged_dialog.h>

#include <panel_modedit_display_options.h>


PANEL_MODEDIT_DISPLAY_OPTIONS::PANEL_MODEDIT_DISPLAY_OPTIONS( FOOTPRINT_EDIT_FRAME* aFrame,
                                                              PAGED_DIALOG* aParent ) :
    wxPanel( aParent->GetTreebook(), wxID_ANY ),
    m_frame( aFrame )
{
    auto mainSizer = new wxBoxSizer( wxHORIZONTAL );
    SetSizer( mainSizer );

    // install GAL options pane
    KIGFX::GAL_DISPLAY_OPTIONS& galOptions = m_frame->GetGalDisplayOptions();

    m_galOptsPanel = new GAL_OPTIONS_PANEL( this, galOptions );
    mainSizer->Add( m_galOptsPanel, 1, wxEXPAND | wxLEFT, 5 );

    // a spacer to take up the other half of the width
    auto spacer = new wxPanel( this, wxID_ANY );
    mainSizer->Add( spacer, 1, wxEXPAND | wxLEFT, 5 );
}


bool PANEL_MODEDIT_DISPLAY_OPTIONS::TransferDataToWindow()
{
    m_galOptsPanel->TransferDataToWindow();
    return true;
}


bool PANEL_MODEDIT_DISPLAY_OPTIONS::TransferDataFromWindow()
{
    m_galOptsPanel->TransferDataFromWindow();

    // refresh view
    KIGFX::VIEW* view = m_frame->GetGalCanvas()->GetView();
    view->RecacheAllItems();
    view->MarkTargetDirty( KIGFX::TARGET_NONCACHED );
    m_frame->GetCanvas()->Refresh();

    return true;
}
