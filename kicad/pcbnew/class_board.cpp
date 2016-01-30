/*******************************************/
/* class_board.cpp - BOARD class functions */
/*******************************************/
#include "fctsys.h"
#include "common.h"

#include "pcbnew.h"


/* This is an odd place for this, but cvpcb won't link if it is
   in class_board_item.cpp like I first tried it.
*/
wxPoint  BOARD_ITEM::ZeroOffset(0,0);



// define SCH_ITEM::Place() (defined and used in eeschema but not for pcbnew)
// this is an ugly workaround to a linking problem in debug mode
// which needs to define  SCH_ITEM::Place() when not really used.
#include "sch_item_struct.h"
void SCH_ITEM::Place( WinEDA_SchematicFrame* frame, wxDC* DC )
{
}



/*****************/
/* Class BOARD: */
/*****************/

/* Constructor */
BOARD::BOARD( EDA_BaseStruct* parent, WinEDA_BasePcbFrame* frame ) :
    BOARD_ITEM( (BOARD_ITEM*) parent, TYPEPCB )
{
    m_PcbFrame   = frame;
    m_Status_Pcb = 0;                   // Mot d'etat: Bit 1 = Chevelu calcule
    m_NbNets = 0;                       // Nombre de nets (equipotentielles)
    m_BoardSettings = &g_DesignSettings;
    m_NbPads  = 0;                      // nombre total de pads
    m_NbNodes = 0;                      // nombre de pads connectes
    m_NbLinks = 0;                      // nombre de chevelus (donc aussi nombre
                                        // minimal de pistes a tracer
    m_NbSegmTrack = 0;                  // nombre d'elements de type segments de piste
    m_NbSegmZone  = 0;                  // nombre d'elements de type segments de zone
    m_NbNoconnect = 0;                  // nombre de chevelus actifs
    m_NbLoclinks  = 0;                  // nb ratsnest local

    m_Drawings = NULL;                  // pointeur sur liste drawings
    m_Modules  = NULL;                  // pointeur sur liste zone modules
    m_Equipots = NULL;                  // pointeur liste zone equipot
    m_Track    = NULL;                  // pointeur relatif zone piste
    m_Zone             = NULL;          // pointeur tableau zone zones de cuivre
    m_Pads             = NULL;          // pointeur liste d'acces aux pads
    m_Ratsnest         = NULL;          // pointeur liste rats
    m_LocalRatsnest    = NULL;          // pointeur liste rats local
    m_CurrentZoneContour = NULL;        // This ZONE_CONTAINER handle the zone contour cuurently in progress
                                        // de determination des contours de zone

    for( int layer=0; layer<NB_COPPER_LAYERS;  ++layer )
    {
        m_Layer[layer].m_Name = ReturnPcbLayerName( layer, true );
        m_Layer[layer].m_Type = LT_SIGNAL;
    }
}


/***************/
/* Destructeur */
/***************/
BOARD::~BOARD()
{
    m_Drawings->DeleteStructList();
    m_Drawings = 0;

    m_Modules->DeleteStructList();
    m_Modules = 0;

    m_Equipots->DeleteStructList();
    m_Equipots = 0;

    m_Track->DeleteStructList();
    m_Track = 0;

    m_Zone->DeleteStructList();
    m_Zone = 0;

    MyFree( m_Pads );
    m_Pads = 0;

    MyFree( m_Ratsnest );
    m_Ratsnest = 0;

    MyFree( m_LocalRatsnest );
    m_LocalRatsnest = 0;

    DeleteMARKERs();
    DeleteZONEOutlines();

    delete m_CurrentZoneContour;
    m_CurrentZoneContour = NULL;
}


wxString BOARD::GetLayerName( int aLayerIndex ) const
{
    // copper layer names are stored in the BOARD.
    if( (unsigned) aLayerIndex < (unsigned) GetCopperLayerCount()
       || aLayerIndex == LAST_COPPER_LAYER )
    {
        // default names were set in BOARD::BOARD() but they may be
        // over-ridden by BOARD::SetLayerName()
        return m_Layer[aLayerIndex].m_Name;
    }

    return ReturnPcbLayerName( aLayerIndex, true );
}


bool BOARD::SetLayerName( int aLayerIndex, const wxString& aLayerName )
{
    if( (unsigned) aLayerIndex < (unsigned) GetCopperLayerCount()
       || aLayerIndex==LAST_COPPER_LAYER )
    {
        if( aLayerName == wxEmptyString  || aLayerName.Len() > 20 )
            return false;

        // no quote chars in the name allowed
        if( aLayerName.Find( wxChar('"') ) != wxNOT_FOUND )
            return false;

        // ensure unique-ness of layer names
        for( int layer=0;  layer<GetCopperLayerCount() || layer==LAST_COPPER_LAYER;  )
        {
            if( layer!=aLayerIndex && aLayerName == m_Layer[layer].m_Name )
                return false;

            if( ++layer == GetCopperLayerCount() )
                layer = LAST_COPPER_LAYER;
        }

        m_Layer[aLayerIndex].m_Name = aLayerName;

        // replace any spaces with underscores
        m_Layer[aLayerIndex].m_Name.Replace( wxT(" "), wxT("_") );

        return true;
    }

    return false;
}


LAYER_T BOARD::GetLayerType( int aLayerIndex ) const
{
    if( (unsigned) aLayerIndex < (unsigned) GetCopperLayerCount() )
        return m_Layer[aLayerIndex].m_Type;
    return LT_SIGNAL;
}


bool BOARD::SetLayerType( int aLayerIndex, LAYER_T aLayerType )
{
    if( (unsigned) aLayerIndex < (unsigned) GetCopperLayerCount() )
    {
        m_Layer[aLayerIndex].m_Type = aLayerType;
        return true;
    }
    return false;
}


const char* LAYER::ShowType( LAYER_T aType )
{
    const char* cp;

    switch( aType )
    {
    default:
    case LT_SIGNAL:     cp = "signal";      break;
    case LT_POWER:      cp = "power";       break;
    case LT_MIXED:      cp = "mixed";       break;
    case LT_JUMPER:     cp = "jumper";      break;
    }

    return cp;
}


LAYER_T LAYER::ParseType( const char* aType )
{
    if( strcmp( aType, "signal" ) == 0 )
        return LT_SIGNAL;
    else if( strcmp( aType, "power" ) == 0 )
        return LT_POWER;
    else if( strcmp( aType, "mixed" ) == 0 )
        return LT_MIXED;
    else if( strcmp( aType, "jumper" ) == 0 )
        return LT_JUMPER;
    else
        return LAYER_T(-1);
}


int BOARD::GetCopperLayerCount() const
{
    return m_BoardSettings->m_CopperLayerCount;
}


wxPoint& BOARD::GetPosition()
{
    static wxPoint dummy(0,0);
    return dummy;   // a reference
}


void BOARD::UnLink()
{
    /* Modification du chainage arriere */
    if( Pback )
    {
        if( Pback->Type() == TYPEPCB )
        {
            Pback->Pnext = Pnext;
        }
        else /* Le chainage arriere pointe sur la structure "Pere" */
        {
//			Pback-> = Pnext;
        }
    }

    /* Modification du chainage avant */
    if( Pnext )
        Pnext->Pback = Pback;

    Pnext = Pback = NULL;
}


void BOARD::Add( BOARD_ITEM* aBoardItem, int aControl )
{
    if ( aBoardItem == NULL )
    {
        wxFAIL_MSG( wxT("BOARD::Add() param error: aBoardItem NULL") );
        return;
    }

    switch( aBoardItem->Type() )
    {
    // this one uses a vector
    case TYPEMARKER:
        aBoardItem->m_Parent = this;
        m_markers.push_back( (MARKER*) aBoardItem );
        break;

    // this one uses a vector
    case TYPEZONE_CONTAINER:
        aBoardItem->m_Parent = this;
        m_ZoneDescriptorList.push_back( (ZONE_CONTAINER*) aBoardItem );
        break;

    case TYPETRACK:
    case TYPEVIA:
        {
            TRACK* insertAid = ((TRACK*)aBoardItem)->GetBestInsertPoint( this );
            ((TRACK*)aBoardItem)->Insert( this, insertAid );
        }
        break;

    case TYPEMODULE:
        // this is an insert, not an append which may also be needed.
        {
            aBoardItem->Pback = this;
            BOARD_ITEM* next = m_Modules;
            aBoardItem->Pnext = next;
            if( next )
                next->Pback = aBoardItem;
            m_Modules = (MODULE*) aBoardItem;
        }
        break;

    // other types may use linked list
    default:
        wxFAIL_MSG( wxT("BOARD::Add() needs work") );
    }
}


void BOARD::Delete( BOARD_ITEM* aBoardItem )
{
    if ( aBoardItem == NULL ) return;

    switch( aBoardItem->Type() )
    {
    case TYPEMARKER:    // this one uses a vector
        // find the item in the vector, then delete then erase it.
        for( unsigned i=0;  i<m_markers.size();  ++i )
        {
            if( m_markers[i] == (MARKER*) aBoardItem )
            {
                DeleteMARKER( i );
                break;
            }
        }
        break;

    case TYPEZONE_CONTAINER:    // this one uses a vector
        // find the item in the vector, then delete then erase it.
        for( unsigned i=0;  i<m_ZoneDescriptorList.size();  ++i )
        {
            if( m_ZoneDescriptorList[i] == (ZONE_CONTAINER*) aBoardItem )
            {
                delete m_ZoneDescriptorList[i];
                m_ZoneDescriptorList.erase(m_ZoneDescriptorList.begin() + i);
                break;
            }
        }
        break;

    // other types may use linked list
    default:
        wxFAIL_MSG( wxT("BOARD::Delete() needs work") );
    }
}


void BOARD::DeleteMARKER( int aIndex )
{
    if( (unsigned) aIndex < m_markers.size() )
    {
        delete m_markers[aIndex];
        m_markers.erase( m_markers.begin() + aIndex );
    }
}


void BOARD::DeleteMARKERs()
{
    // the vector does not know how to delete the MARKER, it holds pointers
    for( unsigned i=0;  i<m_markers.size();  ++i )
        delete m_markers[i];

    m_markers.clear();
}

void BOARD::DeleteZONEOutlines()
{
    // the vector does not know how to delete the ZONE Outlines, it holds pointers
    for( unsigned i=0;  i<m_ZoneDescriptorList.size();  ++i )
        delete m_ZoneDescriptorList[i];

    m_ZoneDescriptorList.clear();
}


/* Calculate the track segment count */
int BOARD::GetNumSegmTrack()
{
    TRACK* CurTrack = m_Track;
    int    ii = 0;

    for( ; CurTrack != NULL; CurTrack = CurTrack->Next() )
        ii++;

    m_NbSegmTrack = ii;
    return ii;
}


/* Calculate the zone segment count */
int BOARD::GetNumSegmZone()
{
    TRACK* CurTrack = m_Zone;
    int    ii = 0;

    for( ; CurTrack != NULL; CurTrack = CurTrack->Next() )
        ii++;

    m_NbSegmZone = ii;
    return ii;
}


// return the unconnection count
int BOARD::GetNumNoconnect()
{
    return m_NbNoconnect;
}


// return the active pad count ( pads with a netcode > 0 )
int BOARD::GetNumNodes()
{
    return m_NbNodes;
}


/***********************************/
bool BOARD::ComputeBoundaryBox()
/***********************************/

/** Function ComputeBoundaryBox()
 * Calculate the bounding box of the board
 *  This box contains pcb edges, pads , vias and tracks
 *  Update m_PcbBox member
 *
 *  @return 0 for an empty board (no items), else 1
 */
{
    int             rayon, cx, cy, d, xmin, ymin, xmax, ymax;
    bool            Has_Items = FALSE;
    EDA_BaseStruct* PtStruct;
    DRAWSEGMENT*    ptr;
    TRACK*          Track;

    xmin = ymin = 0x7FFFFFFFl;
    xmax = ymax = -0x7FFFFFFFl;

    /* Analyse PCB edges*/
    PtStruct = m_Drawings;
    for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext )
    {
        if( PtStruct->Type() != TYPEDRAWSEGMENT )
            continue;

        ptr = (DRAWSEGMENT*) PtStruct;
        d   = (ptr->m_Width / 2) + 1;

        if( ptr->m_Shape == S_CIRCLE )
        {
            cx        = ptr->m_Start.x; cy = ptr->m_Start.y;
            rayon     = (int) hypot( (double) (ptr->m_End.x - cx), (double) (ptr->m_End.y - cy) );
            rayon    += d;
            xmin      = MIN( xmin, cx - rayon );
            ymin      = MIN( ymin, cy - rayon );
            xmax      = MAX( xmax, cx + rayon );
            ymax      = MAX( ymax, cy + rayon );
            Has_Items = TRUE;
        }
        else
        {
            cx        = MIN( ptr->m_Start.x, ptr->m_End.x );
            cy        = MIN( ptr->m_Start.y, ptr->m_End.y );
            xmin      = MIN( xmin, cx - d );
            ymin      = MIN( ymin, cy - d );
            cx        = MAX( ptr->m_Start.x, ptr->m_End.x );
            cy        = MAX( ptr->m_Start.y, ptr->m_End.y );
            xmax      = MAX( xmax, cx + d );
            ymax      = MAX( ymax, cy + d );
            Has_Items = TRUE;
        }
    }

    /* Analyse footprints  */
    MODULE* module = m_Modules;
    for( ; module != NULL; module = (MODULE*) module->Pnext )
    {
        Has_Items = TRUE;
        xmin = MIN( xmin, ( module->m_Pos.x + module->m_BoundaryBox.GetX() ) );
        ymin = MIN( ymin, ( module->m_Pos.y + module->m_BoundaryBox.GetY() ) );
        xmax = MAX( xmax, module->m_Pos.x + module->m_BoundaryBox.GetRight() );
        ymax = MAX( ymax, module->m_Pos.y + module->m_BoundaryBox.GetBottom() );

        D_PAD* pt_pad = module->m_Pads;
        for( ; pt_pad != NULL; pt_pad = (D_PAD*) pt_pad->Pnext )
        {
            const wxPoint& pos = pt_pad->GetPosition();

            d    = pt_pad->m_Rayon;
            xmin = MIN( xmin, pos.x - d );
            ymin = MIN( ymin, pos.y - d );
            xmax = MAX( xmax, pos.x + d );
            ymax = MAX( ymax, pos.y + d );
        }
    }

    /* Analyse track and zones */
    for( Track = m_Track; Track != NULL; Track = (TRACK*) Track->Pnext )
    {
        d         = (Track->m_Width / 2) + 1;
        cx        = MIN( Track->m_Start.x, Track->m_End.x );
        cy        = MIN( Track->m_Start.y, Track->m_End.y );
        xmin      = MIN( xmin, cx - d );
        ymin      = MIN( ymin, cy - d );
        cx        = MAX( Track->m_Start.x, Track->m_End.x );
        cy        = MAX( Track->m_Start.y, Track->m_End.y );
        xmax      = MAX( xmax, cx + d );
        ymax      = MAX( ymax, cy + d );
        Has_Items = TRUE;
    }

    for( Track = m_Zone; Track != NULL; Track = (TRACK*) Track->Pnext )
    {
        d         = (Track->m_Width / 2) + 1;
        cx        = MIN( Track->m_Start.x, Track->m_End.x );
        cy        = MIN( Track->m_Start.y, Track->m_End.y );
        xmin      = MIN( xmin, cx - d );
        ymin      = MIN( ymin, cy - d );
        cx        = MAX( Track->m_Start.x, Track->m_End.x );
        cy        = MAX( Track->m_Start.y, Track->m_End.y );
        xmax      = MAX( xmax, cx + d );
        ymax      = MAX( ymax, cy + d );
        Has_Items = TRUE;
    }

    if( !Has_Items && m_PcbFrame )
    {
        if( m_PcbFrame->m_Draw_Sheet_Ref )
        {
            xmin = ymin = 0;
            xmax = m_PcbFrame->GetScreen()->ReturnPageSize().x;
            ymax = m_PcbFrame->GetScreen()->ReturnPageSize().y;
        }
        else
        {
            xmin = -m_PcbFrame->GetScreen()->ReturnPageSize().x / 2;
            ymin = -m_PcbFrame->GetScreen()->ReturnPageSize().y / 2;
            xmax = m_PcbFrame->GetScreen()->ReturnPageSize().x / 2;
            ymax = m_PcbFrame->GetScreen()->ReturnPageSize().y / 2;
        }
    }

    m_BoundaryBox.SetX( xmin );
    m_BoundaryBox.SetY( ymin );
    m_BoundaryBox.SetWidth( xmax - xmin );
    m_BoundaryBox.SetHeight( ymax - ymin );

    return Has_Items;
}


// virtual, see pcbstruct.h
void BOARD::Display_Infos( WinEDA_DrawFrame* frame )
{
/* Affiche l'etat du PCB : nb de pads, nets , connexions.. */
#define POS_AFF_NBPADS      1
#define POS_AFF_NBVIAS      8
#define POS_AFF_NBNODES     16
#define POS_AFF_NBLINKS     24
#define POS_AFF_NBNETS      32
#define POS_AFF_NBCONNECT   40
#define POS_AFF_NBNOCONNECT 48

    int             nb_vias = 0, ii;
    EDA_BaseStruct* Struct;
    wxString        txt;

    frame->MsgPanel->EraseMsgBox();

    txt.Printf( wxT( "%d" ), m_NbPads );
    Affiche_1_Parametre( frame, POS_AFF_NBPADS, _( "Pads" ), txt, DARKGREEN );

    for( ii = 0, Struct = m_Track; Struct != NULL; Struct = Struct->Pnext )
    {
        ii++;
        if( Struct->Type() == TYPEVIA )
            nb_vias++;
    }

    txt.Printf( wxT( "%d" ), nb_vias );
    Affiche_1_Parametre( frame, POS_AFF_NBVIAS, _( "Vias" ), txt, DARKGREEN );

    txt.Printf( wxT( "%d" ), GetNumNodes() );
    Affiche_1_Parametre( frame, POS_AFF_NBNODES, _( "Nodes" ), txt, DARKCYAN );

    txt.Printf( wxT( "%d" ), m_NbLinks );
    Affiche_1_Parametre( frame, POS_AFF_NBLINKS, _( "Links" ), txt, DARKGREEN );

    txt.Printf( wxT( "%d" ), m_NbNets );
    Affiche_1_Parametre( frame, POS_AFF_NBNETS, _( "Nets" ), txt, RED );

    txt.Printf( wxT( "%d" ), m_NbLinks - GetNumNoconnect() );
    Affiche_1_Parametre( frame, POS_AFF_NBCONNECT, _( "Connect" ), txt, DARKGREEN );

    txt.Printf( wxT( "%d" ), GetNumNoconnect() );
    Affiche_1_Parametre( frame, POS_AFF_NBNOCONNECT, _( "NoConn" ), txt, BLUE );
}


// virtual, see pcbstruct.h
SEARCH_RESULT BOARD::Visit( INSPECTOR* inspector, const void* testData,
    const KICAD_T scanTypes[] )
{
    KICAD_T         stype;
    SEARCH_RESULT   result = SEARCH_CONTINUE;
    const KICAD_T*  p = scanTypes;
    bool            done=false;

#if 0 && defined(DEBUG)
    std::cout <<  GetClass().mb_str() << ' ';
#endif

    while( !done )
    {
        stype = *p;
        switch( stype )
        {
        case TYPEPCB:
            result = inspector->Inspect( this, testData );  // inspect me
            // skip over any types handled in the above call.
            ++p;
            break;

        /*  Instances of the requested KICAD_T live in a list, either one
            that I manage, or that my modules manage.  If it's a type managed
            by class MODULE, then simply pass it on to each module's
            MODULE::Visit() function by way of the
            IterateForward( m_Modules, ... ) call.
        */

        case TYPEMODULE:
        case TYPEPAD:
        case TYPETEXTEMODULE:
        case TYPEEDGEMODULE:
            // this calls MODULE::Visit() on each module.
            result = IterateForward( m_Modules, inspector, testData, p );
            // skip over any types handled in the above call.
            for(;;)
            {
                switch( stype = *++p )
                {
                case TYPEMODULE:
                case TYPEPAD:
                case TYPETEXTEMODULE:
                case TYPEEDGEMODULE:
                    continue;
                default:;
                }
                break;
            }
            break;

        case TYPEDRAWSEGMENT:
        case TYPETEXTE:
        case TYPECOTATION:
        case TYPEMIRE:
            result = IterateForward( m_Drawings, inspector, testData, p );
            // skip over any types handled in the above call.
            for(;;)
            {
                switch( stype = *++p )
                {
                case TYPEDRAWSEGMENT:
                case TYPETEXTE:
                case TYPECOTATION:
                case TYPEMIRE:
                    continue;
                default:;
                }
                break;
            }
                ;
            break;

#if 0   // both these are on same list, so we must scan it twice in order to get VIA priority,
        // using new #else code below.
        // But we are not using separte lists for TRACKs and SEGVIAs, because items are ordered (sortered) in the linked
        // list by netcode AND by physical distance:
        // when created, if a track or via is connected to an existing track or via, it is put in linked list
        // after this existing track or via
        // So usually, connected tracks or vias are grouped in this list
        // So the algorithm (used in rastnest computations) which computes the track connectivity is faster (more than 100 time regarding to
        // a non ordered list) because when it searchs for a connexion, first it tests the near (near in term of linked list) 50 items
        // from the current item (track or via) in test.
        // Usually, because of this sort, a connected item (if exists) is found.
        // If not found (and only in this case) an exhaustive (and time consumming) search is made,
        // but this case is statistically rare.
        case TYPEVIA:
        case TYPETRACK:
            result = IterateForward( m_Track, inspector, testData, p );
            // skip over any types handled in the above call.
            for(;;)
            {
                switch( stype = *++p )
                {
                case TYPEVIA:
                case TYPETRACK:
                    continue;
                default:;
                }
                break;
            }
            break;
#else
        case TYPEVIA:
            result = IterateForward( m_Track, inspector, testData, p );
            ++p;
            break;

        case TYPETRACK:
            result = IterateForward( m_Track, inspector, testData, p );
            ++p;
            break;
#endif

        case TYPEMARKER:
            // MARKERS are in the m_markers std::vector
            for( unsigned i=0;  i<m_markers.size();  ++i )
            {
                result = m_markers[i]->Visit( inspector, testData, p );
                if( result == SEARCH_QUIT )
                    break;
            }
            ++p;
            break;

        case TYPEZONE_CONTAINER:
            // TYPEZONE_CONTAINER are in the m_ZoneDescriptorList std::vector
            for( unsigned i=0;  i< m_ZoneDescriptorList.size();  ++i )
            {
                result = m_ZoneDescriptorList[i]->Visit( inspector, testData, p );
                if( result == SEARCH_QUIT )
                    break;
            }
            ++p;
            break;

        case PCB_EQUIPOT_STRUCT_TYPE:
            result = IterateForward( m_Equipots, inspector, testData, p );
            ++p;
            break;

        case TYPEZONE:
            result = IterateForward( m_Zone, inspector, testData, p );
            ++p;
            break;

        case TYPEZONE_UNUSED:	// Unused type
            break;

        default:        // catch EOT or ANY OTHER type here and return.
            done = true;
            break;
        }

        if( result == SEARCH_QUIT )
            break;
    }

    return result;
}


/*  now using PcbGeneralLocateAndDisplay(), but this remains a useful example
    of how the INSPECTOR can be used in a lightweight way.
// see pcbstruct.h
BOARD_ITEM* BOARD::FindPadOrModule( const wxPoint& refPos, int layer )
{
    class PadOrModule : public INSPECTOR
    {
    public:
        BOARD_ITEM*         found;
        int                 layer;
        int                 layer_mask;

        PadOrModule( int alayer ) :
            found(0), layer(alayer), layer_mask( g_TabOneLayerMask[alayer] )
        {}

        SEARCH_RESULT Inspect( EDA_BaseStruct* testItem, const void* testData )
        {
            BOARD_ITEM*     item   = (BOARD_ITEM*) testItem;
            const wxPoint&  refPos = *(const wxPoint*) testData;

            if( item->Type() == TYPEPAD )
            {
                D_PAD*  pad = (D_PAD*) item;
                if( pad->HitTest( refPos ) )
                {
                    if( layer_mask & pad->m_Masque_Layer )
                    {
                        found = item;
                        return SEARCH_QUIT;
                    }
                    else if( !found )
                    {
                        MODULE* parent = (MODULE*) pad->m_Parent;
                        if( IsModuleLayerVisible( parent->GetLayer() ) )
                            found = item;
                    }
                }
            }

            else if( item->Type() == TYPEMODULE )
            {
                MODULE* module = (MODULE*) item;

                // consider only visible modules
                if( IsModuleLayerVisible( module->GetLayer() ) )
                {
                    if( module->HitTest( refPos ) )
                    {
                        if( layer == module->GetLayer() )
                        {
                            found = item;
                            return SEARCH_QUIT;
                        }

                        // layer mismatch, save in case we don't find a
                        // future layer match hit.
                        if( !found )
                            found = item;
                    }
                }
            }
            return SEARCH_CONTINUE;
        }
    };

    PadOrModule inspector( layer );

    // search only for PADs first, then MODULES, and preferably a layer match
    static const KICAD_T scanTypes[] = { TYPEPAD, TYPEMODULE, EOT };

    // visit this BOARD with the above inspector
    Visit( &inspector, &refPos, scanTypes );

    return inspector.found;
}
*/


/**
 * Function FindNet
 * searches for a net with the given netcode.
 * @param anetcode The netcode to search for.
 * @return EQUIPOT* - the net or NULL if not found.
 */
EQUIPOT* BOARD::FindNet( int anetcode ) const
{
    // the first valid netcode is 1.
    // zero is reserved for "no connection" and is not used.
    if( anetcode > 0 )
    {
        for( EQUIPOT* net = m_Equipots;  net;  net=net->Next() )
        {
            if( net->GetNet() == anetcode )
                return net;
        }
    }
    return NULL;
}


/**
 * Function FindNet overlayed
 * searches for a net with the given name.
 * @param aNetname A Netname to search for.
 * @return EQUIPOT* - the net or NULL if not found.
 */
EQUIPOT* BOARD::FindNet( const wxString & aNetname ) const
{
    // the first valid netcode is 1.
    // zero is reserved for "no connection" and is not used.
    if( ! aNetname.IsEmpty() )
    {
        for( EQUIPOT* net = m_Equipots;  net;  net=net->Next() )
        {
            if( net->m_Netname == aNetname )
                return net;
        }
    }
    return NULL;
}


MODULE* BOARD::FindModuleByReference( const wxString& aReference ) const
{
    struct FindModule : public INSPECTOR
    {
        MODULE*  found;
        FindModule() : found(0)  {}

        // implement interface INSPECTOR
        SEARCH_RESULT Inspect( EDA_BaseStruct* item, const void* data )
        {
            MODULE*         module = (MODULE*) item;
            const wxString& ref = *(const wxString*) data;

            if( ref == module->GetReference() )
            {
                found = module;
                return SEARCH_QUIT;
            }
            return SEARCH_CONTINUE;
        }
    } inspector;

    // search only for MODULES
    static const KICAD_T scanTypes[] = { TYPEMODULE, EOT };

    // visit this BOARD with the above inspector
    BOARD* nonconstMe = (BOARD*) this;
    nonconstMe->Visit( &inspector, &aReference, scanTypes );

    return inspector.found;
}


/* Two sort functions used in BOARD::ReturnSortedNetnamesList */
// Sort nets by name
int s_SortByNames(const void * ptr1, const void * ptr2)
{
    EQUIPOT* item1 = * (EQUIPOT**) ptr1;
    EQUIPOT* item2 = * (EQUIPOT**) ptr2;
    return  item1->m_Netname.CmpNoCase(item2->m_Netname);
}

// Sort nets by decreasing pad count
int s_SortByNodes(const void * ptr1, const void * ptr2)
{
    EQUIPOT* item1 = * (EQUIPOT**) ptr1;
    EQUIPOT* item2 = * (EQUIPOT**) ptr2;
    if ( (item1->m_NbNodes - item2->m_NbNodes) != 0 )
        return  - (item1->m_NbNodes - item2->m_NbNodes);
    return  item1->m_Netname.CmpNoCase(item2->m_Netname);
}


/**
 * Function ReturnSortedNetnamesList
 * searches for a net with the given netcode.
 * @param aNames An array string to fill with net names.
 * @param aSort_Type : NO_SORT = no sort, ALPHA_SORT = sort by alphabetic order, PAD_CNT_SORT = sort by active pads count.
 * @return int - net names count.
 */
int BOARD::ReturnSortedNetnamesList( wxArrayString & aNames, const int aSort_Type)
{
    int NetCount = 0;
    int ii;
    EQUIPOT* net;

    /* count items to list and sort */
    for( net = m_Equipots; net;  net=net->Next() )
    {
        if ( net->m_Netname.IsEmpty() ) continue;
        NetCount++;
    }

    if ( NetCount == 0 ) return 0;

    /* Build the list */
    EQUIPOT* * net_ptr_list = (EQUIPOT* *) MyMalloc( NetCount * sizeof(* net_ptr_list) );
    for( ii = 0, net = m_Equipots; net; net=net->Next() )
    {
        if ( net->m_Netname.IsEmpty() ) continue;
        net_ptr_list[ii] = net;
        ii++;
    }

    /* sort the list */
    switch ( aSort_Type )
    {
        case NO_SORT : break;

        case ALPHA_SORT :
            qsort (net_ptr_list, NetCount, sizeof(EQUIPOT*), s_SortByNames);
            break;

        case PAD_CNT_SORT:
            qsort (net_ptr_list, NetCount, sizeof(EQUIPOT*), s_SortByNodes);
            break;
    }

    /* fill the given list */
    for( ii = 0; ii < NetCount; ii++ )
    {
        net = net_ptr_list[ii];
        aNames.Add(net->m_Netname);
    }

    MyFree(net_ptr_list);

    return NetCount;
}

/************************************/
bool BOARD::Save( FILE* aFile ) const
/************************************/
{
    bool        rc = false;
    BOARD_ITEM* item;

    // save the nets
    for( item = m_Equipots;  item;  item=item->Next() )
        if( !item->Save( aFile ) )
            goto out;

    // save the modules
    for( item = m_Modules;  item;  item=item->Next() )
        if( !item->Save( aFile ) )
            goto out;

    for( item = m_Drawings;  item;  item=item->Next() )
    {
        switch( item->Type() )
        {
        case TYPETEXTE:
        case TYPEDRAWSEGMENT:
        case TYPEMIRE:
        case TYPECOTATION:
            if( !item->Save( aFile ) )
                goto out;
            break;

        default:
            // future: throw exception here
#if defined(DEBUG)
            printf( "BOARD::Save() ignoring m_Drawings type %d\n", item->Type() );
#endif
            break;
        }
    }

    // do not save MARKERs, they can be regenerated easily

    // save the tracks & vias
    fprintf( aFile, "$TRACK\n" );
    for( item = m_Track;  item;  item=item->Next() )
        if( !item->Save( aFile ) )
            goto out;
    fprintf( aFile, "$EndTRACK\n" );

    // save the zones
    fprintf( aFile, "$ZONE\n" );
    for( item = m_Zone;  item;  item=item->Next() )
        if( !item->Save( aFile ) )
            goto out;
    fprintf( aFile, "$EndZONE\n" );

    // save the zone edges
    for( unsigned ii = 0; ii < m_ZoneDescriptorList.size(); ii++ )
    {
        ZONE_CONTAINER* edge_zone = m_ZoneDescriptorList[ii];
        edge_zone->Save( aFile );
    }


    if( fprintf( aFile, "$EndBOARD\n" ) != sizeof("$EndBOARD\n")-1 )
        goto out;

    rc = true;  // wrote all OK

out:
    return rc;
}

/***********************************************************************************************/
void BOARD::RedrawAreasOutlines(WinEDA_DrawPanel* panel, wxDC * aDC, int aDrawMode, int aLayer)
/***********************************************************************************************/
/**
 * Function RedrawAreasOutlines
 * Redraw all areas outlines on layer aLayer ( redraw all if aLayer < 0 )
 */
{
    if ( ! aDC ) return;

    for( int ii = 0; ii < GetAreaCount(); ii++ )
    {
        ZONE_CONTAINER* edge_zone = GetArea(ii);
        if( (aLayer < 0) || (aLayer == edge_zone->GetLayer()) )
            edge_zone->Draw( panel, aDC, aDrawMode );
    }
}



#if defined(DEBUG)

/**
 * Function Show
 * is used to output the object tree, currently for debugging only.
 * @param nestLevel An aid to prettier tree indenting, and is the level
 *          of nesting of this object within the overall tree.
 * @param os The ostream& to output to.
 */
void BOARD::Show( int nestLevel, std::ostream& os )
{
    BOARD_ITEM* p;

    // for now, make it look like XML:
    NestedSpace( nestLevel, os ) << '<' << GetClass().Lower().mb_str() << ">\n";

    // specialization of the output:
    NestedSpace( nestLevel+1, os ) << "<modules>\n";
    p = m_Modules;
    for( ; p; p = p->Next() )
        p->Show( nestLevel+2, os );
    NestedSpace( nestLevel+1, os ) << "</modules>\n";

    NestedSpace( nestLevel+1, os ) << "<pdrawings>\n";
    p = m_Drawings;
    for( ; p; p = p->Next() )
        p->Show( nestLevel+2, os );
    NestedSpace( nestLevel+1, os ) << "</pdrawings>\n";

    NestedSpace( nestLevel+1, os ) << "<nets>\n";
    p = m_Equipots;
    for( ; p; p = p->Next() )
        p->Show( nestLevel+2, os );
    NestedSpace( nestLevel+1, os ) << "</nets>\n";

    NestedSpace( nestLevel+1, os ) << "<tracks>\n";
    p = m_Track;
    for( ; p; p = p->Next() )
        p->Show( nestLevel+2, os );
    NestedSpace( nestLevel+1, os ) << "</tracks>\n";

    NestedSpace( nestLevel+1, os ) << "<zones>\n";
    p = m_Zone;
    for( ; p; p = p->Next() )
        p->Show( nestLevel+2, os );
    NestedSpace( nestLevel+1, os ) << "</zones>\n";
    /*
    NestedSpace( nestLevel+1, os ) << "<zone_container>\n";
    for( ZONE_CONTAINERS::iterator i=m_ZoneDescriptorList.begin();  i!=m_ZoneDescriptorList.end();  ++i )
        (*i)->Show( nestLevel+2, os );
    NestedSpace( nestLevel+1, os ) << "</zone_container>\n";
    */

    p = (BOARD_ITEM*) m_Son;
    for( ; p;  p = p->Next() )
    {
        p->Show( nestLevel+1, os );
    }

    NestedSpace( nestLevel, os ) << "</" << GetClass().Lower().mb_str() << ">\n";
}


#endif
