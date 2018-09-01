///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 22 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_SYMBOL_REMAP_BASE_H__
#define __DIALOG_SYMBOL_REMAP_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class WX_HTML_REPORT_PANEL;

#include "dialog_shim.h"
#include <wx/html/htmlwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_SYMBOL_REMAP_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_SYMBOL_REMAP_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxHtmlWindow* m_htmlCtrl;
		wxButton* m_buttonRemap;
		wxButton* m_buttonClose;
		WX_HTML_REPORT_PANEL* m_messagePanel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRemapSymbols( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateUIRemapButton( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_SYMBOL_REMAP_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Remap Symbols"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_SYMBOL_REMAP_BASE();
	
};

#endif //__DIALOG_SYMBOL_REMAP_BASE_H__
