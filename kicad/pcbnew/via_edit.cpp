/**********************************************/
/* vi_edit.cpp: som editing function for vias */
/**********************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"

#include "id.h"


/**********************************************************************************/
void WinEDA_PcbFrame::Via_Edit_Control( wxDC* DC, int command_type, SEGVIA* via )
/**********************************************************************************/

/*
  * Execute edit commands relative to vias
 */
{
    int    ii;
    TRACK* via_struct;

    switch( command_type )
    {
    case ID_POPUP_PCB_SELECT_VIASIZE1:
    case ID_POPUP_PCB_SELECT_VIASIZE2:
    case ID_POPUP_PCB_SELECT_VIASIZE3:
    case ID_POPUP_PCB_SELECT_VIASIZE4:
    case ID_POPUP_PCB_SELECT_VIASIZE5:
    case ID_POPUP_PCB_SELECT_VIASIZE6:
    case ID_POPUP_PCB_SELECT_VIASIZE7:
    case ID_POPUP_PCB_SELECT_VIASIZE8:      // selec the new current value for via size (via diameter)
        DrawPanel->MouseToCursorSchema();
        ii = command_type - ID_POPUP_PCB_SELECT_VIASIZE1;
        g_DesignSettings.m_CurrentViaSize = g_DesignSettings.m_ViaSizeHistory[ii];
        DisplayTrackSettings();
        break;

    case ID_POPUP_PCB_VIA_HOLE_ENTER_VALUE:     // Enter a new alternate value for drill via
        InstallPcbOptionsFrame( wxDefaultPosition, DC, ID_PCB_TRACK_SIZE_SETUP );
        DrawPanel->MouseToCursorSchema();

    case ID_POPUP_PCB_VIA_HOLE_TO_VALUE:        // Set the drill via to custom
        if( (g_DesignSettings.m_ViaDrillCustomValue > 0) && (g_DesignSettings.m_ViaDrillCustomValue < via->m_Width) )
        {
            via->Draw( DrawPanel, DC, GR_XOR );
            via->SetDrillValue( g_DesignSettings.m_ViaDrillCustomValue );
            via->Draw( DrawPanel, DC, GR_OR );
            GetScreen()->SetModify();
        }
        else
            DisplayError( this, _( "Incorrect value for Via drill. No via drill change" ) );
        break;


    case ID_POPUP_PCB_VIA_HOLE_EXPORT:      // Export the current drill value as the new custom value
        if( via->GetDrillValue() > 0 )
            g_DesignSettings.m_ViaDrillCustomValue = via->GetDrillValue();
        break;

    case ID_POPUP_PCB_VIA_HOLE_EXPORT_TO_OTHERS:    // Export the current drill value to via which h�ave the same size
        if( via->GetDrillValue() > 0 )
            g_DesignSettings.m_ViaDrillCustomValue = via->GetDrillValue();
        via_struct = m_Pcb->m_Track;
        for( ; via_struct != NULL; via_struct = (TRACK*) via_struct->Pnext )
        {
            if( via_struct->Type() == TYPEVIA )     /* mise a jour du diametre de la via */
            {
                if( via_struct->m_Width != via->m_Width )
                    continue;
                via_struct->Draw( DrawPanel, DC, GR_XOR );
                via_struct->SetDrillValue( via->GetDrillValue() );
                via_struct->Draw( DrawPanel, DC, GR_OR );
            }
        }

        GetScreen()->SetModify();
        break;

    case ID_POPUP_PCB_VIA_HOLE_TO_DEFAULT:
        via->Draw( DrawPanel, DC, GR_XOR );
        via->SetDrillDefault();
        via->Draw( DrawPanel, DC, GR_OR );
        GetScreen()->SetModify();
        break;

    case ID_POPUP_PCB_VIA_HOLE_RESET_TO_DEFAULT:        // Reset all via hole to default value
        via_struct = m_Pcb->m_Track;
        for( ; via_struct != NULL; via_struct = via_struct->Next() )
        {
            if( via_struct->Type() == TYPEVIA )     /* mise a jour du diametre de la via */
            {
                if( ! via_struct->IsDrillDefault() )
                {
                    via_struct->Draw( DrawPanel, DC, GR_XOR );
                    via_struct->SetDrillDefault();
                    via_struct->Draw( DrawPanel, DC, GR_OR );
                }
            }
        }

        GetScreen()->SetModify();
        break;

    default:
        DisplayError( this, wxT( "WinEDA_PcbFrame::Via_Edition() error: unknown command" ) );
        break;
    }

    DrawPanel->MouseToCursorSchema();
}
