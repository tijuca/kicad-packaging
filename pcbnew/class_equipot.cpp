/*****************************************************************/
/* fonctions membres de la classe EQUIPOT et fonctions associ�s */
/*****************************************************************/

#include "fctsys.h"
#include "wxstruct.h"

#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"

#ifdef CVPCB
#include "cvpcb.h"
#endif

#include "protos.h"


/*********************************************************/
/* classe EQUIPOT: gestion des listes d'equipotentielles */
/*********************************************************/

/* Constructeur de la classe EQUIPOT */
EQUIPOT::EQUIPOT( BOARD_ITEM* StructFather ) :
    BOARD_ITEM( StructFather, PCB_EQUIPOT_STRUCT_TYPE )
{
    SetNet( 0 );
    m_NbNodes       = m_NbLink = m_NbNoconn = 0;
    m_Masque_Layer  = 0;
    m_Masque_Plan   = 0;
    m_ForceWidth    = 0;
    m_PadzoneStart  = NULL; // pointeur sur debut de liste pads du net
    m_PadzoneEnd    = NULL; // pointeur sur fin de liste pads du net
    m_RatsnestStart = NULL; // pointeur sur debut de liste ratsnests du net
    m_RatsnestEnd   = NULL; // pointeur sur fin de liste ratsnests du net
}


/* destructeut */

EQUIPOT::~EQUIPOT()
{
}


void EQUIPOT::UnLink()
{
    /* Modification du chainage arriere */
    if( Pback )
    {
        if( Pback->Type() != TYPEPCB )
        {
            Pback->Pnext = Pnext;
        }
        else /* Le chainage arriere pointe sur la structure "Pere" */
        {
            ( (BOARD*) Pback )->m_Equipots = (EQUIPOT*) Pnext;
        }
    }

    /* Modification du chainage avant */
    if( Pnext )
        Pnext->Pback = Pback;

    Pnext = Pback = NULL;
}


/*********************************************************/
int EQUIPOT:: ReadEquipotDescr( FILE* File, int* LineNum )
/*********************************************************/

/* Routine de lecture de 1 descr Equipotentielle.
 *  retourne 0 si OK
 *          1 si lecture incomplete
 */
{
    char Line[1024], Ltmp[1024];
    int  tmp;

    while( GetLine( File, Line, LineNum ) )
    {
        if( strnicmp( Line, "$End", 4 ) == 0 )
            return 0;

        if( strncmp( Line, "Na", 2 ) == 0 ) /* Texte */
        {
            sscanf( Line + 2, " %d", &tmp );
            SetNet( tmp );

            ReadDelimitedText( Ltmp, Line + 2, sizeof(Ltmp) );
            m_Netname = CONV_FROM_UTF8( Ltmp );
            continue;
        }

        if( strncmp( Line, "Lw", 2 ) == 0 ) /* Texte */
        {
            sscanf( Line + 2, " %d", &tmp );
            m_ForceWidth = tmp;
            continue;
        }
    }

    return 1;
}


#if 0   // replaced by Save()
/********************************************/
int EQUIPOT:: WriteEquipotDescr( FILE* File )
/********************************************/
{
    if( GetState( DELETED ) )
        return 0;

    fprintf( File, "$EQUIPOT\n" );
    fprintf( File, "Na %d \"%.16s\"\n", GetNet(), CONV_TO_UTF8( m_Netname ) );
    fprintf( File, "St %s\n", "~" );
    if( m_ForceWidth )
        fprintf( File, "Lw %d\n", m_ForceWidth );
    fprintf( File, "$EndEQUIPOT\n" );
    return 1;
}
#endif


bool EQUIPOT::Save( FILE* aFile ) const
{
    if( GetState( DELETED ) )
        return true;

    bool rc = false;
    
    fprintf( aFile, "$EQUIPOT\n" );
    fprintf( aFile, "Na %d \"%.16s\"\n", GetNet(), CONV_TO_UTF8( m_Netname ) );
    fprintf( aFile, "St %s\n", "~" );
    
    if( m_ForceWidth )
        fprintf( aFile, "Lw %d\n", m_ForceWidth );
    
    if( fprintf( aFile, "$EndEQUIPOT\n" ) != sizeof("$EndEQUIPOT\n")-1 )
        goto out;

    rc = true;
    
out:    
    return rc;
}


#if defined(DEBUG)
/**
 * Function Show
 * is used to output the object tree, currently for debugging only.
 * @param nestLevel An aid to prettier tree indenting, and is the level 
 *          of nesting of this object within the overall tree.
 * @param os The ostream& to output to.
 */
void EQUIPOT::Show( int nestLevel, std::ostream& os )
{
    // for now, make it look like XML:
    NestedSpace( nestLevel, os ) << '<' << GetClass().Lower().mb_str() << 
       " name=\"" <<  m_Netname.mb_str() << '"' <<
       " netcode=\"" << GetNet() << "\"/>\n";
}
#endif
