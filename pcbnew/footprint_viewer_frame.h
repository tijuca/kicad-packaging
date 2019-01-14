/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras, jap.charras at wanadoo.fr
 * Copyright (C) 2004-2018 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file footprint_viewer_frame.h
 */

#ifndef FOOTPRINT_VIEWER_FRAME_H
#define FOOTPRINT_VIEWER_FRAME_H


#include <wx/gdicmn.h>
#include <pcb_base_frame.h>

class wxSashLayoutWindow;
class wxListBox;
class FP_LIB_TABLE;
class BOARD_ITEM;

namespace PCB { struct IFACE; }

/**
 * Component library viewer main window.
 */
class FOOTPRINT_VIEWER_FRAME : public PCB_BASE_FRAME
{
    friend struct PCB::IFACE;       // constructor called from here only

protected:
    FOOTPRINT_VIEWER_FRAME( KIWAY* aKiway, wxWindow* aParent, FRAME_T aFrameType );


public:
    ~FOOTPRINT_VIEWER_FRAME();

    PCB_GENERAL_SETTINGS& GetConfigSettings() { return m_configSettings; }

    /// Updates the GAL with display settings changes
    void ApplyDisplaySettingsToGAL();

    virtual COLOR4D GetGridColor() override;

    bool GetAutoZoom() const { return m_autoZoom; }
    void SetAutoZoom( bool aEnable ) { m_autoZoom = aEnable; }

    /**
     * redraws the message panel.
     * display the current footprint info, or
     * clear the message panel if nothing is loaded
     */
    void UpdateMsgPanel() override;

    /**
     * Function ReCreateLibraryList
     *
     * Creates or recreates the list of current loaded libraries.
     * This list is sorted, with the library cache always at end of the list
     */
    void ReCreateLibraryList();

    /**
     * Function ShowModal
     *
     * Runs the Footprint Viewer as a modal dialog.
     * @param aFootprint an optional FPID string to initialize the viewer with and to
     *                   return a selected footprint through.
     */
    bool ShowModal( wxString* aFootprint, wxWindow* aParent ) override;

private:

    wxListBox*          m_libList;               // The list of libs names
    wxListBox*          m_footprintList;         // The list of footprint names

    bool                m_autoZoom;
    double              m_lastZoom;

    const wxString      getCurNickname();
    void                setCurNickname( const wxString& aNickname );

    const wxString      getCurFootprintName();
    void                setCurFootprintName( const wxString& aName );

    void OnSize( wxSizeEvent& event ) override;

    void ReCreateFootprintList();
    void OnIterateFootprintList( wxCommandEvent& event );

    /**
     * Function UpdateTitle
     * updates the window title with current library information.
     */
    void UpdateTitle();

    /**
     * Function RedrawActiveWindow
     * Display the current selected component.
     * If the component is an alias, the ROOT component is displayed
     */
    void RedrawActiveWindow( wxDC* DC, bool EraseBg ) override;

    void OnCloseWindow( wxCloseEvent& Event ) override;
    void CloseFootprintViewer( wxCommandEvent& event );

    void ReCreateHToolbar() override;
    void ReCreateVToolbar() override;
    void ReCreateMenuBar() override;

    void OnLeftClick( wxDC* DC, const wxPoint& MousePos ) override;
    void ClickOnLibList( wxCommandEvent& event );
    void ClickOnFootprintList( wxCommandEvent& event );
    void DClickOnFootprintList( wxCommandEvent& event );
    void OnSetRelativeOffset( wxCommandEvent& event );

    void InstallDisplayOptions( wxCommandEvent& event );

    bool GeneralControl( wxDC* aDC, const wxPoint& aPosition, EDA_KEY aHotKey = 0 ) override;

    ///> @copydoc EDA_DRAW_FRAME::GetHotKeyDescription()
    EDA_HOTKEY* GetHotKeyDescription( int aCommand ) const override;

    /**
     * Function OnHotKey
     * handle hot key events.
     * <p?
     * Some commands are relative to the item under the mouse cursor.  Commands are
     * case insensitive
     * </p>
     */
    bool OnHotKey( wxDC* aDC, int aHotKey, const wxPoint& aPosition, EDA_ITEM* aItem = NULL ) override;

    void LoadSettings( wxConfigBase* aCfg ) override;
    void SaveSettings( wxConfigBase* aCfg ) override;

    /**
     * Function OnActivate
     * is called when the frame frame is activate to reload the libraries and component lists
     * that can be changed by the schematic editor or the library editor.
     */
    virtual void OnActivate( wxActivateEvent& event ) override;

    /**
     * Function SelectCurrentFootprint
     * Selects the current footprint name and display it
     */
    void SelectCurrentFootprint( wxCommandEvent& event );

    /**
     * Function ExportSelectedFootprint
     * exports the current footprint name and close the library browser.
     */
    void ExportSelectedFootprint( wxCommandEvent& event );

    /**
     * Function SelectAndViewFootprint
     * Select and load the next or the previous footprint
     * if no current footprint, Rebuild the list of footprints available in a given footprint
     * library
     * @param aMode = NEXT_PART or PREVIOUS_PART
     */
    void SelectAndViewFootprint( int aMode );

    bool OnRightClick( const wxPoint& MousePos, wxMenu* PopMenu ) override;

    /**
     * Function Show3D_Frame (virtual)
     * displays 3D view of the footprint (module) being edited.
     */
    void Show3D_Frame( wxCommandEvent& event ) override;

    /**
     * Function Update3D_Frame
     * must be called after a footprint selection
     * Updates the 3D view and 3D frame title.
     * @param aForceReloadFootprint = true to reload data (default)
     *   = false to update title only -(after creating the 3D viewer)
     */
    void Update3D_Frame( bool aForceReloadFootprint = true );

    /*
     * Virtual functions, not used here, but needed by PCB_BASE_FRAME
     * (virtual pure functions )
     */
    void OnLeftDClick( wxDC*, const wxPoint& ) override {}
    void SaveCopyInUndoList( BOARD_ITEM*, UNDO_REDO_T, const wxPoint& ) override {}
    void SaveCopyInUndoList( const PICKED_ITEMS_LIST&, UNDO_REDO_T, const wxPoint &) override {}

    void updateView();

    DECLARE_EVENT_TABLE()
};

#endif  // FOOTPRINT_VIEWER_FRAME_H
