/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2017 Bernhard Stegmaier <stegmaier@sw-systems.de>
 * Copyright (C) 2016-2017 Kicad Developers, see change_log.txt for contributors.
 *
 * Base class for HiDPI aware wxGLCanvas implementations.
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

#ifndef HIDPI_GL_CANVAS_H
#define HIDPI_GL_CANVAS_H

#include <wx/glcanvas.h>


/**
 * @brief wxGLCanvas wrapper for HiDPI/Retina support.
 *
 * This is a small wrapper class to enable HiDPI/Retina support for wxGLCanvas.
 * HiDPI currently only works with a patched wxWidgets version, see:
 *   http://trac.wxwidgets.org/ticket/15700
 */
class HIDPI_GL_CANVAS : public wxGLCanvas
{
public:
    // wxGLCanvas constructor
    HIDPI_GL_CANVAS( wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const int *attribList = NULL,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette );


    // wxGLCanvas override
    virtual wxSize GetClientSize() const;

    // wxGLCanvas override (with patch applied) or default value of 1.0
    virtual float GetBackingScaleFactor() const;
};

#endif // HIDPI_GL_CANVAS_H
