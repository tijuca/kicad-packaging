///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __PANEL_PCBNEW_DISPLAY_OPTIONS_BASE_H__
#define __PANEL_PCBNEW_DISPLAY_OPTIONS_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/radiobox.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class PANEL_PCBNEW_DISPLAY_OPTIONS_BASE
///////////////////////////////////////////////////////////////////////////////
class PANEL_PCBNEW_DISPLAY_OPTIONS_BASE : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			ID_SHOW_CLEARANCE = 1000
		};
		
		wxBoxSizer* m_galOptionsSizer;
		wxRadioBox* m_ShowNetNamesOption;
		wxCheckBox* m_OptDisplayPadNumber;
		wxCheckBox* m_OptDisplayPadNoConn;
		wxRadioBox* m_OptDisplayTracksClearance;
		wxCheckBox* m_OptDisplayPadClearence;
	
	public:
		
		PANEL_PCBNEW_DISPLAY_OPTIONS_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL ); 
		~PANEL_PCBNEW_DISPLAY_OPTIONS_BASE();
	
};

#endif //__PANEL_PCBNEW_DISPLAY_OPTIONS_BASE_H__
