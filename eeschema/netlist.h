/**********************************************/
/* Module de calcul de la Netliste: netlist.h */
/**********************************************/

#ifndef _NETLIST_H_
#define _NETLIST_H_

#ifndef eda_global
#define eda_global extern
#endif

#define NETLIST_HEAD_STRING "EESchema Netlist Version 1.1"

#define ISBUS 1

#define CUSTOMPANEL_COUNTMAX 8  // Max number of netlist plugins
/* Id to select netlist type */
typedef enum {
    NET_TYPE_UNINIT = 0,
    NET_TYPE_PCBNEW,
    NET_TYPE_ORCADPCB2,
    NET_TYPE_CADSTAR,
    NET_TYPE_SPICE,
    NET_TYPE_CUSTOM1,   /* NET_TYPE_CUSTOM1
                         * is the first id for user netlist format
                         * NET_TYPE_CUSTOM1+CUSTOMPANEL_COUNTMAX-1
                         * is the last id for user netlist format
                         */
	NET_TYPE_CUSTOM_MAX = NET_TYPE_CUSTOM1+CUSTOMPANEL_COUNTMAX-1
} TypeNetForm;


/* Max pin number per component and footprint */
#define MAXPIN 5000

enum NetObjetType {      /* Type des objets de Net */
    NET_SEGMENT,
    NET_BUS,
    NET_JONCTION,
    NET_LABEL,
    NET_GLOBLABEL,
    NET_BUSLABELMEMBER,
    NET_GLOBBUSLABELMEMBER,
    NET_SHEETBUSLABELMEMBER,
    NET_SHEETLABEL,
    NET_PINLABEL,
    NET_PIN,
    NET_NOCONNECT
};


enum  IsConnectType {   /* Valeur du Flag de connection */
    UNCONNECT,          /* Pin ou Label non connecte */
    NOCONNECT,          /* Pin volontairement non connectee (Symb. NoConnect utilise) */
    CONNECT             /* connexion normale */
};


/* Structure decrivant 1 element de connexion (pour netlist ) */
class ObjetNetListStruct
{
public:
    EDA_BaseStruct* m_Comp;             /* Pointeur sur la definition de l'objet */
    void*           m_Link;             /* Pour SheetLabelStruct: Pointeur sur la feuille de hierarchie
                                         *  Pour les Pins: pointeur sur le composant */
    int             m_Flag;             /* flag pour calculs internes */
    SCH_SCREEN*     m_Screen;           /* Ecran d'appartenance */
    NetObjetType    m_Type;
    int             m_ElectricalType;   /* Pour Pins et sheet labels: type electrique */
private:
    int             m_NetCode;          /* pour elements simples */
public:
    int             m_BusNetCode;       /* pour connexions type bus */
    int             m_Member;           /* pour les labels type BUSWIRE ( labels de bus eclate )
                                         *  numero de membre */
    IsConnectType   m_FlagOfConnection;
    int             m_SheetNumber;  /* Sheet number for this item */
    int             m_NumInclude;   /* Numero de sous schema correpondant a la sheet (Gestion des GLabels et Pin Sheet)*/
    long            m_PinNum;       /* numero de pin( 4 octets -> 4 codes ascii) */
    const wxString* m_Label;        /* Tous types Labels:pointeur sur la wxString definissant le label */
    wxPoint         m_Start, m_End;

#if defined (DEBUG)
    void Show( std::ostream& out, int ndx );

#endif

    void SetNet( int aNetCode ) { m_NetCode = aNetCode; }
    int GetNet() const { return m_NetCode; }
};


/* Structure decrivant 1 composant de la schematique (pour annotation ) */
struct CmpListStruct
{
public:
    EDA_SchComponentStruct* m_Cmp;              /* Pointeur sur le composant */
    int                     m_NbParts;          /* Nombre de parts par boitier */
    bool                    m_PartsLocked;      // For multi part components: True if the part cannot be changed
    int                     m_Unit;             /* Numero de part */
    int                     m_Sheet;            /* Numero de hierarchie */
    unsigned long           m_TimeStamp;        /* Signature temporelle */
    int                     m_IsNew;            /* != 0 pour composants non annotes */
    char                    m_TextValue[32];    /* Valeur */
    char                    m_TextRef[32];      /* Reference ( hors numero ) */
    int                     m_NumRef;           /* Numero de reference */
    int                     m_Flag;             /* flag pour calculs internes */
    wxPoint                 m_Pos;              /* position components */
};


/* Global Variables */
eda_global int g_NbrObjNet;
eda_global ObjetNetListStruct* g_TabObjNet;

/* Prototypes: */
void        WriteNetList( WinEDA_SchematicFrame* frame,
                          const wxString&        FileNameNL,
                          bool                   use_netnames );
void        FreeTabNetList( ObjetNetListStruct* TabNetItems, int NbrNetItems );

/** Function ReturnUserNetlistTypeName
 * to retrieve user netlist type names
 * @param first = true: return first name of the list, false = return next
 * @return a wxString : name of the type netlist or empty string
 * this function must be called first with "first_item" = true
 * and after with "first_item" = false to get all the other existing netlist names
 */
wxString    ReturnUserNetlistTypeName( bool first_item );


#endif
