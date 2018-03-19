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


/**
 * A KICAD version of wxTextEntryDialog which supports the various improvments/work-arounds
 * from DIALOG_SHIM.
 */


#ifndef _DIALOG_TEXT_ENTRY_H_
#define _DIALOG_TEXT_ENTRY_H_

#include <dialog_text_entry_base.h>


class wxTextValidator;


class WX_TEXT_ENTRY_DIALOG : public WX_TEXT_ENTRY_DIALOG_BASE
{
public:
    WX_TEXT_ENTRY_DIALOG( wxWindow* aParent, const wxString& aLabel, const wxString& aCaption,
                          const wxString& aDefaultValue = wxEmptyString );

    void SetTextValidator( wxTextValidatorStyle style );
    void SetTextValidator( const wxTextValidator& validator );

    wxString GetValue();
};

#endif      // _DIALOG_TEXT_ENTRY_H_
