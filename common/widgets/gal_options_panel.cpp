/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017-2019 KiCad Developers, see AUTHORS.txt for contributors.
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


#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>

#include <widgets/gal_options_panel.h>

#include <common.h>

#include <config_map.h>

/*
 * Spin control parameters
 */
static const double gridThicknessMin = 1.0;
static const double gridThicknessMax = 10.0;
static const double gridThicknessStep = 0.5;

static const double gridMinSpacingMin = 5;
static const double gridMinSpacingMax = 200;
static const double gridMinSpacingStep = 5;


static const UTIL::CFG_MAP<KIGFX::GRID_STYLE> gridStyleSelectMap =
{
    { KIGFX::GRID_STYLE::DOTS,        0 },  // Default
    { KIGFX::GRID_STYLE::LINES,       1 },
    { KIGFX::GRID_STYLE::SMALL_CROSS, 2 },
};


GAL_OPTIONS_PANEL::GAL_OPTIONS_PANEL( wxWindow* aParent, KIGFX::GAL_DISPLAY_OPTIONS& aGalOpts ):
    wxPanel( aParent, wxID_ANY ),
    m_galOptions( aGalOpts )
{
    // the main sizer that holds "columns" of settings
    m_mainSizer = new wxBoxSizer( wxHORIZONTAL );
    SetSizer( m_mainSizer );

    // second-level sizers that are one "column" of settings each
    wxBoxSizer* sLeftSizer = new wxBoxSizer( wxVERTICAL );
    m_mainSizer->Add( sLeftSizer, 1, wxALL | wxEXPAND, 0 );

    // @todo LEGACY: not required when legacy is gone
    const wxString galOnlySuffix = _( " (not supported in Legacy Toolset)" );

    /*
     * Grid settings subpanel
     */
    {
        wxStaticBoxSizer* sGridSettings;
        sGridSettings = new wxStaticBoxSizer( new wxStaticBox( this,
                wxID_ANY, _( "Grid Options" ) + galOnlySuffix ), wxVERTICAL );

        wxString m_gridStyleChoices[] = {
            _( "Dots" ),
            _( "Lines" ),
            _( "Small crosses" )
        };
        int m_gridStyleNChoices = sizeof( m_gridStyleChoices ) / sizeof( wxString );
        m_gridStyle = new wxRadioBox( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "Grid Style" ),
                wxDefaultPosition, wxDefaultSize,
                m_gridStyleNChoices, m_gridStyleChoices, 1, wxRA_SPECIFY_COLS );
        sGridSettings->Add( m_gridStyle, 0, wxALL|wxEXPAND, 5 );

        wxFlexGridSizer* sGridSettingsGrid;
        sGridSettingsGrid = new wxFlexGridSizer( 0, 3, 0, 0 );
        sGridSettingsGrid->AddGrowableCol( 1 );
        sGridSettingsGrid->SetFlexibleDirection( wxBOTH );
        sGridSettingsGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

        l_gridLineWidth = new wxStaticText( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "Grid thickness:" ) );
        l_gridLineWidth->Wrap( -1 );
        sGridSettingsGrid->Add( l_gridLineWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        m_gridLineWidth = new wxSpinCtrlDouble( sGridSettings->GetStaticBox(), wxID_ANY );
        m_gridLineWidth->SetRange( gridThicknessMin, gridThicknessMax );
        m_gridLineWidth->SetIncrement( gridThicknessStep );
        m_gridLineWidth->SetDigits( 1 );
        sGridSettingsGrid->Add( m_gridLineWidth, 0, wxEXPAND | wxTOP | wxBOTTOM, 5 );

        l_gridLineWidthUnits = new wxStaticText( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "px" ) );
        l_gridLineWidthUnits->Wrap( -1 );
        sGridSettingsGrid->Add( l_gridLineWidthUnits, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

        l_gridMinSpacing = new wxStaticText( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "Min grid spacing:" ) );
        l_gridMinSpacing->Wrap( -1 );
        sGridSettingsGrid->Add( l_gridMinSpacing, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        m_gridMinSpacing = new wxSpinCtrlDouble( sGridSettings->GetStaticBox(), wxID_ANY);
        m_gridMinSpacing->SetRange( gridMinSpacingMin, gridMinSpacingMax );
        m_gridMinSpacing->SetIncrement( gridMinSpacingStep );
        m_gridMinSpacing->SetDigits( 0 );
        sGridSettingsGrid->Add( m_gridMinSpacing, 0, wxEXPAND | wxTOP | wxBOTTOM, 5 );

        l_gridMinSpacingUnits = new wxStaticText( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "px" ) );
        l_gridMinSpacingUnits->Wrap( -1 );
        sGridSettingsGrid->Add( l_gridMinSpacingUnits, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

        sGridSettings->Add( sGridSettingsGrid, 1, wxALL | wxEXPAND, 5 );

        sLeftSizer->Add( sGridSettings, 0, wxTOP | wxBOTTOM | wxRIGHT | wxEXPAND, 5 );
    }

    /*
     * Cursor settings subpanel
     */
    {
        wxString cursorDisplayTitle = _( "Cursor Options" );

        // cursor is not shown in legacy on OSX
        // @todo LEGACY remove this
#ifdef __APPLE__
        cursorDisplayTitle += galOnlySuffix;
#endif

        auto sCursorSettings = new wxStaticBoxSizer( new wxStaticBox( this,
                wxID_ANY, cursorDisplayTitle ), wxVERTICAL );

        sLeftSizer->Add( sCursorSettings, 1, wxTOP | wxRIGHT | wxEXPAND, 5 );

        wxString m_CursorShapeChoices[] = {
            _( "Small crosshair" ),
            _( "Full window crosshair" )
        };

        int m_CursorShapeNChoices = sizeof( m_CursorShapeChoices ) / sizeof( wxString );
        m_cursorShape = new wxRadioBox( this, wxID_ANY,
                                        _( "Cursor Shape:" ), wxDefaultPosition, wxDefaultSize,
                                        m_CursorShapeNChoices, m_CursorShapeChoices, 1,
                                        wxRA_SPECIFY_COLS );

        m_cursorShape->SetSelection( 0 );
        m_cursorShape->SetToolTip( _( "Cursor shape for drawing, placement and movement tools" ) );

        sCursorSettings->Add( m_cursorShape, 0, wxALL | wxEXPAND, 5 );

#ifdef __APPLE__
        // Whole section is galOnly on OSX; no need for further qualifier here
        m_forceCursorDisplay = new wxCheckBox( this, wxID_ANY, _( "Always show crosshairs" ) );
#else
        // User a shorter galOnly qualifier as otherwise the label is obnoxiously long
        // @todo LEGACY remove this
        m_forceCursorDisplay = new wxCheckBox( this, wxID_ANY,
                                         _( "Always show crosshairs (not in Legacy)" ) );
#endif

        sCursorSettings->Add( m_forceCursorDisplay, 0, wxALL | wxEXPAND, 5 );
    }
}


bool GAL_OPTIONS_PANEL::TransferDataToWindow()
{
    m_gridStyle->SetSelection( UTIL::GetConfigForVal(
            gridStyleSelectMap, m_galOptions.m_gridStyle ) );

    m_gridLineWidth->SetValue( m_galOptions.m_gridLineWidth );

    m_gridMinSpacing->SetValue( m_galOptions.m_gridMinSpacing );

    m_cursorShape->SetSelection( m_galOptions.m_fullscreenCursor );

    m_forceCursorDisplay->SetValue( m_galOptions.m_forceDisplayCursor );

    return true;
}


bool GAL_OPTIONS_PANEL::TransferDataFromWindow()
{
    m_galOptions.m_gridStyle = UTIL::GetValFromConfig(
            gridStyleSelectMap, m_gridStyle->GetSelection() );

    m_galOptions.m_gridLineWidth = m_gridLineWidth->GetValue();

    m_galOptions.m_gridMinSpacing = m_gridMinSpacing->GetValue();

    m_galOptions.m_fullscreenCursor = m_cursorShape->GetSelection();

    m_galOptions.m_forceDisplayCursor = m_forceCursorDisplay->GetValue();

    m_galOptions.NotifyChanged();

    return true;
}
