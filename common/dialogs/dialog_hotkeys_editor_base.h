///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_HOTKEYS_EDITOR_BASE_H__
#define __DIALOG_HOTKEYS_EDITOR_BASE_H__

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
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class HOTKEYS_EDITOR_DIALOG_BASE
///////////////////////////////////////////////////////////////////////////////
class HOTKEYS_EDITOR_DIALOG_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxBoxSizer* m_mainSizer;
		wxStaticText* m_staticText1;
		wxPanel* m_panelHotkeys;
		wxButton* m_resetButton;
		wxButton* m_defaultButton;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void ResetClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void DefaultsClicked( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		HOTKEYS_EDITOR_DIALOG_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Hotkeys Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 513,445 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~HOTKEYS_EDITOR_DIALOG_BASE();
	
};

#endif //__DIALOG_HOTKEYS_EDITOR_BASE_H__
