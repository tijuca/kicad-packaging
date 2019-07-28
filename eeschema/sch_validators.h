/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 Wayne Stambaugh, stambaughw@gmail.com
 * Copyright (C) 2016-2019 KiCad Developers, see change_log.txt for contributors.
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
 * @file sch_validators.h
 * @brief Definitions of control validators for schematic dialogs.
 */

#ifndef _SCH_VALIDATORS_H_
#define _SCH_VALIDATORS_H_

#include <wx/valtext.h>


#define FIELD_NAME  -1


/**
 * A text control validator used for validating the text allowed in library and
 * schematic component fields.
 *
 * - The reference field does not accept spaces.
 * - The value field does not accept spaces in the symbol library editor because in symbol
 *   libraries, the value field is the symbol name in the library.
 */
class SCH_FIELD_VALIDATOR : public wxTextValidator
{
    int  m_fieldId;
    bool m_isLibEditor;

public:
    SCH_FIELD_VALIDATOR( bool aIsLibEditor, int aFieldId, wxString* aValue = NULL );

    SCH_FIELD_VALIDATOR( const SCH_FIELD_VALIDATOR& aValidator );

    virtual wxObject* Clone() const override { return new SCH_FIELD_VALIDATOR( *this ); }

    /**
     * Override the default Validate() function provided by wxTextValidator to provide
     * better error messages.
     *
     * @param aParent - a pointer to the parent window of the error message dialog.
     * @return true if the text in the control is valid otherwise false.
     */
    virtual bool Validate( wxWindow *aParent ) override;
};


#endif // _SCH_VALIDATORS_H_
