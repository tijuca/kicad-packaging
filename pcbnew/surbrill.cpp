/****************************/
/* affichage des empreintes */
/****************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"

#include "protos.h"

#define Pad_fill (Pad_Fill_Item.State == RUN)

static void Pad_Surbrillance( WinEDA_DrawPanel* panel, wxDC* DC, MODULE* Module, int NetCode );

/* variables locales : */
static int draw_mode;


/*********************************************************/
void WinEDA_PcbFrame::Liste_Equipot( wxCommandEvent& event )
/*********************************************************/

/* Display a filtered list of equipot names
 *  if an equipot is selected the corresponding tracks and pads are highlighted
 */
{
    EQUIPOT*          Equipot;
    wxString          msg;
    WinEDA_TextFrame* List;
    int ii, jj;

    msg = wxT( "*" );
    Get_Message( _( "Filter for net names:" ), msg, this );
    if( msg.IsEmpty() )
        return;

    List = new WinEDA_TextFrame( this, _( "List Nets" ) );

    Equipot = (EQUIPOT*) m_Pcb->m_Equipots;
    for( ; Equipot != NULL; Equipot = (EQUIPOT*) Equipot->Pnext )
    {
        wxString Line;
        /* calcul adr relative du nom de la pastille reference de la piste */
        if( !WildCompareString( msg, Equipot->m_Netname, FALSE ) )
            continue;

        Line.Printf( wxT( "net_code = %3.3d  [%.16s] " ), Equipot->GetNet(),
                    Equipot->m_Netname.GetData() );
        List->Append( Line );
    }

    ii = List->ShowModal(); List->Destroy();
    if( ii < 0 )
        return;

    /* Recherche du numero de net rellement selectionn�*/
    Equipot = (EQUIPOT*) m_Pcb->m_Equipots;
    for( jj = 0; Equipot != NULL; Equipot = (EQUIPOT*) Equipot->Pnext )
    {
        /* calcul adr relative du nom de la pastille reference de la piste */
        if( !WildCompareString( msg, Equipot->m_Netname, FALSE ) )
            continue;
        if( ii == jj )
        {
            ii = Equipot->GetNet();
            break;
        }
        jj++;
    }

    wxClientDC dc( DrawPanel );

    DrawPanel->PrepareGraphicContext( &dc );

    if( g_HightLigt_Status )
        Hight_Light( &dc );
    g_HightLigth_NetCode = ii;
    Hight_Light( &dc );
}


/**************************************************/
int WinEDA_PcbFrame::Select_High_Light( wxDC* DC )
/**************************************************/

/* Localise track ou pad et met en surbrillance le net correspondant
 *  Retourne le netcode, ou -1 si pas de net localis�*/
{
    TRACK* pt_piste;
    D_PAD* pt_pad;
    int    masquelayer = g_TabOneLayerMask[GetScreen()->m_Active_Layer];
    int    code = -1;

    if( g_HightLigt_Status )
        Hight_Light( DC );
    
    pt_piste = Locate_Pistes( m_Pcb->m_Track, masquelayer, CURSEUR_OFF_GRILLE );
    if( pt_piste )
    {
        code = g_HightLigth_NetCode = pt_piste->GetNet();
        Hight_Light( DC );
    }
    else
    {
        pt_pad = Locate_Any_Pad( m_Pcb, CURSEUR_OFF_GRILLE );
        if( pt_pad != NULL )
        {
            code = g_HightLigth_NetCode = pt_pad->GetNet();
            Hight_Light( DC );
            SendMessageToEESCHEMA( pt_pad );
        }
    }

    return code;
}


/*******************************************/
void WinEDA_PcbFrame::Hight_Light( wxDC* DC )
/*******************************************/

/*
 *  fonction d'appel de Surbrillance a partir du menu
 *  Met ou supprime la surbrillance d'un net pointe par la souris
 */
{
    g_HightLigt_Status = !g_HightLigt_Status;
    DrawHightLight( DC, g_HightLigth_NetCode );
}


/****************************************************************/
void WinEDA_PcbFrame::DrawHightLight( wxDC* DC, int NetCode )
/****************************************************************/

/* Turn On or OFF the HightLight for trcak and pads with the netcode "NetCode'
 */
{
    TRACK*  pts;
    MODULE* Module;

    if( g_HightLigt_Status )
        draw_mode = GR_SURBRILL | GR_OR;
    else
        draw_mode = GR_AND | GR_SURBRILL;

    Module = m_Pcb->m_Modules;

    /* Redraw pads */
    for( ; Module != NULL; Module = (MODULE*) Module->Pnext )
    {
        Pad_Surbrillance( DrawPanel, DC, Module, NetCode );
    }

    /* Redraw track and vias: */
    for( pts = m_Pcb->m_Track; pts != NULL; pts = (TRACK*) pts->Pnext )
    {
        if( pts->GetNet() == NetCode )
        {
            pts->Draw( DrawPanel, DC, draw_mode );
        }
    }
}


/*******************************************************/
static void Pad_Surbrillance( WinEDA_DrawPanel* panel,
                              wxDC* DC, MODULE* Module, int NetCode )
/*******************************************************/
/* Mise en Surbrillance des Pads */
{
    D_PAD* pt_pad;

    /* trace des pastilles */
    for( pt_pad = Module->m_Pads; pt_pad != NULL; pt_pad = (D_PAD*) pt_pad->Pnext )
    {
        if( pt_pad->GetNet() == NetCode )
        {
            pt_pad->Draw( panel, DC, wxPoint( 0, 0 ), draw_mode );
        }
    }
}
