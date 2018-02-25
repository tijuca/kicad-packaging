///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug  4 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_EDIT_FOOTPRINT_TEXT_BASE_H__
#define __DIALOG_EDIT_FOOTPRINT_TEXT_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class DIALOG_SHIM;
class PCB_LAYER_BOX_SELECTOR;
class TEXT_CTRL_EVAL;

#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/bmpcbox.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_EDIT_FPTEXT_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_EDIT_FPTEXT_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxStaticText* m_ModuleInfoText;
		wxStaticLine* m_staticline1;
		wxStaticText* m_TextDataTitle;
		wxTextCtrl* m_Name;
		wxStaticText* m_SizeXTitle;
		TEXT_CTRL_EVAL* m_TxtSizeCtrlX;
		wxStaticText* m_SizeYTitle;
		TEXT_CTRL_EVAL* m_TxtSizeCtrlY;
		wxStaticText* m_WidthTitle;
		TEXT_CTRL_EVAL* m_TxtWidthCtlr;
		wxStaticText* m_PosXTitle;
		TEXT_CTRL_EVAL* m_TxtPosCtrlX;
		wxStaticText* m_PosYTitle;
		TEXT_CTRL_EVAL* m_TxtPosCtrlY;
		wxStaticText* m_LayerLabel;
		PCB_LAYER_BOX_SELECTOR* m_LayerSelectionCtrl;
		wxRadioBox* m_Show;
		wxRadioBox* m_Style;
		wxRadioBox* m_Orient;
		wxStaticText* m_staticTextRotation;
		wxTextCtrl* m_OrientValueCtrl;
		wxCheckBox* m_unlock;
		wxStaticLine* m_staticline2;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnInitDlg( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void ModuleOrientEvent( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_EDIT_FPTEXT_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Footprint Text Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_EDIT_FPTEXT_BASE();
	
};

#endif //__DIALOG_EDIT_FOOTPRINT_TEXT_BASE_H__
