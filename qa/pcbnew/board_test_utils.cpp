/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 KiCad Developers, see AUTHORS.txt for contributors.
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

#include "board_test_utils.h"

#include <pcbnew_utils/board_file_utils.h>

// For the temp directory logic: can be std::filesystem in C++17
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>


namespace KI_TEST
{

BOARD_DUMPER::BOARD_DUMPER() : m_dump_boards( std::getenv( "KICAD_TEST_DUMP_BOARD_FILES" ) )
{
}


void BOARD_DUMPER::DumpBoardToFile( BOARD& aBoard, const std::string& aName ) const
{
    if( !m_dump_boards )
        return;

    auto path = boost::filesystem::temp_directory_path() / aName;
    path += ".kicad_pcb";

    BOOST_TEST_MESSAGE( "Dumping board file: " << path.string() );
    ::KI_TEST::DumpBoardToFile( aBoard, path.string() );
}

} // namespace KI_TEST