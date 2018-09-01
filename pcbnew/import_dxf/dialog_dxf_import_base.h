///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug  4 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_DXF_IMPORT_BASE_H__
#define __DIALOG_DXF_IMPORT_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class DIALOG_SHIM;
class PCB_LAYER_BOX_SELECTOR;

#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/valtext.h>
#include <wx/choice.h>
#include <wx/bmpcbox.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_ORIGIN_SELECT 1000

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_DXF_IMPORT_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_DXF_IMPORT_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxStaticText* m_staticTextFile;
		wxTextCtrl* m_textCtrlFileName;
		wxButton* m_buttonBrowse;
		wxRadioBox* m_rbOffsetOption;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticTextXpos;
		wxTextCtrl* m_DxfPcbXCoord;
		wxStaticText* m_staticTextYpos;
		wxTextCtrl* m_DxfPcbYCoord;
		wxStaticText* m_staticTextUnits;
		wxChoice* m_DxfPcbPositionUnits;
		wxStaticText* m_staticTextPrms;
		wxStaticText* m_staticTextLineWidth;
		wxTextCtrl* m_textCtrlLineWidth;
		wxChoice* m_choiceUnitLineWidth;
		wxStaticText* m_staticTextBrdlayer;
		PCB_LAYER_BOX_SELECTOR* m_SelLayerBox;
		wxStaticLine* m_staticline;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBrowseDxfFiles( wxCommandEvent& event ) { event.Skip(); }
		virtual void OriginOptionOnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void onUnitPositionSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUnitWidthSelection( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_DXF_IMPORT_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Import DXF File"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_DXF_IMPORT_BASE();
	
};

#endif //__DIALOG_DXF_IMPORT_BASE_H__
