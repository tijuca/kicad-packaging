/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see AUTHORS.txt for contributors.
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
#include "utility_program.h"

#include <wx/msgout.h>


COMBINED_UTILITY::COMBINED_UTILITY( const UTIL_LIST& aSubUtils ) : m_subUtils( aSubUtils )
{
}


/**
 * Print the names and descriptions of the registered tools
 */
void COMBINED_UTILITY::showSubUtilityList( std::ostream& os ) const
{
    for( const auto& tool : m_subUtils )
    {
        os << tool->m_name << ": \t" << tool->m_desc << std::endl;
    }
}


/**
 * Get the utility program that matches a tool name
 * @param  aName the name to look for
 * @return       the tool function
 */
UTILITY_PROGRAM::FUNC* COMBINED_UTILITY::findSubUtility( const std::string& aName ) const
{
    for( const auto& tool : m_subUtils )
    {
        if( tool->m_name == aName )
            return &tool->m_func;
    }

    return nullptr;
}


void COMBINED_UTILITY::printUsage( char* name, std::ostream& os ) const
{
    os << "Run a utility tool." << std::endl;

    os << "Usage: " << name << " [-h] [-l] [TOOL [TOOL_OPTIONS]]" << std::endl;

    os << "  -h      show this message and exit." << std::endl
       << "  -l      print known tools and exit." << std::endl;

    os << std::endl;
    os << "Known tools: " << std::endl;

    showSubUtilityList( os );
}


int COMBINED_UTILITY::HandleCommandLine( int argc, char** argv ) const
{
    wxMessageOutput::Set( new wxMessageOutputStderr );

    // Need at least one parameter
    if( argc < 2 )
    {
        printUsage( argv[0], std::cerr );
        return RET_CODES::BAD_CMDLINE;
    }

    const std::string arg1( argv[1] );

    if( argc == 2 )
    {
        if( arg1 == "-h" )
        {
            printUsage( argv[0], std::cout );
            return RET_CODES::OK;
        }
        else if( arg1 == "-l" )
        {
            showSubUtilityList( std::cout );
            return RET_CODES::OK;
        }
    }

    auto func = findSubUtility( arg1 );

    if( !func )
    {
        std::cerr << "Tool " << arg1 << " not found." << std::endl;
        return RET_CODES::UNKNOWN_TOOL;
    }

    // pass on the rest of the commands
    return ( *func )( argc - 1, argv + 1 );
}