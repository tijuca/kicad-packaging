/***********************************/
/* Module de calcul de la Netliste */
/***********************************/

#include "fctsys.h"
#include "gr_basic.h"
#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "netlist.h" /* Definitions generales liees au calcul de netliste */
#include "protos.h"


/* Routines locales */
static void PropageNetCode( int OldNetCode, int NewNetCode, int IsBus );
static void SheetLabelConnection( ObjetNetListStruct* SheetLabel );
static int  ListeObjetConnection( WinEDA_SchematicFrame* frame,
                                  SCH_SCREEN* screen,
                                  ObjetNetListStruct* ObjNet );
static int  ConvertBusToMembers( ObjetNetListStruct* ObjNet );
static void PointToPointConnect( ObjetNetListStruct* RefObj, int IsBus,
                                 int start );
static void SegmentToPointConnect( ObjetNetListStruct* Jonction, int IsBus,
                                   int start );
static void LabelConnection( ObjetNetListStruct* Label );
static int  TriNetCode( const void* o1, const void* o2 );
static void ConnectBusLabels( ObjetNetListStruct* Label, int NbItems );
static void SetUnconnectedFlag( ObjetNetListStruct* ObjNet, int NbItems );
static int TriBySheet( const void* o1, const void* o2 );

/* Variable locales */
static int FirstNumWireBus, LastNumWireBus, RootBusNameLength;
static int LastNetCode, LastBusNetCode;
static int s_PassNumber;


#if defined(DEBUG)

const char* ShowType( NetObjetType aType )
{
    const char* ret;
    
    switch( aType )
    {
    case NET_SEGMENT:               ret = "segment";            break;
    case NET_BUS:                   ret = "bus";                break;
    case NET_JONCTION:              ret = "junction";           break;
    case NET_LABEL:                 ret = "label";              break;
    case NET_GLOBLABEL:             ret = "glabel";             break;
    case NET_BUSLABELMEMBER:        ret = "buslblmember";       break;
    case NET_GLOBBUSLABELMEMBER:    ret = "gbuslblmember";      break;
    case NET_SHEETBUSLABELMEMBER:   ret = "sbuslblmember";      break;
    case NET_SHEETLABEL:            ret = "sheetlabel";         break;
    case NET_PINLABEL:              ret = "pinlabel";           break;
    case NET_PIN:                   ret = "pin";                break;
    case NET_NOCONNECT:             ret = "noconnect";          break;
    default:                        ret = "??";                 break;
    }
    return ret;
}


void ObjetNetListStruct::Show( std::ostream& out, int ndx )
{
    out << "<netItem ndx=\"" << ndx << '"' << 
            " type=\"" << ShowType(m_Type) << '"' <<
            " netCode=\"" << GetNet() << '"' <<
            " sheet=\"" << m_SheetNumber << '"' <<
            ">\n";

    out << " <start " << m_Start << "/> <end " << m_End << "/>\n";

    if( m_Label )
        out << " <label>" << m_Label->mb_str() << "</label>\n";
    
    if( m_Comp )
        m_Comp->Show( 1, out );
    else
        out << " m_Comp==NULL\n";
    
    out << "</netItem>\n";
}

void dumpNetTable()
{
    for( int i=0;  i<g_NbrObjNet;  ++i )
    {
        g_TabObjNet[i].Show( std::cout, i );
    }
}
#endif


/***********************************************************************/
void FreeTabNetList( ObjetNetListStruct* TabNetItems, int NbrNetItems )
/***********************************************************************/

/*
 *  Routine de liberation memoire des tableaux utilises pour le calcul
 *  de la netliste
 *  TabNetItems = pointeur sur le tableau principal (liste des items )
 *  NbrNetItems = nombre d'elements
 */
{
    int i;

    /* Liberation memoire des strings du champ Label reserve par ConvertBusToMembers */
    for( i = 0; i < NbrNetItems; i++ )
    {
        switch( TabNetItems[i].m_Type )
        {
        case NET_PIN:
        case NET_SHEETLABEL:
        case NET_SEGMENT:
        case NET_JONCTION:
        case NET_BUS:
        case NET_LABEL:
        case NET_GLOBLABEL:
        case NET_PINLABEL:
        case NET_NOCONNECT:
            break;

        case NET_GLOBBUSLABELMEMBER:
        case NET_SHEETBUSLABELMEMBER:
        case NET_BUSLABELMEMBER:
            delete TabNetItems[i].m_Label;
            break;
        }
    }

    MyFree( TabNetItems );
}


/*****************************************************/
void* WinEDA_SchematicFrame::BuildNetListBase()
/*****************************************************/

/* Routine qui construit le tableau des elements connectes du projet
 *  met a jour:
 *      g_TabObjNet
 *      g_NbrObjNet
 */
{
    int                 NetNumber, SheetNumber;
    int                 i, istart, NetCode;
    SCH_SCREEN*         screen;
    wxString            msg;
    wxBusyCursor        Busy;

    NetNumber    = 1;
    s_PassNumber = 0;
    
    MsgPanel->EraseMsgBox();
    Affiche_1_Parametre( this, 1, _( "List" ), wxEmptyString, LIGHTRED );

    /* Build the screen list */
    EDA_ScreenList ScreenList( NULL );

    /* 1ere passe : Comptage du nombre d'objet de Net */
    g_NbrObjNet = 0;
    g_TabObjNet = NULL; /* Init pour le 1er passage dans ListeObjetConnection */

    /* Update the sheet number, sheet count and date and count nelist items */
    ScreenSch->SetModify();
    int kk = 1;
    for( screen = ScreenList.GetFirst(); screen != NULL; screen = ScreenList.GetNext() )
    {
        screen->m_SheetNumber   = kk++;
        screen->m_NumberOfSheet = ScreenList.GetCount();
        screen->m_Date = GenDate();
        g_NbrObjNet   += ListeObjetConnection( this, screen, NULL );
    }

    if( g_NbrObjNet == 0 )
    {
        DisplayError( this, _( "No component" ), 20 );
        return NULL;
    }

    i = sizeof(ObjetNetListStruct) * g_NbrObjNet;
    g_TabObjNet = (ObjetNetListStruct*) MyZMalloc( i );
    if( g_TabObjNet == NULL )
        return NULL;

    /* 2eme passe : Remplissage des champs des structures des objets de Net */

    s_PassNumber++;
    Affiche_1_Parametre( this, 1, _( "List" ), wxEmptyString, RED );
    
    screen = ScreenList.GetFirst(); 
    for( ObjetNetListStruct* tabObjNet = g_TabObjNet; 
        screen != NULL; screen = ScreenList.GetNext() )
    {
        tabObjNet += ListeObjetConnection( this, screen, tabObjNet );
    }

    Affiche_1_Parametre( this, -1, wxEmptyString, _( "Done" ), RED );

    msg.Printf( wxT( "%d" ), g_NbrObjNet );
    Affiche_1_Parametre( this, 8, _( "NbItems" ), msg, GREEN );

    /* Recherche des connections pour les Segments et les Pins */
    /* Tri du Tableau des objets de Net par Sheet */

    qsort( g_TabObjNet, g_NbrObjNet, sizeof(ObjetNetListStruct), TriBySheet );

    Affiche_1_Parametre( this, 18, _( "Conn" ), wxEmptyString, CYAN );

    SheetNumber = g_TabObjNet[0].m_SheetNumber;
    LastNetCode = LastBusNetCode = 1;
    
    for( i = istart = 0;  i<g_NbrObjNet;  i++ )
    {
        if( g_TabObjNet[i].m_SheetNumber != SheetNumber )
        {
            SheetNumber = g_TabObjNet[i].m_SheetNumber; 
            istart = i;
        }

        switch( g_TabObjNet[i].m_Type )
        {
        case NET_PIN:
        case NET_PINLABEL:
        case NET_SHEETLABEL:
        case NET_NOCONNECT:
            if( g_TabObjNet[i].GetNet() != 0 )
                break;                                      /* Deja connecte */

        case NET_SEGMENT:
            /* Controle des connexions type point a point ( Sans BUS ) */
            if( g_TabObjNet[i].GetNet() == 0 )
            {
                g_TabObjNet[i].SetNet( LastNetCode );
                LastNetCode++;
            }
            PointToPointConnect( g_TabObjNet + i, 0, istart );
            break;

        case NET_JONCTION:
            /* Controle des jonction , hors BUS */
            if( g_TabObjNet[i].GetNet() == 0 )
            {
                g_TabObjNet[i].SetNet( LastNetCode );
                LastNetCode++;
            }
            SegmentToPointConnect( g_TabObjNet + i, 0, istart );

            /* Controle des jonction , sur BUS */
            if( g_TabObjNet[i].m_BusNetCode == 0 )
            {
                g_TabObjNet[i].m_BusNetCode = LastBusNetCode;
                LastBusNetCode++;
            }
            SegmentToPointConnect( g_TabObjNet + i, ISBUS, istart );
            break;

        case NET_LABEL:
        case NET_GLOBLABEL:
            /* Controle des connexions type jonction ( Sans BUS ) */
            if( g_TabObjNet[i].GetNet() == 0 )
            {
                g_TabObjNet[i].SetNet( LastNetCode );
                LastNetCode++;
            }
            SegmentToPointConnect( g_TabObjNet + i, 0, istart );
            break;

        case NET_SHEETBUSLABELMEMBER:
            if( g_TabObjNet[i].m_BusNetCode != 0 )
                break;                                         /* Deja connecte */

        case NET_BUS:
            /* Controle des connexions type point a point mode BUS */
            if( g_TabObjNet[i].m_BusNetCode == 0 )
            {
                g_TabObjNet[i].m_BusNetCode = LastBusNetCode;
                LastBusNetCode++;
            }
            PointToPointConnect( g_TabObjNet + i, ISBUS, istart );
            break;

        case NET_BUSLABELMEMBER:
        case NET_GLOBBUSLABELMEMBER:
            /* Controle des connexions semblables a des sur BUS */
            if( g_TabObjNet[i].GetNet() == 0 )
            {
                g_TabObjNet[i].m_BusNetCode = LastBusNetCode;
                LastBusNetCode++;
            }
            SegmentToPointConnect( g_TabObjNet + i, ISBUS, istart );
            break;
        }
    }

   
#if 0 && defined(DEBUG)
    std::cout << "after sheet local\n";
    dumpNetTable();
#endif    


    Affiche_1_Parametre( this, -1, wxEmptyString, _( "Done" ), CYAN );

    /* Mise a jour des NetCodes des Bus Labels connectes par les Bus */
    ConnectBusLabels( g_TabObjNet, g_NbrObjNet );

    Affiche_1_Parametre( this, 26, _( "Labels" ), wxEmptyString, CYAN );

    /* Connections des groupes d'objets par labels identiques */
    for( i = 0;  i<g_NbrObjNet;  i++ )
    {
        switch( g_TabObjNet[i].m_Type )
        {
        case NET_PIN:
        case NET_SHEETLABEL:
        case NET_SEGMENT:
        case NET_JONCTION:
        case NET_BUS:
        case NET_NOCONNECT:
            break;

        case NET_LABEL:
        case NET_GLOBLABEL:
        case NET_PINLABEL:
        case NET_BUSLABELMEMBER:
        case NET_GLOBBUSLABELMEMBER:
            LabelConnection( g_TabObjNet + i );
            break;

        case NET_SHEETBUSLABELMEMBER:
            break;
        }
    }

#if 0 && defined(DEBUG)
    std::cout << "after sheet global\n";
    dumpNetTable();
#endif    
    
    Affiche_1_Parametre( this, -1, wxEmptyString, _( "Done" ), CYAN );

    /* Connexion des hierarchies */
    Affiche_1_Parametre( this, 36, _( "Hierar." ), wxEmptyString, LIGHTRED );

    for( i = 0;  i<g_NbrObjNet; i++ )
    {
        if( g_TabObjNet[i].m_Type == NET_SHEETLABEL
         || g_TabObjNet[i].m_Type == NET_SHEETBUSLABELMEMBER )
            SheetLabelConnection( g_TabObjNet + i );
    }
    
    /* Tri du Tableau des objets de Net par NetCode */
    qsort( g_TabObjNet, g_NbrObjNet, sizeof(ObjetNetListStruct), TriNetCode );

    
#if 0 && defined(DEBUG)
    std::cout << "after qsort()\n";
    dumpNetTable();
#endif    
    
    Affiche_1_Parametre( this, -1, wxEmptyString, _( "Done" ), RED );

    /* Compression des numeros de NetCode a des valeurs consecutives */
    Affiche_1_Parametre( this, 46, _( "Sorting" ), wxEmptyString, GREEN );
    LastNetCode = NetCode = 0;
    for( i = 0; i < g_NbrObjNet; i++ )
    {
        if( g_TabObjNet[i].GetNet() != LastNetCode )
        {
            NetCode++; 
            LastNetCode = g_TabObjNet[i].GetNet();
        }
        g_TabObjNet[i].SetNet( NetCode );
    }

    Affiche_1_Parametre( this, -1, wxEmptyString, _( "Done" ), GREEN );

    /* Affectation du m_FlagOfConnection en fonction de connection ou non */
    SetUnconnectedFlag( g_TabObjNet, g_NbrObjNet );

    return (void*) g_TabObjNet;
}


/*************************************************************
 * Routine qui connecte les sous feuilles par les sheetLabels  *
 **************************************************************/
static void SheetLabelConnection( ObjetNetListStruct* SheetLabel )
{
    int i;
    ObjetNetListStruct* ObjetNet;

    if( SheetLabel->GetNet() == 0 )
        return;

    /* Calcul du numero de sous feuille correspondante au sheetlabel */

    /* Comparaison du SheetLabel avec les GLABELS de la sous feuille
     *      pour regroupement des NetCodes */
    for( i = 0, ObjetNet = g_TabObjNet; i < g_NbrObjNet; i++ )
    {
        if( ObjetNet[i].m_SheetNumber != SheetLabel->m_NumInclude )
            continue;
        
        if( (ObjetNet[i].m_Type != NET_GLOBLABEL )
           && (ObjetNet[i].m_Type != NET_GLOBBUSLABELMEMBER ) )
            continue;
            
        if( ObjetNet[i].GetNet() == SheetLabel->GetNet() )
            continue;
        
        if( ObjetNet[i].m_Label->CmpNoCase( *SheetLabel->m_Label ) != 0 )
            continue;

        /* Propagation du Netcode a tous les Objets de meme NetCode */
        if( ObjetNet[i].GetNet() )
            PropageNetCode( ObjetNet[i].GetNet(), SheetLabel->GetNet(), 0 );
        else
            ObjetNet[i].SetNet( SheetLabel->GetNet() );
    }
}


/*****************************************************************************/
static int ListeObjetConnection( WinEDA_SchematicFrame* frame, SCH_SCREEN* screen,
                                 ObjetNetListStruct* ObjNet )
/*****************************************************************************/

/* Routine generant la liste des objets relatifs aux connection
 *  entree:
 *      screen: pointeur sur l'ecran a traiter
 *      ObjNet:
 *          si NULL: la routine compte seulement le nombre des objets
 *          sinon: pointe le tableau a remplir
 */
{
    int                     ii, NbrItem = 0, NumSheet;
    EDA_BaseStruct*         DrawList;
    EDA_SchComponentStruct* DrawLibItem;
    int                     TransMat[2][2], PartX, PartY, x2, y2;
    EDA_LibComponentStruct* Entry;
    LibEDA_BaseStruct*      DEntry;
    DrawSheetLabelStruct*   SheetLabel;
    int                     NumInclude;

    NumSheet = screen->m_SheetNumber;
    
    DrawList = screen->EEDrawList;
    for( ; DrawList;  DrawList = DrawList->Pnext )
    {
        switch( DrawList->Type() )
        {
        case DRAW_SEGMENT_STRUCT_TYPE:
            #undef STRUCT
            #define STRUCT ( (EDA_DrawLineStruct*) DrawList )
            if( ObjNet )
            {
                if( (STRUCT->m_Layer != LAYER_BUS)
                   && (STRUCT->m_Layer != LAYER_WIRE) )
                    break;

                ObjNet[NbrItem].m_Comp        = STRUCT;
                ObjNet[NbrItem].m_Screen      = screen;
                ObjNet[NbrItem].m_SheetNumber = NumSheet;
                ObjNet[NbrItem].m_Start       = STRUCT->m_Start;
                ObjNet[NbrItem].m_End         = STRUCT->m_End;
                
                if( STRUCT->m_Layer == LAYER_BUS )
                {
                    ObjNet[NbrItem].m_Type = NET_BUS;
                }
                else        /* Cas des WIRE */
                {
                    ObjNet[NbrItem].m_Type = NET_SEGMENT;
                }
            }
            NbrItem++;
            break;

        case DRAW_JUNCTION_STRUCT_TYPE:
            #undef STRUCT
            #define STRUCT ( (DrawJunctionStruct*) DrawList )
            if( ObjNet )
            {
                ObjNet[NbrItem].m_Comp        = STRUCT;
                ObjNet[NbrItem].m_Screen      = screen;
                ObjNet[NbrItem].m_Type        = NET_JONCTION;
                ObjNet[NbrItem].m_SheetNumber = NumSheet;
                ObjNet[NbrItem].m_Start       = STRUCT->m_Pos;
                ObjNet[NbrItem].m_End         = ObjNet[NbrItem].m_Start;
            }
            NbrItem++;
            break;

        case DRAW_NOCONNECT_STRUCT_TYPE:
            #undef STRUCT
            #define STRUCT ( (DrawNoConnectStruct*) DrawList )
            if( ObjNet )
            {
                ObjNet[NbrItem].m_Comp        = STRUCT;
                ObjNet[NbrItem].m_Screen      = screen;
                ObjNet[NbrItem].m_Type        = NET_NOCONNECT;
                ObjNet[NbrItem].m_SheetNumber = NumSheet;
                ObjNet[NbrItem].m_Start       = STRUCT->m_Pos;
                ObjNet[NbrItem].m_End         = ObjNet[NbrItem].m_Start;
            }
            NbrItem++;
            break;

        case DRAW_LABEL_STRUCT_TYPE:
            #undef STRUCT
            #define STRUCT ( (DrawLabelStruct*) DrawList )
            ii = IsBusLabel( STRUCT->m_Text );
            if( ObjNet )
            {
                ObjNet[NbrItem].m_Comp   = STRUCT;
                ObjNet[NbrItem].m_Screen = screen;
                ObjNet[NbrItem].m_Type   = NET_LABEL;
                
                if( STRUCT->m_Layer ==  LAYER_GLOBLABEL )
                    ObjNet[NbrItem].m_Type = NET_GLOBLABEL;
                
                ObjNet[NbrItem].m_Label = &STRUCT->m_Text;
                ObjNet[NbrItem].m_SheetNumber = NumSheet;
                ObjNet[NbrItem].m_Start = STRUCT->m_Pos;
                ObjNet[NbrItem].m_End   = ObjNet[NbrItem].m_Start;
                
                /* Si c'est un Bus, eclatement en Label */
                if( ii )
                    ConvertBusToMembers( ObjNet + NbrItem );
            }
            NbrItem += ii + 1;
            break;

        case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
            #undef STRUCT
            #define STRUCT ( (DrawGlobalLabelStruct*) DrawList )
            ii = IsBusLabel( STRUCT->m_Text );
            if( ObjNet )
            {
                ObjNet[NbrItem].m_Comp   = STRUCT;
                ObjNet[NbrItem].m_Screen = screen;
                ObjNet[NbrItem].m_Type   = NET_LABEL;
                
                if( STRUCT->m_Layer ==  LAYER_GLOBLABEL )
                    ObjNet[NbrItem].m_Type = NET_GLOBLABEL;
                
                ObjNet[NbrItem].m_Label = &STRUCT->m_Text;
                ObjNet[NbrItem].m_SheetNumber = NumSheet;
                ObjNet[NbrItem].m_Start = STRUCT->m_Pos;
                ObjNet[NbrItem].m_End   = ObjNet[NbrItem].m_Start;
                
                /* Si c'est un Bus, eclatement en Label */
                if( ii )
                    ConvertBusToMembers( ObjNet + NbrItem );
            }
            NbrItem += ii + 1;
            break;

        case DRAW_LIB_ITEM_STRUCT_TYPE:
            DrawLibItem = (EDA_SchComponentStruct*) DrawList;
            
            memcpy( TransMat, DrawLibItem->m_Transform, sizeof(TransMat) );
            
            PartX = DrawLibItem->m_Pos.x; 
            PartY = DrawLibItem->m_Pos.y;
            
            Entry = FindLibPart( DrawLibItem->m_ChipName, wxEmptyString, FIND_ROOT );

            if( Entry == NULL )
                break;
            
            if( Entry->m_Drawings == NULL )
                break;
            
            DEntry = Entry->m_Drawings;
            
            for( ;  DEntry;   DEntry = DEntry->Next() )
            {
                LibDrawPin* Pin = (LibDrawPin*) DEntry;
                if( DEntry->Type() != COMPONENT_PIN_DRAW_TYPE )
                    continue;
                
                if( DEntry->m_Unit
                   && (DEntry->m_Unit != DrawLibItem->m_Multi) )
                    continue;
                    
                if( DEntry->m_Convert
                   && (DEntry->m_Convert != DrawLibItem->m_Convert) )
                    continue;

                x2 = PartX + TransMat[0][0] * Pin->m_Pos.x
                     + TransMat[0][1] * Pin->m_Pos.y;
                     
                y2 = PartY + TransMat[1][0] * Pin->m_Pos.x
                     + TransMat[1][1] * Pin->m_Pos.y;

                if( ObjNet )
                {
                    ObjNet[NbrItem].m_Comp   = DEntry;
                    ObjNet[NbrItem].m_Type   = NET_PIN;
                    ObjNet[NbrItem].m_Link   = DrawLibItem;
                    ObjNet[NbrItem].m_Screen = screen;
                    ObjNet[NbrItem].m_ElectricalType = Pin->m_PinType;
                    ObjNet[NbrItem].m_PinNum      = Pin->m_PinNum;
                    ObjNet[NbrItem].m_Label       = &Pin->m_PinName;
                    ObjNet[NbrItem].m_SheetNumber = NumSheet;
                    ObjNet[NbrItem].m_Start.x     = x2;
                    ObjNet[NbrItem].m_Start.y     = y2;
                    ObjNet[NbrItem].m_End         = ObjNet[NbrItem].m_Start;
                }
                NbrItem++;

                if( ( (int) Pin->m_PinType == (int) PIN_POWER_IN )
                   && ( Pin->m_Attributs & PINNOTDRAW ) )
                {           
                    /* Il y a un PIN_LABEL Associe */
                    if( ObjNet )
                    {
                        ObjNet[NbrItem].m_Comp        = NULL;
                        ObjNet[NbrItem].m_Type        = NET_PINLABEL;
                        ObjNet[NbrItem].m_Screen      = screen;
                        ObjNet[NbrItem].m_SheetNumber = NumSheet;
                        ObjNet[NbrItem].m_Label       = &Pin->m_PinName;
                        ObjNet[NbrItem].m_Start.x     = x2;
                        ObjNet[NbrItem].m_Start.y     = y2;
                        ObjNet[NbrItem].m_End         = ObjNet[NbrItem].m_Start;
                    }
                    NbrItem++;
                }
            }

            break;

        case DRAW_PICK_ITEM_STRUCT_TYPE:
        case DRAW_POLYLINE_STRUCT_TYPE:
        case DRAW_BUSENTRY_STRUCT_TYPE:
        case DRAW_MARKER_STRUCT_TYPE:
        case DRAW_TEXT_STRUCT_TYPE:
            break;

        case DRAW_SHEET_STRUCT_TYPE:
            #undef STRUCT
            #define STRUCT ( (DrawSheetStruct*) DrawList )
            NumInclude = STRUCT->m_SheetNumber;

            SheetLabel = STRUCT->m_Label;
            for( ; SheetLabel != NULL;
                 SheetLabel = (DrawSheetLabelStruct*) SheetLabel->Pnext )
            {
                ii = IsBusLabel( SheetLabel->m_Text );
                if( ObjNet )
                {
                    ObjNet[NbrItem].m_Comp   = SheetLabel;
                    ObjNet[NbrItem].m_Link   = DrawList;
                    ObjNet[NbrItem].m_Type   = NET_SHEETLABEL;
                    ObjNet[NbrItem].m_Screen = screen;
                    ObjNet[NbrItem].m_ElectricalType = SheetLabel->m_Shape;
                    ObjNet[NbrItem].m_Label = &SheetLabel->m_Text;
                    ObjNet[NbrItem].m_SheetNumber = NumSheet;
                    ObjNet[NbrItem].m_NumInclude  = NumInclude;
                    ObjNet[NbrItem].m_Start = SheetLabel->m_Pos;
                    ObjNet[NbrItem].m_End   = ObjNet[NbrItem].m_Start;
                    
                    /* Si c'est un Bus, eclatement en Label */
                    if( ii )
                        ConvertBusToMembers( ObjNet + NbrItem );
                }
                NbrItem += ii + 1;
            }

            break;

        case DRAW_SHEETLABEL_STRUCT_TYPE:
            DisplayError( frame, wxT( "Netlist: Type DRAW_SHEETLABEL inattendu" ) );
            break;

        default:
        {
            wxString msg;
            msg.Printf( wxT( "Netlist: unexpected type struct %d" ),
                       DrawList->Type() );
            DisplayError( frame, msg );
            break;
        }
        }
    }

    return NbrItem;
}


/************************************************************************/
static void ConnectBusLabels( ObjetNetListStruct* Label, int NbItems )
/************************************************************************/

/* Routine qui analyse les labels type xxBUSLABELMEMBER
 *  Propage les Netcodes entre labels correspondants ( c'est a dire lorsque
 *  leur numero de membre est identique) lorsqu'ils sont connectes
 *  globalement par leur BusNetCode
 *  Utilise et met a jour la variable LastNetCode
 */
{
    ObjetNetListStruct* LabelInTst, * Lim;

    Lim = Label + NbItems;

    for( ; Label < Lim; Label++ )
    {
        if( (Label->m_Type == NET_SHEETBUSLABELMEMBER)
           || (Label->m_Type == NET_BUSLABELMEMBER)
           || (Label->m_Type == NET_GLOBBUSLABELMEMBER) )
        {
            if( Label->GetNet() == 0 )
            {
                Label->SetNet( LastNetCode ); 
                LastNetCode++;
            }
            
            for( LabelInTst = Label + 1; LabelInTst < Lim; LabelInTst++ )
            {
                if( (LabelInTst->m_Type == NET_SHEETBUSLABELMEMBER)
                   || (LabelInTst->m_Type == NET_BUSLABELMEMBER)
                   || (LabelInTst->m_Type == NET_GLOBBUSLABELMEMBER) )
                {
                    if( LabelInTst->m_BusNetCode != Label->m_BusNetCode )
                        continue;
                    
                    if( LabelInTst->m_Member != Label->m_Member )
                        continue;
                    
                    if( LabelInTst->GetNet() == 0 )
                        LabelInTst->SetNet( Label->GetNet() );
                    else
                        PropageNetCode( LabelInTst->GetNet(), Label->GetNet(), 0 );
                }
            }
        }
    }
}


/**************************************************/
int IsBusLabel( const wxString& LabelDrawList )
/**************************************************/

/* Routine qui verifie si le Label a une notation de type Bus
 *  Retourne 0 si non
 *  nombre de membres si oui
 *  met a jour FirstNumWireBus, LastNumWireBus et RootBusNameLength
 */

{
    unsigned Num;
    int ii;
    wxString BufLine;
    long tmp;
    bool error = FALSE;

    /* Search for  '[' because a bus label is like "busname[nn..mm]" */
    ii = LabelDrawList.Find( '[' );
    if( ii < 0 )
        return 0;
    
    Num = (unsigned) ii;

    FirstNumWireBus   = LastNumWireBus = 9;
    RootBusNameLength = Num;
    Num++;
    while( LabelDrawList[Num] != '.' && Num < LabelDrawList.Len() )
    {
        BufLine.Append( LabelDrawList[Num] );
        Num++;
    }

    if( !BufLine.ToLong( &tmp ) )
        error = TRUE;
    
    FirstNumWireBus = tmp;
    while( LabelDrawList[Num] == '.' && Num < LabelDrawList.Len() )
        Num++;

    BufLine.Empty();
    while( LabelDrawList[Num] != ']' && Num < LabelDrawList.Len() )
    {
        BufLine.Append( LabelDrawList[Num] );
        Num++;
    }

    if( !BufLine.ToLong( &tmp ) )
        error = TRUE;;
    LastNumWireBus = tmp;

    if( FirstNumWireBus < 0 )
        FirstNumWireBus = 0;
    if( LastNumWireBus < 0 )
        LastNumWireBus = 0;
    if( FirstNumWireBus > LastNumWireBus )
    {
        EXCHG( FirstNumWireBus, LastNumWireBus );
    }

    if( error && (s_PassNumber == 0) )
    {
        wxString msg = _( "Bad Bus Label: " ) + LabelDrawList;
        DisplayError( NULL, msg );
    }
    return LastNumWireBus - FirstNumWireBus + 1;
}


/***************************************************************/
static int ConvertBusToMembers( ObjetNetListStruct* BusLabel )
/***************************************************************/

/* Routine qui eclate un label type Bus en autant de Label qu'il contient de membres,
 *  et qui cree les structures avec le type NET_GLOBBUSLABELMEMBER, NET_BUSLABELMEMBER
 *  ou NET_SHEETBUSLABELMEMBER
 *  entree = pointeur sur l'ObjetNetListStruct initialise corresp au buslabel
 *  suppose que FirstNumWireBus, LastNumWireBus et RootBusNameLength sont a jour
 *  modifie l'ObjetNetListStruct de base et remplit les suivants
 *  m_Label is a pointer to a new wxString
 *  m_Label must be deallocated by the user (only for a NET_GLOBBUSLABELMEMBER,
 *  NET_BUSLABELMEMBER or a NET_SHEETBUSLABELMEMBER object type)
 */
{
    int NumItem, BusMember;
    wxString BufLine;

    if( BusLabel->m_Type == NET_GLOBLABEL )
        BusLabel->m_Type = NET_GLOBBUSLABELMEMBER;
    else if( BusLabel->m_Type == NET_SHEETLABEL )
        BusLabel->m_Type = NET_SHEETBUSLABELMEMBER;
    else
        BusLabel->m_Type = NET_BUSLABELMEMBER;

    /* Convertion du BusLabel en la racine du Label + le numero du fil */
    BufLine   = BusLabel->m_Label->Left( RootBusNameLength );
    
    BusMember = FirstNumWireBus;
    BufLine << BusMember;
    BusLabel->m_Label = new wxString( BufLine );

    BusLabel->m_Member = BusMember;
    NumItem = 1;

    for( BusMember++; BusMember <= LastNumWireBus; BusMember++ )
    {
        *(BusLabel + 1) = *BusLabel; 
        BusLabel++; 
        NumItem++;
        
        /* Convertion du BusLabel en la racine du Label + le numero du fil */
        BufLine = BusLabel->m_Label->Left( RootBusNameLength );
        BufLine << BusMember;
        BusLabel->m_Label = new wxString( BufLine );

        BusLabel->m_Member = BusMember;
    }

    return NumItem;
}


/**********************************************************************/
static void PropageNetCode( int OldNetCode, int NewNetCode, int IsBus )
/**********************************************************************/

/* PropageNetCode propage le netcode NewNetCode sur tous les elements
 *  appartenant a l'ancien netcode OldNetCode
 *  Si IsBus == 0; c'est le membre NetCode qui est propage
 *  Si IsBus != 0; c'est le membre BusNetCode qui est propage
 */

{
    int jj;
    ObjetNetListStruct* Objet = g_TabObjNet;

    if( OldNetCode == NewNetCode )
        return;

    if( IsBus == 0 )    /* Propagation du NetCode */
    {
        for( jj = 0; jj < g_NbrObjNet; jj++, Objet++ )
        {
            if( Objet->GetNet() == OldNetCode )
            {
                Objet->SetNet( NewNetCode );
            }
        }
    }
    else            /* Propagation du BusNetCode */
    {
        for( jj = 0; jj < g_NbrObjNet; jj++, Objet++ )
        {
            if( Objet->m_BusNetCode == OldNetCode )
            {
                Objet->m_BusNetCode = NewNetCode;
            }
        }
    }
}


/***************************************************************************/
static void PointToPointConnect( ObjetNetListStruct* Ref, int IsBus, int start )
/***************************************************************************/

/* Routine qui verifie si l'element *Ref est connecte a
 *  d'autres elements de la liste des objets du schema, selon le mode Point
 *  a point ( Extremites superposees )
 * 
 *  si IsBus:
 *      la connexion ne met en jeu que des elements type bus
 *          ( BUS ou BUSLABEL ou JONCTION )
 *  sinon
 *      la connexion ne met en jeu que des elements type non bus
 *          ( autres que BUS ou BUSLABEL )
 * 
 *  L'objet Ref doit avoir un NetCode valide.
 * 
 *  La liste des objets est supposee classe par NumSheet Croissants,
 *  et la recherche se fait a partir de l'element start, 1er element
 *  de la feuille de schema
 *  ( il ne peut y avoir connexion physique entre elements de differentes sheets)
 */
{
    int i, netCode;
    ObjetNetListStruct* netTable = g_TabObjNet;

    if( IsBus == 0 )    /* Objets autres que BUS et BUSLABELS */
    {
        netCode = Ref->GetNet();
        for( i = start; i < g_NbrObjNet; i++ )
        {
            if( netTable[i].m_SheetNumber > Ref->m_SheetNumber )
                break;

            switch( netTable[i].m_Type )
            {
            case NET_SEGMENT:
            case NET_PIN:
            case NET_LABEL:
            case NET_GLOBLABEL:
            case NET_SHEETLABEL:
            case NET_PINLABEL:
            case NET_JONCTION:
            case NET_NOCONNECT:
                if( Ref->m_Start == netTable[i].m_Start
                 || Ref->m_Start == netTable[i].m_End 
                 || Ref->m_End   == netTable[i].m_Start
                 || Ref->m_End   == netTable[i].m_End )
                {
                    if( netTable[i].GetNet() == 0 )
                        netTable[i].SetNet( netCode );
                    else
                        PropageNetCode( netTable[i].GetNet(), netCode, 0 );
                }
                break;

            case NET_BUS:
            case NET_BUSLABELMEMBER:
            case NET_SHEETBUSLABELMEMBER:
            case NET_GLOBBUSLABELMEMBER:
                break;
            }
        }
    }
    
    else    /* Objets type BUS et BUSLABELS ( et JONCTIONS )*/
    {
        netCode = Ref->m_BusNetCode;
        for( i = start;   i<g_NbrObjNet;   i++ )
        {
            if( netTable[i].m_SheetNumber > Ref->m_SheetNumber )
                break;

            switch( netTable[i].m_Type )
            {
            case NET_SEGMENT:
            case NET_PIN:
            case NET_LABEL:
            case NET_GLOBLABEL:
            case NET_SHEETLABEL:
            case NET_PINLABEL:
            case NET_NOCONNECT:
                break;

            case NET_BUS:
            case NET_BUSLABELMEMBER:
            case NET_SHEETBUSLABELMEMBER:
            case NET_GLOBBUSLABELMEMBER:
            case NET_JONCTION:
                if( Ref->m_Start == netTable[i].m_Start
                 || Ref->m_Start == netTable[i].m_End 
                 || Ref->m_End   == netTable[i].m_Start
                 || Ref->m_End   == netTable[i].m_End )
                {
                    if( netTable[i].m_BusNetCode == 0 )
                        netTable[i].m_BusNetCode = netCode;
                    else
                        PropageNetCode( netTable[i].m_BusNetCode, netCode, 1 );
                }
                break;
            }
        }
    }
}


/**************************************************************/
static void SegmentToPointConnect( ObjetNetListStruct* Jonction,
                                   int IsBus, int start )
/***************************************************************/

/*
 *  Routine qui recherche si un point (jonction) est connecte a des segments,
 *  et regroupe les NetCodes des objets connectes a la jonction.
 *  Le point de jonction doit avoir un netcode valide
 *  La liste des objets est supposee classe par NumSheet Croissants,
 *  et la recherche se fait a partir de l'element start, 1er element
 *  de la feuille de schema
 *  ( il ne peut y avoir connexion physique entre elements de differentes sheets)
 */
{
    int i;
    ObjetNetListStruct* Segment = g_TabObjNet;

    for( i = start; i < g_NbrObjNet; i++ )
    {
        if( Segment[i].m_SheetNumber > Jonction->m_SheetNumber )
            break;

        if( IsBus == 0 )
        {
            if( Segment[i].m_Type != NET_SEGMENT )
                continue;
        }
        else
        {
            if( Segment[i].m_Type != NET_BUS )
                continue;
        }

        if( SegmentIntersect( Segment[i].m_Start.x, Segment[i].m_Start.y,
                              Segment[i].m_End.x, Segment[i].m_End.y,
                              Jonction->m_Start.x, Jonction->m_Start.y ) )
        {
            /* Propagation du Netcode a tous les Objets de meme NetCode */
            if( IsBus == 0 )
            {
                if( Segment[i].GetNet() )
                    PropageNetCode( Segment[i].GetNet(),
                                    Jonction->GetNet(), IsBus );
                else
                    Segment[i].SetNet( Jonction->GetNet() );
            }
            else
            {
                if( Segment[i].m_BusNetCode )
                    PropageNetCode( Segment[i].m_BusNetCode,
                                    Jonction->m_BusNetCode, IsBus );
                else
                    Segment[i].m_BusNetCode = Jonction->m_BusNetCode;
            }
        }
    }
}


/*****************************************************************
 * Function which connects the groups of object which have the same label
 *******************************************************************/
static void LabelConnection( ObjetNetListStruct* LabelRef )
{
    if( LabelRef->GetNet() == 0 )
        return;

    ObjetNetListStruct* netTable = g_TabObjNet;

    for( int i=0; i<g_NbrObjNet;  i++ )
    {
        if( netTable[i].GetNet() == LabelRef->GetNet() )
            continue;

        if( netTable[i].m_SheetNumber != LabelRef->m_SheetNumber )
        {
            if( netTable[i].m_Type != NET_PINLABEL )
                continue;
        }

        if( netTable[i].m_Type == NET_LABEL
         || netTable[i].m_Type == NET_GLOBLABEL
         || netTable[i].m_Type == NET_BUSLABELMEMBER
         || netTable[i].m_Type == NET_GLOBBUSLABELMEMBER
         || netTable[i].m_Type == NET_PINLABEL )
        {
            if( netTable[i].m_Label->CmpNoCase( *LabelRef->m_Label ) != 0 )
                continue;

            /* Ici 2 labels identiques */

            /* Propagation du Netcode a tous les Objets de meme NetCode */
            if( netTable[i].GetNet() )
                PropageNetCode( netTable[i].GetNet(), LabelRef->GetNet(), 0 );
            else
                netTable[i].SetNet( LabelRef->GetNet() );
        }
    }
}


/****************************************************************************/
static int TriNetCode( const void* o1, const void* o2 )
/****************************************************************************/

/* Routine de comparaison pour le tri par NetCode croissant
 *  du tableau des elements connectes ( TabPinSort ) par qsort()
 */
{
    ObjetNetListStruct* Objet1 = (ObjetNetListStruct*) o1;
    ObjetNetListStruct* Objet2 = (ObjetNetListStruct*) o2;
    
    return Objet1->GetNet() - Objet2->GetNet();
}


/*****************************************************************************/
static int TriBySheet( const void* o1, const void* o2 )
/*****************************************************************************/

/* Routine de comparaison pour le tri par NumSheet
 *  du tableau des elements connectes ( TabPinSort ) par qsort() */

{
    ObjetNetListStruct* Objet1 = (ObjetNetListStruct*) o1; 
    ObjetNetListStruct* Objet2 = (ObjetNetListStruct*) o2;
    
    return Objet1->m_SheetNumber - Objet2->m_SheetNumber;
}


/**********************************************************************/
static void SetUnconnectedFlag( ObjetNetListStruct* ListObj, int NbItems )
/**********************************************************************/

/* Routine positionnant le membre .FlagNoConnect des elements de
 *  la liste des objets netliste, tries par ordre de NetCode
 */
{
    ObjetNetListStruct* NetItemRef, * NetItemTst, * ItemPtr;
    ObjetNetListStruct* NetStart, * NetEnd, * Lim;
    int Nb;
    IsConnectType StateFlag;


    NetStart = NetEnd = ListObj;
    NetItemRef = NetStart;
    Nb = 0; 
    StateFlag = UNCONNECT;

    Lim = ListObj + NbItems;
    for( ; NetItemRef < Lim; NetItemRef++ )
    {
        if( NetItemRef->m_Type == NET_NOCONNECT )
            if( StateFlag != CONNECT )
                StateFlag = NOCONNECT;

        /* Analyse du net en cours */
        NetItemTst = NetItemRef + 1;

        if( (NetItemTst >= Lim)
           || (NetItemRef->GetNet() != NetItemTst->GetNet()) )
        {   
            /* Net analyse: mise a jour de m_FlagOfConnection */
            NetEnd = NetItemTst;

            for( ItemPtr = NetStart; ItemPtr < NetEnd; ItemPtr++ )
            {
                ItemPtr->m_FlagOfConnection = StateFlag;
            }

            if( NetItemTst >= Lim )
                return;

            /* Start Analyse Nouveau Net */
            StateFlag = UNCONNECT;
            NetStart  = NetItemTst;
            continue;
        }

        for( ; ; NetItemTst++ )
        {
            if( (NetItemTst >= Lim)
               || (NetItemRef->GetNet() != NetItemTst->GetNet()) )
                break;

            switch( NetItemTst->m_Type )
            {
            case NET_SEGMENT:
            case NET_LABEL:
            case NET_GLOBLABEL:
            case NET_SHEETLABEL:
            case NET_PINLABEL:
            case NET_BUS:
            case NET_BUSLABELMEMBER:
            case NET_SHEETBUSLABELMEMBER:
            case NET_GLOBBUSLABELMEMBER:
            case NET_JONCTION:
                break;

            case NET_PIN:
                if( NetItemRef->m_Type == NET_PIN )
                    StateFlag = CONNECT;
                break;

            case NET_NOCONNECT:
                if( StateFlag != CONNECT )
                    StateFlag = NOCONNECT;
                break;
            }
        }
    }
}
