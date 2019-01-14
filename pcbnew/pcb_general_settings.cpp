/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2012-2017 Kicad Developers, see AUTHORS.txt for contributors.
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

#include <pcb_general_settings.h>
#include <wx/tokenzr.h>

PCB_GENERAL_SETTINGS::PCB_GENERAL_SETTINGS( FRAME_T aFrameType )
    : m_frameType( aFrameType ), m_colorsSettings( aFrameType )
{
    switch( m_frameType )
    {
    case FRAME_PCB:
        Add( "LegacyAutoDeleteOldTrack", &m_legacyAutoDeleteOldTrack, true );
        Add( "LegacyUse45DegreeTracks",&m_legacyUse45DegreeTracks, true);
        Add( "LegacyUseTwoSegmentTracks", &m_legacyUseTwoSegmentTracks, true);
        Add( "Use45DegreeGraphicSegments", &m_use45DegreeGraphicSegments, false);
        Add( "MagneticPads", reinterpret_cast<int*>( &m_magneticPads ), CAPTURE_CURSOR_IN_TRACK_TOOL );
        Add( "MagneticTracks", reinterpret_cast<int*>( &m_magneticTracks ), CAPTURE_CURSOR_IN_TRACK_TOOL );
        Add( "EditActionChangesTrackWidth", &m_editActionChangesTrackWidth, false );
        Add( "DragSelects", &m_dragSelects, true );
        break;

    case FRAME_PCB_MODULE_EDITOR:
        Add( "Use45DegreeGraphicSegments", &m_use45DegreeGraphicSegments, false);
        Add( "MagneticPads", reinterpret_cast<int*>( &m_magneticPads ), CAPTURE_CURSOR_IN_TRACK_TOOL );
        Add( "DragSelects", &m_dragSelects, true );
        break;

    default:
        break;
    }
}


void PCB_GENERAL_SETTINGS::Load( wxConfigBase* aCfg )
{
    m_colorsSettings.Load( aCfg );

#if defined(KICAD_SCRIPTING) && defined(KICAD_SCRIPTING_ACTION_MENU)

    m_pluginSettings.clear();

    wxString pluginSettings = aCfg->Read( "ActionPluginButtons" );

    wxStringTokenizer pluginSettingsTokenizer = wxStringTokenizer( pluginSettings, ";" );

    while( pluginSettingsTokenizer.HasMoreTokens() )
    {
        wxString plugin = pluginSettingsTokenizer.GetNextToken();
        wxStringTokenizer pluginTokenizer = wxStringTokenizer( plugin, "=" );

        if( pluginTokenizer.CountTokens() != 2 )
        {
            // Bad config
            continue;
        }

        plugin = pluginTokenizer.GetNextToken();
        m_pluginSettings.push_back( std::make_pair( plugin, pluginTokenizer.GetNextToken() ) );
    }

#endif

    SETTINGS::Load( aCfg );
}


void PCB_GENERAL_SETTINGS::Save( wxConfigBase* aCfg )
{
    m_colorsSettings.Save( aCfg );

#if defined(KICAD_SCRIPTING) && defined(KICAD_SCRIPTING_ACTION_MENU)

    wxString pluginSettings;

    for( auto const& entry : m_pluginSettings )
    {
        if( !pluginSettings.IsEmpty() )
        {
            pluginSettings = pluginSettings + wxT( ";" );
        }
        pluginSettings = pluginSettings + entry.first + wxT( "=" ) + entry.second;
    }

    aCfg->Write( "ActionPluginButtons" , pluginSettings );

#endif

    SETTINGS::Save( aCfg );
}


bool PCB_GENERAL_SETTINGS::m_use45DegreeGraphicSegments = false;
bool PCB_GENERAL_SETTINGS::m_dragSelects = true;
