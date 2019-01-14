///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 17 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_PRINT_GENERIC_BASE_H__
#define __DIALOG_PRINT_GENERIC_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class DIALOG_SHIM;

#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_PRINT_GENERIC_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_PRINT_GENERIC_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		enum
		{
			wxID_FRAME_SEL = 1000,
			wxID_PRINT_OPTIONS
		};
		
		wxBoxSizer* bUpperSizer;
		wxStaticBoxSizer* sbOptionsSizer;
		wxGridBagSizer* gbOptionsSizer;
		wxStaticText* m_outputModeLabel;
		wxChoice* m_outputMode;
		wxCheckBox* m_titleBlock;
		wxRadioButton* m_scale1;
		wxRadioButton* m_scaleFit;
		wxRadioButton* m_scaleCustom;
		wxTextCtrl* m_scaleCustomText;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOption;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Apply;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onSetCustomScale( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPageSetup( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPrintPreview( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCloseButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPrintButtonClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_PRINT_GENERIC_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Print"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 410,476 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_PRINT_GENERIC_BASE();
	
};

#endif //__DIALOG_PRINT_GENERIC_BASE_H__
