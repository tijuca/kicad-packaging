/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2007 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 1992-2019 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file confirm.cpp
 * @brief utilities to display some error, warning and info short messges
 */

#include <wx/stockitem.h>
#include <wx/richmsgdlg.h>

#include <confirm.h>
#include <bitmaps.h>
#include <html_messagebox.h>
#include <dialog_exit_base.h>

#include <functional>
#include <unordered_map>

// Set of dialogs that have been chosen not to be shown again
static std::unordered_map<unsigned long, int> doNotShowAgainDlgs;


KIDIALOG::KIDIALOG( wxWindow* aParent, const wxString& aMessage,
        const wxString& aCaption, long aStyle )
    : wxRichMessageDialog( aParent, aMessage, aCaption, aStyle | wxCENTRE | wxSTAY_ON_TOP ),
      m_hash( 0 )
{
}


KIDIALOG::KIDIALOG( wxWindow* aParent, const wxString& aMessage,
        KD_TYPE aType, const wxString& aCaption )
    : wxRichMessageDialog( aParent, aMessage, getCaption( aType, aCaption ), getStyle( aType ) ),
      m_hash( 0 )
{
}


void KIDIALOG::DoNotShowCheckbox( wxString aUniqueId, int line )
{
    ShowCheckBox( _( "Do not show again" ), false );

    m_hash = std::hash<wxString>{}( aUniqueId ) + line;
}


bool KIDIALOG::DoNotShowAgain() const
{
    return doNotShowAgainDlgs.count( m_hash ) > 0;
}


void KIDIALOG::ForceShowAgain()
{
    doNotShowAgainDlgs.erase( m_hash );
}


bool KIDIALOG::Show( bool aShow )
{
    // We should check the do-not-show-again setting only when the dialog is displayed
    if( aShow )
    {
        // Check if this dialog should be shown to the user
        auto it = doNotShowAgainDlgs.find( m_hash );

        if( it != doNotShowAgainDlgs.end() )
            return it->second;
    }

    bool ret = wxRichMessageDialog::Show( aShow );

    // Has the user asked not to show the dialog again
    if( IsCheckBoxChecked() )
        doNotShowAgainDlgs[m_hash] = ret;

    return ret;
}


int KIDIALOG::ShowModal()
{
    // Check if this dialog should be shown to the user
    auto it = doNotShowAgainDlgs.find( m_hash );

    if( it != doNotShowAgainDlgs.end() )
        return it->second;

    int ret = wxRichMessageDialog::ShowModal();

    // Has the user asked not to show the dialog again
    if( IsCheckBoxChecked() )
        doNotShowAgainDlgs[m_hash] = ret;

    return ret;
}


wxString KIDIALOG::getCaption( KD_TYPE aType, const wxString& aCaption )
{
    if( !aCaption.IsEmpty() )
        return aCaption;

    switch( aType )
    {
        case KD_NONE:       /* fall through */
        case KD_INFO:       return _( "Message" );
        case KD_QUESTION:   return _( "Question" );
        case KD_WARNING:    return _( "Warning" );
        case KD_ERROR:      return _( "Error" );
    }

    return wxEmptyString;
}


long KIDIALOG::getStyle( KD_TYPE aType )
{
    long style = wxOK | wxCENTRE | wxSTAY_ON_TOP;

    switch( aType )
    {
        case KD_NONE:       break;
        case KD_INFO:       style |= wxICON_INFORMATION; break;
        case KD_QUESTION:   style |= wxICON_QUESTION; break;
        case KD_WARNING:    style |= wxICON_WARNING; break;
        case KD_ERROR:      style |= wxICON_ERROR; break;
    }

    return style;
}


class DIALOG_EXIT: public DIALOG_EXIT_BASE
{
public:
    DIALOG_EXIT( wxWindow *aParent, const wxString& aWarning, const wxString& aMessage,
                 const wxString& aOKLabel, const wxString& aCancelLabel ) :
        DIALOG_EXIT_BASE( aParent )
    {
        m_bitmap->SetBitmap( KiBitmap( dialog_warning_xpm ) );
        m_TextInfo->SetLabel( aWarning );
        m_staticTextWarningMessage->SetLabel( aMessage );

        m_sdbSizerOK->SetLabel( aOKLabel );
        m_sdbSizerCancel->SetLabel( aCancelLabel );
        m_sdbSizerOK->SetDefault();

        FinishDialogSettings();
    };

private:
    void OnSave( wxCommandEvent& event ) override { EndModal( wxID_YES ); }
    void OnDiscard( wxCommandEvent& event ) override { EndModal( wxID_NO ); }
};


int UnsavedChangesDialog( wxWindow* parent, const wxString& aMessage, bool* aApplyToAll )
{
    DIALOG_EXIT dlg( parent, aMessage,
                     _( "If you don't save, all your changes will be permanently lost." ),
                     _( "Save" ), _( "Cancel" ) );

    dlg.m_ApplyToAllOpt->Show( aApplyToAll != nullptr );

    int ret = dlg.ShowModal();

    if( aApplyToAll )
        *aApplyToAll = dlg.m_ApplyToAllOpt->GetValue();

    // Returns wxID_YES, wxID_NO, or wxID_CANCEL
    return ret;
}


bool ConfirmRevertDialog( wxWindow* parent, const wxString& aMessage )
{
    DIALOG_EXIT dlg( parent, aMessage,
                     _( "Your current changes will be permanently lost." ),
                     _( "Revert" ), _( "Cancel" ) );

    dlg.m_ApplyToAllOpt->Show( false );
    dlg.m_DiscardButton->Show( false );

    return dlg.ShowModal() == wxID_YES;
}


bool HandleUnsavedChanges( wxWindow* aParent, const wxString& aMessage,
                           const std::function<bool()>& aSaveFunction )
{
    switch( UnsavedChangesDialog( aParent, aMessage, nullptr ) )
    {
    case wxID_YES:    return aSaveFunction();
    case wxID_NO:     return true;
    default:
    case wxID_CANCEL: return false;
    }
}


int YesOrCancelDialog( wxWindow* aParent, const wxString& aWarning, const wxString& aMessage,
                       const wxString& aOKLabel, const wxString& aCancelLabel, bool* aApplyToAll )
{
    DIALOG_EXIT dlg( aParent, aWarning, aMessage, aOKLabel, aCancelLabel );

    dlg.m_ApplyToAllOpt->Show( aApplyToAll != nullptr );
    dlg.m_DiscardButton->Show( false );

    int ret = dlg.ShowModal();

    if( aApplyToAll )
        *aApplyToAll = dlg.m_ApplyToAllOpt->GetValue();

    // Returns wxID_YES, wxID_NO, or wxID_CANCEL
    return ret;
}


// DisplayError should be deprecated, use DisplayErrorMessage instead
void DisplayError( wxWindow* parent, const wxString& text, int displaytime )
{
    wxMessageDialog* dialog;

    int icon = displaytime > 0 ? wxICON_INFORMATION : wxICON_ERROR;

    dialog = new wxMessageDialog( parent, text, _( "Warning" ),
                                      wxOK | wxCENTRE | wxRESIZE_BORDER | icon | wxSTAY_ON_TOP );

    dialog->ShowModal();
    dialog->Destroy();
}


void DisplayErrorMessage( wxWindow* aParent, const wxString& aText, const wxString& aExtraInfo )
{
    wxRichMessageDialog* dlg;

    dlg = new wxRichMessageDialog( aParent, aText, _( "Error" ),
                                   wxOK | wxCENTRE | wxRESIZE_BORDER |
                                   wxICON_ERROR | wxSTAY_ON_TOP );

    if( !aExtraInfo.IsEmpty() )
    {
        dlg->ShowDetailedText( aExtraInfo );
    }

    dlg->ShowModal();
    dlg->Destroy();
}


void DisplayInfoMessage( wxWindow* aParent, const wxString& aMessage, const wxString& aExtraInfo )
{
    wxRichMessageDialog* dlg;

    dlg = new wxRichMessageDialog( aParent, aMessage, _( "Info" ),
                                   wxOK | wxCENTRE | wxRESIZE_BORDER |
                                   wxICON_INFORMATION | wxSTAY_ON_TOP );

    if( !aExtraInfo.IsEmpty() )
    {
        dlg->ShowDetailedText( aExtraInfo );
    }

    dlg->ShowModal();
    dlg->Destroy();
}


bool IsOK( wxWindow* aParent, const wxString& aMessage )
{
    wxMessageDialog dlg( aParent, aMessage, _( "Confirmation" ),
                         wxYES_NO | wxCENTRE | wxICON_QUESTION | wxSTAY_ON_TOP );
    dlg.SetEscapeId( wxID_NO );

    return dlg.ShowModal() == wxID_YES;
}


int SelectSingleOption( wxWindow* aParent, const wxString& aTitle, const wxString& aMessage, const wxArrayString& aOptions )
{
    wxSingleChoiceDialog dlg( aParent, aMessage, aTitle, aOptions );

    if( dlg.ShowModal() != wxID_OK )
        return -1;

    return dlg.GetSelection();
}

