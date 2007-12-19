/////////////////////////////////////////////////////////////////////////////
// Name:        annotate_dialog.h
// Purpose:     
// Author:      jean-pierre Charras
// Modified by: 
// Created:     05/02/2006 12:31:28
// RCS-ID:      
// Copyright:   License GNU
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 05/02/2006 12:31:28

#ifndef _ANNOTATE_DIALOG_H_
#define _ANNOTATE_DIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "annotate_dialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

#include "fctsys.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define ID_RADIOBOX 10001
#define ID_RADIOBOX1 10002
#define ID_RADIOBOX2 10005
#define ID_ANNOTATE_CMP 10003
#define ID_DEANNOTATE_CMP 10004
#define SYMBOL_WINEDA_ANNOTATEFRAME_STYLE wxDEFAULT_DIALOG_STYLE|MAYBE_RESIZE_BORDER
#define SYMBOL_WINEDA_ANNOTATEFRAME_TITLE _("EESchema Annotation")
#define SYMBOL_WINEDA_ANNOTATEFRAME_IDNAME ID_DIALOG
#define SYMBOL_WINEDA_ANNOTATEFRAME_SIZE wxSize(400, 300)
#define SYMBOL_WINEDA_ANNOTATEFRAME_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * WinEDA_AnnotateFrame class declaration
 */

class WinEDA_AnnotateFrame: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( WinEDA_AnnotateFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    WinEDA_AnnotateFrame( );
    WinEDA_AnnotateFrame( WinEDA_SchematicFrame* parent, wxWindowID id = SYMBOL_WINEDA_ANNOTATEFRAME_IDNAME, const wxString& caption = SYMBOL_WINEDA_ANNOTATEFRAME_TITLE, const wxPoint& pos = SYMBOL_WINEDA_ANNOTATEFRAME_POSITION, const wxSize& size = SYMBOL_WINEDA_ANNOTATEFRAME_SIZE, long style = SYMBOL_WINEDA_ANNOTATEFRAME_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WINEDA_ANNOTATEFRAME_IDNAME, const wxString& caption = SYMBOL_WINEDA_ANNOTATEFRAME_TITLE, const wxPoint& pos = SYMBOL_WINEDA_ANNOTATEFRAME_POSITION, const wxSize& size = SYMBOL_WINEDA_ANNOTATEFRAME_SIZE, long style = SYMBOL_WINEDA_ANNOTATEFRAME_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin WinEDA_AnnotateFrame event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ANNOTATE_CMP
    void OnAnnotateCmpClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DEANNOTATE_CMP
    void OnDeannotateCmpClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end WinEDA_AnnotateFrame event handler declarations

////@begin WinEDA_AnnotateFrame member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end WinEDA_AnnotateFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin WinEDA_AnnotateFrame member variables
    wxRadioBox* m_AnnotProjetCtrl;
    wxRadioBox* m_AnnotNewCmpCtrl;
    wxRadioBox* m_AnnotSortCmpCtrl;
////@end WinEDA_AnnotateFrame member variables

	WinEDA_SchematicFrame * m_Parent;
	bool m_Abort;

private:
	void AnnotateComponents(wxCommandEvent& event);
	void DeleteAnnotation(wxCommandEvent& event);
};

#endif
    // _ANNOTATE_DIALOG_H_
