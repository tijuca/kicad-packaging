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

/**
 * @file drc_test_utils.h
 * General utilities for DRC-related PCB tests
 */

#ifndef QA_PCBNEW_DRC_TEST_UTILS__H
#define QA_PCBNEW_DRC_TEST_UTILS__H

#include <iostream>

#include <class_marker_pcb.h>

/**
 * Define a stream function for logging #MARKER_PCB test assertions.
 *
 * This has to be in the same namespace as #MARKER_PCB
 *
 * Note: this assumes there is not a operator<< for this type in the main
 * Pcbnew library. If one is introduced there, this one should be removed.
 *
 * TODO: convert to boost_test_print_type when Boost minver > 1.64. This
 * will keep testing logging and application-level operator<< implementations
 * separate, as they should be.
 */
std::ostream& operator<<( std::ostream& os, const MARKER_PCB& aMarker );


namespace KI_TEST
{
/**
 * Predicate for testing the type of a DRC marker
 * @param  aMarker      the marker to test
 * @param  aErrorCode   the expected DRC violation code
 * @return              true if the marker has this code
 */
bool IsDrcMarkerOfType( const MARKER_PCB& aMarker, int aErrorCode );

} // namespace KI_TEST

#endif // QA_PCBNEW_DRC_TEST_UTILS__H