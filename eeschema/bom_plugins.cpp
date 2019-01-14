/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * https://www.gnu.org/licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "bom_plugins.h"

BOM_PLUGIN::BOM_PLUGIN( const wxString& aFile )
    : m_file( aFile )
{
    if( !wxFile::Exists( aFile ) )
        throw std::runtime_error( ( _( "Cannot open plugin " ) + aFile ).ToStdString() );

    m_name = m_file.GetName();
    wxString extension = m_file.GetExt().Lower();

    // Important note:
    // On Windows the right command command to run a python script is:
    // python <script_path>/script.py
    // and *not* python <script_path>\script.py
    // Otherwise the script does not find some auxiliary pythons scripts needed by this script
    if( extension == "xsl" )
    {
        m_info = readHeader( "-->" );
        m_cmd = wxString::Format( "xsltproc -o \"%%O\" \"%s\" \"%%I\"", m_file.GetFullPath() );
    }
    else if( extension == "py" )
    {
        m_info = readHeader( "\"\"\"" );
#ifdef __WINDOWS__
        m_cmd = wxString::Format( "python \"%s/%s\" \"%%I\" \"%%O\"",
                                  m_file.GetPath(), m_file.GetFullName() );
#else
        m_cmd = wxString::Format( "python \"%s\" \"%%I\" \"%%O\"", m_file.GetFullPath() );
#endif
    }
#ifdef __WINDOWS__
    else if( extension == "pyw" )
    {
        m_info = readHeader( "\"\"\"" );
        m_cmd = wxString::Format( "pythonw \"%s/%s\" \"%%I\" \"%%O\"",
                                  m_file.GetPath(),m_file.GetFullName() );
    }
#endif /* __WINDOWS__ */
    else // fallback
    {
        m_cmd = m_file.GetFullPath();
    }
}


bool BOM_PLUGIN::IsPlugin( const wxString& aFile )
{
    wxFileName fn( aFile );
    wxString ext = fn.GetExt().Lower();

    for( const auto& pluginExt : { "xsl", "py", "pyw" } )
    {
        if( pluginExt == ext )
            return true;
    }

    return false;
}


wxString BOM_PLUGIN::readHeader( const wxString& aEndSection )
{
    if( aEndSection.IsEmpty() )
        return wxEmptyString;

    wxFile fdata( m_file.GetFullPath() );        // dtor will close the file
    wxString data;

    if( !fdata.ReadAll( &data ) )
        return wxEmptyString;

    const wxString header( "@package" );

    // Extract substring between @package and endsection
    int strstart = data.Find( header );

    if( strstart == wxNOT_FOUND )
        return wxEmptyString;

    strstart += header.Length();
    int strend = data.find( aEndSection, strstart );

    if( strend == wxNOT_FOUND)
        return wxEmptyString;

    // Remove empty line if any
    while( data[strstart] < ' ' )
            strstart++;

    return data.SubString( strstart, strend - 1 );
}
