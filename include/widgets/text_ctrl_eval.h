/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017-2018 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef TEXT_CTRL_EVAL_H
#define TEXT_CTRL_EVAL_H

#include <wx/window.h>
#include <wx/textctrl.h>
#include <libeval/numeric_evaluator.h>

/**
 * @brief wxTextCtrl wrapper to handle math expression evaluation.
 * Expressions are evaluated after the text control loses the focus. If user decides to modify
 * the expression, he will get the original expression to modify.
 */

class TEXT_CTRL_EVAL : public wxTextCtrl
{
public:
    TEXT_CTRL_EVAL( wxWindow* aParent, wxWindowID aId, const wxString& aValue = wxEmptyString,
            const wxPoint& aPos = wxDefaultPosition, const wxSize& aSize = wxDefaultSize,
            long aStyle = 0, const wxValidator& aValidator = wxDefaultValidator,
            const wxString& aName = wxTextCtrlNameStr );

    virtual ~TEXT_CTRL_EVAL()
    {
    }

protected:
    ///> Numeric expression evaluator
    NumericEvaluator m_eval;

    void onTextFocusGet( wxFocusEvent& aEvent );
    void onTextFocusLost( wxFocusEvent& aEvent );
    void onTextEnter( wxCommandEvent& aEvent );

    void evaluate();
};


#endif /* TEXT_CTRL_EVAL_H */
