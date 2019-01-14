/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2013 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2004-2013 KiCad Developers, see change_log.txt for contributors.
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
 * @file validators.cpp
 * @brief Custom text control validator implementations.
 */

#include <kicad_string.h>
#include <validators.h>

#include <wx/grid.h>
#include <wx/textctrl.h>
#include <wx/textentry.h>
#include <wx/log.h>


GRID_CELL_TEXT_EDITOR::GRID_CELL_TEXT_EDITOR() : wxGridCellTextEditor()
{
}


void GRID_CELL_TEXT_EDITOR::SetValidator( const wxValidator& validator )
{
    // keep our own copy because wxGridCellTextEditor's is annoyingly private
    m_validator.reset( static_cast<wxValidator*>( validator.Clone() ) );

    wxGridCellTextEditor::SetValidator( *m_validator );
}


void GRID_CELL_TEXT_EDITOR::StartingKey( wxKeyEvent& event )
{
    if( m_validator )
    {
        m_validator.get()->SetWindow( Text() );
        m_validator.get()->ProcessEvent( event );
    }

    if( event.GetSkipped() )
    {
        wxGridCellTextEditor::StartingKey( event );
        event.Skip( false );
    }
}


FILE_NAME_CHAR_VALIDATOR::FILE_NAME_CHAR_VALIDATOR( wxString* aValue ) :
    wxTextValidator( wxFILTER_EXCLUDE_CHAR_LIST, aValue )
{
    // The Windows (DOS) file system forbidden characters already include the forbidden
    // file name characters for both Posix and OSX systems.  The characters \/:*?|"<> are
    // illegal and filtered by the validator.
    wxString illegalChars = wxFileName::GetForbiddenChars( wxPATH_DOS );
    wxTextValidator nameValidator( wxFILTER_EXCLUDE_CHAR_LIST );
    wxArrayString illegalCharList;

    for( unsigned i = 0;  i < illegalChars.size();  i++ )
        illegalCharList.Add( wxString( illegalChars[i] ) );

    SetExcludes( illegalCharList );
 }


FILE_NAME_WITH_PATH_CHAR_VALIDATOR::FILE_NAME_WITH_PATH_CHAR_VALIDATOR( wxString* aValue ) :
    wxTextValidator( wxFILTER_EXCLUDE_CHAR_LIST | wxFILTER_EMPTY, aValue )
{
    // The Windows (DOS) file system forbidden characters already include the forbidden
    // file name characters for both Posix and OSX systems.  The characters *?|"<> are
    // illegal and filtered by the validator, but /\: are valid (\ and : only on Windows.
    wxString illegalChars = wxFileName::GetForbiddenChars( wxPATH_DOS );
    wxTextValidator nameValidator( wxFILTER_EXCLUDE_CHAR_LIST );
    wxArrayString illegalCharList;

    for( unsigned i = 0;  i < illegalChars.size();  i++ )
    {
        if( illegalChars[i] == '/' )
            continue;

#if defined (__WINDOWS__)
        if( illegalChars[i] == '\\' || illegalChars[i] == ':' )
            continue;
#endif
        illegalCharList.Add( wxString( illegalChars[i] ) );
    }

    SetExcludes( illegalCharList );
}


ENV_VAR_NAME_VALIDATOR::ENV_VAR_NAME_VALIDATOR( wxString* aValue ) :
    wxTextValidator()
{
    Connect( wxEVT_CHAR, wxKeyEventHandler( ENV_VAR_NAME_VALIDATOR::OnChar ) );
}


ENV_VAR_NAME_VALIDATOR::ENV_VAR_NAME_VALIDATOR( const ENV_VAR_NAME_VALIDATOR& val )
    : wxTextValidator()
{
    wxValidator::Copy( val );

    Connect( wxEVT_CHAR, wxKeyEventHandler( ENV_VAR_NAME_VALIDATOR::OnChar ) );
}


ENV_VAR_NAME_VALIDATOR::~ENV_VAR_NAME_VALIDATOR()
{
    Disconnect( wxEVT_CHAR, wxKeyEventHandler( ENV_VAR_NAME_VALIDATOR::OnChar ) );
}


void ENV_VAR_NAME_VALIDATOR::OnChar( wxKeyEvent& aEvent  )
{
    if (!m_validatorWindow)
    {
        aEvent.Skip();
        return;
    }

    int keyCode = aEvent.GetKeyCode();

    // we don't filter special keys and delete
    if (keyCode < WXK_SPACE || keyCode == WXK_DELETE || keyCode >= WXK_START)
    {
        aEvent.Skip();
        return;
    }

    wxUniChar c = (wxUChar) keyCode;

    if( c == wxT( '_' ) )
    {
        // OK anywhere
        aEvent.Skip();
    }
    else if( wxIsdigit( c ) )
    {
        // not as first character
        long from, to;
        GetTextEntry()->GetSelection( &from, &to );
        if( from < 1 )
            wxBell();
        else
            aEvent.Skip();
    }
    else if( wxIsalpha( c ) )
    {
        // Capitals only.

        if( wxIslower( c ) )
        {
            // You may wonder why this scope is so twisted, so make yourself comfortable and read:
            // 1. Changing the keyCode and/or uniChar in the event and passing it on
            // doesn't work.  Some platforms look at the original copy as long as the event
            // isn't vetoed.
            // 2. Inserting characters by hand does not move the cursor, meaning either you insert
            // text backwards (lp:#1798869) or always append, no matter where is the cursor.
            // wxTextEntry::{Get/Set}InsertionPoint() do not work at all here.
            // 3. There is wxTextEntry::ForceUpper(), but it is not yet available in common
            // wxWidgets packages.
            //
            // So here we are, with a command event handler that converts
            // the text to upper case upon every change.
            wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>( GetTextEntry() );

            if( textCtrl )
            {
                textCtrl->Connect( textCtrl->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
                        (wxObjectEventFunction) &ENV_VAR_NAME_VALIDATOR::OnTextChanged );
            }
        }

        aEvent.Skip();
    }
    else
    {
        wxBell();
    }
}


void ENV_VAR_NAME_VALIDATOR::OnTextChanged( wxCommandEvent& event )
{
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>( event.GetEventObject() );

    if( textCtrl )
    {
        if( !textCtrl->IsModified() )
            return;

        long insertionPoint = textCtrl->GetInsertionPoint();
        textCtrl->ChangeValue( textCtrl->GetValue().Upper() );
        textCtrl->SetInsertionPoint( insertionPoint );
        textCtrl->Disconnect( textCtrl->GetId(), wxEVT_COMMAND_TEXT_UPDATED );
    }

    event.Skip();
}
