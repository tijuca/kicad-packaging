///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_TARGET_PROPERTIES_BASE_H__
#define __DIALOG_TARGET_PROPERTIES_BASE_H__

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
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_TARGET_PROPERTIES_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_TARGET_PROPERTIES_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxStaticText* m_sizeLabel;
		wxTextCtrl* m_sizeCtrl;
		wxStaticText* m_sizeUnits;
		wxStaticText* m_thicknessLabel;
		wxTextCtrl* m_thicknessCtrl;
		wxStaticText* m_thicknessUnits;
		wxStaticText* m_staticTextShape;
		wxChoice* m_TargetShape;
		wxStaticLine* m_staticline;
		wxStdDialogButtonSizer* m_sdbSizerButts;
		wxButton* m_sdbSizerButtsOK;
		wxButton* m_sdbSizerButtsCancel;
	
	public:
		
		DIALOG_TARGET_PROPERTIES_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Target Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_TARGET_PROPERTIES_BASE();
	
};

#endif //__DIALOG_TARGET_PROPERTIES_BASE_H__
