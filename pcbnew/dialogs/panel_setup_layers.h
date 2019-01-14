/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see change_log.txt for contributors.
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


#ifndef PANEL_SETUP_LAYERS_H
#define PANEL_SETUP_LAYERS_H

#include <widgets/unit_binder.h>
#include <widgets/paged_dialog.h>
#include <panel_setup_layers_base.h>

class PCB_EDIT_FRAME;
class BOARD;
class BOARD_DESIGN_SETTINGS;


/**
 * Holds the 3 UI control pointers for a single board layer.
 */
struct PANEL_SETUP_LAYERS_CTLs
{
    PANEL_SETUP_LAYERS_CTLs( wxControl* aName, wxCheckBox* aCheckBox, wxControl* aChoiceOrDesc )
    {
        name     = aName;
        checkbox = aCheckBox;
        choice   = aChoiceOrDesc;
    }

    wxControl*      name;
    wxCheckBox*     checkbox;
    wxControl*      choice;
};


class PANEL_SETUP_LAYERS : public PANEL_SETUP_LAYERS_BASE
{
public:
    PANEL_SETUP_LAYERS( PAGED_DIALOG* aParent, PCB_EDIT_FRAME* aFrame );

    void ImportSettingsFrom( BOARD* aBoard );

private:
    PAGED_DIALOG*   m_Parent;
    PCB_EDIT_FRAME* m_frame;

    BOARD*          m_pcb;
    LSET            m_enabledLayers;
    UNIT_BINDER     m_pcbThickness;

    void setLayerCheckBox( LAYER_NUM layer, bool isChecked );
    void setCopperLayerCheckBoxes( int copperCount );
    void setMandatoryLayerCheckBoxes();

    void showCopperChoice( int copperCount );
    void showBoardLayerNames();
    void showSelectedLayerCheckBoxes( LSET enableLayerMask );
    void showLayerTypes();
    void showPresets( LSET enabledLayerMask );

    /** Return the selected layer mask within the UI checkboxes */
    LSET getUILayerMask();
    wxString getLayerName( LAYER_NUM layer );
    int getLayerTypeIndex( LAYER_NUM layer );

    void OnCheckBox( wxCommandEvent& event ) override;
    void DenyChangeCheckBox( wxCommandEvent& event ) override;
    void OnPresetsChoice( wxCommandEvent& event ) override;
    void OnCopperLayersChoice( wxCommandEvent& event ) override;
    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;

    bool testLayerNames();

    /**
     * Return a list of layers removed from the board that contain items.
     */
    LSEQ getRemovedLayersWithItems();

    /**
     * Return a list of layers in use in footprints, and therefore not removable.
     */
    LSEQ getNonRemovableLayers();

    PANEL_SETUP_LAYERS_CTLs getCTLs( LAYER_NUM aLayerNumber );
    wxControl*  getName( LAYER_NUM aLayer );
    wxCheckBox* getCheckBox( LAYER_NUM aLayer );
    wxChoice*   getChoice( LAYER_NUM aLayer );
};



#endif //PANEL_SETUP_LAYERS_H


