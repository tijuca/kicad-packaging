/*****************************/
/* Localisation des elements */
/*****************************/


#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"
#include "trigo.h"
#include "autorout.h"

#include "protos.h"

                                             
/* fonctions locales */
MIREPCB* Locate_MirePcb( BOARD_ITEM* PtStruct, int LayerSearch, int typeloc );
D_PAD* Locate_Any_Pad( BOARD* Pcb, const wxPoint& ref_pos, bool OnlyCurrentLayer );


/**
 * Function RefPos
 * returns the reference position, coming from either the mouse position or the
 * the cursor position, based on whether the typeloc has the CURSEUR_OFF_GRILLE 
 * flag ORed in or not.
 * @param typeloc int with possible CURSEUR_OFF_GRILLE bit on.
 * @return wxPoint - The reference point, either the mouse position or 
 *   the cursor position.
 */
wxPoint inline RefPos( int typeloc )
{
    return ActiveScreen->RefPos( (typeloc & CURSEUR_OFF_GRILLE) != 0 );
}


/*************************************************************/
MODULE* ReturnModule( BOARD* pcb, const wxString& reference )
/*************************************************************/

/*
 *  Recherche d'un module par sa reference
 *  Retourne:
 *      un pointeur sur le module
 *      Null si pas localis�
 */
{
    MODULE* Module = pcb->m_Modules;

    for(  ; Module != NULL; Module = (MODULE*) Module->Pnext )
    {
        if( reference.CmpNoCase( Module->m_Reference->m_Text ) == 0 )
            return Module;
    }

    return NULL;
}


/********************************************************/
D_PAD* ReturnPad( MODULE* module, const wxString& name )
/********************************************************/

/* Recherche d'un pad par son nom, pour le module Module
 */
{
    D_PAD*   pt_pad;
    wxString buf;

    if( module == NULL )
        return NULL;

    pt_pad = module->m_Pads;

    for( ; pt_pad != NULL; pt_pad = (D_PAD*) pt_pad->Pnext )
    {
        pt_pad->ReturnStringPadName( buf );
        if( buf.CmpNoCase( name ) == 0 )
            return pt_pad;
    }

    return NULL;
}


/*******************************************************************************/
BOARD_ITEM* WinEDA_BasePcbFrame::Locate( int typeloc, int LayerSearch )
/*******************************************************************************/

/* General locate function
 * Display infos relatives to the item found
 * return a pointer to this item ( or NULL )
 */
{
    int             masque_layer;
    BOARD_ITEM*     item;

    item = Locate_Texte_Pcb( m_Pcb->m_Drawings, LayerSearch, typeloc );
    if( item ) 
    {
        item->Display_Infos( this );
        return item;
    }

    item = Locate_Segment_Pcb( m_Pcb, LayerSearch, typeloc );
    if( item )
    {
        item->Display_Infos( this );
        return item;
    }

    item = Locate_Cotation( m_Pcb, LayerSearch, typeloc );
    if( item )
    {
        item->Display_Infos( this );
        return item;
    }

    item = Locate_MirePcb( m_Pcb->m_Drawings, LayerSearch, typeloc );
    if( item != NULL )
    {
        item->Display_Infos( this );    // MIRES::Display_Infos() not implemented yet.
        return item;
    }

    /* Search for tracks and vias, with via priority */
    if( LayerSearch == -1 )
        masque_layer = ALL_LAYERS;
    else
        masque_layer = g_TabOneLayerMask[LayerSearch];

    TRACK* Track;
    Track = Locate_Pistes( m_Pcb->m_Track, masque_layer, typeloc );
    if( Track != NULL )
    {
        TRACK* TrackLocate = Track;   /* a track or a via is found */
        
        /* Search for a via */
        while( ( TrackLocate = Locate_Pistes( TrackLocate,
                                              masque_layer, typeloc ) ) != NULL )
        {
            Track = TrackLocate;
            if( TrackLocate->Type() == TYPEVIA )
                break;

            TrackLocate = (TRACK*) TrackLocate->Pnext;
        }

        Track->Display_Infos( this );
        return Track;
    }

    item = Locate_Any_Pad( m_Pcb, typeloc );
    if( item )
    {
        item->Display_Infos( this ); 
        return item;
    }

    
    /* Search for a footprint text */

    // First search: locate texts for footprints on copper or component layer
    // Priority to the active layer (component or copper).
    // This is useful for small smd components when 2 texts overlap but are not 
    // on the same layer
    if( LayerSearch == COPPER_LAYER_N   ||   LayerSearch == LAYER_CMP_N )
    {
        MODULE* module = m_Pcb->m_Modules;
        for(   ;   module != NULL;  module = (MODULE*) module->Pnext )
        {
            if( module->GetLayer() != LayerSearch )
                continue;

            item = LocateTexteModule( m_Pcb, &module, typeloc | VISIBLE_ONLY );
            if( item )
            {
                item->Display_Infos( this );
                return item;
            }
        }
    }

    // Now Search footprint texts on all layers
    MODULE* module;
    module = NULL;
    item = LocateTexteModule( m_Pcb, &module, typeloc | VISIBLE_ONLY );
    if( item )
    {
        item->Display_Infos( this );
        return item;
    }

    /* Search for a footprint */
    item = Locate_Prefered_Module( m_Pcb, typeloc | VISIBLE_ONLY );
    if( item )
    {
        item->Display_Infos( this );
        return item;
    }

    /* Search for zones */
    item = Locate_Zone( (TRACK*) m_Pcb->m_Zone,
                                    GetScreen()->m_Active_Layer, typeloc );
    if( item )
    {
        item->Display_Infos( this );
        return item;
    }

    MsgPanel->EraseMsgBox();
    return NULL;
}


/*******************************************************************/
TRACK* Locate_Via( BOARD* Pcb, const wxPoint& pos, int layer )
/*******************************************************************/

/* Localise une via au point pX,pY
 *  Si layer < 0 la via sera localisee quelle que soit la couche
 *  Si layer = 0 .. 15 la via sera localisee selon son type:
 *      - traversante : toutes couches
 *      - aveugle = entre couches utiles
 *      - borgnes idem
 *  Entree : coord du point de reference, couche
 *  Sortie: NULL si pas de via
 *          (TRACK*) adresse de la via
 */
{
    TRACK* Track;

    for( Track = Pcb->m_Track; Track != NULL; Track = Track->Next() )
    {
        if( Track->Type() != TYPEVIA )
            continue;
        if( Track->m_Start != pos )
            continue;
        if( Track->GetState( BUSY | DELETED ) )
            continue;
        if( layer < 0 )
            return Track;
        if( Track->IsOnLayer( layer ) )
            return Track;
    }

    return NULL;
}


/********************************************************************/
D_PAD* Locate_Pad_Connecte( BOARD* Pcb, TRACK* ptr_piste, int extr )
/********************************************************************/

/* localisation de la pastille connectee au point de piste a tester
 *  entree : ptr_piste: pointeur sur le segment de piste
 *          extr = flag = START -> debut du segment a tester
 *                      = END -> fin du segment a tester
 *  retourne:
 *       un pointeur sur la description de la pastille si localisation
 *       pointeur NULL si pastille non trouvee
 */
{
    D_PAD*  ptr_pad = NULL;
    wxPoint ref_pos;

    int     masque_layer = g_TabOneLayerMask[ptr_piste->GetLayer()];
    
    if( extr == START )
    {
        ref_pos = ptr_piste->m_Start;
    }
    else
    {
        ref_pos = ptr_piste->m_End;
    }
    
    for( MODULE* module = Pcb->m_Modules;  module;  module = module->Next() )
    {
        ptr_pad = Locate_Pads( module, ref_pos, masque_layer );
        if( ptr_pad != NULL )
            break;
    }

    return ptr_pad;
}


/****************************************************************/
EDGE_MODULE* Locate_Edge_Module( MODULE* module, int typeloc )
/****************************************************************/

/* Localisation de segments de contour du type edge MODULE
 *      Les contours sont de differents type:
 *      simple : succession de droites
 *      Arcs de cercles : on a alors debut arc, fin arc , centre
 *                      si debut arc = fin arc : cercle complet
 * 
 *  Retourne:
 *      Pointeur sur le segment localise
 *      NULL si rien trouve
 */
{
    if( !module )
        return NULL;

    /* coord du point de localisation */
    wxPoint  ref_pos = RefPos( typeloc );

    EDA_BaseStruct* PtStruct = module->m_Drawings;
    for( ; PtStruct != NULL;  PtStruct = PtStruct->Pnext )
    {
        if( PtStruct->Type() != TYPEEDGEMODULE )
            continue;

        // calls virtual EDGE_MODULE::HitTest() 
        if( PtStruct->HitTest( ref_pos ) )
            return (EDGE_MODULE*) PtStruct;
    }

    return NULL;
}


/*************************************************************************/
COTATION* Locate_Cotation( BOARD* Pcb, int LayerSearch, int typeloc )
/*************************************************************************/

/* Serach for a cotation item , on LayerSearch,
 *  (if LayerSearch == -1 , no yaere restriction )
 *  return a pointer to the located item, or NULL
 */
{
    wxPoint  ref_pos = RefPos( typeloc );

    BOARD_ITEM* PtStruct = Pcb->m_Drawings;
    for( ; PtStruct != NULL; PtStruct = PtStruct->Next() )
    {
        if( PtStruct->Type() != TYPECOTATION )
            continue;

        // calls virtual COTATION::HitTest() 
        if( PtStruct->HitTest( ref_pos ) )
            return (COTATION*) PtStruct;
    }

    return NULL;
}


/*************************************************************************/
DRAWSEGMENT* Locate_Segment_Pcb( BOARD* Pcb, int LayerSearch, int typeloc )
/*************************************************************************/

/* Localisation de segments de contour du type drawing
 *  Retourne:
 *      Pointeur sur DEBUT du segment localise
 *      NULL si rien trouve
 *  Le segment sur la couche active est d�tect� en priorite
 */
{

    DRAWSEGMENT*    locate_segm = NULL;
    PCB_SCREEN*     screen = (PCB_SCREEN*) ActiveScreen;
   
    wxPoint         ref_pos = RefPos( typeloc );

    EDA_BaseStruct* PtStruct = Pcb->m_Drawings;
    for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext )
    {
        if( PtStruct->Type() != TYPEDRAWSEGMENT )
            continue;
        
        DRAWSEGMENT*  pts = (DRAWSEGMENT*) PtStruct;
        
        if( (pts->GetLayer() != LayerSearch) && (LayerSearch != -1) )
            continue;

        if( pts->HitTest( ref_pos ) )
        {
            // return this hit if layer matches, else remember in 
            // case no layer match is found.
            if( pts->GetLayer() == screen->m_Active_Layer )
                return pts;

            else if( !locate_segm )
                locate_segm = pts;
        }
    }

    return locate_segm;
}


/*************************************************
 * D_PAD * Locate_Any_Pad( BOARD* Pcb, int typeloc, bool OnlyCurrentLayer)
 * D_PAD* Locate_Any_Pad( BOARD* Pcb, int ref_pos, bool OnlyCurrentLayer)
 *************************************************/

/*
 *  localisation de la pastille pointee par la coordonnee ref_pos.x,,ref_pos.y, ou
 *  par la souris,  recherche faite sur toutes les empreintes.
 *  entree :
 *      - coord souris
 *       ou ref_pos
 *  retourne:
 *       pointeur sur la description de la pastille si localisation
 *       pointeur NULL si pastille non trouvee
 *       num_empr = numero d'empreinte du pad
 * 
 *  la priorit� est donn�e a la couche active
 */

D_PAD* Locate_Any_Pad( BOARD* Pcb, int typeloc, bool OnlyCurrentLayer )
{
    wxPoint ref_pos = RefPos( typeloc );
    return Locate_Any_Pad( Pcb, ref_pos, OnlyCurrentLayer );
}


D_PAD* Locate_Any_Pad( BOARD* Pcb, const wxPoint& ref_pos, bool OnlyCurrentLayer )
{
    int     layer_mask = g_TabOneLayerMask[ ( (PCB_SCREEN*) ActiveScreen )->m_Active_Layer];

    for( MODULE* module=Pcb->m_Modules;  module;   module = module->Next() )
    {
        D_PAD*  pt_pad;
        
        /* First: Search a pad on the active layer: */
        if( ( pt_pad = Locate_Pads( module, ref_pos, layer_mask ) ) != NULL )
            return pt_pad;

        /* If not found, search on other layers: */
        if( !OnlyCurrentLayer )
        {
            if( ( pt_pad = Locate_Pads( module, ref_pos, ALL_LAYERS ) ) != NULL )
                return pt_pad;
        }
    }

    return NULL;
}


/******************************************************************************/
/* D_PAD* Locate_Pads(MODULE * module, int masque_layer,int typeloc)          */
/* D_PAD* Locate_Pads(MODULE * module, const wxPoint & ref_pos,int masque_layer) */
/******************************************************************************/

/* localisation de la pastille pointee par la coordonnee ref_pos.x,,ref_pos.y, ou
 *  par la souris,  concernant l'empreinte  en cours.
 *  entree :
 *      - parametres generaux de l'empreinte mise a jour par caract()
 *      - masque_layer = couche(s) (bit_masque)sur laquelle doit etre la pastille
 *  retourne:
 *       un pointeur sur la description de la pastille si localisation
 *       pointeur NULL si pastille non trouvee
 */

D_PAD* Locate_Pads( MODULE* module, int masque_layer, int typeloc )
{
    wxPoint ref_pos = RefPos( typeloc );
    return Locate_Pads( module, ref_pos, masque_layer );
}


D_PAD* Locate_Pads( MODULE* module, const wxPoint& ref_pos, int masque_layer )
{
    for( D_PAD* pt_pad = module->m_Pads;   pt_pad;   pt_pad = pt_pad->Next() )
    {
        /* ... et sur la bonne couche */
        if( (pt_pad->m_Masque_Layer & masque_layer) == 0 )
            continue;

        if( pt_pad->HitTest( ref_pos ) )
            return pt_pad;
    }

    return NULL;
}


/**
 * Function Locate_Prefered_Module
 * locates a footprint by its bounding rectangle.  If several footprints
 * are possible, then the priority is: on the active layer, then smallest.
 * The current mouse or cursor coordinates are grabbed from the active window
 * to performe hit-testing.
 *
 * @param Pcb The BOARD to search within.
 * @param typeloc Flag bits, tuning the search, see pcbnew.h
 * @return MODULE* - the best module or NULL if none.
 */
MODULE* Locate_Prefered_Module( BOARD* Pcb, int typeloc )
{
    MODULE* pt_module;
    int     lx, ly;                         /* dimensions du rectangle d'encadrement du module */
    MODULE* module      = NULL;             /* module localise sur la couche active */
    MODULE* Altmodule   = NULL;             /* module localise sur les couches non actives */
    int     min_dim     = 0x7FFFFFFF;       /* dim mini du module localise sur la couche active */
    int     alt_min_dim = 0x7FFFFFFF;       /* dim mini du module localise sur les couches non actives */
    int     layer;                          /* pour calcul de couches prioritaires */
    wxPoint ref_pos;                        /* coord du point de reference pour la localisation */

    ref_pos = RefPos( typeloc );

    pt_module = Pcb->m_Modules;
    for(  ;  pt_module;  pt_module = (MODULE*) pt_module->Pnext )
    {
        // is the ref point within the module's bounds?
        if( !pt_module->HitTest( ref_pos ) )
            continue;

        // if caller wants to ignore locked modules, and this one is locked, skip it.
        if( (typeloc & IGNORE_LOCKED) && pt_module->IsLocked() )
            continue;

        /* calcul de priorite: la priorite est donnee a la couche
         *  d'appartenance du module et a la couche cuivre si le module
         *  est sur couche serigr,adhesive cuivre, a la couche cmp si le module
         *  est sur couche serigr,adhesive composant */
        layer = pt_module->GetLayer();

        if( layer==ADHESIVE_N_CU || layer==SILKSCREEN_N_CU )
            layer = COPPER_LAYER_N;

        else if( layer==ADHESIVE_N_CMP || layer==SILKSCREEN_N_CMP )
            layer = CMP_N;
        
        /* Localisation: test des dimensions minimales, choix du meilleur candidat */

        /* calcul des dimensions du cadre :*/
        lx = pt_module->m_BoundaryBox.GetWidth();
        ly = pt_module->m_BoundaryBox.GetHeight();
        
        if( ( (PCB_SCREEN*) ActiveScreen )->m_Active_Layer == layer )
        {
            if( MIN( lx, ly ) <= min_dim )
            {
                /* meilleure empreinte localisee sur couche active */
                module  = pt_module; 
                min_dim = MIN( lx, ly );
            }
        }
        else if( !(typeloc & MATCH_LAYER)
            && ( !(typeloc & VISIBLE_ONLY) || IsModuleLayerVisible( layer ) ) )
        {
            if( MIN( lx, ly ) <= alt_min_dim )
            {
                /* meilleure empreinte localisee sur autres couches */
                Altmodule   = pt_module;
                alt_min_dim = MIN( lx, ly );
            }
        }
    }

    if( module )
    {
        return module;
    }

    if( Altmodule )
    {
        return Altmodule;
    }

    return NULL;
}


/*****************************************************************************/
TEXTE_MODULE* LocateTexteModule( BOARD* Pcb, MODULE** PtModule, int typeloc )
/*****************************************************************************/

/* localisation du texte pointe par la souris (texte sur empreinte)
 * 
 *  si * PtModule == NULL; recherche sur tous les modules
 *  sinon sur le module pointe par module
 * 
 *  retourne
 *  - pointeur sur le texte localise ( ou NULL )
 *  - si Ptmodule != NULL: pointeur sur module module ( non modifie sinon )
 * 
 *  if typeloc has the flag VISIBLE_ONLY set, only footprints which are 
 * "visible" are considered
 */
{
    EDA_BaseStruct* PtStruct;
    TEXTE_MODULE*   pt_txt_mod;
    MODULE*         module;
    wxPoint         ref_pos;

    ref_pos = RefPos( typeloc );

    module = *PtModule;
    if( module == NULL )
    {
        module = Pcb->m_Modules;
    }

    for( ; module != NULL; module = (MODULE*) module->Pnext )
    {
        int layer = module->GetLayer();
        if( layer==ADHESIVE_N_CU || layer==SILKSCREEN_N_CU )
            layer = COPPER_LAYER_N;
        else if( layer==ADHESIVE_N_CMP || layer==SILKSCREEN_N_CMP )
            layer = CMP_N;

        if( typeloc & VISIBLE_ONLY )
        {
            if( !IsModuleLayerVisible( layer ) )
                continue;
        }

        if( typeloc & MATCH_LAYER )
        {
            if( ( (PCB_SCREEN*) ActiveScreen )->m_Active_Layer != layer )
                continue;
        }

        // hit-test the reference text
        pt_txt_mod = module->m_Reference;
        if( pt_txt_mod->HitTest( ref_pos ) )
        {
            if( PtModule )
                *PtModule = module;
            return pt_txt_mod;
        }

        // hit-test the value text
        pt_txt_mod = module->m_Value;
        if( pt_txt_mod->HitTest( ref_pos ) )
        {
            if( PtModule )
                *PtModule = module;
            return pt_txt_mod;
        }

        // hit-test any other texts
        PtStruct = module->m_Drawings;
        for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext )
        {
            if( PtStruct->Type() != TYPETEXTEMODULE )
                continue;
            
            pt_txt_mod = (TEXTE_MODULE*) PtStruct;
            if( pt_txt_mod->HitTest( ref_pos ) )
            {
                if( PtModule )
                    *PtModule = module;
                return pt_txt_mod;
            }
        }

        if( *PtModule != NULL )
            break;                     /* Recherche limitee a 1 seul module */
    }

    return NULL;
}



/******************************************************************/
inline bool IsPointsAreNear(wxPoint & p1, wxPoint & p2, int max_dist)
/******************************************************************/
/*
return true if the dist between p1 and p2 < max_dist
Currently in test (currently rasnest algos work only if p1 == p2)
*/
{
#if 0	// Do not change it: does not work
{	
int dist;
	dist = abs(p1.x - p2.x) + abs (p1.y - p2.y);
	dist *= 7;
	dist /= 10;
	if ( dist < max_dist ) return true;
}
#else
	if ( p1 == p2 ) return true;
#endif
	return false;
}

/**************************************************************/
TRACK* Locate_Piste_Connectee( TRACK* PtRefSegm, TRACK* pt_base,
                               TRACK* pt_lim, int extr )
/**************************************************************/

/** Search for the track (or via) segment which is connected to the track segment PtRefSegm
 *  if extr == START, the starting track segment PtRefSegm is used to locate a connected segment
 *  if extr == END, the ending track segment PtRefSegm is used
 *  The test connection consider only end track segments
 * 
 *  Search is made from  pt_base to pt_lim (in the track linked list)
 *  if pt_lim == NULL,  the search  is made from  pt_base to the end of list
 *
 *  In order to have a fast computation time:
 *  a first search is made considering only the +/- 50 next door neightbour of PtRefSegm.
 *  if no track is found : the entire list is tested
 *
 *  @param PtRefSegm = reference segment
 *  @param pt_base = lower limit for search
 *  @param pt_lim = upper limit for search (can be NULL)
 *  @param extr = START or END = end of ref track segment to use in tests
 */
{
	#define NEIGHTBOUR_COUNT_MAX 50
    TRACK*  PtSegmB, * PtSegmN;
    int     Reflayer;
    wxPoint pos_ref;
    int     ii;
	int     max_dist;

    if( extr == START )
        pos_ref = PtRefSegm->m_Start;
    else
        pos_ref = PtRefSegm->m_End;

    Reflayer = PtRefSegm->ReturnMaskLayer();

    /* 1ere passe */
    PtSegmB = PtSegmN = PtRefSegm;

    for( ii = 0; ii < NEIGHTBOUR_COUNT_MAX; ii++ )
    {
        if( (PtSegmN == NULL) && (PtSegmB == NULL) )
            break;

        if( PtSegmN )
        {
            if( PtSegmN->GetState( BUSY | DELETED ) )
                goto suite;
            if( PtSegmN == PtRefSegm )
                goto suite;

			/* max_dist is the max distance between 2 tack ends which ensure a copper continuty */
			max_dist = (PtSegmN->m_Width + PtRefSegm->m_Width)/2;

            if( IsPointsAreNear(pos_ref, PtSegmN->m_Start, max_dist) )
            {       /* Test des couches */
                if( Reflayer & PtSegmN->ReturnMaskLayer() )
                    return PtSegmN;
            }

            if( IsPointsAreNear(pos_ref, PtSegmN->m_End, max_dist) )
            {       /* Test des couches */
                if( Reflayer & PtSegmN->ReturnMaskLayer() )
                    return PtSegmN;
            }
suite:
            if( PtSegmN == pt_lim )
                PtSegmN = NULL;
            else
                PtSegmN = (TRACK*) PtSegmN->Pnext;
        }

        if( PtSegmB )
        {
            if( PtSegmB->GetState( BUSY | DELETED ) )
                goto suite1;
            if( PtSegmB == PtRefSegm )
                goto suite1;

			max_dist = (PtSegmB->m_Width + PtRefSegm->m_Width)/2;

            if( IsPointsAreNear(pos_ref, PtSegmB->m_Start, max_dist) )
            {       /* Test des couches */
                if( Reflayer & PtSegmB->ReturnMaskLayer() )
                    return PtSegmB;
            }

            if( IsPointsAreNear(pos_ref, PtSegmB->m_End, max_dist) )
            {       /* Test des couches */
                if( Reflayer & PtSegmB->ReturnMaskLayer() )
                    return PtSegmB;
            }
suite1:
            if( PtSegmB == pt_base )
                PtSegmB = NULL;
            else if( PtSegmB->Type() != TYPEPCB )
                PtSegmB = (TRACK*) PtSegmB->Pback;
            else
                PtSegmB = NULL;
        }
    }

    /* Recherche generale */
    for( PtSegmN = pt_base; PtSegmN != NULL; PtSegmN = (TRACK*) PtSegmN->Pnext )
    {
        if( PtSegmN->GetState( DELETED | BUSY ) )
        {
            if( PtSegmN == pt_lim )
                break;
            continue;
        }
        if( PtSegmN == PtRefSegm )
        {
            if( PtSegmN == pt_lim )
                break;
            continue;
        }


		max_dist = (PtSegmN->m_Width + PtRefSegm->m_Width)/2;

        if( IsPointsAreNear(pos_ref,PtSegmN->m_Start, max_dist) )
        {       /* Test des couches */
            if( Reflayer & PtSegmN->ReturnMaskLayer() )
                return PtSegmN;
        }

        if( IsPointsAreNear(pos_ref, PtSegmN->m_End, max_dist) )
        {       /* Test des couches */
            if( Reflayer & PtSegmN->ReturnMaskLayer() )
                return PtSegmN;
        }
        if( PtSegmN == pt_lim )
            break;
    }

    return NULL;
}


/****************************************************************************/
/* TRACK *Locate_Pistes(TRACK * start_adresse, int MasqueLayer,int typeloc) */
/* TRACK *Locate_Pistes(TRACK * start_adresse, int ref_pos.x, int ref_pos.y,*/
/*                                      int MasqueLayer)                    */
/****************************************************************************/

/*
 *  1 -  routine de localisation du segment de piste pointe par la souris.
 *  2 -  routine de localisation du segment de piste pointe par le point
 *          ref_pos.x , ref_pos.y.r
 * 
 *  La recherche commence a l'adresse start_adresse
 */

TRACK* Locate_Pistes( TRACK* start_adresse, int MasqueLayer, int typeloc )
{
    wxPoint ref_pos = RefPos( typeloc );

    return Locate_Pistes( start_adresse, ref_pos, MasqueLayer );
}


TRACK* Locate_Pistes( TRACK* start_adresse, const wxPoint& ref_pos, int MasqueLayer )
{
    for( TRACK* Track = start_adresse;   Track;  Track = (TRACK*) Track->Pnext )
    {
        if( Track->GetState( BUSY | DELETED ) )
            continue;
        
        if( (g_DesignSettings.m_LayerColor[Track->GetLayer()] & ITEM_NOT_SHOW) )
            continue;

        if( Track->Type() == TYPEVIA ) /* VIA rencontree */
        {
            if( Track->HitTest( ref_pos ) )
                return Track;
        }
        else
        {
            if( MasqueLayer != -1 )
                if( (g_TabOneLayerMask[Track->GetLayer()] & MasqueLayer) == 0 )
                    continue;   /* Segments sur couches differentes */
                
            if( Track->HitTest( ref_pos ) )
                return Track;
        }
    }
    
    return NULL;
}


/****************************************************************/
/* TRACK *  Locate_Zone(TRACK * start_adresse, int layer,    */
/*                                          int typeloc)        */
/* TRACK *  Locate_Zone(TRACK * start_adresse,              */
/*                                      const wxPoint & ref_pos, */
/*                                      int layer)              */
/****************************************************************/

/*
 *  1 -  routine de localisation du segment de zone pointe par la souris.
 *  2 -  routine de localisation du segment de zone pointe par le point
 *          ref_pos.x , ref_pos.y.r
 * 
 *  Si layer == -1 , le tst de la couche n'est pas fait
 * 
 *  La recherche commence a l'adresse start_adresse
 */
TRACK* Locate_Zone( TRACK* start_adresse, int layer, int typeloc )
{
    wxPoint ref_pos = RefPos( typeloc );

    return Locate_Zone( start_adresse, ref_pos, layer );
}


TRACK* Locate_Zone( TRACK* start_adresse, const wxPoint& ref_pos, int layer )
{
    for( TRACK* Zone = start_adresse;  Zone;   Zone = (TRACK*) Zone->Pnext )
    {
        if( (layer != -1) && (Zone->GetLayer() != layer) )
            continue;
        
        if( Zone->HitTest( ref_pos ) )
            return Zone;
    }

    return NULL;
}


/************************************************************************************/
TEXTE_PCB* Locate_Texte_Pcb( EDA_BaseStruct* PtStruct, int LayerSearch, int typeloc )
/************************************************************************************/

/* localisation des inscriptions sur le Pcb:
 *  entree : EDA_BaseStruct pointeur sur le debut de la zone de recherche
 *  retour : pointeur sur la description du texte localise
 */
{
    wxPoint ref = RefPos( typeloc );

    for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext )
    {
        if( PtStruct->Type() != TYPETEXTE )
            continue;
        
        TEXTE_PCB* pt_txt_pcb = (TEXTE_PCB*) PtStruct;
        
        if( pt_txt_pcb->GetLayer() == LayerSearch )
        {
            if( pt_txt_pcb->HitTest( ref ) )
            {
                return pt_txt_pcb;
            }
        }
    }

    return NULL;
}



/*******************************************************************************/
D_PAD* Fast_Locate_Pad_Connecte( BOARD* Pcb, const wxPoint& ref_pos, int masque_layer )
/*******************************************************************************/

/* Routine cherchant le pad de centre px,py,
 *  sur la couche indiquee par masque_layer (bit a bit)
 *  ( extremite de piste )
 *  La liste des pads doit deja exister.
 * 
 *  retourne :
 *      NULL si pas de pad localise.
 *      pointeur sur la structure descr_pad correspondante si pad trouve
 *      (bonne position ET bonne couche).
 */
{
    D_PAD*     pad;
    LISTE_PAD* ptr_pad, * lim;

    lim = (LISTE_PAD*) Pcb->m_Pads + Pcb->m_NbPads;
    for( ptr_pad = (LISTE_PAD*) Pcb->m_Pads; ptr_pad < lim; ptr_pad++ )
    {
        pad = *ptr_pad;
        if( pad->m_Pos != ref_pos )
            continue;

        /* Pad peut-etre trouve ici : il doit etre sur la bonne couche */
        if( pad->m_Masque_Layer & masque_layer )
            return pad;
    }

    return NULL;
}


/***********************************************************************************/
TRACK* Fast_Locate_Piste( TRACK* start_adr, TRACK* end_adr,
                          const wxPoint& ref_pos, int MaskLayer )
/***********************************************************************************/

/* Localiste le segment dont une extremite coincide avec le point x,y
 *  sur les couches donnees par masklayer
 *  la recherche se fait de l'adresse start_adr a end_adr
 *  si end_adr = NULL, recherche jusqu'a la fin de la liste
 *  Les segments de piste marques avec le flag DELETED ne sont pas
 *  pris en compte
 */
{
    TRACK* PtSegm;

    if( start_adr == NULL )
        return NULL;

    for( PtSegm = start_adr; PtSegm != NULL; PtSegm = (TRACK*) PtSegm->Pnext )
    {
        if( PtSegm->GetState( DELETED | BUSY ) == 0 )
        {
            if( ref_pos == PtSegm->m_Start )
            {       
                /* Test des couches */
                if( MaskLayer & PtSegm->ReturnMaskLayer() )
                    return PtSegm;
            }

            if( ref_pos == PtSegm->m_End )
            {       
                /* Test des couches */
                if( MaskLayer & PtSegm->ReturnMaskLayer() )
                    return PtSegm;
            }
        }
        if( PtSegm == end_adr )
            break;
    }

    return NULL;
}


/*******************************************************************/
TRACK* Fast_Locate_Via( TRACK* start_adr, TRACK* end_adr,
                        const wxPoint& pos, int MaskLayer )
/*******************************************************************/

/* Localise la via de centre le point x,y , sur les couches donnees
 *  par masklayer
 *  la recherche se fait de l'adresse start_adr a end_adr.
 *  si end_adr = NULL, recherche jusqu'a la fin de la liste
 *  les vias dont le parametre State a le bit DELETED ou BUSY = 1 sont ignorees
 */
{
    TRACK* PtSegm;

    for( PtSegm = start_adr; PtSegm != NULL; PtSegm = (TRACK*) PtSegm->Pnext )
    {
        if( PtSegm->Type() == TYPEVIA )
        {
            if( pos == PtSegm->m_Start )
            {
                if( PtSegm->GetState( BUSY | DELETED ) == 0 )
                {    
                    /* Test des couches */
                    if( MaskLayer & PtSegm->ReturnMaskLayer() )
                        return PtSegm;
                }
            }
        }
        if( PtSegm == end_adr )
            break;
    }

    return NULL;
}


/***********************************************************************/
MIREPCB* Locate_MirePcb( BOARD_ITEM* PtStruct, int LayerSearch, int typeloc )
/***********************************************************************/

/* Search for a photo target
 */
{
    wxPoint ref_pos;/* coord du point de localisation */

    if( PtStruct == NULL )
        return NULL;

    ref_pos = RefPos( typeloc );

    for( ; PtStruct != NULL; PtStruct = PtStruct->Next() )
    {
        if( PtStruct->Type() != TYPEMIRE )
            continue;

        if( LayerSearch != -1  &&  PtStruct->GetLayer() != LayerSearch )
            continue;

        if( PtStruct->HitTest( ref_pos ) )
            break;
    }

    return (MIREPCB*) PtStruct;
}

