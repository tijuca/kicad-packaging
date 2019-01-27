/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see change_log.txt for contributors.
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


#ifndef DRC_COURTYARD_OVERLAP__H
#define DRC_COURTYARD_OVERLAP__H

#include <class_board.h>

#include <drc/drc_provider.h>

/**
 * A class that provides the courtyard-based DRC checks.
 */
class DRC_COURTYARD_OVERLAP : public DRC_PROVIDER
{
public:
    DRC_COURTYARD_OVERLAP(
            const DRC_MARKER_FACTORY& aMarkerFactory, MARKER_HANDLER aMarkerHandler );

    bool RunDRC( BOARD& aBoard ) const override;
};

#endif // DRC_COURTYARD_OVERLAP__H