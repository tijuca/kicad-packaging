///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_PRINT_USING_PRINTER_BASE_H__
#define __DIALOG_PRINT_USING_PRINTER_BASE_H__

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
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_PRINT_USING_PRINTER_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_PRINT_USING_PRINTER_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxCheckBox* m_checkReference;
		wxCheckBox* m_checkMonochrome;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonPageSetup;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Apply;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseWindow( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnPageSetup( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPrintPreview( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_PRINT_USING_PRINTER_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Print"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_PRINT_USING_PRINTER_BASE();
	
};

#endif //__DIALOG_PRINT_USING_PRINTER_BASE_H__
