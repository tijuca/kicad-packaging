/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2013 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 1992-2016 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <pcb_edit_frame.h>
#include "dialog_import_gfx_base.h"
#include <import_gfx/graphics_importer_pcbnew.h>

class GRAPHICS_IMPORT_MGR;

class DIALOG_IMPORT_GFX : public DIALOG_IMPORT_GFX_BASE
{
public:
    DIALOG_IMPORT_GFX( PCB_BASE_FRAME* aParent, bool aUseModuleItems = false );
    ~DIALOG_IMPORT_GFX();


    /**
     * Function GetImportedItems()
     * @return a list of items imported from a vector graphics file.
     */
    std::list<std::unique_ptr<EDA_ITEM>>& GetImportedItems()
    {
        return m_importer->GetItems();
    }

    /** @return true if the placement is interactive, i.e. all imported
     * items must be moved by the mouse cursor to the final position
     * false means the imported items are placed to the final position after import.
     */
    bool IsPlacementInteractive()
    {
        return m_placementInteractive;
    }

private:
    PCB_BASE_FRAME*      m_parent;
    wxConfigBase*        m_config;              // Current config
    std::unique_ptr<GRAPHICS_IMPORTER_PCBNEW> m_importer;
    std::unique_ptr<GRAPHICS_IMPORT_MGR>      m_gfxImportMgr;
    int                  m_originImportUnits;
    VECTOR2D             m_importOrigin;        // This is the offset to add to imported coordinates
                                                // Always in mm

    static wxString      m_filename;
    static bool          m_placementInteractive;
    static LAYER_NUM     m_layer;
    double               m_default_lineWidth;   // always in mm: line width when a line width is not specified
    int                  m_lineWidthImportUnits;
    static double        m_scaleImport;         // a scale factor to change the size of imported items
                                                // m_scaleImport =1.0 means keep original size

    // Virtual event handlers
    void onOKClick( wxCommandEvent& event ) override;
    void onUnitPositionSelection( wxCommandEvent& event ) override;
    void onUnitWidthSelection( wxCommandEvent& event ) override;
    void onBrowseFiles( wxCommandEvent& event ) override;
    void originOptionOnUpdateUI( wxUpdateUIEvent& event ) override;
	void onInteractivePlacement( wxCommandEvent& event ) override
    {
        m_placementInteractive = true;
    }
	void onAbsolutePlacement( wxCommandEvent& event ) override
    {
        m_placementInteractive = false;
    }

    void updatePcbImportOffsets_mm();
    double getPCBdefaultLineWidthMM();
    void showPCBdefaultLineWidth();
    void showPcbImportOffsets();
};
