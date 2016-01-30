/************************************/
/* fonctions de la classe TEXTE_PCB */
/************************************/

#include "fctsys.h"
#include "wxstruct.h"

#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"


/*******************/
/* class TEXTE_PCB */
/*******************/

TEXTE_PCB::TEXTE_PCB( BOARD_ITEM* parent ) :
    BOARD_ITEM( parent, TYPETEXTE ),
    EDA_TextStruct()
{
}


/* Destructeur */
TEXTE_PCB:: ~TEXTE_PCB()
{
}


/* copie de stucture */
void TEXTE_PCB::Copy( TEXTE_PCB* source )
{
    m_Parent    = source->m_Parent;
    Pback       = Pnext = NULL;
    m_Miroir    = source->m_Miroir;
    m_Size      = source->m_Size;
    m_Orient    = source->m_Orient;
    m_Pos       = source->m_Pos;
    m_Layer     = source->m_Layer;
    m_Width     = source->m_Width;
    m_Attributs = source->m_Attributs;
    m_CharType  = source->m_CharType;
    m_HJustify  = source->m_HJustify;
    m_VJustify  = source->m_VJustify;

    m_Text = source->m_Text;
}


void TEXTE_PCB::UnLink()
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
            ( (BOARD*) Pback )->m_Drawings = (BOARD_ITEM*) Pnext;
        }
    }

    /* Modification du chainage avant */
    if( Pnext )
        Pnext->Pback = Pback;

    Pnext = Pback = NULL;
}


/****************************************************************/
int TEXTE_PCB::ReadTextePcbDescr( FILE* File, int* LineNum )
/****************************************************************/
{
    char text[1024], Line[1024];
    int  dummy;

    while( GetLine( File, Line, LineNum ) != NULL )
    {
        if( strnicmp( Line, "$EndTEXTPCB", 11 ) == 0 )
            return 0;
        if( strncmp( Line, "Te", 2 ) == 0 ) /* Texte */
        {
            ReadDelimitedText( text, Line + 2, sizeof(text) );
            m_Text = CONV_FROM_UTF8( text );
            continue;
        }
        if( strncmp( Line, "Po", 2 ) == 0 )
        {
            sscanf( Line + 2, " %d %d %d %d %d %d",
                    &m_Pos.x, &m_Pos.y, &m_Size.x, &m_Size.y,
                    &m_Width, &m_Orient );
            continue;
        }
        if( strncmp( Line, "De", 2 ) == 0 )
        {
            sscanf( Line + 2, " %d %d %lX %d\n", &m_Layer, &m_Miroir,
                    &m_TimeStamp, &dummy );
            if( m_Layer < FIRST_COPPER_LAYER )
                m_Layer = FIRST_COPPER_LAYER;
            if( m_Layer > LAST_NO_COPPER_LAYER )
                m_Layer = LAST_NO_COPPER_LAYER;

            continue;
        }
    }

    return 1;
}


#if 0   // replaced by Save()
/**************************************************/
int TEXTE_PCB::WriteTextePcbDescr( FILE* File )
/**************************************************/
{
    if( GetState( DELETED ) )
        return 0;

    if( m_Text.IsEmpty() )
        return 0;
    fprintf( File, "$TEXTPCB\n" );
    fprintf( File, "Te \"%s\"\n", CONV_TO_UTF8( m_Text ) );
    fprintf( File, "Po %d %d %d %d %d %d\n",
             m_Pos.x, m_Pos.y, m_Size.x, m_Size.y, m_Width, m_Orient );
    fprintf( File, "De %d %d %lX %d\n", m_Layer, m_Miroir, m_TimeStamp, 0 );
    fprintf( File, "$EndTEXTPCB\n" );
    return 1;
}
#endif


bool TEXTE_PCB::Save( FILE* aFile ) const
{
    if( GetState( DELETED ) )
        return true;

    if( m_Text.IsEmpty() )
        return true;
    
    bool rc = false;
    
    if( fprintf( aFile, "$TEXTPCB\n" ) != sizeof("$TEXTPCB\n")-1 )
        goto out;
    
    fprintf( aFile, "Te \"%s\"\n", CONV_TO_UTF8( m_Text ) );
    fprintf( aFile, "Po %d %d %d %d %d %d\n",
             m_Pos.x, m_Pos.y, m_Size.x, m_Size.y, m_Width, m_Orient );
    fprintf( aFile, "De %d %d %lX %d\n", m_Layer, m_Miroir, m_TimeStamp, 0 );
    
    if( fprintf( aFile, "$EndTEXTPCB\n" ) != sizeof("$EndTEXTPCB\n")-1 )
        goto out;
    
    rc = true;
out:    
    return rc;
}
    
    


/**********************************************************************/
void TEXTE_PCB::Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                      const wxPoint& offset, int DrawMode )
/**********************************************************************/

/*
 *  DrawMode = GR_OR, GR_XOR.., -1 si mode courant.
 */
{
    int color = g_DesignSettings.m_LayerColor[m_Layer];

    if( color & ITEM_NOT_SHOW )
        return;

    EDA_TextStruct::Draw( panel, DC, offset, color,
                         DrawMode, DisplayOpt.DisplayDrawItems,
                         (g_AnchorColor & ITEM_NOT_SHOW) ? -1 : (g_AnchorColor & MASKCOLOR) );
}



// see class_pcb_text.h
void TEXTE_PCB::Display_Infos( WinEDA_DrawFrame* frame )
{
    wxString msg;

    frame->MsgPanel->EraseMsgBox();

    if( m_Parent && m_Parent->Type() == TYPECOTATION )
        Affiche_1_Parametre( frame, 1, _( "COTATION" ), m_Text, DARKGREEN );
    else
        Affiche_1_Parametre( frame, 1, _( "PCB Text" ), m_Text, DARKGREEN );
    
    Affiche_1_Parametre( frame, 28, _( "Layer" ),
                         ReturnPcbLayerName( m_Layer ),
                         g_DesignSettings.m_LayerColor[m_Layer] & MASKCOLOR );

    Affiche_1_Parametre( frame, 36, _( "Mirror" ), wxEmptyString, GREEN );
    if( m_Miroir & 1 )
        Affiche_1_Parametre( frame, -1, wxEmptyString, _( "No" ), DARKGREEN );
    else
        Affiche_1_Parametre( frame, -1, wxEmptyString, _( "Yes" ), DARKGREEN );

    msg.Printf( wxT( "%.1f" ), (float) m_Orient / 10 );
    Affiche_1_Parametre( frame, 43, _( "Orient" ), msg, DARKGREEN );

    valeur_param( m_Width, msg );
    Affiche_1_Parametre( frame, 50, _( "Width" ), msg, MAGENTA );

    valeur_param( m_Size.x, msg );
    Affiche_1_Parametre( frame, 60, _( "H Size" ), msg, RED );

    valeur_param( m_Size.y, msg );
    Affiche_1_Parametre( frame, 70, _( "V Size" ), msg, RED );
}


#if defined(DEBUG)

/**
 * Function Show
 * is used to output the object tree, currently for debugging only.
 * @param nestLevel An aid to prettier tree indenting, and is the level 
 *          of nesting of this object within the overall tree.
 * @param os The ostream& to output to.
 */
void TEXTE_PCB::Show( int nestLevel, std::ostream& os )
{
    // for now, make it look like XML:
    NestedSpace( nestLevel, os ) << '<' << GetClass().Lower().mb_str() <<
        " string=\"" << m_Text.mb_str() << "\"/>\n"; 
    
//    NestedSpace( nestLevel, os ) << "</" << GetClass().Lower().mb_str() << ">\n";
}

#endif
