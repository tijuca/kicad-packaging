///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_NETLIST_BASE_H__
#define __DIALOG_NETLIST_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class WX_HTML_REPORT_PANEL;

#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_NETLIST_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_NETLIST_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		enum
		{
			ID_TEST_NETLIST = 1000
		};
		
		wxTextCtrl* m_NetlistFilenameCtrl;
		wxBitmapButton* m_browseButton;
		wxRadioBox* m_matchByTimestamp;
		wxCheckBox* m_cbUpdateFootprints;
		wxCheckBox* m_cbDeleteShortingTracks;
		wxCheckBox* m_cbDeleteExtraFootprints;
		wxCheckBox* m_cbDeleteSinglePadNets;
		WX_HTML_REPORT_PANEL* m_MessageWindow;
		wxBoxSizer* m_buttonsSizer;
		wxButton* m_buttonFPTest;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Apply;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFilenameKillFocus( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnOpenNetlistClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMatchChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOptionChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTestFootprintsClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateUIValidNetlistFile( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCompileRatsnestClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdatePCB( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_NETLIST_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Netlist"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_NETLIST_BASE();
	
};

#endif //__DIALOG_NETLIST_BASE_H__
