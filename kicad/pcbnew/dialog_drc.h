/////////////////////////////////////////////////////////////////////////////
// Name:        dialog_drc.h
// Purpose:
// Author:      jean-pierre Charras
// Modified by:
// Created:     27/02/2006 20:42:00
// RCS-ID:
// Copyright:   License GNU
// Licence:
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 27/02/2006 20:42:00

#ifndef _DIALOG_DRC_H_
#define _DIALOG_DRC_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "dialog_drc.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
////@end includes


/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxNotebook;
class DRCLISTBOX;
class wxStdDialogButtonSizer;
////@end forward declarations


/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define ID_TEXTCTRL1 10002
#define ID_CHECKBOX 10004
#define ID_TEXTCTRL3 10014
#define ID_BUTTON_BROWSE_RPT_FILE 10018
#define ID_CHECKBOX2 10019
#define ID_CHECKBOX7 10021
#define ID_CHECKBOX3 10011
#define ID_STARTDRC 10006
#define ID_LIST_UNCONNECTED 10003
#define ID_DELETE_ALL 10005
#define ID_DELETE_ONE 10007
#define ID_NOTEBOOK1 10008
#define ID_CLEARANCE_LIST 10001
#define ID_UNCONNECTED_LIST 10009
#define SYMBOL_DRCDIALOG_STYLE wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX
#define SYMBOL_DRCDIALOG_TITLE _("DRC Control")
#define SYMBOL_DRCDIALOG_IDNAME ID_DIALOG
#define SYMBOL_DRCDIALOG_SIZE wxSize(450, 300)
#define SYMBOL_DRCDIALOG_POSITION wxDefaultPosition
////@end control identifiers

#define ID_DRCLISTCTRL 14000   // outside @end control identifiers since DialogBlocks knows not DRCLISTBOX 

#define ID_POPUP_UNCONNECTED_A  14001
#define ID_POPUP_UNCONNECTED_B  14002
#define ID_POPUP_MARKERS_A      14003
#define ID_POPUP_MARKERS_B      14004


/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * DrcDialog class declaration
 */

class DrcDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( DrcDialog )
    DECLARE_EVENT_TABLE()

    /**
     * Function writeReport
     * outputs the MARKER items and unconnecte DRC_ITEMs with commentary to an 
     * open text file.
     * @param fpOut The text file to write the report to.
     */
    void writeReport( FILE* fpOut );
    
    
    
public:
    /// Constructors
    DrcDialog( );
    DrcDialog( DRC* aTester, WinEDA_PcbFrame* parent,
                     wxWindowID id = SYMBOL_DRCDIALOG_IDNAME,
                     const wxString& caption = SYMBOL_DRCDIALOG_TITLE,
                     const wxPoint& pos = SYMBOL_DRCDIALOG_POSITION,
                     const wxSize& size = SYMBOL_DRCDIALOG_SIZE,
                     long style = SYMBOL_DRCDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DRCDIALOG_IDNAME, const wxString& caption = SYMBOL_DRCDIALOG_TITLE, const wxPoint& pos = SYMBOL_DRCDIALOG_POSITION, const wxSize& size = SYMBOL_DRCDIALOG_SIZE, long style = SYMBOL_DRCDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin DrcDialog event handler declarations

    /// wxEVT_INIT_DIALOG event handler for ID_DIALOG
    void OnInitDialog( wxInitDialogEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX
    void OnReportCheckBoxClicked( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BROWSE_RPT_FILE
    void OnButtonBrowseRptFileClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_STARTDRC
    void OnStartdrcClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LIST_UNCONNECTED
    void OnListUnconnectedClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DELETE_ALL
    void OnDeleteAllClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DELETE_ONE
    void OnDeleteOneClick( wxCommandEvent& event );

    /// wxEVT_LEFT_DCLICK event handler for ID_CLEARANCE_LIST
    void OnLeftDClickClearance( wxMouseEvent& event );

    /// wxEVT_RIGHT_UP event handler for ID_CLEARANCE_LIST
    void OnRightUpClearance( wxMouseEvent& event );

    /// wxEVT_LEFT_DCLICK event handler for ID_UNCONNECTED_LIST
    void OnLeftDClickUnconnected( wxMouseEvent& event );

    /// wxEVT_RIGHT_UP event handler for ID_UNCONNECTED_LIST
    void OnRightUpUnconnected( wxMouseEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end DrcDialog event handler declarations

////@begin DrcDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DrcDialog member function declarations

    void OnMarkerSelectionEvent( wxCommandEvent& event );
    void OnUnconnectedSelectionEvent( wxCommandEvent& event );

    /// Should we show tooltips?
    static bool ShowToolTips();

    void DelDRCMarkers();
    void RedrawDrawPanel();

    void OnPopupMenu( wxCommandEvent& event );
    
    
////@begin DrcDialog member variables
    wxBoxSizer* m_MainSizer;
    wxBoxSizer* m_CommandSizer;
    wxStaticText* m_ClearenceTitle;
    wxTextCtrl* m_SetClearance;
    wxCheckBox* m_CreateRptCtrl;
    wxTextCtrl* m_RptFilenameCtrl;
    wxButton* m_BrowseButton;
    wxCheckBox* m_Pad2PadTestCtrl;
    wxCheckBox* m_ZonesTestCtrl;
    wxCheckBox* m_UnconnectedTestCtrl;
    wxButton* m_DeleteAllButton;
    wxButton* m_DeleteCurrentMarkerButton;
    wxNotebook* m_Notebook;
    DRCLISTBOX* m_ClearanceListBox;
    DRCLISTBOX* m_UnconnectedListBox;
    wxStdDialogButtonSizer* StdDialogButtonSizer;
////@end DrcDialog member variables



    DRC*                m_tester;
    WinEDA_PcbFrame*    m_Parent;
    int                 m_UnconnectedCount;
};

#endif  // _DIALOG_DRC_H_

