///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_PUSH_PAD_PROPERTIES_BASE_H__
#define __DIALOG_PUSH_PAD_PROPERTIES_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_PUSH_PAD_PROPERTIES_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_PUSH_PAD_PROPERTIES_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxCheckBox* m_Pad_Shape_Filter_CB;
		wxCheckBox* m_Pad_Layer_Filter_CB;
		wxCheckBox* m_Pad_Orient_Filter_CB;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Apply;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void PadPropertiesAccept( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_PUSH_PAD_PROPERTIES_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Push Pad Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_PUSH_PAD_PROPERTIES_BASE();
	
};

#endif //__DIALOG_PUSH_PAD_PROPERTIES_BASE_H__
