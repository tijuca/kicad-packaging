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

#ifndef BOM_PLUGINS_H
#define BOM_PLUGINS_H

#include <wx/arrstr.h>
#include <wx/file.h>
#include <wx/filename.h>

#include <memory>

/**
 * Class representing a Bill of Material output plugin.
 */
class BOM_PLUGIN
{
public:
    typedef std::unique_ptr<BOM_PLUGIN> PTR;

    /**
     * Constructor.
     * @param aFile is path to the plugin file.
     */
    BOM_PLUGIN( const wxString& aFile );

    /**
     * Returns true if a file name matches a recognized plugin format.
     * @param aFile is path to the plugin file.
     */
    static bool IsPlugin( const wxString& aFile );

    /**
     * Returns plugin description stored in the plugin header file (if available).
     */
    const wxString& GetInfo() const
    {
        return m_info;
    }

    /**
     * Returns the file name of the plugin.
     */
    const wxFileName& GetFile() const
    {
        return m_file;
    }

    /**
     * Returns the customisable plugin name.
     */
    const wxString& GetName() const
    {
        return m_name;
    }

    /**
     * Sets the customisable plugin name.
     * @param aName is the new name.
     */
    void SetName( const wxString& aName )
    {
        m_name = aName;
    }

    /**
     * Returns the command to execute the plugin.
     */
    const wxString& GetCommand() const
    {
        return m_cmd;
    }

    /**
     * Sets the command to execute the plugin.
     */
    void SetCommand( const wxString& aCommand )
    {
        m_cmd = aCommand;
    }

    /**
     * Accessor to array of options.
     */
    wxArrayString& Options()
    {
        return m_options;
    }

protected:
    /**
     * Reads the plugin file header.
     * @param aEndSection is a string marking end of the header.
     */
    wxString readHeader( const wxString& aEndSection );

    ///> Path to the plugin
    const wxFileName m_file;

    ///> User customisable name
    wxString m_name;

    ///> Command to execute the plugin
    wxString m_cmd;

    ///> Description of the plugin (normally from the plugin header)
    wxString m_info;

    ///> Plugin specific options
    wxArrayString m_options;
};

#endif /* BOM_PLUGINS_H */
