///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 11 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_LIB_EDIT_TEXT_BASE_H__
#define __DIALOG_LIB_EDIT_TEXT_BASE_H__

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
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_LIB_EDIT_TEXT_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_LIB_EDIT_TEXT_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_TextValue;
		wxStaticText* m_PowerComponentValues;
		wxButton* m_TextValueSelectButton;
		wxCheckBox* m_visible;
		wxStaticText* m_xPosLabel;
		wxTextCtrl* m_xPosCtrl;
		wxStaticText* m_xPosUnits;
		wxCheckBox* m_italic;
		wxStaticText* m_orientLabel;
		wxChoice* m_orientChoice;
		wxStaticText* m_yPosLabel;
		wxTextCtrl* m_yPosCtrl;
		wxStaticText* m_yPosUnits;
		wxCheckBox* m_bold;
		wxStaticText* m_hAlignLabel;
		wxChoice* m_hAlignChoice;
		wxStaticText* m_textSizeLabel;
		wxTextCtrl* m_textSizeCtrl;
		wxStaticText* m_textSizeUnits;
		wxStaticText* m_vAlignLabel;
		wxChoice* m_vAlignChoice;
		wxCheckBox* m_CommonUnit;
		wxCheckBox* m_CommonConvert;
		wxStaticLine* m_staticline2;
		wxStdDialogButtonSizer* m_sdbSizerButtons;
		wxButton* m_sdbSizerButtonsOK;
		wxButton* m_sdbSizerButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseDialog( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnSetFocusText( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnTextValueSelectButtonClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_LIB_EDIT_TEXT_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Text Item Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_LIB_EDIT_TEXT_BASE();
	
};

#endif //__DIALOG_LIB_EDIT_TEXT_BASE_H__
