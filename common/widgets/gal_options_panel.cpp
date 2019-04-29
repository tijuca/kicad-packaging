/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 KiCad Developers, see AUTHORS.txt for contributors.
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


#include <widgets/gal_options_panel.h>

#include <common.h>

#include <incremental_text_ctrl.h>
#include <config_map.h>

/*
 * Spin control parameters
 */
static const double gridThicknessMin = 1.0;
static const double gridThicknessMax = 10.0;
static const double gridThicknessStep = 1.0;

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
        sGridSettingsGrid = new wxFlexGridSizer( 0, 4, 0, 0 );
        sGridSettingsGrid->AddGrowableCol( 1 );
        sGridSettingsGrid->SetFlexibleDirection( wxBOTH );
        sGridSettingsGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

        l_gridLineWidth = new wxStaticText( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "Grid thickness:" ) );
        l_gridLineWidth->Wrap( -1 );
        sGridSettingsGrid->Add( l_gridLineWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        m_gridLineWidth = new wxTextCtrl( sGridSettings->GetStaticBox(), wxID_ANY );
        sGridSettingsGrid->Add( m_gridLineWidth, 0, wxEXPAND | wxTOP | wxBOTTOM, 5 );

        m_gridLineWidthSpinBtn = new wxSpinButton( sGridSettings->GetStaticBox(),
                wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS );
        sGridSettingsGrid->Add( m_gridLineWidthSpinBtn, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0 );

        l_gridLineWidthUnits = new wxStaticText( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "px" ) );
        l_gridLineWidthUnits->Wrap( -1 );
        sGridSettingsGrid->Add( l_gridLineWidthUnits, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        l_gridMinSpacing = new wxStaticText( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "Min grid spacing:" ) );
        l_gridMinSpacing->Wrap( -1 );
        sGridSettingsGrid->Add( l_gridMinSpacing, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        m_gridMinSpacing = new wxTextCtrl( sGridSettings->GetStaticBox(), wxID_ANY);
        sGridSettingsGrid->Add( m_gridMinSpacing, 0, wxEXPAND | wxTOP | wxBOTTOM, 5 );

        m_gridMinSpacingSpinBtn = new wxSpinButton( sGridSettings->GetStaticBox(),
                wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS );
        sGridSettingsGrid->Add( m_gridMinSpacingSpinBtn, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0 );

        l_gridMinSpacingUnits = new wxStaticText( sGridSettings->GetStaticBox(),
                wxID_ANY, _( "px" ) );
        l_gridMinSpacingUnits->Wrap( -1 );
        sGridSettingsGrid->Add( l_gridMinSpacingUnits, 0, wxALL, 5 );

        sGridSettings->Add( sGridSettingsGrid, 1, wxALL|wxEXPAND, 5 );

        sLeftSizer->Add( sGridSettings, 0, wxTOP | wxBOTTOM | wxRIGHT | wxEXPAND, 5 );

        // bind the spin buttons and text boxes
        m_gridSizeIncrementer = std::make_unique<SPIN_INCREMENTAL_TEXT_CTRL>(
                    *m_gridLineWidthSpinBtn, *m_gridLineWidth );

        m_gridSizeIncrementer->SetStep( gridThicknessMin, gridThicknessMax,
                                        gridThicknessStep );
        m_gridSizeIncrementer->SetPrecision( 0 );

        m_gridMinSpacingIncrementer = std::make_unique<SPIN_INCREMENTAL_TEXT_CTRL>(
                    *m_gridMinSpacingSpinBtn, *m_gridMinSpacing );

        m_gridMinSpacingIncrementer->SetStep( gridMinSpacingMin, gridMinSpacingMax,
                                              gridMinSpacingStep );
        m_gridMinSpacingIncrementer->SetPrecision( 0 ); // restrict to ints
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
                                        _( "Cursor shape:" ), wxDefaultPosition, wxDefaultSize,
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

    m_gridSizeIncrementer->SetValue( m_galOptions.m_gridLineWidth );

    m_gridMinSpacingIncrementer->SetValue( m_galOptions.m_gridMinSpacing );

    m_cursorShape->SetSelection( m_galOptions.m_fullscreenCursor );

    m_forceCursorDisplay->SetValue( m_galOptions.m_forceDisplayCursor );

    return true;
}


bool GAL_OPTIONS_PANEL::TransferDataFromWindow()
{
    m_galOptions.m_gridStyle = UTIL::GetValFromConfig(
            gridStyleSelectMap, m_gridStyle->GetSelection() );

    m_galOptions.m_gridLineWidth = std::floor( m_gridSizeIncrementer->GetValue() + 0.5 );

    m_galOptions.m_gridMinSpacing = m_gridMinSpacingIncrementer->GetValue();

    m_galOptions.m_fullscreenCursor = m_cursorShape->GetSelection();

    m_galOptions.m_forceDisplayCursor = m_forceCursorDisplay->GetValue();

    m_galOptions.NotifyChanged();

    return true;
}
