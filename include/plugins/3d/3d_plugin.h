/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Cirilo Bernardo <cirilo.bernardo@gmail.com>
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
 * @file 3d_plugin.h
 * describes the runtime-loadable interface to support loading
 * and parsing of 3D models.
 */

#ifndef PLUGIN_3D_H
#define PLUGIN_3D_H

//
// KICAD_PLUGIN CLASS INTERFACE
//
// WARNING: DO NOT EDIT THIS FILE OUTSIDE THE KICAD TREE
//

// Note: the plugin class name must match the name expected by the loader
#define KICAD_PLUGIN_CLASS "PLUGIN_3D"
#define MAJOR 1
#define MINOR 0
#define REVISION 0
#define PATCH 0

#include "plugins/kicad_plugin.h"


KICAD_PLUGIN_EXPORT char const* GetKicadPluginClass( void )
{
    return KICAD_PLUGIN_CLASS;
}


KICAD_PLUGIN_EXPORT void GetClassVersion( unsigned char* Major,
    unsigned char* Minor, unsigned char* Patch, unsigned char* Revision )
{
    if( Major )
        *Major = MAJOR;

    if( Minor )
        *Minor = MINOR;

    if( Patch )
        *Patch = PATCH;

    if( Revision )
        *Revision = REVISION;

    return;
}

KICAD_PLUGIN_EXPORT bool CheckClassVersion( unsigned char Major,
    unsigned char Minor, unsigned char Patch, unsigned char Revision )
{
    if( Major != MAJOR )
        return false;

    // at the moment there are no incompatibility rules other than the Major Version check
    return true;
}


class SCENEGRAPH;

/**
 * Function GetNExtensions
 *
 * @return the number of extensions supported by the plugin
 */
KICAD_PLUGIN_EXPORT int GetNExtensions( void );

/**
 * Function GetModelExtension
 *
 * @param aIndex is the extension to return; valid values are
 * 0 to GetNExtensions() - 1.
 * @return the requested extension or a null string if aIndex
 * was invalid.
 */
KICAD_PLUGIN_EXPORT char const* GetModelExtension( int aIndex );

/**
 * Function GetNFilters
 * @returns the number of file filters
 */
KICAD_PLUGIN_EXPORT int GetNFilters( void );

/**
 * Function GetFileFilter
 *
 * @return the file filter string for the given index
 */
KICAD_PLUGIN_EXPORT char const* GetFileFilter( int aIndex );

/**
 * Function CanRender
 *
 * @return true if the plugin can render a model, that is
 * the Load() function is implemented
 */
KICAD_PLUGIN_EXPORT bool CanRender( void );

/**
 * reads a model file and creates a generic display structure
 *
 * @param aFileName is the full path of the model file
 * @return a SCENEGRAPH pointer to the display structure if the model
 * was successfully loaded and NULL if there is no rendering support
 * for the model or there were problems reading the model
 */
KICAD_PLUGIN_EXPORT SCENEGRAPH* Load( char const* aFileName );

#endif  // PLUGIN_3D_H
