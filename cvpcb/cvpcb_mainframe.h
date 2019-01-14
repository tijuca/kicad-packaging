/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file cvpcb_mainframe.h
 */

#ifndef _CVPCB_MAINFRAME_H_
#define _CVPCB_MAINFRAME_H_

#include <wx/listctrl.h>
#include <wx/filename.h>
#include <pcb_netlist.h>
#include <footprint_info.h>

#include <pcb_base_frame.h>
#include <config_params.h>
#include <auto_associate.h>
#include <memory>


/*  Forward declarations of all top-level window classes. */
class wxAuiToolBar;
class FOOTPRINTS_LISTBOX;
class COMPONENTS_LISTBOX;
class LIBRARY_LISTBOX;
class DISPLAY_FOOTPRINTS_FRAME;
class COMPONENT;
class FP_LIB_TABLE;

namespace CV { struct IFACE; }

/**
 * The CvPcb application main window.
 */
class CVPCB_MAINFRAME : public KIWAY_PLAYER
{
    friend struct CV::IFACE;

    wxString                  m_currentSearchPattern;
    NETLIST                   m_netlist;
    int                       m_filteringOptions;
    wxAuiToolBar*             m_mainToolBar;
    FOOTPRINTS_LISTBOX*       m_footprintListBox;
    LIBRARY_LISTBOX*          m_libListBox;
    COMPONENTS_LISTBOX*       m_compListBox;
    wxTextCtrl*               m_tcFilterString;
    wxStaticText*             m_statusLine1;
    wxStaticText*             m_statusLine2;
    wxButton*                 m_saveAndContinue;

public:
    wxArrayString             m_ModuleLibNames;
    wxArrayString             m_EquFilesNames;

    FOOTPRINT_LIST*           m_FootprintsList;

protected:
    bool            m_modified;
    bool            m_skipComponentSelect;      // true to skip OnSelectComponent event
                                                // (in automatic selection/deletion of associations)
    PARAM_CFG_ARRAY m_projectFileParams;

    bool            m_initialized;

    CVPCB_MAINFRAME( KIWAY* aKiway, wxWindow* aParent );

public:
    ~CVPCB_MAINFRAME();

    bool OpenProjectFiles( const std::vector<wxString>& aFileSet, int aCtl=0 ) override;

    void KiwayMailIn( KIWAY_EXPRESS& aEvent ) override;

    /**
     * @return a pointer on the Footprint Viewer frame, if exists, or NULL
     */
    DISPLAY_FOOTPRINTS_FRAME* GetFootprintViewerFrame();

    /**
     * Function OnSelectComponent
     * Called when clicking on a component in component list window
     * * Updates the filtered footprint list, if the filtered list option is selected
     * * Updates the current selected footprint in footprint list
     * * Updates the footprint shown in footprint display window (if opened)
     */
    void             OnSelectComponent( wxListEvent& event );

    /**
     * Function OnEditFootprintLibraryTable
     * displays the footprint library table editing dialog and updates the global and local
     * footprint tables accordingly.
     */
    void             OnEditFootprintLibraryTable( wxCommandEvent& event );

    void             OnCancel( wxCommandEvent& aEvent );
    void             OnOK( wxCommandEvent& aEvent );
    void             OnSaveAndContinue( wxCommandEvent& aEvent );
    void             OnQuit( wxCommandEvent& event );
    void             OnCloseWindow( wxCloseEvent& Event );
    void             OnSize( wxSizeEvent& SizeEvent );
    void             OnKeyDown( wxKeyEvent& aEvent );
    void             ReCreateHToolbar();
    virtual void     ReCreateMenuBar() override;
    void             ShowChangedLanguage() override;

    void             ChangeFocus( bool aMoveRight );

    void             ToFirstNA( wxCommandEvent& event );
    void             ToPreviousNA( wxCommandEvent& event );

    /**
     * Function DelAssociations
     * removes all component footprint associations already made
     */
    void             DelAssociations( wxCommandEvent& event );

    void             OnConfigurePaths( wxCommandEvent& aEvent );

    /**
     * Function OnEditEquFilesList
     * envokes the equ files list edit dialog.
     */
    void             OnEditEquFilesList( wxCommandEvent& aEvent );

    void             DisplayModule( wxCommandEvent& event );

    void             OnComponentRightClick( wxMouseEvent& event );

    void             OnFootprintRightClick( wxMouseEvent& event );

    /**
     * Called by the automatic association button
     * Read *.equ files to try to find corresponding footprint
     * for each component that is not already linked to a footprint ( a "free"
     * component )
     * format of a line:
     * 'cmp_ref' 'footprint_name'
     */
    void             AutomaticFootprintMatching( wxCommandEvent& event );

    /**
     * Function OnSelectFilteringFootprint
     * is the command event handler for enabling and disabling footprint filtering.
     */
    void             OnSelectFilteringFootprint( wxCommandEvent& event );

    /**
     * Function OnEnterFilteringText
     * Is called each time the text of m_tcFilterString is changed.
     */
    void             OnEnterFilteringText( wxCommandEvent& event );

    /**
     * Function SetNewPkg
     * set the footprint name for all selected components in component list
     * and selects the next component.
     * @param aFootprintName = the new footprint name
     */
    void             SetNewPkg( const wxString& aFootprintName );

    /**
     * Function SetNewPkg
     * Set the footprint name for the component of position aIndex in the component list
     *
     * @param aFootprintName = the new footprint name
     * @param aIndex = the index of the component to modify in the component list
     */
    void             SetNewPkg( const wxString& aFootprintName, int aIndex );

    void             BuildCmpListBox();
    void             BuildFOOTPRINTS_LISTBOX();
    void             BuildLIBRARY_LISTBOX();

    /**
     * Create or Update the frame showing the current highlighted footprint
     * and (if showed) the 3D display frame
     */
    void             CreateScreenCmp();

    /**
     * Function SaveFootprintAssociation
     * saves the edits that the user has done by sending them back to eeschema
     * via the kiway.
     * Optionally saves the schematic to disk as well.
     */
    bool SaveFootprintAssociation( bool doSaveSchematic );

    /**
     * Function ReadNetListAndFpFiles
     * loads the netlist file built on the fly by Eeschema and loads
     * footprint libraries from fp lib tables.
     * @param aNetlist is the netlist from eeschema in kicad s-expr format.
     * (see CVPCB_MAINFRAME::KiwayMailIn() to know how to get this netlist)
     */
    bool             ReadNetListAndFpFiles( const std::string& aNetlist );

    /**
     * Function ReadSchematicNetlist
     * read the netlist (.net) file built on the fly by Eeschema.
     * @param aNetlist is the netlist buffer filled by eeschema, in kicad s-expr format.
     * It is the same netlist as the .net file created by Eeschema.
     * (This method is called by ReadNetListAndFpFiles)
     */
    int              ReadSchematicNetlist( const std::string& aNetlist );

    /**
     * Function LoadProjectFile
     * reads the CvPcb configuration parameter from the project (.pro) file \a aFileName
     */
    void LoadProjectFile();

    /**
     * Function SaveProjectFile
     * Saves the CvPcb configuration parameter from the project (.pro) file \a aFileName
     */
    void SaveProjectFile();

    void LoadSettings( wxConfigBase* aCfg ) override;

    void SaveSettings( wxConfigBase* aCfg ) override;

    /**
     * Function DisplayStatus
     * updates the information displayed on the status bar at bottom of the main frame.
     *
     * When the library or component list controls have the focus, the footprint assignment
     * status of the components is displayed in the first status bar pane and the list of
     * filters for the selected component is displayed in the second status bar pane.  When
     * the footprint list control has the focus, the description of the selected footprint is
     * displayed in the first status bar pane and the key words for the selected footprint are
     * displayed in the second status bar pane.  The third status bar pane always displays the
     * current footprint list filtering.
     */
    void             DisplayStatus();

    /**
     * Function LoadFootprintFiles
     * reads the list of footprint (*.mod files) and generate the list of footprints.
     * for each module are stored
     *      the module name
     *      documentation string
     *      associated keywords
     * m_ModuleLibNames is the list of library that must be read (loaded)
     * fills m_footprints
     * @return true if libraries are found, false otherwise.
     */
    bool             LoadFootprintFiles();

    /**
     * Function GetProjectFileParameters
     * return project file parameter list for CvPcb.
     * <p>
     * Populate the project file parameter array specific to CvPcb if it hasn't
     * already been populated and return a reference to the array to the caller.
     * Creating the parameter list at run time has the advantage of being able
     * to define local variables.  The old method of statically building the array
     * at compile time requiring global variable definitions.
     * </p>
     *
     * @return A reference to a PARAM_CFG_ARRAY contain the project settings for CvPcb.
     */
    PARAM_CFG_ARRAY& GetProjectFileParameters( void );

    /**
     * Function SendMessageToEESCHEMA
     * Send a remote command to Eeschema via a socket,
     * Commands are
     * $PART: "reference"   put cursor on component anchor
     * @param aClearHighligntOnly = true if the message to send is only "clear highlight"
     * (used when exiting Cvpcb)
     */
    void SendMessageToEESCHEMA( bool aClearHighligntOnly = false );

    COMPONENT* GetSelectedComponent();

    /**
     * @return the LIB_ID of the selected footprint in footprint listview
     * or a empty string if no selection
     */
    wxString GetSelectedFootprint();

    void SetStatusText( const wxString& aText, int aNumber = 0 ) override;

private:
    // UI event handlers.
    // Keep consistent the display state of toggle menus or tools in toolbar
    void OnFilterFPbyKeywords( wxUpdateUIEvent& event );
    void OnFilterFPbyPinCount( wxUpdateUIEvent& event );
    void OnFilterFPbyLibrary( wxUpdateUIEvent& event );
    void OnFilterFPbyKeyName( wxUpdateUIEvent& event );

    /**
     * read the .equ files and populate the list of equvalents
     * @param aList the list to populate
     * @param aErrorMessages is a pointer to a wxString to store error messages
     *  (can be NULL)
     * @return the error count ( 0 = no error)
     */
    int buildEquivalenceList( FOOTPRINT_EQUIVALENCE_LIST& aList, wxString * aErrorMessages = NULL );

    void refreshAfterComponentSearch (COMPONENT* component);

    DECLARE_EVENT_TABLE()
};

#endif  //#ifndef _CVPCB_MAINFRAME_H_
