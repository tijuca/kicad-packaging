/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 KiCad Developers, see CHANGELOG.TXT for contributors.
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

#ifndef WIDGETS_WIDGET_SAVE_RESTORE__H
#define WIDGETS_WIDGET_SAVE_RESTORE__H

#include <vector>

class wxCheckBox;
class wxChoice;
class wxNotebook;
class wxRadioBox;
class wxString;
class wxTextCtrl;

class UNIT_BINDER;

class WIDGET_SAVE_RESTORE
{
public:
    WIDGET_SAVE_RESTORE( bool& aValidFlag ) : m_valid( aValidFlag )
    {
    }

    /**
     * Bind a radiobox to a choice.
     */
    void Add( wxRadioBox& ctrl, long& dest );

    /**
     * Bind a check box to a binary choice
     */
    void Add( wxCheckBox& ctrl, bool& dest );

    /**
     * Bind a text ctrl to a string: the control value is stored directly
     * into the string
     */
    void Add( wxTextCtrl& ctrl, wxString& dest );

    /**
     * Bind a text ctrl to a integer: the control value is converted to and
     * from integer on save/restore.
     */
    void Add( wxTextCtrl& ctrl, long& dest );

    /**
     * Bind a text ctrl to a double: the control value is converted to and
     * from double on save/restore.
     */
    void Add( wxTextCtrl& ctrl, double& dest );

    /**
     * Bind a control managed by a #UNIT_BINDER into a integer
     */
    void Add( UNIT_BINDER& ctrl, long& dest );

    /**
     * Bind a choice control into a choice (agnostic to the actual
     * meaning of the choice)
     */
    void Add( wxChoice& ctrl, long& dest );

    /**
     * Bind a notebook tab choice to an integer
     */
    void Add( wxNotebook& ctrl, long& dest );

    /**
     * Read values of all bound controls into the internally-stored
     * references to the underlying data.
     */
    void ReadConfigFromControls();

    /**
     * Restore the values from the internally-stored references to the underlying
     * data to each bound control.
     */
    void RestoreConfigToControls();

private:
    /**
     * Recognised parameters types (encodes an implicit widget type,
     * data type and appropriate conversion).
     */
    enum class WIDGET_CTRL_TYPE_T
    {
        TEXT,
        TEXT_INTEGER,
        TEXT_DOUBLE,
        UNIT_BINDER,
        CHECKBOX,
        RADIOBOX,
        CHOICE,
        TAB
    };

    union CONTROL {
        CONTROL( wxCheckBox* aCtrl ) : m_checkbox( aCtrl )
        {
        }

        CONTROL( wxChoice* aCtrl ) : m_choice( aCtrl )
        {
        }

        CONTROL( wxNotebook* aCtrl ) : m_notebook( aCtrl )
        {
        }

        CONTROL( wxRadioBox* aCtrl ) : m_radiobox( aCtrl )
        {
        }

        CONTROL( wxTextCtrl* aCtrl ) : m_textctrl( aCtrl )
        {
        }

        CONTROL( UNIT_BINDER* aCtrl ) : m_unit_binder( aCtrl )
        {
        }

        wxCheckBox*  m_checkbox;
        wxChoice*    m_choice;
        wxNotebook*  m_notebook;
        wxRadioBox*  m_radiobox;
        wxTextCtrl*  m_textctrl;
        UNIT_BINDER* m_unit_binder;
    };

    union DATA {
        DATA( long* aDest ) : m_long( aDest )
        {
        }

        DATA( bool* aDest ) : m_bool( aDest )
        {
        }

        DATA( wxString* aDest ) : m_str( aDest )
        {
        }

        DATA( double* aDest ) : m_double( aDest )
        {
        }

        long*     m_long;
        bool*     m_bool;
        wxString* m_str;
        double*   m_double;
    };

    /**
     * Struct that represents a single bound control
     */
    struct WIDGET_CTRL_T
    {
        template <typename CTRL_T, typename DEST_T>
        WIDGET_CTRL_T( WIDGET_CTRL_TYPE_T aType, CTRL_T& aCtrl, DEST_T& aDest )
                : m_type( aType ), m_control( &aCtrl ), m_dest( &aDest )
        {
        }

        WIDGET_CTRL_TYPE_T m_type;
        CONTROL            m_control;
        DATA               m_dest;
    };

    std::vector<WIDGET_CTRL_T> m_ctrls;
    bool&                      m_valid;
};

#endif // WIDGETS_WIDGET_SAVE_RESTORE__H