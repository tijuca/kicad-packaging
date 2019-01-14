/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2012 Jean-Pierre Charras, jp.charras at wanadoo.fr
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

#include <fctsys.h>
#include <class_drawpanel.h>
#include <kicad_string.h>
#include <board_commit.h>
#include <pcb_edit_frame.h>
#include <pcb_layer_box_selector.h>
#include <class_board.h>
#include <class_module.h>
#include <class_edge_mod.h>
#include <class_pcb_text.h>
#include <widgets/unit_binder.h>

#include <dialog_global_edit_text_and_graphics_base.h>


// Columns of layer classes grid
enum
{
    COL_CLASS_NAME = 0,
    COL_LINE_THICKNESS,
    COL_TEXT_WIDTH,
    COL_TEXT_HEIGHT,
    COL_TEXT_THICKNESS,
    COL_TEXT_ITALIC,
    COL_TEXT_UPRIGHT
};

enum
{
    ROW_HEADER = 0,
    ROW_SILK,
    ROW_COPPER,
    ROW_EDGES,
    ROW_COURTYARD,
    ROW_OTHERS
};


static wxString g_textAndGraphicsReferenceFilter;
static wxString g_textAndGraphicsFootprintFilter;


class DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS : public DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS_BASE
{
    PCB_EDIT_FRAME*        m_parent;
    BOARD_DESIGN_SETTINGS* m_brdSettings;

    UNIT_BINDER            m_lineWidth;
    UNIT_BINDER            m_textWidth;
    UNIT_BINDER            m_textHeight;
    UNIT_BINDER            m_thickness;

public:
    DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS( PCB_EDIT_FRAME* parent );
    ~DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS() override;

protected:
    void OnUpdateUI( wxUpdateUIEvent& event ) override;
    void OnLayerFilterSelect( wxCommandEvent& event ) override
    {
        m_layerFilterOpt->SetValue( true );
    }
    void OnReferenceFilterText( wxCommandEvent& event ) override
    {
        m_referenceFilterOpt->SetValue( true );
    }
    void OnFootprintFilterText( wxCommandEvent& event ) override
    {
        m_footprintFilterOpt->SetValue( true );
    }

    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;

    void visitItem( BOARD_COMMIT& aCommit, BOARD_ITEM* aItem );
    void processItem( BOARD_COMMIT& aCommit, BOARD_ITEM* aItem );
};


DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS::DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS( PCB_EDIT_FRAME* parent ) :
        DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS_BASE( parent ),
        m_lineWidth( parent, m_lineWidthLabel, m_LineWidthCtrl, m_lineWidthUnits, true ),
        m_textWidth( parent, m_SizeXlabel, m_SizeXCtrl, m_SizeXunit, true ),
        m_textHeight( parent, m_SizeYlabel, m_SizeYCtrl, m_SizeYunit, true ),
        m_thickness( parent, m_ThicknessLabel, m_ThicknessCtrl, m_ThicknessUnit, true )
{
    m_parent = parent;
    m_brdSettings = &m_parent->GetDesignSettings();

    m_layerFilter->SetBoardFrame( m_parent );
    m_layerFilter->SetLayersHotkeys( false );
    m_layerFilter->Resync();

    m_LayerCtrl->SetBoardFrame( m_parent );
    m_LayerCtrl->SetLayersHotkeys( false );
    m_LayerCtrl->Resync();

    m_grid->SetCellHighlightPenWidth( 0 );
    wxFont infoFont = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
    infoFont.SetSymbolicSize( wxFONTSIZE_SMALL );
    m_grid->SetDefaultCellFont( infoFont );

    m_sdbSizerButtonsOK->SetDefault();

    FinishDialogSettings();
}


DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS::~DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS()
{
    g_textAndGraphicsReferenceFilter = m_referenceFilter->GetValue();
    g_textAndGraphicsFootprintFilter = m_footprintFilter->GetValue();
}


bool DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS::TransferDataToWindow()
{
    // SetValue() generates events, ChangeValue() does not
    m_referenceFilter->ChangeValue( g_textAndGraphicsReferenceFilter );
    m_footprintFilter->ChangeValue( g_textAndGraphicsFootprintFilter );

    m_lineWidth.SetValue( INDETERMINATE );
    m_textWidth.SetValue( INDETERMINATE );
    m_textHeight.SetValue( INDETERMINATE );
    m_thickness.SetValue( INDETERMINATE );
    m_Italic->Set3StateValue( wxCHK_UNDETERMINED );
    m_keepUpright->Set3StateValue( wxCHK_UNDETERMINED );
    m_Visible->Set3StateValue( wxCHK_UNDETERMINED );
    m_LayerCtrl->SetLayerSelection( UNDEFINED_LAYER );

#define SET_INT_VALUE( aRow, aCol, aValue ) \
        m_grid->SetCellValue( aRow, aCol, StringFromValue( GetUserUnits(), aValue, true, true ) )

#define SET_BOOL_VALUE( aRow, aCol, aValue ) \
        attr = new wxGridCellAttr; \
        attr->SetRenderer( new wxGridCellBoolRenderer() ); \
        attr->SetAlignment( wxALIGN_CENTER, wxALIGN_BOTTOM ); \
        attr->SetReadOnly(); \
        m_grid->SetAttr( aRow, aCol, attr ); \
        m_grid->SetCellValue( aRow, aCol, ( aValue ) ? "1" : "" )

    const BOARD_DESIGN_SETTINGS& bds = m_parent->GetBoard()->GetDesignSettings();
    wxGridCellAttr* attr;

    m_grid->SetCellValue( ROW_SILK,      COL_CLASS_NAME, _( "Silk Layers" ) );
    m_grid->SetCellValue( ROW_COPPER,    COL_CLASS_NAME, _( "Copper Layers" ) );
    m_grid->SetCellValue( ROW_EDGES,     COL_CLASS_NAME, _( "Edge Cuts" ) );
    m_grid->SetCellValue( ROW_COURTYARD, COL_CLASS_NAME, _( "Courtyards" ) );
    m_grid->SetCellValue( ROW_OTHERS,    COL_CLASS_NAME, _( "Other Layers" ) );

    m_grid->SetCellValue( ROW_HEADER, COL_LINE_THICKNESS, _( "Line Thickness" ) );
    SET_INT_VALUE( ROW_SILK,      COL_LINE_THICKNESS, bds.m_LineThickness[ LAYER_CLASS_SILK ] );
    SET_INT_VALUE( ROW_COPPER,    COL_LINE_THICKNESS, bds.m_LineThickness[ LAYER_CLASS_COPPER ] );
    SET_INT_VALUE( ROW_EDGES,     COL_LINE_THICKNESS, bds.m_LineThickness[ LAYER_CLASS_EDGES ] );
    SET_INT_VALUE( ROW_COURTYARD, COL_LINE_THICKNESS, bds.m_LineThickness[ LAYER_CLASS_COURTYARD ] );
    SET_INT_VALUE( ROW_OTHERS,    COL_LINE_THICKNESS, bds.m_LineThickness[ LAYER_CLASS_OTHERS ] );

    m_grid->SetCellValue( ROW_HEADER, COL_TEXT_WIDTH, _( "Text Width" ) );
    SET_INT_VALUE( ROW_SILK,   COL_TEXT_WIDTH, bds.m_TextSize[ LAYER_CLASS_SILK ].x );
    SET_INT_VALUE( ROW_COPPER, COL_TEXT_WIDTH, bds.m_TextSize[ LAYER_CLASS_COPPER ].x );
    SET_INT_VALUE( ROW_OTHERS, COL_TEXT_WIDTH, bds.m_TextSize[ LAYER_CLASS_OTHERS ].x );

    m_grid->SetCellValue( ROW_HEADER, COL_TEXT_HEIGHT, _( "Text Height" ) );
    SET_INT_VALUE( ROW_SILK,   COL_TEXT_HEIGHT, bds.m_TextSize[ LAYER_CLASS_SILK ].y );
    SET_INT_VALUE( ROW_COPPER, COL_TEXT_HEIGHT, bds.m_TextSize[ LAYER_CLASS_COPPER ].y );
    SET_INT_VALUE( ROW_OTHERS, COL_TEXT_HEIGHT, bds.m_TextSize[ LAYER_CLASS_OTHERS ].y );

    m_grid->SetCellValue( ROW_HEADER, COL_TEXT_THICKNESS, _( "Text Thickness" ) );
    SET_INT_VALUE( ROW_SILK,   COL_TEXT_THICKNESS, bds.m_TextThickness[ LAYER_CLASS_SILK ] );
    SET_INT_VALUE( ROW_COPPER, COL_TEXT_THICKNESS, bds.m_TextThickness[ LAYER_CLASS_COPPER ] );
    SET_INT_VALUE( ROW_OTHERS, COL_TEXT_THICKNESS, bds.m_TextThickness[ LAYER_CLASS_OTHERS ] );

    m_grid->SetCellValue(  ROW_HEADER, COL_TEXT_ITALIC, _( "Italic" ) );
    SET_BOOL_VALUE(  ROW_SILK,   COL_TEXT_ITALIC, bds.m_TextItalic[ LAYER_CLASS_SILK ] );
    SET_BOOL_VALUE(  ROW_COPPER, COL_TEXT_ITALIC, bds.m_TextItalic[ LAYER_CLASS_COPPER ] );
    SET_BOOL_VALUE(  ROW_OTHERS, COL_TEXT_ITALIC, bds.m_TextItalic[ LAYER_CLASS_OTHERS ] );

    m_grid->SetCellValue(  ROW_HEADER, COL_TEXT_UPRIGHT, _( "Upright" ) );
    SET_BOOL_VALUE(  ROW_SILK,   COL_TEXT_UPRIGHT, bds.m_TextUpright[ LAYER_CLASS_SILK ] );
    SET_BOOL_VALUE(  ROW_COPPER, COL_TEXT_UPRIGHT, bds.m_TextUpright[ LAYER_CLASS_COPPER ] );
    SET_BOOL_VALUE(  ROW_OTHERS, COL_TEXT_UPRIGHT, bds.m_TextUpright[ LAYER_CLASS_OTHERS ] );

    return true;

#undef SET_INT_VALUE
#undef SET_BOOL_VALUE
}


void DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS::OnUpdateUI( wxUpdateUIEvent&  )
{
    m_lineWidth.Enable( m_setToSpecifiedValues->GetValue() );
    m_textWidth.Enable( m_setToSpecifiedValues->GetValue() );
    m_textHeight.Enable( m_setToSpecifiedValues->GetValue() );
    m_thickness.Enable( m_setToSpecifiedValues->GetValue() );
    m_Italic->Enable( m_setToSpecifiedValues->GetValue() );
    m_Visible->Enable( m_setToSpecifiedValues->GetValue() );
    m_LayerLabel->Enable( m_setToSpecifiedValues->GetValue() );
    m_LayerCtrl->Enable( m_setToSpecifiedValues->GetValue() );
    m_keepUpright->Enable( m_setToSpecifiedValues->GetValue() );
}


void DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS::processItem( BOARD_COMMIT& aCommit, BOARD_ITEM* aItem )
{
    aCommit.Modify( aItem );

    auto textItem = dynamic_cast<EDA_TEXT*>( aItem );
    auto drawItem = dynamic_cast<DRAWSEGMENT*>( aItem );
    auto moduleTextItem = dyn_cast<TEXTE_MODULE*>( aItem );

    if( m_setToSpecifiedValues->GetValue() )
    {
        if( m_LayerCtrl->GetLayerSelection() != UNDEFINED_LAYER )
            aItem->SetLayer( ToLAYER_ID( m_LayerCtrl->GetLayerSelection() ) );

        if( !m_textWidth.IsIndeterminate() && textItem )
            textItem->SetTextSize( wxSize( m_textWidth.GetValue(), textItem->GetTextSize().y ) );

        if( !m_textHeight.IsIndeterminate() && textItem )
            textItem->SetTextSize( wxSize( textItem->GetTextSize().x, m_textHeight.GetValue() ) );

        if( !m_thickness.IsIndeterminate() && textItem )
            textItem->SetThickness( m_thickness.GetValue() );

        if( m_Italic->Get3StateValue() != wxCHK_UNDETERMINED && textItem )
            textItem->SetItalic( m_Italic->GetValue() );

        if( m_Visible->Get3StateValue() != wxCHK_UNDETERMINED && textItem )
            textItem->SetVisible( m_Visible->GetValue() );

        if( m_keepUpright->Get3StateValue() != wxCHK_UNDETERMINED && moduleTextItem )
            moduleTextItem->SetKeepUpright( m_keepUpright->GetValue() );

        if( !m_lineWidth.IsIndeterminate() && drawItem )
            drawItem->SetWidth( m_lineWidth.GetValue() );
    }
    else
    {
        PCB_LAYER_ID layer = aItem->GetLayer();

        if( textItem )
        {
            textItem->SetTextSize( m_brdSettings->GetTextSize( layer ) );
            textItem->SetThickness( m_brdSettings->GetTextThickness( layer ) );
            textItem->SetItalic( m_brdSettings->GetTextItalic( layer ) );
        }

        if( moduleTextItem )
            moduleTextItem->SetKeepUpright( m_brdSettings->GetTextUpright( layer ) );

        if( drawItem )
            drawItem->SetWidth( m_brdSettings->GetLineThickness( layer ) );
    }
}


void DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS::visitItem( BOARD_COMMIT& aCommit, BOARD_ITEM* aItem )
{
    if( m_layerFilterOpt->GetValue() && m_layerFilter->GetLayerSelection() != UNDEFINED_LAYER )
    {
        if( aItem->GetLayer() != m_layerFilter->GetLayerSelection() )
            return;
    }

    if( m_referenceFilterOpt->GetValue() && !m_referenceFilter->GetValue().IsEmpty() )
    {
        MODULE* module = dynamic_cast<MODULE*>( aItem->GetParent() );

        if( module )
        {
            if( !WildCompareString( m_referenceFilter->GetValue(), module->GetReference(), false ) )
                return;
        }
    }

    if( m_footprintFilterOpt->GetValue() && !m_footprintFilter->GetValue().IsEmpty() )
    {
        MODULE* module = dynamic_cast<MODULE*>( aItem->GetParent() );

        if( module )
        {
            if( !WildCompareString( m_footprintFilter->GetValue(), module->GetFPID().Format(), false ) )
                return;
        }
    }

    processItem( aCommit, aItem );
}


bool DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS::TransferDataFromWindow()
{
    if( !m_textWidth.Validate( TEXTS_MIN_SIZE, TEXTS_MAX_SIZE )
        || !m_textHeight.Validate( TEXTS_MIN_SIZE, TEXTS_MAX_SIZE ) )
    {
        return false;
    }

    BOARD_COMMIT commit( m_parent );

    // Go through the modules
    for( MODULE* module : m_parent->GetBoard()->Modules() )
    {
        if( m_references->GetValue() )
            visitItem( commit, &module->Reference() );

        if( m_values->GetValue() )
            visitItem( commit, &module->Value() );

        // Go through all other module items
        for( BOARD_ITEM* boardItem : module->GraphicalItems() )
        {
            if( boardItem->Type() == PCB_MODULE_TEXT_T )
            {
                const wxString& text = dynamic_cast<EDA_TEXT*>( boardItem )->GetText();

                if( m_references->GetValue() && text == wxT( "%R" ) )
                    visitItem( commit, boardItem );
                else if( m_values->GetValue() && text == wxT( "%V" ) )
                    visitItem( commit, boardItem );
                else if( m_otherFields->GetValue() )
                    visitItem( commit, boardItem );
            }
            else if( boardItem->Type() == PCB_MODULE_EDGE_T )
            {
                if( m_footprintGraphics->GetValue() )
                    visitItem( commit, boardItem );
            }
        }
    }

    // Go through the PCB text & graphic items
    for( BOARD_ITEM* boardItem : m_parent->GetBoard()->Drawings() )
    {
        if( boardItem->Type() == PCB_TEXT_T )
        {
            if( m_boardText->GetValue() )
                visitItem( commit, boardItem );
        }
        else if( boardItem->Type() == PCB_LINE_T )
        {
            if( m_boardGraphics->GetValue() )
                visitItem( commit, boardItem );
        }
    }

    commit.Push( "Edit text and graphics properties" );
    m_parent->GetCanvas()->Refresh();

    return true;
}


void PCB_EDIT_FRAME::OnEditTextAndGraphics( wxCommandEvent& event )
{
    DIALOG_GLOBAL_EDIT_TEXT_AND_GRAPHICS dlg( this );
    dlg.ShowModal();
}


