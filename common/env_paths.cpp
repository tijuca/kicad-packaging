/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2017 KiCad Developers, see AUTHORS.txt for contributors.
 * Copyright (C) 2017 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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

#include <env_paths.h>

static bool normalizeAbsolutePaths( const wxFileName& aPathA,
                                        const wxFileName& aPathB,
                                        wxString*         aResultPath )
{
    wxCHECK_MSG( aPathA.IsAbsolute(), false, aPathA.GetPath() + " is not an absolute path." );
    wxCHECK_MSG( aPathB.IsAbsolute(), false, aPathB.GetPath() + " is not an absolute path." );

    if( aPathA.GetPath() == aPathB.GetPath() )
        return true;

    if( ( aPathA.GetDirCount() > aPathB.GetDirCount() )
      || ( aPathA.HasVolume() && !aPathB.HasVolume() )
      || ( !aPathA.HasVolume() && aPathB.HasVolume() )
      || ( ( aPathA.HasVolume() && aPathB.HasVolume() )
         && ( aPathA.GetVolume() == aPathB.GetVolume() ) ) )
        return false;

    wxArrayString aDirs = aPathA.GetDirs();
    wxArrayString bDirs = aPathB.GetDirs();

    size_t i = 0;

    while( i < aDirs.GetCount() )
    {
        if( aDirs[i] != bDirs[i] )
            return false;

        i++;
    }

    if( aResultPath )
    {
        while( i < bDirs.GetCount() )
        {
            *aResultPath += bDirs[i] + wxT( "/" );
            i++;
        }
    }

    return true;
}


wxString NormalizePath( const wxFileName& aFilePath, const ENV_VAR_MAP* aEnvVars,
        const PROJECT* aProject )
{
    wxFileName envPath;
    wxString tmp, varName, normalizedFullPath;

    if( aEnvVars )
    {
        for( auto& entry : *aEnvVars )
        {
            // Don't bother normalizing paths that don't exist or the user cannot read.
            if( !wxFileName::DirExists( entry.second.GetValue() )
                || !wxFileName::IsDirReadable( entry.second.GetValue() ) )
                continue;

            envPath.SetPath( entry.second.GetValue() );

            if( normalizeAbsolutePaths( envPath, aFilePath, &tmp ) )
            {
                varName = entry.first;
                break;
            }
        }
    }

    if( varName.IsEmpty() && aProject )
    {
        envPath.SetPath( aProject->GetProjectPath() );

        if( normalizeAbsolutePaths( envPath, aFilePath, &tmp ) )
            varName = PROJECT_VAR_NAME;
    }

    if( !varName.IsEmpty() )
    {
        normalizedFullPath = wxString::Format( "${%s}/", varName );

        if( !tmp.IsEmpty() )
            normalizedFullPath += tmp;

        normalizedFullPath += aFilePath.GetFullName();
    }

    return normalizedFullPath;
}


// Create file path by appending path and file name. This approach allows the filename
// to contain a relative path, whereas wxFileName::SetPath() would replace the
// relative path
static wxString createFilePath( const wxString& aPath, const wxString& aFileName )
{
    wxString path( aPath );

    if( !path.EndsWith( wxFileName::GetPathSeparator() ) )
        path.Append( wxFileName::GetPathSeparator() );

    return path + aFileName;
}


wxString ResolveFile( const wxString& aFileName, const ENV_VAR_MAP* aEnvVars,
        const PROJECT* aProject )
{
    if( aProject )
    {
        wxFileName fn( createFilePath( aProject->GetProjectPath(), aFileName ) );

        if( fn.Exists() )
            return fn.GetFullPath();
    }

    if( aEnvVars )
    {
        for( auto& entry : *aEnvVars )
        {
            wxFileName fn( createFilePath( entry.second.GetValue(), aFileName ) );

            if( fn.Exists() )
                return fn.GetFullPath();
        }
    }

    return wxEmptyString;
}
