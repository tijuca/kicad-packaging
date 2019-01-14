/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see AUTHORS.txt for contributors.
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

/**
 * @file env_vars.h
 * Functions to provide helpful hints about what environment vars do
 */

#ifndef ENV_VARS_H
#define ENV_VARS_H

#include <wx/string.h>
#include <vector>

using ENV_VAR_LIST = std::vector<wxString>;

/**
 * Determine if an environment variable is "predefined", i.e. if the
 * name of the variable is special to KiCad, and isn't just a user-specified
 * substitution name.
 * @param  aEnvVar the variable to check
 * @return         true if predefined
 */
bool IsEnvVarImmutable( const wxString& aEnvVar );

/**
 * Get the list of pre-defined environment variables.
 */
const ENV_VAR_LIST& GetPredefinedEnvVars();

/**
 * Look up long-form help text for a given environment variable.
 *
 * This is intended for use in more verbose help resources (as opposed to
 * tooltip text)
 *
 * @param  aEnvVar The variable to look up
 * @return         A string with help for that variable. Empty if
 *                 no help available for this variable.
 */
wxString LookUpEnvVarHelp( const wxString& aEnvVar );

#endif /* ENV_VARS_H */
