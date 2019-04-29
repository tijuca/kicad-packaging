///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec  1 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_BOM_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_BOM_BASE : public DIALOG_SHIM
{
	private:

	protected:
		enum
		{
			IN_NAMELINE = 1000,
			ID_CMDLINE
		};

		wxStaticText* m_staticTextPluginTitle;
		wxListBox* m_lbPlugins;
		wxStaticText* m_staticTextName;
		wxTextCtrl* m_textCtrlName;
		wxTextCtrl* m_Messages;
		wxBitmapButton* m_buttonAddPlugin;
		wxBitmapButton* m_buttonEdit;
		wxBitmapButton* m_buttonDelPlugin;
		wxStaticText* m_staticTextCmd;
		wxTextCtrl* m_textCtrlCommand;
		wxCheckBox* m_checkBoxShowConsole;
		wxStaticLine* m_staticline2;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;
		wxButton* m_sdbSizerHelp;

		// Virtual event handlers, overide them in your derived class
		virtual void OnIdle( wxIdleEvent& event ) { event.Skip(); }
		virtual void OnPluginSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNameEdited( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddPlugin( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditPlugin( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemovePlugin( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCommandLineEdited( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowConsoleChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRunPlugin( wxCommandEvent& event ) { event.Skip(); }


	public:

		DIALOG_BOM_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Bill of Material"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 617,433 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DIALOG_BOM_BASE();

};

